#ifndef QUSVGRESULTDATAINTERPRETER_H
#define QUSVGRESULTDATAINTERPRETER_H

#include <QString>

class QuSvgResultData;
class QuDom;

class QuSvgResultDataInterpreter
{
public:

    enum Type { Undefined = -1, None,  Color, Number, String, Bool };

    QuSvgResultDataInterpreter(const QuSvgResultData& rd, const QuDom* qudom);

    QString interpret();

    Type interpretedType() const;

    Type interpreted_type() const;

private:
    Type m_interpreted_type;
    const QuSvgResultData &m_d;
    const QuDom *m_qudom;
};

#endif // QUSVGRESULTDATAINTERPRETER_H
