#ifndef QUSVGITEMPLOT_H
#define QUSVGITEMPLOT_H

#include <qugraphicssvgitem.h>
#include <qugraphicssvgitemxtensionfactory.h>
#include <qgraphicsplotitem.h>
#include <QVariant>

class QuSvgItemPlotFactory : public QuGraphicsSvgItemFactory {
public:


    // QuGraphicsSvgItemFactory interface
public:
    QuGraphicsSvgItem *create() const;
};

class QuSvgItemPlotPrivate;

class QuSvgItemPlot : public QuGraphicsSvgItem
{
    Q_OBJECT
public:
    QuSvgItemPlot();
    virtual ~QuSvgItemPlot();
    QGraphicsPlotItem *plot() const;

private:
    QuSvgItemPlotPrivate *d;

    // QGraphicsItem interface
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // QUSVGITEMPLOT_H
