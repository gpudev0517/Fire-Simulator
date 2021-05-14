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

#include "NEBlock.h"
#include "NEPort.h"
#include "NENode.h"
#include "NEConnection.h"
#include "Managers/NEGUIManager.h"
#include "Base/NENodeFactory.h"
#include "NENodeGraphEditorScene.h"

enum NEFilterType
{
    BY_NODE_FILTER_ALL = -1,
    BY_NODE_CONNECTION_ALL = 0,
};

NEBlock::NEBlock(NENode* pNode, QGraphicsItem *parent) : QGraphicsPathItem(parent)
{
    m_pNode = pNode;
    QPainterPath p;
    p.addRoundedRect(-20, -15, 20, 15, 2, 2);
    setPath(p);
    setPen(QPen(Qt::darkGreen));
    setBrush(Qt::green);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    horzMargin = 30; // default margin 30
    vertMargin = 10; // default margin 20
    width = 2 * horzMargin;
    height = 2 * vertMargin;
    m_heightofName = height;

    m_bDisplaySmall = false;
}

void NEBlock::updatePortsofBlock()
{
    if(m_pNode->isShowFlag()){
        addPort(m_pNode->objectName(), 0, 0, NEPort::NamePort);
    //    addPort("", 0, NEPort::TypePort);

        uint outputCount = m_pNode->numOutputs();
        int enumOutputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
        QMetaEnum outputEnum = NE::Instance().metaObject()->enumerator(enumOutputIndex);
        for (int i = 0; i < outputEnum.keyCount(); ++i)
        {
            // add an entry if the node accepts the according input
            const char* name = outputEnum.key(i);
            uint ioType = (uint)outputEnum.keyToValue(name);
            for (uint j = 0; j < outputCount; j++)
            {
                uint outputs = m_pNode->outputs(j);
                if (outputs & ioType)
                    addOutputPort(name, j);
            }
        }

        uint inputCount = m_pNode->numInputs();
        int enumInputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
        QMetaEnum inputEnum = NE::Instance().metaObject()->enumerator(enumInputIndex);
        for(int i = 0; i < inputEnum.keyCount(); ++i)
        {
            // add an entry if the node accepts the according input
            const char* name = inputEnum.key(i);
            uint ioType = (uint)inputEnum.keyToValue(name);
            for (uint j = 0; j < inputCount; j++)
            {
                uint inputs = m_pNode->inputs(j);
                if (inputs & ioType)
                    addInputPort(name, j);
            }
        }
    }
}

void NEBlock::updateSourceConnectionWithPorts()
{
    if (m_pNode->sourceConnections().size())
    {	//Loop over all connections
        QMap<uint,NENode::NEConn>::const_iterator iter = m_pNode->sourceConnections().begin();
        for (;iter != m_pNode->sourceConnections().end(); ++iter)
        {
            int enumIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
            QMetaEnum ioEnum = NE::Instance().metaObject()->enumerator(enumIndex);
            QStringList names;
            names << iter->source->objectName();
            names << m_pNode->objectName();
            names << QString::number(iter->order);
            names << QString(ioEnum.valueToKey((int)iter->type));

            QList<QGraphicsItem*> items = scene()->items();

            NEConnection *conn = new NEConnection(0);
            conn->setOrder(iter->order);
            scene()->addItem(conn);
            foreach(QGraphicsItem *item, items)
            {
                if (item->type() != NEPort::Type)
                    continue;
                NEPort* itemPort = (NEPort*) item;

                QString sourceBlockName = iter->source->objectName();
                QString targetBlockName = m_pNode->objectName();
                QString typeName = QString(ioEnum.valueToKey((int)iter->type));
                bool    bIsOutputOfPort = itemPort->isOutput();
                QString portName = itemPort->portName();
                QString portBlockName = itemPort->block()->getBlockName();

                if (portName.compare(typeName) != 0)
                    continue;
                if (portBlockName.compare(sourceBlockName) == 0)
                {
                    if (bIsOutputOfPort && itemPort->getIOIndex() == iter->sIndex)
                    {
                        conn->setPort1(itemPort);
                        conn->setPos1(itemPort->scenePos());
                    }
                }
                else if (portBlockName.compare(targetBlockName) == 0)
                {
                    if (!bIsOutputOfPort && itemPort->getIOIndex() == iter->tIndex)
                    {
                        conn->setPort2(itemPort);
                        conn->setPos2(itemPort->scenePos());
                    }
                }
            }
            conn->updatePath();
        }
    }
}

