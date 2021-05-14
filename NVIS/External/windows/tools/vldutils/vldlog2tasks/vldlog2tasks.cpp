// Converts a Visual Leak Detector log into qt creator tasks file.
// Optionally imports the tasks file into the running Qt Creator instance.

#include <QCoreApplication>
#include <QtCore>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    const char* usage = "Usage (omit square brackets): vldlog2tasks [INPUT] [OUTPUT] [TARGETDIR] [optional:PATH_TO_QT_CREATOR]";

    if(args.size() < 4 || args.size() > 5)
    {
        cout<<"Converts a Visual Leak Detector log into Qt Creator tasks file."<<endl;
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

    QString leakedBytes;
    QString sourceFile;
    QString sourceLine;
    QString funcName;

    while(!is.atEnd())
    {
        QString l = is.readLine();
        if(l.contains("---------- Block"))
        {
            QStringList sl = l.split(' ', QString::SkipEmptyParts);
            leakedBytes = sl.at(5);
        }
        else if(l.contains(prefix))
        {
            QString simp = l.simplified();
            QStringList sl = simp.split(' ', QString::SkipEmptyParts);
            sourceFile = sl.at(0);
            sourceLine = sl.at(1);
            sourceLine.remove('(');
            sourceLine.remove(')');
            sourceLine.remove(':');

            QString func = sl.at(2);
            QStringList fl = func.split('!', QString::SkipEmptyParts);
            funcName = fl.at(1);

            while(!is.atEnd())
            {
                QString dl = is.readLine();
                if(dl.contains("ntdll.dll!RtlUserThreadStart"))
                    break;
            }
            os<<sourceFile<<'\t'<<sourceLine<<'\t'<<"warn"<<'\t'<<funcName<<" leaked "<<leakedBytes<<" bytes\n";
        }
    }
    // Open with the active creator instance
    if(args.size() == 5)
    {
        QString creatorPath = args.at(4);
        creatorPath.replace('/', '\\');
        QString cmd = creatorPath + " -client " + args.at(2);
//        cout<<cmd.toStdString()<<endl;

        system(cmd.toUtf8());
//        getchar();
    }
    return 0;
}
