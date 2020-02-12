#ifndef QUSVGDATALISTENER_H
#define QUSVGDATALISTENER_H

#include <qusvgreader.h>
#include <qudom.h>
#include <qusvgresultdata.h>

class QuSvgDataListener
{
public:
    virtual bool onUpdate(const QuSvgResultData& res, QuDom *qudom) = 0;
};

#endif // QUSVGDATALISTENER_H
