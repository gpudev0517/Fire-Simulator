#include <QLayout>

#include "NEPropertyEditor.h"
#include "Windows/PropertyManager/NEObjectController.h"


NEPropertyEditor::NEPropertyEditor()
{
    m_pController = new NEObjectController(this);
}

NEPropertyEditor::~NEPropertyEditor()
{

}

void NEPropertyEditor::showNEObjectProperty(QObject* object)
{
    m_pController->setObject(object);
}

void NEPropertyEditor::init(QLayout* layout)
{
    layout->addWidget(m_pController);
}

void NEPropertyEditor::updateProperties()
{
    if(m_pController->object())
        m_pController->updateProperties();
}

void NEPropertyEditor::removeObject( QObject* object )
{
    if(m_pController->object())
        m_pController->removeObject(object);

}
