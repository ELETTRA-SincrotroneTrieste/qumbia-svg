#ifndef QuSvgReader_H
#define QuSvgReader_H

#include <QObject>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>
#include <QMap>
#include <QString>
class QContextMenuEvent;

class QuSvgReaderPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class CuContext;
class CuLinkStats;
class QuSvgLink;
class QuSvgResultData;

class QuSvgReaderListener {
public:
    virtual void onUpdate(const QuSvgResultData& rd) = 0;
};

/*! \brief QuSvgReader is a reader used by the cumbia-svg module
 *
 * Connection is initiated with setSource. When new data is ready, it can be manipulated before changing properties in the DOM.
 * 
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
 *
*/
class QuSvgReader : public CuDataListener, public CuContextI
{
public:
    QuSvgReader(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuSvgReader(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuSvgReader();

    /** \brief returns the source of the reader
     *
     * @return a QString with the name of the source
     */
    QString source() const;

    /** \brief returns a pointer to the CuContext used as a delegate for the connection.
     *
     * @return CuContext
     */
    CuContext *getContext() const;

    /** \brief set the source and start reading
     *
     * @param the name of the source
     */
    void setSource(const QString& s);

    /** \brief disconnect the source
     *
     * remove the source and stop reading
     */
    void unsetSource();

    int addLink(const QuSvgLink& link);

    int removeLink(const QuSvgLink& link);

    void addListener(QuSvgReaderListener *dl);
    void removeListener(QuSvgReaderListener *dl);

    void setOptions(const QMap<QString, QString> &options);

    void pause();

    void resume();

    QString rawSrc() const;

protected:

private:
    QuSvgReaderPrivate *d;

    void m_init();

    void m_configure(const CuData& d);


    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTLABEL_H
