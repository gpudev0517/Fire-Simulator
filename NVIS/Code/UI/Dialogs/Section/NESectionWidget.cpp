#include "Dialogs/Section/NESectionWidget.h"
#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h"
#include "Dialogs/ParticleSection/NEParticleSectionDialog.h"
/*!
	
*/
NESectionWidget::NESectionWidget(QWidget *parent, const QObject* object, int sectionNr) : QWidget(parent)
{
	m_ParentDialog = qobject_cast<NEParticleSectionDialog*> (parent);	
	NESpatial* spatial = qobject_cast<NESpatial*>(object);
	if(spatial)
	{
		m_PrimitveTypeIndexOfEnum = spatial->primitiveDataTypesIndexOfEnum();
		m_PrimitiveGroupIndexOfEnum = spatial->primitiveGroupIndexOfEnum();
	}
	else
		return;

	m_SectionNumber = sectionNr;
	m_ExternalCheckBox = NULL;
	QVBoxLayout* layout = new QVBoxLayout(this);
	//fill drop down list
	setComboBox(layout, object);
	//fill check box group
	setCheckBoxGroup(layout, object);			

	this->setLayout(layout);
}

NESectionWidget::~NESectionWidget()
{
}

void NESectionWidget::setSectionState(uint sectionId, uint sectionData)
{
	QListIterator<QAbstractButton*> it(m_CheckBoxes->buttons());

	while(it.hasNext())
	{
		QCheckBox* cb = (QCheckBox*)it.next();
		if(m_CheckBoxes->id((QAbstractButton*)cb) & ((int)sectionData))
		{
			cb->setChecked(true);
		}
	}

	int index;
    if(sectionId & NESpatial::kSectionExternal())
	{
		m_ExternalCheckBox->setChecked(true);
        index = m_sectionComboBox->findData(sectionId ^ NESpatial::kSectionExternal());
	}
	else
		index = m_sectionComboBox->findData(sectionId);

	m_sectionComboBox->setCurrentIndex(index);
	//primitiveGroupIndexChanged(index);
}

/*!
	Combo Box groups the primitives into meaningful sections.
*/
uint NESectionWidget::sectionId()
{
	int index = m_sectionComboBox->currentIndex();

	if(m_ExternalCheckBox->checkState())
        return m_sectionComboBox->itemData(index).toInt() | NESpatial::kSectionExternal();
	else
		return m_sectionComboBox->itemData(index).toInt();	
}

uint NESectionWidget::sectionData()
{
	uint data = 0;

	QListIterator<QAbstractButton*> it(m_CheckBoxes->buttons());

	while(it.hasNext())
	{
		QCheckBox* cb = (QCheckBox*)it.next();
		int id = m_CheckBoxes->id((QAbstractButton*)cb);
		if(cb->checkState())
			data |= (uint) id;
	}

	return data;
}

/*!
	Combo Box groups the primitives into meaningful sections.
*/
void NESectionWidget::setComboBox(QVBoxLayout* layout, const QObject* object)
{
	m_sectionComboBox = new QComboBox(this);	
	connect(m_sectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(primitiveGroupIndexChanged(int)));

	if(sectionNumber() > 0)
		m_sectionComboBox->addItem(tr("none (remove section)"), QVariant(0));
	else
        m_sectionComboBox->addItem(tr("All Particles"), QVariant(NESpatial::kSectionAll()));

	m_sectionComboBox->setCurrentIndex(-1);
	// get the QMetaEnum object which holds the name of the checkbox items
	QMetaEnum primitveGroupEnum = object->metaObject()->enumerator(m_PrimitiveGroupIndexOfEnum);
	
	// Loop over all enum keys to create checkboxes
	for(int i = 0; i < primitveGroupEnum.keyCount(); ++i)
	{		
		const char* name = primitveGroupEnum.key(i);
		QString text(name);
		text.remove("kSection");
		int value = primitveGroupEnum.keyToValue(name);
		if(!m_ParentDialog->groupSelected(value, m_SectionNumber))
		{
			//currently special cases like isolated particles are ignored
			//special cases have an odd value
			if(value%2 == 0)
				m_sectionComboBox->addItem(tr(text.toLatin1()), QVariant(value));
		}
	}
	
	layout->addWidget(m_sectionComboBox);	
}

/*!
*/
void NESectionWidget::setCheckBoxGroup(QVBoxLayout* layout, const QObject* object)
{
	m_CheckBoxes = new QButtonGroup(this);
	//allow multiple selections !!
	m_CheckBoxes->setExclusive(false);

	//HACK ? : add the external check box statically
	m_ExternalCheckBox = new QCheckBox(tr("Save externally"),this);
	layout->addWidget(m_ExternalCheckBox);
	// get the QMetaEnum object which holds the name of the checkbox items
	QMetaEnum checkBoxEnum = object->metaObject()->enumerator(m_PrimitveTypeIndexOfEnum);
	
	// Loop over all enum keys to create checkboxes
	for(int i = 0; i < checkBoxEnum.keyCount(); ++i)
	{
		const char* name = checkBoxEnum.key(i);
		QCheckBox* cb = new QCheckBox(tr(name), this);
		m_CheckBoxes->addButton(cb, checkBoxEnum.keyToValue(name));
		layout->addWidget(cb);
	}
}

void NESectionWidget::primitiveGroupIndexChanged(int index)
{
	uint value = m_sectionComboBox->itemData(index).toInt();

	if(m_ExternalCheckBox)
	{
        if(value == NESpatial::kSectionAll())
		{
			m_ExternalCheckBox->setChecked(false);
			m_ExternalCheckBox->setDisabled(true);
		}
		else
		{
			m_ExternalCheckBox->setDisabled(false);
		}
	}
	m_ParentDialog->primitiveGroupIndexChanged(value, sectionNumber());
}

void NESectionWidget::disableAll(bool disable)
{
	//if(disable)
	//	m_sectionComboBox->setCurrentIndex(0);
	//disable rest
	QListIterator<QAbstractButton*> it(m_CheckBoxes->buttons());
	while(it.hasNext())
	{
		QCheckBox* cb = (QCheckBox*)it.next();
		cb->setDisabled(disable);
		disableComboBox(disable);
	}
}

void NESectionWidget::disableComboBox(bool disable)
{
	if(m_sectionComboBox)
		m_sectionComboBox->setDisabled(disable);
}

void NESectionWidget::setComboBoxIndex(int index)
{
	if(m_sectionComboBox)
		m_sectionComboBox->setCurrentIndex(index);
}
