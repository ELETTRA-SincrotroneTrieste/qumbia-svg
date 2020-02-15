#include "qusvgreader.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QContextMenuEvent>
#include <QMetaProperty>
#include <QtDebug>

#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <culinkstats.h>
#include <cucontext.h>

#include "qusvglink.h"
#include "qusvgdatalistener.h" // for QuSvgResultData


/** @private */
class QuSvgReaderPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    CuContext *context;
    QSet<QuSvgLink>links;
    QSet<QuSvgReaderListener *>listeners;
    QString raw_src;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuSvgReader::QuSvgReader(Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuSvgReader::QuSvgReader(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

void QuSvgReader::m_init()
{
    d = new QuSvgReaderPrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->read_ok = false;
}

QuSvgReader::~QuSvgReader()
{
    pdelete("~QuSvgReader %p", this);
    delete d->context;
    delete d;
}

QString QuSvgReader::source() const
{
    if(CuControlsReaderA* r = d->context->getReader())
        return r->source();
    return "";
}

/** \brief returns the pointer to the CuContext
 *
 * CuContext sets up the connection and is used as a mediator to send and get data
 * to and from the reader.
 *
 * @see CuContext
 */
CuContext *QuSvgReader::getContext() const
{
    return d->context;
}

/** \brief Connect the reader to the specified source.
 *
 * If a reader with a different source is configured, it is deleted.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * @see QuContext::setOptions
 * @see source
 */
void QuSvgReader::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r) {
        d->raw_src = s;
        r->setSource(s);
    }
}

void QuSvgReader::unsetSource() {
    d->context->disposeReader();
    d->raw_src.clear();
}

int QuSvgReader::addLink(const QuSvgLink &link) {
    d->links.insert(link);
    return d->links.size();
}

int QuSvgReader::removeLink(const QuSvgLink &link)
{
    d->links.remove(link);
    return d->links.size();
}

void QuSvgReader::addListener(QuSvgReaderListener *dl) {
    d->listeners.insert(dl);
}

void QuSvgReader::removeListener(QuSvgReaderListener *dl) {
    d->listeners.remove(dl);
}

void QuSvgReader::pause() {
    printf("QuSvtReader.pause: pausing reader %s\n", qstoc(d->context->getReader()->source()));
    d->context->disposeReader();
}

void QuSvgReader::resume() {
    printf("QuSvtReader.resume: resuming reader %s\n", qstoc(d->raw_src));
    CuControlsReaderA * r = d->context->replace_reader(d->raw_src.toStdString(), this);
    if(r)
        r->setSource(d->raw_src);
}

QString QuSvgReader::rawSrc() const {
    return d->raw_src;
}

void QuSvgReader::m_configure(const CuData& da) {
    printf("\e[1;33m [CONF]\e[0m: QuSvgReader::m_configure: %s\n", da.toString().c_str());
}

void QuSvgReader::onUpdate(const CuData &da) {
    d->read_ok = !da["err"].toBool();
    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da["msg"].toString());

    // configure object if the type of received data is "property"
    if(d->read_ok && d->auto_configure && da["type"].toString() == "property") {
        m_configure(da);
    }
    foreach(const QuSvgLink &link, d->links) {
        foreach(QuSvgReaderListener* l, d->listeners)
            l->onUpdate(QuSvgResultData(da, link));
    }
}

/*!
 * \brief Set options on the reader
 * \param options a map of option-names - option-values to set on the CuContext
 *
 * The method tries to convert the values into bool (if value is either "true" or "false"),
 * integer (if no '.' or 'e' characters are found) or double. If conversion to numbers
 * fails, the option is set as std::string
 */
void QuSvgReader::setOptions(const QMap<QString, QString> &options) {
    if(options.size() > 0) {
        CuData o;
        foreach(QString key, options.keys()) {
            const std::string& k = key.toStdString();
            const QString &v = options.value(key);
            if(v.compare("true", Qt::CaseInsensitive) == 0)
                o[k] = true;
            else if(v.compare("false", Qt::CaseInsensitive) == 0)
                o[k] = false;
            else {
                int i; double d;
                bool ok = false;
                if(!v.contains('.') && !v.contains('e')) {
                    i = v.toInt(&ok);
                    if(ok) o[k] = i;
                }
                else {
                    d = v.toDouble(&ok);
                    if(ok) o[k] = d;
                }
                if(!ok)
                    o[k] = v.toStdString();
            }
        }
        d->context->setOptions(o);
        printf("QuSvgReader.\e[1;32msetOptions: %s\e[0m\n", o.toString().c_str());
    }
}
