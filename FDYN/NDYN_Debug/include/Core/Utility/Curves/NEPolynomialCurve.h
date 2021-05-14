#pragma once
#include "Utility/Curves/NECurve.h"

class NEPolynomialCurve : public NECurve
{
public:
	NEPolynomialCurve(int exponent);
	~NEPolynomialCurve(void);

    vec2f interpolate(double frameA, vec2f positionA, double frameB, vec2f positionB, double currentFrame);
	vec3f interpolate(double frameA, vec3f positionA, double frameB, vec3f positionB, double currentFrame);
	quatf interpolate(double frameA, quatf rotationA, double frameB, quatf rotationB, double currentFrame);
	bool interpolate(double frameA, bool flagA, double frameB, bool flagB, double currentFrame);
	QColor interpolate(double frameA, QColor colorA, double frameB, QColor colorB, double currentFrame);
	int interpolate(double frameA, int valA, double frameB, int valB, double currentFrame);
	double interpolate(double frameA, double valA, double frameB, double valB, double currentFrame);
	eulerf interpolate(double frameA, eulerf valA, double frameB, eulerf valB, double currentFrame);
private:
	int m_Exponent;
};


class NEQuadraticCurve : public NEPolynomialCurve
{
public:
	NEQuadraticCurve() : NEPolynomialCurve(2) { }
private:
	NE::NECurveType type() { return NE::kQuadratic; }
	QString name() { return QString("NEQuadraticCurve"); }
};

class NECubicCurve : public NEPolynomialCurve
{
public:
	NECubicCurve() : NEPolynomialCurve(3) { }
private:
	NE::NECurveType type() { return NE::kQuadratic; }
	QString name() { return QString("NECubicCurve"); }
};
