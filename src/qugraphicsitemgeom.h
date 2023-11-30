#ifndef QUGRAPHICSITEMGEOM_H
#define QUGRAPHICSITEMGEOM_H

#include <qugraphicssvgitem.h>
#include <qudom.h>

class QuSvgItemGeom_P;

class QuGraphicsItemGeom
{
public:
    QuGraphicsItemGeom(QuGraphicsSvgItem *item, const QuDom *dom);
    virtual ~QuGraphicsItemGeom();
    float rotation() const;

    QPolygon transformedBounds() const;
    QPointF transformedOrigin() const;

    QPointF center() const;
    QPointF origin() const;
    QRectF bounds() const;

    QPointF map(const QPointF& p) const;

    void setOrigin(const float &xrel, const float &yrel);
    QPointF origin_rel() const;

    void setMapScale(const float &wrel, const float& hrel);
    void setMapScale(const float& scale);


    void setXUpperBound(double xu);
    void setXLowerBound(double xl);
    void setYUpperBound(double xu);
    void setYLowerBound(double xl);


private:
    QuSvgItemGeom_P *d;

    float m_get_rotation(const QString& xform) const;
};

#endif // QUGRAPHICSITEMGEOM_H
