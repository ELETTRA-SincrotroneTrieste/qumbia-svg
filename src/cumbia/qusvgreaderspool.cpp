#include "qusvgreaderspool.h"
#include "qusvgreader.h"
#include "qusvgdatalistener.h"
#include <QMap>
#include <QSet>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <cudata.h>

class QuSvgReadersPoolPrivate {
public:
    QuSvgReadersPoolPrivate()
        : cumbia(nullptr),
          cu_pool(nullptr),
          r_fac(nullptr) {
    }
    QMap<QString, QuSvgReader *> r_map;

    Cumbia *cumbia;
    CumbiaPool *cu_pool;
    CuControlsReaderFactoryI *r_fac;
    CuControlsFactoryPool f_pool;
};

QuSvgReadersPool::QuSvgReadersPool(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, QObject *parent)
{
    d = new QuSvgReadersPoolPrivate;
    d->cumbia = cumbia;
    d->r_fac = r_fac.clone();
}

QuSvgReadersPool::QuSvgReadersPool(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, QObject *parent)
{
    d = new QuSvgReadersPoolPrivate;
    d->cu_pool = cumbia_pool;
    d->f_pool = fpool;
}

void QuSvgReadersPool::setup_links(const QList<QuSvgLink> &links, QuSvgReaderListener *lis) {
    foreach (QuSvgLink l, links) {
        add_link(l, lis);
    }
}

void QuSvgReadersPool::add_link(const QuSvgLink &link, QuSvgReaderListener *lis)
{
    QuSvgReader *reader = nullptr;
    if(!d->r_map.contains(link.src)) {
        if(d->cumbia)
            reader = new QuSvgReader(d->cumbia, *d->r_fac);
        else if(d->cu_pool)
            reader = new QuSvgReader(d->cu_pool, d->f_pool);
        if(reader) {
            d->r_map.insert(link.src, reader);
            reader->addListener(lis);
            reader->setSource(link.src);
        }
    }
    else
        reader = d->r_map.value(link.src);
    if(reader) {
        reader->addLink(link);
    }
}
