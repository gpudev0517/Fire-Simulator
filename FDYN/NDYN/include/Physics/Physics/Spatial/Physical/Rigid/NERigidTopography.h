#pragma once

#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NERigidTopography))

class NECEXP_PHYSICS NERigidTopography : public NESphRigid
{
    Q_OBJECT
    NE_CLASS("Rigid Topography")

public:
    enum TopoImportType
    {
        TopographyData = 0,
        TopographyMesh = 1
    };
    enum TopoFileFormatType
    {
        AsciiScatteredData = 0,
        ArcAsciiGrid = 1
    };
    enum class InputGeoCoordSystemType
    {
        LocalEstNorthUp = 0,
        LongitudeLatitudeHeightWgs84 = 1,
        LongitudeLatitudeHeightNad83 = 2
    };
    enum class OutputGeoCoordSystemType
    {
        LocalEstNorthUp = 0
    };
    enum NETopoColoringOptions {Fixed = 0, Ramp = 1};

    Q_INTERFACES(NESphRigid)

    NE_PROP_RW(TopoImportType, TopoImport, "Topography Import", "Format of the topography data to be imported", true, )
    NE_PROP_RW(FilePath, TopoFilePath, "Topography File Path", "File path of the topography data", true, )
    NE_PROP_RW(TopoFileFormatType, TopoFileFormat, "Topography File Format", "File format of the topography data", isTopoImportData, )
    NE_PROP_RW(InputGeoCoordSystemType, InputGeoCoordSystem, "Input Geo. Coord. System", "Geographic coordinate system of the topography data", isTopoImportData, )
    NE_PROP_RW(OutputGeoCoordSystemType, OutputGeoCoordSystem, "Output Geo. Coord. System", "Geographic coordinate system to convert the topography data to", isTopoImportData, )
    NE_PROP_RW(bool, ExportTopoMesh, "Export Topography Mesh", "Exports the topography mesh", isTopoImportData, )

    NE_PROP_GROUP_START(TopoParticleColoring, "Topography Coloring", "Topography colorization related options", true)
    NE_PROP_RW(double, TopoMinColoringRange, "Topography Minimum Value", "The lower limit where the color interpolation starts.", true,)
    NE_PROP_RW(double, TopoMaxColoringRange, "Topography Maximum Value", "The upper limit where the color interpolation starts.", true,)
    NE_PROP_RW(bool, TopoAutomaticRange, "Automatic Color Range", "Should the color range be automatic based on the minimum and maximum values?", true,)
    NE_PROP_RW(NETopoColoringOptions, TopoColoringOptions, "Coloring Options", "Coloring Options", true,)
    NE_SUB_PROP_START(SetTopoColoringOptions, TopoColoringOptions)
    NE_PROP_RW(QColor, TopoMinRangeColor, "Color at Minimum", "Color of the minimum value", true,)
    NE_PROP_RW(QColor, TopoMaxRangeColor, "Color at Maximum", "Color of the maximum value", true,)
    NE_PROP_RW(double, TopoColoringExponent, "Coloring Exponent", "The exponent used to scale the input data before drawing particles.", true,)
    NE_PROP_RW(bool, TopoInterpolateHues, "Interpolate Hues", "Interpolation between Max and Min Colors based on Hue", true,)
    NE_PROP_RW(NEColorRamp, TopoColorRamp, "Color Ramp", "Color Ramp", true,)
    NE_SUB_PROP_END(SetTopoColoringOptions)
    NE_PROP_GROUP_END(TopoParticleColoring)

    Q_ENUMS(TopoImportType TopoFileFormatType InputGeoCoordSystemType OutputGeoCoordSystemType NETopoColoringOptions)

public:
    NERigidTopography(NENodeFactory* factory);
    virtual ~NERigidTopography();
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

    double volume() override;

    // Returns the geographic reference location
    const vec3f& geoRefLocation() const;
    QVector<QPair<qreal, QColor>> getColorList() const;

public slots:
    uint SetOrientation(const eulerf& val) override;

signals:
    void geoRefLocationChanged();

private:
    uint createMesh() override;
    void generateBoundaryParticles() override;
    void initializeGUIActions() override;
    bool isTopoImportData();
    bool isExportDataVisible() override;
    bool isExportParticleDataVisible() override;
    void calculateMinMaxElevation();
    NE_DECL_PRIVATE(NERigidTopography)
};

class NERigidTopographyFactory : public NENodeFactory
{
public:
    NERigidTopographyFactory(NEManager* m);
    ~NERigidTopographyFactory();
    NE_DERIVED(NENodeFactory, Public)
};
