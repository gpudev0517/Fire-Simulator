#pragma once


class NECurve
{
public:
	virtual NE::NECurveType type() = 0;
	virtual QString name() = 0;
    virtual vec2f interpolate(double frameA, vec2f positionA, double frameB, vec2f positionB, double currentFrame) = 0;
	virtual vec3f interpolate(double frameA, vec3f positionA, double frameB, vec3f positionB, double currentFrame) = 0;
	virtual quatf interpolate(double frameA, quatf rotationA, double frameB, quatf rotationB, double currentFrame) = 0;
	virtual bool interpolate(double frameA, bool flagA, double frameB, bool flagB, double currentFrame) = 0;
	virtual QColor interpolate(double frameA, QColor colorA, double frameB, QColor colorB, double currentFrame) = 0;
	virtual int interpolate(double frameA, int valA, double frameB, int valB, double currentFrame) = 0;
	virtual double interpolate(double frameA, double valA, double frameB, double valB, double currentFrame) = 0;
	virtual eulerf interpolate(double frameA, eulerf valA, double frameB, eulerf valB, double currentFrame) = 0;

	//! Convenience Interpolation method for QVariant data. Checks the actual datatype and call the 
	//! corresponding implementation of NECurve::interpolate()
	QVariant interpolate(double frameA, QVariant valA, double frameB, QVariant valB, double currentFrame)
	{
		QVariant retVal;

		switch((int)valA.type())
		{
		case QMetaType::Float:
        {
            retVal = interpolate(frameA, valA.toFloat(), frameB, valB.toFloat(), currentFrame);
			break;
		}
		case QVariant::Double:
		{
			retVal = interpolate(frameA, valA.toDouble(), frameB, valB.toDouble(), currentFrame);
			break;
		}
		case QVariant::Int:
		{
			retVal = interpolate(frameA, valA.toInt(), frameB, valB.toInt(), currentFrame);
			break;
        }
#undef Bool //Workaround
		case QVariant::Bool:
		{
			retVal = interpolate(frameA, valA.toBool(), frameB, valB.toBool(), currentFrame);
			break;
		}
		case QVariant::Color:
		{
			retVal = interpolate(frameA, valA.value<QColor>(), frameB, valB.value<QColor>(), currentFrame);
			break;
		}
		case QVariant::UserType:
		{
			uint usertype = valA.userType();
			
            if(usertype == getNEVarIds().vec2fId)
            {
                retVal = interpolate(frameA, valA.value<vec2f>(), frameB, valB.value<vec2f>(), currentFrame);
            }
            else if(usertype == getNEVarIds().vec3fId)
            {
                retVal = interpolate(frameA, valA.value<vec3f>(), frameB, valB.value<vec3f>(), currentFrame);
			}
            else if(usertype == getNEVarIds().eulerfId)
            {
				retVal = interpolate(frameA, valA.value<eulerf>(), frameB, valB.value<eulerf>(), currentFrame);
			}
		}
		break;

		default: return retVal;
			break;
		}

		return retVal;
	}
};
