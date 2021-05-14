
#include "NERigidTopographyRenderer.h"

#include "Base/NEBase.h"

#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Rendering/Resource/NEMaterial.h"

#include "Physics/Spatial/Physical/Rigid/NERigidTopography.h"
#include "Rendering/Spatial/NEOSDRigidTopography.h"

//

void NERigidTopographyRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NERigidTopography* rigidTopography = qobject_cast<NERigidTopography*>(spatial);
    if (rigidTopography == NULL) return;
    NEIndexedTriMesh* topographyMesh = &rigidTopography->triangleMesh();
    if (topographyMesh == NULL) return;

    if( rigidTopography->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;
    if( NEBASE.guiMode() == false )
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    if(rigidTopography->RenderMode() == NESpatial::NERenderMode::Wireframe)
    {
        GL->glPushAttrib(GL_POLYGON_BIT);
        GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        NERENDERMAN.GLManager()->drawRaw(topographyMesh, surface, false, rigidTopography->DisplayMode() == NESpatial::BoundingBox);
        GL->glPopAttrib();
    }
    else if(rigidTopography->RenderMode() == NESpatial::NERenderMode::FlatShaded)
    {
        NERENDERMAN.GLManager()->drawFlat(topographyMesh, surface, rigidTopography->DisplayMode() == NESpatial::BoundingBox);
    }
    else
    {
        //        NERENDERMAN.GLManager()->draw(topographyMesh, surface, false, rigidTopography->TwoSidedLighting(),rigidTopography->ReceiveShadows(),rigidTopography->transform());
        if( topographyMesh->triangles().size() > 0 )
        {
            NEMaterial*         topographyMaterial = NERENDERMAN.GLManager()->material( const_cast< NEIndexedTriMesh *>( topographyMesh ));
            QOpenGLShaderProgram*   topographyShader = 0;
            // If the mesh is associated with a material, draw it using its shader
            if(topographyMaterial)
            {
                topographyShader = topographyMaterial->shaderProgram();
            }
            if(topographyShader && topographyShader->isLinked())
            {
                surface->setShaderProgram(topographyShader);

                float minVal = rigidTopography->TopoMinColoringRange();
                float maxVal = rigidTopography->TopoMaxColoringRange();

                GL->glUniform1f( GL->glGetUniformLocation( topographyShader->programId(), "minTerrainElevation" ), minVal );
                GL->glUniform1f( GL->glGetUniformLocation( topographyShader->programId(), "maxTerrainElevation" ), maxVal );

                if (rigidTopography->TopoColoringOptions() == NERigidTopography::Fixed)
                {
                    QColor minColor = rigidTopography->TopoMinRangeColor();
                    QColor maxColor = rigidTopography->TopoMaxRangeColor();
                    bool interpolateMode = rigidTopography->TopoInterpolateHues();
                    vec3f minColorv = vec3f(minColor.red() / 255.0, minColor.green() / 255.0, minColor.blue() / 255.0);
                    vec3f maxColorv = vec3f(maxColor.red() / 255.0, maxColor.green() / 255.0, maxColor.blue() / 255.0);

                    // Render Color Bar
                    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), interpolateMode );
                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), 2 );
                    GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[0]").toUtf8() ), 1, &minColorv[0] );
                    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[0]").toUtf8() ), 0.0 );
                    GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[1]").toUtf8() ), 1, &maxColorv[0] );
                    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[1]").toUtf8() ), 1.0 );
                }
                else if (rigidTopography->TopoColoringOptions() == NERigidTopography::Ramp)
                {
                    QVector<QPair<qreal, QColor>> colorList = rigidTopography->getColorList();
                    int nSize = std::min(colorList.size(), MAX_COLORRAMP_SIZE);

                    // Render Color Bar
                    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), 0 );
                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), nSize );
                    for(int i = 0; i < nSize; i++)
                    {
                        float pos = colorList.at(i).first;
                        QColor color = colorList.at(i).second;
                        vec3f colorv = vec3f(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
                        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[" + QString::number(i) + "]").toUtf8() ), 1, &colorv[0] );
                        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[" + QString::number(i) + "]").toUtf8() ), pos );
                    }
                }

                if (topographyMaterial)
                    topographyMaterial->updateUniforms();
                if (surface) surface->updateEnvironmentUniforms();

                NERENDERMAN.GLManager()->updatePositionUniforms(surface);
                matrix44f modelMatrix = rigidTopography->transform();
                NERENDERMAN.GLManager()->setModelMatrixForPBR(surface, modelMatrix);

                surface->shaderProgram()->setUniformValue("TwoSided", rigidTopography->TwoSidedLighting());
                surface->shaderProgram()->setUniformValue("isReceiveShadow", rigidTopography->ReceiveShadows());
                NERENDERMAN.GLManager()->setLightAndClipPlanForTerrain(surface, topographyShader);
            }
            else	// Draw it using color shader otherwise
            {
                NERENDERMAN.GLManager()->bindPositionShader(surface);
                NERENDERMAN.GLManager()->updatePositionUniforms(surface);
            }
            NERENDERMAN.GLManager()->draw( kTriangleListPrim, NERENDERMAN.GLManager()->vertexBuffer( topographyMesh ), NERENDERMAN.GLManager()->indexBuffer( topographyMesh ) );
            surface->setShaderProgram(0);
        }
    }

    GL->glPopAttrib();
}


