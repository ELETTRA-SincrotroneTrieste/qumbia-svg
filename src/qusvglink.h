#ifndef QUSVGLINK_H
#define QUSVGLINK_H

#include <QString>

class QDomElement;

/*!
 * \brief The QuSvgLink class describes sources and targets for a link
 */
class QuSvgLink
{
public:
    enum ObjectiveType { Undefined = -1, AttributeObjectiveType, TagNameObjectiveType };

    QuSvgLink();

    QuSvgLink(const QString& s, const QString& a, const QString &prop, const QString& _id,
              const QString& alias = QString());
    QuSvgLink(const QDomElement &e);

    bool operator == (const QuSvgLink& other) const;

    bool isValid() const;

    QString message() const;

    ObjectiveType type() const;


    QString src;
    QString objective, property;
    QString id;
    QString alias;
    QString key_hint;
    QString tag_name;

    ObjectiveType objective_type;

    QString toString() const;

private:
    QString m_error;
};

uint qHash(const QuSvgLink& tag, uint seed);

#endif // LINK_H
