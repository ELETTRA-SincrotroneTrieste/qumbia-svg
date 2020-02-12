#include "qudom.h"
#include <cumacros.h>
#include <QMap>
#include <QtDebug>
#include <qudomelement.h>

#include "qusvglink.h"

class QuDomPrivate {
public:
    QDomDocument domdoc;
    QMap<QString, QDomElement > id_cache;
    QList<QuDomListener *> dom_listeners;
    QDomElement empty_el;
    QString error;
    QList<QuSvgLink > links;
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

bool QuDom::setContent(const QByteArray &svg)
{
    int line, column;
    QString msg;
    bool ok = d->domdoc.setContent(svg, &msg, &line, &column);
    if(ok) {
        parse(d->domdoc.documentElement());
        if(d->error.isEmpty()) {
            if(d->id_cache.isEmpty())
                printf("\e[1;33m* \e[0mQuDom.setContent: no elements with \"item=\"true\" found in document\n");
            foreach(QuDomListener *l, d->dom_listeners)
                l->onDocumentLoaded(this, d->id_cache.keys());
        }
    }
    else {
        d->error = QString("QDom.setContent: QDomDocument.setContent returned an error:"
                           "\"%1\" at line %2 column %3").arg(msg).arg(line).arg(column);
    }
    return d->error.isEmpty();
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

/* 1. Find the elements having the "item" attribute set to a value different from "false"
 *    and fill a map of ids -> QDomElement that will be associated to a QGraphicsSvgItem
 * 2. Look for the "link" nodes and create a QuSvgLink for each of them. QuSvgLink
 *    represents the link configuration: source, target attribute on the parent,
 *    id of the parent, an optional alias.
 *
 *  NOTE
 *  Elements with the item attribute defined and not set to "false" will have a dedicated QGraphicsSvgItem
 *  in the QGraphicsScene. Its child items will be rendered by the same
 *  QGraphicsSvgItem
 */

int indent = 0;

void QuDom::parse(const QDomNode &parent) const {
    ++indent;
    d->error.clear();
    // recursive into children
    QDomNodeList nl = parent.childNodes();
    for(int i = 0; i < nl.size() && d->error.isEmpty(); i++) {
        QString id;
        QDomElement de = nl.at(i).toElement();
        if(!de.isNull() && !de.hasAttribute("id") && de.hasAttribute("item"))
            d->error = QString("QuDom: line %1 col %2: item %3 has no id").arg(de.lineNumber()).arg(de.columnNumber()).arg(de.tagName());
        else if(!de.isNull() && de.hasAttribute("item")) {
            id = de.attribute("id");
            if(de.attribute("item").compare("false", Qt::CaseInsensitive) != 0)
                d->id_cache[id] = de;
        }
        if(!de.isNull() && d->error.isEmpty() && de.tagName() == linkTagName()) {
            QuSvgLink link(de);
            if(link.isValid())
                d->links.append(link);
            else
                d->error = QString("QuDom: error building link: %1").arg(link.message());
        }
        if(d->error.isEmpty())
            parse(de);
    }
}

bool QuDom::setItemAttribute(const QString &id, const QString &attnam, const QString &value) {
    qDebug() << __PRETTY_FUNCTION__ << d << &d->id_cache;
    qDebug() << __PRETTY_FUNCTION__ << d << d->id_cache.size();
    qDebug() << __PRETTY_FUNCTION__ << "seeing if cache " << d->id_cache.keys() << "contains " << id;
    bool item_exists = d->id_cache.contains(id);
    if(item_exists) {
        QuDomElement e = d->id_cache[id];
        e.a(attnam, value);
        m_notify_attribute_change(id, attnam, value, &e);
    }
    return item_exists;
}

void QuDom::addDomListener(QuDomListener *l) {
    d->dom_listeners.append(l);
}

QList<QuDomListener *> QuDom::getDomListeners() const {
    return d->dom_listeners;
}

QList<QuSvgLink> QuDom::takeLinkDefs() const {
    QList<QuSvgLink> links = d->links;
    d->links.clear();
    return links;
}

/*!
 * \brief Returns true if the dom contains *<link>* nodes.
 * \return true if the DOM contains *<link>* nodes, false otherwise.
 */
bool QuDom::hasLinks() const {
    return d->links.size() > 0;
}

QMap<QString, QDomElement> &QuDom::m_get_id_cache() const {
    return d->id_cache;
}

void QuDom::m_notify_attribute_change(const QString &id,
                                      const QString &attnam,
                                      const QString &attval,
                                      QuDomElement *dome) {
    foreach (QuDomListener *l, d->dom_listeners)
        l->onAttributeChange(id, attnam, attval, dome);
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

QString QuDom::linkTagName() const {
    return "link";
}

QString QuDom::error() const {
    return d->error;
}
