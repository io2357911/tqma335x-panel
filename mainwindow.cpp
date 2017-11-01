#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <assert.h>

double randf() {
    return rand() / (RAND_MAX + 1.);
}

double randf(double a, double b) {
    assert(b >= a);
    return a + randf()*(b-a);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    counter(0) {

    ui->setupUi(this);

    connect(ui->pbCount, &QPushButton::clicked, this, [this](){
        ui->pbCount->setText(QString("Счетчик: %1").arg(++counter));
    });

    // logs
    for (int i = 0; i < 100; i++) {
        ui->teLog->append(QString("Log: %1").arg(i));
    }

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

    // udp send
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        QByteArray bytes;

        bytes.append(0xC0);
        bytes.append(0xFF);
        bytes.append(0xEE);

        udp.writeDatagram(bytes.data(), bytes.size(), QHostAddress("192.168.1.8"), 1121);

    });
    timer->setInterval(1000);
    timer->start();
}

MainWindow::~MainWindow() {
    delete ui;
}
