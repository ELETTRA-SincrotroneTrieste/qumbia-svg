# cumbia svg library documentation

### Use Qt SVG module to show and animate SVG drawings
The <a href="https://doc.qt.io/qt-5/qtsvg-module.html">Qt SVG C++ module</a> provides functionality for handling SVG images. The *cumbia* engines and
infrastructures can be used to animate any SVG element, grouped in user defined QGraphicsSVGItem objects.

### Connect with the Tango and Epics (and more...) control system software
SVG elements in the drawing can be *connected* to values obtained from the available *cumbia* engines and their properties changed accordingly.
In several cases, the connections defined and the type of attributes in the SVG elements allow for automatic changes in the representation of 
the object within the drawing. In more complex ones, the programmer will map values from the engines to values of the *attributes* in the SVG
DOM document.

### Helper application support
Helper applications can be defined by the *helper* attribute in any item. As an alternative, they can be deduced from the source connected to the element, if 
a single one is defined (and if the engine in use supports this feature, e.g. Tango does).

### Item event handling
Events on an item are notified by Qt *signals*. This includes clicks (left button and contextual menu events). Clicks can target a *write operation*, while contextual
menus will by default make available *helper applications* (if defined) and pop up dialogs to perform slightly more complex writings (for example, write a scalar number 
or change a text value on the engine)





### Are you ready?
 
Start reading the <a href="html/index.html">cumbia svg library documentation</a>.
