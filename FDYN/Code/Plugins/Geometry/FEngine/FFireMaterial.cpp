#include "FFireMaterial.h"

class FFireMaterialPrivate : public QObject
{
public:
    FFireMaterialPrivate(FFireMaterial* s) : S(s)
    {
    }

    /* Functions */

    /* Properties */
    int    m_ID;
    int    m_CarbonAtoms;
    int m_HydrogenAtoms;
    int m_OxygenAtoms;
    int m_NitrogenAtoms;
    int m_ChlorineAtoms;
    double m_HeatCombustion;
    double m_RadiativeFraction;

private:
    FFireMaterial* S;
};

FFireMaterialFactory::FFireMaterialFactory(NEManager* m) : NENodeFactory(m)
{
}

FFireMaterialFactory::~FFireMaterialFactory()
{
}

QString FFireMaterialFactory::nodeName()
{
    return "Fire Material";
}

uint FFireMaterialFactory::nodeType()
{
    return NE::kObjectPhysical;
}

QString FFireMaterialFactory::nodeInherits()
{
    return "Physical";
}

QString FFireMaterialFactory::objectName()
{
    return "Physical";
}

QString FFireMaterialFactory::nodeVersion()
{
    return "0.1";
}

QString FFireMaterialFactory::nodeVendor()
{
    return "F Dynamics";
}

QString FFireMaterialFactory::nodeHelp()
{
    return "Rigid and its physical properties";
}

NEObject* FFireMaterialFactory::createInstance()
{
    return new FFireMaterial(this);
}

NE_PROP_DEF(int, FFireMaterial, ID, 1)
NE_PROP_DEF(int, FFireMaterial, CarbonAtoms, 1)
NE_PROP_DEF(int, FFireMaterial, HydrogenAtoms, 4)
NE_PROP_DEF(int, FFireMaterial, OxygenAtoms, 0)
NE_PROP_DEF(int, FFireMaterial, NitrogenAtoms, 0)
NE_PROP_DEF(int, FFireMaterial, ChlorineAtoms, 0)
NE_PROP_DEF(double, FFireMaterial, HeatCombustion, 50000.0)
NE_PROP_DEF(double, FFireMaterial, RadiativeFraction, 0.35)

static void initProperties()
{
    NE_INIT_PROP(FFireMaterial, ID);
    NE_INIT_PROP(FFireMaterial, CarbonAtoms);
    NE_INIT_PROP(FFireMaterial, HydrogenAtoms);
    NE_INIT_PROP(FFireMaterial, OxygenAtoms);
    NE_INIT_PROP(FFireMaterial, NitrogenAtoms);
    NE_INIT_PROP(FFireMaterial, ChlorineAtoms);
    NE_INIT_PROP(FFireMaterial, HeatCombustion);
    NE_INIT_PROP(FFireMaterial, RadiativeFraction);
}

FFireMaterial::FFireMaterial(NENodeFactory *factory) : NENode(factory)
{
    P = new FFireMaterialPrivate(this);

    addNodeTypeToNodeGraphSceneType("Physical", NENode::SceneType::Simulation);

    CALL_ONCE(initProperties);
    P->m_ID = NE_DEFAULT(ID);
    P->m_CarbonAtoms = NE_DEFAULT(CarbonAtoms);
    P->m_HydrogenAtoms = NE_DEFAULT(HydrogenAtoms);
    P->m_OxygenAtoms = NE_DEFAULT(OxygenAtoms);
    P->m_NitrogenAtoms = NE_DEFAULT(NitrogenAtoms);
    P->m_ChlorineAtoms = NE_DEFAULT(ChlorineAtoms);
    P->m_HeatCombustion = NE_DEFAULT(HeatCombustion);
    P->m_RadiativeFraction = NE_DEFAULT(RadiativeFraction);
}

FFireMaterial::~FFireMaterial()
{
    delete P;
}

const int& FFireMaterial::ID() const
{
    return P->m_ID;
}

const int& FFireMaterial::CarbonAtoms() const
{
    return P->m_CarbonAtoms;
}

const int& FFireMaterial::HydrogenAtoms() const
{
    return P->m_HydrogenAtoms;
}

const int& FFireMaterial::NitrogenAtoms() const
{
    return P->m_NitrogenAtoms;
}

const int& FFireMaterial::OxygenAtoms() const
{
    return P->m_OxygenAtoms;
}

const int& FFireMaterial::ChlorineAtoms() const
{
    return P->m_ChlorineAtoms;
}

const double& FFireMaterial::HeatCombustion() const
{
    return P->m_HeatCombustion;
}

const double& FFireMaterial::RadiativeFraction() const
{
    return P->m_RadiativeFraction;
}

uint FFireMaterial::SetID(const int& val)
{
    NE_PROP_SET_EVAL(ID, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::SetCarbonAtoms(const int& val)
{
    NE_PROP_SET_EVAL(CarbonAtoms, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::SetHydrogenAtoms(const int& val)
{
    NE_PROP_SET_EVAL(HydrogenAtoms, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::SetOxygenAtoms(const int& val)
{
    NE_PROP_SET_EVAL(OxygenAtoms, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::SetNitrogenAtoms(const int& val)
{
    NE_PROP_SET_EVAL(NitrogenAtoms, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::SetChlorineAtoms(const int& val)
{
    NE_PROP_SET_EVAL(ChlorineAtoms, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::SetHeatCombustion(const double& val)
{
    NE_PROP_SET_EVAL(HeatCombustion, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::SetRadiativeFraction(const double& val)
{
    NE_PROP_SET_EVAL(RadiativeFraction, P, val);

    return NE::kReturnSuccess;
}

uint FFireMaterial::init(uint initMode)
{
    uint ret = NENode::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    return NE::kReturnSuccess;
}

void FFireMaterial::resetProperties()
{
    NENode::resetProperties();

    ResetCarbonAtoms();
    ResetHydrogenAtoms();
    ResetOxygenAtoms();
    ResetNitrogenAtoms();
    ResetChlorineAtoms();
    ResetHeatCombustion();
}

bool FFireMaterial::addSource(NENode* node, NEConn& conn)
{

    return NENode::addSource(node, conn);
}

void FFireMaterial::removeSource(NENode* node, uint type, uint sIndex, uint tIndex)
{

    return NENode::removeSource(node, type, sIndex, tIndex);
}
