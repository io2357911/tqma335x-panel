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

    globalObject().setProperty(SCRIPT_ACTION_SET_STATUS, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isString()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_SET_STATUS, 0);
                return QScriptValue();
            }

            QString status = context->argument(0).toString();

            BaseScript *script = dynamic_cast<BaseScript*>(engine);
            IScriptActionHandler *handler = script->actionHandler();
            if (handler) handler->setStatus(status);

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
    QObject(parent), _thread(0), _script(0) {
}

void Script::execute() {
    if (isExecuting()) return;

    _thread = new QThread;
    connect(_thread, &QThread::started, [this](){
        _script = new BaseScript;
        _script->setText(text());
        _script->setActionHandler(actionHandler());
        _script->execute();

        _script->deleteLater();
        _script = 0;
        _thread = 0;
    });
    connect(_thread, SIGNAL(finished()), _thread, SLOT(deleteLater()));
    _thread->start();
}

void Script::abortExecuting() {
    if (_script) {
        _script->abortExecuting();
    }
}

bool Script::isExecuting() {
    return _thread;
}
