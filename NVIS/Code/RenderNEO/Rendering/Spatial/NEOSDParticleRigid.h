#pragma once

#include "Rendering/Spatial/NEOSDBase.h"

class NEParticleRigid;
class NEOSDParticleRigidPrivate;
//
// Move all particle rigid related OSD code here

class NEOSDParticleRigid: public NEOSDBase
{
    Q_OBJECT
    NE_CLASS("Particle Rigid OSD")

public:
    enum NEOSDOrientation {Left = 0, Right = 1, Top = 2, Bottom = 3};

    NE_PROP_RW(QString, TitleText, "Title Text", "Title Text", true,)
    NE_PROP_RW(int, TickCount, "Tick Count", "Tick Count of color stages", true,)
    NE_PROP_RW(NEOSDOrientation, OSDOrientation, "Orientation", "Indicates the orientation of the OSD.", true,)

    Q_ENUMS(NEOSDOrientation)

public:
    NEOSDParticleRigid( NENodeFactory* factory );
    ~NEOSDParticleRigid();

    virtual bool isManipulatable(NEManipulator manipType);
    const NEParticleRigid* particleRigid() const;
    virtual vec2f osdSize();
    QOpenGLShaderProgram* getColorBarProgram();

public slots:
    //! Resets all properties totheir default values
    virtual void resetProperties();

protected:

    virtual bool addSource( NENode* node, NEConn& conn );
    virtual void removeSource( NENode* node, uint type, uint sIndex, uint tIndex );

    NEParticleRigid* m_particleRigid;
    //
    // OSD Related functions here...
    //
    QOpenGLShaderProgram* m_ColorBarProgram;

private:
    NE_DECL_PRIVATE(NEOSDParticleRigid)

};

#include "Base/NENodeFactory.h"

class NEOSDParticleRigidFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEOSDParticleRigidFactory(NEManager* m);
    ~NEOSDParticleRigidFactory();

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


