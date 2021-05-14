#include "Dialogs/ParticleSection/NEParticleSectionDialog.h"
#include "Dialogs/Section/NESectionWidget.h"
#include "Core/Spatial/NESpatial.h"

NEParticleSectionDialog::NEParticleSectionDialog(QWidget *parent, const QObject* object) : QDialog(parent)
{	
	m_pObject = object;
	setupUi(this);
	
	m_layout = new QHBoxLayout(this);
	section->setLayout(m_layout);

	/*bool test = */connect(this->addButton, SIGNAL(clicked()), this, SLOT(addSection()));
	connect(this->okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

NEParticleSectionDialog::~NEParticleSectionDialog()
{

}

/*!
	Adds a section to the dialog widget.
	id is the Section Type
	data is the content that should be stored
*/
void NEParticleSectionDialog::addSection(uint id, uint data)
{
	int sectionNr = (int)m_sections.size();
	NESectionWidget* section = new NESectionWidget(this,m_pObject,sectionNr);
	section->setSectionState(id, data);
	m_layout->addWidget(section);
	m_sections.push_back(section);
}

/*!
	Saves for all user selected sections the id and data in the provided map.
	Called by the instance that needs to cache/save data.
*/
void NEParticleSectionDialog::createSectionMap(QMap<uint,uint> &map)
{
	map.clear();

	std::vector<NESectionWidget*>::iterator i;

	NESectionWidget* section;

	for(i = m_sections.begin(); i != m_sections.end(); i++)
	{
		section = *i;
		if(section->sectionId())
			map.insert(section->sectionId(), section->sectionData());
	}
}

/*!
*/
void NEParticleSectionDialog::primitiveGroupIndexChanged(int index, int sectionNr)
{
    if((uint)index == NESpatial::kSectionAll())
	{
		addButton->setDisabled(true);
		for(int_size i=1; i<m_sections.size(); i++)
		{
			m_sections[i]->disableComboBox(true);
			m_sections[i]->setComboBoxIndex(0);
		}
	}
	else
	{
		addButton->setDisabled(false);

		for(int i=0; i< (int)m_sections.size(); i++)
		{
			if(i<sectionNr)
				m_sections[i]->disableComboBox(index != 0);
			else
				m_sections[i]->disableComboBox(false);
		}
	}

		
}


void NEParticleSectionDialog::okButtonClicked()
{
	accept();
}

bool NEParticleSectionDialog::groupSelected(int value, int sectionNr)
{
	for(int_size i=0; i<m_sections.size(); i++)
	{
		if((int)i==sectionNr)
			continue;
		int id = m_sections[i]->sectionId();

		if(id & value)
			return true;
	}
	return false;
}
