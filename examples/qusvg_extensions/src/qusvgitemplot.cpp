#include "qusvgitemplot.h"
#include <QtDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <cumacros.h>
#include <QtDebug>
#include <QPainter>

class QuSvgItemPlotPrivate {
public:
    QuSvgItemPlotPrivate() : plot(nullptr) {}
    QGraphicsPlotItem *plot;
};

QuGraphicsSvgItem *QuSvgItemPlotFactory::create() const {
    return new QuSvgItemPlot();
}


QuSvgItemPlot::QuSvgItemPlot()
    : QuGraphicsSvgItem() {
    d = new QuSvgItemPlotPrivate;
    d->plot = new QGraphicsPlotItem(this);
    d->plot->setOriginPosPercentage(d->plot->xScaleItem(), 0.0);
    d->plot->setOriginPosPercentage(d->plot->yScaleItem(), 0.0);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QuSvgItemPlot::~QuSvgItemPlot() {
    delete d;
}

QGraphicsPlotItem *QuSvgItemPlot::plot() const {
    return d->plot;
}

QVariant QuSvgItemPlot::itemChange(GraphicsItemChange change, const QVariant &value) {
    qDebug() << __PRETTY_FUNCTION__ << change << value;
    if(change == ItemPositionHasChanged && d->plot->boundingRect() != this->boundingRect()) {
        qDebug() << __PRETTY_FUNCTION__ << "setting geometry" << boundingRect();
        d->plot->setGeometry(this->boundingRect());
        d->plot->setPos(0, 0); // relative to parent
    }
    return QGraphicsObject::itemChange(change, value);
}



void QuSvgItemPlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qDebug() << __PRETTY_FUNCTION__ << "boundingRect" << boundingRect() << "option.rect" << option->rect;
    QPen p1(Qt::yellow);
    p1.setStyle(Qt::DashDotDotLine);
    p1.setWidthF(1.5);
    painter->setPen(p1);
    painter->drawRect(boundingRect());
    painter->setPen(Qt::blue);
    painter->drawRoundedRect(d->plot->boundingRect(), 5, 5);
}
