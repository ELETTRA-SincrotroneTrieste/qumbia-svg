#include "qugraphicsitemaxes.h"
#include "qugraphicsitemgeom.h"
#include <math.h>
#include <QtDebug>

class QuGraphicsItemAxes_P {
public:
    QuGraphicsItemAxes_P(QuGraphicsItemGeom *g, QGraphicsItem *parent)
        : X(nullptr), Y(nullptr), parent_I(parent), geom(g),
        xlb(-1000), xub(1000), ylb(-1000), yub(1000) {}
    QGraphicsItem *X, *Y, *parent_I;
    QuGraphicsItemGeom *geom;
    double xlb, xub, ylb, yub;
};

QuGraphicsItemAxes::QuGraphicsItemAxes(QuGraphicsItemGeom *g, QGraphicsItem *parent) {
    d = new QuGraphicsItemAxes_P(g, parent);
}

QuGraphicsItemAxes::~QuGraphicsItemAxes() {
    delete d;
}

QGraphicsItem *QuGraphicsItemAxes::X() const {
    if(d->X)
        delete d->X;
    d->X = new QGraphicsLineItem(d->parent_I);
    QGraphicsLineItem *li = new QGraphicsLineItem(d->X);
    const QPointF& o = d->geom->origin_rel();
    double oy = o.y(); // origin of the axes
    QPointF p1, p2;
    const QRectF& r = d->geom->bounds();
    // points relative to rect coordinates, without rotation
    const QPointF& rp1 = QPointF(r.left(), r.top() + r.height() * oy);
    const QPointF& rp2 = QPointF (r.right(), rp1.y());
    // rotate around center of rect using geom.map
    p1 = d->geom->map(rp1);
    p2 = d->geom->map(rp2);
    li->setLine(QLineF(p1, p2));
    // arrow
    QGraphicsLineItem *a1 = new QGraphicsLineItem(d->X);
    QGraphicsLineItem *a2 = new QGraphicsLineItem(d->X);
    const QPointF& l1end = p2;
    double aw = li->line().length() * 0.05;
//    a1->setLine(QLineF(l1end, QPointF(l1end.x() - aw * cos(M_PI/6.0), l1end.y() + aw * sin(M_PI/6.0))));
//    a2->setLine(QLineF(l1end, QPointF(l1end.x() - aw * cos(M_PI/6.0), l1end.y() - aw * sin(M_PI/6.0))));
    const QPointF& a1end = QPointF(rp2.x() - aw * cos(M_PI/6.0), rp2.y() + aw * sin(M_PI/6.0));
    const QPointF& a2end = QPointF(rp2.x() - aw * cos(M_PI/6.0), rp2.y() - aw * sin(M_PI/6.0));
    a1->setLine(QLineF(l1end, d->geom->map(a1end)));
    a2->setLine(QLineF(l1end, d->geom->map(a2end)));

    return d->X;
}

QGraphicsItem *QuGraphicsItemAxes::Y() const
{    if(d->Y)
        delete d->Y;
    d->Y = new QGraphicsLineItem(d->parent_I);
    QGraphicsLineItem *li = new QGraphicsLineItem(d->Y);
    const QPointF& o = d->geom->origin_rel();
    double ox = o.x(); // origin of the axes
    QPointF p1, p2;
    const QRectF& r = d->geom->bounds();
    // points relative to rect coordinates, without rotation
    const QPointF& rp2 = QPointF(r.left() + r.width() * ox, r.top());
    const QPointF& rp1 = QPointF (rp2.x(),  r.bottom());
    p1 = d->geom->map(rp1);
    p2 = d->geom->map(rp2);
    // Y axis drawn vertically from bottom to top
    li->setLine(QLineF(p1, p2));
    // arrow
    QGraphicsLineItem *a1 = new QGraphicsLineItem(d->Y);
    QGraphicsLineItem *a2 = new QGraphicsLineItem(d->Y);
    double aw = li->line().length() * 0.05;
    const QPointF& a1end = QPointF(rp2.x() - aw * sin(M_PI/6.0), rp2.y() + aw * cos(M_PI/6.0));
    const QPointF& a2end = QPointF(rp2.x() + aw * sin(M_PI/6.0), rp2.y() + aw * cos(M_PI/6.0));
    a1->setLine(QLineF(p2, d->geom->map(a1end)));
    a2->setLine(QLineF(p2, d->geom->map(a2end)));
    return d->Y;
}

void QuGraphicsItemAxes::setPen(QGraphicsItem*axis, const QPen &p) {
    if(axis) {
        QList<QGraphicsItem *>items = axis->childItems();
        foreach(QGraphicsItem *i, items)
            if(qgraphicsitem_cast<QGraphicsLineItem *>(i))
                qgraphicsitem_cast<QGraphicsLineItem *>(i)->setPen(p);
    }
}

void QuGraphicsItemAxes::setAxesBounds(const double &xl, const double &xu, const double &yl, const double &yu) {
    d->xlb = xl; d->xub = xu; d->ylb = yl; d->yub = yu;
}

void QuGraphicsItemAxes::setYAxisBounds(const double &yl, const double &yu) {
    d->ylb = yl; d->yub = yu;
}

QPointF QuGraphicsItemAxes::xp(const double &x) const {
    const double &yp = d->geom->origin().y();
    const double& xv = (x - d->xlb ) / (d->xub - d->xlb);
    return d->geom->map(QPointF(d->geom->bounds().x() + xv * d->geom->bounds().width(), yp ));
}

QPointF QuGraphicsItemAxes::yp(const double &y) const {
    const double &xp = d->geom->origin().x();
    const double& yv = ( y - d->ylb ) / (d->yub - d->ylb);
    const QPointF& p = QPointF(xp, d->geom->bounds().y() + yv * d->geom->bounds().height());
    qDebug() << __PRETTY_FUNCTION__ << "y" << y << "point " << p;
    return d->geom->map(p);
}
