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

/*!
 * \brief The QuDom class loads and represents the DOM of the svg source.
 *
 * Create the DOM calling setContent. This is usually accomplished either
 * by the QuSvg::loadFile or QuSvg::loadSvg methods.
 *
 * A QDomElement can be searched with the findById method or using the overloaded
 * operator [] that takes a string as argument and returns the QDomElement with
 * the specified *id* attribute or a null QDomElement if there is no match.
 */
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

    QDomElement findById(const QString &id, const QDomElement &parent) const;

    QDomElement operator[] (const char *id);
    const QDomElement operator[] (const char *id) const;

    QDomElement operator[] (const QString& id);
    const QDomElement operator[] (const QString&) const;
    QDomElement operator[] (const std::string& id);
    const QDomElement operator[] (const std::string&) const;

    void parse(const QDomNode &parent) const;

    bool setItemAttribute(const QString& id, const QString& attnam, const QString& attval);
    QString itemAttribute(const QString& id, const QString& attnam);

    bool setItemText(const QString& id, const QString& text);
    QString itemText(const QString& id) const;

    bool tagMatch(const QString& tag, const QString& other) const;

    void addDomListener(QuDomListener *l);
    QList<QuDomListener *> getDomListeners() const;

    QList<QuSvgLink> takeLinkDefs() const;
    bool hasLinks() const;

    QString linkTagName() const;

    QString error() const;

    void setCacheOnAccessEnabled(bool enabled);
    bool cacheOnAccessEnabled() const;

private:
    QuDomPrivate *d;

    QMap<QString, QDomElement >& m_get_id_cache() const;
    void m_add_to_cache(const QString& id, QDomElement &dome);
    void m_notify_attribute_change(const QString& id, const QString& attnam,
                                   const QString& attval, QuDomElement *dome);
    QDomNode m_findTexChild(const QDomNode &parent);
};

#endif // QUDOM_H
