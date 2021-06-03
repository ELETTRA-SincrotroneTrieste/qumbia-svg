#ifndef QUSVGITEMEVENTHANDLER_H
#define QUSVGITEMEVENTHANDLER_H

#include <QObject>
#include <QStringList>
#include <QGraphicsItem>

class QuGraphicsSvgItem;
class QuSvgItemEventHandlerPrivate;
class QuSvgView;

class QuSvgActionProviderInterface {
public:
    enum EventType { ClickEvent, ContextualEvent, EnterEvent, LeaveEvent };

    virtual QStringList getActionNames(QuGraphicsSvgItem *it) const = 0;
    virtual bool onClicked(QuGraphicsSvgItem *it) = 0;
    virtual bool onContextAction(QuGraphicsSvgItem *it, const QString& action_name = QString()) = 0;
    virtual bool handlesEventType(QuGraphicsSvgItem *it, EventType et) const = 0;
    virtual QString message() const = 0;
    virtual bool hasError() const = 0;
    virtual QString name() const = 0;
};

class QuSvgItemEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit QuSvgItemEventHandler(QuSvgView *view);
    ~QuSvgItemEventHandler();

    void addActionProvider(QuSvgActionProviderInterface *ap);

public slots:
    void processItemClicked(const QList<QGraphicsItem *> &items);
    void processOnItemContextMenuRequest(const QList<QGraphicsItem *>& items);

private slots:
    void actionTriggered();

signals:
    void error(const QString& origin, const QString& message);

private:
    QuSvgItemEventHandlerPrivate *d;

};

#endif // QUSVGITEMEVENTHANDLER_H
