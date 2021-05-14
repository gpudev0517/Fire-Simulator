#pragma once


//! The NEMatrixStack4x4 class is created to secure matrix stack operations in future versions of OpenGL.
/*!
	It's designed to be used only by the NEGLManager class.
*/

class NECEXP_CORE NEMatrixStack4x4
{
	///friend class NEGLManager;
public:

	NEMatrixStack4x4(int depth = 64);

	void loadIdentity(void);
	void pushIdentity(void);
	void loadMatrix(const matrix44f& matrix);
	void multMatrix(const matrix44f& matrix);
	const matrix44f& top();

	void pushMatrix();
	void pushMatrix(const matrix44f& matrix);
	void popMatrix();

	int m_StackDepth;
	QStack<matrix44f> m_Stack;
};
