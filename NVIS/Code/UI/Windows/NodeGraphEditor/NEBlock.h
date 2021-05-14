/* Copyright (c) 2012, STANISLAW ADASZEWSKI
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STANISLAW ADASZEWSKI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef NEBLOCK_H
#define NEBLOCK_H

#include "NEPort.h"

class NEPort;
class NENode;

class NEBlock : public QGraphicsPathItem
{
public:
    enum { Type = QGraphicsItem::UserType + 103 };

    static const int ItemWidth = 200;
    static const int ItemHeight = 125;
    static const int BorderPenWidth = 1;
    static const int SelectedPenWidth = 3;
    static const int OuterBorderWidth = 10;
    static const int OuterBorderHeight = 10;
    static const int vertMargin1 = 25;
    static const int vertMargin2 = 5;

    NEBlock(NENode* node = 0, QGraphicsItem *parent = 0);
    QRectF boundingRect() const;

    void updatePortsofBlock();
    void updateSourceConnectionWithPorts();
    void updateConnectionWithPorts(QList<int>, QList<int>);

    NEPort* addPort(const QString &name, bool isOutput, uint IOIndex, int flags = NEPort::TypePort, int ptr = 0);
    void addInputPort(const QString &name, uint IOIndex = 0);
    void addOutputPort(const QString &name, uint IOIndex = 0);
    void addInputPorts(const QStringList &names, uint IOIndex = 0);
    void addOutputPorts(const QStringList &names, uint IOIndex = 0);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    NEBlock* clone();
    QVector<NEPort*> ports();

    void setPosition( QPointF r );
    bool advance();
    void setColor( QColor c )
    {
        m_Color = c;
    }

    QColor color()
    {
        return m_Color;
    }

	int type() const { return Type; }

    QString getBlockName();
    NENode* getNode(){return m_pNode;}

    bool connectPtoP(NEBlock* block, NEPort* port, uint sIndex, uint tIndex);
    bool disconnectPtoP(NEBlock* block, NEPort* port, uint sIndex, uint tIndex);

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);


private:
	int horzMargin;
	int vertMargin;
	int width;
	int height;
    int m_heightofName;
    QPointF m_Position;
    QColor m_Color;

    bool m_bDisplaySmall;

public:
    NENode* m_pNode;
    int getWidth(){return width;}
};

#endif // NEBLOCK_H
