#ifndef QUGRAPHICSITEM_H
#define QUGRAPHICSITEM_H

#include <QGraphicsSvgItem>

class QuGraphicsItemPrivate;
class QuGraphicsItemHelper_I;

class QuGraphicsItem : public QGraphicsSvgItem
{
    Q_OBJECT
public:
    enum Shape { Undefined, ShapeText, ShapeRect, ShapeEllipse, ShapePath };
    enum ItemType { QuGraphicsItemType = QGraphicsItem::UserType + 1 };

    QuGraphicsItem();
    virtual ~QuGraphicsItem();

    bool clickable() const;
    Shape getShape() const;
    int type() const;

    QuGraphicsItemHelper_I *helper(int id) const;
    void installHelper(int id, QuGraphicsItemHelper_I* helper);
    QuGraphicsItemHelper_I*  uninstallHelper(int it);

public slots:
    void setClickable(const QString& c);
    void setShape(const QString& s);

signals:
    void itemEntered(QuGraphicsItem *item);
    void itemLeft(QuGraphicsItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *e);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *e);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QuGraphicsItemPrivate *d;
};

#endif // QUGRAPHICSITEM_H
