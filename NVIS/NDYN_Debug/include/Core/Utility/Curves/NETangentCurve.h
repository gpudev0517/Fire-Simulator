#pragma once
#include "Utility/Curves/NECurve.h"

class NETangentCurve :public NECurve
{
public:
    NETangentCurve(void);
    ~NETangentCurve(void);

    NE::NECurveType type() { return NE::kTangent; }
    QString name() { return QString("NETangentCurve"); }
    vec2f interpolate(double frameA, vec2f positionA, double frameB, vec2f positionB, double currentFrame);
    vec3f interpolate(double frameA, vec3f positionA, double frameB, vec3f positionB, double currentFrame);
    quatf interpolate(double frameA, quatf rotationA, double frameB, quatf rotationB, double currentFrame);
    bool interpolate(double frameA, bool flagA, double frameB, bool flagB, double currentFrame);
    QColor interpolate(double frameA, QColor colorA, double frameB, QColor colorB, double currentFrame);
    int interpolate(double frameA, int valA, double frameB, int valB, double currentFrame);
    double interpolate(double frameA, double valA, double frameB, double valB, double currentFrame);
    eulerf interpolate(double frameA, eulerf valA, double frameB, eulerf valB, double currentFrame);
};

