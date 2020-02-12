#include "qusvglink.h"
#include "qudomelement.h"
#include <QHash>
#include <QDomElement>

QuSvgLink::QuSvgLink() {
}

QuSvgLink::QuSvgLink(const QString &s, const QString &a,
                     const QString& prop,
                     const QString &_id, const QString &al) {
    src = s;
    objective = a;
    property = prop;
    id = _id;
    alias = al;
    objective_type = Undefined;
}

QuSvgLink::QuSvgLink(const QDomElement &de) {
    QString a = de.attribute("attribute");
    src = de.attribute("src");
    key_hint = de.attribute("hint");
    if(a.isEmpty()) m_error = QString("QuSvgLink: line %1: \"link\" item has no attribute \"attribute\"").arg(de.lineNumber());
    else if(src.isEmpty()) m_error = QString("QuSvgLink: line %1: \"link\" attribute has no attribute \"src\"").arg(de.lineNumber());
    if(m_error.isEmpty()) {
        QDomElement par_el = de.parentNode().toElement();
        if(!par_el.isNull()) {
            QuDomElement qude(par_el);
            // att contains the attribute name without the property
            // if a is in the form attribute/property, e.g. style/fill
            qude.splitAttName(a, objective, property);
            if(!par_el.hasAttribute(objective) && par_el.tagName() != objective) {
                m_error = QString("QuSvgLink: line %1: \"%2\" item (id \"%3\") has no attribute \"%4\" (%5/%6) "
                                  "and its tag name is not \"%7\"")
                        .arg(de.lineNumber()).arg(par_el.tagName())
                             .arg(par_el.attribute("id"))
                        .arg(objective).arg(objective).arg(property)
                        .arg(objective);
            }
            else {
                tag_name = par_el.tagName();
                id = par_el.attribute("id");
                alias = par_el.attribute("alias");
                par_el.tagName() == objective ? objective_type = TagNameObjectiveType :
                        objective_type = AttributeObjectiveType;
            }
        }
    }
}

bool QuSvgLink::operator ==(const QuSvgLink &other) const {
    return other.src == this->src && other.objective == this->objective
            && other.id == this->id && other.property == this->property
            && other.key_hint == this->key_hint;
}

bool QuSvgLink::isValid() const {
    return m_error.isEmpty() && !id.isEmpty() && !src.isEmpty() && !objective.isEmpty();
}

QString QuSvgLink::message() const {
    return m_error;
}

QString QuSvgLink::toString() const {
    return QString("QuSvgLink src: \"%1\" --> id \"%2\" attribute \"%3\". Alias: \"%4\"")
            .arg(src).arg(id).arg(objective).arg(alias);
}

uint qHash(const QuSvgLink &l, uint seed) {
    QLatin1String s(QString(l.src + "/" + l.id + "/" + l.objective).toLatin1());
    return qHash(s, seed);
}
