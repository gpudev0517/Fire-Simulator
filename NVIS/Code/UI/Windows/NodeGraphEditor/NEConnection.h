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

#ifndef NECONNECTION_H
#define NECONNECTION_H

class NEPort;

class NEConnection : public QGraphicsPathItem
{
public:
    enum { Type = QGraphicsItem::UserType + 102 };
    enum PathCurveType
    {
        Linear,
        Quadratic,
        Cubic
    };

    NEConnection(QGraphicsItem *parent = 0);
    ~NEConnection();
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    static PathCurveType getCurveType() { return m_PathCurveType; }
    static void setCurveType(PathCurveType type) { m_PathCurveType = type; }

	void setPos1(const QPointF &p);
	void setPos2(const QPointF &p);
    void setPort1(NEPort *p);
    void setPort2(NEPort *p);
	void updatePosFromPorts();
	void updatePath();
    NEPort* port1() const;
    NEPort* port2() const;
    int  getOrder() const { return m_order; }
    void  setOrder(int aOrder){ m_order = aOrder; updateOrderLabel(); }
    void updateOrderLabel();

	int type() const { return Type; }
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


private:
	QPointF pos1;
	QPointF pos2;
    NEPort *m_port1;
    NEPort *m_port2;
    int     m_order;
    QGraphicsTextItem *orderLabel;
    mutable QPainterPath m_Path;
    double arrowSize;
    static PathCurveType m_PathCurveType;

    void linearPath(const QPointF& start, const QPointF& end,  QPainterPath& path);
    void cubicPath(const QPointF& start, const QPointF& end,  QPainterPath& path);
    void quadraticPath(const QPointF& start, const QPointF& end,  QPainterPath& path);
};

#endif // NECONNECTION_H
