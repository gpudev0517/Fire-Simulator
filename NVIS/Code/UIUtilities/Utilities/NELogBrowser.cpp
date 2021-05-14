
#include "Base/NEBase.h"
#include "Utilities/NELogBrowser.h"

#include <QMetaType>

NELogBrowser::NELogBrowser( QObject *parent ): QObject( parent )
{
    qRegisterMetaType<QtMsgType>("QtMsgType");
}


NELogBrowser::~NELogBrowser()
{
}


void NELogBrowser::outputMessage( QtMsgType type, const QString &msg )
{
        switch (type) {
        case QtDebugMsg:
            Log() << "Neutrino - Info: " << msg; EndLog( NE::kLogInfo );
            break;
        case QtWarningMsg:
            Log() << "Neutrino - Warning: " << msg; EndLog( NE::kLogWarning );
            break;

        case QtCriticalMsg:
            Log() << "Neutrino - Critical: " << msg ; EndLog( NE::kLogError );
            break;

        case QtFatalMsg:
            Log() << "Neutrino - Fatal: " << msg ; EndLog( NE::kLogError );
            break;
        }

}


