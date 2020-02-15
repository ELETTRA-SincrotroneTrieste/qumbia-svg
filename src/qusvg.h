#ifndef QUMBIASVG_H
#define QUMBIASVG_H

#include <qudomelement.h>
#include <qudom.h>
#include <qusvgreader.h>

class QumbiaSVGPrivate;
class CumbiaPool;
class CuControlsFactoryPool;
class Cumbia;
class CuControlsReaderFactoryI;
class CuControlsWriterFactoryI;
class QuSvgDataListener;
class QuSvgReadersPool;

/*! \mainpage
 *
 * \section qusvg_title cumbia Qt Svg integration module
 * The qumbia-svg module connects Qt Svg objects to cumbia.
 *
 * A SVG file as well as a document as string can be loaded. Elements containing the
 * *item* attribute set to anything different than "false" are mapped to QGraphicsSvgItem objects that populate
 * a QGraphicsScene. Child elements of the *item* are rendered by the same
 * <a href="https://doc.qt.io/qt-5/qgraphicssvgitem.html">QGraphicsSvgItem</a>.
 *
 * The <a href="https://doc.qt.io/qt-5/qgraphicsscene.html">QGraphicsScene</a> class
 * provides a surface for managing a large number of 2D graphical items.
 *
 * \subsection load_svg Loading documents
 * SVG documents are loaded from files or directly from strings by the loadFile and
 * loadSvg functions respectively.
 *
 * \subsection display_content Display content
 * Svg content is displayed by QuSvgView, whose constructor takes an instance of
 * QumbiaSvg. QuSvgView is a QGraphicsView that will display one QGraphicsSvgItem for
 * each element in the svg document with the *item* attribute defined and not set to "false".
 *
 * \subsection cumbia_connect Connect to sources
 * Each property of a svg object can be *connected* to a cumbia *source*.
 * The connection can be directly defined in the *svg* file, by introducing a
 * *<link>* element as *direct child* of the target element.
 * The *<link>* element will contain the following attributes
 * - *src* (compulsory): specifies the name of the source of data
 * - *attribute*: specifies the *target attribute* in the *parent* that will be changed
 *   upon new data from *src*.
 * If *attribute* is missing, then it must be clear from the context what is the
 * target of the data update *in the parent*, for example, the parent is a *<text>*
 * node.
 * Please see also QuSvgLink::isValid
 *
 * \subsubsection src_names Source names
 * Source names must be conform to the syntax understood by the available engines,
 * e.g. Tango, EPICS, random
 *
 * \subsection data_update Data update
 * When possible, the value of the source is directly updated: a number displayed on
 * a text label or the color of a state. In the other cases (an item position change
 * after a change in a value or a circle radius change proportional to another quantity)
 * custom mappings and transformations can be defined.
 *
 *
 *
 */
class QuSvg : public QuSvgReaderListener
{
public:
    QuSvg();
    QuSvg(QuDomListener *domlis);
    virtual ~QuSvg();

    QuDom quDom() const;

    bool loadFile(const QString &fileName);
    bool loadSvg(const QByteArray &svg);

    QString message() const;
    bool hasError() const;

    bool connect(const QString& source, const QString& id, const QString& property);

    void init(CumbiaPool *cupool,  const CuControlsFactoryPool &fpool);
    void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);
    void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac,
              const CuControlsWriterFactoryI& w_fac);

    void addDataListener(QuSvgDataListener *l);
    void addDataListener(const QString &id, QuSvgDataListener *l);
    void removeDataListener(QuSvgDataListener *l);
    void removeDataListeners(const QString& id);

    QuSvgReadersPool *getReadersPool() const;

private:
    QumbiaSVGPrivate *d;


    // QuSvgReaderListener interface
public:
    void onUpdate(const QuSvgResultData& rd);
};

#endif // QUMBIASVG_H
