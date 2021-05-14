#pragma once

#ifdef INFINITY
#undef INFINITY
#define INFINITY numeric_limits<float>::infinity()
#endif


/*!
 * Interface for a 3-dimensional cell
 */
class NE3DCell
{
public:
	static const int N_DIMENSIONS = 3;
};


/*!
  This class represents a cell in a grid-based model
*/

class NEGridCell : public NE3DCell
{
public:

    NEGridCell(vec3f position, vec3f velocity = vec3f(std::INFINITY, std::INFINITY, std::INFINITY), float pressure = std::INFINITY) {
		m_Position = position;
		m_Velocity = velocity;
		m_Pressure = pressure;
	}

	inline const vec3f& position() const { return m_Position; }
	inline void setPosition(const vec3f value) { m_Position = value; }

	inline float pressure() const { return m_Pressure; }
	inline virtual void setPressure(const float value) { m_Pressure = value; }

	inline const vec3f& velocity() const { return m_Velocity; }
	inline void setVelocity(const vec3f velocity) {m_Velocity = velocity; }

	inline float velocity(const int dimension) const { return m_Velocity[dimension]; }
	inline void setVelocity(const int component, const float velocity) { m_Velocity[component] = velocity; }

	inline void addVelocity(int dimension, const float velChange);


protected:
	vec3f m_Position;
	float m_Pressure;
	vec3f m_Velocity;
};


void NEGridCell::addVelocity(int dimension, const float velChange) {
	#pragma omp atomic
	m_Velocity[dimension] += velChange;
}


