#pragma once

#include "Base/NENodeFactory.h"
#include "Rendering/Spatial/NEOSDBase.h"
#include "Rendering/Spatial/Renderers/NEOSDBaseRenderer.h"

class NERigidTopography;
class NEOSDRigidTopographyPrivate;
//
// Move all particle fluid related rendering/visualization code here..
//

class NEOSDRigidTopography: public NEOSDBase
{
    Q_OBJECT
    NE_CLASS("Rigid Topography OSD")

public:
    enum NEOSDOrientation {Left = 0, Right = 1, Top = 2, Bottom = 3};

    NE_PROP_RW(QString, TitleText, "Title Text", "Title Text", true,)
    NE_PROP_RW(int, TickCount, "Tick Count", "Tick Count of color stages", true,)
    NE_PROP_RW(NEOSDOrientation, OSDOrientation, "Orientation", "Indicates the orientation of the OSD.", true,)

    Q_ENUMS(NEOSDOrientation)

public:
    NEOSDRigidTopography( NENodeFactory *factory );
    ~NEOSDRigidTopography();

    virtual bool isManipulatable(NEManipulator manipType);
    virtual vec2f osdSize();

    const NERigidTopography* rigidTopography() const;
    QOpenGLShaderProgram* getColorBarProgram();

public slots:
    //! Resets all properties totheir default values
    virtual void resetProperties();

protected:

    virtual bool addSource( NENode* node, NEConn& conn );
    virtual void removeSource( NENode* node, uint type, uint sIndex, uint tIndex );

    NERigidTopography* m_rigidTopography;
    //
    // OSD Related functions here...
    //

    QOpenGLShaderProgram* m_ColorBarProgram;

private:
    NE_DECL_PRIVATE(NEOSDRigidTopography)
};



class NEOSDRigidTopographyFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEOSDRigidTopographyFactory(NEManager* m);
    ~NEOSDRigidTopographyFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();

private:
    NEObject* createInstance();
};

