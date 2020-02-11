#include "qusvgview.h"
#include "qumbiasvg.h"
#include "qudom.h"

#include <QSvgRenderer>

#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
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
    QMap<QString, QGraphicsSvgItem *> items_cache;
};

QuSvgView::QuSvgView(QWidget *parent)
    : QGraphicsView(parent)
{
    d = new QuSvgViewPrivate;
    d->m_renderer = Native;

    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    //    connect(scene(), SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));
}

void QuSvgView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
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

void QuSvgView::refresh() {
    //    QTime t;
    //    t.start();
    QStringList changed_item_names;
    for(int i = 0; i < d->m_ids.size(); i++) {

        const QString id = QString("circle_%1").arg(i);
        QDomNode old_ellipse;;
        QDomElement ellipse(old_ellipse.toElement());
        changed_item_names << id;
        if(!ellipse.isNull()) {
            QString style = QString("fill:#0010ef;"
                                    "stroke:#00ffed;"
                                    "stroke-width:1;");
            ellipse.setAttribute("style", style);
            old_ellipse.parentNode().replaceChild(ellipse, old_ellipse);
        }

    }
    findChild<QSvgRenderer *>()->load(d->m_dom->getDocument().toString().toLatin1());
    foreach(QString changed_item_name, changed_item_names) {
        QGraphicsSvgItem *it = d->items_cache[changed_item_name];
        if(it) {
            QPointF pos = renderer()->boundsOnElement(changed_item_name).topLeft();
            it->setPos(pos);
            //        it->update();
        }
    }
    //    scene()->update(scene()->sceneRect());
    //     qDebug() << __PRETTY_FUNCTION__ << "refresh took ms" << t.elapsed();
}

void QuSvgView::paintEvent(QPaintEvent *event)
{
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
    //    qDebug() << __PRETTY_FUNCTION__ << "repaint took ms" << t.elapsed();
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

void QuSvgView::onDocumentLoaded(QuDom *dom, const QStringList &ids) {
    d->m_dom = dom;
    QGraphicsScene *s = scene();
    QSvgRenderer *renderer = new QSvgRenderer(dom->getDocument().toString().toLatin1(), this);
    foreach(QString id, ids) {
        QGraphicsSvgItem *svgItem = new QGraphicsSvgItem();
        svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
        svgItem->setSharedRenderer(renderer);
        svgItem->setElementId(id);
        svgItem->setFlag(QGraphicsItem::ItemIsMovable, true);
//        svgItem->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        svgItem->setCacheMode(QGraphicsItem::NoCache);
        svgItem->setObjectName(id);
        s->addItem(svgItem);
        d->items_cache[id] = svgItem;
        svgItem->setPos(renderer->boundsOnElement(id).topLeft());
    }
}

void QuSvgView::onAttributeChange(const QString &id,
                                  const QString &attribute,
                                  const QString &value) {
    findChild<QSvgRenderer *>()->load(d->m_dom->getDocument().toString().toLatin1());

    QGraphicsSvgItem *it = d->items_cache[id];
    if(it) {
        QRectF oldBounds = it->boundingRect();
        QRectF bounds = renderer()->boundsOnElement(id);
        qDebug() << __PRETTY_FUNCTION__ << "bounds on element " << id << bounds;
        QPointF pos = bounds.topLeft();
        it->update();
        if(pos != it->pos()) {
            qDebug() << __PRETTY_FUNCTION__ << it->objectName() << "position changed? " << it->pos() << "-->" << pos;
            it->setPos(pos);
        }

    }
    else {

    }
}
