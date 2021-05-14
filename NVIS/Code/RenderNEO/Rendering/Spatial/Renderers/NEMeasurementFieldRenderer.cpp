#include "NEMeasurementFieldRenderer.h"

#include "Rendering/Spatial/NECamera.h"

#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Physics/Spatial/NEMeasurementField.h"

#include "Base/NEBase.h"


#include "Rendering/Spatial/NEOSDMeasurement.h"

#define PI 3.14159


void markCylinder(GLdouble baseRadius, GLdouble topRadius, GLdouble height, GLint slices, GLint stacks)
{
    GLint i,j;
    GLfloat sinCache[50];
    GLfloat cosCache[50];
    GLfloat sinCache2[50];
    GLfloat cosCache2[50];
    GLfloat sinCache3[50];
    GLfloat cosCache3[50];
    GLfloat angle;
    GLfloat zLow, zHigh;
    GLfloat sintemp, costemp;
    GLfloat length;
    GLfloat deltaRadius;
    GLfloat zNormal;
    GLfloat xyNormalRatio;
    GLfloat radiusLow, radiusHigh;
    int needCache2, needCache3;

    /* Compute length (needed for normal calculations) */
    deltaRadius = baseRadius - topRadius;
    length = sqrt(deltaRadius*deltaRadius + height*height);

    /* Cache is the vertex locations cache */
    /* Cache2 is the various normals at the vertices themselves */
    /* Cache3 is the various normals for the faces */
    needCache2 = needCache3 = 0;
    needCache2 = 1;

    zNormal = deltaRadius / length;
    xyNormalRatio = height / length;

    for (i = 0; i < slices; i++) {
        angle = 2 * PI * i / slices;
        if (needCache2) {
            sinCache2[i] = xyNormalRatio * sin(angle);
            cosCache2[i] = xyNormalRatio * cos(angle);
        }
        sinCache[i] = sin(angle);
        cosCache[i] = cos(angle);
    }

    if (needCache3) {
        for (i = 0; i < slices; i++) {
            angle = 2 * PI * (i-0.5) / slices;
            sinCache3[i] = xyNormalRatio * sin(angle);
            cosCache3[i] = xyNormalRatio * cos(angle);
        }
    }

    sinCache[slices] = sinCache[0];
    cosCache[slices] = cosCache[0];
    if (needCache2) {
    sinCache2[slices] = sinCache2[0];
    cosCache2[slices] = cosCache2[0];
    }
    if (needCache3) {
    sinCache3[slices] = sinCache3[0];
    cosCache3[slices] = cosCache3[0];
    }

    for (j = 0; j < stacks; j++) {
        zLow = j * height / stacks;
        zHigh = (j + 1) * height / stacks;
        radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
        radiusHigh = baseRadius - deltaRadius * ((float) (j + 1) / stacks);

        GL->glBegin(GL_QUAD_STRIP);
        for (i = 0; i <= slices; i++) {
            GL->glNormal3f(sinCache3[i], cosCache3[i], zNormal);

            GL->glVertex3f(radiusLow * sinCache[i], radiusLow * cosCache[i], zLow);

            GL->glVertex3f(radiusHigh * sinCache[i], radiusHigh * cosCache[i], zHigh);

        }
        GL->glEnd();
    }
}

