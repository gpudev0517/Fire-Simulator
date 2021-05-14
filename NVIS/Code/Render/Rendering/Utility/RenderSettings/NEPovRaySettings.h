#pragma once

#include "Rendering/Utility/RenderSettings/NESpecificRenderSetting.h"

class NERenderSetting;

class NEPovRaySettings : public NESpecificRenderSetting
{
	Q_OBJECT 
	
	Q_PROPERTY(Preset Preset_Preset READ preset WRITE setPreset)

	Q_PROPERTY(uint General_Quality READ quality WRITE setQuality)
	Q_PROPERTY(double General_AA READ aa WRITE setAA)
	Q_PROPERTY(double General_Jitter READ jitter WRITE setJitter)
	Q_PROPERTY(uint General_Max_Trace_Level READ maxTraceLevel WRITE setMaxTraceLevel)

	Q_PROPERTY(bool PhotonMapping_Photon_Mapping READ photonMapping WRITE setPhotonMapping)
	Q_PROPERTY(int PhotonMapping_PhotonCount READ photonCount WRITE setPhotonCount)
	Q_PROPERTY(double PhotonMapping_Autostop READ autostop WRITE setAutostop)

	Q_PROPERTY(bool Radiosity_Radiosity READ radiosity WRITE setRadiosity)
	Q_PROPERTY(double Radiosity_Pretrace_Start READ pretraceStart WRITE setPretraceStart)
	Q_PROPERTY(double Radiosity_Pretrace_End READ pretraceEnd WRITE setPretraceEnd)
	Q_PROPERTY(uint Radiosity_Count READ count WRITE setCount)
	Q_PROPERTY(double Radiosity_Error_Bound READ errorBound WRITE setErrorBound)
	Q_PROPERTY(uint Radiosity_Recursion_Limit READ recursionLimit WRITE setRecursionLimit)

	Q_ENUMS(Preset)

public:
	NEPovRaySettings();
	NEPovRaySettings(NERenderSetting* common);
	~NEPovRaySettings();

	void callPreset();
	void callDefaultPreset();
	void callRadiosityPreset();
	void callPMPreset();

	enum Preset
	{
		defaultPreset = 1,
		radiosityPreset = 2,
		photonMapPreset = 3
	};

	virtual void init();
	//! Returns the render command
	virtual QString getRenderCommand(QString filePath);
	virtual QString getBatchRenderCommand(QString filePath);

	virtual QString includeFile(QString fileName);
	virtual QString outputFileName(const int frameNr);
	virtual QString createStaticIncludes(QString path);

	//! Quality get/set methods
	uint quality() const {return m_QualityMode;}
	void setQuality(uint q) {m_QualityMode = q;}

	//! Antialiasing get/set methods
	float aa() const {return m_AntiAliasing;}
	void setAA(float q) {m_AntiAliasing = q;}

	//! Jitter get/set methods
	float jitter() const {return m_Jitter;}
	void setJitter(float q) {m_Jitter = q;}

	//! Globals get/set methods
	uint maxTraceLevel() const {return m_MaxTraceLevel;}
	void setMaxTraceLevel(uint q) {m_MaxTraceLevel = q;}

	//! Photon Mapping get/set methods
	bool photonMapping() const {return m_PhotonMapping;}
	void setPhotonMapping (bool enabled) {m_PhotonMapping = enabled;}

	int photonCount() const {return m_PhotonCount;}
	void setPhotonCount(int val) {m_PhotonCount = val;}

	double autostop() const {return m_Autostop;}
	void setAutostop(double val) {m_Autostop = val;}

	//! Radiosity get/set methods
	bool radiosity() const {return m_Radiosity;}
	void setRadiosity (bool enabled) {m_Radiosity = enabled;}

	double pretraceStart() const {return m_PretraceStart;}
	void setPretraceStart(double val) {m_PretraceStart = val;}

	double pretraceEnd() const {return m_PretraceEnd;}
	void setPretraceEnd(double val) {m_PretraceEnd = val;}

	uint count() const {return m_Count;}
	void setCount(uint q) {m_Count = q;}

	double errorBound() const {return m_ErrorBound;}
	void setErrorBound(double val) {m_ErrorBound = val;}

	uint recursionLimit() const {return m_RecursionLimit;}
	void setRecursionLimit(uint q) {m_RecursionLimit = q;}

	Preset preset() const { return m_Preset; }
	void setPreset(Preset val) { m_Preset = val; callPreset();}

protected:
	void initValidators();
	QString enabledCommand(bool enabled) const {if(enabled) return "on"; return "off";}
	QString imageSizeCommand(const NEImageSize *size);
	QString fileTypeCommand(const uint fileType);
	QString qualityCommand(){return QString(" +Q%1 ").arg(m_QualityMode);}
	QString aliasingCommand(){return QString(" +A%1 ").arg(m_AntiAliasing);}
	QString jitterCommand(){return QString(" +J%1 ").arg(m_Jitter);}
	QString threadCommand();
	
private:

	QString getRenderOptions(QString filePath);
	//!
	uint m_QualityMode;
	//!
	float m_AntiAliasing;
	//!
	float m_Jitter;

	//! Global settings related
	uint m_MaxTraceLevel;

	//! Photon Mapping Related
	bool m_PhotonMapping;
	double m_PhotonCount;
	double m_Autostop;

	//! Radiosity Related
	bool m_Radiosity;
	double m_PretraceStart;
	double m_PretraceEnd;
	uint m_Count;
	double m_ErrorBound;
	uint m_RecursionLimit;

	Preset m_Preset;
};
