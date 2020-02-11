#include "qusvgexample.h"

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
#include <qumbiasvg.h>
#include <QtDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

QuSvgExample::QuSvgExample(CumbiaPool *cumbia_pool, QWidget *parent) :
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
    m_qu_svg = new QumbiaSvg(svgview);
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

    bool ok = m_qu_svg->loadFile(":src/drawing.svg");
    if(!ok) {
        QMessageBox::critical(this, "Error loading svg file", m_qu_svg->message());
    }

    qDebug() << __PRETTY_FUNCTION__ << "1. Test find elements that are items";
    QuDom dom = m_qu_svg->quDom();
    foreach(QString id, QStringList() << "ellipse" << "stella" << "rettangolo"
            << "duecerchi" << "spirale") {
        qDebug() << id << dom[id].attribute("item") << &dom[id] << "NULL? " << dom[id].isNull();
    }

    qDebug() << "";
    qDebug() << "";

    printf("\e[1;32m2. Test find elements that are not items, with findById\e[0m\n");
    foreach(QString id, QStringList() << "ellipse" << "stella" << "rettangolo_blu"
            << "spirale") {
        QDomElement found = dom.findById(id, dom.getDocument().documentElement());
        qDebug() << id  << &found << "NULL? " << found.isNull();
    }

    qDebug() << "";
    qDebug() << "";

    qDebug() << __PRETTY_FUNCTION__ << "3. test change attributes";
    dom.setItemAttribute("rettangolo", "duck", "now");
//    qDebug() << __PRETTY_FUNCTION__ << "dom svg" << dom.getDocument().toString();

    printf("\e[1;32m4. test QuDomElement\e[0m\n");
    QuDomElement de(dom);
    qDebug() << "- ellipse" << "IS NULL? " << de["ellipse"].isNull();
    qDebug() << "- rettangolo" << "IS NULL? " << de["rettangolo"].isNull();
    de["rettangolo"].a("x", "30");
    de["rettangolo"].a("style", "fill:#06fff0;stroke:#007cff;"
                                           "stroke-width:2;");
    qDebug() << "- cambio style under duecerchi/ellipse";

    de["duecerchi/ellipse"].a("style", "fill:#f6f0f0;stroke:#000c00;");
    de["duecerchi/gr2/cerchio_blu"].a("rx", "41");
    de["duecerchi"]["gr2"]["cerchio_blu"].a("style", "fill:#ccccff;stroke:#ff0009;");
    de["duecerchi/gr2/stellina"].a("style", "fill:#ff00ac;stroke:#33ffaa");


}

QuSvgExample::~QuSvgExample()
{

}

void QuSvgExample::applyClicked()
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
