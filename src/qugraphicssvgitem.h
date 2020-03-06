#ifndef QUGRAPHICSSVGITEM_H
#define QUGRAPHICSSVGITEM_H

#include <QGraphicsSvgItem>

class QuGraphicsSvgItemPrivate;

class QuGraphicsSvgItem : public QGraphicsSvgItem
{
    Q_PROPERTY(QString clickable READ clickable WRITE setClickable)
    Q_PROPERTY(QString shape READ getShape WRITE setShape)
    Q_OBJECT
public:
    enum Shape { Undefined, ShapeText, ShapeRect, ShapeEllipse, ShapePath };

    QuGraphicsSvgItem();

    bool clickable() const;
    Shape getShape() const;

public slots:
    void setClickable(const QString& c);
    void setShape(const QString& s);

signals:
    void clicked(QuGraphicsSvgItem *item, const QPointF& scenePos, const QPointF& pos);
    void contextMenuRequest(QuGraphicsSvgItem *item, const QPointF& scenePos, const QPointF& pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *e);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *e);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *e);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QuGraphicsSvgItemPrivate *d;
};

#endif // QUGRAPHICSSVGITEM_H
