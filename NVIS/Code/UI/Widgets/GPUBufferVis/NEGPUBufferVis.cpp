#include "NEGPUBufferVis.h"
#include "Managers/NEGUIManager.h"
#include "Managers/NEGUIManager_p.h"
#include "NEGLWidget.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

NEGPUBufferVis::NEGPUBufferVis(QWidget* parent) : QTabWidget(parent)
{
    //setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
    m_pMenu = 0;
    glWidth = 1024;
    glHeight = 768;

    //m_LastSavedFrame = -1;

    // Initialize color mapper shader
}

NEGPUBufferVis::~NEGPUBufferVis()
{

}

void NEGPUBufferVis::addBuffer( QString name)
{
    m_BufferNames.push_back(name);
}

void NEGPUBufferVis::updateBufferLuminance( QString name, float* buf )
{
    QMap<QString, NEGLBufferTab*>::iterator i = m_BufferTab.find(name);

    if(i == m_BufferTab.end())
        return;

    int w = glWidth;
    int h = glHeight;

    (*i)->glWidget->m_BufType = kLuminance;

    if((*i)->glWidget->m_Buffer)
        delete [] (*i)->glWidget->m_Buffer;

    (*i)->glWidget->m_Buffer = new float[w * h];

    memcpy((*i)->glWidget->m_Buffer, buf, w*h*sizeof(float));

    (*i)->glWidget->makeCurrent();
    (*i)->glWidget->update();

    NEGUI.P->mainGLWidget()->makeCurrent();

}

void NEGPUBufferVis::updateBufferTexture( QString name, GLuint textureId, bool saveFrame /*= false*/, bool mapColors/*=false*/, int srcChannel/*=0*/, vec3f destWeights/*=vec3f(1.0,1.0,1.0)*/ )
{
    QMap<QString, NEGLBufferTab*>::iterator i = m_BufferTab.find(name);

    if(i == m_BufferTab.end())
    {
        //m_LastSavedFrame = NESCENE.getCurrentFrame();
        return;
    }

    (*i)->glWidget->m_BufType = kTexture;
    (*i)->glWidget->m_TexId = textureId;
    (*i)->glWidget->m_MapColors = mapColors;
    (*i)->glWidget->m_SourceChannel = srcChannel;
    (*i)->glWidget->m_DestWeights = destWeights;

    (*i)->glWidget->makeCurrent();
    //if((*i)->m_pUpdateAction->isChecked())
    (*i)->glWidget->update();

    (*i)->glWidget->setFixedSize(glWidth, glHeight);

    if(NEGUI.SaveGLFrames() && saveFrame && ((*i)->m_LastSavedFrame !=NESCENE.CurrentFrame()) && NESCENE.isUpdating())
    {

        QImage img = (*i)->glWidget->grabFramebuffer();
        QDir saveDir(NEBASE.CacheDir());
        saveDir.mkpath("GLBufferData/" + name);
        QString frameNr;
        frameNr.sprintf("%06d.png", NESCENE.CurrentFrame());
        QString savePath = NEBASE.CacheDir() + "/GLBufferData/" + name + "/" + frameNr;
        img.save(savePath);
        Log() << "GLLogDataPath: " << savePath; EndLog(NE::kLogInfo);
        (*i)->m_LastSavedFrame = NESCENE.CurrentFrame();
    }

    //if(i == m_BufferTab.end()-1)
    //{
    //	m_LastSavedFrame = NESCENE.getCurrentFrame();
    //}


    NEGUI.P->mainGLWidget()->makeCurrent();
}

void NEGPUBufferVis::removeBuffer(QString name)
{
    //QMap<QString, NEGLBufferTab*>::iterator i = m_BufferTab.find(name);

    //if(i != m_BufferTab.end())
    //	return;

    //NEGLBufferTab* b =  m_BufferTab[name];
    //this->removeTab(this->indexOf(b->tabWidget));
    //delete b->tabWidget;
    //
    //m_BufferTab.erase(i);
}

void NEGPUBufferVis::removeAllBuffers()
{

}

