#ifndef QUSVGACTIONPROVIDER_H
#define QUSVGACTIONPROVIDER_H

class QuGraphicsSvgItem;
class QPointF;
class QAction;
class CumbiaPool;
class CuControlsFactoryPool;
class Cumbia;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class QDialog;
class QWidget;

#include <QList>
#include <QObject>
#include "qusvgitemeventhandler.h"

class QuSvgActionProviderPrivate;
class QuSvgView;
class QuDom;

class QuSvgWriteActionProvider : public QObject, public QuSvgActionProviderInterface
{
    Q_OBJECT
public:
    QuSvgWriteActionProvider(QObject *parent,
                      QuDom *dom,
                      CumbiaPool *cu_pool,
                      const CuControlsFactoryPool &f_pool);
    QuSvgWriteActionProvider(QObject *parent,
                      QuDom *dom,
                      Cumbia *cu,
                      const CuControlsReaderFactoryI &rf,
                      const CuControlsWriterFactoryI& wf);
    ~QuSvgWriteActionProvider();

private slots:
    void exec_dialog(const QString &type, const QString &target, const QPointF& pos);


private:
    QuSvgActionProviderPrivate *d;

    QDialog *m_makeDialog(QWidget* parent, const QString& type,
                          const QString &target);

    void m_pre_process(QuGraphicsSvgItem *item, QString &point, QString &type) const ;

    // QuSvgActionProviderInterface interface
public:
    QStringList getActionNames(QuGraphicsSvgItem *it) const;
    bool onContextAction(QuGraphicsSvgItem *it, const QString &action_name);
    bool onClicked(QuGraphicsSvgItem *it);
    bool handlesEventType(QuGraphicsSvgItem *it, EventType et) const;
    bool hasError() const;
    QString message() const;
    QString name() const;
};

#endif // QuSvgItemEventHandler_H
