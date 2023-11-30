#ifndef QUGRAPHICSITEMAXES_H
#define QUGRAPHICSITEMAXES_H

#include <QGraphicsItem>
class QuGraphicsItemAxes_P;
class QuGraphicsItemGeom;

class QuGraphicsItemAxes
{
public:
    QuGraphicsItemAxes(QuGraphicsItemGeom *g, QGraphicsItem *parent = nullptr);
    virtual ~QuGraphicsItemAxes();
    QGraphicsItem *X() const;
    QGraphicsItem *Y() const;
    void setPen(QGraphicsItem *axis, const QPen& p);
    void setAxesBounds(const double& xl, const double& xu, const double &yl, const double &yu);
    void setYAxisBounds(const double &yl, const double& yu);

    QPointF xp(const double& x) const;
    QPointF yp(const double& y) const;

private:
    QuGraphicsItemAxes_P *d;
};

#endif // QUGRAPHICSITEMAXES_H
