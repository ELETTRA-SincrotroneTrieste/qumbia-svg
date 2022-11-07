#ifndef QUREPLACEWILDCARDHELPERINTERFACE_H
#define QUREPLACEWILDCARDHELPERINTERFACE_H

#include <QString>

class CuContextI;

class QuSvgReplaceWildcardHelperInterface
{
public:
    virtual ~QuSvgReplaceWildcardHelperInterface() {}

    /*!
     * \brief replace the wildcards in wildcard_src according to the application
     *        arguments and the engine(s) in use
     * \param wildcard_src source with wildcards, e.g. $1/double_scalar or $1->GetValue(0,10)
     * \return a valid source according to the engine(s) in use, e.g.
     *      *test/device/1/double_scalar* or *my/current/device->GetValue(0,10)*
     */
    virtual QString replaceWildcards(const QString& wildcard_src) const = 0;

    /*!
     * \brief provide a pointer to a valid context either built with Cumbia or
     *        CumbiaPool, whatever is available
     * \return a CuContext that can be used by the helper application plugin
     *
     * \par Note
     * It is not necessary that the context is taken from a reader or writer.
     *
     * \par Example
     * \code
        CuContext *MyRWildImpl::getContext() {
           if(d->cumbia != nullptr) ctx = new CuContext(d->cumbia, *(d->reader_factory));
           if(d->cumbia_pool != nullptr) ctx = new CuContext(d->cumbia_pool, d->factory_pool);
           return ctx;
        }
     * \endcode
     */
    virtual CuContextI *getContextI() const = 0;
};

#endif // QUREPLACEWILDCARDHELPERINTERFACE_H
