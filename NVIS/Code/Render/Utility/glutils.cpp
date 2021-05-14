#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>


#include <math.h>



#include "Utility/linmath.h"
#include "Rendering/Manager/NERenderManager.h"



/* == basic Q^3 vector math functions == */

void crossproduct(
        double ax, double ay, double az,
        double bx, double by, double bz,
        double *rx, double *ry, double *rz )
{
    *rx = ay*bz - az*by;
    *ry = az*bx - ax*bz;
    *rz = ax*by - ay*bx;
}

void crossproduct_v(
        double const * const a,
        double const * const b,
        double * const c )
{
    crossproduct(
                a[0], a[1], a[2],
            b[0], b[1], b[2],
            c, c+1, c+2 );
}

double scalarproduct(
        double ax, double ay, double az,
        double bx, double by, double bz )
{
    return ax*bx + ay*by + az*bz;
}

double scalarproduct_v(
        double const * const a,
        double const * const b )
{
    return scalarproduct(
                a[0], a[1], a[2],
            b[0], b[1], b[2] );
}

double length(
        double ax, double ay, double az )
{
    return sqrt(
                scalarproduct(
                    ax, ay, az,
                    ax, ay, az ) );
}

double length_v( double const * const a )
{
    return sqrt( scalarproduct_v(a, a) );
}

double normalize(
        double *x, double *y, double *z)
{
    double const k = 1./length(*x, *y, *z);

    *x *= k;
    *y *= k;
    *z *= k;

    return 1.0;
}

double normalize_v( double *a )
{
    double l = length_v(a);
    double k = 0;
    if( l > 0 )
        k = 1./length_v(a);
    a[0] *= k;
    a[1] *= k;
    a[2] *= k;

    return 1.0;
}


void draw_arrow(
        float ax, float ay, float az,
        float bx, float by, float bz,
        float ah, float bh)
{

    GLdouble mv[16];
    GL->glGetDoublev(GL_MODELVIEW_MATRIX, mv);

    /* We're assuming the modelview RS part is (isotropically scaled)
         * orthonormal, so the inverse is the transpose.
         * The local view direction vector is the 3rd column of the matrix;
         * assuming the view direction to be the normal on the arrows tangent
         * space  taking the cross product of this with the arrow direction
         * yields the binormal to be used as the orthonormal base to the
         * arrow direction to be used for drawing the arrowheads */

    double d[3] = {
        bx - ax,
        by - ay,
        bz - az
    };
    normalize_v(d);


    double n[3] = { mv[2], mv[6], mv[10] };
    {
        double const s = scalarproduct_v(d,n);
        for(int i = 0; i < 3; i++)
            n[i] -= d[i]*s;
    }
    normalize_v(n);

    double b[3];
    crossproduct_v( n, d, b );

    if( qIsNaN( d[0] ) ||
            qIsNaN( d[1] ) ||
            qIsNaN( d[2]) )
        return;


    GLfloat const pos[][3] = {
        {ax, ay, az},
        {(float)bx, (float)by, (float)bz},
        { ax + (0.866f*(float)d[0] + 0.5f*(float)b[0])*ah,
          ay + (0.866f*(float)d[1] + 0.5f*(float)b[1])*ah,
          az + (0.866f*(float)d[2] + 0.5f*(float)b[2])*ah },
        { ax + (0.866f*(float)d[0] - 0.5f*(float)b[0])*ah,
          ay + (0.866f*(float)d[1] - 0.5f*(float)b[1])*ah,
          az + (0.866f*(float)d[2] - 0.5f*(float)b[2])*ah },
        { (float)bx + (-2*(float)d[0] + 0.25f*(float)b[0])*(float)bh,
          (float)by + (-2*(float)d[1] + 0.25f*(float)b[1])*(float)bh,
          (float)bz + (-2*(float)d[2] + 0.25f*(float)b[2])*(float)bh },
        { (float)bx + (-2*(float)d[0] - 0.25f*(float)b[0])*(float)bh,
          (float)by + (-2*(float)d[1] - 0.25f*(float)b[1])*(float)bh,
          (float)bz + (-2*(float)d[2] - 0.25f*(float)b[2])*(float)bh }
    };
    GLushort const idx_line[][2] = {
        { 0, 1 },
    };
    GLushort const idx_heads[][3] = {
        { 0, 2, 3 },
        { 1, 4, 5 }
    };
    GL->glDisableClientState( GL_COLOR_ARRAY );
    GL->glDisableClientState( GL_NORMAL_ARRAY );
    GL->glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    GL->glEnableClientState( GL_VERTEX_ARRAY );
    GL->glVertexPointer( 3, GL_FLOAT, 0, pos );

    GL->glDrawElements( GL_LINES, 2, GL_UNSIGNED_SHORT, idx_line );
    //glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_SHORT, idx_heads);
    GL->glDisableClientState( GL_VERTEX_ARRAY );


}

