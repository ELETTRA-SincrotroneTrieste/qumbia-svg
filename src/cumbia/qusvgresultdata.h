#ifndef QUSVGRESULTDATA_H
#define QUSVGRESULTDATA_H

#include <QString>
#include <cudata.h>
#include <qusvglink.h>

class QuSvgResultDataPrivate;

class QuSvgResultData {
public:

    QuSvgResultData(const CuData& _data,
                    const QuSvgLink& link);

    ~QuSvgResultData();

    bool attributeHasProperty() const;

    QString full_attribute() const;

    QString property() const;
    const CuData& data; //!< CuData: result as CuData
    const QuSvgLink& link; //! QuSvgLink: link information
private:

    QuSvgResultDataPrivate *d;
};

#endif // QUSVGRESULTDATA_H
