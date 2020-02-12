#include "qusvgresultdatainterpreter.h"
#include "qusvgresultdata.h"

#include <cumacros.h>
#include <QColor>
#include <QtDebug>

QuSvgResultDataInterpreter::QuSvgResultDataInterpreter(const QuSvgResultData &rd)
    : m_d(rd)
{
    m_interpreted_type = Undefined;
}

QString QuSvgResultDataInterpreter::interpret()
{
    QString res;
    QString key;
    m_d.key_hint.size() > 0 ? key = m_d.key_hint : key = "value";

    if(m_d.attribute.compare("style", Qt::CaseInsensitive) == 0) {
        if(m_d.att_property.compare("fill", Qt::CaseInsensitive) == 0 ||
                m_d.att_property.compare("stroke", Qt::CaseInsensitive) == 0 ) {
            m_interpreted_type = Color;
            res = QString::fromStdString(m_d.data[key.toStdString()].toString());
            QColor c(res);
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
