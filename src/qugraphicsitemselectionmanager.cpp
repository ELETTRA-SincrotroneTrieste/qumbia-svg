#include "qugraphicsitemselectionmanager.h"
#include "qusvgview.h"
#include <QGraphicsView>
#include <qugraphicsitem.h>
#include <QtDebug>

class QuGraphicsItemSelectionManager_P {
public:
    QuGraphicsItem * current_sel; // current selection
};

QuGraphicsItemSelectionManager::QuGraphicsItemSelectionManager(QGraphicsView *gview)
    : QObject{gview}
{
    d = new QuGraphicsItemSelectionManager_P();
    connect(gview, SIGNAL(clicked(QList<QGraphicsItem*>)),
            this, SLOT(on_clicked(QList<QGraphicsItem*>)));

}

QuGraphicsItemSelectionManager::~QuGraphicsItemSelectionManager() {
    delete d;
}

QuGraphicsItem *QuGraphicsItemSelectionManager::selectedItem() const {
    return d->current_sel;
}

void QuGraphicsItemSelectionManager::deselectAll() {
    foreach(QuGraphicsItem *svgi, qobject_cast<QuSvgView *>(parent())->qusvgitems())
        if(svgi->isSelected())
            svgi->setSelected(false);
}

void QuGraphicsItemSelectionManager::on_clicked(const QList<QGraphicsItem *>& items) {
    QuGraphicsItem *quitem = nullptr;
    foreach(QGraphicsItem *it, items) {
        // only the first item (topmost item in Z direction) is processed.
        // a `break' exits the for loop after first iteration
        if(it->type() == QuGraphicsItem::QuGraphicsItemType) {
            quitem = static_cast<QuGraphicsItem *>(it);
            break;
        }
    }
    if(quitem != d->current_sel) {
        qDebug() << __PRETTY_FUNCTION__ << quitem << "old" << d->current_sel;
        emit selectionChanged(quitem, d->current_sel);
        d->current_sel = quitem;
    }
}

