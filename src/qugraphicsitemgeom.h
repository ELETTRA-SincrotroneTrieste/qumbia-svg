#ifndef QUGRAPHICSITEMGEOM_H
#define QUGRAPHICSITEMGEOM_H

#include <qugraphicsitem.h>
#include <qugraphicsitemhelper_i.h>
#include <qudom.h>

class QuSvgItemGeom_P;

/*!
 * \brief Item geometry helper
 *
 * Even though QuGraphicsItem is laid out according to *transformations* specified in the *SVG* file,
 * Qt does not preserve the transform matrix. In other words, QuGraphicsItem::transform returns an
 * identity matrix.
 * Actually, QuGraphicsItem items often have a rotation property. QuGraphicsItemGeom help mapping points
 * from the *non transformed, identity matrix* QuGraphicsItem to the actual item's layout.
 *
 * \par note
 * The points returned by the methods in this class are in *scene coordinates*, so that they can be used
 * globally and regardless the specific item to which the geometry helper is attached
 *
 * The methods starting with *transformed* return the same objects returned by their counterparts taking into
 * account the transformation. For example:
 *
 * - bounds -> transformedBounds
 * - origin -> transformedOrigin
 *
 * The bounds method returns the QGraphicsSvgItem bounds as obtained by QSvgRenderer::boundsOnItem
 * The transformedBounds returns a QPolygon with the four vertexes of the *bounds* transformed according
 * to the rotation applied to the item.
 *
 * A *scale* factor can be applied to alter the *bounds* of the geometrical area managed by QuGraphicsItemGeom.
 * This can be useful to provide scaled versions of any drawing made on top of the attached graphics item.
 * After a scale factor is applied with setMapScale, bounds shall return the scaled bounds.
 * Setting zero or negative values in setMapScale, restores the bounds to QSvgRenderer::boundsOnItem
 *
 * The *item* method can be used to retrieve the attached item, especially when bounds have been scaled up and do not
 * match QGraphicsItem *boundingRect* any more.
 *
 * The *map* method is the core function that maps a point from an *untransformed, identity matrix* QuGraphicsItem
 * to the point accounting for the actual layout
 *
 * The *id* method returns a value equal to QuGraphicsItemGeom::QuGraphicsItemGeom_Helper and can be used to
 * fetch the QuGraphicsItemGeom helper installed on a QuGraphicsItem.
 * Upon construction, QuGraphicsItemGeom installs himself on QuGraphicsItem and yields ownership to QuGraphicsItem.
 * For this reason QuGraphicsItemGeom shall outlive its attached item. The latter will delete its helpers upon destruction.
 *
 */
class QuGraphicsItemGeom : public QuGraphicsItemHelper_I
{
public:
    enum HelperId { QuGraphicsItemGeom_Helper = 0x2 };

    QuGraphicsItemGeom(QuGraphicsItem *item, const QuDom *dom);
    virtual ~QuGraphicsItemGeom();
    float rotation() const;

    QRectF transformedBoundingRect() const;
    QPolygon transformedBounds() const;
    QPointF transformedOrigin() const;
    QPointF transformedTopLeft() const;

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

    // helper interface methods
    int id() const;
    QuGraphicsItem *item() const;
    void detach();

private:
    QuSvgItemGeom_P *d;

    float m_get_rotation(const QString& xform) const;
};

#endif // QUGRAPHICSITEMGEOM_H
