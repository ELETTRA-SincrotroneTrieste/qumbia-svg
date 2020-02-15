#include "qusvgresultdatainterpreter.h"
#include "qusvgresultdata.h"
#include "qudom.h"

#include <cumacros.h>
#include <QColor>
#include <QtDebug>

QuSvgResultDataInterpreter::QuSvgResultDataInterpreter(const QuSvgResultData &rd, const QuDom *qudom)
    : m_d(rd), m_qudom(qudom)
{
    m_interpreted_type = Undefined;
}

QString QuSvgResultDataInterpreter::interpret()
{
    QString res;
    QString key;
    m_d.link.key_hint.size() > 0 ? key = m_d.link.key_hint : key = "value";

    if(m_d.link.attribute.isEmpty() && !m_d.link.id.isEmpty()) {
        // objective type is parent node. It must be clear from the context
        // what must be changed into the parent
        //
        // text node ?
        if(m_qudom->tagMatch(m_d.link.tag_name, "text"))
            res = QString::fromStdString(m_d.data[key.toStdString()].toString());
    }
    else if(m_d.link.attribute.compare("style", Qt::CaseInsensitive) == 0) {
        if(m_d.link.property.compare("fill", Qt::CaseInsensitive) == 0 ||
                m_d.link.property.compare("stroke", Qt::CaseInsensitive) == 0 ) {
            m_interpreted_type = Color;
            QColor c(QString::fromStdString(m_d.data[key.toStdString()].toString()));
            if(c.isValid())
                res = c.name(QColor::HexRgb);
        }
    }
    else {
        m_interpreted_type = Undefined;
    }

    return res;
}

QuSvgResultDataInterpreter::Type QuSvgResultDataInterpreter::interpreted_type() const
{
    return m_interpreted_type;
}
