#include <quapplication.h>
#include "simple_qusvg.h"

int main(int argc, char *argv[])
{
    int ret;
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("Elettra");
    qu_app.setApplicationName("cumbia svg example");
    qu_app.setApplicationVersion("1.0.0");
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "+390403758073");
    qu_app.setProperty("office", "T2PT025");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    SimpleQuSvg *w = new SimpleQuSvg(nullptr);
    w->show();

    ret = qu_app.exec();
    delete w;
    return ret;
}
