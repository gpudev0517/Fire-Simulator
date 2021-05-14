#pragma once

class NERenderSetting;

class DeleteThreadEvent : public QEvent
{
public:
	DeleteThreadEvent(QThread *thread) : QEvent(DeleteThreadEvent::type()), m_pThread(thread)
    {}

    virtual ~DeleteThreadEvent()
    {
		while (m_pThread->isRunning())
			m_pThread->wait();

		delete m_pThread;
	}

    static QEvent::Type type()
    {
        if (deleteThreadEventType == QEvent::None)
        {
            int generatedType = QEvent::registerEventType();
            deleteThreadEventType = static_cast<QEvent::Type>(generatedType);
        }
        return deleteThreadEventType;
    }

private:
    static QEvent::Type deleteThreadEventType;
	QThread* m_pThread;
};


class NEFrameExportThread : public QThread
{
	Q_OBJECT
public:
	NEFrameExportThread(QObject* parent = 0);
	~NEFrameExportThread();
	void setCommand(QString path){m_Command = path;}
	void setArguments(QString path){m_Arguments = path;}
	void run();

signals:
	void renderedSuccessfully();
	void finished(NEFrameExportThread*);
private slots:
	void emitFinished();
	void callCommand();
protected:
	QString m_Command;
	QString m_Arguments;
	//bool emitSignalWhenFinished;
};
