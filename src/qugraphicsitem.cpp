#include "qugraphicsitem.h"
#include "qugraphicsitemhelper_i.h"
#include <QGraphicsSceneMouseEvent>
#include <QtDebug>
#include <QPainter>
#include <QMapIterator>
#include <QStyleOptionGraphicsItem>

class QuGraphicsItemPrivate {
public:
    bool clickable;
    bool pressed, hover;
    QuGraphicsItem::Shape shape;
    QRectF bounding_rect;
    QMap<int, QuGraphicsItemHelper_I *> helpermap;
};

QuGraphicsItem::QuGraphicsItem()
    : QGraphicsSvgItem()
{
    d = new QuGraphicsItemPrivate;
    d->clickable = false;
    d->pressed = false;
    d->hover = false;
    d->shape = Undefined;
    setAcceptHoverEvents(true);
}

QuGraphicsItem::~QuGraphicsItem() {
    QMapIterator<int, QuGraphicsItemHelper_I*> it(d->helpermap);
    while(it.hasNext()) {
        delete it.next().value();
    }
    delete d;
}

bool QuGraphicsItem::clickable() const {
    return d->clickable;
}

void QuGraphicsItem::setClickable(const QString& c) {
    d->clickable = (c.compare("true", Qt::CaseInsensitive) == 0);
    if(d->shape == Undefined)
        d->shape = ShapeRect;
}

/*!
 * \brief set the shape of the item, from the svg tag name
 * \param s the shape as string (e.g. text, rect, ellipse)
 */
void QuGraphicsItem::setShape(const QString &s) {
    if(s.endsWith("rect", Qt::CaseInsensitive) == 0)
        d->shape = ShapeRect;
    else if(s.endsWith("ellipse", Qt::CaseInsensitive) == 0)
        d->shape = ShapeEllipse;
    else if(s.endsWith("text", Qt::CaseInsensitive) == 0)
        d->shape = ShapeText;
    else if(s.endsWith("path", Qt::CaseInsensitive) == 0)
        d->shape = ShapePath;
    else
        d->shape = ShapeRect;
}

/*!
 * \brief Get the name of the shape of this item
 * \return the name of the shape set with setShape
 */
QuGraphicsItem::Shape QuGraphicsItem::getShape() const {
    return d->shape;
}

int QuGraphicsItem::type() const {
    return static_cast<int>(QuGraphicsItemType);
}

/*!
 * \brief get the helper associated with the helper id
 * \param id an identifier of a helper class
 * \return the registered helper or nullptr
 *
 * Helper classes register themselves to QuGraphicsItem when instantiated
 */
QuGraphicsItemHelper_I *QuGraphicsItem::helper(int id) const {
    return d->helpermap.value(id, nullptr);
}

void QuGraphicsItem::installHelper(int id, QuGraphicsItemHelper_I *helper) {
    d->helpermap[id] = helper;
}

QuGraphicsItemHelper_I* QuGraphicsItem::uninstallHelper(int id) {
    QuGraphicsItemHelper_I* h = d->helpermap[id];
    d->helpermap.remove(id);
    return h;
}

void QuGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(d->clickable) {
        d->pressed = true;
        update();
    }
    else
        QGraphicsSvgItem::mousePressEvent(event);
}

void QuGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
    if(d->pressed && e->button() == Qt::LeftButton) {
        d->pressed = false;
        update();
    }
    else
        QGraphicsSvgItem::mouseReleaseEvent(e);
}

void QuGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
//    qDebug() << __PRETTY_FUNCTION__ << this;
    emit itemEntered(this);
    if(d->clickable) {
        d->hover = true;
        update();
    }
    QGraphicsSvgItem::hoverEnterEvent(e);
}

void QuGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    emit itemLeft(this);
//    qDebug() << __PRETTY_FUNCTION__ << this;
    if(d->hover) {
        d->hover = false;
        update();
    }
    QGraphicsSvgItem::hoverLeaveEvent(e);
}

void QuGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
