#pragma once

#include "NEObject.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEVolume))

class NECEXP_GEOMETRY NEVolume: public NEObject
{
    Q_OBJECT
    NE_CLASS("NEVolume")

public:
    enum VolumeMode
    {
      SignedDistance = 0,
      Density = 1
    };

    typedef openvdb::FloatGrid::Ptr GridPtr;

    NE_PROP_RW(QString, VolumeName, "Volume Name", "Name of the Volume.", true,)
    NE_PROP_RW(double, VoxelSize, "Voxel Size", "Voxel Size of the Grid - Usually Particle Size", true,)
    NE_PROP_RW(VolumeMode, VolumeType, "Volume Type", "Volume Type Whether Signed Distance or Just densities or other", true,)
    NE_PROP_RW(double, NarrowBandSize, "Narrow Band Size", "Narrow band for the Volume", true,)

    Q_ENUMS(VolumeMode)
public:

    NEVolume();
    NEVolume( const NEVolume& otherVolume );
    ~NEVolume();

    GridPtr Volume() const;
    void SetVolume( openvdb::FloatGrid::Ptr volume );


private:
    NE_DECL_PRIVATE( NEVolume )

};

typedef  std::shared_ptr< NEVolume > NEVolumePtr;



