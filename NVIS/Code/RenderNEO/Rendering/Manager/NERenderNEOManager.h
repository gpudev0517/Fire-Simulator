#pragma once

#include "Rendering/Manager/NERenderManager.h"

NE_FWD_DECL_1( class, NE_PRIVATE(NERenderNEOManager))
class NERenderSetting;

//! Manages all render related nodes.
/*!
	A more elaborate class description.
*/
class NECEXP_NERENDERNEO NERenderNEOManager  : public NERenderManager
{
    Q_OBJECT

public:

    NERenderNEOManager();
    ~NERenderNEOManager();

public slots:
    uint init();
    void initBasicSceneElements();

private:
    NE_DECL_PRIVATE(NERenderNEOManager)
};

