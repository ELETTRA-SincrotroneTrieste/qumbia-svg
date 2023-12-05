#ifndef QUGRAPHICSITEMSELECTIONMANAGER_H
#define QUGRAPHICSITEMSELECTIONMANAGER_H

#include <QObject>
#include <QList>

class QGraphicsView;
class QGraphicsItem;
class QuGraphicsItem;

class QuGraphicsItemSelectionManager_P;

class QuGraphicsItemSelectionManager : public QObject
{
    Q_OBJECT
public:
    explicit QuGraphicsItemSelectionManager(QGraphicsView *view );
    virtual ~QuGraphicsItemSelectionManager();

    QuGraphicsItem* selectedItem() const;

public slots:
    void deselectAll();

signals:
    void selectionChanged(QuGraphicsItem* newi, QuGraphicsItem *oldi);

private slots:
    void on_clicked(const QList<QGraphicsItem *> &);

private:
    QuGraphicsItemSelectionManager_P *d;
};

#endif // QUGRAPHICSITEMSELECTIONMANAGER_H
