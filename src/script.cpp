#include "script.h"

#include "utils.h"

IScript::IScript() :
    _actionHandler(0)
{}

IScript::~IScript() {
}

QString IScript::text() const {
    return _text;
}

void IScript::setText(const QString &text) {
    _text = text;
}

IScriptActionHandler *IScript::actionHandler() const {
    return _actionHandler;
}

void IScript::setActionHandler(IScriptActionHandler *actionHandler) {
    _actionHandler = actionHandler;
}


BaseScript::BaseScript(QObject *parent) :
    QScriptEngine(parent) {

    // инициализация callback'ов операций скрипта
    globalObject().setProperty(SCRIPT_ACTION_GET_TAG, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isString()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_GET_TAG, 0);
                return QScriptValue();
            }

            QString tagName = context->argument(0).toString();

            BaseScript *script = dynamic_cast<BaseScript*>(engine);
            IScriptActionHandler *handler = script->actionHandler();
            return handler ? handler->tag(tagName) : 0;
    }));

    globalObject().setProperty(SCRIPT_ACTION_SET_TAG, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isString()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_SET_TAG, 0);
                return QScriptValue();
            }
            if (!context->argument(1).isNumber()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_SET_TAG, 1);
                return QScriptValue();
            }

            QString tagName = context->argument(0).toString();
            int value = context->argument(1).toInt32();

            BaseScript *script = dynamic_cast<BaseScript*>(engine);
            IScriptActionHandler *handler = script->actionHandler();
            if (handler) handler->setTag(tagName, value);

            return QScriptValue();
    }));

    globalObject().setProperty(SCRIPT_ACTION_LOG, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isString()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_LOG, 0);
                return QScriptValue();
            }

            QString log = context->argument(0).toString();

            BaseScript *script = dynamic_cast<BaseScript*>(engine);
            IScriptActionHandler *handler = script->actionHandler();
            if (handler) handler->log(log);

            return QScriptValue();
    }));

    globalObject().setProperty(SCRIPT_ACTION_WAIT, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isNumber()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_WAIT, 0);
                return QScriptValue();
            }

            int timeMs = context->argument(0).toInt32();

            BaseScript *script = dynamic_cast<BaseScript*>(engine);
            IScriptActionHandler *handler = script->actionHandler();
            if (handler) handler->wait(timeMs);

            return QScriptValue();
    }));

    globalObject().setProperty(SCRIPT_ACTION_SET_COUNTER, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isNumber()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_SET_COUNTER, 0);
                return QScriptValue();
            }

            int counter = context->argument(0).toInt32();

            BaseScript *script = dynamic_cast<BaseScript*>(engine);
            IScriptActionHandler *handler = script->actionHandler();
            if (handler) handler->setCounter(counter);

            return QScriptValue();
    }));

    globalObject().setProperty(SCRIPT_ACTION_FINISH, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isNumber()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_FINISH, 0);
                return QScriptValue();
            }

            int code = context->argument(0).toInt32();

            BaseScript *script = dynamic_cast<BaseScript*>(engine);
            IScriptActionHandler *handler = script->actionHandler();
            if (handler) handler->finish(code);

            return QScriptValue();
    }));
}

void BaseScript::execute() {
    evaluate(_text);
}

void BaseScript::abortExecuting() {
    abortEvaluation();
}

bool BaseScript::isExecuting() {
    return isEvaluating();
}


Script::Script(QObject *parent) :
    QObject(parent) {
}

void Script::execute() {
    if (isExecuting()) return;

    _finishCode = -1;

    setText(Utils::readTextFile(_path));

    _thread = new QThread;
    connect(_thread, &QThread::started, [this](){
        started();

        _script = new BaseScript;
        _script->setText(text());
        _script->setActionHandler(this);
        _script->execute();

        _script->deleteLater();
        _script = 0;
        _thread = 0;

        // забыли вызвать finish(0);
        if (_finishCode == -1) {
            finish(0);
        }

        finished();
    });
    connect(_thread, SIGNAL(finished()), _thread, SLOT(deleteLater()));
    _thread->start();
}

void Script::abortExecuting() {
    finish(1);
}

bool Script::isExecuting() {
    return _thread;
}

int Script::tag(QString name) {
    qDebug("script: %s('%s')", SCRIPT_ACTION_GET_TAG, name.toStdString().c_str());

    return _actionHandler ? _actionHandler->tag(name) : -1;
}

void Script::setTag(QString name, int value) {
    qDebug("script: %s('%s',%d)", SCRIPT_ACTION_SET_TAG, name.toStdString().c_str(), value);

    if (_actionHandler) _actionHandler->setTag(name, value);
}

void Script::log(QString log) {
    qDebug("script: %s('%s')", SCRIPT_ACTION_LOG, log.toStdString().c_str());

    if (_actionHandler) _actionHandler->log(log);
}

void Script::wait(int timeMs) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_WAIT, timeMs);

    Utils::sleepMs(timeMs);
    if (_actionHandler) _actionHandler->wait(timeMs);
}

void Script::setCounter(int counter) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_SET_COUNTER, counter);

    if (_actionHandler) _actionHandler->setCounter(counter);
}

void Script::finish(int code) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_FINISH, code);

    if (_script) {
        _script->abortExecuting();
    }

    _finishCode = code;

    if (_actionHandler) _actionHandler->finish(code);
}

QString Script::path() const
{
    return _path;
}

void Script::setPath(const QString &path)
{
    _path = path;
}

QString Script::name() const
{
    return _name;
}

void Script::setName(const QString &name)
{
    _name = name;
}
