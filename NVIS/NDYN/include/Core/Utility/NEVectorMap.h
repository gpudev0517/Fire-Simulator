//for NEVectorMetaData
//#include "globals.h"
/*!
	This Map stores a std::vector of key value pairs. It can be used as an auxilary data structure for fast sorting of std::vector of structs.
*/
class NEKeyValuePair
{

public:
	NEKeyValuePair() {}
	NEKeyValuePair(int key, int value) : m_Key(key), m_Value(value) {}

	int key() const { return m_Key;}
	void setKey(int k) {m_Key = k;}

	int value() const { return m_Value;}
	void setValue(int v) {m_Value = v;}

	//operator required for sorting
	bool operator<(const NEKeyValuePair &i) const { return (m_Value < i.value());}

private:
	//! Key is e.g. the std::vector id
	int m_Key;
	//! Value is e.g a value that should be sorted for.
	int m_Value;
};

///*!
//	This Map stores a std::vector of key value pairs. It can be used as an auxilary data structure for fast sorting of std::vector of structs.
//*/
//
//class NEVectorMap
//{
//public:
//	NEVectorMap();
//	
//	NEVectorMap(const NEVectorMetaData* vectorData)
//	{
//
//	}
//private:
//	std::vector<NEKeyValuePair> m_VectorMap;
//
//};
