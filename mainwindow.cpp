#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "script.h"

#ifdef ARM
#define CONFIG_FILE   "/opt/config.ini"
#define TAGS_FILE   "/opt/tags.efs"
#define SCRIPT_FILE "/opt/test.script"
#else
#define CONFIG_FILE   "../../../config.ini"
#define TAGS_FILE   "../../../tags.efs"
#define SCRIPT_FILE "../../../test2.script"
#endif

using namespace Utils;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);

    _config = Config::load(CONFIG_FILE);

    // tags

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

        item = new QTableWidgetItem(tag->device());
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 1, item);

        item = new QTableWidgetItem(tag->type());
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 2, item);

        item = new QTableWidgetItem(QString::number(tag->value()));
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 3, item);
    }

    _tagsRefreshTimer.setInterval(_config.commonTagsRefreshMs());
    connect(&_tagsRefreshTimer, &QTimer::timeout, this, [this](){
        for (int i = 0; i < _tags.size(); i++) {
            Tag *tag = _tags[i];
            ui->twTags->item(i, 3)->setText(QString::number(tag->value()));
        }
    });
    _tagsRefreshTimer.start();

    // graph

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

    // driver

    _driver.setTags(_tags);
    _driver.setIp(_config.driverIp());
    _driver.setPort(_config.driverPort());
    _driver.setListenPort(_config.driverListenPort());
    _driver.setPollMs(_config.driverPollMs());
    _driver.setSendCount(_config.driverSendCount());
    _driver.start();

    // scripts

    _scripts = _config.scripts();
    for (Script *script : _scripts) {
        script->setParent(this);
        script->setActionHandler(this);
        connect(script, SIGNAL(started()), this, SLOT(updateButtons()));
        connect(script, SIGNAL(finished()), this, SLOT(updateButtons()));

        QPushButton *button = new QPushButton(script->name(), this);
        connect(button, &QPushButton::clicked, this, [this, script]() {
            if (isScriptExecuting()) return;

            ui->teLog->clear();
            displayStatus(Status_InProgress);

            script->execute();
        });
        ui->hlScripts->insertWidget(ui->hlScripts->count()-2, button);
        _scriptsButton.append(button);
    }

    connect(ui->pbAbort, &QPushButton::clicked, this, [this](){
        bool executing = false;
        for (Script *script : _scripts) {
            if (script->isExecuting()) {
                script->abortExecuting();
                executing = true;
            }
        }

        if (!executing) {
            ui->teLog->clear();
            displayStatus(Status_Ready);
        }
    });

    // misc

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

MainWindow::~MainWindow() {
    delete ui;
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

bool MainWindow::isScriptExecuting() {
    for (Script *script : _scripts) {
        if (script->isExecuting()) {
            return true;
        }
    }
    return false;
}
