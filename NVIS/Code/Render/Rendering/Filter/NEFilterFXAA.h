// -------------------------------------------------------------------------- //
//! @file   NEFilterFXAA.h
//! @brief  PostFilter FXAA:FX Antialiasing
//! @author Nal
//! @since  17/06/19(Mon)
//!
// -------------------------------------------------------------------------- //
#pragma	once
/* ====< Include >====================================================== */
#include "NEFilterBase.h"

/* ========================================================================= */
//		FilterFXAA															 */
/* ========================================================================= */
/*class NEFilterFXAA : public NEFilterBase
{
public:
    NEFilterFXAA();
    virtual ~NEFilterFXAA();

public:	
	// -----------------------------------------------------------------
    //		Function
	// -----------------------------------------------------------------
    //!	Filter creation
	void	createBuffer();
    //!	Filter deletion
	void	deleteBuffer();
    //!	Update render target
	void	updateRT( RenderTex* pRTSrc );
    //!	Render
	void	render();

    //!	Is it enabled to process
	virtual bool	isEnable() const;

    //!	Set shader
    virtual void	setEffect( const std::string& strName );

private:
	// -----------------------------------------------------------------
    //		Define
	// -----------------------------------------------------------------

	// -----------------------------------------------------------------
    //		Function
	// -----------------------------------------------------------------
    //!	Render primitive
	void	drawPrimitive( const char* pTechnique );

	// -----------------------------------------------------------------
    //		Variables
	// -----------------------------------------------------------------
    ShaderEffect*	mpEffect;	//!< Shader
    RenderTex*		mpLumaRT;	//!< Luminance render target
};*/
/* ========================================================================= */
/*		EOF																	 */
/* ========================================================================= */
