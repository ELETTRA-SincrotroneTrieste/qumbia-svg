#include "qusvglayerhelper.h"
#include "qusvgview.h"
#include "qudom.h"
#include "qusvg.h"
#include "qudomelement.h"
#include <QtDebug>
#include <cumacros.h>

class QuSvgLayerHelperPrivate {
public:
    QuSvgLayerHelperPrivate(QuDom* _qu_dom) : qu_dom(_qu_dom) {

    }
    QuDom *qu_dom;
    QuSvgView *view;
};

QuSvgLayerHelper::QuSvgLayerHelper(QuDom *dom, QuSvgView *view) : QObject(nullptr)
{
    d = new QuSvgLayerHelperPrivate(dom);
    d->view = view;
}

QuSvgLayerHelper::~QuSvgLayerHelper()
{
    delete d;
}

bool QuSvgLayerHelper::layerVisible(const QString &name) {
    QuDomElement e(d->qu_dom);
    QuDomElement layer = e[name];
    if(!layer.isNull()) {
        return layer.a("visibility").isEmpty() || layer.a("visibility").compare("visible", Qt::CaseInsensitive) == 0;
    }
    perr("QuSvgLayerHelper::layerVisible no layer found with name \"%s\"", qstoc(name));
    return false;
}

void QuSvgLayerHelper::setLayerVisible(const QString &name, bool v) {
    QuDomElement e(d->qu_dom);
    QuDomElement layer = e[name];
    if(!layer.isNull()) {
        v ? layer.a("visibility", "visible") : layer.a("visibility", "hidden");
        QStringList ids = m_findSrcIds(layer.element());
        d->view->scene()->update();
        emit activeSourcesChanged(ids, v);
    }
}

QStringList QuSvgLayerHelper::m_findSrcIds(const QDomNode &parent) {
    QStringList r;
    QDomElement e = parent.toElement();
    if(e.hasAttribute("src") ) {
        QDomElement parent_e = e.parentNode().toElement();
        if(!parent_e.isNull() && parent_e.hasAttribute("id"))
            r << parent_e.attribute("id");
    }
    QDomNodeList children = e.childNodes();
    for(int i = 0; i < children.size(); i++) {
        r += m_findSrcIds(children.at(i));
    }
    return r;
}
