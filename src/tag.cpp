#include "tag.h"
#include <algorithm>
#include "ini.h"
#include "utils.h"


Tag::Tag(QObject *parent) :
    QObject(parent) {}

QString Tag::name() const
{
    return _name;
}

void Tag::setName(const QString &name)
{
    _name = name;
}

QString Tag::native() const
{
    return _native;
}

void Tag::setNative(const QString &native)
{
    _native = native;
}

int Tag::offset() const
{
    return _offset;
}

void Tag::setOffset(int offset)
{
    _offset = offset;
}

int Tag::segment() const
{
    return _segment;
}

void Tag::setSegment(int segment)
{
    _segment = segment;
}

int Tag::size() const
{
    return _size;
}

void Tag::setSize(int size)
{
    _size = size;
}

int Tag::step() const
{
    return _step;
}

void Tag::setStep(int step)
{
    _step = step;
}

QString Tag::device() const
{
    return _device;
}

void Tag::setDevice(const QString &device)
{
    _device = device;
}

QString Tag::type() const
{
    return _type;
}

void Tag::setType(const QString &type)
{
    _type = type;
}

int Tag::value()  {
    std::lock_guard<std::mutex> lock(_valueMutex);
    return _value;
}

void Tag::setValue(int value) {
    bool changed = false;
    if (_value != value) {
        std::lock_guard<std::mutex> lock(_valueMutex);
        _value = value;
        changed = true;
    }

    if (changed) {
        emit valueChanged(_value);
    }
}

QString Tag::comment() const
{
    return _comment;
}

void Tag::setComment(const QString &comment)
{
    _comment = comment;
}


Tags Tags::load(QString fileName) {
    Tags tags;

    QString encoding = "Windows-1251";

    QStringList keys;
    Ini::Settings settings;

    keys = QStringList({ "HardID" });
    settings = Ini::restore(keys, fileName, "Meta");
    tags.setHardId(settings.integer("HardID", 0));

    keys = QStringList({ "SegS", "SegI", "SegQ" });
    settings = Ini::restore(keys, fileName, "Segments");
    tags.setSegmentSizeS(settings.integer("SegS"));
    tags.setSegmentSizeI(settings.integer("SegI"));
    tags.setSegmentSizeQ(settings.integer("SegQ"));

    QStringList groups = Ini::groups(fileName, "Tag", encoding);

    keys = QStringList({ "Name", "Native", "Offset", "Segment", "Comment", "Size", "Step", "Device", "Type" });
    for (QString group : groups) {
        settings = Ini::restore(keys, fileName, group, encoding);

        Tag *tag = new Tag();

        tag->setName(settings.string("Name"));
        tag->setNative(settings.string("Native"));
        tag->setOffset(settings.integer("Offset"));
        tag->setSegment(settings.integer("Segment"));
        tag->setComment(settings.string("Comment"));
        tag->setSize(settings.integer("Size", 1));
        tag->setStep(settings.integer("Step", 0));
        tag->setDevice(settings.string("Device"));
        tag->setType(settings.string("Type"));

        tags.append(tag);
    }

    return tags;
}

void Tags::update(uint8_t *buffer, uint size) {
    Q_ASSERT(size >= 1500);

    // проверим magic
    bool magicOk = (buffer[0] == 0x15) && (buffer[1] == 0xA1) && (buffer[4] == (24|0x80));
    if (!magicOk) return;

    // проверим hardId
    int hardId = 0;
    memcpy((void *)&hardId, (void *)(buffer+6), 2);
    if (_hardId != hardId) return;

    uint8_t *data = buffer + 16;

    for (Tag *tag : *this) {
        int value = Utils::getInt(data, size-16, tag->offset(), tag->size());
//        qDebug("tag: offset: %d, size: %d, value: %d", tag->offset(), tag->size(), value);
        tag->setValue(value);
    }
}

Tag *Tags::find(QString name) const {
    for (Tag *tag : *this) {
        if (tag->name() == name) {
            return tag;
        }
    }
    return 0;
}

QVector<Tag *> Tags::deviceTags() const {
    QVector<Tag *> tags;

    for (Tag *tag : *this) {
        if (tag->segment() == 0) {
            tags.append(tag);
        }
    }

    return tags;
}

int Tags::segmentSizeS() const
{
    return _segmentSizeS;
}

int Tags::segmentSizeI() const
{
    return _segmentSizeI;
}

int Tags::segmentSizeQ() const
{
    return _segmentSizeQ;
}

int Tags::hardId() const
{
    return _hardId;
}

void Tags::setHardId(int hardId)
{
    _hardId = hardId;
}

void Tags::setSegmentSizeS(int segmentSizeS)
{
    _segmentSizeS = segmentSizeS;
}

void Tags::setSegmentSizeI(int segmentSizeI)
{
    _segmentSizeI = segmentSizeI;
}

void Tags::setSegmentSizeQ(int segmentSizeQ)
{
    _segmentSizeQ = segmentSizeQ;
}
