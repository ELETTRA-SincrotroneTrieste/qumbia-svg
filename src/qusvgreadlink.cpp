#include "qusvgreadlink.h"
#include "qudomelement.h"
#include <cumacros.h>
#include <QHash>
#include <QDomElement>

QuSvgReadLink::QuSvgReadLink() {
}

QuSvgReadLink::QuSvgReadLink(const QDomElement &de) {
    inactive = false;
    objective_type = Undefined;
    QString a = de.attribute("attribute");
    src = de.attribute("src");
    link_id = de.attribute("id"); // not strictly required for the link
    key_hint = de.attribute("hint");
    if(src.isEmpty())
        m_error = QString("QuSvgReadLink: line %1: \"read\" attribute has no \"src\" attribute").arg(de.lineNumber());
    if(m_error.isEmpty()) {
        QDomElement par_el = de.parentNode().toElement();
        if(!par_el.isNull()) {
            QuDomElement qude(par_el);
            if(a.isEmpty()) {
                if(m_parent_count(par_el) == 1) {
                    objective_type = RootObjectiveType;
                }
                else {
                    // the <read> node has no "attribute" attribute defined:
                    // we assume it is clear from the parent node what to do
                    // with the result from "src", for example, we are dealing
                    // with a <text> node and the result is placed there as string
                    objective_type = NodeObjectiveType;
                }
            }
            else { // the element contains the "attribute" attribute
                //
                // split its value into the actual attribute name and property
                // that will be used in the property list, if the value is
                // expressed in the form attribute-name/property-in-property-list
                // e.g. "style/fill"
                qude.splitAttName(a, attribute, property);
                // check that parent node has the specified attribute
                if(!par_el.hasAttribute(attribute)) {
                    m_error = QString("QuSvgReadLink: line %1: \"%2\" item (id \"%3\") "
                                      "has no attribute \"%4\" (%4/%5)")
                            .arg(de.lineNumber()).arg(par_el.tagName())
                            .arg(par_el.attribute("id"))
                            .arg(attribute).arg(property);
                }
                else
                    objective_type = AttributeObjectiveType;
            }

            alias = de.attribute("alias");
            tag_name = par_el.tagName();
            id = par_el.attribute("id");
            if(de.hasAttribute("options"))
                m_get_options(de.attribute("options"));

        } // !par_el.isNull()
    } // m_error is empty
}

bool QuSvgReadLink::operator ==(const QuSvgReadLink &other) const {
    return other.src == this->src && other.attribute == this->attribute
            && other.id == this->id && other.property == this->property
            && other.key_hint == this->key_hint && other.inactive == this->inactive;
}

/*!
 * \brief QuSvgReadLink::isValid returns true if the information stored is enough
 *
 * \par Validity
 * A read element is valid if
 * - its parent has an id
 * - its parent is a valid node with a tag name
 * - if an attribute is specified in the <read> element, the parent must have
 *   that attribute defined as well
 * - if an attribute is not specified in the <read> element, then it must be
 *   clear from the context how to change the contents of the *parent*. For example
 *   a <text> element can host a <read> to change the text data.
 *
 * \return True if the read element contains valid information, false otherwise
 */
bool QuSvgReadLink::isValid() const {
    return m_error.isEmpty() &&
            !id.isEmpty() &&
            !src.isEmpty() &&
            !tag_name.isEmpty() &&
            objective_type != Undefined;
}

QString QuSvgReadLink::message() const {
    return m_error;
}

QString QuSvgReadLink::toString() const {
    return QString("QuSvgReadLink src: \"%1\" --> id \"%2\" attribute \"%3\". Alias: \"%4\"")
            .arg(src).arg(id).arg(attribute).arg(alias);
}

void QuSvgReadLink::m_get_options(const QString& opt_att) {
    foreach(QString prop, opt_att.split(";", QString::SkipEmptyParts)) {
        QStringList sl = prop.split(":", QString::SkipEmptyParts);
        if(sl.size() == 2 && !sl.first().isEmpty() && !sl.last().isEmpty())
            options[sl.first()] = sl.last();
    }
}

int QuSvgReadLink::m_parent_count(const QDomNode &e) const {
    int c = 0;
    QDomNode par = e.parentNode();
    while(!par.isNull()) {
        c++;
        par = par.parentNode();
    }
    return c;
}
