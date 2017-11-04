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
    void setTagValue(const Tags &tags, Tag *tag, int value);

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

    int sendCount() const;
    void setSendCount(int sendCount);

private:
    Tags            _tags;
    QThread         *_thread        = 0;

    QHostAddress    _ip             = QHostAddress("127.0.0.1");
    int             _port           = 1122;

    QUdpSocket      *_udp           = 0;
    int             _listenPort     = 1122;
    int             _sendCount      = 3;
    QTimer          _pollTimer;
};

#endif // DRIVER_H
