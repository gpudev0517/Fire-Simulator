#pragma once

namespace Ui {
class NEMessageWindow;
}

class NEMessageWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit NEMessageWindow(QWidget *parent = 0);
	~NEMessageWindow();

	//! Returns a stream of type QDebug for type safe input
	QDebug& getNewDebugStream();
	//! Pushes the log message into the log window
	void pushLogInformation(NE::NELogType type);
	//! Saves the log message into a log file
	void saveToFile(QString path);
	//! Clears the contents of the log window
	void clear();

private:
	Ui::NEMessageWindow *ui;
	QDebug* m_Debug;
	QString m_LogBuffer;
};
