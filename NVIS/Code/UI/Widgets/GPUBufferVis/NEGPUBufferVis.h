#pragma once

class QGraphicsPathItem;
class QScrollArea;
class QVBoxLayout;


class NEGUIManager;
struct NEGLBufferTab;
struct NEGLBufferData;
struct ShaderMetaInfo;

enum GLBufferType
{
    kLuminance,
    kRGB,
    kTexture
};


class NEGPUBufferVis : public QTabWidget
{
    friend class NEGUIManager;
    friend class NEGUIManagerPrivate;
public:
    NEGPUBufferVis(QWidget* parent);
    ~NEGPUBufferVis();

    void addBuffer(QString name);
    void updateBufferLuminance(QString name, float* buf);
    void updateBufferTexture(QString name, GLuint textureId, bool saveFrame = false, bool mapColors=false, int srcChannel=0, vec3f destWeights=vec3f(1.0,1.0,1.0));
    void removeBuffer(QString name);
    void removeAllBuffers();
    //void setLastSavedFrame(int val) { m_LastSavedFrame = val; }

private:
    void initWidgets();
    QMap<QString, NEGLBufferTab*> m_BufferTab;
    QMenu* m_pMenu;
    QVector<QString> m_BufferNames;
    //ShaderMetaInfo* m_ShaderColorMapper;
    //int m_LastSavedFrame;
    //int lastSavedFrame() const { return m_LastSavedFrame; }

    // TODO: Initialize these values with the selected widget's values
    int glWidth;
    int glHeight;


protected:
    void contextMenuEvent(QContextMenuEvent* event);


};


class SimpleGLWidget;
struct NEGLBufferTab
{
    QString name;
    QWidget* tabWidget;
    QVBoxLayout* verticalLayout;
    QScrollArea* scrollArea;
    SimpleGLWidget* glWidget;
    int m_LastSavedFrame;
    //QAction* m_pSaveAction;
    //QAction* m_pUpdateAction;

};

class SimpleGLWidget : public QOpenGLWidget
{
public:
    SimpleGLWidget(QWidget* parent = 0);
    ~SimpleGLWidget();

    GLBufferType m_BufType;
    float* m_Buffer;
    GLuint m_TexId;

    // Below three varaibles are used by a shader to map a color component to other color components
    bool m_MapColors;
    int m_SourceChannel;
    vec3f m_DestWeights;

    int glWidth;
    int glHeight;


protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
};
