#pragma once

#include "Utilities/FDYNTreeItem.h"

class FDYNFRANXInspectorItem : public FDYNTreeItem
{
    Q_OBJECT
public:
    FDYNFRANXInspectorItem() : FDYNTreeItem(), m_pObject(0) {}
    ~FDYNFRANXInspectorItem() {}

    QObject* Object() const { return m_pObject; }
    void setObject(QObject* val) { m_pObject = val; }

private:
    QObject* m_pObject;
};
