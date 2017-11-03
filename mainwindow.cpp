#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "script.h"

#ifdef ARM
#define SCRIPT_FILE "/opt/test.script"
#else
#define SCRIPT_FILE "test.script"
#endif

using namespace Utils;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // tags
    QTableWidget *tagsTable = ui->twTags;
    tagsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int rowCount = 50;
    tagsTable->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++) {
        QTableWidgetItem *item;

        item = new QTableWidgetItem(QString("Название %1").arg(i));
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 0, item);

        item = new QTableWidgetItem(QString("Устройство %1").arg(i));
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 1, item);

        item = new QTableWidgetItem(QString("Тип %1").arg(i));
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 2, item);

        item = new QTableWidgetItem(QString("Значение %1").arg(i));
        item->setTextAlignment(Qt::AlignCenter);
        tagsTable->setItem(i, 3, item);
    }

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
    _driver.setIp(QHostAddress("192.168.109.30"));
    _driver.setPort(1122);
    _driver.setListenPort(1122);
    _driver.setPollMs(100);
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

    if (name == "T1") {
        return 1;

    } else if (name == "T2") {
        return 2;

    } else {
        return 0;
    }
}

void MainWindow::setTag(QString name, int value) {
    qDebug("script: %s(%s,%d)", SCRIPT_ACTION_SET_TAG, name.toStdString().c_str(), value);
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