void NEBlock::updateConnectionWithPorts(QList<int> portType, QList<int> nodeType)
{
    if (nodeType.count() == 0){             // select all in "By Node Type"
        return;
    }
    else{
        int i = nodeType.indexOf(m_pNode->factory().nodeType());             //   none select in in "By Node Type"
        if(i == -1 && nodeType.at(0) != BY_NODE_FILTER_ALL)
        {
            return;
        }
    }

    if (m_pNode->connections().size())
    {	//Loop over all connections
        QMap<uint,NENode::NEConn>::const_iterator iter = m_pNode->connections().begin();
        for (;iter != m_pNode->connections().end(); ++iter)
        {
            if(!iter->target->isShowFlag())
                continue;

            int enumIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
            QMetaEnum ioEnum = NE::Instance().metaObject()->enumerator(enumIndex);
            NEConnection *conn = new NEConnection(0);

            QStringList names;
            names << m_pNode->objectName();
            names << iter->target->objectName();
            bool    bOrderExist = false;
            QMap<uint,NENode::NEConn>::const_iterator targetConnectionsIter = iter->target->sourceConnections().begin();
            for( ;targetConnectionsIter != iter->target->sourceConnections().end(); ++targetConnectionsIter )
            {
                if( targetConnectionsIter->source == iter->source )
                {
                    names << QString::number(targetConnectionsIter->order);
                    conn->setOrder(targetConnectionsIter->order);
                    bOrderExist = true;
                }
            }
//            names << QString::number(iter->order);
            if (!bOrderExist)
            {
                names << QString::number(0); // iter->order);
                conn->setOrder(0); // iter->order);
            }
            names << QString(ioEnum.valueToKey((int)iter->type));

            QList<QGraphicsItem*> items = scene()->items();

            scene()->addItem(conn);
            foreach(QGraphicsItem *item, items)
            {
                if (item->type() != NEPort::Type)
                    continue;
                NEPort* itemPort = (NEPort*) item;

                QString sourceBlockName = m_pNode->objectName();
                QString targetBlockName = iter->target->objectName();
                QString typeName = QString(ioEnum.valueToKey((int)iter->type));
                bool    bIsOutputOfPort = itemPort->isOutput();
                QString portName = itemPort->portName();
                QString portBlockName = itemPort->block()->getBlockName();

                if (portName.compare(typeName) != 0)
                    continue;

                if(portType.count() == 0)
                    continue;

                int i = portType.indexOf(iter->type);
                if(i == -1 && portType.at(0) != BY_NODE_CONNECTION_ALL){
                    continue;
                }

                if (portBlockName.compare(sourceBlockName) == 0)
                {
                    if (bIsOutputOfPort && itemPort->getIOIndex() == iter->sIndex)
                    {
                        conn->setPort1(itemPort);
                        conn->setPos1(itemPort->scenePos());
                    }
                }
                else if (portBlockName.compare(targetBlockName) == 0)
                {
                    if (!bIsOutputOfPort && itemPort->getIOIndex() == iter->tIndex)
                    {
                        conn->setPort2(itemPort);
                        conn->setPos2(itemPort->scenePos());
                    }
                }
            }
            conn->updatePath();
        }
    }
}

QRectF NEBlock::boundingRect() const
{
    int penWidth;
    if (isSelected())
        penWidth = SelectedPenWidth;
    else
        penWidth = BorderPenWidth;

    return QRectF( -width / 2 - OuterBorderWidth, -height / 2 - OuterBorderHeight,
                   width + OuterBorderWidth, height + OuterBorderHeight);
}

