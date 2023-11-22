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

#include "qugraphicssvgitem.h"
#include "qusvgconnectionspool.h"
#include "qusvgdatalistener.h"
#include "qusvgresultdatainterpreter.h"

class QumbiaSVGPrivate {
public:
    QString m_msg;
    QuDom *qudom;
    QuSvgConnectionsPool *connections_pool;
    QMultiMap<QString, QuSvgDataListener *> listeners;
};

QuSvg::QuSvg(QuDomListener *domlis)
{
    d = new QumbiaSVGPrivate;
    d->connections_pool = nullptr;
    d->qudom = new QuDom();
    d->qudom->addDomListener(domlis);
}

QuSvg::~QuSvg() {
    delete d->qudom;
    delete d;
}

QuDom *QuSvg::quDom() const {
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
    bool success = d->qudom->setContent(svg);
    if(!success)
        d->m_msg = QString("QumbiaSvg.loadSvg: %1").arg(d->qudom->error());
    else  {//  connections
        if(d->connections_pool && d->qudom->hasLinks()) {
            d->connections_pool->setup_links(d->qudom->takeReadLinkDefs(), this);
        }
        else if(!d->connections_pool && d->qudom->hasLinks())
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

void QuSvg::init(CumbiaPool *cupool, const CuControlsFactoryPool &fpool)
{
    if(!d->connections_pool)
        d->connections_pool = new QuSvgConnectionsPool();
    d->connections_pool->init(cupool, fpool);
}

void QuSvg::init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac,
                 const CuControlsWriterFactoryI &w_fac) {
    if(!d->connections_pool)
        d->connections_pool = new QuSvgConnectionsPool();
    d->connections_pool->init(cumbia, r_fac, w_fac);
}

/*!
 * \brief Add an observer for new data events
 * \param l QuSvgDataListener implementation
 *
 * @see removeDataListener
 */
void QuSvg::addDataListener(QuSvgDataListener *l) {
    d->listeners.insert(QString(), l);
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
    d->listeners.insert(id, l);
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

/*!
 * \brief get a reference to a QuSvgConnectionsPool
 * \return a pointer to the QuSvgConnectionsPool in use
 */
QuSvgConnectionsPool *QuSvg::getConnectionsPool() const {
    // allocate a non init-ed QuSvgConnectionsPool in case of early
    // calls (e.g. to provide a reference to QuSvgConnectionsPool to
    // objects requiring it)
    if(!d->connections_pool)
        d->connections_pool = new QuSvgConnectionsPool(nullptr);
    return d->connections_pool;
}

void QuSvg::onUpdate(const QuSvgResultData &rd) {
    QList<QuSvgDataListener *> liss;
    const QString& id = rd.link.id;
    d->listeners.contains(id) ? liss = d->listeners.values(id)
            : liss = d->listeners.values(QString());

    foreach(QuSvgDataListener *l, liss) {
        bool accept = l->onUpdate(rd, d->qudom);
        if(!accept) {
            QString err;
            // do our best
            QuSvgResultDataInterpreter interpreter(rd, d->qudom);
            QString i = interpreter.interpret();
            if(!i.isEmpty() && !rd.link.attribute.isEmpty()) {
                if(!d->qudom->setItemAttribute(rd.link.id, rd.full_attribute(), i))
                    err = QString("QuSvg.onUpdate: failed to set item text  on node <%1> id %2")
                         .arg(rd.link.tag_name).arg(rd.link.id);
            }
            else if(!i.isEmpty()) { // no attribute specified
                if(!d->qudom->setItemText(rd.link.id, i))
                    err = QString("QuSvg.onUpdate: failed to set item text  on node <%1> id %1")
                         .arg(rd.link.tag_name).arg(rd.link.id);
            }
            else {
                err = QString("QuSvg.onUpdate: failed to interpret the value from "
                     "\"%1\" target node: \"%2\", id: \"%3\"")
                     .arg(rd.link.src).arg(rd.link.id).arg(rd.link.tag_name);
            }
            if(!err.isEmpty())
                l->onError(err);
        }
    }
}
