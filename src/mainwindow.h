#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "config.h"
#include "tag.h"
#include "driver.h"
#include "script.h"

typedef QVector<QPushButton*> Buttons;

namespace Ui {
class MainWindow;
}

class MainWindow :
    public QMainWindow,
    public IScriptActionHandler {
    Q_OBJECT

public:
    enum Status {
        Status_Ready = -1,
        Status_InProgress = -2,
        Status_Ok = 0,
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    void initTags();
    void initDriver();
    void initScripts();
    void initGraph();
    void initMisc();

    bool isDevicesReady();
    bool isScriptExecuting();
    bool abortScriptExecuting();

private:
    Ui::MainWindow  *ui;
    Buttons         _scriptsButton;

    Config          _config;
    Tags            _tags;
    Driver          _driver;
    Scripts         _scripts;

    QTimer          _tagsRefreshTimer;
};

#endif // MAINWINDOW_H