GLfloat* buildCylinder(GLdouble baseRadius, GLdouble topRadius, GLdouble height, GLint slices, GLint stacks)
{
    GLfloat *outVerts = new GLfloat[6*(slices+1)];
    GLint i,j;
    GLfloat sinCache[50];
    GLfloat cosCache[50];
    GLfloat sinCache2[50];
    GLfloat cosCache2[50];
    GLfloat sinCache3[50];
    GLfloat cosCache3[50];
    GLfloat angle;
    GLfloat zLow, zHigh;
    GLfloat sintemp, costemp;
    GLfloat length;
    GLfloat deltaRadius;
    GLfloat zNormal;
    GLfloat xyNormalRatio;
    GLfloat radiusLow, radiusHigh;
    int needCache2, needCache3;

    /* Compute length (needed for normal calculations) */
    deltaRadius = baseRadius - topRadius;
    length = sqrt(deltaRadius*deltaRadius + height*height);

    /* Cache is the vertex locations cache */
    /* Cache2 is the various normals at the vertices themselves */
    /* Cache3 is the various normals for the faces */
    needCache2 = needCache3 = 0;
    needCache2 = 1;

    zNormal = deltaRadius / length;
    xyNormalRatio = height / length;

    for (i = 0; i < slices; i++) {
        angle = 2 * PI * i / slices;
        if (needCache2) {
            sinCache2[i] = xyNormalRatio * sin(angle);
            cosCache2[i] = xyNormalRatio * cos(angle);
        }
        sinCache[i] = sin(angle);
        cosCache[i] = cos(angle);
    }

    if (needCache3) {
        for (i = 0; i < slices; i++) {
            angle = 2 * PI * (i-0.5) / slices;
            sinCache3[i] = xyNormalRatio * sin(angle);
            cosCache3[i] = xyNormalRatio * cos(angle);
        }
    }

    sinCache[slices] = sinCache[0];
    cosCache[slices] = cosCache[0];
    if (needCache2) {
    sinCache2[slices] = sinCache2[0];
    cosCache2[slices] = cosCache2[0];
    }
    if (needCache3) {
    sinCache3[slices] = sinCache3[0];
    cosCache3[slices] = cosCache3[0];
    }

    for (j = 0; j < stacks; j++) {
        zLow = j * height / stacks;
        zHigh = (j + 1) * height / stacks;
        radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
        radiusHigh = baseRadius - deltaRadius * ((float) (j + 1) / stacks);



        //GL->glBegin(GL_QUAD_STRIP);
        for (i = 0; i <= slices; i++) {

            outVerts[6*i] = radiusLow * sinCache[i];
            outVerts[6*i+1] = radiusLow * cosCache[i];
            outVerts[6*i+2] = zLow;

            outVerts[6*i+3] = radiusHigh * sinCache[i];
            outVerts[6*i+4] = radiusHigh * cosCache[i];
            outVerts[6*i+5] = zHigh;

//            GL->glNormal3f(sinCache3[i], cosCache3[i], zNormal);

//            GL->glVertex3f(radiusLow * sinCache[i], radiusLow * cosCache[i], zLow);

//            GL->glVertex3f(radiusHigh * sinCache[i], radiusHigh * cosCache[i], zHigh);

        }
        //GL->glEnd();
    }

    return outVerts;
}

void markPartialDisk(GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops, GLdouble startAngle, GLdouble sweepAngle)
{
    GLint i,j;
    GLfloat sinCache[50];
    GLfloat cosCache[50];
    GLfloat angle;
    GLfloat sintemp, costemp;
    GLfloat deltaRadius;
    GLfloat radiusLow, radiusHigh;
    GLfloat texLow = 0.0, texHigh = 0.0;
    GLfloat angleOffset;
    GLint slices2;
    GLint finish;

    if (sweepAngle < -360.0) sweepAngle = 360.0;
    if (sweepAngle > 360.0) sweepAngle = 360.0;
    if (sweepAngle < 0) {
    startAngle += sweepAngle;
    sweepAngle = -sweepAngle;
    }

    if (sweepAngle == 360.0) {
    slices2 = slices;
    } else {
    slices2 = slices + 1;
    }

    /* Compute length (needed for normal calculations) */
    deltaRadius = outerRadius - innerRadius;

    /* Cache is the vertex locations cache */

    angleOffset = startAngle / 180.0 * PI;
    for (i = 0; i <= slices; i++) {
    angle = angleOffset + ((PI * sweepAngle) / 180.0) * i / slices;
    sinCache[i] = sin(angle);
    cosCache[i] = cos(angle);
    }

    if (sweepAngle == 360.0) {
    sinCache[slices] = sinCache[0];
    cosCache[slices] = cosCache[0];
    }

    GL->glNormal3f(0.0, 0.0, 1.0);

    if (innerRadius == 0.0) {
        finish = loops - 1;
        /* Triangle strip for inner polygons */
        GL->glBegin(GL_TRIANGLE_FAN);
        GL->glVertex3f(0.0, 0.0, 0.0);
        radiusLow = outerRadius - deltaRadius * ((float) (loops-1) / loops);

        for (i = slices; i >= 0; i--) {
            GL->glVertex3f(radiusLow * sinCache[i],
                radiusLow * cosCache[i], 0.0);
        }
        GL->glEnd();
    }
    else {
        finish = loops;
    }
    for (j = 0; j < finish; j++) {
        radiusLow = outerRadius - deltaRadius * ((float) j / loops);
        radiusHigh = outerRadius - deltaRadius * ((float) (j + 1) / loops);

        GL->glBegin(GL_QUAD_STRIP);
        for (i = 0; i <= slices; i++) {
            GL->glVertex3f(radiusLow * sinCache[i],
                radiusLow * cosCache[i], 0.0);

            GL->glVertex3f(radiusHigh * sinCache[i],
                radiusHigh * cosCache[i], 0.0);
        }
        GL->glEnd();
    }
}



