#include "Dialogs/Help/NEHelpEngine.h"

/*!
*/
void NEHelpEngine::init(const QString path)
{
/*	m_pHelpEngine = new QHelpEngine(path);
	m_Valid = m_pHelpEngine->setupData();	*/	
}

bool NEHelpEngine::show()
{
     if (!m_pProcess)
         m_pProcess = new QProcess();

     if (m_pProcess->state() != QProcess::Running) {
         QString app = "assistant";/*QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
 #if !defined(Q_OS_MAC)
         app += QLatin1String("assistant");
 #else
         app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
 #endif*/

         QStringList args;
         args << QLatin1String("-collectionFile")
             << QLatin1String("../Doc/doxy/qthelp/neutrino.qhc")
             << QLatin1String("-enableRemoteControl");

         m_pProcess->start(app, args);

         if (!m_pProcess->waitForStarted()) {
             QMessageBox::critical(0, QObject::tr("Neutrino Assistant"),
                 QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
             return false;
         }
     }
     return true;
}
