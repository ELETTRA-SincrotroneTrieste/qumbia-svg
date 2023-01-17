#include "qusvg_extensions.h"
#include "ui_qusvg_extensions.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
#include <QGridLayout>
#include <qusvg.h>
#include <qusvgview.h>
#include <qusvgitemeventhandler.h>
#include <qusvghelperappactionprovider.h>
#include <qusvgwriteactionprovider.h>
#include <QMessageBox>
#include <QtAlgorithms>
#include <math.h>
#include <QTimer>
#include <scenecurve.h>
#include <curveitem.h>
#include <linepainter.h>

#include "qusvgitemplot.h"

// cumbia

Qusvg_extensions::Qusvg_extensions(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
    ui = new Ui::Qusvg_extensions;
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    QGridLayout *lo = new QGridLayout(ui->syno_widget);
    svgview = new QuSvgView(this);
    m_qu_svg = new QuSvg(svgview);
    QuSvgItemEventHandler *item_event_han = new QuSvgItemEventHandler(svgview);
    item_event_han->addActionProvider(new QuSvgHelperAppActionProvider(this, m_qu_svg->quDom(), m_qu_svg->getConnectionsPool()));
    item_event_han->addActionProvider(new QuSvgWriteActionProvider(this, m_qu_svg->quDom(), cu_pool, m_ctrl_factory_pool));
    connect(item_event_han, SIGNAL(error(QString,QString)), this, SLOT(onItemEventHandlerError(QString, QString)));
    m_qu_svg->init(cu_pool, m_ctrl_factory_pool);
    lo->addWidget(svgview, 0,0, 5, 5 );

    QuSvgItemPlotFactory *plot_factory = new QuSvgItemPlotFactory;
    svgview->extension_factory()->registerItemFactory("plot", plot_factory);

    // mloader.modules() to get the list of loaded modules
    // cumbia
    bool ok = m_qu_svg->loadFile(":/uh-plain.svg");
    if(!ok) {
        QMessageBox::critical(this, "Error loading svg file", m_qu_svg->message());
    }
    QuSvgItemPlot * ip = qobject_cast<QuSvgItemPlot *>(svgview->item("plot1"));
    QList<QColor> palette = QList<QColor> () << QColor(Qt::green) << QColor(Qt::blue) << Qt::magenta << Qt::cyan;
    mBufsiz = 100;
    nCurves = 1;
    /* create the curves */
    for(int i = 0; i < nCurves; i++)
    {
        /* each curve MUST have a name */
        QString name = QString("Curve %1").arg(i + 1);
        /* SceneCurve manages items in the curve. It does not draw anything */

            qDebug() << __PRETTY_FUNCTION__ << ip << ip->plot();
        SceneCurve *c = ip->plot()->addCurve(name);
        /* set the buffer size */
        c->setBufferSize(mBufsiz);

        /* add the reference to the new curve to the list of curves */
        curves << c;
        /* each curve can be configured by the property dialog, so we must add each
         * curve to the list of configurable objects
         */
//        ui->graphicsPlot->addConfigurableObjects(c->name(), c);

        /* do we want the curves be represented by lines? */
        CurveItem *curveItem = new CurveItem(c, ip->plot());
        c->installCurveChangeListener(curveItem);
        LinePainter *lp = new LinePainter(curveItem);
        lp->setLineColor(palette.at(i % palette.size()));
//        ui->graphicsPlot->addConfigurableObjects(c->name() + " Properties", curveItem);
    }

    ip->plot()->xScaleItem()->setLowerBound(0.0);
    ip->plot()->xScaleItem()->setUpperBound(1.0);
    ip->plot()->yScaleItem()->setLowerBound(0.0);
    ip->plot()->yScaleItem()->setUpperBound(1.0);
    ip->plot()->yScaleItem()->setTickStepLen(0.5);

    mCnt = 0;
    x1 = 0.0;
    y1 = 0.0;
    x2 = 0.0;
    y2 = -10.0;
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePlots()));
    timer->start();
}

void Qusvg_extensions::updatePlots() {
    double amplitude;
    double maxAmplitude;
    double x, y;
    int buf = 100;
    for(int i = 0; i < nCurves; i++)
    {
        SceneCurve *c = curves.at(i);
        maxAmplitude = 1 + i * 2;
        amplitude = qrand() / (double) RAND_MAX * maxAmplitude - maxAmplitude/2.0;
        x1 += 3 / (double) 100.0 /*ui->sbPrecision->value()*/;
        y = sin(x1) * maxAmplitude;
        QuSvgItemPlot * ip = qobject_cast<QuSvgItemPlot *>(svgview->item("plot1"));
        qDebug() << __PRETTY_FUNCTION__ << "QuSvgItemPlot item " << ip;
        ip->plot()->appendData(c->name(), x1, y);
    }
    mCnt++;
}


Qusvg_extensions::~Qusvg_extensions()
{
    delete ui;
}
