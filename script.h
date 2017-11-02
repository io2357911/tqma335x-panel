#ifndef SCRIPT_H
#define SCRIPT_H

#include <QtScript/QScriptEngine>
#include <QThread>

#define SCRIPT_ACTION_GET_TAG       "tag"
#define SCRIPT_ACTION_SET_TAG       "setTag"
#define SCRIPT_ACTION_LOG           "log"
#define SCRIPT_ACTION_WAIT          "wait"
#define SCRIPT_ACTION_SET_COUNTER   "setCounter"
#define SCRIPT_ACTION_SET_STATUS    "setStatus"


class IScript {
public:
    virtual int tag(QString name) = 0;
    virtual void setTag(QString name, int value) = 0;
    virtual void log(QString log) = 0;
    virtual void wait(int timeMs) = 0;
    virtual void setCounter(int counter) = 0;
    virtual void setStatus(int status) = 0;
};


class AbstractScript : public QScriptEngine, public IScript {
    Q_OBJECT
public:
    AbstractScript(QObject *parent = 0);
    virtual ~AbstractScript() {}

    virtual void setVariable(QString variable, QVariant value);
    virtual void execute(QString script);
    virtual bool check(QString script);
    virtual bool isRunning();
};


class Script : public AbstractScript {
    Q_OBJECT
public:
    Script(QString text = QString(), QObject *parent = 0);

    QString text() const;
    void setText(const QString &text);

    void start();
    void stop();
    bool isRunning();

    // IScript interface
public:
    int tag(QString name);
    void setTag(QString name, int value);
    void log(QString log);
    void wait(int timeMs);
    void setCounter(int counter);
    void setStatus(int status);

private:
    QString text_;
    QThread *thread_;
};

#endif // SCRIPT_H
