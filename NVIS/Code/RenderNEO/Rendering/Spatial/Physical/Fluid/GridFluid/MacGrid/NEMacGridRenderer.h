#pragma once

#include "Grid/Physics/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacGridCellRepository.h"
#include "Grid/Physics/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacSolverGrid.h"

/**
 * Auxiliary class for defining the enums, since Qt does not
 * allow to define them in template classes.
 */
class NEStaggeredGridRenderOptions : QObject
{
    Q_OBJECT

public:

    enum NECellDrawing
    {
        dNone,
        AllActive,
        Fluid,
        Air,
        Solid
    };

    enum NEPressureDrawing
    {
        pNone,
        pAll,
        NonGhost,
        GhostAll,
        GhostX,
        GhostY,
        GhostZ
    };

    enum NEVelocityComponentDrawing
    {
        cNone = 0,
        All,
        X,
        Y,
        Z,
        Updatable
    };

    enum NEDistanceVectorDrawing
    {
        vNone = 0,
        vAll,
        Inside,
        Outside,
    };

    Q_ENUMS(NECellDrawing)
    Q_ENUMS(NEPressureDrawing)
    Q_ENUMS(NEVelocityComponentDrawing)
    Q_ENUMS(NEDistanceVectorDrawing)
};


template <typename GridType>
class NEMacGridRenderer : private NEStaggeredGridRenderOptions
{
public:
    NEMacGridRenderer(const GridType& grid);
    void drawCells(NECellDrawing mode);
    void drawVelocities(NEVelocityComponentDrawing velocityComponentMode, bool velocityVectorMode, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent);
    void drawPressure(NEPressureDrawing mode, float minPressure, float maxPressure, float atmosphericPressure, QColor minRangeColor, QColor maxRangeColor);
    void drawFluidFractions(NEVelocityComponentDrawing mode);
    void drawDistanceVectors(NEDistanceVectorDrawing mode);

    void drawVelocityComponents(const std::vector<VelocityComponentId>& components);

private:
    void drawVelocityComponents(NEVelocityComponentDrawing mode, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent);
    void drawCellVelocityVectors(float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent);
    void drawPressure(float pressure, vec3f position, float minPressure, float maxPressure, float atmosphericPressure, vec3f minColor, vec3f maxColor, vec3f compColor);

    vec4f colorByVelocity(float velocity, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent);
    vec4f colorByPressure(float pressure, float minPressure, float minColoringRange, float maxColoringRange, QColor minRangeColor, QColor maxRangeColor, float coloringExponent);
    void drawCell(vec3f position);
    float solidBlendingFactor(const CellId& cell);

    const GridType& m_Grid;
};
