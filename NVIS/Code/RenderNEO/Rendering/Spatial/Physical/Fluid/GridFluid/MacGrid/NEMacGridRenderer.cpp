/*
 * NEMacGridRenderer.cpp
 *
 *  Created on: 08.08.2012
 *      Author: andi
 */

#include "Rendering/Rendering/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacGridRenderer.h"
#include "Physics/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacCell.h"
#include "Physics/Spatial/Physical/Fluid/GridFluid/MacGrid/NEFlipCell.h"


#include "UI/Rendering/NEGLRenderer.h"


//#include <math.h>

using namespace std;


template <typename GridType>
NEMacGridRenderer<GridType>::NEMacGridRenderer(const GridType& grid) : m_Grid(grid)
{
}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawCells(NEStaggeredGridRenderOptions::NECellDrawing mode)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int ci = 0; ci < (int)m_Grid.totalNrOfCells(); ci++)
	{
		const CellId cell = m_Grid.cell(ci);

		bool shouldDraw = false;

		Color32 color;

		if (m_Grid.containsFluid(cell))
		{
			if (mode == NEStaggeredGridRenderOptions::NECellDrawing::Fluid || mode == NEStaggeredGridRenderOptions::NECellDrawing::AllActive)
			{
				float blendingFactor = solidBlendingFactor(cell);
				color = Color32(blendingFactor * 0xFF, 0, (1 - blendingFactor) * 0xFF, 0xFF);
				shouldDraw = true;
			}
		}
		else
		{
			float blendingFactor = solidBlendingFactor(cell);
			
			color = Color32(blendingFactor * 0xFF, (1 - blendingFactor) * 0xFF, 0, 0xFF);
			
			shouldDraw =    mode == NEStaggeredGridRenderOptions::NECellDrawing::AllActive
						|| (mode == NEStaggeredGridRenderOptions::NECellDrawing::Air && blendingFactor < 1)
						|| (mode == NEStaggeredGridRenderOptions::NECellDrawing::Solid && blendingFactor > 0);
		}

		if (shouldDraw) {
			glColor4ubv((GLubyte*)&color);
			drawCell(m_Grid.position(cell));
		}
	}
	glEnd();
}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawVelocities(NEVelocityComponentDrawing velocityComponentMode, bool velocityVectorMode, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent)
{
	if (velocityComponentMode != NEVelocityComponentDrawing::cNone)
		drawVelocityComponents(velocityComponentMode, minColoringRange, maxColoringRange, minRangeColor, maxRangeColor, coloringExponent);

	if (velocityVectorMode)
		drawCellVelocityVectors(minColoringRange, maxColoringRange, minRangeColor, maxRangeColor, coloringExponent);
}

