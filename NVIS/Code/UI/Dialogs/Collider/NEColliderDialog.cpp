#include "Dialogs/Collider/NEColliderDialog.h"
#include "Collider/NECollider.h"
#include "Manager/NESimulationManager.h"
#include "Manager/NECollisionManager.h"

/*!
*/
NEColliderDialog::NEColliderDialog(QWidget* parent) : NEAbstractDialog(parent,1)
{
    init();
}

NEColliderDialog::~NEColliderDialog()
{
}

/*!
*/
void NEColliderDialog::init()
{
    if( !NESIMMGR.collisionManager() || !NESIMMGR.collisionManager()->collider() )
        return;

    //init layout and variables
    m_SpecificGridLayout = new QGridLayout(this);
    m_MainContentLayout->addLayout(m_SpecificGridLayout,2,0,-1,-1);

    //add collider combo box
    addLabel("Collider", 0, 0, m_MainContentLayout);
    m_ColliderComboBox = addComboBox(NECOLMGR.factories().keys(), 0, 1,m_MainContentLayout);

    //set index to current collider
    int index = m_ColliderComboBox->findText(NECOLMGR.collider()->objectName());
    m_ColliderComboBox->setCurrentIndex(index);

    //create and initialize specific property widgets
    initSpecificWidgets();
    //make connections
    connect(m_ColliderComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(colliderChanged(int)));
}

/*!
*/
void NEColliderDialog::colliderChanged(int index)
{
//    NECOLMGR.createCollider(m_ColliderComboBox->itemText(index));
    NECOLMGR.createCollider();
    clearLayout(m_SpecificGridLayout);
    initSpecificWidgets();
}

/*!
*/
void NEColliderDialog::initSpecificWidgets()
{
    addPropertyWidgets(NECOLMGR.collider(),1, 0, m_SpecificGridLayout, 1);
}

void NEColliderDialog::accept()
{
    writeAllProperties(NECOLMGR.collider(), m_SpecificGridLayout, 1);
    NEAbstractDialog::accept();
}
