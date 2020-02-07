#include "qumbiasvg.h"
#include <QMap>
#include <QFileInfo>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QDir>

#include <cumacros.h>

class QumbiaSVGPrivate {
public:
    QString m_msg;
    QuDom qudom;
};

QumbiaSvg::QumbiaSvg() {
    d = new QumbiaSVGPrivate;
}

QumbiaSvg::QumbiaSvg(QuDomListener *domlis)
{
    d = new QumbiaSVGPrivate;
    d->qudom.addDomListener(domlis);
}

QumbiaSvg::~QumbiaSvg() {
    delete d;
}

QuDom &QumbiaSvg::quDom() {
    return d->qudom;
}


bool QumbiaSvg::loadFile(const QString &fileName)
{
    d->m_msg.clear();
    if (!QFileInfo::exists(fileName)) {
        d->m_msg = QString("Could not open file '%1'.").arg(QDir::toNativeSeparators(fileName));
        return false;
    }
    QFile f(fileName);
    if(f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f);
        QString svg = in.readAll();
        f.close();
        return loadSvg(svg.toLatin1());
    }
    d->m_msg = f.errorString();
    return false;
}

bool QumbiaSvg::loadSvg(const QByteArray &svg)
{
    d->m_msg.clear();
    int line, column;
    bool success = d->qudom.setContent(svg, &d->m_msg, &line, &column);
    if(!success)
        d->m_msg += QString(" line %1 column %2").arg(line).arg(column);
    return success;
}

QString QumbiaSvg::message() const {
    return d->m_msg;
}

bool QumbiaSvg::hasError() const {
    return d->m_msg.size() > 0;
}

bool QumbiaSvg::connect(const QString &source, const QString &id, const QString &property)
{

    return true;
}
