#ifndef SCRIPT_H
#define SCRIPT_H

#include <QtScript/QScriptEngine>
#include <QThread>

#define SCRIPT_ACTION_GET_TAG       "tag"
#define SCRIPT_ACTION_SET_TAG       "setTag"
#define SCRIPT_ACTION_LOG           "log"
#define SCRIPT_ACTION_WAIT          "wait"
#define SCRIPT_ACTION_SET_COUNTER   "setCounter"
#define SCRIPT_ACTION_FINISH        "finish"


class IScriptActionHandler {
public:
    virtual int tag(QString name) = 0;
    virtual void setTag(QString name, int value) = 0;
    virtual void log(QString log) = 0;
    virtual void wait(int timeMs) = 0;
    virtual void setCounter(int counter) = 0;
    virtual void finish(int code) = 0;
};


class IScript {
public:
    IScript();
    virtual ~IScript();

    virtual void execute() = 0;
    virtual void abortExecuting() = 0;
    virtual bool isExecuting() = 0;

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
    void abortExecuting();
    bool isExecuting();
};


class Script :
    public QObject,
    public IScript,
    public IScriptActionHandler {

    Q_OBJECT

public:
    Script(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    QString path() const;
    void setPath(const QString &path);

signals:
    void started();
    void finished();

    // IScript interface
public:
    void execute();
    void abortExecuting();
    bool isExecuting();

    // IScriptActionHandler interface
public:
    int tag(QString name);
    void setTag(QString name, int value);
    void log(QString log);
    void wait(int timeMs);
    void setCounter(int counter);
    void finish(int code);

private:
    QString     _name;
    QString     _path;

    QThread     *_thread    = 0;
    BaseScript  *_script    = 0;

    int         _finishCode = -1;
};
typedef QVector<Script*> Scripts;


#endif // SCRIPT_H
