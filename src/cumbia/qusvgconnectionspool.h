#ifndef QUSVGCONNECTIONSPOOL_H
#define QUSVGCONNECTIONSPOOL_H

#include <QObject>
#include "qusvgreadlink.h"
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

class QuSvgConnectionsPoolPrivate;

class QuSvgConnectionsPool : public QObject
{
    Q_OBJECT
public:
    QuSvgConnectionsPool(QObject *parent = nullptr);

    void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, const CuControlsWriterFactoryI &w_fac);
    void init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    void setup_links(const QList<QuSvgReadLink>& links, QuSvgConnectionListener *l);

    void add_link(const QuSvgReadLink &link, QuSvgConnectionListener *l);

    QuSvgReader *findReaderById(const QString& id) const;

public slots:
    void activateSources(const QStringList& ids, bool activate);

signals:

private:
    QuSvgConnectionsPoolPrivate *d;
};

#endif // QUSVGCONNECTIONSPOOL_H
