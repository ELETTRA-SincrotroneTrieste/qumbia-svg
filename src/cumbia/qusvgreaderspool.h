#ifndef QUSVGREADERSPOOL_H
#define QUSVGREADERSPOOL_H

#include <QObject>
#include "qusvglink.h"
#include <QList>
#include <qusvgreader.h>

class CumbiaPool;
class CuControlsFactoryPool;
class Cumbia;
class CuData;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class QDomDocument;
class QuSvgDataListener;

class QuSvgReadersPoolPrivate;

class QuSvgReadersPool : public QObject
{
    Q_OBJECT
public:
    QuSvgReadersPool(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, QObject *parent);

    QuSvgReadersPool(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, QObject *parent = nullptr);

    void setup_links(const QList<QuSvgLink>& links, QuSvgReaderListener *l);

    void add_link(const QuSvgLink &link, QuSvgReaderListener *l);

public slots:
    void activateSources(const QStringList& srcs, bool activate);

signals:

private:
    QuSvgReadersPoolPrivate *d;
};

#endif // QUSVGREADERSPOOL_H
