#pragma once

#define CAT(A,B) A##B
#define CAT2(A,B,C) A##B##C
#define STR(A) #A

#define NE_CLASS(_Name_)\
	Q_CLASSINFO("ClassName", _Name_)

#define NE_PROP_RW(_PType_, _PName_, _VisName_, _PHelp_, _PFlag_, _Prefix_)\
private:\
	Q_PROPERTY(_PType_  _PName_ READ _PName_ WRITE Set##_PName_ RESET Reset##_PName_ DESIGNABLE _PFlag_)\
	Q_CLASSINFO("name-" STR(_PName_), _VisName_)\
	Q_CLASSINFO("help-" #_PName_, _PHelp_)\
public:\
	const _PType_& _PName_() const;\
public slots:\
	_Prefix_ uint CAT(Set, _PName_)(const _PType_& val);\
	void CAT(Reset,_PName_)();\
Q_SIGNALS:\
	void _PName_##Changed(const _PType_&);\
private:

#define NE_PROP_RW_USER(_PType_, _PName_, _VisName_, _PHelp_, _PFlag_, _PUserFlag_, _Prefix_)\
private:\
        Q_PROPERTY(_PType_  _PName_ READ _PName_ WRITE Set##_PName_ RESET Reset##_PName_ DESIGNABLE _PFlag_ USER _PUserFlag_)\
        Q_CLASSINFO("name-" STR(_PName_), _VisName_)\
        Q_CLASSINFO("help-" #_PName_, _PHelp_)\
public:\
        const _PType_& _PName_() const;\
public slots:\
        _Prefix_ uint CAT(Set, _PName_)(const _PType_& val);\
        void CAT(Reset,_PName_)();\
Q_SIGNALS:\
        void _PName_##Changed(const _PType_&);\
private:

#define NE_PROP_R(_PType_, _PName_, _VisName_, _PHelp_, _PFlag_)\
private:\
	Q_PROPERTY(_PType_  _PName_ READ _PName_ DESIGNABLE _PFlag_)\
	Q_CLASSINFO("name-" #_PName_, _VisName_)\
	Q_CLASSINFO("help-" #_PName_, _PHelp_)\
public:\
	const _PType_& _PName_() const;\
Q_SIGNALS:\
    void _PName_##Changed(const _PType_&);\
private:

#define NE_PROP_2D_R(_PType_, _SType_, _PName_, _VisName_, _PHelp_, _PFlag_)\
private:\
    Q_PROPERTY(_PType_  _PName_ READ _PName_ DESIGNABLE _PFlag_)\
    Q_CLASSINFO("name-" #_PName_, _VisName_)\
    Q_CLASSINFO("help-" #_PName_, _PHelp_)\
public:\
    const _PType_& _PName_() const;\
Q_SIGNALS:\
    void _PName_##Changed(const _PType_&);\
private:

#define NE_PROP_2D_RW(_PType_, _SType_, _PName_, _VisName_, _PHelp_, _PFlag_, _Prefix_)\
private:\
    Q_PROPERTY(_PType_  _PName_ READ _PName_ WRITE CAT(Set,_PName_) RESET CAT(Reset,_PName_) DESIGNABLE _PFlag_)\
    Q_CLASSINFO("name-" STR(_PName_), _VisName_)\
    Q_CLASSINFO("help-" STR(_PName_), _PHelp_)\
    Q_PROPERTY(_SType_ CAT(_PName_,x) READ CAT(_PName_,x) WRITE CAT2(Set,_PName_,x) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,y) READ CAT(_PName_,y) WRITE CAT2(Set,_PName_,y) STORED false DESIGNABLE false)\
public:\
    const _PType_& _PName_() const;\
    _SType_ CAT(_PName_,x)() const;\
    _SType_ CAT(_PName_,y)() const;\
public slots:\
    _Prefix_ uint CAT(Set, _PName_)(const _PType_& val);\
    uint CAT2(Set,_PName_,x)(_SType_ x);\
    uint CAT2(Set,_PName_,y)(_SType_ y);\
    void CAT(Reset,_PName_)();\
Q_SIGNALS:\
    void _PName_##Changed(const _PType_&);\
    void _PName_##XChanged(const _SType_);\
    void _PName_##YChanged(const _SType_);\
private:

#define NE_PROP_3D_R(_PType_, _SType_, _PName_, _VisName_, _PHelp_, _PFlag_)\
private:\
	Q_PROPERTY(_PType_  _PName_ READ _PName_ DESIGNABLE _PFlag_)\
	Q_CLASSINFO("name-" #_PName_, _VisName_)\
	Q_CLASSINFO("help-" #_PName_, _PHelp_)\
public:\
	const _PType_& _PName_() const;\
Q_SIGNALS:\
    void _PName_##Changed(const _PType_&);\
private:

#define NE_PROP_3D_RW(_PType_, _SType_, _PName_, _VisName_, _PHelp_, _PFlag_, _Prefix_)\
private:\
	Q_PROPERTY(_PType_  _PName_ READ _PName_ WRITE CAT(Set,_PName_) RESET CAT(Reset,_PName_) DESIGNABLE _PFlag_)\
	Q_CLASSINFO("name-" STR(_PName_), _VisName_)\
	Q_CLASSINFO("help-" STR(_PName_), _PHelp_)\
	Q_PROPERTY(_SType_ CAT(_PName_,x) READ CAT(_PName_,x) WRITE CAT2(Set,_PName_,x) STORED false DESIGNABLE false)\
	Q_PROPERTY(_SType_ CAT(_PName_,y) READ CAT(_PName_,y) WRITE CAT2(Set,_PName_,y) STORED false DESIGNABLE false)\
	Q_PROPERTY(_SType_ CAT(_PName_,z) READ CAT(_PName_,z) WRITE CAT2(Set,_PName_,z) STORED false DESIGNABLE false)\
public:\
	const _PType_& _PName_() const;\
	_SType_ CAT(_PName_,x)() const;\
	_SType_ CAT(_PName_,y)() const;\
	_SType_ CAT(_PName_,z)() const;\
public slots:\
	_Prefix_ uint CAT(Set, _PName_)(const _PType_& val);\
	uint CAT2(Set,_PName_,x)(_SType_ x);\
	uint CAT2(Set,_PName_,y)(_SType_ y);\
	uint CAT2(Set,_PName_,z)(_SType_ z);\
	void CAT(Reset,_PName_)();\
Q_SIGNALS:\
	void _PName_##Changed(const _PType_&);\
	void _PName_##XChanged(const _SType_);\
	void _PName_##YChanged(const _SType_);\
	void _PName_##ZChanged(const _SType_);\
private:

#define NE_PROP_33D_RW(_PType_, _SType_, _PName_, _VisName_, _PHelp_, _PFlag_, _Prefix_)\
private:\
    Q_PROPERTY(_PType_  _PName_ READ _PName_ WRITE CAT(Set,_PName_) RESET CAT(Reset,_PName_) DESIGNABLE _PFlag_)\
    Q_CLASSINFO("name-" STR(_PName_), _VisName_)\
    Q_CLASSINFO("help-" STR(_PName_), _PHelp_)\
    Q_PROPERTY(_SType_ CAT(_PName_,m11) READ CAT(_PName_,m11) WRITE CAT2(Set,_PName_,m11) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m12) READ CAT(_PName_,m12) WRITE CAT2(Set,_PName_,m12) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m13) READ CAT(_PName_,m13) WRITE CAT2(Set,_PName_,m13) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m11) READ CAT(_PName_,m22) WRITE CAT2(Set,_PName_,m21) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m12) READ CAT(_PName_,m22) WRITE CAT2(Set,_PName_,m22) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m13) READ CAT(_PName_,m23) WRITE CAT2(Set,_PName_,m23) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m11) READ CAT(_PName_,m31) WRITE CAT2(Set,_PName_,m31) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m12) READ CAT(_PName_,m32) WRITE CAT2(Set,_PName_,m32) STORED false DESIGNABLE false)\
    Q_PROPERTY(_SType_ CAT(_PName_,m13) READ CAT(_PName_,m33) WRITE CAT2(Set,_PName_,m33) STORED false DESIGNABLE false)\
public:\
    const _PType_& _PName_() const;\
    _SType_ CAT(_PName_,m11)() const;\
    _SType_ CAT(_PName_,m12)() const;\
    _SType_ CAT(_PName_,m13)() const;\
    _SType_ CAT(_PName_,m21)() const;\
    _SType_ CAT(_PName_,m22)() const;\
    _SType_ CAT(_PName_,m23)() const;\
    _SType_ CAT(_PName_,m31)() const;\
    _SType_ CAT(_PName_,m32)() const;\
    _SType_ CAT(_PName_,m33)() const;\
public slots:\
    _Prefix_ uint CAT(Set, _PName_)(const _PType_& val);\
    uint CAT2(Set,_PName_,m11)(_SType_ m11);\
    uint CAT2(Set,_PName_,m12)(_SType_ m12);\
    uint CAT2(Set,_PName_,m13)(_SType_ m13);\
    uint CAT2(Set,_PName_,m21)(_SType_ m21);\
    uint CAT2(Set,_PName_,m22)(_SType_ m22);\
    uint CAT2(Set,_PName_,m23)(_SType_ m23);\
    uint CAT2(Set,_PName_,m31)(_SType_ m31);\
    uint CAT2(Set,_PName_,m32)(_SType_ m32);\
    uint CAT2(Set,_PName_,m33)(_SType_ m33);\
    void CAT(Reset,_PName_)();\
Q_SIGNALS:\
    void _PName_##Changed(const _PType_&);\
    void _PName_##m11Changed(const _SType_);\
    void _PName_##m12Changed(const _SType_);\
    void _PName_##m13Changed(const _SType_);\
    void _PName_##m21Changed(const _SType_);\
    void _PName_##m22Changed(const _SType_);\
    void _PName_##m23Changed(const _SType_);\
    void _PName_##m31Changed(const _SType_);\
    void _PName_##m32Changed(const _SType_);\
    void _PName_##m33Changed(const _SType_);\
private:


#define NE_PROP_33D_R(_PType_, _SType_, _PName_, _VisName_, _PHelp_, _PFlag_)\
private:\
    Q_PROPERTY(_PType_  _PName_ READ _PName_ DESIGNABLE _PFlag_)\
    Q_CLASSINFO("name-" STR(_PName_), _VisName_)\
    Q_CLASSINFO("help-" STR(_PName_), _PHelp_)\
public:\
    const _PType_& _PName_() const;\
Q_SIGNALS:\
    void _PName_##Changed(const _PType_&);\
private:


#define NE_SUB_PROP_START(_PName_, _PFlag_)\
	Q_PROPERTY(SubPropertyStart _PName_ READ subStart DESIGNABLE _PFlag_)

#define NE_SUB_PROP_END(_PName_)\
	Q_PROPERTY(SubPropertyEnd _PName_##_end READ subEnd)

#define NE_PROP_GROUP_START(_PName_, _VisName_, _PHelp_, _PFlag_)\
	Q_PROPERTY(PropertyGroupStart _PName_ READ groupStart DESIGNABLE _PFlag_)\
	Q_CLASSINFO("name-" STR(_PName_), _VisName_)\
	Q_CLASSINFO("help-" STR(_PName_), _PHelp_)

#define NE_PROP_GROUP_END(_PName_)\
	Q_PROPERTY(PropertyGroupEnd _PName_##_end READ groupEnd)

#define NE_BASE(_Class_, _IType_)\
	INTERFACE_##_Class_##_##_IType_(;)

#define NE_BASE_PURE(_Class_, _IType_)\
	INTERFACE_##_Class_##_##_IType_(=0;)

#define NE_DERIVED(_Class_, _IType_)\
	INTERFACE_##_Class_##_##_IType_(override;)
