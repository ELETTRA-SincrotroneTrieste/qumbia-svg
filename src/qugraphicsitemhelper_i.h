#ifndef QUGRAPHICSITEMHELPER_I_H
#define QUGRAPHICSITEMHELPER_I_H

class QuGraphicsItem;

/*!
 * \brief interface for item helpers.
 *
 * The interface imposes an *id* method that forces subclasses to identify themselves and
 * an item method that returns the attached item
 *
 * Graphics item helpers are registered on QuGraphicsItem and can be obtained through
 * the QGraphicsSvgItem::helper method.
 *
 * Helpers ownership is yielded to QuGraphicsItem, that deletes them upon destruction.
 * Life of helpers shall be guaranteed to span beyond the attached QuGraphicsItem.
 * In particular cases, QuGraphicsItemHelper_I::detach shall be called before explicit
 * deletion.
 *
 * \see QGraphicsSvgItem
 * \see QGraphicsItemGeom
 * \see QGraphicsItemAxes
 */
class QuGraphicsItemHelper_I {
public:
    virtual ~QuGraphicsItemHelper_I() {}

    /*!
     * \brief subclasses shall provide a value to identify themselves and allow a static cast
     *        to their type
     * \return the helper class id
     *
     * This can be used to cast a helper to a specific instance or to obtain a reference to a
     * specific helper through QuGraphicsItem::helper
     */
    virtual int id() const = 0;

    /*!
     * \brief return the item to which the helper is attached
     * \return a pointer to the associated item
     */
    virtual QuGraphicsItem* item() const = 0;

    /*!
     * \brief detach the helper from the item before explicitly deleting it
     *
     * Ownership is yielded to the item, that deletes the helpers upon destruction.
     * If explicit deletion is desired, the item shall be detached before.
     */
    virtual void detach() = 0;
};

#endif // QUGRAPHICSITEMHELPER_I_H
