#pragma once

class NEDownloadManagerPrivate;

class NEDownloadManager : public QObject
{
    Q_OBJECT
    NE_DECL_PRIVATE(NEDownloadManager)

public:
    explicit NEDownloadManager(QObject* parent = 0);
    ~NEDownloadManager();
    void doDownload(const QUrl &url, const QString& folder, NEDownloadManager* nextDownloader = 0);

signals:
    void finished(const QString& fileName);
    void finished(const QString& fileName, NEDownloadManager* dlm);
};


