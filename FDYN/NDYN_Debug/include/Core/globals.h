#pragma once

//! Used to prevent compiler warnings for unused variables
#define _unused(a) ((void)a)
//! Puts the expression in the parantheses into quatation
#define STRINGIFY(a) #a

#define NEFEXP_PYTHONQT Q_DECL_EXPORT
#define DLLEXPORT Q_DECL_EXPORT

#ifdef WIN32
#define Inline __forceinline
#else
#define Inline inline
#endif

#if defined(WIN64) || defined(_WIN64) || defined(__LP64__) || defined(_LP64)
#define NE_BUILD_64
#else
#define NE_BUILD_32
#endif

#if defined(WIN32) || defined(WIN64)
#define NE_FOR_WINDOWS
#elif defined(__APPLE__)
#define NE_FOR_MAC
#elif defined(__GNUC__)
#define NE_FOR_LINUX
#endif

#define EXEC_IF(_Cond_, _Exp_) \
    if(_Cond_) \
    _Exp_

//! Macro for accessing default properties
#define NE_DEFAULT(_Prop_) \
    s_Default_##_Prop_

//! Installs properties, their default values, and gererates property reset function
#define NE_PROP_DEF(_Type_, _Class_, _Prop_, _Val_) \
    static int s_IX_##_Prop_; \
    static const _Type_ s_Default_##_Prop_ = _Val_; \
    void _Class_::Reset##_Prop_(){ Set##_Prop_(NE_DEFAULT(_Prop_)); }

#define NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, _SubP_, _Loc_) \
    _PType_ _Class_::_Prop_##_SubP_() const { return _Loc_->m_##_Prop_._SubP_(); }