void NEGPUBufferVis::initWidgets()
{
    m_pMenu = new QMenu();
    QMenu* pMenuSaveFrames = new QMenu("Save Frames");
    m_pMenu->addMenu(pMenuSaveFrames);

    for(int i=0; i<m_BufferNames.size(); i++)
    {
        const QString& name = m_BufferNames[i];

        QMap<QString, NEGLBufferTab*>::iterator iter = m_BufferTab.find(name);

        if(iter != m_BufferTab.end())
            return;

        NEGLBufferTab* b = new NEGLBufferTab();
        b->m_LastSavedFrame = -1;
        //b->texId = texId;
        b->name = name;

        b->tabWidget = new QWidget();

        b->verticalLayout = new QVBoxLayout(b->tabWidget);

        b->scrollArea = new QScrollArea(b->tabWidget);
        b->scrollArea->setWidgetResizable(true);

        b->glWidget = new SimpleGLWidget(b->scrollArea);
        //b->glWidget->m_TexId = texUd

        b->scrollArea->setWidget(b->glWidget);

        b->verticalLayout->addWidget(b->scrollArea);

        this->addTab(b->tabWidget, name);

        m_BufferTab[name] = b;

        //b->m_pSaveAction = new QAction(m_BufferNames[i], this);
        //b->m_pUpdateAction = new QAction(m_BufferNames[i], this);
        //b->m_pSaveAction->setCheckable(true);
        //b->m_pUpdateAction->setCheckable(true);
        //b->m_pUpdateAction->setChecked


        //pMenuSaveFrames->addAction(b->m_pSaveAction);
    }
}

void NEGPUBufferVis::contextMenuEvent( QContextMenuEvent* event )
{
    //m_pMenu->show();

}

SimpleGLWidget::SimpleGLWidget( QWidget* parent )
    :QOpenGLWidget(parent)
{
    /*QOpenGLFormat fmt = format();
        fmt.setProfile(QOpenGLFormat::CompatibilityProfile);
        fmt.setVersion(4,1);
    setFormat(fmt);*/

    glWidth = 1024;
    glHeight = 768;

    this->makeCurrent();	// Make this widget's OpenGL context the current context

    m_Buffer = 0;
    m_TexId = 0;

}

SimpleGLWidget::~SimpleGLWidget()
{

}

void SimpleGLWidget::initializeGL()
{
}

void SimpleGLWidget::resizeGL( int w, int h )
{
    GL->glViewport(0, 0, glWidth, glHeight);
    GL->glMatrixMode(GL_PROJECTION);
    GL->glLoadIdentity();
    GL->glOrtho(0, glWidth, 0, glHeight, -10, 10);
    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();
}

void SimpleGLWidget::paintGL()
{

    resize(glWidth, glHeight);
    switch(m_BufType)
    {
    case kLuminance:
    {
        if(!m_Buffer)
            return;
        if(m_BufType == kLuminance)
            GL->glDrawPixels(glWidth, glHeight, GL_LUMINANCE, GL_FLOAT, m_Buffer);
        break;
    }
    case kTexture:
    {
        //if(m_TexId >= 0)
        {
            if(m_MapColors == false)
            {
                //drawTexture(QRectF(QPointF(0, glHeight), QPointF(glWidth, 0)), m_TexId);
            }
            else
            {

                GL->glActiveTexture(GL_TEXTURE11);
                GL->glBindTexture(GL_TEXTURE_2D, m_TexId);

                //NERENDERMAN.GLManager()->bindColorMapperShader(this);

                //gustaw tmp
                //surface->shaderProgram()->setUniformValue("width", (float)glWidth);
                //surface->shaderProgram()->setUniformValue("height", (float)glHeight);
                //surface->shaderProgram()->setUniformValue("sourceChannel", m_SourceChannel);

                QVector3D* destWPtr = reinterpret_cast<QVector3D*>(&m_DestWeights);
                //surface->shaderProgram()->setUniformValue("destWeights", *(destWPtr));
                //surface->shaderProgram()->setUniformValue("tex0", 11);

                GL->glRectf(0, 0, (float)glWidth, (float)glHeight);
                //drawTexture(QRectF(QPointF(0, glHeight), QPointF(glWidth, 0)), m_TexId);

                //surface->setShaderProgram(0);
            }
        }
        break;
    default:
            break;
        }
    }
    GL->glFinish();
}
