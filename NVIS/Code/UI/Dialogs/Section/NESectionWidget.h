#pragma once

class QComboBox;
class QCheckBox;
class QVBoxLayout;
class QButtonGroup;

class NEParticleSectionDialog;

class NESectionWidget : public QWidget
{
	Q_OBJECT

public:
	NESectionWidget(QWidget* parent, const QObject* object, int sectionNr);
	~NESectionWidget();
	void setSectionState(uint sectionId, uint sectionData);
	uint sectionId();
	uint sectionData();

	int sectionNumber() const {return m_SectionNumber;}
	void setSectionNumber(int nr) {m_SectionNumber = nr;}

	void disableAll(bool disable);

public slots:
	void primitiveGroupIndexChanged(int index);
	void disableComboBox(bool disable);
	void setComboBoxIndex(int index);

protected:
	virtual void setComboBox(QVBoxLayout*, const QObject*);
	virtual void setCheckBoxGroup(QVBoxLayout*, const QObject*);
		
private:
	NEParticleSectionDialog* m_ParentDialog;

	QComboBox* m_sectionComboBox;
	//!If the section is stored externally this is checked.
	QCheckBox* m_ExternalCheckBox;
	//!Groups the check boxes in a generic way.
	QButtonGroup* m_CheckBoxes;
	//!Primitive data type enum index
	int m_PrimitveTypeIndexOfEnum;
	//!Primitive group enum index
	int m_PrimitiveGroupIndexOfEnum;
	//! Counter of the sections.
	int m_SectionNumber;
};
