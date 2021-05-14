#pragma once

#include "ui_NEParticleSectionDialog.h"

class NESectionWidget;
class QHBoxLayout;

/*!
	Dialog for determining what data is cached in which sections.
	This widget contains section widgets, see NESectionWidget.
*/
class NEParticleSectionDialog : public QDialog, public Ui::NEParticleSectionDialog
{
	Q_OBJECT

public:
	NEParticleSectionDialog(QWidget* parent, const QObject* object);
	~NEParticleSectionDialog();
	void createSectionMap(QMap<uint,uint>& map);
public slots:
	void addSection(uint sectionId = 0, uint sectionData = 0);
	//! When the group selection changes some options change, e.g. disabling of sections buttons. User notification.
	void primitiveGroupIndexChanged(int index, int sectionNr);
	//! When the user submits his changes, some checks are performed before accepting.
	void okButtonClicked();
	bool groupSelected(int value, int sectionNr);

private:
	std::vector<NESectionWidget*> m_sections;
	QHBoxLayout* m_layout;
	const QObject* m_pObject;
};
