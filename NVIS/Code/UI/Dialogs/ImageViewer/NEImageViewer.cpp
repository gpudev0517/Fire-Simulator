#include "Dialogs/ImageViewer/NEImageViewer.h"


NEImageViewer::NEImageViewer(QWidget* parent): QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	m_ImageLabel = new QLabel();
	layout->addWidget(m_ImageLabel);

	m_TmpImage = new QPixmap();
	m_Image = new QImage();
	m_FileWatcher = NULL;
	m_ImfProcess = NULL;
	formats();
}

NEImageViewer::~NEImageViewer()
{
	delete m_TmpImage;
	delete m_Image;

}


void NEImageViewer::showImage()
{
	show();
}

void NEImageViewer::refreshImage()
{	
	//if(m_Image)
		m_Image->load(m_FilePath,m_FileFormat.toUtf8());
	//else
		//m_Image = new QImage(m_FilePath,m_FileFormat.toUtf8());
	if(m_Image)
	{
		//create pixmap out of image
		QPixmap pixmap = QPixmap::fromImage(*m_Image);
		//ignore alpha channel by using a null bitmap
		pixmap.setMask(QBitmap());
		//finally, set the pixmap
		m_ImageLabel->setPixmap(pixmap);
	}
}

void NEImageViewer::imfImage(QString path)
{
	if(QFile(m_FilePath).exists())
		runImf(path);
	else
	{
		if(!m_FileWatcher)
			m_FileWatcher = new QFileSystemWatcher(this);
		m_FileWatcher->addPath(path);
		connect(m_FileWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(runImf(const QString&)));
	}
}

void NEImageViewer::runImf(const QString& path)
{
	if(m_ImfProcess == NULL)
		m_ImfProcess = new QProcess();
	m_ImfProcess->start("imf_disp \""+m_FilePath.replace(tr("/"),tr("\\"))+"\"");	
}

void NEImageViewer::stopImf()
{
	if(m_ImfProcess)
	{
		m_ImfProcess->kill();
		m_ImfProcess->deleteLater();
		m_ImfProcess = NULL;
	}
	if(m_FileWatcher)
	{
		delete m_FileWatcher;
		m_FileWatcher = NULL;
	}
}
void NEImageViewer::clear()
{
	//delete m_Image;
	m_ImageLabel->clear();		
}

void NEImageViewer::formats()
{
//	bool bTGA = false;
	//QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
	//foreach(QByteArray format, supportedFormats)
	//{
	//	QString name(format);
	//	if(name == "tga")		
	//		bTGA = true;
	//}

	//if(bTGA)
	//{
	//	Log()<< "TGA is supported"; EndLog(NE::kLogInfo);
	//}
	//else
	//{
	//	Log()<< "TGA is not supported"; EndLog(NE::kLogInfo);
	//}
}
