#ifndef QUGRAPHICSITEMSEARCH_H
#define QUGRAPHICSITEMSEARCH_H

class QuDom;
class QuSvgView;
class QuGraphicsItem;
class QDomElement;

class QuGraphicsItemSearch_P;

#include <QString>

/*!
 * \brief perform some advanced search on a QuSvgView
 *
 * Perform QRegularExpression based search across the item ids.
 *
 * If search patterns are limited and known programmatically, operations
 * can benefit from caching.
 * In all other cases (for example, if the user is allowed to perform
 * an undefined number of searches), you may disable caching in order
 * to save memory
 *
 * Simple search with QuGraphicsItemSearch::findById exploits the QuSvgView
 * cache
 */
class QuGraphicsItemSearch
{
public:
    QuGraphicsItemSearch(QuDom *dom, QuSvgView *view);
    virtual ~QuGraphicsItemSearch();

    QuGraphicsItem *findById(const QString& id) const;
    QList<QuGraphicsItem*> find(const QRegularExpression& re) const;

    void setCacheEnabled(bool en);
    bool cacheEnabled() const;

private:
    QuGraphicsItemSearch_P *d;
};

#endif // QUGRAPHICSITEMSEARCH_H
