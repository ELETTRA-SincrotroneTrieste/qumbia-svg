#include "qusvgsynoptic.h"

#include <cumbiapool.h>
#include <cumbiaepics.h>
#include <cumbiatango.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuepics-world.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#include <QMessageBox>
#include <QGridLayout>

// cumbia svg includes
#include <qusvgview.h>
#include <qusvg.h>
#include <qusvglayerhelper.h>
#include <qusvgreaderspool.h>
#include <QtDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDateTime>
#include <QComboBox>
#include <QCheckBox>

#include <QSettings> // save line edit values

QuSvgSynoptic::QuSvgSynoptic(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    cu_pool = cumbia_pool;
    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());

    // log
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

//    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);
    QGridLayout *lo = new QGridLayout(this);
    QuSvgView *svgview = new QuSvgView(this);
    m_qu_svg = new QuSvg(svgview);
    m_qu_svg->init(cu_pool, m_ctrl_factory_pool);
    lo->addWidget(svgview, 0,0, 5, 5 );

    QLabel *l = new QLabel("Set attribute", this);
    QLineEdit *le_id = new QLineEdit(this);
    le_id->setObjectName("le_id");
    QLineEdit *le_attnam = new QLineEdit(this);
    le_attnam->setObjectName("le_attnam");
    QLineEdit *le_attval = new QLineEdit(this);
    le_attval->setObjectName("le_attval");
    QPushButton *pb = new QPushButton("APPLY", this);
    lo->addWidget(l, 5, 0, 1, 1);
    lo->addWidget(le_id, 5, 1, 1, 1);
    lo->addWidget(le_attnam, 5, 2, 1, 1);
    lo->addWidget(le_attval, 5, 3, 1, 1);
    lo->addWidget(pb, 5, 4, 1, 1);
    connect(pb, SIGNAL(clicked()), this, SLOT(applyClicked()));

    QSettings s;
    le_id->setText(s.value("id").toString());
    le_attnam->setText(s.value("attnam").toString());
    le_attval->setText(s.value("attval").toString());

    bool ok = m_qu_svg->loadFile(":src/drawing.svg");
    if(!ok) {
        QMessageBox::critical(this, "Error loading svg file", m_qu_svg->message());
    }
    else {
        m_qu_svg->addDataListener(this);
        m_qu_svg->addDataListener("mysvg", new QuSvgSynoGlobalListener);
    }

    QComboBox *cob = new QComboBox(this);
    cob->insertItems(0, QStringList() << "daduck_layer" << "duckrect_layer");
    QCheckBox *cb = new QCheckBox("Visible", this);
    onLayerSelectChanged(0); // initialize checkbox
    connect(cob, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayerSelectChanged(int)));
    connect(cb, SIGNAL(clicked(bool)), this, SLOT(onLayerVisibilityChange(bool)));
    lo->addWidget(cob, 6, 0, 1, 4);
    lo->addWidget(cb, 6, 4, 1, 1);
}

QuSvgSynoptic::~QuSvgSynoptic()
{
    QString id = findChild<QLineEdit *>("le_id")->text();
    QString attn = findChild<QLineEdit *>("le_attnam")->text();
    QString attv = findChild<QLineEdit *>("le_attval")->text();
    QSettings s;
    s.setValue("id", id);
    s.setValue("attnam", attn);
    s.setValue("attval", attv);
}

void QuSvgSynoptic::applyClicked()
{
    QString id = findChild<QLineEdit *>("le_id")->text();
    QString attn = findChild<QLineEdit *>("le_attnam")->text();
    QString attv = findChild<QLineEdit *>("le_attval")->text();
    QuDom dom = m_qu_svg->quDom();
    QuDomElement dome(dom);
    QuDomElement de = dome[id];
    if(de.isNull()) {
        QMessageBox::information(this, "Could not change attribute",
                                 QString("Failed to set value \"%1\" on attribute"
                                         "\'%2\" element id \"%3\"").arg(attv)
                                 .arg(attn).arg(id));
    }
    else
        de.a(attn, attv);
}

void QuSvgSynoptic::onLayerSelectChanged(int ) {
    QString layernam = findChild<QComboBox *>()->currentText();
    QuDomElement de(m_qu_svg->quDom());
    QuDomElement layer = de[layernam];
    qDebug() << __PRETTY_FUNCTION__ << layer.a("visibility");
    findChild<QCheckBox *>()->setChecked(layer.a("visibility") == "visible"
                                      || layer.a("visibility").isEmpty());
}

void QuSvgSynoptic::onLayerVisibilityChange(bool visible) {
    QuSvgLayerHelper lh(*m_qu_svg);
    QObject::connect(&lh, SIGNAL(activeSourcesChanged(QStringList,bool)),
            m_qu_svg->getReadersPool(), SLOT(activateSources(QStringList,bool)));
    lh.setLayerVisible(findChild<QComboBox *>()->currentText(), visible);
}

bool QuSvgSynoptic::onUpdate(const QuSvgResultData &res, QuDom *qudom)
{
    printf("QuSvgSynoptic.onUpdate [\e[1;32mDATA\e[0m]: %s %s [\e[1;36mRECIPIENT\e[0m]: "
        "id: \"\e[0;32m%s\e[0m\" attribute: \"%s/\e[1;36m%s\e[0m\"\n", qstoc(QDateTime::fromMSecsSinceEpoch(
                     res.data["timestamp_ms"].toLongInt()).toString()),
            res.data["value"].toString().c_str(),
         qstoc(res.link.id), qstoc(res.link.attribute), qstoc(res.link.property));

    printf("\e[1;35m: returning false: automatic processing...\e[0m\n");
    return false;
}


bool QuSvgSynoGlobalListener::onUpdate(const QuSvgResultData &res, QuDom *qudom)
{
    printf("\e[1;32mQuSvgSynoGlobalListener::onUpdate: specific global listener received"
           "id %s node %s data %s\e[0m\n",
           qstoc(res.link.id), qstoc(res.link.tag_name), res.data.toString().c_str());
    return true;
}
