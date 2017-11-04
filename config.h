#ifndef CONFIG_H
#define CONFIG_H

#include <QHostAddress>

class Config {
public:
    Config();

    static Config load(QString fileName);

    QHostAddress driverIp() const;
    void setDriverIp(const QHostAddress &driverIp);

    int driverPort() const;
    void setDriverPort(int driverPort);

    int driverListenPort() const;
    void setDriverListenPort(int driverListenPort);

    int driverPollMs() const;
    void setDriverPollMs(int driverPollMs);

    int commonTagsRefreshMs() const;
    void setCommonTagsRefreshMs(int commonTagsRefreshMs);

private:
    // driver
    QHostAddress    _driverIp               = QHostAddress("127.0.0.1");
    int             _driverPort             = 1122;
    int             _driverListenPort       = 1122;
    int             _driverPollMs           = 100;

    // common
    int             _commonTagsRefreshMs    = 250;
};

#endif // CONFIG_H
