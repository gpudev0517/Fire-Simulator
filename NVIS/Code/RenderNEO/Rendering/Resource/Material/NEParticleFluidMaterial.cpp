#include "Rendering/Resource/Material/NEParticleFluidMaterial.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"

#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"

#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"


NEParticleFluidMaterial::NEParticleFluidMaterial(NENodeFactory* factory) : NETransparentMaterial(factory)
{

    if (NEBASE.guiMode())
    {
        setShaderProgram(new QOpenGLShaderProgram());
//        QString materialPath = "System/Shaders/Material/PerPixel/fluid.frag";
//        if(!shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment, materialPath))
//        {
//            qWarning() << materialPath <<  " " << shaderProgram()->log();
//            std::cerr << materialPath.toStdString() << " " << shaderProgram()->log().toStdString() << std::endl;
//        }

        setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
        if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/fluid.frag") == false )
        {
            qWarning() << " fluid.frag : " << defaultFS()->log();
            std::cerr << " fluid.frag : "  << defaultFS()->log().toStdString() << std::endl;
        }
        shaderProgram()->addShader(defaultFS());
    }

    setInputs(inputs() | NE::kIOParticles);

    m_smoothingRadius = 0.5f;
    m_maxRadialSamples = 5.0f;
    m_depthEdgeFalloff = 0.01f;
    m_depthBlurFalloff = 5.5f;
    m_thicknessParticleScale = 4.0f;
    m_depthParticleScale = 1.5f;

    m_fresnelStrength = 1.0f;
    m_RenderOptLevel = 0;
    m_WaterRendering = false;
    m_flatShading = false;
}

NEParticleFluidMaterial::~NEParticleFluidMaterial()
{
    // To prevent crashes on Mac
    if(!NeutrinoInitialized())
        return;


    if (NEBASE.guiMode() && shaderProgram())
        shaderProgram()->removeAllShaders();

}



const double& NEParticleFluidMaterial::SmoothingRadius() const { return m_smoothingRadius; }
uint NEParticleFluidMaterial::SetSmoothingRadius( const double& val ) { m_smoothingRadius = val; return 1; }
void NEParticleFluidMaterial::ResetSmoothingRadius() { m_smoothingRadius = 0.5f; return; }


const double& NEParticleFluidMaterial::MaxRadialSamples() const { return m_maxRadialSamples; }
uint NEParticleFluidMaterial::SetMaxRadialSamples(const double& val) { m_maxRadialSamples = val; return 1;}
void NEParticleFluidMaterial::ResetMaxRadialSamples() { m_maxRadialSamples = 5.0f; return; }


const double& NEParticleFluidMaterial::DepthEdgeFalloff() const { return m_depthEdgeFalloff; }
uint NEParticleFluidMaterial::SetDepthEdgeFalloff(const double& val) { m_depthEdgeFalloff = val; return 1;}
void NEParticleFluidMaterial::ResetDepthEdgeFalloff() {  m_depthEdgeFalloff = 0.01f;; return; }


const double& NEParticleFluidMaterial::DepthBlurFalloff() const { return m_depthBlurFalloff; }
uint NEParticleFluidMaterial::SetDepthBlurFalloff(const double& val) { m_depthBlurFalloff = val; return 1;}
void NEParticleFluidMaterial::ResetDepthBlurFalloff() {  m_depthBlurFalloff = 5.5f; return; }

const double& NEParticleFluidMaterial::ThicknessParticleScale() const { return m_thicknessParticleScale; }
uint NEParticleFluidMaterial::SetThicknessParticleScale(const double& val) { m_thicknessParticleScale = val;return 1; }
void NEParticleFluidMaterial::ResetThicknessParticleScale() {  m_thicknessParticleScale = 4.0f; return; }

const double& NEParticleFluidMaterial::DepthParticleScale() const { return m_depthParticleScale; }
uint NEParticleFluidMaterial::SetDepthParticleScale(const double& val) { m_depthParticleScale = val; return 1; }
void NEParticleFluidMaterial::ResetDepthParticleScale() {  m_depthParticleScale = 1.5f; return; }

const double& NEParticleFluidMaterial::FresnelStrength() const { return m_fresnelStrength; }
uint NEParticleFluidMaterial::SetFresnelStrength(const double& val) { m_fresnelStrength = val > 1.0 ? 1.0 : val < 0 ? 0 : val; return 1; }
void NEParticleFluidMaterial::ResetFresnelStrength() {   m_fresnelStrength = 1.0f; return; }