template <typename GridType>
void NEMacGridRenderer<GridType>::drawVelocityComponents(NEStaggeredGridRenderOptions::NEVelocityComponentDrawing mode, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent)
{
	NERENDERMAN.GLManager()->beginParticleDraw(m_Grid.cellSize() / 30, false, false, false);
	glBegin(GL_POINTS);

	for (int ci = 0; ci < (int)m_Grid.totalNrOfVelocityComponents(); ci++)
	{
		const VelocityComponentId& component = m_Grid.velocityComponent(ci);

		int dimension = m_Grid.dimension(component);

		if (mode == NEStaggeredGridRenderOptions::NEVelocityComponentDrawing::All || mode == NEStaggeredGridRenderOptions::NEVelocityComponentDrawing::X + dimension)
		{
			vec4f color = colorByVelocity(m_Grid.velocity(component), minColoringRange, maxColoringRange, minRangeColor, maxRangeColor, coloringExponent);
			glColor4f(color.x(), color.y(), color.z(), color.w());

			vec3f pos = m_Grid.position(component);

			glPushMatrix ();
			glTranslatef (pos.x(), pos.y(), pos.z());
			glVertex3f(pos.x(), pos.y(), pos.z());
			glPopMatrix ();
		}
	}

	glEnd();
    NERENDERMAN.GLManager()->endParticleDraw();


	NERENDERMAN.GLManager()->beginUnLitDraw();
	glColor3f(1.0f,0.0f,1.0f);
	glBegin(GL_LINES);


	for (int ci = 0; ci < (int)m_Grid.totalNrOfVelocityComponents(); ci++)
	{
		const VelocityComponentId& component = m_Grid.velocityComponent(ci);

		int dimension = m_Grid.dimension(component);

		if (mode == NEStaggeredGridRenderOptions::NEVelocityComponentDrawing::All || mode == NEStaggeredGridRenderOptions::NEVelocityComponentDrawing::X + dimension)
		{
			vec4f color = colorByVelocity(m_Grid.velocity(component), minColoringRange, maxColoringRange, minRangeColor, maxRangeColor, coloringExponent);
			glColor4f(color.x(), color.y(), color.z(), color.w());

			const float scaling = m_Grid.cellSize() * sqrt(1.0f / maxColoringRange);

			vec3f pos = m_Grid.position(component);
			glVertex3fv((float*)&pos);
			vec3f pos2 = pos;
			pos2[dimension] += m_Grid.velocity(component) * scaling;
			glVertex3fv((float*)&pos2);
		}
	}

	glEnd();
	NERENDERMAN.GLManager()->endUnLitDraw();
}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawCellVelocityVectors(float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent)
{
	NERENDERMAN.GLManager()->beginUnLitDraw();
	glColor3f(1.0f,0.0f,1.0f);
	glBegin(GL_LINES);

	for (int ci = 0; ci < (int)m_Grid.totalNrOfCells(); ci++)
	{
		const CellId cell = m_Grid.cell(ci);

		vec3f pos = m_Grid.position(cell);
		vec3f vel = m_Grid.interpolateVelocity(pos);

		vec4f color = colorByVelocity(vel.length(), minColoringRange, maxColoringRange, minRangeColor, maxRangeColor, coloringExponent);
		glColor4f(color.x(), color.y(), color.z(), color.w());


		const float scaling = m_Grid.cellSize() * sqrt(1 / maxColoringRange);

		glVertex3fv((float*)&pos);
		vec3f pos2 = pos + scaling * vel;
		glVertex3fv((float*)&pos2);
	}

	glEnd();
	NERENDERMAN.GLManager()->endUnLitDraw();
}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawPressure(NEPressureDrawing mode, float minPressure, float maxPressure, float atmosphericPressure, QColor minRangeColor, QColor maxRangeColor)
{
	vec3f minCol(minRangeColor.red() / 255.0f, minRangeColor.green() / 255.0f, minRangeColor.blue() / 255.0f);
	vec3f maxCol(maxRangeColor.red() / 255.0f, maxRangeColor.green() / 255.0f, maxRangeColor.blue() / 255.0f);

	vec3f compCol = vec3f(1, 1, 1) - maxCol;

	NERENDERMAN.GLManager()->beginParticleDraw(m_Grid.cellSize() / 10, false, false, false);
	glBegin(GL_POINTS);

	for (int ci = 0; ci < (int)m_Grid.totalNrOfCells(); ci++)
	{
		const CellId cell = m_Grid.cell(ci);
		float pressure = m_Grid.pressure(cell);
		vec3f position = m_Grid.position(cell);

		if (mode == NEPressureDrawing::pAll || mode == NEPressureDrawing::NonGhost)
			drawPressure(pressure, position, minPressure, maxPressure, atmosphericPressure, minCol, maxCol, compCol);

		// render ghost pressure
		for (int d = 0; d < GridType::N_DIMENSIONS; d++)
		{
			if (mode == NEPressureDrawing::pAll || mode == NEPressureDrawing::GhostAll || mode == NEPressureDrawing::pAll || mode == NEPressureDrawing::GhostX + d)
			{
				float lowerGhostPressure = m_Grid.ghostPressure(cell, d, Direction::Lower);
				vec3f lowerGhostPos = position;
				lowerGhostPos[d] -= m_Grid.cellSize() / 6;
				drawPressure(lowerGhostPressure, lowerGhostPos, minPressure, maxPressure, atmosphericPressure, minCol, maxCol, compCol);

				float upperGhostPressure = m_Grid.ghostPressure(cell, d, Direction::Upper);
				vec3f upperGhostPos = position;
				upperGhostPos[d] += m_Grid.cellSize() / 6;
				drawPressure(upperGhostPressure, upperGhostPos, minPressure, maxPressure, atmosphericPressure, minCol, maxCol, compCol);
			}
		}
	}
	glEnd();
	NERENDERMAN.GLManager()->endParticleDraw();
}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawPressure(float pressure, vec3f position, float minPressure, float maxPressure, float atmosphericPressure, vec3f minColor, vec3f maxColor, vec3f compColor)
{

	if (pressure != atmosphericPressure && pressure < INFINITY)
	{
		vec3f color;
		if (pressure >= 0)
		{
			color = minColor + (maxColor - minColor) * (pressure / maxPressure);
		}
		else
		{
			color = minColor + (compColor - minColor) * (pressure / minPressure);
		}

		glColor3f(color.x(), color.y(), color.z());

		glPushMatrix ();
		glTranslatef (position.x(), position.y(), position.z());
		glVertex3f(position.x(), position.y(), position.z());
		glPopMatrix ();
	}

}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawFluidFractions(NEVelocityComponentDrawing mode)
{

	NERENDERMAN.GLManager()->beginParticleDraw(m_Grid.cellSize() / 15, false, false, false);
	glBegin(GL_POINTS);

	for (int ci = 0; ci < (int)m_Grid.totalNrOfVelocityComponents(); ci++)
	{
		const VelocityComponentId& component = m_Grid.velocityComponent(ci);
		int dimension = m_Grid.dimension(component);

		if (mode == NEVelocityComponentDrawing::All || mode == NEVelocityComponentDrawing::X + dimension)
		{
			float volume = m_Grid.fluidFraction(component);
			glColor4f(1 - volume, 1 - volume, 1, 1);

			vec3f pos = m_Grid.position(component);
			glPushMatrix ();

			glTranslatef (pos.x(), pos.y(), pos.z());
			glVertex3f(pos.x(), pos.y(), pos.z());
			glPopMatrix ();
		}
	}
	glEnd();
	NERENDERMAN.GLManager()->endParticleDraw();

}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawDistanceVectors(NEDistanceVectorDrawing mode)
{

	if (mode == NEStaggeredGridRenderOptions::NEDistanceVectorDrawing::vNone)
		return;

	NERENDERMAN.GLManager()->beginUnLitDraw();
	glColor3f(1.0f,0.0f,1.0f);
	glBegin(GL_LINES);

	for (int ci = 0; ci < (int)m_Grid.totalNrOfCells(); ci++)
	{
		const CellId cell = m_Grid.cell(ci);

		const set<const NERigid*> rigids = this->m_Grid.rigids(cell);
		for (set<const NERigid*>::const_iterator ri = rigids.begin(); ri != rigids.end(); ri++)
		{
			const multiset<DistanceInfo>& distances = this->m_Grid.distances(*ri, cell);

			for (multiset<DistanceInfo>::const_iterator di = distances.begin(); di != distances.end(); di++)
			{
				float signedDistance = di->signedDistance();

				if (mode == NEStaggeredGridRenderOptions::NEDistanceVectorDrawing::vAll
						|| (mode == NEStaggeredGridRenderOptions::NEDistanceVectorDrawing::Inside && signedDistance <= 0)
						|| (mode == NEStaggeredGridRenderOptions::NEDistanceVectorDrawing::Outside && signedDistance >= 0) )
				{

					vec3f pos = m_Grid.position(cell);

					if (signedDistance >= 0)
						glColor3f(1.0f, 0.0f, 1.0f);
					else
						glColor3f(1.0f ,1.0f, 0.0f);

					vec3f pos2 = pos + di->distanceVector();

					glVertex3fv((float*)&pos);
					glVertex3fv((float*)&pos2);
				}
			}
		}
	}

	glEnd();
	NERENDERMAN.GLManager()->endUnLitDraw();

}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawVelocityComponents(const std::vector<VelocityComponentId>& components)
{

	NERENDERMAN.GLManager()->beginParticleDraw(m_Grid.cellSize() / 30, false, false, false);
	glBegin(GL_POINTS);

	for (int ci = 0; ci < (int)components.size(); ci++)
	{
		const VelocityComponentId& component = components.at(ci);

		vec3f pos = m_Grid.position(component);

		glColor3f(0, 1, 1);
		glPushMatrix ();
		glTranslatef (pos.x(), pos.y(), pos.z());
		glVertex3f(pos.x(), pos.y(), pos.z());
		glPopMatrix ();
	}

	glEnd();
	NERENDERMAN.GLManager()->endParticleDraw();

}


