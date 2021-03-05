#pragma once

#include "Utilities/FDYNTreeItem.h"

class FDYNScenarioInspectorItem : public FDYNTreeItem
{
    Q_OBJECT
public:
    FDYNScenarioInspectorItem() : FDYNTreeItem(), m_pObject(0) {}
    ~FDYNScenarioInspectorItem() {}

    QObject* Object() const { return m_pObject; }
    void setObject(QObject* val) { m_pObject = val; }

private:
    QObject* m_pObject;
};
