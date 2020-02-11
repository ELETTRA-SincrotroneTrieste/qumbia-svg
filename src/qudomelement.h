#ifndef QUDOMELEMENT_H
#define QUDOMELEMENT_H

#include <QDomElement>
#include <QDomDocument>
#include <qudom.h>


class QuDomElement
{
public:
    QuDomElement(const QuDom &x);
    QuDomElement(const QDomElement& dome);
    QuDomElement();

    bool isNull() const;

    QuDomElement& operator []  (const QString &id_path);
    QuDomElement& operator []  (const std::string& id);
    QuDomElement& operator []  (const char* id);

    QDomElement element() const;

    QString attribute(const QString& a) const;
    void setAttribute(const QString& name, const QString& value);
    QString a(const QString& name) const;
    void a(const QString& name, const QString& value);

    QuDomElement& findById(const QString &id, const QDomElement &parent);

    QString itemId() const;

private:
    QuDom m_qudom;
    QDomElement m_dome;
    QDomElement m_recursiveFind(const QString& id, const QDomElement &parent);
};

#endif // QUDOMELEMENT_H
