#include "Rendering/NEOsd3d.h"

void NEOsd3d::add(QString id, NEOsd3dMessage& message)
{
    m_messages[id] = message;
}

QMap<QString, NEOsd3dMessage> &NEOsd3d::messages()
{
    return m_messages;
}

void NEOsd3d::clear()
{
    m_messages.clear();
}