//! Installs vec2f properties, its subproperties, their default values, and generates property reset functions
//! and generates subproperty get/set functions
#define NE_PROP_DEF_VEC2(_Type_, _PType_, _Class_, _Prop_, _Val_, _Loc_) \
    NE_PROP_DEF(_Type_, _Class_, _Prop_, _Val_) \
    static int s_IX_##_Prop_##x; \
    static int s_IX_##_Prop_##y; \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, x, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, y, _Loc_) \
    uint _Class_::Set##_Prop_##x(_PType_ x) \
{                               \
    if(_Loc_->m_##_Prop_.x() == x)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(x, _Loc_->m_##_Prop_.y())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##y(_PType_ y) \
{                               \
    if(_Loc_->m_##_Prop_.y() == y)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.x(), y)); \
    return NE::kReturnSuccess; \
    }

//! Installs vec3f properties, its subproperties, their default values, and generates property reset functions
//! and generates subproperty get/set functions
#define NE_PROP_DEF_VEC3(_Type_, _PType_, _Class_, _Prop_, _Val_, _Loc_) \
    NE_PROP_DEF(_Type_, _Class_, _Prop_, _Val_) \
    static int s_IX_##_Prop_##x; \
    static int s_IX_##_Prop_##y; \
    static int s_IX_##_Prop_##z; \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, x, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, y, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, z, _Loc_) \
    uint _Class_::Set##_Prop_##x(_PType_ x) \
{                               \
    if(_Loc_->m_##_Prop_.x() == x)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(x, _Loc_->m_##_Prop_.y(), _Loc_->m_##_Prop_.z())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##y(_PType_ y) \
{                               \
    if(_Loc_->m_##_Prop_.y() == y)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.x(), y, _Loc_->m_##_Prop_.z())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##z(_PType_ z) \
{                               \
    if(_Loc_->m_##_Prop_.z() == z)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.x(), _Loc_->m_##_Prop_.y(), z)); \
    return NE::kReturnSuccess; \
    }

//! Installs matrim33d or matrix33f properties, its subproperties, their default values, and generates property reset functions
//! and generates subproperty get/set functions
#define NE_PROP_DEF_MATRIX33(_Type_, _PType_, _Class_, _Prop_, _Val_, _Loc_) \
    NE_PROP_DEF(_Type_, _Class_, _Prop_, _Val_) \
    static int s_IX_##_Prop_##m11; \
    static int s_IX_##_Prop_##m12; \
    static int s_IX_##_Prop_##m13; \
    static int s_IX_##_Prop_##m21; \
    static int s_IX_##_Prop_##m22; \
    static int s_IX_##_Prop_##m23; \
    static int s_IX_##_Prop_##m31; \
    static int s_IX_##_Prop_##m32; \
    static int s_IX_##_Prop_##m33; \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m11, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m12, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m13, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m21, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m22, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m23, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m31, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m32, _Loc_) \
    NE_DEF_SUBP_GET_FUNC(_PType_, _Class_, _Prop_, m33, _Loc_) \
    uint _Class_::Set##_Prop_##m11(_PType_ m11) \
{                               \
    if(_Loc_->m_##_Prop_.m11() == m11)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(m11, _Loc_->m_##_Prop_.m12(), _Loc_->m_##_Prop_.m13() \
                      , _Loc_->m_##_Prop_.m21(), _Loc_->m_##_Prop_.m22(), _Loc_->m_##_Prop_.m23() \
                     , _Loc_->m_##_Prop_.m31(), _Loc_->m_##_Prop_.m32(), _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m12(_PType_ m12) \
{                               \
    if(_Loc_->m_##_Prop_.m12() == m12)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), m12, _Loc_->m_##_Prop_.m13() \
                      , _Loc_->m_##_Prop_.m21(), _Loc_->m_##_Prop_.m22(), _Loc_->m_##_Prop_.m23() \
                     , _Loc_->m_##_Prop_.m31(), _Loc_->m_##_Prop_.m32(), _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m13(_PType_ m13) \
{                               \
    if(_Loc_->m_##_Prop_.m13() == m13)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), _Loc_->m_##_Prop_.m12(), m13 \
                      , _Loc_->m_##_Prop_.m21(), _Loc_->m_##_Prop_.m22(), _Loc_->m_##_Prop_.m23() \
                     , _Loc_->m_##_Prop_.m31(), _Loc_->m_##_Prop_.m32(), _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m21(_PType_ m21) \
{                               \
    if(_Loc_->m_##_Prop_.m21() == m21)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), _Loc_->m_##_Prop_.m12(), _Loc_->m_##_Prop_.m13() \
                      , m21, _Loc_->m_##_Prop_.m22(), _Loc_->m_##_Prop_.m23() \
                     , _Loc_->m_##_Prop_.m31(), _Loc_->m_##_Prop_.m32(), _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m22(_PType_ m22) \
{                               \
    if(_Loc_->m_##_Prop_.m22() == m22)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), _Loc_->m_##_Prop_.m12(), _Loc_->m_##_Prop_.m13() \
                      , _Loc_->m_##_Prop_.m21(), m22, _Loc_->m_##_Prop_.m23() \
                     , _Loc_->m_##_Prop_.m31(), _Loc_->m_##_Prop_.m32(), _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m23(_PType_ m23) \
{                               \
    if(_Loc_->m_##_Prop_.m23() == m23)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), _Loc_->m_##_Prop_.m12(), _Loc_->m_##_Prop_.m13() \
                      , _Loc_->m_##_Prop_.m21(), _Loc_->m_##_Prop_.m22(), m23 \
                     , _Loc_->m_##_Prop_.m31(), _Loc_->m_##_Prop_.m32(), _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m31(_PType_ m31) \
{                               \
    if(_Loc_->m_##_Prop_.m31() == m31)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), _Loc_->m_##_Prop_.m12(), _Loc_->m_##_Prop_.m13() \
                      , _Loc_->m_##_Prop_.m21(), _Loc_->m_##_Prop_.m22(), _Loc_->m_##_Prop_.m23() \
                     , m31, _Loc_->m_##_Prop_.m32(), _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m32(_PType_ m32) \
{                               \
    if(_Loc_->m_##_Prop_.m32() == m32)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), _Loc_->m_##_Prop_.m12(), _Loc_->m_##_Prop_.m13() \
                      , _Loc_->m_##_Prop_.m21(), _Loc_->m_##_Prop_.m22(), _Loc_->m_##_Prop_.m23() \
                     , _Loc_->m_##_Prop_.m31(), m32, _Loc_->m_##_Prop_.m33())); \
    return NE::kReturnSuccess; \
    }               \
    uint _Class_::Set##_Prop_##m33(_PType_ m33) \
{                               \
    if(_Loc_->m_##_Prop_.m33() == m33)  \
    return NE::kReturnNotChanged; \
    Set##_Prop_(_Type_(_Loc_->m_##_Prop_.m11(), _Loc_->m_##_Prop_.m12(), _Loc_->m_##_Prop_.m13() \
                      , _Loc_->m_##_Prop_.m21(), _Loc_->m_##_Prop_.m22(), _Loc_->m_##_Prop_.m23() \
                     , _Loc_->m_##_Prop_.m31(), _Loc_->m_##_Prop_.m32(), m33)); \
    return NE::kReturnSuccess; \
    }

//! Initializes the passed property to Neutrino
#define NE_INIT_PROP(_Class_, _Prop_) \
    s_IX_##_Prop_ = _Class_::staticMetaObject.indexOfProperty(#_Prop_)
//    if(( s_IX_##_Prop_ = _Class_::staticMetaObject.indexOfProperty(#_Prop_)) == -1) \
//    std::cout<<"Property initialization failed for "<<_Class_::staticMetaObject.className()<<"::"<<#_Prop_<<endl

//! Macro for accessing property identifiers
#define NE_PROP(_Prop_) \
    s_IX_##_Prop_

//! Macro for evaluating property expressions
#define EvalPropExps(_Prop_, _Loc_) \
    evalPropExps(NE_PROP(_Prop_)); \
    emit _Prop_##Changed(_Loc_->m_##_Prop_)

//! Macro for evaluating vec2 type, and its subproperties
#define EvalPropExpsVec2(_Prop_, _Loc_) \
    evalPropExps(NE_PROP(_Prop_)); \
    evalPropExps(NE_PROP(_Prop_##x)); \
    evalPropExps(NE_PROP(_Prop_##y)); \
    emit _Prop_##Changed(_Loc_->m_##_Prop_)

//! Macro for evaluating vec3 type, and its subproperties
#define EvalPropExpsVec3(_Prop_, _Loc_) \
    evalPropExps(NE_PROP(_Prop_)); \
    evalPropExps(NE_PROP(_Prop_##x)); \
    evalPropExps(NE_PROP(_Prop_##y)); \
    evalPropExps(NE_PROP(_Prop_##z)); \
    emit _Prop_##Changed(_Loc_->m_##_Prop_)

//! Macro for evaluating matrix33 type, and its subproperties
#define EvalPropExpsMatrix33(_Prop_, _Loc_) \
    evalPropExps(NE_PROP(_Prop_)); \
    evalPropExps(NE_PROP(_Prop_##m11)); \
    evalPropExps(NE_PROP(_Prop_##m12)); \
    evalPropExps(NE_PROP(_Prop_##m13)); \
    evalPropExps(NE_PROP(_Prop_##m21)); \
    evalPropExps(NE_PROP(_Prop_##m22)); \
    evalPropExps(NE_PROP(_Prop_##m23)); \
    evalPropExps(NE_PROP(_Prop_##m31)); \
    evalPropExps(NE_PROP(_Prop_##m32)); \
    evalPropExps(NE_PROP(_Prop_##m33)); \
    emit _Prop_##Changed(_Loc_->m_##_Prop_)

#define NE_PROP_SET_AND_EVAL(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExps(_Prop_, _Loc_)

//! Macro similar to NE_PROP_SET_AND_EVAL
//! but which force returning in case the scene is not initialized
#define NE_PROP_SET_EVAL(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExps(_Prop_, _Loc_); \
    if(!PropertiesInitialized()) \
    return NE::kReturnPropNotInit

//! Macro similar to NE_PROP_SET_AND_NOTIFY
//! but which force returning in case the scene is not initialized
#define NE_PROP_SET_NOTIFY(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    emit _Prop_##Changed(_Loc_->m_##_Prop_); \
    if(!PropertiesInitialized()) \
    return NE::kReturnPropNotInit

#define NE_PROP_SET_AND_NOTIFY(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    emit _Prop_##Changed(_Loc_->m_##_Prop_)

#define NE_PROP_SET(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_;

#define NE_PROP_SET_AND_EVAL_VEC2(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExpsVec2(_Prop_, _Loc_)

//! Macro similar to NE_PROP_SET_AND_EVAL_VEC2
//! but which force returning in case the scene is not initialized
#define NE_PROP_SET_EVAL_VEC2(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExpsVec2(_Prop_, _Loc_); \
    if(!PropertiesInitialized()) \
    return NE::kReturnPropNotInit

#define NE_PROP_SET_AND_EVAL_VEC3(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExpsVec3(_Prop_, _Loc_)

//! Macro similar to NE_PROP_SET_AND_EVAL_VEC3
//! but which force returning in case the scene is not initialized
#define NE_PROP_SET_EVAL_VEC3(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExpsVec3(_Prop_, _Loc_); \
    if(!PropertiesInitialized()) \
    return NE::kReturnPropNotInit

#define NE_PROP_SET_AND_EVAL_MATRIX33(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExpsMatrix33(_Prop_, _Loc_)

//! Macro similar to NE_PROP_SET_AND_EVAL_MATRIX33
//! but which force returning in case the scene is not initialized
#define NE_PROP_SET_EVAL_MATRIX33(_Prop_, _Loc_, _Param_) \
    if(_Loc_->m_##_Prop_ == _Param_) \
    return NE::kReturnNotChanged; \
    _Loc_->m_##_Prop_ = _Param_; \
    EvalPropExpsMatrix33(_Prop_, _Loc_); \
    if(!PropertiesInitialized()) \
    return NE::kReturnPropNotInit

//! Macro for calling the passed function only once
#define CALL_ONCE(_Func_) do {static bool dummy = (_Func_(), true); } while(0)

template <class T>
struct NEPrivateInitializer
{
	NEPrivateInitializer(T** SP, T* P)
	{
		*SP = P;
	}
};

template <class T, class X>
struct NESingletonPrivateInitializer
{
	NESingletonPrivateInitializer(T** SP, T* P, X** ABB, X* S)
	{
		*SP = P;
		*ABB = S;
	}
};

#define NE_DECL_PRIVATE(Class) \
    Class##Private* P; \
    friend class Class##Private;

#define NE_DECL_S(_CLASS_) \
	_CLASS_* S; \
	NEPrivateInitializer<_CLASS_##Private> PI;

#define NE_DECL_SINGLETON_S(_CLASS_) \
	_CLASS_* S; \
	NESingletonPrivateInitializer<_CLASS_##Private, _CLASS_> PI;

#define NE_INIT_SINGLETON_S(_ABBRV_)\
	S(s), PI(&S->P, this, &_ABBRV_, s)

#define NE_INIT_S()\
	S(s), PI(&S->P, this)

//#define NE_INIT_SINGLETON(_ABBRV_)\
//	_ABBRV_ = S;

#define NE_CREATE_PRIVATE(_CLASS_, ...)\
    P(new _CLASS_##Private(this, __VA_ARGS__))

#define NE_PRIVATE(Class)\
    Class##Private

#define NE_DECL_PRIVATE_SINGLETON(Class) \
    Class##Private* P; \
    friend class Class##Private; \
    friend class Singleton<Class>; \
    Class(); \
    ~Class();

#define NE_CLASS_PRIVATE(Class) \
    class Class##Private; \
    class Class;

#define FRIEND_NE_CLASS_PRIVATE(Class) \
    friend class Class##Private; \
    friend class Class;


#define NE_BASE(_Class_, _IType_)\
    INTERFACE_##_Class_##_##_IType_(;)

#define NE_BASE_PURE(_Class_, _IType_)\
    INTERFACE_##_Class_##_##_IType_(=0;)

#define NE_DERIVED(_Class_, _IType_)\
    INTERFACE_##_Class_##_##_IType_(override;)

#define NE_FWD_DECL_1(_Type_, _C1_)\
    _Type_ _C1_;

#define NE_FWD_DECL_2(_Type_, _C1_, _C2_)\
    NE_FWD_DECL_1(_Type_, _C1_)\
    _Type_ _C2_;\

#define NE_FWD_DECL_3(_Type_, _C1_, _C2_, _C3_)\
    NE_FWD_DECL_2(_Type_, _C1_, _C2_)\
    _Type_ _C3_;

#define NE_FWD_DECL_4(_Type_, _C1_, _C2_, _C3_, _C4_)\
    NE_FWD_DECL_3(_Type_, _C1_, _C2_, _C3_)\
    _Type_ _C4_;

#define NE_FWD_DECL_5(_Type_, _C1_, _C2_, _C3_, _C4_, _C5_)\
    NE_FWD_DECL_4(_Type_, _C1_, _C2_, _C3_, _C4_)\
    _Type_ _C5_;

#define NE_FWD_DECL_6(_Type_, _C1_, _C2_, _C3_, _C4_, _C5_, _C6_)\
    NE_FWD_DECL_5(_Type_, _C1_, _C2_, _C3_, _C4_, _C5_)\
    _Type_ _C6_;

// FIXME: This macro somehow not working and should be fixed
#define NE_FWD_DECL(_Count_, ...) \
    NE_FWD_DECL_##_Count_(__VA_ARGS__)

#define NE_FRIEND(_Type_, _C1_)\
    friend _Type_ _C1_;

#define NE_FRIENDS_2(_Type_, _C1_, _C2_)\
    NE_FRIEND(_Type_, _C1_)\
    friend _Type_ _C2_;

#define NE_FRIENDS_3(_Type_, _C1_, _C2_, _C3_)\
    NE_FRIENDS_2(_Type_, _C1_, _C2_)\
    friend _Type_ _C3_;

#define NE_FRIENDS_4(_Type_, _C1_, _C2_, _C3_, _C4_)\
    NE_FRIENDS_3(_Type_, _C1_, _C2_, _C3_)\
    friend _Type_ _C4_;

//! Macro for raising compile-time assert
#define cassert(expn) typedef char __C_ASSERT__[(expn)?1:-1]

enum NEUIFlags
{
	NE_UI_UPDATE_STATE,
	NE_UNDO_REDO_STATE
};

//#############################################//
//########## Global enumerated types ##########//
//#############################################//

#include "Utility/Singleton.h"
class NECEXP_CORE NEConstants : public QObject, public Singleton<NEConstants>
{
	Q_OBJECT
	Q_ENUMS(NEReturnType NELogType NEObjectType NEIoType)
	Q_FLAGS(NEPropFlag)
public:
	enum NEInitMode
	{
		kInitOnCreate,
		kInitOnLoad
	};

	enum NEReturnType
	{
		kReturnFail = 0,	// This should be set to 0
		kReturnSuccess,
		kReturnNotChanged,
        kReturnPropNotInit,
		kReturnFileNotFound,
		kReturnCannotOpenFile,
		kReturnCannotSaveFile,
		kReturnBufferInUse,
		kReturnBufferUnavailable,
		kReturnAlreadyExists
	};

	enum NEPropFlag
	{
		kPropReadOnly = 0,
		kPropWritable = 1
	};

	enum NELogType
	{
		kLogInfo,
		kLogWarning,
		kLogError
	};

    enum NEObjectType
    {
        kObjectNone = 0,
        kObjectExperimental = 1,
        kObjectParticleFluidSolver = 1<<1,
        kObjectParticleRigid = 1<<2,
        kObjectParticleDeformable = 1<<3,
        kObjectKiller = 1<<4,
        kObjectGridFluidSolver = 1<<5,
        kObjectCollider = 1<<6,
        kObjectMeshing = 1<<7,
        kObjectMaterial = 1<<8,
        kObjectParticleSource = 1<<10,
        kObjectPhysical = 1<<11,
        kObjectRigidSolver = 1 << 12,
        kObjectCamera = 1<<13,
        kObjectLight = 1<<14,
        kObjectMeshPP = 1<<15,
        kObjectDataField = 1<<16,
        kObjectForceField = 1<<17,
        kObjectParticleEmitter = 1<<18,
        kObjectMeshVolumeOP = 1<<19,
        kObjectVolumeOP = 1<< 20,
        kObjectMeasurementField = 1<< 21,
        kObjectGroup = 1<<22,
        kObjectDataCollector = 1<<23,
        kObjectDualBoundary = 1<<24,
        kObjectOSD = 1<<25,
        kObjectClipPlane = 1<<26,
        kObjectGizmo = 1<<27,
        kObjectFluid = 1<<28,
        kObjectChemistry = 1<<29
    };

    enum NEIoType
    {
        kIONone = 0,
        kIOParticles = 1,
        kIOTriangleMesh = 1<<1,
        kIOForceField = 1<<2,
        kIOExtent = 1<<3,
        kIOVolume = 1<<4,
        kIOFlowRate = 1<<5,
        kIOFluidHeight = 1<<6,
        kIONumParticles = 1<<7,
        kIONumParticlesDrain = 1<<8,
        kIOHydrostaticPressure = 1<<9,
        kIOFluid = 1<<10,
        kIOGrains = 1<<11,
        kIOMixture = 1<<12,
        kIORigid = 1<<13,
        kIOMeasurement = 1<<14,
        kIOGizmo = 1<<15,
        kIOGeoConversion = 1<<16,
        kIOChemSpecies = 1<<17,
        kIOChemReaction = 1<<18,
        kIOScalar = 1<<19,
        kIOVector = 1<<20
    };

    enum NEMapping
    {
        kMapLinear,
        kMapTransfer
    };

    enum NECurveType {
        kLinear,
        kQuadratic,
        kCubic,
        kSinusoidal,
        kSinusoidalDecrease,
        kSinusoidalIncrease,
        kTangent,
        kStep
    };

    static const uint kParticleDeletionId = UINT_MAX;
    static const uint kUnregisteredCellId = UINT_MAX;

    Q_DECLARE_FLAGS(IoTypes, NEIoType)

};

Q_DECLARE_OPERATORS_FOR_FLAGS(NEConstants::IoTypes)

#define NE NEConstants


//#############################################//
//############# Utility functions #############//
//#############################################//

NEFEXP_CORE QString splitFromCapitals(const QString& s);

class NEPythonInput;
class PythonQtObjectPtr;
//! Pointer to the main python module
extern NEFEXP_CORE PythonQtObjectPtr& getPyMain();

//! Function for evaluating Python scripts
extern NEFEXP_CORE void EvalPy(const QString& script);
extern NEFEXP_CORE void EvalPyX(const QString& script);

//! For starting logging
extern NEFEXP_CORE QDebug& Log();

//! For ending logging
extern NEFEXP_CORE void EndLog(NE::NELogType type);

//! A utility function that removes all files/folders in the given directory
extern NEFEXP_CORE bool RemoveDir(const QString& dirName);

//! A utility function that renames the given dir by adding a trash prefix to its name
extern NEFEXP_CORE bool TrashDir(const QString &dirName, uint trashIndex);

//! A fast way to check whether the scene is initialized. This is used to avoid including the NESceneManager.h
extern NEFEXP_CORE bool SceneInitialized();

//! A fast way to check whether the scene properties are initialized, which is just before the calling the init functions of the nodes
extern NEFEXP_CORE bool PropertiesInitialized();

//! To check whether UI update is enabled
extern NEFEXP_CORE bool UpdateUI();

//! For enabling/disabling UI update
extern NEFEXP_CORE void SetUpdateUI(bool val);

//! To check whether undo/redo update is enabled
extern NEFEXP_CORE bool UndoRedoEnabled();

//! For enabling/disabling undo/redo
extern NEFEXP_CORE void EnableUndoRedo(bool val);

//! To check whether Node Selection Process
extern NEFEXP_CORE bool isDuringSelectionProcess();

//! For enabling/disabling Node Selection Processing
extern NEFEXP_CORE void SetSelectionProcess(bool val);

//! For pushing NE attributes into a global stack
extern NEFEXP_CORE void NEPushAttrib(NEUIFlags flag);

//! For popping NE attributes from a global stack
extern NEFEXP_CORE void NEPopAttrib(NEUIFlags flag);


//! Queries whether the program is initialized
extern NEFEXP_CORE bool NeutrinoInitialized();

extern NEFEXP_CORE void AddPyObject(const QString& name, QObject* object);
extern NEFEXP_CORE void RemovePyVariable(const QString& name);

extern NEFEXP_CORE void updateInternalConnectionsTable();


#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define CLAMP(a,b,c)		MAX(MIN(a,c), b)
#define SIGN(x) (x >= 0 ? 1.0f : -1.0f)

#define DEGREES_IN_1_RADIAN 57.295779513082320876798154814105
#define RADIANS_IN_1_DEGREE 0.01745329251994329576923690768489
#define RADIANS_IN_1_DEGREE_OVER_2 0.00872664625997164788461845384244

#define RADIAN_TO_DEGREE(a)	a*DEGREES_IN_1_RADIAN
#define DEGREE_TO_RADIAN(a) a*RADIANS_IN_1_DEGREE

struct Color32
{
    inline Color32(){}
    inline Color32(uchar r, uchar g, uchar b, uchar a)
    {
        red = r; green = g; blue = b; alpha = a;
    }
    uchar red;
    uchar green;
    uchar blue;
    uchar alpha;
    inline void Set(uchar r, uchar g, uchar b, uchar a)
    {
        red = r; green = g; blue = b; alpha = a;
    }

    // Some preset colors
    static Color32 kRed;
    static Color32 kGreen;
    static Color32 kBlue;
    static Color32 kMagenta;
    static Color32 kYellow;
    static Color32 kCyan;
    static Color32 kOrange;
    static Color32 kWhite;
    static Color32 kBlack;
};

NEFEXP_CORE QDataStream &operator<<(QDataStream &, const Color32 &);
NEFEXP_CORE QDataStream &operator>>(QDataStream &, Color32 &);


/*!
    For keeping cell id and pointer pairs
*/
struct IdPointerPair
{
    //! CellId of the item
    uint CellID;
    //! Pointer to the item
    void* pointer;
    //! Used for ordering items
    bool operator<(const IdPointerPair &k1) const { return (CellID < k1.CellID);}
};

/*!
    For storing metadata about any vector data. This allows iteration of vectors of sub-classes from parent classes.
    It can also be used for passing vector data among classes.
*/
class NEVectorMetaData
{
private:
	//! Private copy constructor and assignment operator to avoid object copying
	NEVectorMetaData&  operator = (const NEVectorMetaData&)
	{
		return *this;
	}
	NEVectorMetaData(const NEVectorMetaData&) { }

public:
	NEVectorMetaData() : vecData(0), objectCount(0), objectSize(0) {}
	//! For computing the offset between another type
	uint getOffsetTo(uint offsetOfOtherType) const
	{
		return (objectSize - offsetOfOtherType);
	}

#ifdef NE_BUILD_64  // For 64 bit
    //! For computing the offset to the given item
    quint64 getItemOffset(void* item) const
    {
        return (quint64)reinterpret_cast<uchar*>(item) - (quint64)reinterpret_cast<uchar*>(vecData);
    }
    //! Index of the given item in the vector
    quint64 dataIndex(void* item) const
    {
        return ( ((quint64)reinterpret_cast<uchar*>(item) - (quint64)reinterpret_cast<uchar*>(vecData)) / (quint64)objectSize);
    }

#else   // For 32 bit
    quint32 getItemOffset(void* item) const
    {
        return (quint32)reinterpret_cast<uchar*>(item) - (quint32)reinterpret_cast<uchar*>(vecData);
    }

	quint32 dataIndex(void* item) const
    {
        return ((quint32)reinterpret_cast<uchar*>(item) - (quint32)reinterpret_cast<uchar*>(vecData)) / objectSize;
    }

#endif
	//! For setting a vector to the metadata
	void setVecData(void* newItemAdress)
	{
		vecData = newItemAdress;
	}
	//! Returns whether the vector is relocated
	bool vecRelocated(void* oldAddress) const
	{
		return (vecData != oldAddress);
	}

    //! A pointer to the beginning of the vector data
    void* vecData;
    //! The total number of objects
    uint objectCount;
    //! The size of the actual object
    uint objectSize;
};

/*!
    This class combines metadata of a vector with a pointer to the actual vector.
*/
template <class T>
class NEVector
{
public:
	//! Constructs the vector with an invalid state
	NEVector() : m_pMetaData(0), m_pVector(0), m_bValid(false) {}
	//! Constructs the vector with the supplied metadata and vector pointers
	NEVector(NEVectorMetaData* metaData, std::vector<T>* vector) : m_pMetaData(metaData), m_pVector(vector) {}
	//! Marks the vector as valid if the pointers to the actual vector and the metadata is set, otherwise, sets it invalid
	void validate()
	{
		if(m_pVector == NULL || m_pMetaData == NULL) m_bValid = false;
		else m_bValid = true;
	}
	//! Returns the metadata
	NEVectorMetaData* getMetaData()
	{
		return m_pMetaData;
	}
	//! For supplying the metadata
	void setMetaData(NEVectorMetaData* metaData)
	{
		m_pMetaData = metaData;
		validate();
	}
	//! Returns a pointer to the actual vector
	std::vector<T>* getVector()
	{
		return m_pVector;
	}
	//! For setting the actual vector
	void setVector(std::vector<T>* vector)
	{
		m_pVector = vector;
		validate();
	}
	//! For reserving space in the actual vector
	void reserve(uint nItems)
	{
		// Check validity first
		validate();
		if(!m_bValid) return;
		// Reserve the storage for the required number of items
		m_pVector->reserve(nItems);
		// Syncronize the metadata with the new number of objects
		synchronizeMetaData(m_pMetaData->objectCount);
	}
	//! Resizes the vector to hold totally nItems
	void resize(uint nItems)
	{
		// Check validity first
		validate();
		if(!m_bValid) return;
		// Resize the actual vector
		m_pVector->resize(nItems);
		// Syncronize the metadata with the new size
		synchronizeMetaData(nItems);
		// Set the number of items inside the metadata
		m_pMetaData->objectCount = nItems;
	}
	//! Updates the vector metadata with the information about the actual vector
	void synchronizeMetaData(uint nSize)
	{
		if(nSize > 0)
		{
			T& firstElement = m_pVector->front();
			m_pMetaData->setVecData(&firstElement);
		}
		else
			m_pMetaData->setVecData(NULL);
	}

private:
	//! Pointer to the metadata
	NEVectorMetaData* m_pMetaData;
	//! Pointer to the actual vector
	std::vector<T>* m_pVector;
	//! State of the vector
	bool m_bValid;
};


#define declare_type(__objtype__, __target__, __data__) \
    __target__.objectSize = sizeof(__objtype__); \
    __target__.setVecData(reinterpret_cast<void*>(__data__)); \

#define declareAutoVector(__objtype__, __target__, __controller__, __vec__) \
    __target__.objectSize = sizeof(__objtype__); \
    __controller__.setMetaData(&__target__); \
    __controller__.setVector(&__vec__);

#define for_all(__objtype__, __objname__, __metadata__) \
    uint __offset__ = __metadata__.getOffsetTo(sizeof(__objtype__)); \
    __objtype__* __pIter__ = (__objtype__*)__metadata__.vecData; \
    uchar* __bIter__ = reinterpret_cast<uchar*>(__pIter__); \
    __objtype__* __objname__ = __pIter__; \
    for(int __i__=0; __i__< (int)__metadata__.objectCount; __i__++, \
    __bIter__ += sizeof(__objtype__) + __offset__, \
    __pIter__ = reinterpret_cast<__objtype__*>(__bIter__), \
    __objname__ = __pIter__)

#define for_some(__objtype__, __objname__, __metadata__, __start__, __end__) \
    uint __offset__ = __metadata__.getOffsetTo(sizeof(__objtype__)); \
    __objtype__* __pIter__ = (__objtype__*)__metadata__.vecData; \
    uchar* __bIter__ = reinterpret_cast<uchar*>(__pIter__) + (__start__ * (sizeof(__objtype__) + __offset__)); \
    __pIter__ = reinterpret_cast<__objtype__*>(__bIter__);\
    __objtype__* __objname__ = __pIter__; \
    for(int __i__=__start__; __i__<__end__; __i__++, \
    __bIter__ += sizeof(__objtype__) + __offset__, \
    __pIter__ = reinterpret_cast<__objtype__*>(__bIter__), \
    __objname__ = __pIter__)

/*#define def_loop_data(__objtype__, __objname__, __metadata__) \
        uint __offset__ = __metadata__.getOffsetTo(sizeof(__objtype__)); \
        __objtype__* __pIter__ = (__objtype__*)__metadata__.vecData; \
        uchar* __bIter__ = reinterpret_cast<uchar*>(__pIter__); \
        __objtype__* __objname__ = __pIter__;
*/

#ifdef WIN32

#define omp_for_all(__objtype__, __objname__, __metadata__, __omp_statement__) \
    uint __offset__ = __metadata__.getOffsetTo(sizeof(__objtype__)); \
    __offset__ += sizeof(__objtype__); \
    __objtype__* __pIter__ = (__objtype__*)__metadata__.vecData; \
    uchar* __bIter__ = reinterpret_cast<uchar*>(__pIter__); \
    __objtype__* __objname__ = __pIter__; \
    __pragma(omp for __omp_statement__ private(__bIter__, __pIter__, __objname__)) \
    for(int __i__=0; __i__< (int) __metadata__.objectCount; __i__++)

#define omp_parallel_for_all(__objtype__, __objname__, __metadata__, __omp_statement__) \
    uint __offset__ = __metadata__.getOffsetTo(sizeof(__objtype__)); \
    __offset__ += sizeof(__objtype__); \
    __objtype__* __pIter__ = (__objtype__*)__metadata__.vecData; \
    uchar* __bIter__ = reinterpret_cast<uchar*>(__pIter__); \
    __objtype__* __objname__ = __pIter__; \
    __pragma(omp parallel for __omp_statement__ private(__bIter__, __pIter__, __objname__)) \
    for(int __i__=0; __i__< (int) __metadata__.objectCount; __i__++)

#else

#define omp_for_all(__objtype__, __objname__, __metadata__, __omp_statement__) \
    uint __offset__ = __metadata__.getOffsetTo(sizeof(__objtype__)); \
    __offset__ += sizeof(__objtype__); \
    __objtype__* __pIter__ = (__objtype__*)__metadata__.vecData; \
    uchar* __bIter__ = reinterpret_cast<uchar*>(__pIter__); \
    __objtype__* __objname__ = __pIter__; \
    _Pragma(STRINGIFY(omp for __omp_statement__ private(__bIter__, __pIter__, __objname__))) \
    for(int __i__=0; __i__< (int) __metadata__.objectCount; __i__++)


#define omp_parallel_for_all(__objtype__, __objname__, __metadata__, __omp_statement__) \
    uint __offset__ = __metadata__.getOffsetTo(sizeof(__objtype__)); \
    __offset__ += sizeof(__objtype__); \
    __objtype__* __pIter__ = (__objtype__*)__metadata__.vecData; \
    uchar* __bIter__ = reinterpret_cast<uchar*>(__pIter__); \
    __objtype__* __objname__ = __pIter__; \
    _Pragma(STRINGIFY(omp parallel for __omp_statement__ private(__bIter__, __pIter__, __objname__))) \
    for(int __i__=0; __i__< (int) __metadata__.objectCount; __i__++)
#endif

#define omp_next_item(__objtype__, __objname__, __metadata__) \
    __bIter__ = ((uchar*)__metadata__.vecData) + (__i__ *__offset__); \
    __pIter__ = reinterpret_cast<__objtype__*>(__bIter__); \
    __objname__ = __pIter__;

#define item_at(__objtype__, __objname__, __metadata__, __position__) \
    uchar* __bIter__ = reinterpret_cast<uchar*>(__metadata__.vecData); \
    __bIter__ += (__position__ *__metadata__.objectSize); \
    __objname__ = reinterpret_cast<__objtype__*>(__bIter__);


#define call_if_init(__statement__) \
    if(SceneInitialized())			\
    __statement__;				\



//! Keeps IDs for user defined variants
struct NEVariantIds
{
    uint vec2fId;
    uint vec3iId;
    uint vec3fId;
    uint vec4fId;
    uint quatfId;
    uint eulerfId;
    uint matrix33dId;
    uint filePathId;
    uint colorRampId;
    uint propertyGroupStartId;
    uint propertyGroupEndId;
    uint subPropertyStartId;
    uint subPropertyEndId;
    uint propSettingsId;
    uint frameInfoId;
    uint neBoolId;
    uint neFloatTableId;
    //    uint imageSizeId;
};

NECEXP_CORE struct NEVariantIds& getNEVarIds();

class NEBool
{
public:

    NEBool()
    {
    }
    NEBool( bool val )
    {
        m_value = val;
    }

	operator QVariant() const;
	operator bool() const
	{
		return m_value;
	}

private:

	bool m_value;

};


Q_DECLARE_METATYPE(NEBool)

// These dummy classes are used for grouping properties in the property browser
class PropertyGroupStart {};
Q_DECLARE_METATYPE(PropertyGroupStart)
class PropertyGroupEnd {};
Q_DECLARE_METATYPE(PropertyGroupEnd)
class SubPropertyStart {};
Q_DECLARE_METATYPE(SubPropertyStart)
class SubPropertyEnd {};
Q_DECLARE_METATYPE(SubPropertyEnd)


typedef QSize NEImageSize;
typedef std::vector<int>::size_type int_size;
typedef std::vector<uint>::size_type uint_size;

// Needed for compiler-specific template instantiation
#ifdef _MSC_VER 
#define TEMPLATE_INSTANCE template
#else
#define TEMPLATE_INSTANCE template class
#endif

NEFEXP_CORE const QString& ColorRampFileFilter();
NEFEXP_CORE const QString& ReferenceFilter();
NEFEXP_CORE const QString& ImportFilter();
NEFEXP_CORE const QString& QFileFilter();
NEFEXP_CORE const QString& XYZFileFilter();

//extern QString g_referenceFilter;
//extern QString g_importFilter;
//extern QString g_qFileFilter;
//extern QString g_xyzFileFilter;
//extern QString g_colorRampFileFilter;

constexpr size_t maxNumOfFluids = 2;
constexpr size_t maxNumOfChemSpecies = 7;

// Physical constants
constexpr double planckConst = 6.62607015e-34;
constexpr double boltzmannConst = 1.380649e-23;
constexpr double avogadroConst = 6.02214076e23;
constexpr double idealGasConst = avogadroConst*boltzmannConst;
