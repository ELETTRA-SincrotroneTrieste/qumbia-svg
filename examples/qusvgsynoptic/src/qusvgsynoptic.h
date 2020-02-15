#ifndef QuSvgExample_H
#define QuSvgExample_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiatango.h>
#include <cumbiaepics.h>

#include <qusvgdatalistener.h>

class CuData;
class CumbiaPool;
class QuSvg;

class QuSvgSynoGlobalListener : public QuSvgDataListener {
public:

    // QuSvgDataListener interface
public:
    bool onUpdate(const QuSvgResultData &res, QuDom *qudom);
};

class QuSvgSynoptic : public QWidget, public QuSvgDataListener
{
    Q_OBJECT

public:
    explicit QuSvgSynoptic(CumbiaPool *cu_p, QWidget *parent = 0);
    ~QuSvgSynoptic();

private slots:
    void applyClicked();
    void onLayerSelectChanged(int index);
    void onLayerVisibilityChange(bool visible);

private:
    QuSvg *m_qu_svg;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;

    // QuSvgDataListener interface
public:
    bool onUpdate(const QuSvgResultData &res, QuDom *qudom);
};

#endif // QuSvgExample_H
