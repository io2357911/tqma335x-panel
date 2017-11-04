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

    // script
    _script.setActionHandler(this);
    connect(ui->pbCount, &QPushButton::clicked, this, [this](){
        if (_script.isExecuting()) {
            return;
        }

        ui->teLog->clear();

        _script.setText(Utils::readTextFile(SCRIPT_FILE));
        _script.execute();
    });
    connect(ui->pbAbort, &QPushButton::clicked, this, [this](){
        _script.abortExecuting();
    });

    connect(this, &MainWindow::displayLog, this, [this](QString log){
        ui->teLog->append(log);
    });
    connect(this, &MainWindow::displayCounter, this, [this](int counter){
        ui->lCounter->setText(QString::number(counter));
    });
    connect(this, &MainWindow::displayStatus, this, [this](QString status){
        ui->lStatus->setText(status);
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

int MainWindow::tag(QString name) {
    qDebug("script: %s(%s)", SCRIPT_ACTION_GET_TAG, name.toStdString().c_str());

    Tag *tag = _tags.find(name);
    if (!tag) {
        qDebug("script: tag('%s') - tag not found", name.toStdString().c_str());
        return 0;
    }

    return tag->value();
}

void MainWindow::setTag(QString name, int value) {
    qDebug("script: %s(%s,%d)", SCRIPT_ACTION_SET_TAG, name.toStdString().c_str(), value);

    Tag *tag = _tags.find(name);
    if (!tag) {
        qDebug("script: setTag('%s') - tag not found", name.toStdString().c_str());
        return;
    }

    _driver.setTagValue(_tags, tag, value);
}

void MainWindow::log(QString log) {
    qDebug("script: %s(%s)", SCRIPT_ACTION_LOG, log.toStdString().c_str());

    displayLog(log);
}

void MainWindow::wait(int timeMs) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_WAIT, timeMs);

    Utils::sleepMs(timeMs);
}

void MainWindow::setCounter(int counter) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_SET_COUNTER, counter);

    displayCounter(counter);
}

void MainWindow::setStatus(QString status) {
    qDebug("script: %s(%s)", SCRIPT_ACTION_SET_STATUS, status.toStdString().c_str());

    displayStatus(status);
}
