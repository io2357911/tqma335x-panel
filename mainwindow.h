#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "config.h"
#include "tag.h"
#include "driver.h"
#include "script.h"


namespace Ui {
class MainWindow;
}

class MainWindow :
    public QMainWindow,
    public IScriptActionHandler {

    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum Status {
        Status_Ready = -1,
        Status_InProgress = -2,
        Status_Ok = 0,
    };

    // IScriptActionHandler interface
public:
    int tag(QString name);
    void setTag(QString name, int value);
    void log(QString log);
    void wait(int timeMs);
    void setCounter(int _counter);
    void finish(int code);

signals:
    void displayLog(QString log);
    void displayCounter(int counter);
    void displayStatus(int code);

private slots:
    void updateButtons();

private:
    bool isDevicesReady();
    bool isScriptExecuting();
    bool abortScriptExecuting();

private:
    Ui::MainWindow  *ui;
    QVector<QPushButton*> _scriptsButton;

    Config          _config;
    Tags            _tags;
    Driver          _driver;
    Scripts         _scripts;

    QTimer          _tagsRefreshTimer;
};

#endif // MAINWINDOW_H
