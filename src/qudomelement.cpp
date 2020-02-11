#include "qudomelement.h"
#include <QtDebug>

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

QString QuDomElement::itemId() const {
    QString item;
    QDomElement el = m_dome;
    item = el.attribute("item");
    while(item.isEmpty() && !el.parentNode().isNull())
        item = e
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

QuDomElement &QuDomElement::operator [](const QString& id_path) {
    QMap<QString, QDomElement>& idcache = m_qudom.m_get_id_cache();
    QString id;
    id_path.contains("/") ? id = id_path.section('/', 0, 0) : id = id_path;
    if(idcache.contains(id)) {
        m_dome = idcache[id];
    }
    else {
        findById(id, m_dome);
    }
    if(id_path.contains('/') && !m_dome.isNull()) {
        return operator [](id_path.section('/', 1, id_path.count('/')));
    }
    return *this;
}

QuDomElement &QuDomElement::operator [](const std::string& id)
{
    return operator [](QString::fromStdString(id));
}

QuDomElement &QuDomElement::operator [](const char *id)
{
    return operator [](QString(id));
}

QDomElement QuDomElement::element() const {
    return m_dome;
}

QString QuDomElement::attribute(const QString &a) const
{
    return m_dome.attribute(a);
}

void QuDomElement::setAttribute(const QString &name, const QString &value)
{
    m_dome.setAttribute(name, value);
    m_qudom.m_notify_attribute_change(m_dome.attribute("id"), name, value);
}

QString QuDomElement::a(const QString &name) const {
    return attribute(name);
}

void QuDomElement::a(const QString &name, const QString &value) {
    setAttribute(name, value);
}




