#include "qusvgresultdata.h"
#include <QMap>
#include <QColor>
#include <cumacros.h>
#include <QtDebug>

class QuSvgResultDataPrivate {
public:
};

QuSvgResultData::QuSvgResultData(const CuData &_data, const QuSvgLink &lnk) :
    data(_data), link(lnk)
{
    d = new QuSvgResultDataPrivate();
}

QuSvgResultData::~QuSvgResultData()
{
    delete d;
}

/*!
 * \brief Returns true if the result addresses a specific property in the
 *        attribute *property list*, e.g. *"style/fill"* or *"style/stroke"*
 * \return true if the result addresses a specific property in the
 *        attribute *property list*, false if the result addresses a simple
 *        attribute, like "rx", or "width".
 */
bool QuSvgResultData::attributeHasProperty() const {
    return link.property.size() > 0;
}

/*!
 * \brief full_attribute returns the full name of the attribute, including a
 *        property if the result addresses an element of a *property
 *        list*, like *"style/fill"*.
 *
 * If no property is addressed, the method returns the simple attribute name,
 * e.g. "width" or "rx".
 *
 * \return attribute-name/attribute-property-in-property-list if property-in-property-list is
 *         not empty or  the simple *attribute-name* otherwise
 */
QString QuSvgResultData::full_attribute() const {
    if(!link.property.isEmpty())
        return QString("%1/%2").arg(link.attribute).arg(link.property);
    return link.attribute;
}

QString QuSvgResultData::property() const {
    return link.property;
}

