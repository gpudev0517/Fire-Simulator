#pragma once

#include "Utilities/FDYNTreeItem.h"

class FDYNZoneInspectorItem : public FDYNTreeItem
{
    Q_OBJECT
public:
    FDYNZoneInspectorItem() : FDYNTreeItem(), m_pObject(0) {}
    ~FDYNZoneInspectorItem() {}

    QObject* Object() const { return m_pObject; }
    void setObject(QObject* val) { m_pObject = val; }

private:
    QObject* m_pObject;
};
