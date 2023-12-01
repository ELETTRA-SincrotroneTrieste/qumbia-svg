#ifndef QUGRAPHICSITEMAXES_H
#define QUGRAPHICSITEMAXES_H

#include <QGraphicsItem>
#include <qugraphicsitemhelper_i.h>

class QuGraphicsItemAxes_P;
class QuGraphicsItemGeom;

/*!
 * \brief The QuGraphicsItemAxes is a QuGraphicsItem helper that allows mapping
 *        values from an X-Y plane to a point within the QuGraphicsItemGeom bounds
 *        attached to a QuGraphicsItem
 *
 * X and Y axes bounds can be set with QuGraphicsItemAxes::setAxesBounds (for both X
 * and Y) and QuGraphicsItemAxes::setYAxisBounds (specific for the Y axis) according
 * to the minimum and maximum values of the function to be displayed on the X-Y plane.
 * QuGraphicsItemAxes::xp and QuGraphicsItemAxes::yp can be used to get the coordinates
 * within the associated *geom* (QuGraphicsItemGeom) *bounds* (QuGraphicsItemGeom::bounds).
 * *xp* and *yp* account for the associated QuGraphicsItem layout (e.g. rotation),
 * which is managed by QuGraphicsItemGeom. (Because QGraphicsSvgItem loses SVG transformation
 * matrix information when laid out).
 *
 * \see QuGraphicsItemGeom
 * \see QuGraphicsItem
 */
class QuGraphicsItemAxes : public QuGraphicsItemHelper_I
{
public:
    enum HelperId { QuGraphicsItemAxes_Helper = 0x8 };

    QuGraphicsItemAxes(QuGraphicsItemGeom *g);
    virtual ~QuGraphicsItemAxes();
    QGraphicsLineItem *X();
    QGraphicsLineItem *Y();
    void setPen(QGraphicsLineItem *axis, const QPen& p);
    void setAxesBounds(const double& xl, const double& xu, const double &yl, const double &yu);
    void setYAxisBounds(const double &yl, const double& yu);

    QPointF xp(const double& x) const;
    QPointF yp(const double& y) const;

    double yLowerBound() const;
    double yUpperBound() const;

    void updateAxes();
    void updateXAxis();
    void updateYAxis();

    // helper interface methods
    int id() const;
    QuGraphicsItem *item() const;
    void detach();

private:
    QuGraphicsItemAxes_P *d;
};

#endif // QUGRAPHICSITEMAXES_H
