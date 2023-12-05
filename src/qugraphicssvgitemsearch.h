#ifndef QUGRAPHICSSVGITEMSEARCH_H
#define QUGRAPHICSSVGITEMSEARCH_H

class QuDom;
class QuSvgView;
class QuGraphicsItem;

class QuGraphicsSvgItemSearch_P;

#include <QString>

class QuGraphicsSvgItemSearch
{
public:
    QuGraphicsSvgItemSearch(QuDom *dom, QuSvgView *view);
    virtual ~QuGraphicsSvgItemSearch();

    QuGraphicsItem *findById(const QString& s) const;
    QList<QuGraphicsItem> *findById(const QRegularExpression& re) const;

    void invalidateCache();
    void invalidateCache(const QString& id);

private:
    QuGraphicsSvgItemSearch_P *d;
};

#endif // QUGRAPHICSSVGITEMSEARCH_H
