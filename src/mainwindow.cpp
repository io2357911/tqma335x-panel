#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "utils.h"
#include "script.h"

#define DEVICE_READY_VALUE  3

#if defined(ARM)
#define CONFIG_FILE "/opt/config.ini"
#define TAGS_FILE   "/opt/tags.efs"
#define OUTPUT_FILE "/opt/script.log"

#elif defined(BUILD_RELEASE)
#define CONFIG_FILE "config.ini"
#define TAGS_FILE   "tags.efs"
#define OUTPUT_FILE "script.log"

#else
#define CONFIG_FILE "../../../config.ini"
#define TAGS_FILE   "../../../tags.efs"
#define OUTPUT_FILE "../../../script.log"
#endif

using namespace Utils;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);

    _config = Config::load(CONFIG_FILE);

    initTags();
    initDriver();
    initScripts();
    initGraph();
    initMisc();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initTags() {
    _tags = Tags::load(TAGS_FILE);

    QTableWidget *tagsTable = ui->twTags;
    tagsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tagsTable->setRowCount(_tags.size());
    for (int i = 0; i < _tags.size(); i++) {
        Tag *tag = _tags[i];

        QTableWidgetItem *item;

        item = new QTableWidgetItem(tag->name());
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 0, item);

        item = new QTableWidgetItem(QString::number(tag->value()));
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 1, item);

        item = new QTableWidgetItem(tag->comment());
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 2, item);
    }

    _tagsRefreshTimer.setInterval(_config.commonTagsRefreshMs());
    connect(&_tagsRefreshTimer, &QTimer::timeout, this, [this](){
        for (int i = 0; i < _tags.size(); i++) {
            Tag *tag = _tags[i];
            ui->twTags->item(i, 1)->setText(QString::number(tag->value()));
        }
    });
    _tagsRefreshTimer.start();

    // индикация устройств
    QVector<Tag*> deviceTags = _tags.deviceTags();
    for (Tag *tag : deviceTags) {
        QLabel *label = new QLabel(tag->device(), this);
        label->setStyleSheet("color : red;");
        ui->hlDevices->insertWidget(ui->hlDevices->count()-1, label);

        connect(tag, &Tag::valueChanged, this, [this, label](int value){
            if (value == DEVICE_READY_VALUE) {
                label->setStyleSheet("color : green;");

            } else {
                label->setStyleSheet("color : red;");

                if (_config.checkDevices() &&
                    abortScriptExecuting()) {
                    qDebug("main: device failed - script stopped");
                    QMessageBox::warning(this, "Внимание", "Устройство не подключено или неисправно - скрипт остановлен");
                }
            }
        });
    }
}

void MainWindow::initDriver() {
    _driver.setTags(_tags);
    _driver.setIp(_config.driverIp());
    _driver.setPort(_config.driverPort());
    _driver.setListenPort(_config.driverListenPort());
    _driver.setPollMs(_config.driverPollMs());
    _driver.setSendCount(_config.driverSendCount());
    _driver.start();
}

void MainWindow::initScripts() {
    _scripts = _config.scripts();
    for (Script *script : _scripts) {
        script->setParent(this);
        script->setActionHandler(this);
        connect(script, SIGNAL(started()), this, SLOT(updateButtons()));
        connect(script, &Script::finished, this, [this](){
            updateButtons();
            Utils::writeTextFile(OUTPUT_FILE, ui->teLog->toPlainText());
        });

        QPushButton *button = new QPushButton(script->name(), this);
        connect(button, &QPushButton::clicked, this, [this, script]() {
            if (isScriptExecuting()) {
                qDebug("main: failed to start script - already running one");
                QMessageBox::warning(this, "Внимание", "Скрипт уже запущен");
                return;
            }
            if (!isDevicesReady()) {
                qDebug("main: failed to start script - devices are not ready");
                QMessageBox::warning(this, "Внимание", "Устройство не подключено или неисправно");
                return;
            }

            ui->teLog->clear();
            displayStatus(Status_InProgress);
            ui->lCounter->setText("0");

            script->execute();
        });
        ui->hlScripts->insertWidget(ui->hlScripts->count()-2, button);
        _scriptsButton.append(button);
    }

    connect(ui->pbAbort, &QPushButton::clicked, this, [this](){
        bool executing = abortScriptExecuting();

        if (!executing) {
            ui->teLog->clear();
            displayStatus(Status_Ready);
        }
    });
}

