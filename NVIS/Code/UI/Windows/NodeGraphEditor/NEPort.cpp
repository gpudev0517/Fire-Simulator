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

#include "NEPort.h"
#include "NEBlock.h"
#include "NEConnection.h"

NEPort::NEPort(QGraphicsItem *parent):
    QGraphicsPathItem(parent)
{
    label = new QGraphicsTextItem(this);
    label->setDefaultTextColor(Qt::white);
//    if (parent->type() == NEBlock::Type)
//    {
//        NEBlock* blockParent = (NEBlock*) parent;
//        QColor parentColor = blockParent->color();
//        if ((parentColor == Qt::yellow) ||
//            // (parentColor == Qt::darkYellow) ||
//            (parentColor == Qt::white))
//        {
//            label->setDefaultTextColor(Qt::black);
//        }
//    }

    radius_ = 8;
    margin = 10;
    m_block = NULL;

	setBrush(Qt::red);

    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

    m_portFlags = Unknown;
    setIOIndex(0);
}

NEPort::~NEPort()
{
    foreach(NEConnection *conn, m_connections)
    {
        if (conn->port1() == this)
            conn->setPort1(NULL);
        if (conn->port2() == this)
            conn->setPort2(NULL);
        if (conn->port1() == NULL && conn->port2() == NULL)
            delete conn;
    }
}

void NEPort::setNEBlock(NEBlock *b)
{
	m_block = b;
}

void NEPort::setName(const QString &n)
{
	name = n;
    QString dispname(name);
    QFontMetrics fm(scene()->font());
    QRect r = fm.boundingRect(dispname);
    if(m_block && r.width() > 0.5 * m_block->getWidth())
    {
        while(r.width() > 0.5 * m_block->getWidth())
        {
            dispname = dispname.left(dispname.length() - 3);
            r = fm.boundingRect(dispname);
        }
        label->setPlainText(dispname+"...");
    }
    else
        label->setPlainText(dispname);

    int enumIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
    QMetaEnum ioEnum = NE::Instance().metaObject()->enumerator(enumIndex);
    std::string charName = name.toStdString();
    NE::IoTypes m_type = (NE::IoTypes)ioEnum.keyToValue(charName.c_str());
    switch (m_type)
    {
    case NE::kIOParticles:
        setBrush(Qt::green);
        break;
    case NE::kIOTriangleMesh:
        setBrush(Qt::darkGreen);
        break;
    case NE::kIOForceField:
        setBrush(Qt::red);
        break;
    case NE::kIOExtent:
        setBrush(Qt::blue);
        break;
    case NE::kIOVolume:
        setBrush(Qt::magenta);
        break;
    case NE::kIOFlowRate:
        setBrush(Qt::cyan);
        break;
    case NE::kIOFluidHeight:
        setBrush(Qt::yellow);
        break;
    case NE::kIONumParticles:
        setBrush(Qt::darkMagenta);
        break;
    case NE::kIONumParticlesDrain:
        setBrush(Qt::darkYellow);
        break;
    case NE::kIOHydrostaticPressure:
        setBrush(Qt::darkCyan);
        break;
    case NE::kIOFluid:
        setBrush(Qt::lightGray);
        break;
    case NE::kIORigid:
        setBrush(Qt::darkRed);
        break;
    case NE::kIOMeasurement:
        setBrush(Qt::darkGray);
        break;
    case NE::kIOGizmo:
        setBrush(Qt::darkBlue);
        break;
    default:
        setBrush(Qt::gray);
        break;
    }
}

void NEPort::setIsOutput(bool o)
{
	isOutput_ = o;

	QFontMetrics fm(scene()->font());
	QRect r = fm.boundingRect(name);

	if (isOutput_)
		label->setPos(-radius_ - margin - label->boundingRect().width(), -label->boundingRect().height()/2);
	else
		label->setPos(radius_ + margin, -label->boundingRect().height()/2);
}

int NEPort::radius()
{
	return radius_;
}

bool NEPort::isOutput()
{
	return isOutput_;
}

QVector<NEConnection*>& NEPort::connections()
{
	return m_connections;
}

void NEPort::setPortFlags(int f)
{
	m_portFlags = f;

	if (m_portFlags & TypePort)
	{
		QFont font(scene()->font());
		font.setItalic(true);
		label->setFont(font);
        QPainterPath p;
        p.addEllipse(-radius_, -radius_, 2*radius_, 2*radius_);
        setPath(p);
    }
    else if (m_portFlags & NamePort)
	{
		QFont font(scene()->font());
		font.setBold(true);
		label->setFont(font);
		setPath(QPainterPath());
	}
}

NEBlock* NEPort::block() const
{
	return m_block;
}

quint64 NEPort::ptr()
{
	return m_ptr;
}

void NEPort::setPtr(quint64 p)
{
	m_ptr = p;
}

bool NEPort::isConnected(NEPort *other)
{
    foreach(NEConnection *conn, m_connections)
		if (conn->port1() == other || conn->port2() == other)
			return true;

	return false;
}

bool NEPort::isConnectable(NEPort *other)
{
    if(name == other->portName())
        return true;
    return false;
}

QVariant NEPort::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemScenePositionHasChanged)
	{
        foreach(NEConnection *conn, m_connections)
		{
			conn->updatePosFromPorts();
			conn->updatePath();
		}
	}
	return value;
}

void NEPort::setSmall(bool bSmall)
{
    if(bSmall)
    {
        label->hide();
    }
    else
    {
        label->show();
    }
}
