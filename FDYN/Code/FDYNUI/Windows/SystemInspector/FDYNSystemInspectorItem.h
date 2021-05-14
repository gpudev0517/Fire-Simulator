#pragma once

#include "Utilities/FDYNTreeItem.h"

class FDYNSystemInspectorItem : public FDYNTreeItem
{
    Q_OBJECT
public:
    FDYNSystemInspectorItem() : FDYNTreeItem(), m_pObject(0) {}
    ~FDYNSystemInspectorItem() {}

    QObject* Object() const { return m_pObject; }
    void setObject(QObject* val) { m_pObject = val; }

private:
    QObject* m_pObject;
};


