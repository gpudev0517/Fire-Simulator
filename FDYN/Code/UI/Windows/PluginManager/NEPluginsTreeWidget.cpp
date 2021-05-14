#include "NEPluginsTreeWidget.h"

NEPluginsTreeWidget::NEPluginsTreeWidget(QWidget* parent) : QTreeWidget(parent)
{

}

NEPrefTreeWidgetItem::NEPrefTreeWidgetItem(const QStringList& strings, int type) : QTreeWidgetItem(strings, type), m_pDFactoryInfo(0)
{
    setFlags(flags() | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Checked);
}

void NEPrefTreeWidgetItem::setData(int column, int role, const QVariant& value)
{
    const bool isCheckChange = column == 0
            && role == Qt::CheckStateRole
            && data(column, role).isValid() // Don't "change" during initialization
            && checkState(0) != value;
    QTreeWidgetItem::setData(column, role, value);
    if(isCheckChange)
    {
        NEPluginsTreeWidget* tree = static_cast<NEPluginsTreeWidget*>(treeWidget());
        emit tree->itemCheckStateChanged(this, checkState(0) == Qt::Checked);
    }

}
