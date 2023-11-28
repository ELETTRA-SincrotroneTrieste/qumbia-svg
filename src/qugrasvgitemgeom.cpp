#include "qugrasvgitemgeom.h"

#include <QPolygonF>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSvgRenderer>
#include <QtDebug>
#include <math.h>

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, re, {".*rotate\\(([0-9\\.\\+\\-]+)\\)"});

class QuSvgItemGeom_P {
public:
    QuSvgItemGeom_P(QuGraphicsSvgItem *it, float rot) : rotation(rot), item(it), x0{0.5}, y0{0.5} {  }
    float rotation;
    QuGraphicsSvgItem *item;
    QRectF bounds;
    float x0, y0; // origin of the axes
};

QuGraSvgItemGeom::QuGraSvgItemGeom(QuGraphicsSvgItem *item, const QuDom *dom) {
    d = new QuSvgItemGeom_P(item, m_get_rotation(dom->itemAttribute(item->elementId(), "transform")));
}

QuGraSvgItemGeom::~QuGraSvgItemGeom() {
    delete d;
}

float QuGraSvgItemGeom::rotation() const {
    return d->rotation;
}

float QuGraSvgItemGeom::m_get_rotation(const QString &xform) const {
    float rotate = 0.0;
    QRegularExpressionMatch ma = re->match(xform);
    if(ma.capturedTexts().size() > 1)
        rotate = ma.captured(1).toDouble();
    return rotate;
}

QPolygonF QuGraSvgItemGeom::rect() const {
    const QRectF & bo = bounds();
    qDebug() << __PRETTY_FUNCTION__ << d->item->elementId() << "rect" << bo << "bounding rect" << d->item->boundingRect();
    double rad = 2 * M_PI * d->rotation / 360.0;
    const QPointF& c = bo.center();
    const double &x0 = c.x(), &y0 = c.y();
    double p1x = bo.topLeft().x(), p1y = bo.topLeft().y();
    qDebug() << __PRETTY_FUNCTION__ << "rotated by " << 360 * rad / 2.0 / M_PI << "which was " << d->rotation;
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
    return poly;
}

QPointF QuGraSvgItemGeom::center() const {
    return bounds().center();
}

QRectF QuGraSvgItemGeom::bounds() const {
    return d->bounds.isNull() ?
               d->bounds = d->item->renderer()->boundsOnElement(d->item->elementId()) : d->bounds;
}

/*!
 * \brief return the  origin of the axes, relative to the scene
 * \return
 */
QPointF QuGraSvgItemGeom::origin() const {
    const QRectF& b = bounds();
    float x = b.x() + b.width() * d->x0;
    float y = b.y() + b.height() * d->y0;
    double rad = 2 * M_PI * d->rotation / 360.0;
    const QPointF& c = b.center();
    qDebug() << __PRETTY_FUNCTION__ << "rotated by " << 360 * rad / 2.0 / M_PI << "which was " << d->rotation;
    const float& ox = x * cos(rad) - y * sin(rad) +  c.x() * (1 - cos(rad)) + c.y() * sin(rad);
    const float& oy = y * cos(rad) + x * sin(rad) +  c.y() * (1 - cos(rad)) - c.x() * sin(rad);
    return QPointF(ox, oy);
}

void QuGraSvgItemGeom::setMapScale(const float &wrel, const float& hrel) {
    d->bounds = QRectF();
    if(wrel > 0 || hrel > 0) {
        bounds();
        const QPointF& c = bounds().center();
        d->bounds.setWidth(d->bounds.width() * wrel);
        d->bounds.setHeight(d->bounds.height() * hrel);
        d->bounds.moveCenter(c); // same center
    }
}

void QuGraSvgItemGeom::setMapScale(const float &scale) {
    setMapScale(scale, scale);
}



/*!
 * \brief set the relative origin of the XY plane respect to the item rect
 * \param xrel number between 0.0 and 1.0 for the Y axis origin. Default 0.5
 * \param yrel number between 0.0 and 1.0 for the X axis origin. Default 0.5
 *
 * Values equal to 0.5 for both xrel and yrel place the origin of the axes at the
 * center of the item's bounding rectangle
 */
void QuGraSvgItemGeom::setOrigin(const float &xrel, const float& yrel) {
    d->x0 = xrel;
    d->y0 = yrel;
}
