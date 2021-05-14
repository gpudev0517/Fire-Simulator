#pragma once

#include "Utility/Singleton.h"
#include "Spatial/NESpatial.h"

#include "Rendering/Manager/NEGLManager.h"



NE_FWD_DECL_6(class, NETriMesh, NEIndexedTriMesh, NEMaterial, NERenderManager, NEDirectionalLight, NEPointLight)
NE_FWD_DECL_5(class, NESpotLight, NEVolume, NESpatial, NE_PRIVATE(NEGLRenderer), NEClipPlane)

//! The GL renderer provides some utility functions for rendering common entity types
/*!
    This class uses the functionality inside the NEGLManager to do all the rendering.
*/

class QOpenGLTexture;
class NESpatial;
class NEOSDBase;
class IGLSurface;

class NECEXP_NERENDERING NEGLRenderer final : public QObject
{
    Q_OBJECT;

public:

    //! For drawing the volume without setting up any material (faster)
    void drawRaw(const AABB& aabb);

    //-----------------------------------------------------------------
    //      Define
    //-----------------------------------------------------------------
    enum RenderOutput
    {
        Lighting = 1,
        Depth = 2,
        Ambient = 4,
        Normal = 5
    };

    enum ePRENDERER_PASS
    {
        // Render Pass
        DPASS_SHADOW = 0,   //!< Shadow Map
        DPASS_MAIN,         //!< Main
        DPASS_SCREEN,       //!< Screen
        DPASS_SYSTEM,       //!< System
        DPASS_MAX,

        // Post Filter ID
        FILTER_FXAA = 0,
        FILTER_SSAO
    };

    void renderPicked(NESpatial* spatial, IGLSurface* surface);
    void renderPickable(NESpatial* spatial, IGLSurface* surface);
    void renderShadow(NESpatial* spatial, IGLSurface* surface);

    //! Initializes the class members and shaders
    bool init();
    void deInit();

    NE_DECL_PRIVATE_SINGLETON(NEGLRenderer)
};

Q_DECLARE_INTERFACE(NEGLRenderer, "NEGLRenderer")

