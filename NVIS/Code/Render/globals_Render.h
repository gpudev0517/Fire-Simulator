#pragma once

#if NEUTRINO_NERENDERING_BUILD
#define NEFEXP_NERENDERING Q_DECL_EXPORT
#define NECEXP_NERENDERING Q_DECL_EXPORT
#else
#define NEFEXP_NERENDERING Q_DECL_IMPORT
#define NECEXP_NERENDERING Q_DECL_IMPORT
#endif

#ifdef WIN32
#define USE_EXT_GFSDK_SSAO 1
#endif


//#############################################//
//############### OpenGL related ##############//
//#############################################//

enum NEBlendFunc
{
    kNoBlendFunc,
    kOpacityBlendFunc,
    kAddBlendFunc,
    kMultiplyBlendFunc,
    kBlendFuncCount
};

enum NEFillMode
{
    kWireframeFrontAndBack,
    kWireframeFront,
    kSolidFrontAndBack,
    kSolidFront,
    kSolidBackWireFront,
    NEFillModeCount
};

enum NEShadeMode
{
    kFlatShaded,
    kGouraudShaded,
    NEShadeModeCount
};

enum NEClearBuffer
{
    kColorClear,
    kDepthClear,
    kColorDepthClear,
    NEClearCount
};

enum NEDepthTestFunc
{
    kDisableDepthTest,
    kGreaterDepthTest,
    kGreaterEqualDepthTest,
    kLessDepthTest,
    kLessEqualDepthTest,
    kDepthTestCount
};

enum NEPrimType
{
    kPointListPrim,
    kLineListPrim,
    kLineStripPrim,
    kLineLoopPrim,
    kTriangleListPrim,
    kTriangleStripPrim,
    kTriangleFanPrim,
    kQuadListPrim,
    kQuadStripPrim,
    kPolyListPrim,
    kTriangleListAdjacencyPrim,
    kPrimTypeCount,
};

enum NEVertexFormat 
{
    kCPFormat,      // color, position
    kTPFormat,      // texture coord, position
    kNPFormat,      // normal, position
    kCNPFormat,		// color, normal, position
    kTCPFormat,     // texture coord, color, position
    kTNPFormat,      // texture coord, normal, position
    kPFormat,       // position only
    kCTNPFormat,    // texture coord, normal, position, color
    kPBFormat,   // position, bary centric (for quad wireframe),
    kCPAFormat,      // Color, Position, Anisotropicity format
    kVertexFormatCount
};

//enum NEDrawingMode
//{
//	kStatic = GL_STATIC_DRAW,
//	kDynamic = GL_DYNAMIC_DRAW,
//	kStream = GL_STREAM_DRAW
//};

//enum NEGPUBufferUsage
//{
//	kStreamDraw = GL_STREAM_DRAW,
//	kStreamRead = GL_STREAM_READ,
//	kStreamCopy = GL_STREAM_COPY,
//	kStaticDraw = GL_STATIC_DRAW,
//	kStaticRead = GL_STATIC_READ,
//	kStaticCopy = GL_STATIC_COPY,
//	kDynamicDraw = GL_DYNAMIC_DRAW,
//	kDynamicRead = GL_DYNAMIC_READ,
//	kDynamicCopy = GL_DYNAMIC_COPY
//};

#define MAX_COLORRAMP_SIZE 32

#define OSD_PARTICLE_COLORBAR_WIDTH 50
#define OSD_PARTICLE_COLORBAR_HEIGHT 400
#define OSD_PARTICLE_MEASURE_MARGIN 25


#define MAX_MATERIAL_TEXTURES 6

