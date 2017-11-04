#include "driver.h"

Driver::Driver(QObject *parent) :
    QObject(parent), _thread(0), _ip("127.0.0.1"), _port(1122), _udp(0), _listenPort(1122) {

    _pollTimer.setInterval(100);
    connect(&_pollTimer, &QTimer::timeout, this, [this](){
        getTagsValues();
    });
}

Driver::~Driver() {
    stop();
}

void Driver::start() {
    if (isRunning()) return;

    qDebug("driver: start(ip: %s, port: %d, listenPort: %d, pollMs: %d)",
           _ip.toString().toStdString().c_str(), _port, _listenPort, pollMs());

    _thread = new QThread;
    connect(_thread, &QThread::started, [this]() {

        _udp = new QUdpSocket();
        _udp->moveToThread(_thread);
        connect(_udp, &QUdpSocket::readyRead, [this]() {
            while (_udp->hasPendingDatagrams()) {
                uint8_t buffer[1500];
                memset(buffer, 0, sizeof(buffer));
                _udp->readDatagram((char*)buffer, sizeof(buffer));

                // проверим magic
                bool magicOk = (buffer[0] == 0x15) && (buffer[1] == 0xA1) && (buffer[4] == (24|0x80));
                if (!magicOk) return;

                // проверим hardId
                int hardId;
                memcpy((void *)&hardId, (void *)(buffer+6), 2);
                if (_tags.hardId() != hardId) return;

                qDebug("magic and hardId - OK");

//                for (Tag *tag : _tags) {

//                }
            }
        });

        _udp->bind(QHostAddress::AnyIPv4, _listenPort);
    });
    connect(_thread, &QThread::finished, [this]() {
        _udp->close();
        _udp->deleteLater();
        _udp = 0;
    });
    _thread->start();

    _pollTimer.start();
}

void Driver::stop() {
    if (!isRunning()) return;

    qDebug("driver: stop()");

    _pollTimer.stop();

    _thread->quit();
    _thread->wait();
    _thread->deleteLater();
    _thread = 0;
}

bool Driver::isRunning() {
    return _thread;
}

void Driver::getTagsValues() {
    if (!_udp) return;

    uint8_t buffer[12];
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x21;   // сигнатура
    buffer[1] = 0xAB;
    buffer[2] = 0;
    buffer[3] = 0;
    buffer[4] = 24;     // код READ_IO^M

    _udp->writeDatagram((const char*)buffer, sizeof(buffer), _ip, _port);
}

void Driver::setTagValue(Tag *tag, int value) {

}

Tags Driver::tags() const {
    return _tags;
}

void Driver::setTags(const Tags &tags) {
    _tags = tags;
}

QHostAddress Driver::ip() const {
    return _ip;
}

void Driver::setIp(const QHostAddress &ip) {
    _ip = ip;
}

int Driver::port() const {
    return _port;
}

void Driver::setPort(int port) {
    _port = port;
}

int Driver::listenPort() const {
    return _listenPort;
}

void Driver::setListenPort(int listenPort) {
    _listenPort = listenPort;
}

int Driver::pollMs() const {
    return _pollTimer.interval();
}

void Driver::setPollMs(int pollMs) {
    _pollTimer.setInterval(pollMs);
}
