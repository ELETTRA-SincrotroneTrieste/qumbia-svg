#include "qudomelement.h"
#include <cumacros.h>
#include <QtDebug>
#include <QRegularExpression>

QuDomElement::QuDomElement(const QuDom &x) : m_qudom(x) {
}

QuDomElement::QuDomElement(const QDomElement &dome)
{
    m_dome = dome;
}

bool QuDomElement::isNull() const {
    return m_dome.isNull();
}

QuDomElement& QuDomElement::findById(const QString& id, const QDomElement &parent) {
    QDomElement root;
    parent.isNull() ?  root = m_qudom.getDocument().firstChildElement()
            : root = parent;
    if(root.attribute("id") == id)
        m_dome = root;
    else
        m_dome = m_recursiveFind(id, root);
    return *this;
}

/*!
 * \brief search from this element up across its parents to find the "item" attribute.
 *        Return the *id* of such item
 *
 * \return the id of the dom element that has the attribute "item"
 *
 * The search is performed from this element up across its parents
 */
QString QuDomElement::itemId() const {
    QString item_name;
    QDomElement e = m_dome;
    item_name = e.attribute("item");
    while(item_name.isEmpty() && !e.isNull()) {
        if(e.hasAttribute("id") && e.hasAttribute("item") && e.attribute("item").compare("false", Qt::CaseInsensitive) != 0) {
            item_name = e.attribute("item");
        } else
            e = e.parentNode().toElement();
    }
    if(!item_name.isEmpty())
        return e.attribute("id");
    return QString();
}

QDomElement QuDomElement::m_recursiveFind(const QString &id, const QDomElement &parent)
{
    QDomNodeList nl = parent.childNodes();
    for(int i = 0; i < nl.size(); i++) {
        QDomElement child = nl.at(i).toElement();
        if(!child.isNull() && child.attribute("id") == id)
            return child;
        if(child.hasChildNodes()) {
            QDomElement e = m_recursiveFind(id, child);
            if(!e.isNull())
                return e;
        }
    }
    return QDomElement();
}

/*!
 * \brief QuDomElement::operator [] search a QuDomElement with the attribute *id*
 *        matching the given id or *id path*
 * \param id_path A single *id* or an *id path* in the form id1/id2/.../idN
 * \return Either a null QuDomElement if the search criteria did not produce a result
 *         or the QuDomElement matching the input *id* or *id path*.
 *
 * \par Observation
 * If a *search path* is provided, the child nodes are selected hierarchically
 * and the result is found faster.
 *
 * \par Notes
 * - The *id* matching is performed on *this* QuDomElement first, before descending
 *   into the children
 * - The QuDom *id cache* is searched first. If the result is a miss, a recursive
 *   search is performed on this QuDomElement's children.
 *
 * @see QuDom::setCacheElementsOnAccessEnabled
 *
 *
 */
QuDomElement &QuDomElement::operator [](const QString& id_path) {
    QMap<QString, QDomElement>& idcache = m_qudom.m_get_id_cache();
    QString id;
    id_path.contains("/") ? id = id_path.section('/', 0, 0) : id = id_path;
    if(idcache.contains(id)) {
        m_dome = idcache[id];
        return *this;
    }
    findById(id, m_dome);
    if(id_path.contains('/') && !m_dome.isNull()) {
        return operator [](id_path.section('/', 1, id_path.count('/')));
    }
    if(!m_dome.isNull() && m_qudom.cacheOnAccessEnabled()) {
        m_qudom.m_add_to_cache(id, m_dome);
    }
    return *this;
}

QuDomElement &QuDomElement::operator [](const std::string& id) {
    return operator [](QString::fromStdString(id));
}

QuDomElement &QuDomElement::operator [](const char *id) {
    return operator [](QString(id));
}

/*!
 * \brief Returns the QDomElement referenced by this object
 * \return The QDomElement referenced by this object
 */
QDomElement QuDomElement::element() const {
    return m_dome;
}

/*!
 * \brief QuDomElement::attribute returns the attribute value corresponding to the
 *        attribute name *a*
 * \param a the attribute name
 * \return the value of the attribute "a"
 *
 * \par Note
 * Calls QDomElement::attribute on the underlying QDomElement
 */