GLfloat* buildPartialDisk(GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops, GLdouble startAngle, GLdouble sweepAngle)
{
    GLfloat *outVerts = new GLfloat[3*(slices + 2)];
    GLint i,j;
    GLfloat sinCache[50];
    GLfloat cosCache[50];
    GLfloat angle;
    GLfloat sintemp, costemp;
    GLfloat deltaRadius;
    GLfloat radiusLow, radiusHigh;
    GLfloat texLow = 0.0, texHigh = 0.0;
    GLfloat angleOffset;
    GLint finish;

    if (sweepAngle < -360.0) sweepAngle = 360.0;
    if (sweepAngle > 360.0) sweepAngle = 360.0;
    if (sweepAngle < 0) {
    startAngle += sweepAngle;
    sweepAngle = -sweepAngle;
    }


    /* Compute length (needed for normal calculations) */
    deltaRadius = outerRadius - innerRadius;

    /* Cache is the vertex locations cache */

    angleOffset = startAngle / 180.0 * PI;
    for (i = 0; i <= slices; i++) {
    angle = angleOffset + ((PI * sweepAngle) / 180.0) * i / slices;
    sinCache[i] = sin(angle);
    cosCache[i] = cos(angle);
    }

    if (sweepAngle == 360.0) {
    sinCache[slices] = sinCache[0];
    cosCache[slices] = cosCache[0];
    }

    //GL->glNormal3f(0.0, 0.0, 1.0);

    if (innerRadius == 0.0) {
        finish = loops - 1;
        /* Triangle strip for inner polygons */
//        GL->glBegin(GL_TRIANGLE_FAN);
//        GL->glVertex3f(0.0, 0.0, 0.0);

        outVerts[0] = outVerts[1] = outVerts[2] = 0.0;

        radiusLow = outerRadius - deltaRadius * ((float) (loops-1) / loops);

        for (i = slices; i >= 0; i--) {

//            GL->glVertex3f(radiusLow * sinCache[i],
//                radiusLow * cosCache[i], 0.0);

            outVerts[3*(slices-i+1)] = radiusLow * sinCache[i];
            outVerts[3*(slices-i+1)+1] = radiusLow * cosCache[i];
            outVerts[3*(slices-i+1)+2] = 0.0;

        }
        //GL->glEnd();
    }
    else {
        finish = loops;
    }
//    for (j = 0; j < finish; j++) {
//        radiusLow = outerRadius - deltaRadius * ((float) j / loops);
//        radiusHigh = outerRadius - deltaRadius * ((float) (j + 1) / loops);

//        GL->glBegin(GL_QUAD_STRIP);
//        for (i = 0; i <= slices; i++) {
//            GL->glVertex3f(radiusLow * sinCache[i],
//                radiusLow * cosCache[i], 0.0);

//            GL->glVertex3f(radiusHigh * sinCache[i],
//                radiusHigh * cosCache[i], 0.0);
//        }
//        GL->glEnd();
//    }
    return outVerts;
}


void markDisk(GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops)
{
    markPartialDisk(innerRadius, outerRadius, slices, loops, 0.0, 360.0);
}

GLfloat* buildDisk(GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint loops)
{
    return buildPartialDisk(innerRadius, outerRadius, slices, loops, 0.0, 360.0);
}



//
// MeasurementField
//
#define RADPERDEG 0.0174533
#define USE_SHADER 1

void Arrow(GLdouble x1,GLdouble y1,GLdouble z1,GLdouble x2,GLdouble y2,GLdouble z2,GLdouble D_Arrow, GLdouble D_Cylinder)
{
    double x=x2-x1;
    double y=y2-y1;
    double z=z2-z1;
    double L=sqrt(x*x+y*y+z*z);

    //GLUquadricObj *quadObj;

    GL->glPushMatrix ();

    GL->glTranslated(x1,y1,z1);

    if((x!=0.)||(y!=0.)) {
        GL->glRotated(atan2(y,x)/RADPERDEG,0.,0.,1.);
        GL->glRotated(atan2(sqrt(x*x+y*y),z)/RADPERDEG,0.,1.,0.);
    } else if (z<0){
        GL->glRotated(180,1.,0.,0.);
    }

    GL->glTranslatef(0,0,L-2*D_Arrow);

    markCylinder(D_Arrow, 0.0, 2*D_Arrow, 32, 1);
    markDisk(0.0, D_Arrow, 32, 1);

    GL->glTranslatef(0,0,-L+2*D_Arrow);

    markCylinder(D_Cylinder, D_Cylinder, L-2*D_Arrow, 32, 1);

//    quadObj = gluNewQuadric ();
//    gluQuadricDrawStyle (quadObj, GLU_FILL);
//    gluQuadricNormals (quadObj, GLU_SMOOTH);
//    gluDisk(quadObj, 0.0, D_Cylinder, 32, 1);
//    gluDeleteQuadric(quadObj);

    GL->glPopMatrix ();

}