NEPort* NEBlock::addPort(const QString &name, bool isOutput, uint IOIndex, int flags, int ptr)
{

    width = ItemWidth;
    height = ItemHeight + vertMargin1;

    NEPort *port = new NEPort(this);
    port->setNEBlock(this);
    port->setName(name);
	port->setIsOutput(isOutput);
	port->setPortFlags(flags);
    port->setPtr(ptr);
    port->setIOIndex(IOIndex);

	QFontMetrics fm(scene()->font());
	int w = fm.width(name);
    int h = fm.height();

    int nPortCount = 0;
    foreach(QGraphicsItem *port_, childItems())
    {
        if (port_->type() == NEPort::Type)
            nPortCount++;
    }
    int nTotalHeight = h * nPortCount + vertMargin2 * (MAX(0, nPortCount - 2)) + vertMargin * 2;
    if (nPortCount > 1)
        nTotalHeight += vertMargin1;

//    width   = MAX(width, w + 2 * horzMargin);
//    height  = MAX(height, nTotalHeight);
//    width   = MAX(width, ItemWidth);
//    height  = MAX(height, ItemHeight);
//    width   = MAX(width, height * 4 / 3);
//    height  = MAX(height, width * 3 / 4);

    int nTopSpace = (height - nTotalHeight)/2;

    QPainterPath p;
    p.addRoundedRect(-width/2, -height/2, width, height, height/20, height/20);
    setPath(p);

    int y = vertMargin + h/2 - height/2;
    int nPortIndex = 0;
    foreach(QGraphicsItem *port_, childItems())
    {
        if (port_->type() != NEPort::Type)
			continue;

        NEPort *port = (NEPort*) port_;
        if (nPortIndex == 0)
        {
            if (nPortCount == 1)
                y += nTopSpace;
            w = fm.width(port->portName());
            int nLeftSpace = (width - 2 * horzMargin - w )/2;
            nLeftSpace = 10;
            if (port->isOutput())
                port->setPos(width/2 + port->radius() - nLeftSpace, y);
            else
                port->setPos(-width/2 - port->radius() + nLeftSpace, y);
            y += (h + nTopSpace + vertMargin1);
            m_heightofName = height;
        } else {
            if (port->isOutput())
                port->setPos(width/2 + port->radius(), y);
            else
                port->setPos(-width/2 - port->radius(), y);
            if (nPortIndex == 1)
                m_heightofName = y - nTopSpace - h/2 - vertMargin1 + vertMargin + height/2;
            y += (h + vertMargin2);
        }
        nPortIndex++;
    }

	return port;
}

void NEBlock::addInputPort(const QString &name, uint IOIndex)
{
    addPort(name, false, IOIndex);
}

void NEBlock::addOutputPort(const QString &name, uint IOIndex)
{
    addPort(name, true, IOIndex);
}

void NEBlock::addInputPorts(const QStringList &names, uint IOIndex)
{
	foreach(QString n, names)
        addInputPort(n, IOIndex);
}

void NEBlock::addOutputPorts(const QStringList &names, uint IOIndex)
{
	foreach(QString n, names)
        addOutputPort(n, IOIndex);
}

#include <QStyleOptionGraphicsItem>

void NEBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_pNode->isShowFlag()){
        qreal radius = (qreal)height / (qreal)10;
        int YPos = -height/2;

        QPen selectedPen( QPen( QGuiApplication::palette().color( QPalette::Highlight ) ) );
        selectedPen.setWidth( 2 );
        if( isSelected() )
            painter->setPen( selectedPen );
        else
            painter->setPen( QGuiApplication::palette().color(QPalette::Window) );
        //painter->drawRect(-width/2, -height/2, width, height);
        if (m_heightofName == height)
            painter->drawRoundedRect(-width/2 - horzMargin, YPos + height / 4, width, height / 2, radius * 3, radius * 5);
        else
            painter->drawRoundedRect(-width/2, YPos, width, height, radius, radius);

        QBrush  titleBrush(QColor(0x40, 0x40, 0x40));
        QBrush  bodyBrush(QColor(0x60, 0x60, 0x60));
        if (m_heightofName == height)
        {
    //        painter->setBrush( Qt::darkGray );
            painter->setBrush(titleBrush);
            painter->drawRoundedRect(-width/2 + 1 - horzMargin, YPos + 1 + height / 4, width - 2, height / 2 - 2, radius * 3, radius * 5);

            if(m_pNode->factory().nodeInherits() == "Lights")
                painter->drawImage(-width/2, YPos + 1 + ( m_heightofName - 22 ) / 2, QImage(":/Neutrino/Resources/22/spotLight22.png") );
            else if(m_pNode->factory().objectName() == "Camera")
                painter->drawImage(-width/2, YPos + 1 + ( m_heightofName - 22 ) / 2, QImage(":/Neutrino/Resources/22/spotLight22.png") );

        } else {
    //        painter->setBrush( Qt::gray );
            painter->setBrush(bodyBrush);
            painter->drawRoundedRect(-width/2 + 1, YPos + 1, width - 2, height - 2, radius, radius);

            QPainterPath path;
            path.addRoundedRect(-width/2 + 1, YPos + 1, width - 2, m_heightofName + radius - 2, radius, radius);
    //        painter->fillPath(path, Qt::darkGray);
            painter->fillPath(path, titleBrush);
            QPainterPath path1;
            path1.addRect(-width/2 + 2, YPos + m_heightofName + 2, width - 4, radius);
    //        painter->fillPath(path1, Qt::gray);
            painter->fillPath(path1, bodyBrush);

            if(m_pNode->factory().nodeInherits() == "Rigids")
                painter->drawImage(width/2 - vertMargin - 22 , YPos + ( m_heightofName - 22 ) / 2, QImage(":/Neutrino/Resources/22/holowBox22.png") );  // 22 is width, height of image.
        }
    }
}

