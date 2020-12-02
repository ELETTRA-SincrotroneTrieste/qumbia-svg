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
#include <quaction-extension-plugininterface.h>
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
    QString src;
    CuContext *ctx;
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
    d->ctx = nullptr;
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
        if(!e.a("helper").isEmpty()) {
            return m_start_helper_from_cmd(e.a("helper"));
        }
        else {
            // d->src and d->ctx should have been provided by a previous
            // call to m_get_src_and_ctx from QuSvgHelperAppActionProvider::handlesEventType
            if(d->src.isEmpty() || !d->ctx) // try to get now
                m_get_src_and_ctx(e);
            if(d->errmsg.isEmpty())
                return m_start_helper_from_reader();
            return false; // some error happened within m_get_src_and_ctx
        }
    }
    d->errmsg = QString("QuSvgHelperAppActionProvider::onContextAction no element found with id \"%1\"")
            .arg(it->elementId());
    return false;
}

bool QuSvgHelperAppActionProvider::handlesEventType(QuGraphicsSvgItem *it, QuSvgActionProviderInterface::EventType et) const {
    d->errmsg.clear();
    const QuDomElement root(d->qudom);
    // find the target
    QuDomElement e(root[it->elementId()]);
    bool ok1 = !e.isNull() && !e.a("helper").isEmpty() && et == QuSvgActionProviderInterface::ContextualEvent;
    return ok1 || m_get_src_and_ctx(e);
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

bool QuSvgHelperAppActionProvider::m_start_helper_from_reader() {
    d->errmsg.clear();
    if(!d->src.isEmpty() && d->ctx) { // tango source
        CuPluginLoader pl;
        const char* wstd_menu_a_ctions_plugin_name = "widgets-std-context-menu-actions.so";
        QObject *plugin_o;
        CuContextMenuActionsPlugin_I *wstd_menu_a_ctions_plugin = nullptr;
        wstd_menu_a_ctions_plugin =
                pl.get<CuContextMenuActionsPlugin_I>(wstd_menu_a_ctions_plugin_name, &plugin_o);
        if(wstd_menu_a_ctions_plugin && d->ctx) {
            wstd_menu_a_ctions_plugin->setup(nullptr, d->ctx);
            if(!QMetaObject::invokeMethod(plugin_o,  "onHelperAActionTriggered", Q_ARG(QString, d->src)))
                d->errmsg = QString("QuSvgHelperAppActionProvider.m_start_helper_from_reader: failed to invoke onHelperAActionTriggered method on %1")
                        .arg(plugin_o->objectName());
        }
    }
    return d->errmsg.isEmpty();
}

bool QuSvgHelperAppActionProvider::m_start_helper_from_cmd(const QString &cmd)
{
    d->errmsg.clear();
    const char* extensions_plugin_name = "libactions-extension-plugin.so";
    QObject *plugin_o;
    CuPluginLoader pl;
    QuActionExtensionPluginInterface* aepi = pl.get<QuActionExtensionPluginInterface>(extensions_plugin_name, &plugin_o);
    if(aepi) {
        QuActionExtensionFactoryI *ae_fac = aepi->getExtensionFactory();
        QuActionExtensionI *ale = ae_fac->create("CuApplicationLauncherExtension", nullptr);
        CuData cmdline("command", cmd.toStdString());
        ale->execute(cmdline, d->ctx);
    }
    else
        d->errmsg = QString("QuSvgHelperAppActionProvider::m_start_helper_from_cmd: "
                            "failed to load plugin \"%1\"").arg(extensions_plugin_name);
    return d->errmsg.isEmpty();
}

bool QuSvgHelperAppActionProvider::m_get_src_and_ctx(const QuDomElement &itemel) const
{
    d->errmsg.clear();
    QDomElement link_e = itemel.firstChild("read");
    if(!link_e.isNull()) {
        QString input_src, lnk_id = link_e.parentNode().toElement().attribute("id");
        QuSvgReader *reader = d->conn_pool->findReaderById(lnk_id);
        if(reader) {
            d->src = reader->source();
            d->ctx = reader->getContext();
        }
        else if(d->src.isEmpty() && d->replace_wildcards_i) {
            // try with replace wildcard helper, if available
            d->src = link_e.attribute("src");
            if(!d->src.isEmpty()) {
                input_src = d->src;
                d->src = d->replace_wildcards_i->replaceWildcards(QString(input_src));
                if(!d->src.isEmpty())
                    d->ctx = d->replace_wildcards_i->getContext();
                else
                    d->errmsg = QString("QuSvgHelperAppActionProvider.m_get_src_and_ctx: "
                                        "QuSvgReplaceWildcardHelperInterface.replaceWildcards "
                                        "returned an empty string for input \"%1\"").arg(input_src);
            }
            else
                d->errmsg = QString("QuSvgHelperAppActionProvider.m_get_src_and_ctx: neither \"src\" "
                                    "nor \"target\" attributes found for %1 under %2 id %3").
                        arg(link_e.tagName()).arg(itemel.element().tagName())
                        .arg(itemel.element().attribute("id"));
        }
        if(d->src.isEmpty() || !d->ctx) {
            d->errmsg = QString("QuSvgHelperAppActionProvider.m_get_src_and_ctx: no reader found for "
                                "<read> with id \"%1\". For writers, a QuSvgReplaceWildcardHelperInterface "
                                "implementation is required to process wildcards in targets").arg(lnk_id);
        }
    } // !link_e.isNull()
    else
        d->errmsg = QString("QuSvgHelperAppActionProvider::m_get_src_and_ctx: no \"read\" "
                            "element found under %1 id %2").arg(itemel.element().tagName()).arg(itemel.a("id"));
    return d->errmsg.isEmpty();
}

