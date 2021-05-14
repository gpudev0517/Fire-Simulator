#pragma once

#include "Utility/Singleton.h"

class NEOsd3dMessage
{
public:
	NEOsd3dMessage(QString text, const vec3f& position, float size = 0.2, Color32 color = Color32(255,255,255,255))
		: m_Message(text), m_Position(position), m_Size(size), m_Color(color)
	{
	}
	NEOsd3dMessage() : m_Size(0.2f), m_Color(255,255,255,255) {}
	QString m_Message;
	vec3f m_Position;
	float m_Size;
	Color32 m_Color;
};

class NEOsd3d : public Singleton<NEOsd3d>
{

public:
	void add(QString id, NEOsd3dMessage& message);
    QMap<QString, NEOsd3dMessage>& messages();

private:
    QMap<QString, NEOsd3dMessage> m_messages;

	void clear();
};

#define NEOSD3D NEOsd3d::Instance()
