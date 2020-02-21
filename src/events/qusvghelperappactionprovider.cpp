#include "qusvghelperappactionprovider.h"
#include "qusvgconnectionspool.h"
#include "qusvgreplacewildcardhelperinterface.h"
#include <qudom.h>
#include <qudomelement.h>
#include <qugraphicssvgitem.h>
#include <cupluginloader.h>
#include <quaction-extension-plugininterface.h>
#include <QPluginLoader>
#include <cucontext.h>
#include <cumbia.h>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>
#include <cucontrolsfactories_i.h>
#include <cucontextmenuactionsplugin_i.h>
#include <QtDebug>

class QuSvgHelperAppActionProviderPrivate {
public:
    QuSvgHelperAppActionProviderPrivate(QuDom* dom)
        : qudom(dom) {
    }
    QuSvgConnectionsPool *conn_pool;
    QuSvgReplaceWildcardHelperInterface *replace_wildcards_i;
    QuDom* qudom;
    QString errmsg;
};

QuSvgHelperAppActionProvider::QuSvgHelperAppActionProvider(QObject *parent,
                                                           QuDom *dom,
                                                           QuSvgConnectionsPool *c_pool,
                                                           QuSvgReplaceWildcardHelperInterface *rwci)
    : QObject(parent)
{
    d = new QuSvgHelperAppActionProviderPrivate(dom);
    d->conn_pool = c_pool;
    d->replace_wildcards_i = rwci;
}

QuSvgHelperAppActionProvider::~QuSvgHelperAppActionProvider()
{
    delete d;
}

QStringList QuSvgHelperAppActionProvider::getActionNames(QuGraphicsSvgItem *it) const {
    Q_UNUSED(it)
    return QStringList() << "Helper Application";
}

bool QuSvgHelperAppActionProvider::onClicked(QuGraphicsSvgItem *it) {
    Q_UNUSED(it)
    d->errmsg.clear();
    return false;
}

bool QuSvgHelperAppActionProvider::onContextAction(QuGraphicsSvgItem *it, const QString &action_name) {
    qDebug() << __PRETTY_FUNCTION__ << it->elementId() << action_name   ;
    d->errmsg.clear();
    if(action_name != "Helper Application")
        return false;
    const QuDomElement root(d->qudom);
    // find the target
    QuDomElement e(root[it->elementId()]);
    if(!e.isNull()) {
        QString src;
        // look for link element under e
        QDomElement link_e = e.firstChild("read");
        if(link_e.isNull()) // try "write"
            link_e = e.firstChild("write");
        if(!link_e.isNull()) {
            QString lnk_id = link_e.parentNode().toElement().attribute("id");
            CuContext *ctx = nullptr;
            QuSvgReader *reader = d->conn_pool->findReaderById(lnk_id);
            if(reader) {
                src = reader->source();
                ctx = reader->getContext();
            }

            if(src.isEmpty() && d->replace_wildcards_i) {
                src = link_e.attribute("src");
                if(src.isEmpty())
                    src = link_e.attribute("target");
                if(!src.isEmpty()) {
                    const QString input_src(src);
                    src = d->replace_wildcards_i->replaceWildcards(input_src);
                    if(!src.isEmpty())
                        ctx = d->replace_wildcards_i->getContext();
                    else
                        d->errmsg = QString("QuSvgHelperAppActionProvider.onContextAction: "
                             "QuSvgReplaceWildcardHelperInterface.replaceWildcards "
                             "returned an empty string for input \"%1\"").arg(input_src);
                }
                else
                    d->errmsg = QString("QuSvgHelperAppActionProvider.onContextAction: neither \"src\" "
                         "nor \"target\" attributes found for %1 under %2 id %3").
                         arg(link_e.tagName()).arg(e.element().tagName())
                                        .arg(e.element().attribute("id"));
            }
            if(src.isEmpty() || !ctx) {
                d->errmsg = QString("QuSvgHelperAppActionProvider.onContextAction: no reader found for "
                     "<read> with id \"%1\". For writers, a QuSvgReplaceWildcardHelperInterface "
                     "implementation is required to process wildcards in targets")
                        .arg(lnk_id);
            }
            else { // tango source
                CuPluginLoader pl;
                const char* wstd_menu_a_ctions_plugin_name = "widgets-std-context-menu-actions.so";
                QObject *plugin_o;
                CuContextMenuActionsPlugin_I *wstd_menu_a_ctions_plugin = nullptr;
                wstd_menu_a_ctions_plugin =
                        pl.get<CuContextMenuActionsPlugin_I>(wstd_menu_a_ctions_plugin_name, &plugin_o);
                if(wstd_menu_a_ctions_plugin) {
                    wstd_menu_a_ctions_plugin->setup(nullptr, ctx);
                    bool success = QMetaObject::invokeMethod(plugin_o,
                                                             "onHelperAActionTriggered",
                                                             Q_ARG(QString, src));
                    if(!success)
                        d->errmsg = QString("QuSvgHelperAppActionProvider.onContextAction: failed to invoke onHelperAActionTriggered method on %1")
                             .arg(plugin_o->objectName());
                }
            } // !src.isEmpty
        } // !link_e.isNull
    }
    return false;
}

bool QuSvgHelperAppActionProvider::handlesEventType(QuGraphicsSvgItem *, QuSvgActionProviderInterface::EventType et) const {
    return et == QuSvgActionProviderInterface::ContextualEvent;
}

QString QuSvgHelperAppActionProvider::message() const {
    return d->errmsg;
}

bool QuSvgHelperAppActionProvider::hasError() const {
    return !d->errmsg.isEmpty();
}

QString QuSvgHelperAppActionProvider::name() const {
    return this->metaObject()->className();
}
