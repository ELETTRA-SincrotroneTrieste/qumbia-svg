#include "qudom.h"
#include <cumacros.h>
#include <QMap>
#include <QPair>
#include <QtDebug>
#include <qudomelement.h>

#include "qusvgreadlink.h"

class QuDomPrivate {
public:
    QDomDocument domdoc;
    QMap<QString, QDomElement > id_cache;
    QMap<QString, QPair<QDomText, QuDomElement> > txt_cache;
    QList<QuDomListener *> dom_listeners;
    QDomElement empty_el;
    QString error;
    QList<QuSvgReadLink > read_links;
    bool cache_on_access;
};

QuDom::~QuDom() {
    delete d;
}

QuDom::QuDom() {
    d = new QuDomPrivate;
    d->cache_on_access = true;
}

QuDom::QuDom(const QuDom &other) {
    m_init_from(other);
}

QuDom &QuDom::operator=(const QuDom &other)
{
    m_init_from(other);
    return *this;
}

void QuDom::m_init_from(const QuDom &other) {
    d = new QuDomPrivate;
    d->domdoc = other.d->domdoc;
    d->id_cache = other.d->id_cache;
    d->dom_listeners = other.d->dom_listeners;
    d->read_links = other.d->read_links;
    d->error = other.d->error;
    d->cache_on_access = other.d->cache_on_access;
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
 * 2. Look for the "read" nodes and create a QuSvgReadLink for each of them. QuSvgReadLink
 *    represents the read configuration: source, target attribute on the parent,
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
        if(!de.isNull() && d->error.isEmpty() && de.tagName() == readerTagName()) {
            QuSvgReadLink readlink(de);
            if(readlink.isValid())
                d->read_links.append(readlink);
            else
                d->error = QString("QuDom: error building read link: %1").arg(readlink.message());
        }
        if(d->error.isEmpty())
            parse(de);
    }
}

bool QuDom::setItemAttribute(const QString &id, const QString &attnam, const QString &value) {
    QuDomElement e = QuDomElement(findById(id, getDocument().firstChildElement()));
    if(!e.isNull()) {
        e.a(attnam, value); // calls m_notify_element_change on this
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
    QuDomElement root;
    QDomNode e;
    if(d->txt_cache.contains(id)) {
        QPair<QDomText, QuDomElement> txt_n = d->txt_cache.value(id);
        root = txt_n.second;
        e = txt_n.first;
    }
    if(e.isNull()) {
        e = findById(id, getDocument().firstChildElement()).toElement();
        if(!e.isNull() && !e.isText()) {
            root = e.toElement();
            e = m_findTexChild(e);
        }
        else if(e.isText()) {
            root = e.parentNode().toElement();
        }
        if(!e.isNull() && e.isText()) // cache id --> (text, text's parent node)
            d->txt_cache[id] = QPair<QDomText, QuDomElement>(e.toText(), root);
    }
    if(!e.isNull()) {
        if(e.toText().nodeValue() != text) {
            e.toText().setNodeValue(text);
            m_notify_element_change(id, &root);
        }
        // checked dumping the result into a QTextStream + QString
    }
    return !e.isNull();
}

QString QuDom::itemText(const QString &id) const {
    Q_UNUSED(id)
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

QList<QuSvgReadLink> QuDom::takeReadLinkDefs() const {
    QList<QuSvgReadLink> links = d->read_links;
    d->read_links.clear();
    return links;
}

/*!
 * \brief Returns true if the dom contains *<read>* nodes.
 * \return true if the DOM contains *<read>* nodes, false otherwise.
 */
bool QuDom::hasLinks() const {
    return d->read_links.size() > 0;
}

QMap<QString, QDomElement> &QuDom::m_get_id_cache() const {
    return d->id_cache;
}

void QuDom::m_add_to_cache(const QString &id, const QDomElement &dome) {
    d->id_cache[id] = dome;
}

void QuDom::m_notify_element_change(const QString &id,
                                      QuDomElement *dome) {
    foreach (QuDomListener *l, d->dom_listeners) {
        l->onElementChange(id, dome);
    }
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

QString QuDom::readerTagName() const {
    return "read";
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