const bool& NEParticleFluidMaterial::WaterRendering() const { return m_WaterRendering; }
uint NEParticleFluidMaterial::SetWaterRendering(const bool& val) { m_WaterRendering = val; return 1; }
void NEParticleFluidMaterial::ResetWaterRendering() {   m_WaterRendering = false; return; }

const int& NEParticleFluidMaterial::RenderOptLevel() const { return m_RenderOptLevel; }
uint NEParticleFluidMaterial::SetRenderOptLevel(const int& val) { m_RenderOptLevel = val; return 1; }
void NEParticleFluidMaterial::ResetRenderOptLevel() {   m_RenderOptLevel = false; return; }

const bool& NEParticleFluidMaterial::FlatShading() const { return m_flatShading; }
uint NEParticleFluidMaterial::SetFlatShading(const bool& val)
{

    for( NESpatial* object : NESCENE.spatials() )
    {
        NEParticleFluidSolver* fluidSource = qobject_cast< NEParticleFluidSolver *>( object );
        if(fluidSource)
        {
            NESpatialRenderer* renderer = NERENDERMAN.getRenderer(fluidSource);
            if( renderer->FluidMaterial() == this)
            {
                if( val)
                    fluidSource->SetRenderMode(NESpatial::FlatShaded);
                else
                    fluidSource->SetRenderMode(NESpatial::SmoothShaded);
            }
        }

    }
    m_flatShading = val;
    return 1;
}
void NEParticleFluidMaterial::ResetFlatShading() {   m_flatShading = false; return; }

void NEParticleFluidMaterial::updateUniforms()
{
    NEMaterial::updateUniforms();

    // Fluid Rendering
    shaderProgram()->setUniformValue("Shininess", (float)Shininess());
    shaderProgram()->setUniformValue("fresnelStrength", (float)FresnelStrength());
}

NEMaterial::MaterialType NEParticleFluidMaterial::materialType()
{
    return NEMaterial::Water;
}

bool NEParticleFluidMaterial::addSource(NENode *node, NENode::NEConn &conn)
{
    NEParticleFluidSolver* fluidSource = qobject_cast< NEParticleFluidSolver *>( node );
    if ( fluidSource )
    {
        NESpatialRenderer* renderer = NERENDERMAN.getRenderer(fluidSource);
        if(renderer)
        {
            if( renderer->FluidMaterial() )
            {
                //check if connection already exists
                for(const NEConn& eraseConn : node->connections())
                {
                    if( eraseConn.type == NE::kIOParticles &&
                            eraseConn.target != this &&
                            qobject_cast<NEMaterial*> (eraseConn.target))
                    {
                        eraseConn.target->disconnectFrom( eraseConn.source, NE::kIOParticles, eraseConn.sIndex, eraseConn.tIndex );
                    }
                }
            }
            renderer->setFluidMaterial(this);
        }
        return NENode::addSource( node, conn );
    }
    return false;
}

void NEParticleFluidMaterial::removeSource(NENode *node, uint type, uint sIndex, uint tIndex)
{
    NEParticleFluidSolver* fluidSource = qobject_cast< NEParticleFluidSolver *>( node );
    if ( fluidSource )
    {
        NESpatialRenderer* renderer = NERENDERMAN.getRenderer(fluidSource);
        if(renderer)
            renderer->setFluidMaterial(NULL);
        NENode::removeSource( node, type, sIndex, tIndex );
    }

    return;
}



NEParticleFluidMaterialFactory::NEParticleFluidMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEParticleFluidMaterialFactory::~NEParticleFluidMaterialFactory() {}

QString NEParticleFluidMaterialFactory::nodeName() { return "Fluid Material"; }

uint NEParticleFluidMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEParticleFluidMaterialFactory::nodeInherits() { return "Materials";}

QString NEParticleFluidMaterialFactory::objectName() { return "ParticleFluid"; }

QString NEParticleFluidMaterialFactory::nodeVersion() { return "0.1"; }

QString NEParticleFluidMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEParticleFluidMaterialFactory::nodeHelp() { return "Fluid material"; }

NEObject*NEParticleFluidMaterialFactory::createInstance() { return new NEParticleFluidMaterial(this); }
