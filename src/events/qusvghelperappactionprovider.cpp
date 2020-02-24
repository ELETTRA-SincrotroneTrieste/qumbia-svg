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
        CuContext *ctx = nullptr;
        if(!e.a("helper").isEmpty()) {
            src = e.a("helper");
        }
        else {
            // look for link element under e
            QDomElement link_e = e.firstChild("read");
            if(link_e.isNull()) // try "write"
                link_e = e.firstChild("write");
            if(!link_e.isNull()) {
                QString lnk_id = link_e.parentNode().toElement().attribute("id");
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
            } // !link_e.isNull()
            if(src.isEmpty() || !ctx) {
                d->errmsg = QString("QuSvgHelperAppActionProvider.onContextAction: no reader found for "
                                    "<read> with id \"%1\". For writers, a QuSvgReplaceWildcardHelperInterface "
                                    "implementation is required to process wildcards in targets")
                        .arg(lnk_id);
            } // no "helper" attribute defined
        }
        if(!src.isEmpty()) { // tango source
            CuPluginLoader pl;
            const char* wstd_menu_a_ctions_plugin_name = "widgets-std-context-menu-actions.so";
            QObject *plugin_o;
            CuContextMenuActionsPlugin_I *wstd_menu_a_ctions_plugin = nullptr;
            wstd_menu_a_ctions_plugin =
                    pl.get<CuContextMenuActionsPlugin_I>(wstd_menu_a_ctions_plugin_name, &plugin_o);
            if(wstd_menu_a_ctions_plugin && ctx) {
                wstd_menu_a_ctions_plugin->setup(nullptr, ctx);
                bool success = QMetaObject::invokeMethod(plugin_o,
                                                         "onHelperAActionTriggered",
                                                         Q_ARG(QString, src));
                if(!success)
                    d->errmsg = QString("QuSvgHelperAppActionProvider.onContextAction: failed to invoke onHelperAActionTriggered method on %1")
                            .arg(plugin_o->objectName());
            }
            else {
//                const char* extensions_plugin_name = "libactions-extension-plugin.so";
//                pl.get<
//                if(!pluginFilePath.isEmpty()) {
//                    QPluginLoader pluginLoader(pluginFilePath);
//                    QObject *plugin = pluginLoader.instance();
//                    if(plugin) {
//                        QuActionExtensionPluginInterface *action_ex = qobject_cast<QuActionExtensionPluginInterface *>(plugin);
//                        if(action_ex) {
//                            QuActionExtensionFactoryI *ae_fac = action_ex->getExtensionFactory();
//                            QuActionExtensionI *ale = ae_fac->create("CuApplicationLauncherExtension", nullptr);
//                            CuData cmdline("command", "xclock -analog -twentyfour");
//                            ale->execute(cmdline);
//                        }
//                    }
//                }
                }
                if(app_launcher) {
                    std::string dev = prop.substr(0, prop.find(":helperApplication"));
                    if(dev.length()  > 0)
                        app += std::string(" ") + dev;
                    CuData in("command", app);
                    app_launcher->execute(in);
                }
                else
                    perr("WidgetStdContextMenuActions::onDataReady: no CuApplicationLauncherExtension found in "
                         "extension factory");
            }
        } // !src.isEmpty
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