template <typename GridType>
vec4f NEMacGridRenderer<GridType>::colorByVelocity(float velocity, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent)
{
	vec4f color(0, 0, 0, 1);

	vec3f minCol(minRangeColor.red(), minRangeColor.green(), minRangeColor.blue());
	vec3f maxCol(maxRangeColor.red(), maxRangeColor.green(), maxRangeColor.blue());
	vec3f colDiff = maxCol - minCol;
	float maxVel2Inv = 1.0f / maxColoringRange;

	float vel2 = pow(velocity, 2);
	if(vel2 <= minColoringRange)
		color = vec4f(minRangeColor.red() / 255.0f, minRangeColor.green() / 255.0f, minRangeColor.blue() / 255.0f, minRangeColor.alpha() / 255.0f);
	else if(vel2 > maxColoringRange)
		color = vec4f(maxRangeColor.red() / 255.0f, maxRangeColor.green() / 255.0f, maxRangeColor.blue() / 255.0f, maxRangeColor.alpha() / 255.0f);
	else
	{
		float colorValue = powf(vel2 * maxVel2Inv, coloringExponent);
		vec3f tmpCol = minCol + colDiff*colorValue;
		color = vec4f(tmpCol.x() / 255.0f, tmpCol.y() / 255.0f, tmpCol.z() / 255.0f, 1);
	}
	return color;
}


