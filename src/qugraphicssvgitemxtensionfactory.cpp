#include "qugraphicssvgitemxtensionfactory.h"

#include <qudomelement.h>
#include <QMap>
#include <QString>
#include <qugraphicssvgitem.h>
#include <QtDebug>

class QuGraphicsSvgItemXtensionFactoryPrivate {
public:
    QMap<QString, QuGraphicsSvgItemFactory *> factories;
};

QuGraphicsSvgItemXtensionFactory::QuGraphicsSvgItemXtensionFactory() : d(new QuGraphicsSvgItemXtensionFactoryPrivate) {

}

QuGraphicsSvgItemXtensionFactory::~QuGraphicsSvgItemXtensionFactory() {
    d->factories.clear();
}

QuGraphicsSvgItem *QuGraphicsSvgItemXtensionFactory::create(const QuDomElement &de) const {
    qDebug() << __PRETTY_FUNCTION__ << "type of QuDomElement" << de.attribute("type") << "factories" << d->factories;
    if(d->factories.contains(de.attribute("type")))
        return d->factories[de.attribute("type")]->create();
    return new QuGraphicsSvgItem();
}

void QuGraphicsSvgItemXtensionFactory::registerItemFactory(const QString &type, QuGraphicsSvgItemFactory *itemfa) {
    d->factories[type] = itemfa;
}

void QuGraphicsSvgItemXtensionFactory::unregisterItemFactory(const QString &type) {
    d->factories.remove(type);
}
