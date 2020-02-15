#include "qusvg.h"
#include <QMap>
#include <QFileInfo>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QDir>
#include <QMultiMap>
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
    QMultiMap<QString, QuSvgDataListener *> listeners;
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
    d->listeners.insertMulti(QString(), l);
}

/*!
 * \brief associate the given listener to the specified id
 * \param id the *id* attribute
 * \param l the data listener
 *
 * \par Note
 * *l* will receive updates only for that specific *id*.
 */
void QuSvg::addDataListener(const QString& id, QuSvgDataListener *l) {
    d->listeners.insertMulti(id, l);
}

/*!
 * \brief Remove the given observer from the list of data listeners
 * \param l QuSvgDataListener to remove
 *
 * @see addDataListener
 */
void QuSvg::removeDataListener(QuSvgDataListener *l) {
    QMutableMapIterator<QString, QuSvgDataListener *> mi(d->listeners);
    while(mi.hasNext()) {
        mi.next();
        if(mi.value() == l)
            mi.remove();
    }
}

/*!
 * \brief removes all listeners associated to id
 * \param id the element with id *id*
 *
 * \par Note
 * calling removeDataListener with an empty string causes all listeners
 * previously added with QuSvg::addDataListener(QuSvgDataListener *l) to
 * be removed at once.
 */
void QuSvg::removeDataListeners(const QString &id){
    d->listeners.remove(id);
}

QuSvgReadersPool *QuSvg::getReadersPool() const {
    return d->readers_pool;
}

void QuSvg::onUpdate(const QuSvgResultData &rd) {
    QList<QuSvgDataListener *> liss;
    const QString& id = rd.link.id;
    d->listeners.contains(id) ? liss = d->listeners.values(id)
            : liss = d->listeners.values(QString());

    foreach(QuSvgDataListener *l, liss) {
        bool accept = l->onUpdate(rd, &d->qudom);
        if(!accept) {
            // do our best
            printf("QuSvg.onUpdate: \e[1;32mautomatically processing\e[0m %s\t\t"
                   "%s attribute \"%s/%s\" \e[1;34mhint: %s\e[0m\n",
                   rd.data.toString().c_str(), qstoc(rd.link.id), qstoc(rd.link.attribute),
                   qstoc(rd.link.property), qstoc(rd.link.key_hint));
            QuSvgResultDataInterpreter interpreter(rd, &d->qudom);
            QString i = interpreter.interpret();
            printf("interpreted value (as string): \"%s\" objective id \"%s\" node \"%s\" att: %s {\e[1;36m%s\e[0m}\n", qstoc(i),
                   qstoc(rd.link.id), qstoc(rd.link.tag_name), qstoc(rd.full_attribute()), qstoc(rd.link.src));
            if(!i.isEmpty() && !rd.link.attribute.isEmpty()) {
                if(!d->qudom.setItemAttribute(rd.link.id, rd.full_attribute(), i))
                    perr("QuSvg.onUpdate: failed to set item text  on node <%s> id %s",
                         qstoc(rd.link.tag_name), qstoc(rd.link.id));
            }
            else if(!i.isEmpty()) { // no attribute specified
                if(!d->qudom.setItemText(rd.link.id, i))
                    perr("QuSvg.onUpdate: failed to set item text  on node <%s> id %s",
                         qstoc(rd.link.tag_name), qstoc(rd.link.id));
            }
            else {
                perr("QuSvg.onUpdate: failed to automatically interpret the value from "
                     "\"%s\" with objective node \"%s\" and id \"%s\"",
                     qstoc(rd.link.src), qstoc(rd.link.id), qstoc(rd.link.tag_name));
                perr("QuSvg.onUpdate: please let your QuSvgDataListener.onUpdate method");
                perr("                process the result and return \e[1;31mtrue\e[0m");
            }
        }
    }
}
