#ifndef QUSVGLAYERHELPER_H
#define QUSVGLAYERHELPER_H

#include <QObject>

class QuSvg;
class QDomNode;
class QuSvgLayerHelperPrivate;

class QuSvgLayerHelper : public QObject
{
    Q_OBJECT
public:
    explicit QuSvgLayerHelper(const QuSvg& qusvg);

    ~QuSvgLayerHelper();

    bool layerVisible(const QString& name);

public slots:
    void setLayerVisible(const QString& name, bool v);


signals:
    void activeSourcesChanged(const QStringList& srcs, bool active);

private:
    QuSvgLayerHelperPrivate *d;

    QStringList m_findSrcs(const QDomNode &parent);

};

#endif // QUSVGLAYERHELPER_H
