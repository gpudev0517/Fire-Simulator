#pragma once

namespace Ui {
class FDYNMessageWindow;
}

class FDYNMessageWindow : public QMainWindow
{
	Q_OBJECT

public:
    explicit FDYNMessageWindow(QWidget *parent = 0);
    ~FDYNMessageWindow();

	//! Returns a stream of type QDebug for type safe input
	QDebug& getNewDebugStream();
	//! Pushes the log message into the log window
	void pushLogInformation(NE::NELogType type);
	//! Saves the log message into a log file
	void saveToFile(QString path);
	//! Clears the contents of the log window
	void clear();

private:
    Ui::FDYNMessageWindow *ui;
	QDebug* m_Debug;
	QString m_LogBuffer;
};
