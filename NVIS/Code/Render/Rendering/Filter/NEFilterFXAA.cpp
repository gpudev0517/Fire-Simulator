// -------------------------------------------------------------------------- //
//! @file   NEFilterFXAA.cpp
//! @brief  PostFilter FXAA:FX Antialiasing
//! @author Nal
//! @since  17/06/19(Mon)
//!
// -------------------------------------------------------------------------- //
/* ====< Include >====================================================== */
#include "NEFilterFXAA.h"

/* ========================================================================= */
/*		NEFilterFXAA															 */
/* ========================================================================= */
// ---------------------------------------------
//! @brief  Constructor "NEFilterFXAA"
// ---------------------------------------------
/*NEFilterFXAA::NEFilterFXAA()
:	mpEffect(NULL), mpLumaRT(NULL)
{
}

// ---------------------------------------------
//! @brief  デストラクタ "FilterFXAA"
// ---------------------------------------------
NEFilterFXAA::~NEFilterFXAA()
{
	deleteBuffer();
}

// ---------------------------------------------
//! @brief  Create buffer
// ---------------------------------------------
void NEFilterFXAA::createBuffer()
{
	deleteBuffer();

	//!	レンダーターゲット生成
	mpLumaRT = SYS_NEW RenderTex( IGraphics->getBackBufferSize(), RenderState::FORMAT_ARGB8 );

#ifdef _ENABLE_DEBUG_SURFACE
	DebugGUI::TextureViewer* pTextureViewer = null;
	//!	輝度
	pTextureViewer = DebugGUI::TextureViewer::Create("FXAA輝度");
	pTextureViewer->SetTexture( mpLumaRT->Get2DObj() );
	s_DebugRTWindow.push_back( pTextureViewer );
#endif
}

// ---------------------------------------------
//! @brief  Delete buffer
// ---------------------------------------------
void NEFilterFXAA::deleteBuffer()
{
	SAFE_DELETE( mpLumaRT );

#ifdef _WINDOWS
	foreach( DEBUG_RT_ARRAY, s_DebugRTWindow, it )
		SAFE_DELETE(*it);
#endif
}

// ---------------------------------------------
//! @brief  Allocate shader effect
//! @param  strName	[in]	Shader name
// ---------------------------------------------
void NEFilterFXAA::setEffect( const stl::string& strName )
{
	mpEffect = IShader->getShader( strName );
}

// ---------------------------------------------
//! @brief  Update render target
//! @param  pRTSrc	[i/o]	Source render target
// ---------------------------------------------
void NEFilterFXAA::updateRT( RenderTex* pRTSrc )
{
#ifdef DX9
	if( !isEnable() )	return;

	//!	バッファ生成
	if( !mpLumaRT )
		createBuffer();

	PIX_BEGIN_EVENT(IRGBA(0xff, 0,0,0xff), "FXAA_Pass1_Luma");

	//!	1Pass : 輝度計算
	mpLumaRT->setRenderTarget();
	mpLumaRT->clear();

	IGraphics->setViewport( mpLumaRT->getViewPort() );	//!< 描画用ビューポート
	mpEffect->setTexture( "gScreenTexture", pRTSrc );
	IGraphics->getRS().setBlend( RSBL_HALF, false );

	drawPrimitive( "Luma" );

	PIX_END_EVENT();
#endif
}

// ---------------------------------------------
//! @brief  Render
// ---------------------------------------------
void NEFilterFXAA::render()
{
#ifdef DX9
	if( !isEnable() )	return;
	PIX_BEGIN_EVENT(IRGBA(0xff, 0,0,0xff), "FXAA_Final");

	const f32	SCREEN_W = IGraphics->getBackBufferSize().x;
	const f32	SCREEN_H = IGraphics->getBackBufferSize().y;
	Vec2	vRcpFrame( 1.0f/SCREEN_W, 1.0f/SCREEN_H );
	Vec4	vRcpFrameOpt( 2.0f/SCREEN_W, 2.0f/SCREEN_H, 0.5f/SCREEN_W, 0.5f/SCREEN_H );
	
	mpEffect->setValue( "gRcpFrame",		&vRcpFrame,		sizeof(Vec2) );
	mpEffect->setValue( "gRcpFrameOpt",		&vRcpFrameOpt,	sizeof(Vec4) );
	mpEffect->setTexture( "gLumaTexture",	mpLumaRT );

	IGraphics->getRS().setBlend( RSBL_HALF, true );
	drawPrimitive( "FXAA" );

	PIX_END_EVENT();
#endif
}

// ---------------------------------------------
//! @brief  Render primitive
//! @param  pTechnique	[in]	Technique name
// ---------------------------------------------
void NEFilterFXAA::drawPrimitive( const char* pTechnique )
{
#ifdef DX9
	//!	テクニック
	mpEffect->setTechnique( pTechnique );
	
	mpEffect->begin( 0, 0 );

	//!	頂点設定
	Vec2	pixelSize = Vec2(1.0f / IGraphics->getBackBufferSize().x, 1.0f / IGraphics->getBackBufferSize().y) * 0.25f;
    f32	quad[4][5] = {
        { -1.0f+pixelSize.x,  1.0f-pixelSize.y, 0.5f,	0.0f, 0.0f },
        {  1.0f+pixelSize.x,  1.0f-pixelSize.y, 0.5f,	1.0f, 0.0f },
        { -1.0f+pixelSize.x, -1.0f-pixelSize.y, 0.5f,	0.0f, 1.0f },
        {  1.0f+pixelSize.x, -1.0f-pixelSize.y, 0.5f,	1.0f, 1.0f }
    };

	IVertex->setVertexLayout( Vertex::VTXDEC_TEX, quad, 4, mpEffect );
	IGraphics->draw( Graphics::PRIM_TRIANGLESTRIP );

	mpEffect->end();
#endif
}

// ---------------------------------------------
//! @brief  処理可能な状態か
//! @return true:可, false:不可
// ---------------------------------------------
bool NEFilterFXAA::isEnable() const
{
	if( !FilterBase::isEnable() )	return false;
	if( !mpEffect )	return false;
	//!	品質チェック
	if( ITGLSystem->getAA() != SYSINFOAA_FXAA )	return false;
	//!	シェーダーバージョンチェック
//	if( !IGraphics->isPixelShaderVersion( D3DPS_VERSION(3,0) ) )	return false;
	
	return true;
}*/
/* ========================================================================= */
/*		EOF																	 */
/* ========================================================================= */
