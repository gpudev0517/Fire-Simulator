#pragma once

typedef int NEVertexID;
typedef int NEFaceID;
typedef int NEEdgeID;
typedef int NETriangleID;
typedef int NEMeshID;

struct NEPVertex
{
    vec3f position;
};

struct NECPVertex
{
    Color32 color;
    vec3f position;
};

struct NETPVertex
{
    vec3f position;
    vec2f tCoord;
};

struct NENPVertex
{
    vec3f normal;
    vec3f position;
};

struct NEVDPVertex
{
    vec3f velocity;
    float distance;
    vec3f position;
};

struct NECNPVertex
{
    Color32 color;
    vec3f normal;
    vec3f position;
};

struct NETCPVertex
{
    vec2f tCoord;
    Color32 color;
    vec3f position;
};

struct NETNPVertex
{
    //NETNPVertex(){normal.setX(0.0); normal.setY(0.0); normal.setZ(0.0);}
    vec2f tCoord;
    vec3f normal;
    vec3f position;
};

struct NECTNPVertex
{
    //NETNPVertex(){normal.setX(0.0); normal.setY(0.0); normal.setZ(0.0);}
    vec2f tCoord;
    vec4f color;
    vec3f normal;
    vec3f position;
};

struct NECPAVertex
{
    //NETNPVertex(){normal.setX(0.0); normal.setY(0.0); normal.setZ(0.0);}
    vec4f color;
    vec3f position;
    matrix33f aniso;
};

struct NEPBVertex
{
    vec3f position;
    vec3f barycentric;
};

static const unsigned int kNEVFSize[] =
{
    sizeof(NECPVertex),
    sizeof(NETPVertex),
    sizeof(NENPVertex),
    sizeof(NECNPVertex),
    sizeof(NETCPVertex),
    sizeof(NETNPVertex),
    sizeof(NEPVertex),
    sizeof(NECTNPVertex),
    sizeof(NEPBVertex),
    sizeof(NECPAVertex)
};

struct NETriangle
{
    NECTNPVertex v1;
    NECTNPVertex v2;
    NECTNPVertex v3;
};

struct NETriangleIndexed
{
    NETriangleIndexed(){}
    NETriangleIndexed(uint id1, uint id2, uint id3) : p1(id1), p2(id2), p3(id3) {}
    uint p1, p2, p3;
};

struct NETriangleIndexedAdjacency
{
    NETriangleIndexedAdjacency(){}
    NETriangleIndexedAdjacency(uint id1, uint id2, uint id3, uint id4, uint id5, uint id6) :
        p1(id1), p2(id2), p3(id3), p4(id4), p5(id5), p6(id6) {}
    uint p1, p2, p3, p4, p5, p6;
};

