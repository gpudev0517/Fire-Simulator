#pragma once

#include "Spatial/NESpatial.h"

class NEOSDBasePrivate;
class FTTextureFont;
class FTOutlineFont;


//! Base class for OSD spatial object
//! Subclass of this class will have gizmo with view coordinate system
class NECEXP_NERENDERING NEOSDBase: public NESpatial
{
    Q_OBJECT
    NE_CLASS("2D OSD")

public:
    enum NEFontName {
                        Consola = 0,
                        CourierNew = 1,
                        RobotaLight = 2,
                        RobotaRegular= 3,
                        Monofonto = 4,
                        Anonymous = 5,
                        VaguelyFatal = 6,
                        Monspace_CD_LT= 7,
                        Monospace_CD_RG = 8,
                        Monospace_NO_BK = 9,
                        Monospace_NO_LT = 10,
                        Monospace_NO_RG = 11,
                        Monospace_SC_BD = 12,
                        Monospace_SC_BK = 14,
                        Monospace_SC_LT = 15,
                        Monospace_SC_RG = 16,
                        Monospace_SC_SB = 17,
                        Monospace_SE_BK = 18,
                        Monospace_SE_LT = 19,
                        Monospace_SE_RG = 20,
                        Minim = 21};

    enum NEDisplayNotation {
                Exponent = 0,
                FloatingPoint = 1 };

    NE_PROP_RW(NEFontName, FontName, "Font Name", "Font Name", true,)
    NE_PROP_RW(int, FontSize, "Font Size", "Font Size", true,)
    NE_PROP_RW(QColor, FontColor, "Font Color", "Font Color", true,)
    NE_PROP_RW(NEDisplayNotation, DisplayNotation, "Display Notation", "The scheme to use to display values", true,)
    NE_PROP_RW(int, FloatTypePrecision, "Floating point precision", "The scheme to use to display values(value range is 2..4)", isDisplayNotationFloatingPoint,)
    NE_PROP_RW(int, TotalNumberLength, "Total Number Length", "The scheme to use to display values(value range is 2,3,4,5 etc...)", isDisplayNotationFloatingPoint,)
    NE_PROP_RW(bool, TextOutline, "Show Outline", "Shows Outline of All Text.", true,)
    NE_SUB_PROP_START(TextOutlineParams, true)
        NE_PROP_RW(QColor, OutlineColor, "Outline Color", "Color of the Outline.", true,)
    NE_SUB_PROP_END(TextOutlineParams)
    Q_ENUMS(NEFontName)
    Q_ENUMS(NEDisplayNotation)

public:
    NEOSDBase(NENodeFactory *factory);
    ~NEOSDBase();

    virtual void render();
    virtual void renderPickable();
    virtual void renderPicked();
    virtual bool isManipulatable(NEManipulator manipType);
    virtual NESpatialType spatialType();
    void drawQuad(int x1, int y1, int x2, int y2);
    void glPrint(QString name, int x, int y, QColor color = QColor(255, 255, 255));

    virtual void initFont();
    virtual void deInitFont();
    virtual FTTextureFont* getTextureFont();
    virtual FTOutlineFont* getTextureOutlineFont();
    //
    // Rendering related properties here
    //
    virtual vec2f osdSize();         //!< Total size of osd

    virtual float fontHeight();     //!< Font height
    virtual float fontWidth();      //!< Font width
    virtual int leftMargin();       //!< Left and top margin of OSD
    virtual int topMargin();
    virtual QColor fontColor();     //!< Font color
    virtual bool isDisplayNotationFloatingPoint() { return DisplayNotation() == NEDisplayNotation::FloatingPoint;}

public slots:
    //! Resets all properties totheir default values
    virtual void resetProperties();

protected:
    //
    // OSD Related functions here...
    //

private:
    NE_DECL_PRIVATE(NEOSDBase)

};



// This is necessary to make a plugin instance castable to NESpatial type
Q_DECLARE_INTERFACE(NEOSDBase, "NEOSDBase")
