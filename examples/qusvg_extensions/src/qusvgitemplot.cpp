#include "qusvgitemplot.h"
#include <QtDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <cumacros.h>

QuGraphicsSvgItem *QuSvgItemPlotFactory::create() const {
    return new QuSvgItemPlot();
}


QuSvgItemPlot::QuSvgItemPlot()
    : QuGraphicsSvgItem() {
    qDebug() << __PRETTY_FUNCTION__ << "instantiated";
}

QuSvgItemPlot::~QuSvgItemPlot() {

}

void QuSvgItemPlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    qDebug() << __PRETTY_FUNCTION__ << "painter rect " << painter->window() << widget << "option.rect" << option->rect <<
      "mapRectFromScene" << this->mapRectFromScene(option->rect) << "mapRectToScene" << this->mapRectToScene(option->rect);

    const QRectF& r = option->rect;
    painter->setPen(QPen(Qt::blue));
    painter->setBrush(QBrush(Qt::green));
    painter->drawRect(r.center().x() - r.width() / 4, r.center().y() - r.height() / 4, r.width() / 2, r.height() / 2);

    painter->setPen(QPen(Qt::red));
    painter->setBrush(QBrush());
    painter->drawRect(option->rect);
}