NERigidTopographyRendererFactory::NERigidTopographyRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NERigidTopographyRendererFactory::nodeName() { return "Rigid Topography Renderer"; }
NEObject *NERigidTopographyRendererFactory::createInstance() { return new NERigidTopographyRenderer(); }


//
// OSDRigidTopographyRenderer
//
void NEOSDRigidTopographyRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDRigidTopography* osdRigidTopography = qobject_cast<NEOSDRigidTopography*>(spatial);
    if (osdRigidTopography == NULL) return;
    const NERigidTopography* rigidTopography = osdRigidTopography->rigidTopography();
    if (!rigidTopography) return;
    if(osdRigidTopography->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    float minVal = rigidTopography->TopoMinColoringRange();
    float maxVal = rigidTopography->TopoMaxColoringRange();

    if (rigidTopography->TopoColoringOptions() == NERigidTopography::Fixed)
    {
        QColor minColor = rigidTopography->TopoMinRangeColor();
        QColor maxColor = rigidTopography->TopoMaxRangeColor();
        bool interpolateMode = rigidTopography->TopoInterpolateHues();
        vec3f minColorv = vec3f(minColor.red() / 255.0, minColor.green() / 255.0, minColor.blue() / 255.0);
        vec3f maxColorv = vec3f(maxColor.red() / 255.0, maxColor.green() / 255.0, maxColor.blue() / 255.0);

        // Render Color Bar
        surface->setShaderProgram(osdRigidTopography->getColorBarProgram());
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), interpolateMode );
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), 2 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[0]").toUtf8() ), 1, &minColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[0]").toUtf8() ), 0.0 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[1]").toUtf8() ), 1, &maxColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[1]").toUtf8() ), 1.0 );
    }
    else if (rigidTopography->TopoColoringOptions() == NERigidTopography::Ramp)
    {
        QVector<QPair<qreal, QColor>> colorList = rigidTopography->getColorList();
        int nSize = std::min(colorList.size(), MAX_COLORRAMP_SIZE);

        // Render Color Bar
        surface->setShaderProgram(osdRigidTopography->getColorBarProgram());
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), 0 );
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), nSize );
        for(int i = 0; i < nSize; i++)
        {
            float pos = colorList.at(i).first;
            QColor color = colorList.at(i).second;
            vec3f colorv = vec3f(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
            GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[" + QString::number(i) + "]").toUtf8() ), 1, &colorv[0] );
            GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[" + QString::number(i) + "]").toUtf8() ), pos );
        }
    }

    int width = surface->getWidth();
    int height = surface->getHeight();

    int xRes = width * osdRigidTopography->Positionx();
    int yRes = height * osdRigidTopography->Positiony();
    int wRes = OSD_PARTICLE_COLORBAR_WIDTH * osdRigidTopography->Scalex();      //!< Width of color bar
    int hRes = OSD_PARTICLE_COLORBAR_HEIGHT * osdRigidTopography->Scaley();     //!< Height of color bar
    int hTitle = osdRigidTopography->fontHeight() * 2;
    int wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdRigidTopography->Scalex();   //!< Width of margin between color bar and measure
    unsigned int widthU = 0;   //!< Width of measurement text
    NEOSDRigidTopography::NEOSDOrientation enumOrientation = osdRigidTopography->OSDOrientation();
    if (enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Top ||
        enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Bottom)
    {
        if (osdRigidTopography->FontSize() == 20)
            osdRigidTopography->SetFontSize(12);
    } else
    {
        if (osdRigidTopography->FontSize() == 12)
            osdRigidTopography->SetFontSize(20);
    }
    vec2f OsdSize = osdRigidTopography->osdSize();
    if (enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Top ||
        enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Bottom)
    {
        wRes = OsdSize.x() - (int)(osdRigidTopography->fontWidth());
        hRes = OSD_PARTICLE_COLORBAR_WIDTH * osdRigidTopography->Scaley();
        wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdRigidTopography->Scaley();
    }

    int xPos = 0, yPos = 0;
    vec3f colorBarRect[4];
    vec3f colorBarRect_2D[4];
    vec2f widgetSize = vec2f(width, height);
    vec3f offsetOfOrientation;
    switch(enumOrientation)
    {
    case NEOSDRigidTopography::NEOSDOrientation::Top:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes - hTitle, 0);
        colorBarRect[1] = vec3f(xRes, yRes - hTitle - hRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes - hTitle - hRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes - hTitle, 3);
        break;
    case NEOSDRigidTopography::NEOSDOrientation::Bottom:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes + hRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes + hRes, 3);
        break;
    case NEOSDRigidTopography::NEOSDOrientation::Right:
        offsetOfOrientation = vec3f(OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes - wRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes - wRes, yRes + hRes, 3);
        break;
    case NEOSDRigidTopography::NEOSDOrientation::Left:
    default:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes + wRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes, yRes + hRes, 3);
        break;
    }
    for (int nIndex = 0; nIndex < 4; nIndex++)
    {
        colorBarRect[nIndex] = (colorBarRect[nIndex] + offsetOfOrientation); // widgetSize;
        colorBarRect_2D[nIndex] = vec3f(colorBarRect[nIndex].x(), colorBarRect[nIndex].y(), 0);
        colorBarRect[nIndex].setX(colorBarRect[nIndex].x() / widgetSize.x());
        colorBarRect[nIndex].setY(colorBarRect[nIndex].y() / widgetSize.y());
    }

    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect);
    GL->glDrawArrays(GL_TRIANGLE_FAN,0,4);
    GL->glDisableClientState(GL_VERTEX_ARRAY);

    // Render black colorbar border
    NERENDERMAN.GLManager()->bindPositionShader(surface);
    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glColor4f(0,0,0,1);
    NERENDERMAN.GLManager()->updatePositionUniforms(surface);
    GL->glLineWidth(1.5f);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect_2D);
    GL->glDrawArrays(GL_LINE_LOOP,0,4);
    GL->glLineWidth(1.0f);
    GL->glDisableClientState(GL_VERTEX_ARRAY);

