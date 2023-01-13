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
    QuSvgView *svgview = new QuSvgView(this);
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
}

Qusvg_extensions::~Qusvg_extensions()
{
    delete ui;
}
