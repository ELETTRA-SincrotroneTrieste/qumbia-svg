#include "qugraphicsitemaxes.h"
#include "qugraphicsitemgeom.h"
#include <math.h>
#include <QtDebug>

class QuGraphicsItemAxes_P {
public:
    QuGraphicsItemAxes_P(QuGraphicsItemGeom *g)
        : X(nullptr), Y(nullptr), owner_i(g->item()), geom(g),
        xlb(-1000), xub(1000), ylb(-1000), yub(1000), x0{0.5}, y0{0.5}  {}
    QGraphicsLineItem *X, *Y;
    QuGraphicsItem *owner_i;
    QuGraphicsItemGeom *geom;
    double xlb, xub, ylb, yub;
    double x0, y0; // origin of the axes
};

QuGraphicsItemAxes::QuGraphicsItemAxes(QuGraphicsItemGeom *g) {
    d = new QuGraphicsItemAxes_P(g);
    d->owner_i->installHelper(Axes, this);
}

QuGraphicsItemAxes::~QuGraphicsItemAxes() {
    delete d;
}

QPointF QuGraphicsItemAxes::origin() const {
    const QRectF& b = d->geom->bounds();
    float x = b.x() + b.width() * d->x0;
    float y = b.y() + b.height() * d->y0;
    return QPointF(x, y);
}

/*!
 * \brief set the relative origin of the XY plane respect to the item rect
 * \param xrel number between 0.0 and 1.0 for the Y axis origin. Default 0.5
 * \param yrel number between 0.0 and 1.0 for the X axis origin. Default 0.5
 *
 * Values equal to 0.5 for both xrel and yrel place the origin of the axes at the
 * center of the item's bounding rectangle
 */
void QuGraphicsItemAxes::setOriginRel(const float &xrel, const float& yrel) {
    d->x0 = xrel;
    d->y0 = yrel;
}

/*!
 * \brief return the  origin of the axes, relative to the scene
 * \return
 */
QPointF QuGraphicsItemAxes::transformedOrigin() const {
    return d->geom->map(origin());
}

/*!
 * \brief get the values of the x and y axes origin expressed as percentage of
 *        width and height
 * \return point.x: the relative position of Y axis, from 0 to 1
 *         point.y: the relative position of the X axis, from 0 to 1
 * \see setOrigin
 */
QPointF QuGraphicsItemAxes::originRel() const {
    return QPointF(d->x0, d->y0);
}

/*!
 * \brief creates and returns a line item representing the X axis
 *
 * A line item with two more short *arrow* child items is created according to the
 * QuGraphicsItemGeom provided upon construction.
 * QuGraphicsItemGeom bounds and origin determine the position and length of the axis.
 * The axis is configured according to the attached item's layout (e.g. rotation)
 *
 * \note
 * QuGraphicsItemAxes::X shall be explicitly called after geometry changes
 *
 * \note ownership of X is handed to the caller. A second call to QuGraphicsItemAxes::X
 * will allocate another axis
 *
 * \see QuGraphicsItemAxes::Y
 *
 * \return a QGraphicsLineItem representing the X axis
 */
QGraphicsLineItem *QuGraphicsItemAxes::X() {
    d->X = new QGraphicsLineItem(nullptr);
    QGraphicsLineItem *a1 = new QGraphicsLineItem(d->X);
    QGraphicsLineItem *a2 = new QGraphicsLineItem(d->X);
    a1->setData(0, "arrow1");
    a2->setData(0, "arrow2");
    updateXAxis();
    return d->X;
}

QGraphicsLineItem *QuGraphicsItemAxes::Y()
{
    d->Y = new QGraphicsLineItem(nullptr);
    QGraphicsLineItem *a1 = new QGraphicsLineItem(d->Y);
    QGraphicsLineItem *a2 = new QGraphicsLineItem(d->Y);
    a1->setData(0, "arrow1");
    a2->setData(0, "arrow2");
    updateYAxis();
    return d->Y;
}