void buildArrow(GLdouble D_Arrow, GLdouble D_Cylinder, GLfloat* &pArrowHead, GLfloat* &pArrowCover, GLfloat* &pArrowFoot)
{
    pArrowHead = buildCylinder(D_Arrow, 0.0, 2*D_Arrow, 32, 1);
    pArrowCover = buildDisk(0.0, D_Arrow, 32, 1);
    pArrowFoot = buildCylinder(D_Cylinder, D_Cylinder, 1.0, 32, 1);
}

NEMeasurementFieldRenderer::NEMeasurementFieldRenderer()
{
    m_DataArrowHead = m_DataArrowCover = m_DataArrowFoot = 0;
}

NEMeasurementFieldRenderer::~NEMeasurementFieldRenderer()
{
}

void NEMeasurementFieldRenderer::updateDrawingBuffers(){

}


void NEMeasurementFieldRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEMeasurementField* measurementField = qobject_cast<NEMeasurementField*>(spatial);
    if (measurementField == NULL) return;

//    measurementField->SetPosition( measurementField->Position() );

    renderBoundingBox(spatial, surface);

    vec3f aabbMaxPos = measurementField->localTransform() * measurementField->aabb().maxPos();
    vec3f aabbMinPos = measurementField->localTransform() * measurementField->aabb().minPos();

    if( measurementField->RenderMode() != NESpatial::NERenderMode::Invisible )
    {
        GL->glLineWidth( 2.0f );

        if( measurementField->computeFlowRate() )
        {

            vec3f pos = measurementField->Position();
            vec3f dir = measurementField->flowPlaneNormal().normalized();
            dir *= fabs( (aabbMaxPos - aabbMinPos) * dir ) * 0.5f;
            dir += pos;
            GL->glBegin( GL_LINES );
            GL->glVertex3f( pos.x(), pos.y(), pos.z() );
            GL->glVertex3f( dir.x(), dir.y(), dir.z() );
            GL->glEnd();
        }

        if(USE_SHADER){
            NERENDERMAN.GLManager()->bindPositionShader(surface);
            GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelMatrix"), 1, GL_FALSE,  (GLfloat *)(&measurementField->transform().data()[0]));
        }

        float maxV = 0.0;
        float colorV = 0.0;
        float v = 0.0;
        QVector<QPair<qreal, QColor>> colorGroup;
        colorGroup = measurementField->getColorList();
        int color_nSize = colorGroup.size();
        qreal posVel = 0.0;
        int colorPos1 = 0;
        int colorPos2 = 0;
        if( measurementField->subdivideMeasurementField() )
        {
            for( int k = 0; k < measurementField->subdiv().z(); ++k )
                for( int j = 0; j < measurementField->subdiv().y(); ++j )
                    for( int i = 0; i<measurementField->subdiv().x(); ++i )
                    {
                        vec3f pos( i+0.5f, j+0.5f, k+0.5f );
                        pos *= measurementField->subdivh();
                        pos += aabbMinPos;

                        //glBegin( GL_LINE );
                        //glVertex3f( pos.x(), pos.y(), pos.z() );

                        vec3f pos2 = pos;
                        vec3f orgVel = measurementField->avgVelField()[i][j][k];
                        vec3f vel = orgVel * measurementField->subDivArrowScale();
                        v = orgVel.length();
                        if(v > maxV)
                        {
                            maxV = v;                   //max velocity
                            measurementField->setMaxV(maxV);
                            colorV = maxV / 4.0;        //velocity for coloring
                        }
                        if( measurementField->subDivNormalizeArrows() )
                        {
                            vel.normalize();
                            vel *= measurementField->subDivMaxArrowLength();
                        } else
                        {
                            float l = vel.lengthSquared();
                            if( l > measurementField->subDivMaxArrowLength()*measurementField->subDivMaxArrowLength() )
                            {
                                vel *= measurementField->subDivMaxArrowLength()/sqrt(l);
                            }
                        }
                        pos2 += vel;
                        //glVertex3f( pos2.x(), pos2.y(), pos2.z() );
                        //glEnd();

                        //std::cout << "Pos  " << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;
                        //std::cout << "Pos2 " << pos2.x() << " " << pos2.y() << " " << pos2.z() << std::endl;

                        /*

                        GLUquadric* cyl = gluNewQuadric();
                        glTranslatef( pos2.x(), pos2.y(), pos2.z() );
                        gluCylinder( cyl, 0.04, 0.000, 0.01, 12,1);
                        */
                        //set color according to velocity
                        posVel = v / maxV;

                        for(int i = 0;i < color_nSize; i++)
                        {
                            if(colorGroup[i].first > posVel)
                            {
                                colorPos2 = i;
                                colorPos1 = i - 1;
                                break;
                            }

                        }
                        GL->glColor3ub(colorGroup[colorPos1].second.red() +
                                   (colorGroup[colorPos2].second.red() - colorGroup[colorPos1].second.red()) /
                                   (colorGroup[colorPos2].first - colorGroup[colorPos1].first) * (posVel - colorGroup[colorPos1].first),
                                   colorGroup[colorPos1].second.green() +
                                   (colorGroup[colorPos2].second.green() - colorGroup[colorPos1].second.green()) /
                                   (colorGroup[colorPos2].first - colorGroup[colorPos1].first) * (posVel - colorGroup[colorPos1].first)  ,
                                   colorGroup[colorPos1].second.blue() +
                                   (colorGroup[colorPos2].second.blue() - colorGroup[colorPos1].second.blue()) /
                                   (colorGroup[colorPos2].first - colorGroup[colorPos1].first) * (posVel - colorGroup[colorPos1].first)
                                   );

                        if(v == 0.0)
                        {
                            GL->glColor3ub(colorGroup[0].second.red(),colorGroup[0].second.green(),colorGroup[0].second.blue());
                            continue;
                        }
                        //glColor3f(v * 7 / 255.0, v * 7 / 255.0, 1.0);
                        if(!USE_SHADER){
                            Arrow( pos.x(), pos.y(), pos.z(), pos2.x(), pos2.y(), pos2.z(),
                                   measurementField->subDivArrowDiameter(), measurementField->subDivCylinderDiameter()); //.005 );
                        }
                        else{
                            double x1 = pos.x();
                            double y1 = pos.y();
                            double z1 = pos.z();

                            double x2 = pos2.x();
                            double y2 = pos2.y();
                            double z2 = pos2.z();

                            double x=x2-x1;
                            double y=y2-y1;
                            double z=z2-z1;
                            double L=sqrt(x*x+y*y+z*z);

                            if(!m_DataArrowHead){
                                buildArrow(1, 1, m_DataArrowHead, m_DataArrowCover, m_DataArrowFoot);
                            }


                            GL->glPushMatrix ();

                            GL->glTranslated(x1,y1,z1);

                            if((x!=0.)||(y!=0.)) {
                                GL->glRotated(atan2(y,x)/RADPERDEG,0.,0.,1.);
                                GL->glRotated(atan2(sqrt(x*x+y*y),z)/RADPERDEG,0.,1.,0.);
                            } else if (z<0){
                                GL->glRotated(180,1.,0.,0.);
                            }

                            GL->glTranslatef(0,0,L-2*measurementField->subDivArrowDiameter());
                            double scale = measurementField->subDivArrowDiameter();
                            GL->glScaled(scale, scale, scale);


                            NERENDERMAN.GLManager()->updatePositionUniforms(surface);
                            GL->glEnableClientState(GL_VERTEX_ARRAY);
                            GL->glVertexPointer(3, GL_FLOAT, 0, m_DataArrowHead);
                            GL->glDrawArrays(GL_QUAD_STRIP,0,2*(32+1));
                            GL->glDisableClientState(GL_VERTEX_ARRAY);

                            NERENDERMAN.GLManager()->updatePositionUniforms(surface);
                            GL->glEnableClientState(GL_VERTEX_ARRAY);
                            GL->glVertexPointer(3, GL_FLOAT, 0, m_DataArrowCover);
                            GL->glDrawArrays(GL_TRIANGLE_FAN,0,(32+2));
                            GL->glDisableClientState(GL_VERTEX_ARRAY);

                            GL->glScaled(1/scale, 1/scale, 1/scale);
                            //GL->glTranslatef(0,0,-L+2*measurementField->subDivArrowDiameter());

                            scale = measurementField->subDivCylinderDiameter();

                            //GL->glTranslated(0, 0, L-2*measurementField->subDivArrowDiameter());
                            GL->glScaled(scale, scale, -L+2*measurementField->subDivArrowDiameter());


                            NERENDERMAN.GLManager()->updatePositionUniforms(surface);
                            GL->glEnableClientState(GL_VERTEX_ARRAY);
                            GL->glVertexPointer(3, GL_FLOAT, 0, m_DataArrowFoot);
                            GL->glDrawArrays(GL_QUAD_STRIP,0,2*(32+1));
                            GL->glDisableClientState(GL_VERTEX_ARRAY);

                            GL->glPopMatrix ();

                        }
                    }
        }
        GL->glLineWidth( 1.0f );
        if(USE_SHADER)
            surface->setShaderProgram(0);

        renderTitle(spatial, surface);
    }
}

void NEMeasurementFieldRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

void NEMeasurementFieldRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
//    render(spatial, widget);
    renderBoundingBox(spatial, surface);
}

void NEMeasurementFieldRenderer::renderBoundingBox(NESpatial* spatial, IGLSurface* surface)
{
    NEMeasurementField* measurementField = qobject_cast<NEMeasurementField*>(spatial);
    if (measurementField == NULL) return;

    vec3f aabbMaxPos = measurementField->localTransform() * measurementField->aabb().maxPos();
    vec3f aabbMinPos = measurementField->localTransform() * measurementField->aabb().minPos();

    if( measurementField->RenderMode() != NESpatial::NERenderMode::Invisible )
    {
        GL->glLineWidth( 2.0f );

        GL->glBegin( GL_LINES );
        //bottom
        GL->glVertex3f( aabbMinPos.x(), aabbMinPos.y(), aabbMinPos.z());
        GL->glVertex3f(aabbMaxPos.x(), aabbMinPos.y(), aabbMinPos.z());

        GL->glVertex3f(aabbMaxPos.x(), aabbMinPos.y(), aabbMinPos.z());
        GL->glVertex3f(aabbMaxPos.x(), aabbMinPos.y(),aabbMaxPos.z());

        GL->glVertex3f(aabbMaxPos.x(), aabbMinPos.y(),aabbMaxPos.z());
        GL->glVertex3f( aabbMinPos.x(), aabbMinPos.y(),aabbMaxPos.z());

        GL->glVertex3f( aabbMinPos.x(), aabbMinPos.y(),aabbMaxPos.z());
        GL->glVertex3f( aabbMinPos.x(), aabbMinPos.y(), aabbMinPos.z());

        //vertical
        GL->glVertex3f( aabbMinPos.x(), aabbMinPos.y(), aabbMinPos.z() );
        GL->glVertex3f( aabbMinPos.x(),aabbMaxPos.y(), aabbMinPos.z() );

        GL->glVertex3f(aabbMaxPos.x(), aabbMinPos.y(), aabbMinPos.z() );
        GL->glVertex3f(aabbMaxPos.x(),aabbMaxPos.y(), aabbMinPos.z() );

        GL->glVertex3f(aabbMaxPos.x(), aabbMinPos.y(),aabbMaxPos.z() );
        GL->glVertex3f(aabbMaxPos.x(),aabbMaxPos.y(),aabbMaxPos.z() );

        GL->glVertex3f( aabbMinPos.x(), aabbMinPos.y(),aabbMaxPos.z() );
        GL->glVertex3f( aabbMinPos.x(),aabbMaxPos.y(),aabbMaxPos.z() );

        //top
        GL->glVertex3f( aabbMinPos.x(),aabbMaxPos.y(), aabbMinPos.z() );
        GL->glVertex3f(aabbMaxPos.x(),aabbMaxPos.y(), aabbMinPos.z() );

        GL->glVertex3f(aabbMaxPos.x(),aabbMaxPos.y(), aabbMinPos.z() );
        GL->glVertex3f(aabbMaxPos.x(),aabbMaxPos.y(),aabbMaxPos.z() );

        GL->glVertex3f(aabbMaxPos.x(),aabbMaxPos.y(),aabbMaxPos.z() );
        GL->glVertex3f( aabbMinPos.x(),aabbMaxPos.y(),aabbMaxPos.z() );

        GL->glVertex3f( aabbMinPos.x(),aabbMaxPos.y(),aabbMaxPos.z() );
        GL->glVertex3f( aabbMinPos.x(),aabbMaxPos.y(), aabbMinPos.z() );

        GL->glEnd();
        GL->glLineWidth( 1.0f );
    }
}

