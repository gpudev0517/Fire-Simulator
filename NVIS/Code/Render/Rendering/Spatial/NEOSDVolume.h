#pragma once

#include "NEOSDBase.h"

class NEVolumeNode;
class NEVolumeNodeRenderer;
class NEOSDVolumePrivate;

//
// Move all volume related rendering/visualization code here..
//

class NEOSDVolume: public NEOSDBase
{
    Q_OBJECT
    NE_CLASS("Volume OSD")

public:
    enum NEOSDOrientation {Left = 0, Right = 1, Top = 2, Bottom = 3};

    NE_PROP_RW(QString, TitleText, "Title Text", "Title Text", true,)
    NE_PROP_RW(int, TickCount, "Tick Count", "Tick Count of color stages", true,)
    NE_PROP_RW(NEOSDOrientation, OSDOrientation, "Orientation", "Indicates the orientation of the OSD.", true,)

    Q_ENUMS(NEOSDOrientation)

public:
    NEOSDVolume(NENodeFactory *factory);
    ~NEOSDVolume();

    virtual bool isManipulatable(NEManipulator manipType);
    virtual vec2f osdSize();

    QOpenGLShaderProgram* getColorBarProgram();
    NESpatial* Volume();
    QString getTitleText();

    NESpatial* m_volume;

public slots:
    //! Resets all properties totheir default values
    virtual void resetProperties();

protected:

    virtual bool addSource( NENode* node, NEConn& conn );
    virtual void removeSource( NENode* node, uint type, uint sIndex, uint tIndex );

    QOpenGLShaderProgram* m_ColorBarProgram;

private:
    NE_DECL_PRIVATE(NEOSDVolume)
};


#include "Base/NENodeFactory.h"

class NEOSDVolumeFactory : public NENodeFactory
{
	Q_OBJECT
	Q_INTERFACES(NENodeFactory)

public:
	NEOSDVolumeFactory(NEManager* m);
	~NEOSDVolumeFactory();

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
