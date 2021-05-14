#pragma once

class NEPrefTreeWidgetItem;
struct NEDynaFactoryInfo;

class NEPluginsTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit NEPluginsTreeWidget(QWidget* parent = 0);
signals:
        void itemCheckStateChanged(NEPrefTreeWidgetItem*, bool);
};


class NEPrefTreeWidgetItem : public QTreeWidgetItem
{
public:
	NEPrefTreeWidgetItem(const QStringList& strings, int type = Type);


	void setData(int column, int role, const QVariant& value);

	NEDynaFactoryInfo* dynaFactory() const;
	void setDynaFactory(NEDynaFactoryInfo* val);


private:
	NEDynaFactoryInfo* m_pDFactoryInfo;
};


