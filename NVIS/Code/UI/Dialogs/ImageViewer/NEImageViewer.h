#pragma once

class QLabel;
class QPixmap;

class NEImageViewer : public QWidget
{
	Q_OBJECT

public:
	NEImageViewer(QWidget* parent=NULL);
	~NEImageViewer();

	void setFilePath(QString path) {m_FilePath = path;}
	void setFileFormat(QString format) {m_FileFormat = format;}
	void formats();
	
public slots:
	void showImage();
	void refreshImage();	
	void clear();
	void imfImage(QString path);
	void runImf(const QString& path);
	void stopImf();

signals:
	void imageFound();
protected:
	//!
	QPixmap* m_TmpImage;
	QLabel *m_ImageLabel;
	QImage *m_Image;

	QString m_FilePath;
	QString m_FileFormat;
	QFileSystemWatcher* m_FileWatcher;
	QProcess* m_ImfProcess;
};
