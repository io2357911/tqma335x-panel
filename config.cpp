#include "config.h"

#include "ini.h"

Config::Config() {
}

Config Config::load(QString fileName) {
    Config config;

    QStringList keys;
    INI::Settings settings;

    keys = QStringList({ "ip", "port", "listenPort", "pollMs" });
    settings = INI::restore(keys, fileName, "Driver");

    config.setDriverIp(QHostAddress(settings.string("ip", config.driverIp().toString())));
    config.setDriverPort(settings.integer("port", config.driverPort()));
    config.setDriverListenPort(settings.integer("listenPort", config.driverListenPort()));
    config.setDriverPollMs(settings.integer("pollMs", config.driverPollMs()));

    return config;
}

QHostAddress Config::driverIp() const
{
    return _driverIp;
}

void Config::setDriverIp(const QHostAddress &driverIp)
{
    _driverIp = driverIp;
}

int Config::driverPort() const
{
    return _driverPort;
}

void Config::setDriverPort(int driverPort)
{
    _driverPort = driverPort;
}

int Config::driverListenPort() const
{
    return _driverListenPort;
}

void Config::setDriverListenPort(int driverListenPort)
{
    _driverListenPort = driverListenPort;
}

int Config::driverPollMs() const
{
    return _driverPollMs;
}

void Config::setDriverPollMs(int driverPollMs)
{
    _driverPollMs = driverPollMs;
}

