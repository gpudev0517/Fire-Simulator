# pragma once
#include "Dialogs/Abstract/NEAbstractDialog.h"


class NECollider;
/*!
*/
class NEColliderDialog : public NEAbstractDialog
{
	Q_OBJECT	

public:
	NEColliderDialog(QWidget* parent);
	~NEColliderDialog();
	
	//!
	void init();
	//!
	void initSpecificWidgets();

public slots:
	virtual void accept();

protected slots:
	void colliderChanged(int);

private:
	//! Drop down list of all implemented colliders
	QComboBox* m_ColliderComboBox;

	QGridLayout* m_SpecificGridLayout;
	//! Pointer to current collider
	NECollider* m_Collider;
};
