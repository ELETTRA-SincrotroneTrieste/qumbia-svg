#ifndef QUSVGLAYERHELPER_H
#define QUSVGLAYERHELPER_H

#include <QObject>
#include <QStringList>

class QuDom;
class QuSvgView;
class QDomNode;
class QuSvgLayerHelperPrivate;

class QuSvgLayerHelper : public QObject
{
    Q_OBJECT
public:
    explicit QuSvgLayerHelper(QuDom *dom, QuSvgView *view);

    ~QuSvgLayerHelper();

    bool layerVisible(const QString& name);

public slots:
    void setLayerVisible(const QString& name, bool v);


signals:
    void activeSourcesChanged(const QStringList& ids, bool active);

private:
    QuSvgLayerHelperPrivate *d;

    QStringList m_findSrcIds(const QDomNode &parent);

};

#endif // QUSVGLAYERHELPER_H
