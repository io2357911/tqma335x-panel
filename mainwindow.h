#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "script.h"
#include "driver.h"

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

signals:
    void displayLog(QString log);
    void displayCounter(int counter);
    void displayStatus(QString status);

    // IScriptActionHandler interface
public:
    int tag(QString name);
    void setTag(QString name, int value);
    void log(QString log);
    void wait(int timeMs);
    void setCounter(int _counter);
    void setStatus(QString status);

private:
    Ui::MainWindow  *ui;
    Driver          _driver;
    Script          _script;
};

#endif // MAINWINDOW_H
