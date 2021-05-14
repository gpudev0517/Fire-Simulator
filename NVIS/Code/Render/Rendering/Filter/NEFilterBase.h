// -------------------------------------------------------------------------- //
//! @file   NEFilterBase.h
//! @brief  Post Filter Interface
//! @author Nal
//! @since  17/06/18(Sun)
//!
// -------------------------------------------------------------------------- //
#pragma	once
/* ====< Include >====================================================== */

/* ========================================================================= */
//		NEFilterBase														 */
/* ========================================================================= */
class NEFilterBase
{
public:	
    //!	Constructor
    NEFilterBase()
	:	mbEnable(true)
	{}
	
    //!	Destructor
    virtual ~NEFilterBase() {}

	// -----------------------------------------------------------------
    //		Function
	// -----------------------------------------------------------------
    //!	Buffer creation
	virtual void	createBuffer() = 0;
    //! Buffer deletion
	virtual void	deleteBuffer() = 0;

    //!	Set shader
    virtual void	setEffect( const std::string& strName ) = 0;

    //!	Is it enabled
	virtual bool	isEnable() const	{ return mbEnable; }
	bool	isEnableValue() const		{ return mbEnable; }
	void	setEnableValue( bool b0 ) 	{ mbEnable = b0; }

protected:
	// -----------------------------------------------------------------
    //		Variables
	// -----------------------------------------------------------------
    bool	mbEnable;	//!< Enable
};
/* ========================================================================= */
/*		EOF																	 */
/* ========================================================================= */
