#include "qusvgview.h"
#include "qusvg.h"
#include "qugraphicsitem.h"
#include "qudom.h"
#include "qugraphicssvgitemxtensionfactory.h"

#include <QSvgRenderer>

#include <QWheelEvent>
#include <QMouseEvent>

#include <QPaintEvent>
#include <QtDebug>
#include <qmath.h>

#include <QDomDocument>
#include <QDomElement>

// random color
#include <time.h>
#include <math.h>
// refresh svg
#include <QTimer>
#include <QTime>

#include <QApplication>
#include <QRandomGenerator>

#ifndef QT_NO_OPENGL
#include <QGLWidget>
#endif

class QuSvgViewPrivate {
public:
    QuDom *m_dom;
    QuSvgView::RendererType m_renderer;
    QImage m_image;
    QStringList m_ids;
    QMap<QString, QString> svg_cache;
    QMap<QString, QuGraphicsItem *> items_cache;
    QSvgRenderer *renderer;
    QuGraphicsItemXtensionFactory *xt_factory;
    bool mouse_pressed;
};

QuSvgView::QuSvgView(QWidget *parent)
    : QGraphicsView(parent)
{
    d = new QuSvgViewPrivate;
    d->m_renderer = Native;
    d->mouse_pressed = false;
    d->xt_factory = new QuGraphicsItemXtensionFactory;

    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
}

QSize QuSvgView::svgSize() const
{
    return QSize();
}

void QuSvgView::setRenderer(RendererType type)
{
    d->m_renderer = type;

    if (d->m_renderer == OpenGL) {
#ifndef QT_NO_OPENGL
        setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    } else {
        setViewport(new QWidget);
    }
}

void QuSvgView::setHighQualityAntialiasing(bool highQualityAntialiasing)
{
#ifndef QT_NO_OPENGL
    setRenderHint(QPainter::HighQualityAntialiasing, highQualityAntialiasing);
#else
    Q_UNUSED(highQualityAntialiasing);
#endif
}

qreal QuSvgView::zoomFactor() const
{
    return transform().m11();
}

QuGraphicsItemXtensionFactory *QuSvgView::extension_factory() const {
    return d->xt_factory;
}

void QuSvgView::zoomIn()
{
    m_zoomBy(2);
}

void QuSvgView::zoomOut()
{
    m_zoomBy(0.5);
}

void QuSvgView::resetZoom()
{
    if (!qFuzzyCompare(zoomFactor(), qreal(1))) {
        resetTransform();
        emit zoomChanged();
    }
}

void QuSvgView::paintEvent(QPaintEvent *event)
{
    const QRect& r = event->rect();
    //    QTime t;
    //    t.start();
    if (d->m_renderer == Image) {
        if (d->m_image.size() != viewport()->size()) {
            d->m_image = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
        }

        QPainter imagePainter(&d->m_image);
        QGraphicsView::render(&imagePainter);
        imagePainter.end();

        QPainter p(viewport());
        p.drawImage(0, 0, d->m_image);

    } else {
        QGraphicsView::paintEvent(event);
    }
}

void QuSvgView::mousePressEvent(QMouseEvent *event) {
    d->mouse_pressed = true;
    QGraphicsView::mousePressEvent(event);
}

/*!
 * \brief emits itemClicked with the list of items at the position pos in the view.
 *
 * The items are listed in descending stacking order (i.e., the first item in
 * the list is the uppermost item, and the last item is the lowermost item). pos is in viewport coordinates.
 *
 * \param event
 */
