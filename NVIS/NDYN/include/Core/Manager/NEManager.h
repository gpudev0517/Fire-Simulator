#pragma once

NE_FWD_DECL_4(class, NENode, NENodeFactory, NEObject, NE_PRIVATE(NEManager))

struct NEOsdMsg;

#include "NEObject.h"
/*! \brief NEManager class is the abstract base class that can create and delete objects in Neutrino
 *
 * The class allows creation and deletion of varions types of nodes in Neutrino. Different manager classes can override the functionality in this class
 * to create more sophisticated managers
*/


struct NEDynaFactoryInfo
{
	NENodeFactory* nf;
	QString path;
	QString filename;
	bool enabled;
};

class NECEXP_CORE NEManager : public NEObject
{
	Q_OBJECT

public:
	//! Manager is passed a pointer to a parent manager
	NEManager(QObject* parent);
	virtual ~NEManager();
	virtual uint init();
	//! Generic creation method for all simple managers
	virtual NENode* createObject(QString type, QString name, const NEObject* parent = 0);
	//! Deletes an object, removes all dependencies, and trashes the cache directory.
	virtual NENode* deleteObject(const QString& type , const QString& name, uint trashIndex);
	//! Deletes an object and removes all dependencies.
	virtual NENode* deleteObject(const QString& type , const QString& name);
	//! Renames an object
	void renameObject(const QString& oldName, const QString& newName);
	//! Returns a generic name for the next created object.
	QString nextObjectName(QString type, QString name);
	//! Resets the object number of all factories to the correct value -> should be called after loading a scene
	void resetObjectNumbers();
	//! Returns the map of created objects
	QMap<QString, NENode*>& objects() const;
	//! Returns the map of factories
	QMap<QString, NENodeFactory*>& factories() const;
	QMap<QString, NEDynaFactoryInfo>& dynaFactories();

	virtual void loadPlugins();
	virtual void loadStatics();

	void unregisterDynaNode(NEDynaFactoryInfo* nf);
	void registerDynaNode(NENodeFactory* factory, QString absPath, bool addToUI = true);
	void registerStaticNode(NENodeFactory* factory, bool addToUI);

public slots:
	void callPyScriptForObjectCreation(const QString& type);

signals:
	void addOSD(uint rank, const NEOsdMsg& msg);
	void addCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType);
	void removeCreateActionToGUI(const QString& type, const NEManager* manager, const uint nodeType);

protected:
	//! This is used for loading static libraries into Neutrino
	void addStaticToMaps(NENodeFactory* factory, bool addToUI = true);
	//! This is used for loading dynamic plugins into Neutrino
	void addDynamicsToMaps(QString pluginPath, bool addToUI = true);

private:
	NE_DECL_PRIVATE(NEManager)
};


