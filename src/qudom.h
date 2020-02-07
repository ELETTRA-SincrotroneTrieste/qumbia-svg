#ifndef QUDOM_H
#define QUDOM_H

#include <QDomDocument>
#include <qudomelement.h>

class QuDomPrivate;
class QuDom;

class QuDomListener {
public:
    virtual void onDocumentLoaded(QuDom *dom,
                                  const QStringList& ids) = 0;

    virtual void onAttributeChange(const QString& source,
                                   const QString& id,
                                   const QString& value) = 0;
};

class QuDom
{
public:
    QuDom();
    virtual ~QuDom();

    QDomDocument getDocument() const;
    bool setContent(const QByteArray &svg, QString *m_msg, int *line,  int *column);

    QString getAttribute(const QDomElement& el, const QString& attribute);
    double getAttributeAsDouble(const QDomElement& el, const QString& attribute, bool *ok = nullptr);

    QDomElement findById(const QString &id, const QDomNode &parent) const;
    QDomElement &findByItemId(const QString &id);

    QDomElement& operator[] (const char *id);
    const QDomElement& operator[] (const char *id) const;

    QDomElement& operator[] (const QString& id);
    const QDomElement& operator[] (const QString&) const;
    QDomElement& operator[] (const std::string& id);
    const QDomElement& operator[] (const std::string&) const;

    void collectItemIds(const QDomNode &parent) const;

    bool setItemAttribute(const QString& id, const QString& attnam, const QString& attval);
    QString itemAttribute(const QString& id, const QString& attnam);


    void addDomListener(QuDomListener *l);
    QList<QuDomListener *> getDomListeners() const;

private:
    QuDomPrivate *d;
};

#endif // QUDOM_H
