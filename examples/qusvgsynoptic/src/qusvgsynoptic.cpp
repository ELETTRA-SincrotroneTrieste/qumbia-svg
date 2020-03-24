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
#include <qusvgconnectionspool.h>
#include <qusvgitemeventhandler.h>
#include <qusvgwriteactionprovider.h>
#include <qusvghelperappactionprovider.h>
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
    QuSvgItemEventHandler *item_event_han = new QuSvgItemEventHandler(svgview);
    item_event_han->addActionProvider(new QuSvgHelperAppActionProvider(this, m_qu_svg->quDom(), m_qu_svg->getConnectionsPool()));
    item_event_han->addActionProvider(new QuSvgWriteActionProvider(this, m_qu_svg->quDom(), cu_pool, m_ctrl_factory_pool));
    connect(item_event_han, SIGNAL(error(QString,QString)), this, SLOT(onItemEventHandlerError(QString, QString)));
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

    QLineEdit *lerr = new QLineEdit("-", this);
    lerr->setObjectName("le_error");
    QPushButton *pbClearErr = new QPushButton("Clear error", this);
    pbClearErr->setObjectName("clear_error");
    connect(pbClearErr, SIGNAL(clicked()), this, SLOT(clearError()));
    lo->addWidget(lerr, 7, 0, 1, 4);
    lo->addWidget(pbClearErr, 7, 4, 1, 1);
    lerr->setVisible(false);
    pbClearErr->setVisible(false);

    resize(800, 1000);
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
    QuDom *dom = m_qu_svg->quDom();
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
    findChild<QCheckBox *>()->setChecked(layer.a("visibility") == "visible"
                                      || layer.a("visibility").isEmpty());
}

void QuSvgSynoptic::onLayerVisibilityChange(bool visible) {
    QuSvgLayerHelper lh(m_qu_svg->quDom(), findChild<QuSvgView *>());
    QObject::connect(&lh, SIGNAL(activeSourcesChanged(QStringList,bool)),
            m_qu_svg->getConnectionsPool(), SLOT(activateSources(QStringList,bool)));
    lh.setLayerVisible(findChild<QComboBox *>()->currentText(), visible);
}

void QuSvgSynoptic::clearError() {
    findChild<QLabel *>("label_error")->setText("-");
}

void QuSvgSynoptic::onItemEventHandlerError(const QString &origin, const QString &msg) {
    QMessageBox::critical(this, "Error from " + origin, QString("Error from %1\n%2")
                          .arg(origin).arg(msg));
}

bool QuSvgSynoptic::onUpdate(const QuSvgResultData &res, QuDom *qudom) {
//    if(res.link.id == "rect") {
//        const CuData& d = res.data;
//        const QuDomElement root(qudom);
//        if(d["state_color"].toString( ) == std::string("red")) {
//            root["upper_rect"].a("style/fill", "#00ffff");
//            root["rect"].a("style/fill", "#000fff");
//            root["rect_star"].a("style/fill", "#fffff0");
//            root["lower_rect"].a("style/fill", "#0df009");
//            root["button"].a("style/fill", "#ffffff");
//            root["button"].a("style/stroke", "#f69409");
//            root["button"].a("style/stroke-width", "3");
//            root["rect_circle"].a("style/fill", "#f0f609");
//            qudom->setItemText("states_text", "SWITCH STATES");
//        }
//        else {
//            root["upper_rect"].a("style/fill", "#ff0000"); // orange
//            root["rect"].a("style/fill", "#f69409"); // yellow
//            root["rect_star"].a("style/fill", "#f0f609"); // yellow
//            root["lower_rect"].a("style/fill", "#b718dd"); // violet
//            root["rect_circle"].a("style/fill", "#3ba141"); // green
//            root["button"].a("style/fill", "#ffffff");
//            root["button"].a("style/stroke", "#ff0000");
//            root["button"].a("style/stroke-width", "3");
//            qudom->setItemText("states_text", "SWITCH STATES");
//        }
//        return true;
//    }
    return false;
}

void QuSvgSynoptic::onError(const QString &msg) {
    QLineEdit *le = findChild<QLineEdit *>("le_error");
    QPushButton *clear_err = findChild<QPushButton *>("clear_error");
    clear_err->setVisible(true);
    le->setVisible(true);
    le->setText(msg);
}


bool QuSvgSynoGlobalListener::onUpdate(const QuSvgResultData &res, QuDom *qudom)
{
    return true;
}

void QuSvgSynoGlobalListener::onError(const QString &msg) {
    printf("\e[1;31mQuSvgSynoGlobalListener::onUpdate: error received"
           "%s\e[0m\n",
           qstoc(msg));
}


