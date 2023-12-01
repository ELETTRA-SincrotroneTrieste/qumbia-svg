#include "qugraphicssvgitemxtensionfactory.h"

#include <qudomelement.h>
#include <QMap>
#include <QString>
#include <qugraphicsitem.h>
#include <QtDebug>

class QuGraphicsItemXtensionFactoryPrivate {
public:
    QMap<QString, QuGraphicsItemFactory *> factories;
};

QuGraphicsItemXtensionFactory::QuGraphicsItemXtensionFactory() : d(new QuGraphicsItemXtensionFactoryPrivate) {

}

QuGraphicsItemXtensionFactory::~QuGraphicsItemXtensionFactory() {
    d->factories.clear();
}

QuGraphicsItem *QuGraphicsItemXtensionFactory::create(const QuDomElement &de) const {
    qDebug() << __PRETTY_FUNCTION__ << "type of QuDomElement" << de.attribute("type") << "factories" << d->factories;
    if(d->factories.contains(de.attribute("type")))
        return d->factories[de.attribute("type")]->create();
    return new QuGraphicsItem();
}

void QuGraphicsItemXtensionFactory::registerItemFactory(const QString &type, QuGraphicsItemFactory *itemfa) {
    d->factories[type] = itemfa;
}

void QuGraphicsItemXtensionFactory::unregisterItemFactory(const QString &type) {
    d->factories.remove(type);
}
