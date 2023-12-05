#include "qugraphicssvgitemsearch.h"
#include "qudom.h"
#include "qudomelement.h"
#include "qusvgview.h"
#include "qugraphicsitem.h"

#include <QMap>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class QuGraphicsSvgItemSearch_P {
public:
    QuGraphicsSvgItemSearch_P(QuDom *_dom, QuSvgView *_svgv) : dom(_dom), svgv(_svgv) {}
    QuDom *dom;
    QuSvgView *svgv;
    QMap<QString, QList<QuGraphicsItem *> > re_cache;
};

QuGraphicsSvgItemSearch::QuGraphicsSvgItemSearch(QuDom *dom, QuSvgView *view) {
    d = new QuGraphicsSvgItemSearch_P(dom, view);
}

QuGraphicsSvgItemSearch::~QuGraphicsSvgItemSearch() {
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
 * const QuDomElement& el = qudom->findById(s, QDomElement()); // qudom being my QuDom
 * return svgview->item(el.itemId());  // svgview being my QuSvgView
 * \endcode
 *
 * \note QuDom and QuSvgView caching is used
 */
QuGraphicsItem *QuGraphicsSvgItemSearch::findById(const QString &s) const {
    const QuDomElement& el = d->dom->findById(s, QDomElement());
    return d->svgv->item(el.itemId());
}

/*!
 * \brief regular expression based search to fetch a group of items with matching id attributes
 * \param re regular expression search
 * \return a list of QuGraphicsItem matching the search pattern
 *
 * \par Caching
 * Subsequent calls to this method with the same regex pattern will return cached results
 */
QList<QuGraphicsItem> *QuGraphicsSvgItemSearch::findById(const QRegularExpression &re) const {

}

void QuGraphicsSvgItemSearch::invalidateCache() {

}

void QuGraphicsSvgItemSearch::invalidateCache(const QString &id) {
    QMutableMapIterator<QString, QList<QuGraphicsItem *> > it(d->re_cache);
    while(it.hasNext()) {
        const QMap<QString, QList<QuGraphicsItem *> m = it.next();
        const QList<QuGraphicsItem *> &items = m.value();
        foreach(QuGraphicsItem * i, items)
            if(i->elementId() == id)
                d->re_cache.remove()
    }
}


