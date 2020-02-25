#ifndef QUSVGHELPERAPPACTIONPROVIDER_H
#define QUSVGHELPERAPPACTIONPROVIDER_H

#include <qusvgitemeventhandler.h>
#include <QDomElement>
#include <QObject>

class QuDom;
class CumbiaPool;
class CuControlsFactoryPool;
class Cumbia;
class CuControlsReaderFactoryI;
class QuSvgConnectionsPool;
class QuSvgReplaceWildcardHelperInterface;
class QuDomElement;

class QuSvgHelperAppActionProviderPrivate;

class QuSvgHelperAppActionProvider : public QObject, public QuSvgActionProviderInterface
{
    Q_OBJECT
public:    

    QuSvgHelperAppActionProvider(QObject *parent,
                                 QuDom *dom,
                                 QuSvgConnectionsPool *c_pool,
                                 QuSvgReplaceWildcardHelperInterface* rwci = nullptr);

    ~QuSvgHelperAppActionProvider();

    // QuSvgActionProviderInterface interface
public:
    QStringList getActionNames(QuGraphicsSvgItem *it) const;
    bool onClicked(QuGraphicsSvgItem *it);
    bool onContextAction(QuGraphicsSvgItem *it, const QString &action_name);
    bool handlesEventType(QuGraphicsSvgItem *it, EventType et) const;
    QString message() const;
    bool hasError() const;
    QString name() const;

private:

    QuSvgHelperAppActionProviderPrivate *d;

    bool m_start_helper_from_reader();
    bool m_start_helper_from_cmd(const QString& cmd);
    bool m_get_src_and_ctx(const QuDomElement &itemel) const;
};

#endif // QUSVGHELPERAPPACTIONPROVIDER_H