NEBlock* NEBlock::clone()
{
    NEBlock *b = new NEBlock(0);
    this->scene()->addItem(b);

	foreach(QGraphicsItem *port_, childItems())
	{
        if (port_->type() == NEPort::Type)
		{
            NEPort *port = (NEPort*) port_;
			b->addPort(port->portName(), port->isOutput(), port->portFlags(), port->ptr());
		}
	}

	return b;
}

QVector<NEPort*> NEBlock::ports()
{
    QVector<NEPort*> res;
	foreach(QGraphicsItem *port_, childItems())
	{
        if (port_->type() == NEPort::Type)
            res.append((NEPort*) port_);
	}
	return res;
}

QString NEBlock::getBlockName()
{
    return m_pNode->objectName();
}

bool NEBlock::connectPtoP(NEBlock *block, NEPort* port, uint sIndex, uint tIndex)
{
    int enumOutputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
    QMetaEnum outputEnum = NE::Instance().metaObject()->enumerator(enumOutputIndex);
    uint ioType = (uint)outputEnum.keyToValue(port->portName().toLatin1().constData());

    NENode* node = block->getNode();
    bool bResult;
    if(port->isOutput())
        bResult = m_pNode->connectTo(node,ioType, sIndex, tIndex);
    else
        bResult = node->connectTo(m_pNode, ioType, tIndex, sIndex);
//    if (bResult)
        NEGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector
    return bResult;
}

bool NEBlock::disconnectPtoP(NEBlock *block, NEPort *port, uint sIndex, uint tIndex)
{
    int enumOutputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
    QMetaEnum outputEnum = NE::Instance().metaObject()->enumerator(enumOutputIndex);
    uint ioType = (uint)outputEnum.keyToValue(port->portName().toLatin1().constData());

    NENode* node = block->getNode();
    bool bResult;
    if(!port->isOutput())
        bResult = node->disconnectFrom(m_pNode, ioType, tIndex, sIndex);
    else
        bResult = m_pNode->disconnectFrom(node,ioType, sIndex, tIndex);
//    if (bResult)
        NEGUI.updateNodeConnectionsTable(); //Update node connection table of connection inspector
    return bResult;
}

QVariant NEBlock::itemChange(GraphicsItemChange change, const QVariant &value)
{
    Q_UNUSED(change);

	return value;
}


void NEBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF newP = pos();
    if(((NENodeGraphEditorScene *)scene())->snapToGrid())
    {
        if( ((int)newP.x()) % 25 >= 13 )
            newP.setX( ((int)newP.x() - ((int)newP.x()) % 25) + 25);
        else
            newP.setX( ((int)newP.x() - ((int)newP.x()) % 25));

        if( ((int)newP.y()) % 25 >= 13 )
            newP.setY( ((int)newP.y() - ((int)newP.y()) % 25) + 25);
        else
            newP.setY( ((int)newP.y() - ((int)newP.y()) % 25));
        setPos( newP );
    }

    ((NENodeGraphEditorScene *)scene())->BackupAllNodePos();
    QGraphicsPathItem::mouseReleaseEvent(event);
}

void NEBlock::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

    m_bDisplaySmall = !m_bDisplaySmall;

    if(m_bDisplaySmall)
    {
        this->setScale(0.5);

        QVector<NEPort *> pts = ports();
        for(int i =0; i < pts.size(); i++)
        {
            if(pts[i]->portFlags() == NEPort::TypePort){
                //pts[i]->hide();
                pts[i]->setSmall(true);
                foreach(NEConnection *conn, pts[i]->connections())
                {
                    conn->updatePosFromPorts();
                    conn->updatePath();
                }
            }
        }

    }
    else
    {
        this->setScale(1.0);
        QVector<NEPort *> pts = ports();
        for(int i =0; i < pts.size(); i++)
        {
            if(pts[i]->portFlags() == NEPort::TypePort)
            {
                //pts[i]->show();
                pts[i]->setSmall(false);
                foreach(NEConnection *conn, pts[i]->connections())
                {
                    conn->updatePosFromPorts();
                    conn->updatePath();
                }
            }
        }

    }

    update();
    QGraphicsPathItem::mouseDoubleClickEvent(event);
}

void NEBlock::setPosition(QPointF r)
{
    m_Position = r;
}

bool NEBlock::advance()
{
    if( m_Position == pos() )
        return false;

    setPos( m_Position );
    return true;
}
