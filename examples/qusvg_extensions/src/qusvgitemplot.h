#ifndef QUSVGITEMPLOT_H
#define QUSVGITEMPLOT_H

#include <qugraphicssvgitem.h>
#include <qugraphicssvgitemxtensionfactory.h>

class QuSvgItemPlotFactory : public QuGraphicsSvgItemFactory {
public:


    // QuGraphicsSvgItemFactory interface
public:
    QuGraphicsSvgItem *create() const;
};

class QuSvgItemPlot : public QuGraphicsSvgItem
{
    Q_OBJECT
public:
    QuSvgItemPlot();
    virtual ~QuSvgItemPlot();

    virtual void 	paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

#endif // QUSVGITEMPLOT_H
