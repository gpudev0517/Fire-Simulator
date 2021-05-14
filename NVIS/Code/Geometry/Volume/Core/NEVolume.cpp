#include "NEVolume.h"


class NEVolumePrivate
{
public:
    QString m_Name;
    double  m_VoxelSize;
    double  m_NarrowBandSize;
    NEVolume::VolumeMode m_VolumeType;

    openvdb::FloatGrid::Ptr m_Volume;
};

NE_PROP_DEF(QString, NEVolume, VolumeName, QString(""))
NE_PROP_DEF(double, NEVolume, VoxelSize, 1.0)
NE_PROP_DEF(NEVolume::VolumeMode, NEVolume, VolumeType, NEVolume::SignedDistance)
NE_PROP_DEF(double, NEVolume, NarrowBandSize, 2.0)


NEVolume::NEVolume() : NEObject()
{
    P = new NEVolumePrivate();
    P->m_VoxelSize = NE_DEFAULT(VoxelSize);
    P->m_VolumeType = NE_DEFAULT(VolumeType);
    P->m_NarrowBandSize = NE_DEFAULT(NarrowBandSize);
    P->m_Name = NE_DEFAULT(VolumeName);
}

NEVolume::NEVolume( const NEVolume& otherVolume ): NEObject()
{
    P = new NEVolumePrivate();
    P->m_VoxelSize      = otherVolume.VoxelSize();
    P->m_VolumeType     = otherVolume.VolumeType();
    P->m_NarrowBandSize = otherVolume.NarrowBandSize();
    if( otherVolume.Volume() )
        SetVolume( openvdb::deepCopyTypedGrid<openvdb::FloatGrid>(* (otherVolume.Volume()) ) );
}

NEVolume::~NEVolume()
{
    delete P;
}

const double& NEVolume::VoxelSize() const
{
    return P->m_VoxelSize;
}

const NEVolume::VolumeMode& NEVolume::VolumeType() const
{
    return P->m_VolumeType;
}

const double& NEVolume::NarrowBandSize() const
{
    return P->m_NarrowBandSize;
}

NEVolume::GridPtr NEVolume::Volume() const
{
    return P->m_Volume;
}

void NEVolume::SetVolume( openvdb::FloatGrid::Ptr volume)
{
    P->m_Volume = volume;
}

const QString& NEVolume::VolumeName() const
{
    return P->m_Name;
}

uint NEVolume::SetVoxelSize( const double& arg )
{
    P->m_VoxelSize = arg;
    return 1;
}

uint NEVolume::SetVolumeType( const NEVolume::VolumeMode& arg )
{
    P->m_VolumeType = arg;
    return 1;
}

uint NEVolume::SetNarrowBandSize( const double& arg )
{
    P->m_NarrowBandSize = arg;
    return 1;
}

uint NEVolume::SetVolumeName( const QString& arg )
{
    P->m_Name = arg;
    return 1;
}

