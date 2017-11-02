#include "script.h"

#include "utils.h"

AbstractScript::AbstractScript(QObject *parent) :
    QScriptEngine(parent) {

    // инициализация callback'ов операций скрипта
    globalObject().setProperty(SCRIPT_ACTION_GET_TAG, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isString()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_GET_TAG, 0);
                return QScriptValue();
            }

            QString tagName = context->argument(0).toString();

            AbstractScript *script = dynamic_cast<AbstractScript*>(engine);
            return script->tag(tagName);
    }));

    globalObject().setProperty(SCRIPT_ACTION_SET_TAG, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isString()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_SET_TAG, 0);
                return QScriptValue();
            }
            if (!context->argument(0).isNumber()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_SET_TAG, 1);
                return QScriptValue();
            }

            QString tagName = context->argument(0).toString();
            int value = context->argument(0).toInt32();

            AbstractScript *script = dynamic_cast<AbstractScript*>(engine);
            script->setTag(tagName, value);

            return QScriptValue();
    }));

    globalObject().setProperty(SCRIPT_ACTION_LOG, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isString()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_LOG, 0);
                return QScriptValue();
            }

            QString log = context->argument(0).toString();

            AbstractScript *script = dynamic_cast<AbstractScript*>(engine);
            script->log(log);

            return QScriptValue();
    }));

    globalObject().setProperty(SCRIPT_ACTION_WAIT, newFunction(
        [](QScriptContext *context, QScriptEngine *engine) -> QScriptValue {
            if (!context->argument(0).isNumber()) {
                qDebug("script: action '%s' invalid argument %d", SCRIPT_ACTION_WAIT, 0);
                return QScriptValue();
            }

            int timeMs = context->argument(0).toInt32();

            AbstractScript *script = dynamic_cast<AbstractScript*>(engine);
            script->wait(timeMs);

            return QScriptValue();
    }));
}

void AbstractScript::setVariable(QString variable, QVariant value) {
    switch (value.type()) {
    case QVariant::Int:
        globalObject().setProperty(variable, value.toInt());
        break;
    case QVariant::Double:
        globalObject().setProperty(variable, value.toDouble());
        break;
    case QVariant::String:
        globalObject().setProperty(variable, value.toString());
        break;
    default:
        break;
    }
}

void AbstractScript::execute(QString script) {
    evaluate(script);
}

bool AbstractScript::check(QString script) {
    return evaluate(script).toBool();
}

bool AbstractScript::isRunning() {
    return isEvaluating();
}


Script::Script(QString text, QObject *parent)
    : AbstractScript(parent), text_(text), thread_(0)
{}

QString Script::text() const {
    return text_;
}

void Script::setText(const QString &text) {
    text_ = text;
}

void Script::start() {
    execute(text_);
//    if (isRunning()) return;

//    // запустим управляющий поток для выполнения программы
//    thread_ = new QThread;
//    connect(thread_, &QThread::started, [this](){
//        this->moveToThread(thread_);

//        execute(text_);

//        this->thread_->deleteLater();
//        this->thread_ = 0;
//    });
//    thread_->start();
}

void Script::stop() {
    abortEvaluation();
}

bool Script::isRunning() {
//    return thread_;
    return AbstractScript::isRunning();
}

int Script::tag(QString name) {
    if (name == "T1") {
        return 1;

    } else if (name == "T2") {
        return 2;

    } else {
        return 0;
    }
}

void Script::setTag(QString name, int value) {
    qDebug("script: %s(%s,%d)", SCRIPT_ACTION_GET_TAG, name.toStdString().c_str(), value);
}

void Script::log(QString log) {
    qDebug("script: %s(%s)", SCRIPT_ACTION_LOG, log.toStdString().c_str());
}

void Script::wait(int timeMs) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_WAIT, timeMs);
    Utils::sleepMs(timeMs);
}

void Script::setCounter(int counter) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_SET_COUNTER, counter);
}

void Script::setStatus(int status) {
    qDebug("script: %s(%d)", SCRIPT_ACTION_SET_STATUS, status);
}

