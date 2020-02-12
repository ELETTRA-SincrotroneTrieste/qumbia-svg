#include "qusvg.h"
#include <QMap>
#include <QFileInfo>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QDir>
#include <QSet>
#include <QtDebug>

#include <cumacros.h>

#include "qusvgreaderspool.h"
#include "qusvgdatalistener.h"
#include "qusvgresultdatainterpreter.h"

class QumbiaSVGPrivate {
public:
    QString m_msg;
    QuDom qudom;
    QuSvgReadersPool *readers_pool;
    QSet<QuSvgDataListener *> listeners;
};

QuSvg::QuSvg() {
    d = new QumbiaSVGPrivate;
    d->readers_pool = nullptr;
}

QuSvg::QuSvg(QuDomListener *domlis)
{
    d = new QumbiaSVGPrivate;
    d->readers_pool = nullptr;
    d->qudom.addDomListener(domlis);
}

QuSvg::~QuSvg() {
    delete d;
}

QuDom QuSvg::quDom() const {
    return d->qudom;
}

bool QuSvg::loadFile(const QString &fileName)
{
    d->m_msg.clear();
    if (!QFileInfo::exists(fileName)) {
        d->m_msg = QString("Could not open file '%1'.").arg(QDir::toNativeSeparators(fileName));
        return false;
    }
    QFile f(fileName);
    if(f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f);
        QString svg = in.readAll();
        f.close();
        loadSvg(svg.toLatin1());
    }
    else
        d->m_msg = f.errorString();
    return d->m_msg.isEmpty();
}

bool QuSvg::loadSvg(const QByteArray &svg)
{
    d->m_msg.clear();
    bool success = d->qudom.setContent(svg);
    if(!success)
        d->m_msg = QString("QumbiaSvg.loadSvg: %1").arg(d->qudom.error());
    else  {//  connections
        if(d->readers_pool && d->qudom.hasLinks()) {
            d->readers_pool->setup_links(d->qudom.takeLinkDefs(), this);
        }
        else if(!d->readers_pool && d->qudom.hasLinks())
           d->m_msg = "QumbiaSvg.loadSvg: init() has not been called";
        // do not set an error condition if there are no <link> nodes in the DOM
    }
    return d->m_msg.isEmpty();
}

QString QuSvg::message() const {
    return d->m_msg;
}

bool QuSvg::hasError() const {
    return d->m_msg.size() > 0;
}

bool QuSvg::connect(const QString &source, const QString &id, const QString &property)
{

    return true;
}

void QuSvg::init(CumbiaPool *cupool, const CuControlsFactoryPool &fpool)
{
    d->readers_pool = new QuSvgReadersPool(cupool, fpool, nullptr);
}

void QuSvg::init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
{
    d->readers_pool = new QuSvgReadersPool(cumbia, r_fac, nullptr);
}

void QuSvg::init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac,
                     const CuControlsWriterFactoryI &w_fac)
{
    d->readers_pool = new QuSvgReadersPool(cumbia, r_fac, nullptr);
    // create writers here
    Q_UNUSED(w_fac)
}

/*!
 * \brief Add an observer for new data events
 * \param l QuSvgDataListener implementation
 *
 * @see removeDataListener
 */
void QuSvg::addDataListener(QuSvgDataListener *l) {
    d->listeners.insert(l);
}


/*!
 * \brief Remove the given observer from the list of data listeners
 * \param l QuSvgDataListener to remove
 *
 * @see addDataListener
 */
void QuSvg::removeDataListener(QuSvgDataListener *l) {
    d->listeners.remove(l);
}

void QuSvg::onUpdate(const QuSvgResultData &rd) {
    foreach(QuSvgDataListener *l, d->listeners) {
        bool accept = l->onUpdate(rd, &d->qudom);
        if(!accept) {
            // do our best
            printf("QuSvg.onUpdate: \e[1;32mautomatically processing\e[0m %s\t\t"
                   "%s %s/%s \e[1;34mhint: %s\e[0m\n",
                   rd.data.toString().c_str(), qstoc(rd.id), qstoc(rd.attribute),
                   qstoc(rd.property()), qstoc(rd.key_hint));
            QuSvgResultDataInterpreter interpreter(rd);
            QString i = interpreter.interpret();
            printf("interpreted: %s to set on %s %s\n", qstoc(i),
                   qstoc(rd.id), qstoc(rd.full_attribute()));
            if(!i.isEmpty()) {
                d->qudom.setItemAttribute(rd.id, rd.full_attribute(), i);
            }
        }
    }
}
