#include "qusvglayerhelper.h"
#include "qusvgview.h"
#include "qudom.h"
#include "qusvg.h"
#include "qudomelement.h"
#include <QtDebug>
#include <cumacros.h>

class QuSvgLayerHelperPrivate {
public:
    QuSvgLayerHelperPrivate(const QuSvg& _qu_svg) : qu_svg(_qu_svg) {

    }
    const QuSvg& qu_svg;
};

QuSvgLayerHelper::QuSvgLayerHelper(const QuSvg &qusvg) : QObject(nullptr)
{
    d = new QuSvgLayerHelperPrivate(qusvg);
}

QuSvgLayerHelper::~QuSvgLayerHelper()
{
    delete d;
}

bool QuSvgLayerHelper::layerVisible(const QString &name) {
    QuDomElement e(d->qu_svg.quDom());
    QuDomElement layer = e[name];
    if(!layer.isNull()) {
        return layer.a("visibility").isEmpty() || layer.a("visibility").compare("visible", Qt::CaseInsensitive) == 0;
    }
    perr("QuSvgLayerHelper::layerVisible no layer found with name \"%s\"", qstoc(name));
    return false;
}

void QuSvgLayerHelper::setLayerVisible(const QString &name, bool v) {
    QuDomElement e(d->qu_svg.quDom());
    QuDomElement layer = e[name];
    if(!layer.isNull()) {
        v ? layer.a("visibility", "visible") : layer.a("visibility", "hidden");
        QStringList srcs = m_findSrcs(layer.element());
        emit activeSourcesChanged(srcs, v);
    }
}

QStringList QuSvgLayerHelper::m_findSrcs(const QDomNode &parent) {
    QStringList r;
    QDomElement e = parent.toElement();
    if(e.hasAttribute("src"))
        r << e.attribute("src");
    QDomNodeList children = e.childNodes();
    for(int i = 0; i < children.size(); i++) {
        r += m_findSrcs(children.at(i));
    }
    return r;
}
