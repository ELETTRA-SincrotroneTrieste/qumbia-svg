#ifndef QUGRAPHICSSVGITEMXTENSIONFACTORY_H
#define QUGRAPHICSSVGITEMXTENSIONFACTORY_H

class QuGraphicsSvgItem;
class QuDomElement;
class QString;

class QuGraphicsSvgItemXtensionFactoryPrivate;

class QuGraphicsSvgItemFactory {
public:
    virtual QuGraphicsSvgItem *create() const = 0;
};

class QuGraphicsSvgItemXtensionFactory
{
public:
    QuGraphicsSvgItemXtensionFactory();
    virtual ~QuGraphicsSvgItemXtensionFactory();

    QuGraphicsSvgItem *create(const QuDomElement &de) const;

    void registerItemFactory(const QString& type, QuGraphicsSvgItemFactory *itemfa);
    void unregisterItemFactory(const QString& type);

private:
    QuGraphicsSvgItemXtensionFactoryPrivate *d;
};

#endif // QUGRAPHICSSVGITEMXTENSIONFACTORY_H
