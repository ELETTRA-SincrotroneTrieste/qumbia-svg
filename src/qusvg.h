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
 * The <a href="https://doc.qt.io/qt-5/qtsvg-module.html">Qt SVG C++ module</a> provides functionality for handling SVG images. The *cumbia* engines and
 * infrastructures can be used to change any SVG element.
 * An *svg* node with the *item* attribute defined and not set to *false* is
 * rendered in a dedicated QuGraphicsSvgItem, an extension of
 * the Qt <a href="https://doc.qt.io/qt-5/qgraphicssvgitem.html">QGraphicsSvgItem</a>.
 * QuGraphicsSvgItem represents the "*item*" node and its children on a
 * <a href="https://doc.qt.io/qt-5/qgraphicsscene.html">QGraphicsScene</a>.
 * The <a href="https://doc.qt.io/qt-5/qgraphicsscene.html">QGraphicsScene</a> class
 * provides a surface for managing a large number of 2D graphical items.
 *
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
 *
 * \subsubsection main_classes Classes
 * QuSvg (main class), QuDom (DOM representation), QuDomElement (QDomElement wrapper with
 * improved search and access functionality), QuGraphicsSvgItem (extends QGraphicsSvgItem),
 * QuSvgView (<a href="https://doc.qt.io/qt-5/qgraphicsscene.html">QGraphicsView</a>
 * extension that creates items, manages zoom operations, signals click and context
 * menu events).
 *
 * \subsection accessing_items  Accessing items and attributes
 * Items, that map the *nodes* defined in the *svg* DOM document, can be
 * accessed in a very simple fashion by means of their *id* within the *square brackets* operator
 * defined in the QuDom and QuDomElement classes. Slash ('/') separate *ids* can be
 * specified to traverse the DOM more efficiently through a hierarchical path.

 * \code
    const QuDom* qudom = m_qusvg->quDom(); // m_qusvg is a reference to QuSvg
    const QuDomElement root(*qudom);
    root["ellipse/blue_circle"].a("style/fill", "#ffffff"); // white
    root["lil_star"].a("style/fill", "#ff0000"); // red
 * \endcode
 *
  * \image html screenshots/svg_circle_attributes_access.png "Fig 1. Items after changing style/fill property"
  * \image latex screenshots/svg_circle_attributes_access.png "Fig 1. Items after changing style/fill property" width=12cm
 *
 * \subsubsection access_item_classes Classes
 * QuDom, QuDomElement
 *
 * \subsection cumbia_connect Connect to the Tango and Epics (and more...) control system software
 * SVG elements in the drawing can be *connected* to values obtained from the available *cumbia* engines and their properties changed accordingly.
 * In several cases, the connections defined and the type of attributes in the SVG elements allow for automatic changes in the representation of
 * the object within the drawing. In more complex ones, the programmer will map values from the engines to values of the *attributes* in the SVG
 * DOM document.
 *
 * \subsubsection cumbia_connect_details Setting up connections in the svg file
 * Each property of a svg object can be *connected* to a cumbia *source*.
 * The connection can be directly defined in the *svg* file, by introducing a
 * *<read>* element as *direct child* of the target element.
 * The *<read>* element will contain the following attributes
 * - *src* (compulsory): specifies the name of the source of data
 * - *attribute*: specifies the *target attribute* in the *parent* that will be changed
 *   upon new data from *src*.
 * - *hint* suggest *cumbia-svg* which key to use to fetch the value from the *data bundle*.
 *   In the example below, *state_color* is used to extract the color that will be used
 *   to change the *fill* property of the *style* attribute of the parent *<rect>*.
 *
 * rect elements changing their colors according to a Tango state  | svg code to declare an item with a reader
 * ------------------------------------------------------------------|------------------------------------------------------
 * ![](svg_demo.gif) | ![](svg_read_src.png)
 *
 *
 * *attribute* can be omitted if it is clear from the context what is the
 * target of the data update *in the parent*. For example, the parent is a *<text>*
 * node.
 *
 * In the svg source snippet below
 *
 * |source connected to a *text* node | text label |
 * |----------------------------------|------------|
 * |![](svg_text_read_src.png)        | ![](svg_text.gif) |
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
 * \subsection target_and_events Target names and item events
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
