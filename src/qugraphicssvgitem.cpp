#include "qugraphicssvgitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class QuGraphicsSvgItemPrivate {
public:
    bool clickable;
    bool pressed;
    QuGraphicsSvgItem::Shape shape;
};

QuGraphicsSvgItem::QuGraphicsSvgItem()
    : QGraphicsSvgItem()
{
    d = new QuGraphicsSvgItemPrivate;
    d->clickable = false;
    d->pressed = false;
    d->shape = Undefined;
}

bool QuGraphicsSvgItem::clickable() const {
    return d->clickable;
}

void QuGraphicsSvgItem::setClickable(const QString& c) {
    d->clickable = (c.compare("true", Qt::CaseInsensitive) == 0);
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
    if(d->clickable) {
        d->pressed = true;
        update();
    }
    else
        QGraphicsSvgItem::mousePressEvent(event);
}

void QuGraphicsSvgItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
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
    emit contextMenuRequest(this, e->screenPos(), e->pos());
}

void QuGraphicsSvgItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    if(d->clickable) {
        d->pressed = true;
        update();
    }
    else
        QGraphicsSvgItem::hoverEnterEvent(e);
}

void QuGraphicsSvgItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    if(d->pressed) {
        d->pressed = false;
        update();
    }
    else
        QGraphicsSvgItem::hoverLeaveEvent(e);
}

void QuGraphicsSvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsSvgItem::paint(painter, option, widget);
    if(d->pressed) {
        QPen pen = painter->pen();
        QColor c = pen.color().lighter();
        c.setAlpha(120);
        pen.setColor(c);
        painter->setPen(pen);
        painter->setBrush(QBrush(c));
        if(d->shape == ShapeRect || d->shape == ShapePath)
            painter->drawRect(option->rect);
        else if(d->shape == ShapeEllipse)
            painter->drawEllipse(option->rect);
    }
}
