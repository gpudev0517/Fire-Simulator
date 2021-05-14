#pragma once

template <typename T>
class NEExternalProperties
{
public:

	NEExternalProperties()
	{
	}


	~NEExternalProperties()
	{
		delete m_pPropertyObject;
	}
	QObject* propertyObject()
	{
		return m_pPropertyObject;
	}
	T* P()
	{
		return static_cast<T*>(m_pPropertyObject);
	}

	void setPropertyObject(QObject* pPropertyObject)
	{
		m_pPropertyObject = pPropertyObject;
	}

private:
	QObject* m_pPropertyObject;
};