void NEMeasurementFieldRenderer::renderTitle(NESpatial* spatial, IGLSurface* surface)
{
    NEMeasurementField* measurementField = qobject_cast<NEMeasurementField*>(spatial);
    if (measurementField == NULL) return;

    vec3f aabbMaxPos = measurementField->localTransform() * measurementField->aabb().maxPos();
    vec3f aabbMinPos = measurementField->localTransform() * measurementField->aabb().minPos();

    //boundbox vertexs
    vec3f vertices[8];
    vertices[0] = aabbMinPos;
    vertices[7] = aabbMaxPos;
    vertices[1] = vec3f(vertices[0].x(),vertices[0].y(),vertices[7].z());
    vertices[2] = vec3f(vertices[0].x(),vertices[7].y(),vertices[0].z());
    vertices[3] = vec3f(vertices[7].x(),vertices[0].y(),vertices[0].z());
    vertices[4] = vec3f(vertices[0].x(),vertices[7].y(),vertices[7].z());
    vertices[5] = vec3f(vertices[7].x(),vertices[0].y(),vertices[7].z());
    vertices[6] = vec3f(vertices[7].x(),vertices[7].y(),vertices[0].z());

    vec3f cameraUp(0,0,0);
    vec3f cameraSide(0,0,0);
    vec3f cameraLookAt(0,0,0);
    vec3f cameraPos(0,0,0);
    if(surface->camera())
    {
        cameraPos = surface->camera()->Position();
        cameraUp = surface->camera()->up();
        cameraUp.normalize();
        cameraSide = surface->camera()->side();
        cameraSide.normalize();
        cameraLookAt = surface->camera()->look();
        cameraLookAt.normalize();
//             std::cout << "NEMeasurementFieldRenderer::renderTitle cameraUp   x=" << cameraUp.x() << ", y=" << cameraUp.y() << ", z=" << cameraUp.z() << ", len=" << cameraUp.length() << std::endl;
//             std::cout << "NEMeasurementFieldRenderer::renderTitle cameraSide x=" << cameraSide.x() << ", y=" << cameraSide.y() << ", z=" << cameraSide.z() << ", len=" << cameraSide.length() << std::endl;
//             std::cout << "NEMeasurementFieldRenderer::renderTitle cameraLook x=" << cameraLookAt.x() << ", y=" << cameraLookAt.y() << ", z=" << cameraLookAt.z() << ", len=" << cameraLookAt.length() << std::endl;
    }

    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
    GL->glPushMatrix();

    QString     titleMessage = measurementField->objectName(); // Title
    vec3f       objectScale = vec3f(measurementField->Scalex(), measurementField->Scaley(), measurementField->Scalez());
    float       drawTextScale = qPow(objectScale.x()*objectScale.y()*objectScale.z(), 1.0/3.0);
    float       size = 0.2f * drawTextScale;
    QColor      drawColor = measurementField->Color();
    vec3f       drawPos;
    vec3f       drawDir;
    eulerf      drawAngle = vec3f(0, 0, 0);
    eulerf      drawAngle1 = vec3f(0, 0, 0);

    drawPos = 0.5f * (vertices[2] + vertices[7]);
    drawPos.setY(drawPos.y() + 0.2 * size);
    drawDir = vec3f::crossProduct(cameraLookAt, vec3f(0, (cameraUp.y() >= 0 ? 1 : -1), 0)); drawDir.normalize();
    if (/*cameraUp.y() == 0 && */drawDir.length() < 0.01)
    {
        drawDir = cameraSide;
    }
   if (surface->camera() && surface->camera()->fixedOrthographic() && surface->camera()->OrthoType() == NECamera::Front) // 2 is front
        drawDir = cameraSide;

//        std::cout << "NEMeasurementFieldRenderer::renderTitle drawDir    x=" << drawDir.x() << ", y=" << drawDir.y() << ", z=" << drawDir.z() << ", len=" << drawDir.length() << std::endl;
    drawPos -= drawDir * (titleMessage.length() / 2.0 * size * 0.1);

    if (surface->camera() && surface->camera()->fixedOrthographic())
    {
        switch (surface->camera()->OrthoType())
        {
        case NECamera::OrthographicType::Top: // top
            drawAngle = vec3f(-90, 0, 0);
            break;
        case NECamera::OrthographicType::Left: // left
            drawAngle = vec3f(0, -90, 0);
            break;
        case NECamera::OrthographicType::Front: // front
            drawAngle = vec3f(0, 0, 0);
            break;
        default:
            break;
        }
    } else
    {
        cameraLookAt.setX(-cameraLookAt.x()); cameraLookAt.setY(-cameraLookAt.y()); cameraLookAt.setZ(-cameraLookAt.z());
        drawAngle = eulerAnglesFromDirection(cameraLookAt);
        drawAngle1 = eulerAnglesFromDirection(cameraUp);
        if (fabs(cameraUp.y()) < 0.001 || (fabs(cameraLookAt.x()) < 0.001 && fabs(cameraLookAt.z()) < 0.001))
        {
            if (cameraLookAt.y() < 0)
                drawAngle.setTheta(drawAngle1.theta());
            else
            {
                if (drawAngle1.theta() >= 0)
                    drawAngle.setTheta(drawAngle1.theta() - 180.0);
                else
                    drawAngle.setTheta(drawAngle1.theta() + 180.0);
            }
        } else
        {
            if (cameraUp.y() < 0)
            {
                if (drawAngle.theta() >= 0)
                    drawAngle.setTheta(drawAngle.theta() - 180.0);
                else
                    drawAngle.setTheta(drawAngle.theta() + 180.0);
                if (drawAngle.phi() >= 0)
                    drawAngle.setPhi(-drawAngle.phi() + 180.0);
                else
                    drawAngle.setPhi(-drawAngle.phi() - 180.0);
            }
        }
    }

    surface->drawText3D_new(titleMessage,drawPos, drawAngle, size/2, objectScale, Color32(drawColor.red(),drawColor.green(),drawColor.blue(),drawColor.alpha()));

    GL->glPopMatrix();
    GL->glPopAttrib();
}

