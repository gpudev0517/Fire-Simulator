
#ifndef FENGINEWORKER_H
#define FENGINEWORKER_H


#include <QRunnable>
#include <QThread>

class FEnginePrivate;


class FEngineWorker: public QThread
{
public:
    enum FWorkerType
    {
        CFAST = 0,
        FDS = 1,
        FRANX = 2
    };

    FEngineWorker(FEnginePrivate *_P,
                  QString jsonScn,
                  QString jsonSrc,
                  QString simInfoSrc,
                  QString scnName,
                  QString zoneName );

    void setWorkerType( FWorkerType ftype );
    FWorkerType workerType();

    ~FEngineWorker();

    void run();


signals:
    void finished();
    void stopSim();
    void endSim();
    void error(QString err);
    void processData( QJsonDocument jdoc );


public:
    QThread *m_workerThread;
    QString m_scnName;
    QString m_zoneName;
    QString m_jsonSrc;
    QString m_jsonScn;
    QString m_simInfo;

    FWorkerType m_WorkerType;
    int         m_WorkerResult;

    FEnginePrivate *P;
};

#endif // FENGINEWORKER_H
