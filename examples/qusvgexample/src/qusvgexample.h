#ifndef QuSvgExample_H
#define QuSvgExample_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiatango.h>
#include <cumbiaepics.h>


class CuData;
class CumbiaPool;
class QumbiaSvg;
class QuSvgExample : public QWidget
{
    Q_OBJECT

public:
    explicit QuSvgExample(CumbiaPool *cu_p, QWidget *parent = 0);
    ~QuSvgExample();

private slots:
    void applyClicked();

private:
    QumbiaSvg *m_qu_svg;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
};

#endif // QuSvgExample_H
