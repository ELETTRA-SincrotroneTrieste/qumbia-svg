#ifndef QUSVGVIEW_H
#define QUSVGVIEW_H
#include <QGraphicsView>
#include <QDomDocument>
#include <QMap>

#include <qudom.h>

QT_BEGIN_NAMESPACE
class QGraphicsSvgItem;
class QSvgRenderer;
class QWheelEvent;
class QPaintEvent;
QT_END_NAMESPACE

class QuSvgViewPrivate;
class QuGraphicsItem;
class QuSvg;
class QuDom;
class QuGraphicsItemXtensionFactory;

class QuSvgView : public QGraphicsView, public QuDomListener
{
    Q_OBJECT

public:
    enum RendererType { Native, OpenGL, Image };

    explicit QuSvgView(QWidget *parent = nullptr);

    void setRenderer(RendererType type = Native);

    QSize svgSize() const;
    QSvgRenderer *renderer() const;
    QuGraphicsItem *item(const QString& id) const;
    QString id(QuGraphicsItem *it) const;
    QList<QuGraphicsItem *> itemsByTag(const QString& tag) const;
    QList<QuGraphicsItem *> qusvgitems() const;

    qreal zoomFactor() const;

    QuGraphicsItemXtensionFactory *extension_factory() const;

public slots:
    void setHighQualityAntialiasing(bool highQualityAntialiasing);
    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void zoomChanged();

    /*!
     * \brief click event
     * \param items list of items under the mouse
     * \param scenePos position in scene coordinates
     * \param pos position in item coordinates
     */
    void itemClicked(const QList<QGraphicsItem *>& items,
                     const QPointF& scenePos,
                     const QPointF& pos);

    void itemContextMenuRequest(const QList<QGraphicsItem *>& items,
                                  const QPointF& scenePos,
                                  const QPointF& pos) ;
protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

protected slots:
    void sceneChanged(const QList<QRectF>& rects);

private:
    void m_zoomBy(qreal factor);

    QuSvgViewPrivate *d;

    // QuDomListener interface
    void m_createItem(QString id);
public:
    void onDocumentLoaded(QuDom *dom, const QStringList &ids) override;
    void onElementChange(const QString &id, QuDomElement* dom_e) override;
};

#endif // QUSVGVIEW_H
