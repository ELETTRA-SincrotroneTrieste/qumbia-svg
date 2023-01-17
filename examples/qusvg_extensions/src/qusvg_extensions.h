#ifndef Qusvg_extensions_H
#define Qusvg_extensions_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
class QuSvg;
class SceneCurve;
class QuSvgView;
// cumbia

namespace Ui {
class Qusvg_extensions;
}

class Qusvg_extensions : public QWidget
{
    Q_OBJECT

public:
    explicit Qusvg_extensions(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Qusvg_extensions();

protected slots:
    void updatePlots();
private:
    Ui::Qusvg_extensions *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia

    QuSvg* m_qu_svg;
    QuSvgView *svgview ;
    QList<SceneCurve *> curves;
    double x1, y1, x2, y2;
    int mCnt;
    int nCurves;
    int mBufsiz;
};

#endif // Qusvg_extensions_H
