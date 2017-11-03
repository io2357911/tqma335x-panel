#ifndef DRIVER_H
#define DRIVER_H

#include <QThread>
#include <QUdpSocket>
#include <QTimer>
#include "tag.h"

class Driver : public QObject {
    Q_OBJECT
public:
    Driver(QObject *parent = 0);
    ~Driver();

    void start();
    void stop();
    bool isRunning();

    void getTagsValues();
    void setTagValue(Tag *tag, int value);


    Tags tags() const;
    void setTags(const Tags &tags);

    QHostAddress ip() const;
    void setIp(const QHostAddress &ip);

    int port() const;
    void setPort(int port);

    int listenPort() const;
    void setListenPort(int listenPort);

    int pollMs() const;
    void setPollMs(int pollMs);

private:
    Tags            _tags;
    QThread         *_thread;

    QHostAddress    _ip;
    int             _port;

    QUdpSocket      *_udp;
    int             _listenPort;
    QTimer          _pollTimer;
};

#endif // DRIVER_H
