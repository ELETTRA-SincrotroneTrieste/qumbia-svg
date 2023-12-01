#ifndef QUGRAPHICSSVGITEMXTENSIONFACTORY_H
#define QUGRAPHICSSVGITEMXTENSIONFACTORY_H

class QuGraphicsItem;
class QuDomElement;
class QString;

class QuGraphicsItemXtensionFactoryPrivate;

class QuGraphicsItemFactory {
public:
    virtual QuGraphicsItem *create() const = 0;
};

class QuGraphicsItemXtensionFactory
{
public:
    QuGraphicsItemXtensionFactory();
    virtual ~QuGraphicsItemXtensionFactory();

    QuGraphicsItem *create(const QuDomElement &de) const;

    void registerItemFactory(const QString& type, QuGraphicsItemFactory *itemfa);
    void unregisterItemFactory(const QString& type);

private:
    QuGraphicsItemXtensionFactoryPrivate *d;
};

#endif // QUGRAPHICSSVGITEMXTENSIONFACTORY_H
