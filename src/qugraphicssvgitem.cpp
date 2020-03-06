#include "qugraphicssvgitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class QuGraphicsSvgItemPrivate {
public:
    bool clickable;
    bool pressed, hover;
    QuGraphicsSvgItem::Shape shape;
    QRectF bounding_rect;
};

QuGraphicsSvgItem::QuGraphicsSvgItem()
    : QGraphicsSvgItem()
{
    d = new QuGraphicsSvgItemPrivate;
    d->clickable = false;
    d->pressed = false;
    d->hover = false;
    d->shape = Undefined;
}

bool QuGraphicsSvgItem::clickable() const {
    return d->clickable;
}

void QuGraphicsSvgItem::setClickable(const QString& c) {
    d->clickable = (c.compare("true", Qt::CaseInsensitive) == 0);
    if(d->shape == Undefined)
        d->shape = ShapeRect;
    setAcceptHoverEvents(d->clickable);
}

/*!
 * \brief set the shape of the item, from the svg tag name
 * \param s the shape as string (e.g. text, rect, ellipse)
 */
void QuGraphicsSvgItem::setShape(const QString &s) {
    if(s.endsWith("rect", Qt::CaseInsensitive) == 0)
        d->shape = ShapeRect;
    else if(s.endsWith("ellipse", Qt::CaseInsensitive) == 0)
        d->shape = ShapeEllipse;
    else if(s.endsWith("text", Qt::CaseInsensitive) == 0)
        d->shape = ShapeText;
    else if(s.endsWith("path", Qt::CaseInsensitive) == 0)
        d->shape = ShapePath;
    else
        d->shape = Undefined;
}

/*!
 * \brief Get the name of the shape of this item
 * \return the name of the shape set with setShape
 */
QuGraphicsSvgItem::Shape QuGraphicsSvgItem::getShape() const {
    return d->shape;
}

void QuGraphicsSvgItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << __PRETTY_FUNCTION__ ;
    if(d->clickable) {
        d->pressed = true;
        update();
    }
    else
        QGraphicsSvgItem::mousePressEvent(event);
}

void QuGraphicsSvgItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
    qDebug() << __PRETTY_FUNCTION__ ;
    if(d->pressed && e->button() == Qt::LeftButton) {
        d->pressed = false;
        update();
        if(isUnderMouse())
            emit clicked(this, e->screenPos(), e->pos());
    }
    else
        QGraphicsSvgItem::mouseReleaseEvent(e);
}

void QuGraphicsSvgItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *e) {
    qDebug() << __PRETTY_FUNCTION__ ;
    emit contextMenuRequest(this, e->screenPos(), e->pos());
    d->pressed = false;
    update();
}

void QuGraphicsSvgItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    qDebug() << __PRETTY_FUNCTION__ ;
    if(d->clickable) {
        d->hover = true;
        update();
    }
    else
        QGraphicsSvgItem::hoverEnterEvent(e);
}

void QuGraphicsSvgItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    qDebug() << __PRETTY_FUNCTION__ ;
    if(d->hover) {
        d->hover = false;
        update();
    }
    else
        QGraphicsSvgItem::hoverLeaveEvent(e);
}

void QuGraphicsSvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsSvgItem::paint(painter, option, widget);
    if(d->pressed || d->hover) {
        QPen pen = painter->pen();
        QColor c = pen.color().lighter();
        if(d->hover && !d->pressed) c.setAlpha(80);
        else if(d->pressed) c.setAlpha(105);
        pen.setColor(c);
        painter->setPen(pen);
        painter->setBrush(QBrush(c));
        if(d->shape == ShapeRect || d->shape == ShapePath)
            painter->drawRect(option->rect);
        else if(d->shape == ShapeEllipse)
            painter->drawEllipse(option->rect);
    }
}
