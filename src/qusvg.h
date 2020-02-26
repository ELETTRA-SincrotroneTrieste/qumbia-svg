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
class QuSvgConnectionsPool;
class QuGraphicsSvgItem;

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
 * *<read>* element as *direct child* of the target element.
 * The *<read>* element will contain the following attributes
 * - *src* (compulsory): specifies the name of the source of data
 * - *attribute*: specifies the *target attribute* in the *parent* that will be changed
 *   upon new data from *src*.
 *
 * \image html svg_read_src.png "Fig. 1 - Readers definition in the svg file"
 * \image latex svg_read_src.png "Fig. 1 - Readers definition in the svg file"  width=12cm
 *
 * \image html svg_read.png "Fig. 2 - SVG items connected to Tango sources"
 * \image latex svg_read.png "Fig. 2 - SVG items connected to Tango sources"  width=12cm
 *
 *
 * If *attribute* is missing, then it must be clear from the context what is the
 * target of the data update *in the parent*, for example, the parent is a *<text>*
 * node.
 *
 * For example, in the svg source snippet below
 *
 * \image html svg_text_read_src.png "Sources connected to <text> nodes"
 * \image latex svg_text_read_src.png "Sources connected to <text> nodes"  width=12cm
 *
 * it is easy for *cumbia-svg* to understand that the value of *long_scalar* will
 * update the text node child of <svg:text> with id *ducktext*.
 *
 * In a similar way, under the *<read>* node, the *hint=state_color*
 * linked to the *style/fill* target attribute in Fig. 1 will update
 * the *fill color* of the parent *<rect>*.
 *
 * Please see also QuSvgReadLink::isValid
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
 * \subsection target_and_events Targets names and *item events*
 *
 * \subsubsection targets Targets
 * Target names must be conform to the syntax understood by the available engines,
 * e.g. Tango, EPICS, random.
 * Writings without arguments, like those triggered by a *push button*, can be done
 * through a *clickable item*. Those involving input values are performed by dedicated
 * dialogs, either executed from a contextual menu or a simple click.
 * Items with a *target* attribute defined and the *clickable* attribute set to *true*
 * will start a write operation after a left button click event.
 * Items with a *target* attribute defined and *without* a clickable attribute will
 * make a write operation available through a contextual menu. In the latter case,
 * if an *action* string attribute is defined, its value will be set as text in the contextual
 * menu action associated.
 * The *type* attribute defines the type of desired *writer* used in the pop up dialog.
 * Its value can be one of (case insensitive):
 * - "quapplynumeric"
 * - "qubutton"
 * - "quinputoutput"
 * - "qulineedit"
 *
 * \subsubsection Item events.
 * If an item has the *clickable* attribute set to true, it will be highlighted when
 * mouse enters the item as well as a click operation is performed.
 * QuSvgView itemContextMenuRequest and itemClicked signals are emitted according to
 * the event detected.
 *
 * The following scenarios are valid:
 * - *target and clickable defined, type not defined*: left button click executes
 *   the operation as written in the *target* attribute value
 * - *target, clickable and type are defined*: left click pops up a dialog with a widget
 *   of the desired type.
 * - *target and type defined, clickable not defined*: a context menu provides an
 *   action to execute a dialog with the desired widget to execute the target;
 * - *target defined, clickable and type not defined*: a context menu provides an action
 *   to execute a dialog with a *simple apply button* to execute the command as specified
 *   in *target*
 * - *target, clickable and type* not defined: itemContextMenuRequest signal only is
 *   emitted upon right click
 * - *clickable defined, target and type not defined*, itemClicked is emitted on left
 *   button click, itemContextMenuRequest is emitted on right button click.
 *
 *
 */
class QuSvg : public QuSvgConnectionListener
{
public:
    QuSvg(QuDomListener *domlis);
    virtual ~QuSvg();

    QuDom *quDom() const;

    bool loadFile(const QString &fileName);
    bool loadSvg(const QByteArray &svg);

    QString message() const;
    bool hasError() const;

    void init(CumbiaPool *cupool,  const CuControlsFactoryPool &fpool);
    void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac,
              const CuControlsWriterFactoryI& w_fac);

    void addDataListener(QuSvgDataListener *l);
    void addDataListener(const QString &id, QuSvgDataListener *l);
    void removeDataListener(QuSvgDataListener *l);
    void removeDataListeners(const QString& id);

    QuSvgConnectionsPool *getConnectionsPool() const;

private:
    QumbiaSVGPrivate *d;


    // QuSvgReaderListener interface
public:
    void onUpdate(const QuSvgResultData& rd);
};

#endif // QUMBIASVG_H
