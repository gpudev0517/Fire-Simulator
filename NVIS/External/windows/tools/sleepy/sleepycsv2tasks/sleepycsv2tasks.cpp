// Converts sleepy csv log to Qt Creator tasks file
// Optionally imports the tasks file into the running Qt Creator instance.

#include <QCoreApplication>
#include <QtCore>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    const char* usage = "Usage (omit square brackets): sleepycsv2tasks [INPUT] [OUTPUT] [optional:PATH_TO_QT_CREATOR]";

    if(args.size() < 3 || args.size() > 4)
    {
        cout<<"Converts a Sleepy comma separated log into Qt Creator tasks file."<<endl;
        cout<<"Optionally imports the tasks file into the running Qt Creator instance."<<endl<<endl;
        cout<<"Error: Invalid number of arguments"<<endl<<endl;
        cout<<usage<<endl;
        return -1;
    }
    QFile ifile(args.at(1));
    if(!ifile.open(QIODevice::ReadOnly))
    {
        cout<<"Cannot open input file: "<<(args.at(1).toStdString())<<endl;
        return -1;
    }
    QFile ofile(args.at(2));
    if(!ofile.open(QIODevice::WriteOnly))
    {
        cout<<"Cannot open output file: "<<(args.at(2).toStdString())<<endl;
        return -1;
    }
    QString prefix = args.at(3);
    prefix = prefix.toLower();

    QTextStream is(&ifile);
    QTextStream os(&ofile);

    QString funcName;
    QString exclusiveTime;
    QString inclusiveTime;
    QString exclusivePerc;
    QString inclusivePerc;
    QString moduleName;
    QString sourceFile;
    QString sourceLine;

    while(!is.atEnd())
    {
        QString l = is.readLine();
        QStringList sl0 = l.split("::");
        QString sl01;
        QStringList rest;
        // Function name syntax detected
        if(sl0.size() == 2)
        {
            // Take the right of it
            sl01 = sl0.at(1);
            // Get the index of the first comma
            int idx = sl01.indexOf(',');
            // Construct the function name
            funcName = sl0.at(0) + "::" + sl01.left(idx);
            // Split the rest of the right
            rest = sl01.mid(idx).split(',');
        }
        else
        {
            rest = l.split(',');
            funcName = rest.at(0);
        }
        exclusiveTime = rest.at(1);
        inclusiveTime = rest.at(2);
        exclusivePerc = rest.at(3);
        inclusivePerc = rest.at(4);
        moduleName = rest.at(5);
        sourceFile = rest.at(6);
        sourceLine = rest.at(7);

        if(sourceFile.isEmpty())
            sourceFile = "[unknown source file]";

        os<<sourceFile<<'\t'<<sourceLine<<'\t'<<"warn"<<'\t'<<moduleName.toUpper()<<": "<<funcName<<" spent: "<<exclusiveTime
         <<"s ("<<exclusivePerc<<"%), "<<"in total: "<<inclusiveTime<<"s ("<<inclusivePerc<<"%)\n";
    }

    // Open with the active creator instance
    if(args.size() == 4)
    {
        QString creatorPath = args.at(3);
        creatorPath.replace('/', '\\');
        QString cmd = creatorPath + " -client " + args.at(2);
//        cout<<cmd.toStdString()<<endl;

        system(cmd.toUtf8());
//        getchar();
    }
    return 0;

}
