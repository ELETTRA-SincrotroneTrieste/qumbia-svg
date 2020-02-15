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
    bool cache_on_access;
};

QuDom::QuDom() {
    d = new QuDomPrivate;
    d->cache_on_access = true;
}

QuDom::QuDom(const QuDom &other) {
    d = new QuDomPrivate;
    d->domdoc = other.d->domdoc;
    d->id_cache = other.d->id_cache;
    d->dom_listeners = other.d->dom_listeners;
    d->links = other.d->links;
    d->error = other.d->error;
    d->cache_on_access = other.d->cache_on_access;
}

QuDom::~QuDom() {
    delete d;
}

QDomDocument QuDom::getDocument() const {
    return d->domdoc;
}

/*!
 * \brief Populate the DOM with the content in *svg*
 * \param svg the svg source document as QByteArray
 * \return true if successful, false otherwise
 *
 * If the method fails, the error message is available through the
 * QuDom::error call.
 *
 * Cache is cleared before loading the document, so that multiple
 * calls to setContent are possible on the same object.
 */
bool QuDom::setContent(const QByteArray &svg)
{
    int line, column;
    QString msg;
    d->id_cache.clear();
    d->error.clear();
    bool ok = d->domdoc.setContent(svg, &msg, &line, &column);
    if(ok) {
        parse(d->domdoc.documentElement());
        if(d->error.isEmpty() && !d->id_cache.isEmpty()) {
            foreach(QuDomListener *l, d->dom_listeners)
                l->onDocumentLoaded(this, d->id_cache.keys());
        }
    }
    else {
        d->error = QString("QuDom.setContent: error from QDomDocument.setContent:"
                           "\"%1\" at line %2 column %3").arg(msg).arg(line).arg(column);
    }
    return d->error.isEmpty();
}

QString QuDom::getAttribute(const QDomElement &el, const QString &attribute)
{
    return el.attribute(attribute);
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
void QuDom::parse(const QDomNode &parent) const {
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
    QuDomElement e = QuDomElement(findById(id, getDocument().firstChildElement()));
    if(!e.isNull()) {
        e.a(attnam, value);
        m_notify_attribute_change(id, attnam, value, &e);
    }
    return !e.isNull();
}

QDomNode QuDom::m_findTexChild(const QDomNode &parent) {
    QDomNode tn; // element with text
    QDomNodeList dnl = parent.childNodes();
    for(int i = 0; i < dnl.size(); i++) {
        tn = dnl.at(i);
        if(tn.isText())
            return tn;
        return m_findTexChild(tn);
    }
    return QDomNode();
}

bool QuDom::setItemText(const QString &id, const QString &text) {
    QDomNode e = findById(id, getDocument().firstChildElement()).toElement();
    if(!e.isNull() && !e.isText())
        e = m_findTexChild(e);
    if(!e.isNull()) {
        e.toText().setNodeValue(text);
        // checked dumping the result into a QTextStream + QString
    }
    return !e.isNull();
}

QString QuDom::itemText(const QString &id) const {
    QString text;

    return text;
}

bool QuDom::tagMatch(const QString &tag, const QString &other) const {
    return other == tag || (QString("svg:%1").arg(other) == tag);
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

void QuDom::m_add_to_cache(const QString &id, QDomElement &dome) {
    d->id_cache[id] = dome;
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
 * \par Important observation
 * Please consider using QuDomElement::operator [] if you want to perform a faster
 * search when a *id search path* can be provided.
 *
 * Example:
 * \code
 * QuDomElement root_de(this);
 * QuDomElement e = root_de["layer1/rectangle"];
 * if(!e.isNull())
 *      // found!
 * \endcode
 *
 * QuDomElement::operator [] first performs a search on the cache.
 *
 * \par Notes
 * Elements with the *item* attribute set to any value different from "false" are
 * stored in a cache by default. Those represent nodes in the *svg* tree that are
 * directly mapped into QGraphicsSvgItem items in the QuSvgView.
 * If cacheElementsOnAccessEnabled is *true*, all other QDomElement are cached
 * when they are found by this method, so that subsequent searches can hit the
 * cache and be faster.
 * If cacheElementsOnAccessEnabled is *false*, only elements with the *item*
 * attribute not set to false remain cached, and every other search is done
 * recursively traversing the tree.
 */
QDomElement QuDom::findById(const QString& id, const QDomElement& parent) const {
    QDomElement root;
    if(d->id_cache.contains(id))
        root = d->id_cache[id].toElement();
    if(root.isNull())
        root = parent.toElement();
    else // found in cache
        return root;
    // search among children
    QuDomElement qde(parent);
    QDomElement found = qde.findById(id, root).element();
    if(!found.isNull() && d->cache_on_access)
        d->id_cache[id] = found;
    return found;
}

/*!
 * \brief QuDom::operator [] returns the QDomElement with the given id or a
 *        null QDomElement if the *svg* does not contain an element with the given id.
 * \param id the id to search across the *svg* document
 * \return the QDomElement with the given id or a null QDomElement
 *
 * This is a convenience method for findById. It is equivalent to
 * \code findById(id, getDocument().firstChildElement()); \endcode
 *
 * @see findById
 *
 */
QDomElement QuDom::operator[](const char *id) {
    return findById(id, d->domdoc.firstChildElement());
}

const QDomElement QuDom::operator[](const char *id) const {
    return findById(id, d->domdoc.firstChildElement());
}

QDomElement QuDom::operator[](const QString &id) {
    return operator [](id.toStdString().c_str());
}

const QDomElement QuDom::operator[](const QString &id) const {
    return operator [](id.toStdString().c_str());
}

QDomElement QuDom::operator[](const std::string &id) {
    return operator [](id.c_str());
}

const QDomElement QuDom::operator[](const std::string & id) const {
    return operator [](id.c_str());
}

QString QuDom::linkTagName() const {
    return "link";
}

QString QuDom::error() const {
    return d->error;
}

void QuDom::setCacheOnAccessEnabled(bool enabled) {
    d->cache_on_access = enabled;
}

bool QuDom::cacheOnAccessEnabled() const {
    return d->cache_on_access;
}