#if USE_TICK_LARGE_MIDDLE_SHORT
    // Render Ruler
    int rulerLeft   = colorBarRect_2D[0].x(); // point of index 0 is bottomleft(or topleft)
    int rulerTop    = colorBarRect_2D[0].y();
    int rulerRight  = colorBarRect_2D[1].x(); // point of index 1 is bottomright(or bottomleft)
    int rulerbottom = colorBarRect_2D[1].y();
    int tickWidthL  = 5; // wRes / 10 + 1;
    int tickWidthM  = 3; // wRes / 20 + 1;
    int tickWidthS  = 3; // wRes / 25 + 1;
    int tickHeightL = 0;
    int tickHeightM = 0;
    int tickHeightS = 0;
    if (enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Top ||
        enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Bottom)
    {
        tickWidthL = 0;
        tickWidthM = 0;
        tickWidthS = 0;
        tickHeightL = 5; // hRes / 10 + 1;
        tickHeightM = 3; // hRes / 20 + 1;
        tickHeightS = 3; // hRes / 25 + 1;
    }
//    glLineWidth(2.0f);
    vec3f q3_1[2], q3_2[2];
    float offsetX, offsetY;

    glEnableClientState(GL_VERTEX_ARRAY);

    for( int i = 0; i <= 10 * osdRigidTopography->TickCount(); i++)
    {
        float s = i / 10.0 / osdRigidTopography->TickCount();
        if (enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Top ||
            enumOrientation == NEOSDRigidTopography::NEOSDOrientation::Bottom)
        {
            offsetX = s * wRes; offsetY = 0;
        } else
        {
            offsetX = 0; offsetY = s * hRes;
        }

        q3_1[0] = vec2f(rulerLeft + offsetX, rulerTop + offsetY);
        q3_2[0] = vec2f(rulerRight + offsetX, rulerbottom + offsetY);
        if (i % 10 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthL, rulerTop + offsetY - tickHeightL);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthL, rulerbottom + offsetY + tickHeightL);
        }
        else if (i % 5 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthM, rulerTop + offsetY - tickHeightM);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthM, rulerbottom + offsetY + tickHeightM);
        }
        else
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthS, rulerTop + offsetY - tickHeightS);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthS, rulerbottom + offsetY + tickHeightS);
        }
        glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_1);
        glDrawArrays(GL_LINE_LOOP,0,2);
        glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_2);
        glDrawArrays(GL_LINE_LOOP,0,2);
    }
