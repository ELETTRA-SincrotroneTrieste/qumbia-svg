#include "qusvgwriteactionprovider.h"
#include "qugraphicssvgitem.h"
#include "qusvgview.h"
#include "qudom.h"
#include "qudomelement.h"
#include <quapplynumeric.h>
#include <qubutton.h>
#include <quinputoutput.h>
#include <qulineedit.h>
#include <quwriter.h>
#include <QPointF>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QHBoxLayout>
#include <cumacros.h>
#include <QtDebug>

class QuSvgActionProviderPrivate {
public:
    QuSvgActionProviderPrivate(QuDom* _dom) : dom(_dom) {

    }
    CumbiaPool *cu_pool;
    Cumbia *cumbia;
    CuControlsFactoryPool fpoo;
    CuControlsReaderFactoryI *rf;
    CuControlsWriterFactoryI *wf;
    QuDom* dom;
    QString msg;
};

QuSvgWriteActionProvider::QuSvgWriteActionProvider(QObject *parent,
                                                   QuDom *dom,
                                                   CumbiaPool *cu_pool,
                                                   const CuControlsFactoryPool &f_pool)
    : QObject(parent)
{
    d = new QuSvgActionProviderPrivate(dom);
    d->cu_pool = cu_pool;
    d->fpoo = f_pool;
    d->rf = nullptr;
    d->wf = nullptr;
}

QuSvgWriteActionProvider::QuSvgWriteActionProvider(QObject *parent,
                                                   QuDom *dom,
                                                   Cumbia *cu,
                                                   const CuControlsReaderFactoryI &rf,
                                                   const CuControlsWriterFactoryI &wf)
    : QObject(parent)
{
    d = new QuSvgActionProviderPrivate(dom);
    d->cumbia = cu;
    d->rf = rf.clone();
    d->wf = wf.clone();
    d->cu_pool = nullptr;
}

QuSvgWriteActionProvider::~QuSvgWriteActionProvider()
{
    printf("QuSvgActionProvider %p\n", this);
    if(d->rf)
        delete d->rf; // clone
    if(d->wf)
        delete d->wf; // clone
    delete d;
}

void QuSvgWriteActionProvider::exec_dialog(const QString& type, const QString& target) {
    QDialog *dlg = m_makeDialog(nullptr, type, target);
    if(dlg) {
        dlg->exec();
        delete dlg;
    }
}

QDialog *QuSvgWriteActionProvider::m_makeDialog(QWidget *parent,
                                                const QString &type,
                                                const QString& target)
{
    QDialog *dlg = new QDialog(parent);
    dlg->setWindowTitle(target);
    QHBoxLayout *hlo = new QHBoxLayout(dlg);
    if(type == "quapplynumeric") {
        QuApplyNumeric *w;
        d->cumbia != nullptr ? w = new QuApplyNumeric(parent, d->cumbia, *(d->wf))
                :  w = new QuApplyNumeric(parent, d->cu_pool, d->fpoo);
        hlo->addWidget(w);
        w->setTarget(target);
    }
    else if(type == "qulineedit") {
        QuButton *b;
        QuLineEdit *le;
        d->cumbia != nullptr ? b = new QuButton(parent,  d->cumbia, *(d->wf))
                : b = new QuButton(parent, d->cu_pool, d->fpoo);
        d->cumbia != nullptr ? le = new QuLineEdit(parent,  d->cumbia, *(d->wf))
                : le = new QuLineEdit(parent, d->cu_pool, d->fpoo);
        b->setText("APPLY");
        b->setTarget(QString("%1(%2)").arg(target).arg(le->text()));
        hlo->addWidget(le);
        hlo->addWidget(b);
    }
    else if(type == "quinputoutput") {
        QuInputOutput* w;
        d->cumbia != nullptr ?  w = new QuInputOutput(parent,  d->cumbia, *(d->rf), *(d->wf))
                : w = new QuInputOutput(parent,  d->cu_pool, d->fpoo);
        w->setSource(target);
        hlo->addWidget(w);
    }
    else if(type == "qubutton") {
        QuButton* w;
        d->cumbia != nullptr ? w = new QuButton(parent,  d->cumbia, *(d->wf))
                : w = new QuButton(parent,  d->cu_pool, d->fpoo);
        w->setTarget(target);
        hlo->addWidget(w);
    }
    return dlg;
}

QStringList QuSvgWriteActionProvider::getActionNames(QuGraphicsSvgItem *it) const {
    QStringList al;
    QString target, type;
    m_pre_process(it, target, type);
    if(!target.isEmpty())
        al << target;
    return al;
}

bool QuSvgWriteActionProvider::onClicked(QuGraphicsSvgItem *it) {
    QString target, type;
    QuWriter *w = nullptr;
    m_pre_process(it, target, type);
    if(type == "button") {
        if(d->cu_pool != nullptr)
            w = new QuWriter(nullptr, d->cu_pool, d->fpoo);
        else if(d->cumbia != nullptr)
            w = new QuWriter(nullptr, d->cumbia, *(d->wf));
        if(w) {
            printf("QuSvgItemEventHandler::processItemClicked executing %s directly!\n",
                   qstoc(target));
            w->setAutoDestroy(true);
            w->setTarget(target);
            w->execute();
        }
    }
    else if(!type.isEmpty())
        exec_dialog(target, type);
    return !target.isEmpty() && !type.isEmpty();
}

bool QuSvgWriteActionProvider::handlesEventType(QuGraphicsSvgItem *it, QuSvgActionProviderInterface::EventType et) const {
    QString target, type;
    m_pre_process(it, target, type);
    if(et == QuSvgActionProviderInterface::ClickEvent && type == "button" && !target.isEmpty())
        return true;
    if(et == QuSvgActionProviderInterface::ContextualEvent && !target.isEmpty())
        return true;
    return false;
}

bool QuSvgWriteActionProvider::hasError() const {
    return !d->msg.isEmpty();
}

QString QuSvgWriteActionProvider::message() const {
    return d->msg;
}

bool QuSvgWriteActionProvider::onContextAction(QuGraphicsSvgItem *it, const QString &action_name) {
    QString target, type;
    m_pre_process(it, target, type);
    if(target == action_name) {
        exec_dialog(type, target);
    }
    return !type.isEmpty() && target == action_name;
}

void QuSvgWriteActionProvider::m_pre_process(QuGraphicsSvgItem *item,
                                             QString& target,
                                             QString& type) const {
    QuDomElement root(d->dom);
    QuDomElement e(root[item->elementId()]);
    if(!e.isNull()) {
        // look for link element under e
        QDomElement link_e = e.firstChild("write");
        target = link_e.attribute("target");
        if(!target.isEmpty() && !e.a("type").isEmpty())
            type = e.a("type").toLower();
        else if(!target.isEmpty())
            type = "button";
    }
}

QString QuSvgWriteActionProvider::name() const {
    return this->metaObject()->className();
}
