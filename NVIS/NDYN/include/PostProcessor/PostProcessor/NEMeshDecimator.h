#pragma once

#include "PostProcessor/NEMeshPostProcessor.h"
#include "Base/NENodeFactory.h"

class NECEXP_NEPOSTPROCESSOR NEMeshDecimator : public NEMeshPostProcessor
{
    Q_OBJECT
    Q_PROPERTY(NEMeshDecimatorMethod method READ decMethod WRITE setDecMethod)

    Q_PROPERTY(PropertyGroupStart SimpleDecimation READ groupStart)
    Q_PROPERTY(double TargetReduction READ targetReduction WRITE setTargetReduction)
    Q_PROPERTY(double FeatureAngle READ featureAngle WRITE setFeatureAngle)
    Q_PROPERTY(PropertyGroupEnd SimpleDecimation_End READ groupEnd)

    Q_PROPERTY(PropertyGroupStart QuadricDecimation READ groupStart)
    Q_PROPERTY(double QuadricTargetReduction READ quadricTargetReduction WRITE setQuadricTargetReduction)
    Q_PROPERTY(PropertyGroupEnd QuadricDecimation_End READ groupEnd)

    Q_PROPERTY(bool subdivision READ subdivision WRITE setSubdivision)

    Q_PROPERTY(PropertyGroupStart IsolatedPieceRemoval READ groupStart)
    Q_PROPERTY(bool isolatedPieceRemoval READ isolatedPieceRemoval WRITE setIsolatedPieceRemoval)
    Q_PROPERTY(double isolatedPieceDiameter READ isolatedPieceDiameter WRITE setIsolatedPieceDiameter)
    Q_PROPERTY(PropertyGroupEnd IsolatedPieceRemoval_End READ groupEnd)

    Q_ENUMS(NEMeshDecimatorMethod)

public:

    enum NEMeshDecimatorMethod
    {
        SimpleDecimation,
        QuadricDecimation,
        QuadricClustering
    };

    NEMeshDecimator();
    NEMeshDecimator(NENodeFactory* factory);
    virtual ~NEMeshDecimator();
    //! The main update method of the post processor, called from outside
    virtual  uint update();
    virtual  void deformMesh();

public slots:
    float featureAngle() const;
    void setFeatureAngle(float val);
    float targetReduction() const;
    void setTargetReduction(float val);

    float quadricTargetReduction() const;
    void setQuadricTargetReduction(float val);

    NEMeshDecimatorMethod decMethod() const;
    void setDecMethod(NEMeshDecimatorMethod value);

    bool subdivision() const;
    void setSubdivision(bool val);

    bool isolatedPieceRemoval() const;
    void setIsolatedPieceRemoval(bool val);

    float isolatedPieceDiameter() const;
    void setIsolatedPieceDiameter(float val);

private:
    float m_FeatureAngle;
    float m_TargetReduction;
    float m_QuadricTargetReduction;
    bool m_Subdivision;
    bool m_IsolatedPieceRemoval;
    float m_IsolatedPieceDiameter;

    NEMeshDecimatorMethod m_DecMethod;

    float m_FluidBodyMeshReduction;
    int m_FluidBodyTriangles;
};

class NEMeshDecimatorFactory : public NENodeFactory
{
public:
    NEMeshDecimatorFactory(NEManager* m);
    ~NEMeshDecimatorFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();
    NEObject* createInstance();
};