//    glLineWidth(1.0f);
    glDisableClientState(GL_VERTEX_ARRAY);
#endif

    // Draw Title Text
    surface->setShaderProgram(0);
    GL->glDisable(GL_BLEND);
    NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );
    float titleTextWidth = osdRigidTopography->TitleText().length() * osdRigidTopography->fontWidth();
    if (enumOrientation != NEOSDRigidTopography::NEOSDOrientation::Top &&
        enumOrientation != NEOSDRigidTopography::NEOSDOrientation::Bottom)
    {
        glPrint(osdRigidTopography->TitleText(),
                xRes - titleTextWidth / 2,
                yRes + hRes + hTitle - OsdSize.y() / 2,
                osdRigidTopography->fontColor(),
                osdRigidTopography->getTextureFont(), osdRigidTopography->OutlineColor(), osdRigidTopography->getTextureOutlineFont());
    } else {
        glPrint(osdRigidTopography->TitleText(),
                xRes - titleTextWidth / 2,
                yRes - osdRigidTopography->fontHeight() + OsdSize.y() / 2,
                osdRigidTopography->fontColor(),
                osdRigidTopography->getTextureFont(), osdRigidTopography->OutlineColor(), osdRigidTopography->getTextureOutlineFont());
    }

    // Draw measurement values (1..step)
    for (int i = 0; i <= osdRigidTopography->TickCount(); i++)
    {
        float s = i / (float)osdRigidTopography->TickCount();
        float val = minVal * (1-s) + maxVal * s;
        QString text;
        if( osdRigidTopography->DisplayNotation() == NEOSDBase::Exponent )
        {
            text = QString("%1").arg( (double)val, 2, 'E', 2 );
        }
        else if( osdRigidTopography->DisplayNotation() == NEOSDBase::FloatingPoint )
        {
            text = QString("%1").arg( (double)val, osdRigidTopography->TotalNumberLength(), 'f', osdRigidTopography->FloatTypePrecision() );
        }
        widthU = (float)osdRigidTopography->TotalNumberLength() * osdRigidTopography->fontWidth();
        switch(enumOrientation)
        {
        case NEOSDRigidTopography::NEOSDOrientation::Top:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - OsdSize.y()/2;
            break;
        case NEOSDRigidTopography::NEOSDOrientation::Bottom:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - hTitle - (int)(osdRigidTopography->fontHeight()) + OsdSize.y()/2;
            break;
        case NEOSDRigidTopography::NEOSDOrientation::Right:
            xPos = xRes + (int)(osdRigidTopography->fontWidth()) -  OsdSize.x()/2;
            yPos = yRes + s * hRes - OsdSize.y()/2;
            break;
        case NEOSDRigidTopography::NEOSDOrientation::Left:
        default:
            xPos = xRes + wRes + wMargin -  OsdSize.x() / 2;
            yPos = yRes + s * hRes - OsdSize.y() / 2;
            break;
        }
        glPrint(text, xPos, yPos,
                osdRigidTopography->fontColor(),
                osdRigidTopography->getTextureFont(), osdRigidTopography->OutlineColor(), osdRigidTopography->getTextureOutlineFont());
    }

    GL->glPopAttrib();
}


NEOSDRigidTopographyRendererFactory::NEOSDRigidTopographyRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEOSDRigidTopographyRendererFactory::nodeName() { return "Rigid Topography OSD Renderer"; }
NEObject*NEOSDRigidTopographyRendererFactory::createInstance() { return new NEOSDRigidTopographyRenderer(); }




