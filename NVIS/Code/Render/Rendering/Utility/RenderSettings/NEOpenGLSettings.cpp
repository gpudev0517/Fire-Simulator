#include "Rendering/Utility/RenderSettings/NEOpenGLSettings.h"
#include "Rendering/Utility/RenderSettings/NERenderSetting.h"
#include "Rendering/Manager/NERenderManager.h"

NEOpenGLSettings::NEOpenGLSettings()
{
	init();
}

NEOpenGLSettings::NEOpenGLSettings(NERenderSetting* common)
{
	m_pCommonSettings = common;
	setParent(common);
	setObjectName("PovraySettings");
	init();
}

NEOpenGLSettings::~NEOpenGLSettings()
{
}

/*!
	Initializes the PovRaySettings, should be called only on startup.
*/
void NEOpenGLSettings::init()
{
	m_PossibleFileTypes = 29;
	m_QualityMode = 9;
	m_Jitter = 0.0f;
	m_AntiAliasing = 0.3f;

	callDefaultPreset();

	initValidators();
}

void NEOpenGLSettings::initValidators()
{
	addValidator("General_Quality",0,20);
	addValidator("General_AA",0,1);
	addValidator("General_Max_Trace_Level",0,100);

	addValidator("PhotonMapping_PhotonCount",0,1000);
	addValidator("PhotonMapping_Autostop",0,1);
	
	addValidator("Radiosity_Pretrace_Start",0,1);
	addValidator("Radiosity_Pretrace_End",0,1);
	addValidator("Radiosity_Count",0,200);
	addValidator("Radiosity_Error_Bound",0,1);
	addValidator("Radiosity_Recursion_Limit",0,10);
}

/*! 
	Returns the render command
*/
QString NEOpenGLSettings::getRenderCommand(QString filePath)
{
	QString  command(m_RenderExePath);
	command += getRenderOptions(filePath);
	return command;
}

QString NEOpenGLSettings::getBatchRenderCommand(QString filePath)
{
	return getRenderCommand(filePath);
}

QString NEOpenGLSettings::outputFileName(const int frameNr)
{
	QString fileName;
	QString frameNumber;
	frameNumber.sprintf("%06d", frameNr);
    fileName = frameNumber + ".png";

	QString outputImageName = frameNumber;
	setOutputImageName(outputImageName);
	return fileName;
}

QString NEOpenGLSettings::includeFile(QString fileName)
{
	return "#include \""+ fileName +"\"\n";;
}

/*!
	For assembling the render command.
*/

QString NEOpenGLSettings::getRenderOptions(QString filePath)
{
	QString options(" ");

	options +=  tr(" /RENDER ") + filePath.replace(tr("/"),tr("\\"));
	//add suffix, will be done using render settings
	//see http://library.thinkquest.org/3285/language/cmdln.html
	//QString options = getRenderOptions();
	if(m_pCommonSettings)
	{			
		NEImageSize imgSize = m_pCommonSettings->Resolution();
		options += imageSizeCommand(&imgSize);
	}
	options += qualityCommand();
	options += aliasingCommand();
	options += jitterCommand();
    //options += threadCommand();
	
	options += " /EXIT ";

	return options;
}


QString NEOpenGLSettings::threadCommand()
{
#if 0
	if(NERENDERMAN.renderSequence() == false)
        return QString(" +WT%1").arg((double)NERENDERMAN.RenderSetting().Threads());
	else
		return QString(" +WT %numthreads%");
#endif
    return QString("");
}

QString NEOpenGLSettings::imageSizeCommand(const NEImageSize *size)
{	
	return QString(" +W%1 +H%2").arg(size->width()).arg(size->height());
}

QString NEOpenGLSettings::createStaticIncludes(QString path)
{	
	QString fileName = "staticHeader_povray.inc"; 
	QFile fout(path +fileName);
	if(!fout.open(QFile::WriteOnly | QFile::Truncate))
	{
		//Log() << "Cannot create camera file, for path::" << path <<fileName; EndLog(NE::kLogError);
        return QString("NE::kReturnFail");
	}
	QTextStream out(&fout);
	out.setCodec("UTF-8");

	out << "#include \"colors.inc\"\n";
	out << "#include \"shapes.inc\"\n";
	out << "#include \"shapes2.inc\"\n";
	out << "#include \"textures.inc\"\n\n";

	out << "global_settings\n";
	out << "{\n";
	out << "	max_trace_level "<< m_MaxTraceLevel<<"\n";
	out << "	assumed_gamma 1.0\n";
	if(photonMapping())
	{
		out << "	photons\n";
		out << "	{\n";
		out << "		count "<<photonCount()<<"\n";
		out << "		autostop "<<autostop()<<"\n";
		out << "	}\n";
	}
	if(radiosity())
	{
		out << "	radiosity\n";
		out << "	{\n";
		out << "		pretrace_start "<<pretraceStart()<<"\n";
		out << "		pretrace_end "<<pretraceEnd()<<"\n";
		out << "		count "<<count()<<"\n";
		out << "		error_bound "<<errorBound()<<"\n";
		out << "		recursion_limit "<<recursionLimit()<<"\n";
		out << "		always_sample on\n";
		out << "	}\n";
	}

	out << "}\n";
	
	if(radiosity())
	{
		out << "#default\n";
		out << "{\n";
		out << "		finish\n"; 
		out << "		{\n"; 
		out << "			ambient 0\n"; 
		out << "			diffuse 1\n"; 
		out << "			conserve_energy\n"; 
		out << "		}\n";
		out << "}\n";    
	}

	out << "background { Black }\n";
	
	return fileName;
}

void NEOpenGLSettings::callPreset()
{
	if(preset() == defaultPreset)
		callDefaultPreset();
	else if(preset() == radiosityPreset)
		callRadiosityPreset();
	else if(preset() == photonMapPreset)
		callPMPreset();
}

void NEOpenGLSettings::callDefaultPreset()
{
	setMaxTraceLevel(16);

	setPhotonMapping(true);
	setPhotonCount(10);
	setAutostop(0);

	setRadiosity(false);
	setPretraceStart(0.08f);
	setPretraceEnd(0.01f);
	setCount(320);
	setErrorBound(0.25f);
	setRecursionLimit(1);
}

void NEOpenGLSettings::callPMPreset()
{
	setPhotonMapping(true);
	setPhotonCount(100);
	setAutostop(0);

	setRadiosity(false);
}

void NEOpenGLSettings::callRadiosityPreset()
{
	setMaxTraceLevel(8);

	setPhotonMapping(false);

	setRadiosity(true);
	setPretraceStart(0.08f);
	setPretraceEnd(0.01f);
	setCount(320);
	setErrorBound(0.25f);
	setRecursionLimit(1);
}
