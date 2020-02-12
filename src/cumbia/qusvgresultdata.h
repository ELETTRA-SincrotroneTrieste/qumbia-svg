#ifndef QUSVGRESULTDATA_H
#define QUSVGRESULTDATA_H

#include <QString>
#include <cudata.h>

class QuSvgResultDataPrivate;

class QuSvgResultData {
public:

    QuSvgResultData(const CuData& _data, const QString& _id,
                    const QString& _attribute,
                    const QString& _att_property,
                    const QString& _key_hint);

    ~QuSvgResultData();

    bool attributeHasProperty() const;

    QString full_attribute() const;

    QString property() const;

    const CuData& data; // CuData
    const QString& id; // id of link node's parent
    const QString& attribute;  // e.g. "style"
    const QString& att_property; // e.g. fill or stroke
    // hint for the key to use to extract data from CuData
    // can be useful during automatic processing
    const QString& key_hint;

private:

    QuSvgResultDataPrivate *d;
};

#endif // QUSVGRESULTDATA_H
