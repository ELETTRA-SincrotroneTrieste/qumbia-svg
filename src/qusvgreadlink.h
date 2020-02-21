#ifndef QUSVG_READ_LINK_H
#define QUSVG_READ_LINK_H

#include <QString>
#include <QMap>

class QDomElement;
class QDomNode;

/*!
 * \brief The QuSvgLink class describes sources of readers
 */
class QuSvgReadLink
{
public:
    /*!
     * \brief The ObjectiveType enum defines the type of target of the link
     */
    enum ObjectiveType { Undefined = -1, //!< invalid objective type
                         AttributeObjectiveType, //!< an attribute in the parent
                         //! this is more generic: the parent node is the objective
                         NodeObjectiveType,
                         //! the objective is the root element of the svg DOM
                         //! this usually indicates sources that do not target
                         //! a specific attribute or node and need be treated
                         //! manually
                         RootObjectiveType };

    QuSvgReadLink();

    /*!
     * \brief inspect a QDomElement and infer the configuration of a QuSvgLink
     * \param e Qt QDomElement
     */
    QuSvgReadLink(const QDomElement &e);

    bool operator == (const QuSvgReadLink& other) const;

    bool isValid() const;

    QString message() const;

    ObjectiveType type() const;

    //! The source to connect to for reading: e.g. test/device/1/double_scalar or epics:waveform1
    QString src;
    //! The attribute in the *parent node* that will be modified when the
    //! result from src changes
    //! Can be the bare attribute name, like "width" or "rx" or "x"
    //! or specify a property within the attribute property list, for example
    //! "style/fill" or "style/stroke"
    QString attribute;

    //! If the attribute is in the form *attribute-name/property-in-property-list,
    //! e.g. "style/fill", this will store the property name (e.g. "fill")
    QString property;

    //! The id of the *parent* element (containing the <link> node)
    QString id;

    //! The id attribute of the <link> node, if specified. Not strictly required,
    //! but useful if you want to search the <link> node across the DOM
    QString link_id;

    //! An alias for the link that can be used upon result reception
    QString alias;

    //! A hint on the key to use when extracting data from the CuData bundle
    //! Useful in case the result is automatically interpreted.
    //! For example, if the key_hint is either "state_color" or "quality_color"
    //! and the attribute is "style/fill", then the object can be automatically
    //! filled with the color taken from the result.
    QString key_hint;

    //! The *tag name* of the *parent*
    QString tag_name;

    //! The type of target that is changed upon data change: it can be an attribute
    //! in the parent node or an entire node. In the latter case, it must be clear
    //! from the context what the target is. An example, is a <text> node.
    ObjectiveType objective_type;

    QString toString() const;

    //! map of string options extracted from the "options" attribute, which is a
    //! property-list
    //! Example: "options=properties-only:true
    QMap<QString, QString> options;

    //! Used by QuSvgReader, indicates whether the source is active or paused
    bool inactive;

private:
    QString m_error;

    void m_get_options(const QString &opt_att);

    int m_parent_count(const QDomNode &e) const;
};

#endif // QUSVG_READ_LINK_H
