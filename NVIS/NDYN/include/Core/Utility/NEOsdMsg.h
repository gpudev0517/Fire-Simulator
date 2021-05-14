#pragma once

#include "NEObject.h"

struct NEFEXP_CORE NEOsdMsg
{
    NEOsdMsg(){}
    NEOsdMsg(const QString& itemName);
    NEOsdMsg(const QString& itemName, const QString& itemText,
                    NEObject::NEOsdMsgPos msgpos = NEObject::TopRight, Color32 color = Color32(0,0,0,255));
	QString m_ItemName;
	QString m_ItemText;
	NEObject::NEOsdMsgPos pos;
	Color32 m_Color;

    //! Every object can have at maximum only this number of messages in the osd.
    //! Used to give unique ranks for all objects.
    static uint m_NrOfOsdMessagesPerObject;
    static uint& NrOfOsdMessagesPerObject();
    //! The current unique rank for a new object.
    //! Everytime a new object is created this value is increased by nextStartRank()
    static uint m_CurrentStartRank;
    static uint& CurrentStartRank();

    //! Everytime a new object is created this method returns the next available start rank,
    //! thereby it increases the value of the current start rank
    static uint nextStartRank();
};
