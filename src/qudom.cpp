#include "qudom.h"
#include <cumacros.h>
#include <QMap>
#include <QtDebug>
#include <qudomelement.h>

class QuDomPrivate {
public:
    QDomDocument domdoc;
    QMap<QString, QDomElement > id_cache;
    QList<QuDomListener *> dom_listeners;
    QDomElement empty_el;
};

QuDom::QuDom() {
    d = new QuDomPrivate;
}

QuDom::QuDom(const QuDom &other) {
    d = new QuDomPrivate;
    d->domdoc = other.d->domdoc;
    d->id_cache = other.d->id_cache;
    d->dom_listeners = other.d->dom_listeners;
}

QuDom::~QuDom() {
    delete d;
}

QDomDocument QuDom::getDocument() const {
    return d->domdoc;
}

bool QuDom::setContent(const QByteArray &svg, QString *msg, int *line, int *column)
{
    bool ok = d->domdoc.setContent(svg, msg, line, column);
    if(ok) {
        collectItemIds(d->domdoc.documentElement());
        if(d->id_cache.isEmpty())
            printf("\e[1;33m* \e[0mQuDom.setContent: no elements with \"item=\"true\" found in document\n");
        foreach(QuDomListener *l, d->dom_listeners)
            l->onDocumentLoaded(this, d->id_cache.keys());
        qDebug() << __PRETTY_FUNCTION__ << "id cache" << d->id_cache.keys();
    }
    return ok;
}

QString QuDom::getAttribute(const QDomElement &el, const QString &attribute)
{
    return el.attribute(attribute);
}

double QuDom::getAttributeAsDouble(const QDomElement &el, const QString &attribute, bool *ok)
{
    double d = el.attribute(attribute).toDouble(ok);
    return d;
}

/*!
 * \brief Find the elements having the *item* attribute set to "true" and return a
 *        string list with their ids.
 * \return List of the ids of the elements in the DOM that have the *item* attribute
 *         set to "true"
 *
 * \note Elements with the *item="true" attribute will have a dedicated QGraphicsSvgItem
 *       in the QGraphicsScene. Respective child items will be rendered by the same
 *       QGraphicsSvgItem
 */
void QuDom::collectItemIds(const QDomNode &parent) const {
    QDomNodeList nl = parent.childNodes();
    for(int i = 0; i < nl.size(); i++) {
        QString id;
        QDomElement de = nl.at(i).toElement();
        if(!de.isNull() && de.hasAttribute("item")) {
            id = de.attribute("id");
            if(id.compare("false", Qt::CaseInsensitive) != 0)
                d->id_cache[id] = de;
        }
        collectItemIds(de);
    }
}

bool QuDom::setItemAttribute(const QString &id, const QString &attnam, const QString &value) {
    qDebug() << __PRETTY_FUNCTION__ << d << &d->id_cache;
    qDebug() << __PRETTY_FUNCTION__ << d << d->id_cache.size();
    qDebug() << __PRETTY_FUNCTION__ << "seeing if cache " << d->id_cache.keys() << "contains " << id;
    bool item_exists = d->id_cache.contains(id);
    if(item_exists) {
        QDomElement& e = d->id_cache[id];
        e.setAttribute(attnam, value);
        foreach (QuDomListener *l, d->dom_listeners) {
            l->onAttributeChange(id, attnam, value);
        }
    }
    return item_exists;
}

void QuDom::addDomListener(QuDomListener *l) {
    d->dom_listeners.append(l);
}

QList<QuDomListener *> QuDom::getDomListeners() const {
    return d->dom_listeners;
}

QMap<QString, QDomElement> &QuDom::m_get_id_cache() const {
    return d->id_cache;
}

void QuDom::m_notify_attribute_change(const QString &id, const QString &attnam, const QString &attval) {
    foreach (QuDomListener *l, d->dom_listeners)
        l->onAttributeChange(id, attnam, attval);
}

/*!
 * \brief returns the QDomElement with the given id, searched under parent
 * \param id the id to search for
 * \param parent the parent node
 * \return the found QDomElement or an empty QDomElement
 *
 * \note If you want to get references to dom elements that are *items* (i.e. have
 * the *item* attribute), use the operator [] or findByItemId, that perform the search
 * in a cached id - QDomElement map.
 */
QDomElement QuDom::findById(const QString& id, const QDomElement& parent) const {
    QDomElement root;
    if(d->id_cache.contains(id))
        root = d->id_cache[id].toElement();
    if(root.isNull())
        root = parent.toElement();
    else
        return root;
    QuDomElement qde(parent);
    return qde.findById(id, root).element();
}

/*! \brief Same as operator []: finds the element with the provided id among those
 *         with the *item* attribute defined
 *
 * @param id the id of the item to be searched
 * @return a reference to the QDomElement with the given id or a reference to an
 *         empty QDomElement
 *
 * \note
 * Use findById to search among all ids, including those that are not *item*s.
 */
QDomElement& QuDom::findByItemId(const QString &id)
{
    return d->id_cache[id];
}

/*!
 * \brief QuDom::operator [] returns the QDomElement with the given id or inserts a
 *        new element with tag name *"element"*, *id="item_id"* and *item="true"*
 * \param item_id the id to search in the *items cache*, i.e. the id must belong to
 *        an element whose attribute "item" is defined in the svg.
 * \return the QDomElement with the given id among the elements with the *item* attribute
 *         defined or a new element with tag name *"element"*, *id="item_id"* and *item="true"*
 *
 * \note item_id is searched under the elements with the *item* attribute defined.
 *       if you want to search through all the IDs, please use findById
 *
 */
QDomElement &QuDom::operator[](const char *item_id) {

    if(!d->id_cache.contains(item_id)) {
        return d->empty_el;
    }
    return d->id_cache[item_id];
}

const QDomElement &QuDom::operator[](const char *id) const {
    if(d->id_cache.contains(id))
        return d->id_cache[id];
    return d->empty_el;
}

QDomElement &QuDom::operator[](const QString &id) {
    return operator [](id.toStdString().c_str());
}

const QDomElement &QuDom::operator[](const QString &id) const {
    return operator [](id.toStdString().c_str());
}

QDomElement &QuDom::operator[](const std::string &id) {
    return operator [](id.c_str());
}

const QDomElement &QuDom::operator[](const std::string & id) const {
    return operator [](id.c_str());
}