NEMeasurementFieldRendererFactory::NEMeasurementFieldRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEMeasurementFieldRendererFactory::nodeName() { return "Measurement Field Renderer"; }
NEObject *NEMeasurementFieldRendererFactory::createInstance() {
    return new NEMeasurementFieldRenderer();
}



//
// OSDMeasurement
//
void NEOSDMeasurementRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDMeasurement* osdMeasurement = qobject_cast<NEOSDMeasurement*>(spatial);
    if (osdMeasurement == NULL) return;
    const NEMeasurementField* measure = osdMeasurement->measure();
    if (!measure) return;
    if(osdMeasurement->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    surface->setShaderProgram(0);
    GL->glDisable(GL_BLEND);
    NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();
    osdMeasurement->SetFontSize(12);
    QStringList osdMessage;
    osdMessage << osdMeasurement->objectName() << "" // Title
               << QString("Height: %1").arg(measure->avgFluidHeight())
               << QString("Pressure: %1").arg(measure->avgPressure())
               << QString("Vel: %1 %2 %3").arg(measure->avgVel().x()).arg(measure->avgVel().y()).arg(measure->avgVel().z())
               << QString("FlowRate: %1").arg(measure->flowRate());



    int xRes = surface->getWidth() * osdMeasurement->Positionx();
    int yRes = surface->getHeight() * osdMeasurement->Positiony();
    int posyU = yRes;

    for (int i = 0; i < osdMessage.size(); i++)
    {
        int xPos = xRes - osdMeasurement->osdSize().x() / 2;
        if (i == 0) xPos = xRes - osdMessage[i].length() / 2 * osdMeasurement->fontWidth();
        QString text = osdMessage[i];
        glPrint(text, xPos,
                posyU + osdMeasurement->osdSize().y() / 2 - osdMeasurement->fontHeight(),
                osdMeasurement->fontColor(),
                osdMeasurement->getTextureFont(), osdMeasurement->OutlineColor(), osdMeasurement->getTextureOutlineFont());
        posyU -= osdMeasurement->fontHeight();
    }

    //osdMeasurement->SetFontSize(15);    //set font for colorbar
    //render characters for colorbar
    glPrint(QString("velocity (m/s)"),xRes - osdMeasurement->fontWidth() * 6,
            posyU - osdMeasurement->osdSize().y() / 2 + osdMeasurement->fontHeight() * 6,
            osdMeasurement->fontColor(),
            osdMeasurement->getTextureFont(), osdMeasurement->OutlineColor(), osdMeasurement->getTextureOutlineFont());
    posyU -= osdMeasurement->fontHeight();

    int nTickCount = 5;
    for (int i = 0; i < nTickCount; i++)
    {
        QString velText = QString("%1").arg(measure->maxV() * i / 4.0f, 0, 'f', 3);
        glPrint(velText, xRes + osdMeasurement->osdSize().x() * (i - 2) / 4 - velText.length() / 2 * osdMeasurement->fontWidth(),
                posyU - osdMeasurement->osdSize().y() / 2 + osdMeasurement->fontHeight() * 6,
                osdMeasurement->fontColor(),
                osdMeasurement->getTextureFont(), osdMeasurement->OutlineColor(), osdMeasurement->getTextureOutlineFont());

    }

    posyU += osdMeasurement->fontHeight();
    //render colorbar

    QVector<QPair<qreal, QColor>> colorGroup;
    colorGroup = measure->getColorList();
    int color_nSize = colorGroup.size();
    GL->glShadeModel(GL_SMOOTH);
    GL->glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < color_nSize; i++)
    {
        GL->glColor3ub(colorGroup[i].second.red(),colorGroup[i].second.green(),colorGroup[i].second.blue());

        GL->glVertex2f(xRes + osdMeasurement->osdSize().x() * (colorGroup[i].first - 0.5),
                   posyU - osdMeasurement->osdSize().y() / 2 + osdMeasurement->fontHeight() * 5 - osdMeasurement->fontHeight() / 3);
        GL->glVertex2f(xRes + osdMeasurement->osdSize().x() * (colorGroup[i].first - 0.5),
                   posyU - osdMeasurement->osdSize().y() / 2 + osdMeasurement->fontHeight() * 4 - osdMeasurement->fontHeight() / 3);

    }
    GL->glEnd();


    GL->glPopAttrib();
}


NEOSDMeasurementRendererFactory::NEOSDMeasurementRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEOSDMeasurementRendererFactory::nodeName() { return "Measurement OSD Renderer"; }
NEObject*NEOSDMeasurementRendererFactory::createInstance() { return new NEOSDMeasurementRenderer(); }



