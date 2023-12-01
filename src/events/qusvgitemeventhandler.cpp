#include "qusvgitemeventhandler.h"
#include "qugraphicsitem.h"
#include "qusvgview.h"
#include <QGraphicsItem>
#include <cumacros.h>
#include <QMenu>
#include <QtDebug>

class QuSvgItemEventHandlerPrivate {
public:
    QList<QuSvgActionProviderInterface *> action_providers;
    QMap<QAction *, QuGraphicsItem *> items_map;
};

QuSvgItemEventHandler::QuSvgItemEventHandler(QuSvgView *view) : QObject(view) {
    d = new QuSvgItemEventHandlerPrivate;
    connect(view, SIGNAL(itemClicked(const QList<QGraphicsItem *> &,QPointF,QPointF)),
            this, SLOT(processItemClicked(const QList<QGraphicsItem *> &)));
    connect(view, SIGNAL(itemContextMenuRequest(const QList<QGraphicsItem *> &,QPointF,QPointF)),
            this, SLOT(processOnItemContextMenuRequest(const QList<QGraphicsItem *> &)));
    connect(view, SIGNAL(itemEntered(QuGraphicsItem *)),
            this, SLOT(processItemEntered(QuGraphicsItem *)));
    connect(view, SIGNAL(itemLeft(QuGraphicsItem *)),
            this, SLOT(processItemLeft(QuGraphicsItem *)));
}

QuSvgItemEventHandler::~QuSvgItemEventHandler() {
    printf("QuSvgItemEventHandler %p\n", this);
    delete d;
}

void QuSvgItemEventHandler::addActionProvider(QuSvgActionProviderInterface *ap) {
    d->action_providers.append(ap);
}

void QuSvgItemEventHandler::processItemClicked(const QList<QGraphicsItem *> &items) {
    foreach(QGraphicsItem *git, items) {
        QuGraphicsItem *it = qobject_cast<QuGraphicsItem *>(git->toGraphicsObject());
        if(it) {
            foreach(QuSvgActionProviderInterface *ai, d->action_providers) {
                if(ai->handlesEventType(it, QuSvgActionProviderInterface::ClickEvent)) {
                    ai->onClicked(it);
                    if(ai->hasError())
                        emit error(ai->name(), ai->message());
                }
            }
        }
    }
}

void QuSvgItemEventHandler::processOnItemContextMenuRequest(const QList<QGraphicsItem *> &items)
{
    qDebug() << __PRETTY_FUNCTION__ << "items" << items.size() << "action providers size" << d->action_providers.size();
    d->items_map.clear();
    QMenu *menu = nullptr;
    foreach(QGraphicsItem *git, items) {
        QuGraphicsItem *it = qobject_cast<QuGraphicsItem *>(git->toGraphicsObject());
        if(it) {
            QStringList action_names;
            foreach(QuSvgActionProviderInterface *ai, d->action_providers) {
                if(ai->handlesEventType(it, QuSvgActionProviderInterface::ContextualEvent)) {
                    if(!menu)
                        menu = new QMenu(nullptr);
                    action_names += ai->getActionNames(it);
                    foreach(QString a, action_names) {
                        QAction *ac = new QAction(a, menu);
                        d->items_map.insert(ac, it);
                        connect(ac, SIGNAL(triggered()), this, SLOT(actionTriggered()));
                        menu->addAction(ac);
                    }
                }
            }
        }
    }
    if(menu) {
        menu->exec(QCursor::pos());
        d->items_map.clear();
        delete menu;
    }
}

void QuSvgItemEventHandler::actionTriggered() {
    qDebug() << __PRETTY_FUNCTION__ << "action providers" << d->action_providers.size();
    QAction *a = qobject_cast<QAction *>(sender());
    foreach(QuSvgActionProviderInterface *ai, d->action_providers) {
        ai->onContextAction(d->items_map.value(a), a->text());
        if(ai->hasError())
            emit error(ai->name(), ai->message());
    }
}

