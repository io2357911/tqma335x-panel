#include "plot.h"


QString Graph::name() const
{
    return _name;
}

void Graph::setName(const QString &name)
{
    _name = name;
}

QString Graph::tagName() const
{
    return _tagName;
}

void Graph::setNameTag(const QString &tag)
{
    _tagName = tag;
}

int Graph::adc1() const
{
    return _adc1;
}

void Graph::setAdc1(int adc1) {
    _adc1 = adc1;
    updateCoeffs();
}

int Graph::adc2() const {
    return _adc2;
}

void Graph::setAdc2(int adc2) {
    _adc2 = adc2;
    updateCoeffs();
}

double Graph::real1() const {
    return _real1;
}

void Graph::setReal1(double real1) {
    _real1 = real1;
    updateCoeffs();
}

double Graph::real2() const {
    return _real2;
}

void Graph::setReal2(double real2) {
    _real2 = real2;
    updateCoeffs();
}

QColor Graph::color() const {
    return _color;
}

void Graph::setColor(const QColor &color) {
    _color = color;
}

Tag *Graph::tag() const {
    return _tag;
}

void Graph::setTag(Tag *tag) {
    _tag = tag;
}

void Graph::update() {
    double value = 0;
    if (_tag) {
        value = _K * _tag->value() + _B;
    }
    _values.append(value);
}

void Graph::reset() {
    _values.clear();
}

QVector<double> Graph::values() const {
    return _values;
}

double Graph::K(int x0, int y0, double x1, double y1) {
    double dx = x1 - x0;
    return dx == 0 ? 0 : (y1 - y0) / dx;
}

double Graph::B(int x0, int y0, double x1, double y1) {
    double k = K(x0, y0, x1, y1);
    return y0 - x0 * k;
}

void Graph::updateCoeffs() {
    _K = K(_adc1, _real1, _adc2, _real2);
    _B = B(_adc1, _real1, _adc2, _real2);
}


Plot::Plot(QObject *parent) :
    QObject(parent) {

    connect(&_refreshTimer, &QTimer::timeout, this, [this](){
        if (!_plot) return;

        // обновим шкалу времени
        double time = _timer.time();
        _time.append(time);

        for (int i = 0; i < _graphs.size(); i++) {
            Graph *graph = _graphs[i];
            // обновим точки графов
            graph->update();

            _plot->graph(i)->setData(_time, graph->values(), true);
        }
        _plot->rescaleAxes();
        _plot->replot();
    });
    _refreshTimer.setInterval(1000);
}

Plot::~Plot() {
    for (Graph *graph : _graphs) {
        delete graph;
    }
    _graphs.clear();
}

void Plot::start() {
    if (_refreshTimer.isActive()) return;
    if (!_plot) return;

    _timer.start();
    _time.clear();
    _refreshTimer.start();
}

void Plot::stop() {
    _refreshTimer.stop();
}

void Plot::reset() {
    _time.clear();
    for (int i = 0; i < _graphs.size(); i++) {
        Graph *graph = _graphs[i];
        graph->reset();

        _plot->graph(i)->setData(_time, graph->values(), true);
        _plot->rescaleAxes();
    }
    _plot->replot();
}

int Plot::refreshMs() const {
    return _refreshTimer.interval();
}

void Plot::setRefreshMs(int updateMs) {
    _refreshTimer.setInterval(updateMs);
}

Graphs Plot::graphs() const {
    return _graphs;
}

void Plot::setGraphs(const Graphs &graphs) {
    _graphs = graphs;
}

void Plot::setTags(const Tags &tags) {
    // свяжем графы с тегами
    for (Graph *graph : _graphs) {
        Tag *tag = tags.find(graph->tagName());
        if (!tag) {
            qDebug("plot: no tag '%s' found for plot",
                   graph->tagName().toStdString().c_str());
            continue;
        }

        graph->setTag(tag);
    }
}

QCustomPlot *Plot::plot() const {
    return _plot;
}

void Plot::setPlot(QCustomPlot *plot) {
    if (_plot) return;
    _plot = plot;

    plot->legend->setVisible(true);

    QCPAxisRect *axisRect = plot->axisRect();
    axisRect->setupFullAxesBox(true);
    axisRect->removeAxis(axisRect->axis(QCPAxis::atLeft, 0));

    // назовем оси по Y
    for (int i = 0; i < _graphs.size(); i++) {
        Graph *graph = _graphs[i];

        QCPAxis* axis = axisRect->addAxis(QCPAxis::atLeft);
        axis->setLabel(graph->name());
    }

    for (int i = 0; i < _graphs.size(); i++) {
        Graph *graph = _graphs[i];

        // диапазон значений
        QCPAxis *axis = axisRect->axis(QCPAxis::atLeft, i);
        axis->setRange(graph->real1(), graph->real2());

        // параметры графика
        QCPGraph *plotGraph = plot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atLeft, i));
        plotGraph->setName(graph->name());
        plotGraph->setPen(graph->color());
    }
}
