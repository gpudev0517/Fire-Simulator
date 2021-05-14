#pragma once

class NEObjectController;
class NEGUIManager;

class NEPropertyEditor : public QWidget
{
	Q_OBJECT
	friend class NEGUIManager;
public:
	NEPropertyEditor();
	~NEPropertyEditor();

	//! Initializes the property editor
	void init(QLayout* layout);

public slots:
	//! Show the properties for the given item
	void showNEObjectProperty(QObject* object);
	//! Updates the properties for the current object
	void updateProperties();
	//! Removes an object's property editor data
	void removeObject(QObject* object);

private:
	NEObjectController* m_pController;
};
