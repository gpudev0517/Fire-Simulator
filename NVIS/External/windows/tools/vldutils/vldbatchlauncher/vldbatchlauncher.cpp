// This is the vldqueue batch file launcher with
#include <QCoreApplication>
#include <QtCore>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.size() < 6)
    {
        cout<<"Error: Invalid number of arguments"<<endl<<endl;
        return -1;
    }
    // Convert slashes to backslashes
    for(int i =0; i < args.size(); i++)
    {
        QString& s = args[i];
        s.replace('/', '\\');
    }

    QString cmd = "..\\trunk\\neutrino\\External\\windows\\tools\\vldutils\\vldqueue.bat \"" + args.at(1) + "\" \"" + args.at(2) + "\" \"" + args.at(3) + "\" \"" + args.at(4) + "\" \"" + args.at(5) + "\"";

//    cout<<cmd.toStdString();
    system(cmd.toUtf8());

    return 0;
}
