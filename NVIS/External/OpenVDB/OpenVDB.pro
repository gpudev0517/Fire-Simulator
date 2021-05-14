
include(../../QMake/utility.pri)
include(../../QMake/external_lib_config.pri)

QT       -= core gui

TARGET = OpenVDB
TEMPLATE = lib

DEFINES += OPENVDB_LIBRARY OPENVDB_PRIVATE NOMINMAX _USE_MATH_DEFINES

SOURCES += \
  openvdb/Grid.cc \
  openvdb/io/Archive.cc \
  openvdb/io/Compression.cc \
  openvdb/io/DelayedLoadMetadata.cc \
  openvdb/io/File.cc \
  openvdb/io/GridDescriptor.cc \
  openvdb/io/Queue.cc \
  openvdb/io/Stream.cc \
  openvdb/io/TempFile.cc \
  openvdb/math/Maps.cc \
  openvdb/math/Proximity.cc \
  openvdb/math/QuantizedUnitVec.cc \
  openvdb/math/Transform.cc \
  openvdb/Metadata.cc \
  openvdb/MetaMap.cc \
  openvdb/openvdb.cc \
  openvdb/Platform.cc \
  openvdb/points/AttributeArray.cc \
  openvdb/points/AttributeArrayString.cc \
  openvdb/points/AttributeGroup.cc \
  openvdb/points/AttributeSet.cc \
  openvdb/points/StreamCompression.cc \
  openvdb/points/points.cc \
  openvdb/util/Formats.cc \
  openvdb/util/Util.cc
  
 