void QuSvgView::mouseReleaseEvent(QMouseEvent *event) {
    if(d->mouse_pressed && event->button() == Qt::LeftButton) {
        d->mouse_pressed = false;
        QList<QGraphicsItem *> items_under = items(event->pos());
        emit itemClicked(items_under, mapToScene(event->pos()), event->pos());
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void QuSvgView::contextMenuEvent(QContextMenuEvent *event) {
    emit itemContextMenuRequest(items(event->pos()), mapToScene(event->pos()), event->pos());
    QGraphicsView::contextMenuEvent(event);
}

void QuSvgView::sceneChanged(const QList<QRectF> &rects)
{
    qDebug() << __PRETTY_FUNCTION__ << rects;
}

void QuSvgView::wheelEvent(QWheelEvent *event)
{
    m_zoomBy(qPow(1.2, event->delta() / 240.0));
}

void QuSvgView::m_zoomBy(qreal factor)
{
    const qreal currentZoom = zoomFactor();
    if ((factor < 1 && currentZoom < 0.1) || (factor > 1 && currentZoom > 10))
        return;
    scale(factor, factor);
    emit zoomChanged();
}

QSvgRenderer *QuSvgView::renderer() const
{
    return findChild<QSvgRenderer *>();
}

QuGraphicsItem *QuSvgView::item(const QString &id) const {
    return d->items_cache.contains(id) ? d->items_cache[id] : nullptr;
}

QString QuSvgView::id(QuGraphicsItem *it) const {
    return d->items_cache.key(it, QString());
}

QList<QuGraphicsItem *> QuSvgView::itemsByTag(const QString &tag) const {
    QList<QuGraphicsItem *> l;
    const QStringList& ids = d->m_dom->idsByTagName(tag, QDomElement());
    foreach(const QString& id, ids)
        l << d->items_cache[id];
    return l;
}

QList<QuGraphicsItem *> QuSvgView::qusvgitems() const {
    QList<QuGraphicsItem *> l;
    foreach(QGraphicsItem* i, QuSvgView::items())
        if(i->type() == QuGraphicsItem::QuGraphicsItemType)
            l.append(static_cast<QuGraphicsItem *>(i));
    return l;
}

void QuSvgView::m_createItem(QString id) {
    QuDomElement rootel(d->m_dom);
    QuDomElement el = rootel[id];
    QuGraphicsItem *svgItem = d->xt_factory->create(el);

    svgItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
//    svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    svgItem->setSharedRenderer(d->renderer);
    svgItem->setElementId(id);
//        svgItem->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    svgItem->setCacheMode(QGraphicsItem::NoCache);
    svgItem->setObjectName(id);
    scene()->addItem(svgItem);
    d->items_cache[id] = svgItem;
    // item clickable?
    // set item properties: shape and clickable have dedicated
    // Q_PROPERTY with set and get methods
    foreach(QString property, QStringList() << "shape" << "clickable" << "z")
        if(!el.isNull() && !el.a(property).isEmpty())
            svgItem->setProperty(qstoc(property), el.a(property));

    const QRectF &item_r = d->renderer->boundsOnElement(id);
    const QTransform& m = d->renderer->transformForElement(id);
    !m.isIdentity() ? svgItem->setPos(m.mapRect(item_r).topLeft())
                    : svgItem->setPos(item_r.topLeft());
//    if(!m.isIdentity()) {
//        svgItem->setTransform(QTransform(m));
//    }
}

void QuSvgView::onDocumentLoaded(QuDom *dom, const QStringList &ids) {
    d->m_dom = dom;
    d->renderer = new QSvgRenderer(d->m_dom->getDocument().toString().toLatin1(), this);
    foreach(QString id, ids) {
        m_createItem(id);
    }
}

void QuSvgView::onElementChange(const QString &id, QuDomElement *dom_e) {
    QString item_id;
    d->items_cache.contains(id) ? item_id = id : item_id = dom_e->itemId();
    QuGraphicsItem *it = d->items_cache.value(item_id);
    if(it) {
        QRectF oldBounds = renderer()->boundsOnElement(item_id);
        QSvgRenderer *renderer = findChild<QSvgRenderer *>();
        renderer->load(d->m_dom->getDocument().toString().toLatin1());
        // it seems that if the shape of the object changes the bounding rect
        // the item is not drawn correctly. We are forced to remove and add it
        // over again.
        QRectF bounds = renderer->boundsOnElement(item_id);
        if(bounds != oldBounds) {
            delete it;
            m_createItem(item_id); // replaces item_id in cache
        }
        else {
            it->update();
        }
    }
    else {
        perr("QuSvgView.onAttributeChange: could not find item with id %s", qstoc(id));
    }
}
