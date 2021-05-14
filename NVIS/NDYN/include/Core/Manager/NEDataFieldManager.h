#pragma once

#include "Manager/NEManager.h"

//! Data-Field Manager holds manages and updates all data fields.
/*!
	A more elaborate class description.
*/
class NEDataFieldManager : public NEManager
{
public:
	NEDataFieldManager(QObject* scene) : NEManager(scene) {}
	virtual ~NEDataFieldManager() {}

	//! Initializes the instance. Load all available plugins and static of simulation classes.
	uint init();
	//! Deletes all stored data
	uint cleanSavedData(QString dataPath);
	//! Updates the data field output
	uint update();
	//! Stores the result to file system
	uint saveFrameData(QString path, int frameNumber);
protected:
	QString saveDirectory(QString parent) const { return parent + "/Fields"; }

private:
	void loadStatics();
};
