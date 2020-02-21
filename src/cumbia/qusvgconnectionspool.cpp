#include "qusvgconnectionspool.h"
#include "qusvgreader.h"
#include "qusvgdatalistener.h"
#include <QMap>
#include <QSet>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <cudata.h>
#include <cucontrolsutils.h>
#include <quwriter.h>

class QuSvgConnectionsPoolPrivate {
public:
    QuSvgConnectionsPoolPrivate()
        : cumbia(nullptr),
          cu_pool(nullptr),
          r_fac(nullptr) {
    }
    QMap<QString, QuSvgReader *> r_map;

    Cumbia *cumbia;
    CumbiaPool *cu_pool;
    CuControlsReaderFactoryI *r_fac;
    CuControlsWriterFactoryI *w_fac;
    CuControlsFactoryPool f_pool;
};

QuSvgConnectionsPool::QuSvgConnectionsPool(QObject *parent)
    : QObject(parent)
{
    d = new QuSvgConnectionsPoolPrivate;
    d->cumbia = nullptr;
    d->r_fac = nullptr;
    d->w_fac = nullptr;
    d->cu_pool = nullptr;
}

void QuSvgConnectionsPool::init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, const CuControlsWriterFactoryI &w_fac) {
    d->cumbia = cumbia;
    d->r_fac = r_fac.clone();
    d->w_fac = w_fac.clone();
}

void QuSvgConnectionsPool::init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) {
    d->cu_pool = cumbia_pool;
    d->f_pool = fpool;
}

void QuSvgConnectionsPool::setup_links(const QList<QuSvgReadLink> &links, QuSvgConnectionListener *lis) {
    foreach (QuSvgReadLink l, links) {
        add_link(l, lis);
    }
}

void QuSvgConnectionsPool::add_link(const QuSvgReadLink &link, QuSvgConnectionListener *lis) {
    QuSvgReader *reader = nullptr;
    if(!d->r_map.contains(link.src)) {
        if(d->cumbia)
            reader = new QuSvgReader(d->cumbia, *d->r_fac);
        else if(d->cu_pool)
            reader = new QuSvgReader(d->cu_pool, d->f_pool);
        if(reader) {
            d->r_map.insert(link.src, reader);
            reader->addListener(lis);
            if(link.options.size() > 0)
                reader->setOptions(link.options);
            reader->setSource(link.src);
        }
    }
    else
        reader = d->r_map.value(link.src);
    if(reader) {
        reader->addLink(link);
    }

}

QuSvgReader *QuSvgConnectionsPool::findReaderById(const QString &id) const
{
    foreach(QuSvgReader *r, d->r_map.values()) {
        const QVector<QuSvgReadLink> &lnks = r->links();
        foreach(const QuSvgReadLink &l, lnks) {
            if(id == l.id)
                return r;
        }
    }
    return nullptr;
}

void QuSvgConnectionsPool::activateSources(const QStringList &ids, bool activate) {
    foreach(QuSvgReader *r, d->r_map.values()) {
        const QVector<QuSvgReadLink> &lnks = r->links();
        foreach(const QuSvgReadLink &l, lnks) {
            if(ids.contains(l.id))
                activate ? r->reactivate(l.id) : r->deactivate(l.id);
        }
    }
}

