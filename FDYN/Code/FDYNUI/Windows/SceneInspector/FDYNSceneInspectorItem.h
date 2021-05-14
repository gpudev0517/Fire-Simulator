#pragma once

#include "Utilities/FDYNTreeItem.h"

class FDYNSceneInspectorItem : public FDYNTreeItem
{
    Q_OBJECT
public:
    FDYNSceneInspectorItem() : FDYNTreeItem(), m_pObject(0) {}
    ~FDYNSceneInspectorItem() {}

    QObject* Object() const { return m_pObject; }
    void setObject(QObject* val) { m_pObject = val; }

private:
    QObject* m_pObject;
};


