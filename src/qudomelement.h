#ifndef QUDOMELEMENT_H
#define QUDOMELEMENT_H

#include <QDomElement>
#include <QDomDocument>
#include <qudom.h>


class QuDomElement
{
public:
    QuDomElement(const QuDom *x);
    QuDomElement(const QDomElement& dome);
    QuDomElement(const QuDom *x, const QDomElement& e);
    QuDomElement(const QuDomElement& other);
    QuDomElement();
    QuDomElement& operator=(const QuDomElement&);
    void setDom(const QuDom* dom);

    bool isNull() const;

    QuDomElement operator []  (const QString &id_path);
    QuDomElement operator []  (const std::string& id);
    QuDomElement operator []  (const char* id);

    QuDomElement operator [] (const QString &id_path) const;

    QDomElement element() const;

    QString attribute(const QString& a) const;
    void setAttribute(const QString& name, const QString& value);
    QString a(const QString& name) const;
    void a(const QString& name, const QString& value);

    QuDomElement findById(const QString &id, const QuDomElement &parent) const;
    QDomElement firstChild(const QString& tagnam) const;
    QDomNodeList children(const QString& tagnam) const;

    QString itemId() const;

    bool splitAttName(const QString& full, QString& attribute, QString& property);
    QString toDeclarationList(QString &name, const QString &value);

private:
    const QuDom* m_qudom;
    QDomElement m_dome;
    QuDomElement m_recursiveFind(const QString& id, const QuDomElement &parent) const;
    QuDomElement m_find_el(const QString &id_path) const;
};

#endif // QUDOMELEMENT_H
