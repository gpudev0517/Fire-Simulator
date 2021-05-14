#pragma once

#include "Rendering/Spatial/NELight.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEDirectionalLight))

class IGLSurface;

class NECEXP_NERENDERING NEDirectionalLight : public NELight
{
	Q_OBJECT
	NE_CLASS("Directional Light")

	NE_PROP_3D_RW(vec3f, float, Direction, "Direction", "The direction of the light", true,)\

public:
	NEDirectionalLight(NENodeFactory* factory);
	virtual ~NEDirectionalLight();
	uint init(uint initMode = NE::kInitOnCreate);
    virtual void updateUniforms(IGLSurface* surface, QOpenGLShaderProgram* program, int lightID, bool isActive);
    virtual void propertyTweak(QtPropertyBase* p);
	const NEIndexedTriMesh* mesh() const;

protected:
	NE_DERIVED(NESpatial, Protected)

public slots:
    uint SetOrientation(const eulerf &rotation);

private:
    NE_DECL_PRIVATE(NEDirectionalLight)
};


#include "Base/NENodeFactory.h"

class NEDirectionalLightFactory : public NENodeFactory
{
public:
	NEDirectionalLightFactory(NEManager* m);
	~NEDirectionalLightFactory();
	NE_DERIVED(NENodeFactory, Public)

	const NEIndexedTriMesh* mesh();

private:
	NEIndexedTriMesh* m_pMesh;
};
