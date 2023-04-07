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
#include <markeritem.h>
#include <time.h>
#include <QOpenGLWidget>

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

    mBufsiz = 10000;
    mCnt = 0;
    x1 = 0.0;
    y1 = 0.0;
    x2 = 0.0;
    y2 = -10.0;

    QGridLayout *lo = new QGridLayout(ui->syno_widget);
    svgview = new QuSvgView(this);

    // OPENGL
    if(qApp->arguments().contains("--opengl")) {
        QOpenGLWidget *gl = new QOpenGLWidget();
        QSurfaceFormat format;
        format.setSamples(4);
        gl->setFormat(format);
        svgview->setViewport(gl);
        svgview->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    }
    // OPENGL

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
    mBufsiz = 10000;
    nCurves = 5;
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
        QColor fill_color(lp->lineColor());
        fill_color.setAlpha(120);
        lp->setFillArea(fill_color);
    }

    ip->plot()->xScaleItem()->setLowerBound(0.0);
    ip->plot()->xScaleItem()->setUpperBound(mBufsiz);
    ip->plot()->yScaleItem()->setLowerBound(0.0);
    ip->plot()->yScaleItem()->setUpperBound(1.0);
    ip->plot()->yScaleItem()->setTickStepLen(0.5);

    //    QColor bgc(Qt::lightGray);
    //    bgc.setAlpha(180);
    //    ip->plot()->setBackgroundColor(bgc);
    ip->plot()->xScaleItem()->setGridEnabled(false);
    ip->plot()->yScaleItem()->setGridEnabled(false);
    ip->plot()->setXScaleEnabled(false);
    ip->plot()->setYScaleEnabled(false);

    ip->plot()->setMouseZoomEnabled(true);

    MarkerItem* marker = new MarkerItem(ip->plot());
    ip->plot()->installMouseEventListener(marker);

    QTimer *timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePlots()));
    timer->start();
}

void Qusvg_extensions::updatePlots() {
    double amplitude;
    double maxAmplitude;
    double x1 = 0, y1;
    for(int i = 0; i < nCurves; i++)
    {
        QVector<double> y, x;
        SceneCurve *c = curves.at(i);
        maxAmplitude = 0.5;
        struct timeval tv;
        srand(mCnt++);
        amplitude = rand() / (double) RAND_MAX * (maxAmplitude);
        for(int j = 0; j < mBufsiz; j++) {
            x1 += 10.0 * M_PI / (double) mBufsiz /*ui->sbPrecision->value()*/;
            y1 = sin(x1) * amplitude + maxAmplitude;
            x << j;
            y << y1;
        }
        QuSvgItemPlot * ip = qobject_cast<QuSvgItemPlot *>(svgview->item("plot1"));
        ip->plot()->setData(c->name(), x, y);
    }
    mCnt++;
}


Qusvg_extensions::~Qusvg_extensions()
{
    delete ui;
}