QString QuDomElement::attribute(const QString &a) const
{
    return m_dome.attribute(a);
}

/*! \brief set the attribute *name* to value *value* on this element
 *
 * \param name the attribute name
 * \param value the value to set
 *
 * \par Attribute name
 * You can access sub properties in a *property list* attribute, like "style",
 * specifying the sub property name after the attribute name separated by a "/",
 * e.g.
 *
 * \code setAttribute("style/fill", "#00ff00");
 * \endcode
 *
 * This will either set the fill property to #00ff00 or add it to the "style" attribute
 * if yet missing.
 *
 * @see toDeclarationList
 */
void QuDomElement::setAttribute(const QString &name, const QString &value)
{
    QString v, nam(name);
    name.count("/") == 1 ? v = toDeclarationList(nam, value) : v = value;
    if(!v.isEmpty()) {
        m_dome.setAttribute(nam, v);
        m_qudom.m_notify_attribute_change(m_dome.attribute("id"), nam, v, this);
    }
}

/*!
 * \brief Get the attribute name and the property name from a attribute/property
 *        name
 * \param full the attribute name plus the property selector, e.g. "style/fill"
 * \param attribute the attribute name, e.g. "style"
 * \param property the property name, e.g. "fill"
 * \return true if the *full* parameter is a composite name, e.g. "style/fill",
 *         false if *full* does not contain a '/' separator
 */
bool QuDomElement::splitAttName(const QString &full,
                                QString &attribute,
                                QString &property) {
    attribute = full.section('/', 0, 0);
    property = full.section('/', 1, 1);
    return full.count('/') == 1;
}

/*!
 * \brief From an attribute name like name/sub, get the full attribute value
 *        obtained modifying only the *sub* section of the attribute.
 *
 * If the input *name* does not contain the '/' separator, value is returned without
 * modification.
 * If this element does not have the specified attribute, an empty string is returned.
 *
 * \param name  attribute_name/sub_property: e.g. style/fill  or style/stroke
 *        After the call, *name* will contain the attribute name (e.g. style)
 * \param value the new value, e.g. #ff0000 (red color)
 * \return The attribute with the property changed within the property list, or an
 *         empty string if the element has no attribute with the given name.
 *         If the element contains the attribute with name *name*, but the property
 *         does not exist, the property is added to the property list.
 *         For example you can add to the style the property *stroke-width* and set it
 *         to 5 calling
 *
 *  \code toDeclarationList("style/stroke-width", "5"); \endcode
 *
 * \par Example
 * Before calling the method, a "rect" element has the attribute *style="fill:#ccc00f;stroke:#ff0009"*
 * Call
 *
 * \code toDeclarationList("style/fill", "#ff0000");
 * \endcode
 * returns *style="fill:#ff0000;stroke:#ff0009"* and *name* will be "style"
 */
QString QuDomElement::toDeclarationList(QString& name, const QString &value) {
    QString a;
    QString nam, sub;
    if(!splitAttName(name, nam, sub) && m_dome.hasAttribute(nam))
        a = value;
    if(m_dome.hasAttribute(nam)) {
        a = m_dome.attribute(nam);
        a.endsWith(';') ? a = a : a += ';';
        QRegularExpression re(QString("%1:(.+?);").arg(sub));
        QRegularExpressionMatch ma = re.match(a);
        if(ma.hasMatch()) {
            QStringList caps = ma.capturedTexts();
            if(caps.size() == 2) {
                a.replace(caps.first(), QString("%1:%2;").arg(sub).arg(value));
            }
        }
        else {
            a += QString("%1:%2;").arg(sub).arg(value);
        }
    }
    name = nam;
    return a;
}

/*!
 * \brief QuDomElement::a(const QString &name) is a shortcut for QuDomElement::attribute
 *
 * @see QuDomElement::attribute
 */
QString QuDomElement::a(const QString &name) const {
    return attribute(name);
}

/*!
 * \brief QuDomElement::a(const QString &name, const QString &value) is a shortcut for QuDomElement::setAttribute
 *
 * @see QuDomElement::setAttribute
 */
void QuDomElement::a(const QString &name, const QString &value) {
    setAttribute(name, value);
}




