TEMPLATE = subdirs

SUBDIRS = \
Geometry \
Render \
RenderNEO \
GLIW \
UIUtilities \
UIProperties \
UI \
App


CONFIG += ordered recursive

Geometry.depends = Core
Render.depends = Core Geometry
RenderNEO.depends = Core Geometry Render PostProcessors
GLIW.depends = Core Geometry Render
UIUtilities.depends = Core
UIProperties.depends = Core UIUtilities
UI.depends  = Core Geometry  Render RenderNEO  UIUtilities UIProperties
App.depends = Core Geometry Render RenderNEO UI


Core.file = Core/NECore.pro
Geometry.file = Geometry/NEGeometry.pro
GLIW.file = GLI/NEGLIW.pro
Render.file = Render/NERender.pro
RenderNEO.file = RenderNEO/NERenderNEO.pro
UIUtilities.file = UIUtilities/NEUIUtilities.pro
UIProperties.file = UIProperties/NEUIProperties.pro
UI.file   = UI/NEUI.pro
App.file  = AppMain/NEAppMain.pro



