#ifndef NELOGBROWSER_H
#define NELOGBROWSER_H

class NECEXP_NEUIUTILITIES NELogBrowser : public QObject
{
    Q_OBJECT
public:
    explicit NELogBrowser(QObject *parent = 0);
    ~NELogBrowser();

 public slots:
     void outputMessage( QtMsgType type, const QString &msg );

 signals:
     void sendMessage( QtMsgType type, const QString &msg );

 private:



};

#endif // NELOGBROWSER_H
