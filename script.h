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


class IScriptActionHandler {
public:
    virtual int tag(QString name) = 0;
    virtual void setTag(QString name, int value) = 0;
    virtual void log(QString log) = 0;
    virtual void wait(int timeMs) = 0;
    virtual void setCounter(int counter) = 0;
    virtual void setStatus(QString status) = 0;
};


class IScript {
public:
    IScript();
    virtual ~IScript();

    virtual void execute() = 0;
    virtual bool isExecuting() = 0;
    virtual void abortExecuting() = 0;

    QString text() const;
    void setText(const QString &text);

    IScriptActionHandler *actionHandler() const;
    void setActionHandler(IScriptActionHandler *actionHandler);

protected:
    QString                 _text;
    IScriptActionHandler    *_actionHandler;
};


class BaseScript :
    public QScriptEngine,
    public IScript {

    Q_OBJECT

public:
    BaseScript(QObject *parent = 0);

    // IScript interface
public:
    void execute();
    bool isExecuting();
    void abortExecuting();
};


class Script :
    public QObject,
    public IScript {

    Q_OBJECT

public:
    Script(QObject *parent = 0);

    // IScript interface
public:
    void execute();
    bool isExecuting();
    void abortExecuting();

private:
    QThread     *_thread;
    BaseScript  *_script;
};

#endif // SCRIPT_H
