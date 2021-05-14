#pragma once

#include "Manager/NEManager.h"
#include "Rendering/Manager/NEGLManager.h"

class NEGLOfflineRenderer;

NE_FWD_DECL_6(class, NECamera, NELight, NEMaterial, NENodeFactory, NESceneManager, NE_PRIVATE(NERenderManager))
NE_FWD_DECL_5(class, NESpatial, NERendererFactory, NESpatialRenderer, NEClipPlane, NERenderSetting)

NE_FWD_DECL_1( class, NEGLManager )

//! Manages all render related nodes.
/*!
	A more elaborate class description.
*/

class NECEXP_NERENDERING NERenderManager  : public NEManager
{
    Q_OBJECT

public:

    static NERenderManager& Instance( std::shared_ptr<NERenderManager> renderManagerInstance = 0 );


    NERenderManager();
    ~NERenderManager();

    virtual NERenderSetting &RenderSetting();

    //!
    //! Deletes an object and removes all dependencies.
    //!
    NENode* createObject(QString type, QString name, const NEObject* parent);
    NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
    NENode* deleteObject(const QString& type ,const QString& name);
    NESpatialRenderer* getRenderer(NESpatial* spatial);
    const QMap<QString, NECamera*>& cameras() const;
    const QMap<QString, NEMaterial*>& materials() const;
    const QMap<QString, NELight*>& lights() const;
    const QMap<QString, NEClipPlane*>& clipPlanes() const;


    void renderScenePerFrame(IGLSurface* surface, float width, float height, float devPixelRation = 1);
    void renderOffline(IGLSurface *surface, QString cameraName, QSize resolution, QString path, float framerate, float speed);
    void renderOffline(QList<IGLSurface *> surfaces, QList<QString> cameraName, QList<QSize> resolution, QList<QString> paths, float framerate, float speed);
    void renderOffline(QList<IGLSurface *> surfaces, QList<QString> cameraName, QList<QRect> layouts, QSize totalResolution, QString path, float framerate, float speed);

    QList<NEGLOfflineRenderer *> m_offlineRenderers;
    IGLSurface *m_offlineRenderSurface;

    bool m_bDrawGrid;

public slots:

    uint init();
    void deInit();

    void initRenderer();
    void deInitRenderer();

    virtual unsigned int update();
    NESpatialRenderer* createRenderer(NESpatial* spatial);

    //! Saves the current frame in a format specific for the selected external renderer, e.g PovRay, mental ray.
    virtual uint exportFrameToExternalRenderer( long long glWidget, bool startFrame = true, bool renderNow=true );

    //! Transforms the sequence (min to max frame) into a format readable by the specified external ray tracer / renderer and saves it to disc.
    virtual uint exportSequenceToExternalRenderer( long long glWidgets, long long  renderCameras,
                                                   long long  renderRects, int tWidth, int tHeight,
                                                   int startFrame, int endFrame);

    virtual uint exportSequenceToExternalRenderer();

    //! Transforms the sequence (min to max frame) into a format readable by the specified external ray tracer,
    //! renders and immediately deletes the current frame and continues with the next.
    virtual uint exportSequenceRenderDelete(int startFrame, int endFrame);

    //! Exports the scene for pbrt
    uint exportToPBRT(void);


    virtual void initBasicSceneElements();



    void render();

    //! Flag that controls whether the sequence between first and last playback frames should be rendered
    bool renderSequence() const;
    void setRenderSequence(bool val);

    void setDrawGrid(bool val);
    bool bDrawGrid(){return m_bDrawGrid;}

    //! Returns pointers to the static cameras
    NECamera* defaultCamera() const;
    NECamera* topCamera() const;
    NECamera* leftCamera() const;
    NECamera* frontCamera() const;

    NEGLManager* GLManager() const;

signals:
    //! Signals
    void managerCreated( NEManager* pManager );
    void getActiveCameraAndResolution( QMap<QString, QSize> &activeCameras );

private:
    NE_DECL_PRIVATE(NERenderManager)

public slots:
    //! This is used for loading renderer into Neutrino
    void addRendererToMaps(NERendererFactory* factory);
    void addDynamicRendererToMaps(QString path);


    void deleteChildNode(NENode* node);
};

#define NERENDERMAN NERenderManager::Instance()
