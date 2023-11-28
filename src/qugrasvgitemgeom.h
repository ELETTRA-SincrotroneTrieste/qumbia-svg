#ifndef QUGRASVGITEMGEOM_H
#define QUGRASVGITEMGEOM_H

#include <qugraphicssvgitem.h>
#include <qudom.h>

class QuSvgItemGeom_P;

class QuGraSvgItemGeom
{
public:
    QuGraSvgItemGeom(QuGraphicsSvgItem *item, const QuDom *dom);
    virtual ~QuGraSvgItemGeom();
    float rotation() const;

    QPolygonF rect() const;
    QPointF center() const;
    QRectF bounds() const;

    void setOrigin(const float &xrel, const float &yrel);
    QPointF origin() const;

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

#endif // QUGRASVGITEMGEOM_H
