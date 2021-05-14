#pragma once

class NEPropertySettingsWrapper;

class NEPropertySettings
{
	friend class NEPropertySettingsWrapper;

public:	
	NEPropertySettings() : m_Keyed(1), m_Keyable(1)
	{

	}
    NEFEXP_CORE operator QVariant() const
	{
        return QVariant(getNEVarIds().propSettingsId, this);
	}

	bool keyable() const { return m_Keyable; }
	void setKeyable(bool val) { m_Keyable = val; }

	bool keyed() const { return m_Keyed; }
	void setKeyed(bool val) { m_Keyed = val; }

private:
	bool m_Keyed;
	bool m_Keyable;
};


Q_DECLARE_METATYPE(NEPropertySettings)

class NEPropertySettingsWrapper : public QObject
{
	Q_OBJECT

public slots:
	bool keyable(NEPropertySettings* o) 
	{
		return o->m_Keyable; 
	}
	bool keyed(NEPropertySettings* o) { return o->m_Keyed; }
	void setKeyable(NEPropertySettings* o, bool flag) 
	{
		o->m_Keyable = flag; 
	}
	void setKeyed(NEPropertySettings* o, bool flag) { o->m_Keyed = flag; }
};