HEADERS += \
  openvdb/Exceptions.h \ \
  openvdb/Grid.h \
  Mopenvdb/etadata.h \
  openvdb/MetaMap.h \
  openvdb/openvdb.h \
  openvdb/Platform.h \
  openvdb/PlatformConfig.h \
  openvdb/Types.h \
  openvdb/version.h \
  openvdb/io/Archive.h \
  openvdb/io/Compression.h \
  openvdb/io/DelayedLoadMetadata.h \
  openvdb/io/File.h \
  openvdb/io/GridDescriptor.h \
  openvdb/io/io.h \
  openvdb/io/Queue.h \
  openvdb/io/Stream.h \
  openvdb/io/TempFile.h \
  openvdb/math/BBox.h \
  openvdb/math/ConjGradient.h \
  openvdb/math/Coord.h \
  openvdb/math/DDA.h \
  openvdb/math/FiniteDifference.h \
  openvdb/math/LegacyFrustum.h \
  openvdb/math/Maps.h \
  openvdb/math/Mat.h \
  openvdb/math/Mat3.h \
  openvdb/math/Mat4.h \
  openvdb/math/Math.h \
  openvdb/math/Operators.h \
  openvdb/math/Proximity.h \
  openvdb/math/QuantizedUnitVec.h \
  openvdb/math/Quat.h \
  openvdb/math/Ray.h \
  openvdb/math/Stats.h \
  openvdb/math/Stencils.h \
  openvdb/math/Transform.h \
  openvdb/math/Tuple.h \
  openvdb/math/Vec2.h \
  openvdb/math/Vec3.h \
  openvdb/math/Vec4.h \
  openvdb/points/AttributeArray.h \
  openvdb/points/AttributeArrayString.h \
  openvdb/points/AttributeGroup.h \
  openvdb/points/AttributeSet.h \
  openvdb/points/IndexFilter.h \
  openvdb/points/IndexIterator.h \
  openvdb/points/PointAdvect.h \
  openvdb/points/PointAttribute.h \
  openvdb/points/PointConversion.h \
  openvdb/points/PointCount.h \
  openvdb/points/PointDataGrid.h \
  openvdb/points/PointDelete.h \
  openvdb/points/PointGroup.h \
  openvdb/points/PointMask.h \
  openvdb/points/PointMove.h \
  openvdb/points/PointSample.h \
  openvdb/points/PointScatter.h \
  openvdb/points/StreamCompression.h \
  openvdb/tools/ChangeBackground.h \
  openvdb/tools/Clip.h \
  openvdb/tools/Composite.h \
  openvdb/tools/Dense.h \
  openvdb/tools/DenseSparseTools.h \
  openvdb/tools/Diagnostics.h \
  openvdb/tools/Filter.h \
  openvdb/tools/FindActiveValues.h \
  openvdb/tools/GridOperators.h \
  openvdb/tools/GridTransformer.h \
  openvdb/tools/Interpolation.h \
  openvdb/tools/LevelSetAdvect.h \
  openvdb/tools/LevelSetFilter.h \
  openvdb/tools/LevelSetFracture.h \
  openvdb/tools/LevelSetMeasure.h \
  openvdb/tools/LevelSetMorph.h \
  openvdb/tools/LevelSetPlatonic.h \
  openvdb/tools/LevelSetRebuild.h \
  openvdb/tools/LevelSetSphere.h \
  openvdb/tools/LevelSetTracker.h \
  openvdb/tools/LevelSetUtil.h \
  openvdb/tools/Mask.h \
  openvdb/tools/MeshToVolume.h \
  openvdb/tools/Morphology.h \
  openvdb/tools/MultiResGrid.h \
  openvdb/tools/ParticleAtlas.h \
  openvdb/tools/ParticlesToLevelSet.h \
  openvdb/tools/PointAdvect.h \
  openvdb/tools/PointIndexGrid.h \
  openvdb/tools/PointPartitioner.h \
  openvdb/tools/PointScatter.h \
  openvdb/tools/PointsToMask.h \
  openvdb/tools/PoissonSolver.h \
  openvdb/tools/PotentialFlow.h \
  openvdb/tools/Prune.h \
  openvdb/tools/RayIntersector.h \
  openvdb/tools/RayTracer.h \
  openvdb/tools/SignedFloodFill.h \
  openvdb/tools/Statistics.h \
  openvdb/tools/TopologyToLevelSet.h \
  openvdb/tools/ValueTransformer.h \
  openvdb/tools/VectorTransformer.h \
  openvdb/tools/VelocityFields.h \
  openvdb/tools/VolumeAdvect.h \
  openvdb/tools/VolumeToMesh.h \
  openvdb/tools/VolumeToSpheres.h \
  openvdb/tree/InternalNode.h \
  openvdb/tree/Iterator.h \
  openvdb/tree/LeafBuffer.h \
  openvdb/tree/LeafManager.h \
  openvdb/tree/LeafNode.h \
  openvdb/tree/LeafNodeBool.h \
  openvdb/tree/LeafNodeMask.h \
  openvdb/tree/NodeManager.h \
  openvdb/tree/NodeUnion.h \
  openvdb/tree/RootNode.h \
  openvdb/tree/Tree.h \
  openvdb/tree/TreeIterator.h \
  openvdb/tree/ValueAccessor.h \
  openvdb/util/CpuTimer.h \
  openvdb/util/Formats.h \
  openvdb/util/logging.h \
  openvdb/util/MapsUtil.h \
  openvdb/util/Name.h \
  openvdb/util/NodeMasks.h \
  openvdb/util/NullInterrupter.h \
  openvdb/util/PagedArray.h \
  openvdb/util/Util.h \
  


win32 {
    LIBS += -lzlibwapi -lHalf
    DEFINES += ZLIB_DLL ZLIB_WINAPI
    QMAKE_CXXFLAGS += /bigobj
}


LIBS += -lHalf -ltbb

unix {
 LIBS += -lboost_system -lboost_iostreams
}
win32 {
    CONFIG(release, debug|release) {
        LIBS += -llibboost_system-vc141-mt-x64-1_73
        LIBS += -llibboost_iostreams-vc141-mt-x64-1_73
    }
    CONFIG(debug, debug|release) {
        LIBS += -llibboost_system-vc141-mt-x64-1_73
        LIBS += -llibboost_iostreams-vc141-mt-x64-1_73
    }
}


OTHER_FILES += \
    openvdb/CHANGES \
    openvdb/COPYRIGHT \
    openvdb/doxygen-config \
    openvdb/INSTALL \
    openvdb/LICENSE \
    openvdb/README \
    openvdb/Makefile

INSTALL_PREFIX = $$INSTALL_INC_DIR
INSTALL_HEADERS = $$HEADERS
for(header, INSTALL_HEADERS) {
  path = $${INSTALL_PREFIX}/$${dirname(header)}
  path = $$relative_path($$path)
  eval(headers_$${path}.files += $$header)
  eval(headers_$${path}.path = $$path)
  eval(INSTALLS *= headers_$${path})
}
