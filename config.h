#ifndef CONFIG_H
#define CONFIG_H

#include <QHostAddress>
#include <QVector>
#include "script.h"


//class ScriptInfo {
//public:
//    QString name() const;
//    void setName(const QString &name);

//    QString path() const;
//    void setPath(const QString &path);

//private:
//    QString _name;
//    QString _path;
//};
//typedef QVector<ScriptInfo> ScriptInfos;


class Config {
public:
    static Config load(QString fileName);

    // driver

    QHostAddress driverIp() const;
    void setDriverIp(const QHostAddress &driverIp);

    int driverPort() const;
    void setDriverPort(int driverPort);

    int driverListenPort() const;
    void setDriverListenPort(int driverListenPort);

    int driverPollMs() const;
    void setDriverPollMs(int driverPollMs);

    int driverSendCount() const;
    void setDriverSendCount(int driverSendCount);

    // common

    int commonTagsRefreshMs() const;
    void setCommonTagsRefreshMs(int commonTagsRefreshMs);

    // scripts

    Scripts scripts() const;
    void setScripts(const Scripts &scripts);

private:
    // driver
    QHostAddress    _driverIp               = QHostAddress("127.0.0.1");
    int             _driverPort             = 1122;
    int             _driverListenPort       = 1122;
    int             _driverPollMs           = 100;
    int             _driverSendCount        = 3;

    // common
    int             _commonTagsRefreshMs    = 250;

    // scripts
    Scripts         _scripts;
};

#endif // CONFIG_H