void MainWindow::initGraph() {
    QCustomPlot *plot = ui->wPlot;
    plot->addGraph();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        QCustomPlot *plot = ui->wPlot;

        int samplesCount = 50;
        QVector<double> x(samplesCount), y(samplesCount);
        for (int i = 0; i < samplesCount; i++) {
            x[i] = i;
            y[i] = randf(1, 5);
        }
        plot->graph()->setData(x, y, true);
        plot->rescaleAxes();
        plot->replot();
    });
    timer->setInterval(250);
    timer->start();
}

void MainWindow::initMisc() {
    connect(this, &MainWindow::displayLog, this, [this](QString log){
        ui->teLog->append(log);
    });

    connect(this, &MainWindow::displayCounter, this, [this](int counter){
        ui->lCounter->setText(QString::number(counter));
    });

    connect(this, &MainWindow::displayStatus, this, [this](int code){
        if (code == Status_Ready) {
            ui->lStatus->setText("Готовность");
            ui->lStatus->setStyleSheet("");

        } else if (code == Status_InProgress) {
            ui->lStatus->setText("Идет проверка");
            ui->lStatus->setStyleSheet("");

        }  else if (code == Status_Ok) {
            ui->lStatus->setText("Завершено");
            ui->lStatus->setStyleSheet("color : green;");

        } else {
            ui->lStatus->setText("Завершено с ошибкой");
            ui->lStatus->setStyleSheet("color : red;");
        }
    });
    displayStatus(Status_Ready);
}

int MainWindow::tag(QString name) {
    Tag *tag = _tags.find(name);
    if (!tag) {
        qDebug("main: tag('%s') - tag not found", name.toStdString().c_str());
        return -1;
    }

    return tag->value();
}

void MainWindow::setTag(QString name, int value) {
    Tag *tag = _tags.find(name);
    if (!tag) {
        qDebug("main: setTag('%s') - tag not found", name.toStdString().c_str());
        return;
    }

    _driver.setTagValue(_tags, tag, value);
}

void MainWindow::log(QString log) {
    displayLog(log);
}

void MainWindow::wait(int/* timeMs*/) {
}

void MainWindow::setCounter(int counter) {
    displayCounter(counter);
}

void MainWindow::finish(int code) {
    displayStatus(code);
}

void MainWindow::updateButtons() {
    if (isScriptExecuting()) {
        for (int i = 0; i < _scripts.size(); i++) {
            _scriptsButton[i]->setEnabled(_scripts[i]->isExecuting());
        }

        ui->pbAbort->setText("Прервать");

    } else {
        for (int i = 0; i < _scripts.size(); i++) {
            _scriptsButton[i]->setEnabled(true);
        }

        ui->pbAbort->setText("Сброс");
    }
}

bool MainWindow::isDevicesReady() {
    if (!_config.checkDevices()) return true;

    QVector<Tag*> deviceTags = _tags.deviceTags();
    for (Tag *tag : deviceTags) {
        if (tag->value() != DEVICE_READY_VALUE) {
            return false;
        }
    }
    return true;
}

bool MainWindow::isScriptExecuting() {
    for (Script *script : _scripts) {
        if (script->isExecuting()) {
            return true;
        }
    }
    return false;
}

bool MainWindow::abortScriptExecuting() {
    bool res = false;
    for (Script *script : _scripts) {
        if (script->isExecuting()) {
            script->abortExecuting();
            res = true;
        }
    }
    return res;
}