template <typename GridType>
vec4f NEMacGridRenderer<GridType>::colorByPressure(float pressure, float minPressure, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent)
{
	vec4f color(0, 0, 0, 1);

	vec3f minCol(minRangeColor.red(), minRangeColor.green(), minRangeColor.blue());
	vec3f maxCol(maxRangeColor.red(), maxRangeColor.green(), maxRangeColor.blue());
	vec3f colDiff = maxCol - minCol;
	float maxPress2Inv = 1.0f / (maxColoringRange - minColoringRange);

	float press2 = pow(pressure - minPressure, 2);
	if(press2 <= minColoringRange)
		color = vec4f(minRangeColor.red() / 255.0f, minRangeColor.green() / 255.0f, minRangeColor.blue() / 255.0f, minRangeColor.alpha() / 255.0f);
	else if(press2 > maxColoringRange)
		color = vec4f(maxRangeColor.red() / 255.0f, maxRangeColor.green() / 255.0f, maxRangeColor.blue() / 255.0f, maxRangeColor.alpha() / 255.0f);
	else
	{
		float colorValue = powf(press2 * maxPress2Inv, coloringExponent);
		vec3f tmpCol = minCol + colDiff*colorValue;
		color = vec4f(tmpCol.x() / 255.0f, tmpCol.y() / 255.0f, tmpCol.z() / 255.0f, 1);
	}

	return color;
}


template <typename GridType>
void NEMacGridRenderer<GridType>::drawCell(vec3f position)
{
	float halfCellSize = m_Grid.cellSize() / 2;

	static const GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
			{0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
			{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };

	GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertices. */

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = position.x() - halfCellSize;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = position.x() + halfCellSize;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = position.y() - halfCellSize;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = position.y() + halfCellSize;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = position.z() - halfCellSize;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = position.z() + halfCellSize;

	for (int i = 0; i < 6; i++)
	{
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
	}
}


template <typename GridType>
float NEMacGridRenderer<GridType>::solidBlendingFactor(const CellId& cell)
{
	return 1 - m_Grid.fluidFraction(cell);
}


// template instantiation

#include "Physics/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacSolverGrid.h"
#include "Physics/Spatial/Physical/Fluid/GridFluid/MacGrid/NEFlipSolverGrid.h"
#include "Physics/Spatial/Physical/Fluid/GridFluid/MacGridFluid/NEFog_MacSolverGrid.h"
#include "Physics/Spatial/Physical/Fluid/GridFluid/MacGridFluid/NEFog_FlipSolverGrid.h"

TEMPLATE_INSTANCE NEMacGridRenderer<NEMacSolverGrid>;
TEMPLATE_INSTANCE NEMacGridRenderer<NEFlipSolverGrid<NEFlipCell> >;
TEMPLATE_INSTANCE NEMacGridRenderer<NEFog_MacSolverGrid>;
TEMPLATE_INSTANCE NEMacGridRenderer<NEFog_FlipSolverGrid>;
