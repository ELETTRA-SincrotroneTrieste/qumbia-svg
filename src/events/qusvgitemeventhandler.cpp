#include "qusvgitemeventhandler.h"
#include "qusvgview.h"
#include <cumacros.h>
#include <QMenu>

class QuSvgItemEventHandlerPrivate {
public:
    QList<QuSvgActionProviderInterface *> action_providers;
    QMap<QAction *, QuGraphicsSvgItem *> items_map;
};

QuSvgItemEventHandler::QuSvgItemEventHandler(QuSvgView *view) : QObject(view) {
    d = new QuSvgItemEventHandlerPrivate;
    connect(view, SIGNAL(itemClicked(QuGraphicsSvgItem*,QPointF,QPointF)),
            this, SLOT(processItemClicked(QuGraphicsSvgItem*)));
    connect(view, SIGNAL(itemContextMenuRequest(QuGraphicsSvgItem*,QPointF,QPointF)),
            this, SLOT(processOnItemContextMenuRequest(QuGraphicsSvgItem*)));
}

QuSvgItemEventHandler::~QuSvgItemEventHandler() {
    printf("QuSvgItemEventHandler %p\n", this);
    delete d;
}

void QuSvgItemEventHandler::addActionProvider(QuSvgActionProviderInterface *ap) {
    d->action_providers.append(ap);
}

void QuSvgItemEventHandler::processItemClicked(QuGraphicsSvgItem *it)
{
    foreach(QuSvgActionProviderInterface *ai, d->action_providers) {
        if(ai->handlesEventType(it, QuSvgActionProviderInterface::ClickEvent)) {
            ai->onClicked(it);
            if(ai->hasError())
                emit error(ai->name(), ai->message());
        }
    }
}

void QuSvgItemEventHandler::processOnItemContextMenuRequest(QuGraphicsSvgItem *it)
{
    d->items_map.clear();
    QStringList action_names;
    foreach(QuSvgActionProviderInterface *ai, d->action_providers) {
        if(ai->handlesEventType(it, QuSvgActionProviderInterface::ContextualEvent))
            action_names += ai->getActionNames(it);
    }
    if(!action_names.isEmpty()) {
        QMenu menu(nullptr);
        foreach(QString a, action_names) {
            QAction *ac = new QAction(a, &menu);
            d->items_map.insert(ac, it);
            connect(ac, SIGNAL(triggered()), this, SLOT(actionTriggered()));
            menu.addAction(ac);
        }
        menu.exec(QCursor::pos());
        d->items_map.clear();
    }
}

void QuSvgItemEventHandler::actionTriggered() {
    QAction *a = qobject_cast<QAction *>(sender());
    foreach(QuSvgActionProviderInterface *ai, d->action_providers) {
        ai->onContextAction(d->items_map.value(a), a->text());
        if(ai->hasError())
            emit error(ai->name(), ai->message());
    }
}

