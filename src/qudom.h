#ifndef QUDOM_H
#define QUDOM_H

#include <QDomDocument>
#include <QMap>

class QuDomPrivate;
class QuDom;
class QuDomElement;
class QuSvgLink;

class QuDomListener {
public:
    virtual void onDocumentLoaded(QuDom *dom,
                                  const QStringList& ids) = 0;

    virtual void onAttributeChange(const QString& source,
                                   const QString& id,
                                   const QString& value,
                                   QuDomElement* dom_e) = 0;
};

class QuDom
{
    friend class QuDomElement;
public:
    QuDom();
    QuDom(const QuDom &other);
    virtual ~QuDom();

    QDomDocument getDocument() const;
    bool setContent(const QByteArray &svg);

    QString getAttribute(const QDomElement& el, const QString& attribute);
    double getAttributeAsDouble(const QDomElement& el, const QString& attribute, bool *ok = nullptr);

    QDomElement findById(const QString &id, const QDomElement &parent) const;
    QDomElement &findByItemId(const QString &id);

    QDomElement& operator[] (const char *id);
    const QDomElement& operator[] (const char *id) const;

    QDomElement& operator[] (const QString& id);
    const QDomElement& operator[] (const QString&) const;
    QDomElement& operator[] (const std::string& id);
    const QDomElement& operator[] (const std::string&) const;

    void parse(const QDomNode &parent) const;

    bool setItemAttribute(const QString& id, const QString& attnam, const QString& attval);
    QString itemAttribute(const QString& id, const QString& attnam);


    void addDomListener(QuDomListener *l);
    QList<QuDomListener *> getDomListeners() const;

    QList<QuSvgLink> takeLinkDefs() const;
    bool hasLinks() const;

    QString linkTagName() const;

    QString error() const;

private:
    QuDomPrivate *d;

    QMap<QString, QDomElement >& m_get_id_cache() const;
    void m_notify_attribute_change(const QString& id, const QString& attnam,
                                   const QString& attval, QuDomElement *dome);
};

#endif // QUDOM_H
