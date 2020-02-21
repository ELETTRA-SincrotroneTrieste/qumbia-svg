#ifndef QUSVGRESULTDATA_H
#define QUSVGRESULTDATA_H

#include <QString>
#include <cudata.h>
#include <qusvgreadlink.h>

class QuSvgResultDataPrivate;

class QuSvgResultData {
public:

    QuSvgResultData(const CuData& _data,
                    const QuSvgReadLink& link);
    ~QuSvgResultData();

    bool attributeHasProperty() const;
    QString full_attribute() const;
    QString property() const;
    const CuData& data; //!< CuData: result as CuData
    const QuSvgReadLink& link; //! QuSvgLink: link information
private:

    QuSvgResultDataPrivate *d;
};

#endif // QUSVGRESULTDATA_H
