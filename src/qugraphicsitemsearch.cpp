#include "qugraphicsitemsearch.h"
#include "qudom.h"
#include "qudomelement.h"
#include "qusvgview.h"
#include "qugraphicsitem.h"
#include <cumacros.h>
#include <QMap>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class QuGraphicsItemSearch_P {
public:
    QuGraphicsItemSearch_P(QuDom *_dom, QuSvgView *_svgv)
        : dom(_dom), svgv(_svgv), cache_en(true) {}
    QuDom *dom;
    QuSvgView *svgv;
    QMap<QString, QList<QuGraphicsItem *> > re_cache;
    bool cache_en;
};

QuGraphicsItemSearch::QuGraphicsItemSearch(QuDom *dom, QuSvgView *view) {
    d = new QuGraphicsItemSearch_P(dom, view);
}

QuGraphicsItemSearch::~QuGraphicsItemSearch() {
    delete d;
}

/*!
 * \brief find a QuGraphicsSvgItem by its id
 * \param s id attribute to search for
 * \return the found QuGraphicsItem or nullptr
 *
 * This is a shortcut for
 *
 * \code
 *  svgview->item(id);  // svgview being my QuSvgView
 * \endcode
 *
 * \note benefits from QuSvgView caching
 */
QuGraphicsItem *QuGraphicsItemSearch::findById(const QString &id) const {
    return d->svgv->item(id);
}

/*!
 * \brief regular expression based search to fetch a group of items with matching id attributes
 * \param re regular expression search
 * \return a list of QuGraphicsItem matching the search re
 *
 * \par Caching
 * Subsequent calls to this method with the same regex pattern will return cached results
 * If the search pattern is new, QuSvgView::items is used
 */
QList<QuGraphicsItem *> QuGraphicsItemSearch::find(const QRegularExpression &re) const {
    if(d->cache_en && d->re_cache.contains(re.pattern())) {
        printf("QuGraphicsItemSearch::find: pattern '%s' \e[1;32mcache hit\e[0m!!\n", qstoc(re.pattern()));
        return d->re_cache[re.pattern()];
    }
    QList<QuGraphicsItem *> items = d->svgv->findItems(re);
    if(d->cache_en)
        d->re_cache[re.pattern()] = items;
    return items;
}

bool QuGraphicsItemSearch::cacheEnabled() const {
    return d->cache_en;
}

void QuGraphicsItemSearch::setCacheEnabled(bool en) {
    d->cache_en = en;
}