void draw_arc(
        vec3 center,
        vec3 a, vec3 b,
        float ah, float bh)
{
    a[0] = b[2];
}




void draw_frustum(
        float l, float r, float b, float t,
        float n, float f )
{
    GLfloat const kf = f/n;
    GLfloat const pos[][3] =
    {
        {0,0,0},
        {l, b, -n},
        {r, b, -n},
        {r, t, -n},
        {l, t, -n},
        {kf*l, kf*b, -f},
        {kf*r, kf*b, -f},
        {kf*r, kf*t, -f},
        {kf*l, kf*t, -f}
    };
    GLushort const idx_tip[][2] = {
        {0, 1},
        {0, 2},
        {0, 3},
        {0, 4}
    };
    GLushort const idx_vol[][2] = {
        {1, 5}, {2, 6},	{3, 7},	{4, 8},
        {1, 2},	{2, 3},	{3, 4},	{4, 1},
        {5, 6},	{6, 7},	{7, 8},	{8, 5}
    };

    GL->glDisableClientState(GL_COLOR_ARRAY);
    GL->glDisableClientState(GL_NORMAL_ARRAY);
    GL->glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glVertexPointer(3, GL_FLOAT, 0, pos);

    GL->glLineWidth(1);
    GL->glLineStipple(2, 0xf3cf);
    GL->glEnable(GL_LINE_STIPPLE);
    GL->glDrawElements(GL_LINES, 2*4, GL_UNSIGNED_SHORT, idx_tip);

    GL->glLineWidth(2);
    GL->glLineStipple(1, 0xffff);
    GL->glDisable(GL_LINE_STIPPLE);
    GL->glDrawElements(GL_LINES, 2*4*3, GL_UNSIGNED_SHORT, idx_vol);
    GL->glDisableClientState(GL_VERTEX_ARRAY);

    GL->glLineWidth(1.5);

    draw_arrow(l, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, 0.0f );
    draw_arrow(0.f, 0.f, 0.f, r, 0.f, 0.f, 0.0f, 0.1f );
    draw_arrow(0.f, b, 0.f, 0.f, 0.f, 0.f, 0.1f, 0.0f );
    draw_arrow(0.f, 0.f, 0.f, 0.f, t, 0.f, 0.0f, 0.1f );

    draw_arrow(r, 0.f, 0.f, r, 0.f, -n, 0.1f, 0.1f);
    draw_arrow(l, 0, 0.f, l, 0.f, -n, 0.1f, 0.1f);
    draw_arrow(0.f, t, 0.f, 0.f, t, -n, 0.1f, 0.1f);
    draw_arrow(0.f, b, 0.f, 0.f, b, -n, 0.1f, 0.1f);

    draw_arrow(0.f, f*t/n, 0.f, 0.f, f*t/n, -f, 0.1f, 0.1f);
    draw_arrow(0.f, f*b/n, 0.f, 0.f, f*b/n, -f, 0.1f, 0.1f);
    draw_arrow(f*l/n, 0.f, 0.f, f*l/n, 0.f, -f, 0.1f, 0.1f);
    draw_arrow(f*r/n, 0.f, 0.f, f*r/n, 0.f, -f, 0.1f, 0.1f);
}