void QuGraphicsItemAxes::setPen(QGraphicsLineItem* axis, const QPen &p) {
    if(axis) {
        axis->setPen(p);
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

/*!
 * \brief returns the QuGraphicsItemAxes id
 * \return QuGraphicsItemAxes::Axes
 *
 * \see QuGraphicsItem::helper
 */
int QuGraphicsItemAxes::id() const {
    return Axes;
}

QuGraphicsItem *QuGraphicsItemAxes::item() const {
    return d->owner_i;
}

/*!
 * \brief detach this helper from the item before deleting it
 * \see QugraphicsItemHelper_I::detach
 */
void QuGraphicsItemAxes::detach() {
    d->owner_i->uninstallHelper(this->id());
}

QPointF QuGraphicsItemAxes::xp(const double &x) const {
    const double &yp = origin().y();
    const double& xv = (x - d->xlb ) / (d->xub - d->xlb);
    return d->geom->map(QPointF(d->geom->bounds().x() + xv * d->geom->bounds().width(), yp ));
}

QPointF QuGraphicsItemAxes::yp(const double &y) const {
    const double &xp = origin().x();
    const double& yv = ( y - d->ylb ) / (d->yub - d->ylb);
    const QPointF& p = QPointF(xp, d->geom->bounds().y() + yv * d->geom->bounds().height());
    return d->geom->map(p);
}

double QuGraphicsItemAxes::yLowerBound() const {
    return d->ylb;
}

double QuGraphicsItemAxes::yUpperBound() const {
    return d->yub;
}

void QuGraphicsItemAxes::updateAxes() {
    updateXAxis();
    updateYAxis();
}

void QuGraphicsItemAxes::updateXAxis() {
    if(d->X) {
        const QPointF& o = originRel();
        double oy = o.y(); // origin of the axes
        QPointF p1, p2;
        const QRectF& r = d->geom->bounds();
        // points relative to rect coordinates, without rotation
        const QPointF& rp1 = QPointF(r.left(), r.top() + r.height() * oy);
        const QPointF& rp2 = QPointF (r.right(), rp1.y());
        // rotate around center of rect using geom.map
        p1 = d->geom->map(rp1);
        p2 = d->geom->map(rp2);
        d->X->setLine(QLineF(p1, p2));
        // arrow
        QGraphicsLineItem *a1 = nullptr, *a2 = nullptr;
        foreach(QGraphicsItem *i, d->X->childItems()) {
            if(i->data(0).toString() == "arrow1") a1 = static_cast<QGraphicsLineItem *>(i);
            else if(i->data(0).toString() == "arrow2") a2 = static_cast<QGraphicsLineItem *>(i);
        }
        if(a1 != nullptr && a2 != nullptr) {
            const QPointF& l1end = p2;
            double aw = d->X->line().length() * 0.05;
            const QPointF& a1end = QPointF(rp2.x() - aw * cos(M_PI/6.0), rp2.y() + aw * sin(M_PI/6.0));
            const QPointF& a2end = QPointF(rp2.x() - aw * cos(M_PI/6.0), rp2.y() - aw * sin(M_PI/6.0));
            a1->setLine(QLineF(l1end, d->geom->map(a1end)));
            a2->setLine(QLineF(l1end, d->geom->map(a2end)));
        }
    }
}

void QuGraphicsItemAxes::updateYAxis() {
    if(d->Y) {
        const QPointF& o = originRel();
        double ox = o.x(); // origin of the axes
        QPointF p1, p2;
        const QRectF& r = d->geom->bounds();
        // points relative to rect coordinates, without rotation
        const QPointF& rp2 = QPointF(r.left() + r.width() * ox, r.top());
        const QPointF& rp1 = QPointF (rp2.x(),  r.bottom());
        p1 = d->geom->map(rp1);
        p2 = d->geom->map(rp2);
        // Y axis drawn vertically from bottom to top
        d->Y->setLine(QLineF(p1, p2));
        // arrows
        QGraphicsLineItem *a1 = nullptr, *a2 = nullptr;
        foreach(QGraphicsItem *i, d->X->childItems()) {
            if(i->data(0).toString() == "arrow1") a1 = static_cast<QGraphicsLineItem *>(i);
            else if(i->data(0).toString() == "arrow2") a2 = static_cast<QGraphicsLineItem *>(i);
        }
        if(a1 != nullptr && a2 != nullptr) {
            double aw = d->Y->line().length() * 0.05;
            const QPointF& a1end = QPointF(rp2.x() - aw * sin(M_PI/6.0), rp2.y() + aw * cos(M_PI/6.0));
            const QPointF& a2end = QPointF(rp2.x() + aw * sin(M_PI/6.0), rp2.y() + aw * cos(M_PI/6.0));
            a1->setLine(QLineF(p2, d->geom->map(a1end)));
            a2->setLine(QLineF(p2, d->geom->map(a2end)));
        }
    }
}

