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
class QumbiaSvg;
class QuDom;

class QuSvgView : public QGraphicsView, public QuDomListener
{
    Q_OBJECT

public:
    enum RendererType { Native, OpenGL, Image };

    explicit QuSvgView(QWidget *parent = nullptr);

    void setRenderer(RendererType type = Native);
    void drawBackground(QPainter *p, const QRectF &rect) override;

    QSize svgSize() const;
    QSvgRenderer *renderer() const;

    qreal zoomFactor() const;

public slots:
    void setHighQualityAntialiasing(bool highQualityAntialiasing);
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void refresh();

signals:
    void zoomChanged();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

protected slots:
    void sceneChanged(const QList<QRectF>& rects);

private:
    void m_zoomBy(qreal factor);

    QuSvgViewPrivate *d;

    // QuDomListener interface
public:
    void onDocumentLoaded(QuDom *dom, const QStringList &ids);
    void onAttributeChange(const QString &id, const QString &attribute, const QString &value);
};

#endif // QUSVGVIEW_H
