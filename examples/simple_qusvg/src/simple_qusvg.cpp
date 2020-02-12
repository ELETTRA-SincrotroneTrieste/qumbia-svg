#include "simple_qusvg.h"
#include <cumacros.h>

#include <QMessageBox>
#include <QGridLayout>

// cumbia svg includes
#include <qusvgview.h>
#include <qusvg.h>
#include <QtDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <QSettings> // save line edit values

SimpleQuSvg::SimpleQuSvg(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *lo = new QGridLayout(this);
    QuSvgView *svgview = new QuSvgView(this);
    m_qu_svg = new QuSvg(svgview);
    lo->addWidget(svgview, 0,0, 5, 5 );

    QLabel *l = new QLabel("Set attribute", this);
    QLineEdit *le_id = new QLineEdit(this);
    le_id->setObjectName("le_id");
    QLineEdit *le_attnam = new QLineEdit(this);
    le_attnam->setObjectName("le_attnam");
    QLineEdit *le_attval = new QLineEdit(this);
    le_attval->setObjectName("le_attval");
    QPushButton *pb = new QPushButton("APPLY", this);
    lo->addWidget(l, 5, 0, 1, 1);
    lo->addWidget(le_id, 5, 1, 1, 1);
    lo->addWidget(le_attnam, 5, 2, 1, 1);
    lo->addWidget(le_attval, 5, 3, 1, 1);
    lo->addWidget(pb, 5, 4, 1, 1);
    connect(pb, SIGNAL(clicked()), this, SLOT(applyClicked()));

    QSettings s;
    le_id->setText(s.value("id").toString());
    le_attnam->setText(s.value("attnam").toString());
    le_attval->setText(s.value("attval").toString());

    bool ok = m_qu_svg->loadFile(":src/drawing.svg");
    if(!ok) {
        QMessageBox::critical(this, "Error loading svg file", m_qu_svg->message());
    }
}

SimpleQuSvg::~SimpleQuSvg()
{
    QString id = findChild<QLineEdit *>("le_id")->text();
    QString attn = findChild<QLineEdit *>("le_attnam")->text();
    QString attv = findChild<QLineEdit *>("le_attval")->text();
    QSettings s;
    s.setValue("id", id);
    s.setValue("attnam", attn);
    s.setValue("attval", attv);
}

void SimpleQuSvg::applyClicked()
{
    QString id = findChild<QLineEdit *>("le_id")->text();
    QString attn = findChild<QLineEdit *>("le_attnam")->text();
    QString attv = findChild<QLineEdit *>("le_attval")->text();
    QuDom dom = m_qu_svg->quDom();
    QuDomElement dome(dom);
    QuDomElement de = dome[id];
    if(de.isNull()) {
        QMessageBox::information(this, "Could not change attribute",
                                 QString("Failed to set value \"%1\" on attribute"
                                         "\'%2\" element id \"%3\"").arg(attv)
                                 .arg(attn).arg(id));
    }
    else
        de.a(attn, attv);
}
