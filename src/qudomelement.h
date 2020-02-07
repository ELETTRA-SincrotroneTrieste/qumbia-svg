#ifndef QUDOMELEMENT_H
#define QUDOMELEMENT_H

#include <QDomElement>

class QuDomElement
{
public:
    QuDomElement(QDomElement &x);

private:
    QDomElement &m_qdome;
};

#endif // QUDOMELEMENT_H
