#pragma once

#include "Utility/Singleton.h"


class NEHelpEngine : public Singleton<NEHelpEngine> 
{
	//Q_OBJECT

	public:

	void init(const QString path);	
	
	bool show();

	protected:

	bool m_Valid;

	QProcess* m_pProcess;

};

#define NEHELP NEHelpEngine::Instance()
