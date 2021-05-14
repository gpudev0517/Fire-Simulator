#pragma once

#include "NEObject.h"
#include "Utility/NEOsdMsg.h"

class NEManager;
class NENode;
class IGLSurface;

class NECEXP_NERENDERNEO NEOsd : public NEObject
{
    Q_OBJECT

    Q_PROPERTY(bool logging READ logging WRITE setLogging)
    Q_PROPERTY(SubPropertyStart OsdSubProps READ subStart)
        Q_PROPERTY(bool loggingTop READ loggingTop WRITE setLoggingTop DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondTop READ logsPerSecondTop WRITE setLogsPerSecondTop DESIGNABLE logging)
        Q_PROPERTY(bool loggingTopRight READ loggingTopRight WRITE setLoggingTopRight DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondTopRight READ logsPerSecondTopRight WRITE setLogsPerSecondTopRight DESIGNABLE logging)
        Q_PROPERTY(bool loggingRight READ loggingRight WRITE setLoggingRight DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondRight READ logsPerSecondRight WRITE setLogsPerSecondRight DESIGNABLE logging)
        Q_PROPERTY(bool loggingBottomRight READ loggingBottomRight WRITE setLoggingBottomRight DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondBottomRight READ logsPerSecondBottomRight WRITE setLogsPerSecondBottomRight DESIGNABLE logging)
        Q_PROPERTY(bool loggingBottom READ loggingBottom WRITE setLoggingBottom DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondBottom READ logsPerSecondBottom WRITE setLogsPerSecondBottom DESIGNABLE logging)
        Q_PROPERTY(bool loggingBottomLeft READ loggingBottomLeft WRITE setLoggingBottomLeft DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondBottomLeft READ logsPerSecondBottomLeft WRITE setLogsPerSecondBottomLeft DESIGNABLE logging)
        Q_PROPERTY(bool loggingLeft READ loggingLeft WRITE setLoggingLeft DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondLeft READ logsPerSecondLeft WRITE setLogsPerSecondLeft DESIGNABLE logging)
        Q_PROPERTY(bool loggingTopLeft READ loggingTopLeft WRITE setLoggingTopLeft DESIGNABLE logging)
        Q_PROPERTY(int logsPerSecondTopLeft READ logsPerSecondTopLeft WRITE setLogsPerSecondTopLeft DESIGNABLE logging)
    Q_PROPERTY(SubPropertyEnd OsdSubProps_End READ subEnd)

public:

    NEOsd();

    static NEOsd& Instance();


public slots:

    void init();

	//!
	void add(uint rank, const NEOsdMsg& message, const NEObject* object);
	//!
	void add(uint rank, const NEOsdMsg& message);
	//!
	void remove(uint rank, NEObject::NEOsdMsgPos position);
    //!
    void remove(uint rank, const NEObject * object);
	//! Clear all buffers.
	void clearBuffers();
	//! Clears the content of the buffer at the given position.
	void clearBuffer(const NEObject::NEOsdMsgPos position);
	//! Moves the content of buffer at oldPos to the buffer at newPos.
	void moveBuffer(const NEObject::NEOsdMsgPos oldPos, const NEObject::NEOsdMsgPos newPos, const NEObject* object);

	//! Logs the osd messages to a file
	uint logToFile(QString scenePath, const double elapsedTime);
	//! Stops the logging to file
	//uint stopLogToFile();

	bool logging();
	void setLogging(bool flag);
	
	uint logsPerSecondTop() const;
	void setLogsPerSecondTop(uint val);

	uint logsPerSecondTopRight() const;
	void setLogsPerSecondTopRight(uint val);

	uint logsPerSecondRight() const;
	void setLogsPerSecondRight(uint val);

	uint logsPerSecondBottomRight() const;
	void setLogsPerSecondBottomRight(uint val);

	uint logsPerSecondBottom() const;
	void setLogsPerSecondBottom(uint val);

	uint logsPerSecondBottomLeft() const;
	void setLogsPerSecondBottomLeft(uint val);

	uint logsPerSecondLeft() const;
	void setLogsPerSecondLeft(uint val);

	uint logsPerSecondTopLeft() const;
	void setLogsPerSecondTopLeft(uint val);

	bool loggingTop() const;
	void setLoggingTop(bool val);

	bool loggingTopRight() const;
	void setLoggingTopRight(bool val);

	bool loggingRight() const;
	void setLoggingRight(bool val);

	bool loggingBottomRight() const;
	void setLoggingBottomRight(bool val);

	bool loggingBottom() const;
	void setLoggingBottom(bool val);

	bool loggingBottomLeft() const;
	void setLoggingBottomLeft(bool val);

	bool loggingLeft() const;
	void setLoggingLeft(bool val);

	bool loggingTopLeft() const;
	void setLoggingTopLeft(bool val);

	bool requiresLog(uint lps, const double elapsedTime);
	//check this method again
	bool requiresLog(uint lps);

	void managersCreated(QList<std::shared_ptr<NEManager> > managers );
	void managerCreated(NEManager *pManager);
	void objectCreated( NENode *object );

public:
	void clear();
	void setOffsety(const uint& value);
    void draw(IGLSurface *surface, int width, int height);
	void drawInQt(int width, int height, QPainter *painter);
    void drawRect(IGLSurface* surface, int x, int y, int w, int h);

private:


	uint writeFile(const QMap<uint, NEOsdMsg>& messages, QString fileName);
	uint writeMainFile(QString path);
	uint logHeadlines(const QMap<uint, NEOsdMsg>& messages, QTextStream& out);
	

	QMap<uint, NEOsdMsg>* getPositionMap(const NEObject::NEOsdMsgPos position);

	uint getOffsety() const;


	uint offsety = 12;
	bool m_Logging;

	uint m_LogsPerSecondTop;
	uint m_LogsPerSecondTopRight;
	uint m_LogsPerSecondRight;
	uint m_LogsPerSecondBottomRight;
	uint m_LogsPerSecondBottom;
	uint m_LogsPerSecondBottomLeft;
	uint m_LogsPerSecondLeft;
	uint m_LogsPerSecondTopLeft;
	//!
	uint m_LogsRecorded;
	uint m_LogsTopRecorded;
	uint m_LogsRightRecorded;

	bool m_LoggingTop;
	bool m_LoggingTopRight;
	bool m_LoggingRight;
	bool m_LoggingBottomRight;
	bool m_LoggingBottom;
	bool m_LoggingBottomLeft;
	bool m_LoggingLeft;
	bool m_LoggingTopLeft;

	QString m_LogFolderName;

	QMap<uint, NEOsdMsg> messagesTop;
	QMap<uint, NEOsdMsg> messagesTopRight;
	QMap<uint, NEOsdMsg> messagesRight;
	QMap<uint, NEOsdMsg> messagesBottomRight;
	QMap<uint, NEOsdMsg> messagesBottom;
	QMap<uint, NEOsdMsg> messagesBottomLeft;
	QMap<uint, NEOsdMsg> messagesLeft;
	QMap<uint, NEOsdMsg> messagesTopLeft;

	struct MessageAndPos
	{
		MessageAndPos() {}
		MessageAndPos(QString& m, int x, int y) : msg(m), posx(x), posy(y) {}
		QString msg;
		int posx;
		int posy;
	};

	QVector<MessageAndPos> m_MsgPos;


};

#define NEOSD NEOsd::Instance()
