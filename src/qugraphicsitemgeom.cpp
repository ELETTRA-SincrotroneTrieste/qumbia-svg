#include "qugraphicsitemgeom.h"

#include <QPolygonF>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSvgRenderer>
#include <QtDebug>
#include <math.h>

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, re, {".*rotate\\(([0-9\\.\\+\\-]+)\\)"});

class QuSvgItemGeom_P {
public:
    QuSvgItemGeom_P(QuGraphicsItem *it, float rot) : rotation(rot), item(it){  }
    float rotation;
    QuGraphicsItem *item;
    QRectF bounds;
};

QuGraphicsItemGeom::QuGraphicsItemGeom(QuGraphicsItem *item, const QuDom *dom) {
    d = new QuSvgItemGeom_P(item, m_get_rotation(dom->itemAttribute(item->elementId(), "transform")));
    d->item->installHelper(Geom, this);
}

QuGraphicsItemGeom::~QuGraphicsItemGeom() {
    delete d;
}

float QuGraphicsItemGeom::rotation() const {
    return d->rotation;
}

/*!
 * \brief bounding rect of the polygon enclosing the bounds of the item *transformed*
 *        by the transformation matrix (e.g. rotation)
 * \return bounding rect of QuGraphicsItemGeom::transformedBounds
 * \see QuGraphicsItemGeom::transformedBounds
 */
QRectF QuGraphicsItemGeom::transformedBoundingRect() const {
    return transformedBounds().boundingRect();
}

float QuGraphicsItemGeom::m_get_rotation(const QString &xform) const {
    float rotate = 0.0;
    QRegularExpressionMatch ma = re->match(xform);
    if(ma.capturedTexts().size() > 1)
        rotate = ma.captured(1).toDouble();
    return rotate;
}

/*!
 * \brief returns a QPolygon with *four points* representing the four
 *        vertexes of the QuGraphicsItemGeom::bounds rect rotated
 *        by the value QuGraphicsItemGeom::rotation
 *
 * \return a QPolygon representing a rectangle where p1 is the top left, p2 is the
 *         top right, p3 the bottom left, p4 the bottom right
 *
 * The polygon is the rectangle obtained with QuGraSvgItemGeom::bounds rotated
 * by QuGraSvgItemGeom::rotation degrees
 */
QPolygon QuGraphicsItemGeom::transformedBounds() const {
    const QRectF & bo = bounds();
    double rad = 2 * M_PI * d->rotation / 360.0;
    const QPointF& c = bo.center();
    const double &x0 = c.x(), &y0 = c.y();
    double p1x = bo.topLeft().x(), p1y = bo.topLeft().y();
    const double pn1x = p1x * cos(rad) - p1y * sin(rad) +  x0 * (1 - cos(rad)) + y0 * sin(rad);
    const double pn1y = p1y * cos(rad) + p1x * sin(rad) +  y0 * (1 - cos(rad)) - x0 * sin(rad);

    const double p2x = bo.topRight().x(), p2y = bo.topRight().y();
    const double pn2x = p2x * cos(rad) - p2y * sin(rad) +  x0 * (1 - cos(rad)) + y0 * sin(rad);
    const double pn2y = p2y * cos(rad) + p2x * sin(rad) +  y0 * (1 - cos(rad)) - x0 * sin(rad);

    double p3x = bo.bottomLeft().x(), p3y = bo.bottomLeft().y();
    const double pn3x = p3x * cos(rad) - p3y * sin(rad) +  x0 * (1 - cos(rad)) + y0 * sin(rad);
    const double pn3y = p3y * cos(rad) + p3x * sin(rad) +  y0 * (1 - cos(rad)) - x0 * sin(rad);

    double p4x = bo.bottomRight().x(), p4y = bo.bottomRight().y();
    const double pn4x = p4x * cos(rad) - p4y * sin(rad) +  x0 * (1 - cos(rad)) + y0 * sin(rad);
    const double pn4y = p4y * cos(rad) + p4x * sin(rad) +  y0 * (1 - cos(rad)) - x0 * sin(rad);

    const QVector<QPointF> &pts{QPointF(pn1x,pn1y), QPointF(pn2x,pn2y), QPointF(pn4x,pn4y), QPointF(pn3x,pn3y)};
    QPolygonF poly(pts);
    return poly.toPolygon();
}

/*! \brief returns the center of the rectangle given by *bounds*
 * \return a point representing the center of the item rectangle
 * \see bounds
 */
QPointF QuGraphicsItemGeom::center() const {
    return bounds().center();
}

/*!
 * \brief item bounds, untransformed rectangle
 * \return svg renderer *boundsOnElement* on item or bounds
 *         set manually through QuGraphicsItemGeom::setMapScale
 */
QRectF QuGraphicsItemGeom::bounds() const {
    return d->bounds.isNull() ?
               d->bounds = d->item->renderer()->boundsOnElement(d->item->elementId()) : d->bounds;
}

/*!
 * \brief map a point according to the rotation and center
 * \param p point in the item
 * \return the point *p* rotated around the center
 */
QPointF QuGraphicsItemGeom::map(const QPointF &p) const {
    double rad = 2 * M_PI * d->rotation / 360.0;
    const QRectF& b = bounds();
    const QPointF& c = b.center();
    const float& mx = p.x() * cos(rad) - p.y() * sin(rad) +  c.x() * (1 - cos(rad)) + c.y() * sin(rad);
    const float& my = p.y() * cos(rad) + p.x() * sin(rad) +  c.y() * (1 - cos(rad)) - c.x() * sin(rad);
    return QPointF(mx, my);
}

/*!
 * \brief convenience function that maps the bounds top left point into
 *        the transformed coordinates
 * \return the equivalent of QuGraphicsItemGeom::map(QPointF(bounds().x(), bounds().y()))
 */
QPointF QuGraphicsItemGeom::transformedTopLeft() const {
    const QRectF & bo = bounds();
    return map(QPointF(bo.left(),  bo.top()));
}



/*!
 * \brief scale the item rectangle defined by the svg renderer *boundsOnItem*
 *        to a custom rectangle, scaled by a width and height *factor*
 * \param wrel factor > 0 for width scaling
 * \param hrel factor > 0 for height scaling
 *
 * \note if either wrel or hrel are <= 0, the item bounds are reset to
 *       the rectangle established by QSvgRenderer::boundsOnItem
 */
void QuGraphicsItemGeom::setMapScale(const float &wrel, const float& hrel) {
    d->bounds = QRectF();
    if(wrel > 0 || hrel > 0) {
        bounds();
        const QPointF& c = bounds().center();
        d->bounds.setWidth(d->bounds.width() * wrel);
        d->bounds.setHeight(d->bounds.height() * hrel);
        d->bounds.moveCenter(c); // same center
    }
}

void QuGraphicsItemGeom::setMapScale(const float &scale) {
    setMapScale(scale, scale);
}

/*!
 * \brief returns the QuGraphicsItemGeom id
 * \return QuGraphicsItemGeom::Geom
 *
 * \see QuGraphicsItem::helper
 */
int QuGraphicsItemGeom::id() const {
    return Geom;
}

/*!
 * \brief return a pointer to the owner item
 * \return pointer to QuGraphicsItem
 */
QuGraphicsItem *QuGraphicsItemGeom::item() const {
    return d->item;
}

/*!
 * \brief detach this helper from the item.
 * \see QugraphicsItemHelper_I::detach
 */
void QuGraphicsItemGeom::detach() {
    d->item->uninstallHelper(this->id());
}
