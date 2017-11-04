#ifndef TAG_H
#define TAG_H

#include <QObject>
#include <QVector>
#include <mutex>

class Tag : public QObject {
    Q_OBJECT
public:
    Tag(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    QString native() const;
    void setNative(const QString &native);

    int offset() const;
    void setOffset(int offset);

    int segment() const;
    void setSegment(int segment);

    QString comment() const;
    void setComment(const QString &comment);

    int size() const;
    void setSize(int size);

    int step() const;
    void setStep(int step);

    QString device() const;
    void setDevice(const QString &device);

    QString type() const;
    void setType(const QString &type);

    int value();
    void setValue(int value);


signals:
    void valueChanged(int value);

private:
    QString     _name;
    QString     _native;
    int         _offset     = 0;
    int         _segment    = 0;
    QString     _comment;
    int         _size       = 1;
    int         _step       = 0;
    QString     _device;
    QString     _type;

    int         _value      = 0;
    std::mutex  _valueMutex;
};

class Tags : public QVector<Tag*> {
public:
    static Tags load(QString fileName);

    void update(uint8_t *buffer, uint size);

    Tag *find(QString name) const;

    QVector<Tag*> deviceTags() const;

    int hardId() const;
    void setHardId(int hardId);

    int segmentSizeS() const;
    void setSegmentSizeS(int segmentSizeS);

    int segmentSizeI() const;
    void setSegmentSizeI(int segmentSizeI);

    int segmentSizeQ() const;
    void setSegmentSizeQ(int segmentSizeQ);

private:
    int _hardId         = 0;
    int _segmentSizeS   = 0;
    int _segmentSizeI   = 0;
    int _segmentSizeQ   = 0;
};

#endif // TAG_H
