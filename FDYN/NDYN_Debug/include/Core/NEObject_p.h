#pragma once

class NEObject;




class NEObjectPrivate
{
public:
    NEObjectPrivate(NEObject* s):S(s), m_pObjCont(0) {}
    void handleDynamicPropertyChangeEvent(QDynamicPropertyChangeEvent* e);

    bool m_Shown;        /**< If true the object is listed in the scene inspector. */
    bool m_Static;                      /**< Keeps wheter the node is static (non-deletable). */
    NEObject::NEOsdMsgPos m_OsdMsgPos;  /**< OSD position of the object. */
    uint m_OsdStartRank;                /**< The lower bound for the rank of this object in the osd. */
    bool m_Verbose;                     /**< When verbose is true, additional information is shown for debuging purposes. */
    QList<QObject*> m_Actions;          /**< List of actions that can be triggered by the user, used in e.g. context menu. */
    QMap<QString, NEDynaPropMetaData> m_DynaProMD;      /**< Map of metadata for dynamic properties. */
    QMap<QString, NEPropMetaData> m_ProMD;  /**< Map of metadata for all property types */
    IObjectControllerPrivate* m_pObjCont;    /**< Pointer to the object controller */

    void initProperties();

private:
    NEObject* S;
};
