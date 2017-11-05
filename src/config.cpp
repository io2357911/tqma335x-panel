#include "config.h"

#include "ini.h"

Config Config::load(QString fileName) {
    Config config;

    QString encoding = "UTF-8";

    QStringList keys;
    Ini::Settings settings;

    // driver

    keys = QStringList({ "ip", "port", "listenPort", "pollMs", "sendCount" });
    settings = Ini::restore(keys, fileName, "Driver", encoding);

    config.setDriverIp(QHostAddress(settings.string("ip", config.driverIp().toString())));
    config.setDriverPort(settings.integer("port", config.driverPort()));
    config.setDriverListenPort(settings.integer("listenPort", config.driverListenPort()));
    config.setDriverPollMs(settings.integer("pollMs", config.driverPollMs()));
    config.setDriverSendCount(settings.integer("sendCount", config.driverSendCount()));

    // common

    keys = QStringList({ "tagsRefreshMs", "checkDevices" });
    settings = Ini::restore(keys, fileName, "Common", encoding);

    config.setCommonTagsRefreshMs(settings.integer("tagsRefreshMs", config.commonTagsRefreshMs()));
    config.setCheckDevices(settings.integer("checkDevices", config.checkDevices()));

    // scripts

    Scripts scripts;

    QStringList groups = Ini::groups(fileName, "Script", encoding);
    keys = QStringList({ "name", "path" });

    for (QString group : groups) {
        settings = Ini::restore(keys, fileName, group, encoding);

        Script *script = new Script();

        script->setName(settings.string("name"));
        script->setPath(settings.string("path"));

        scripts.append(script);
    }
    config.setScripts(scripts);

    // plot

    Plot *plot = new Plot();

    keys = QStringList({ "refreshMs" });
    settings = Ini::restore(keys, fileName, "Plot", encoding);
    plot->setRefreshMs(settings.integer("refreshMs", plot->refreshMs()));

    Graphs graphs;

    groups = Ini::groups(fileName, "Graph", encoding);
    keys = QStringList({ "name", "tag", "adc1", "adc2", "real1", "real2", "red", "green", "blue" });

    for (QString group : groups) {
        settings = Ini::restore(keys, fileName, group, encoding);

        Graph *graph = new Graph();

        graph->setName(settings.string("name"));
        graph->setNameTag(settings.string("tag"));
        graph->setAdc1(settings.integer("adc1"));
        graph->setAdc2(settings.integer("adc2"));
        graph->setReal1(settings.doubl("real1"));
        graph->setReal2(settings.doubl("real2"));

        QColor color = graph->color();
        graph->setColor(QColor(settings.integer("red", color.red()),
                               settings.integer("green", color.green()),
                               settings.integer("blue", color.blue())));

        graphs.append(graph);
    }
    plot->setGraphs(graphs);

    config.setPlot(plot);

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

int Config::commonTagsRefreshMs() const
{
    return _commonTagsRefreshMs;
}

void Config::setCommonTagsRefreshMs(int commonTagsRefreshMs)
{
    _commonTagsRefreshMs = commonTagsRefreshMs;
}

Scripts Config::scripts() const
{
    return _scripts;
}

void Config::setScripts(const Scripts &scripts)
{
    _scripts = scripts;
}

Plot *Config::plot() const
{
    return _plot;
}

void Config::setPlot(Plot *plot)
{
    _plot = plot;
}

bool Config::checkDevices() const
{
    return _checkDevices;
}

void Config::setCheckDevices(bool checkDevices)
{
    _checkDevices = checkDevices;
}

int Config::driverSendCount() const
{
    return _driverSendCount;
}

void Config::setDriverSendCount(int driverSendCount)
{
    _driverSendCount = driverSendCount;
}

