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

#include "NodesEditor.h"
#include "NEPort.h"
#include "NEConnection.h"
#include "NEBlock.h"
#include "Managers/NEGUIManager.h"

NodesEditor::NodesEditor(QObject *parent) :
    QObject(parent)
{
	conn = 0;
}

void NodesEditor::install(QGraphicsScene *s)
{
	s->installEventFilter(this);
	scene = s;
}

QGraphicsItem* NodesEditor::itemAt(const QPointF &pos)
{
	QList<QGraphicsItem*> items = scene->items(QRectF(pos - QPointF(1,1), QSize(3,3)));

	foreach(QGraphicsItem *item, items)
		if (item->type() > QGraphicsItem::UserType)
			return item;

	return 0;
}

bool NodesEditor::eventFilter(QObject *o, QEvent *e)
{
	QGraphicsSceneMouseEvent *me = (QGraphicsSceneMouseEvent*) e;

	switch ((int) e->type())
	{
        case QEvent::GraphicsSceneMousePress:
        {
            switch ((int) me->button())
            {
                case Qt::LeftButton:
                {
                    if(me->modifiers() == Qt::ControlModifier)
                    {
                        QGraphicsItem *item = itemAt(me->scenePos());
                        if (item && item->type() == NEPort::Type)
                        {
                            temp_port = (NEPort*) item;
                            conn_list = temp_port->connections();
                            for(int i = 0; i < conn_list.count(); i++)
                            {
                                NEConnection *con = conn_list.at(i);
                                if(con->port1() == temp_port){
                                    con->setPos1(me->scenePos());
                                }
                                else
                                    con->setPos2(me->scenePos());
                                con->updatePath();
                            }
                            return true;
                        }
                    }
                    else{
                        QGraphicsItem *item = itemAt(me->scenePos());
                        if (item && item->type() == NEPort::Type)
                        {
                            conn = new NEConnection(0);
                            scene->addItem(conn);
                            conn->setPort1((NEPort*) item);
                            conn->setPos1(item->scenePos());
                            conn->setPos2(me->scenePos());
                            conn->updatePath();

                            return true;
                        }
                    }
                    break;
                }
                case Qt::RightButton:
                {
                    QGraphicsItem *item = itemAt(me->scenePos());
                    if (item && (item->type() == NEConnection::Type)) //|| item->type() == NEBlock::Type))
                    {
                        if(item->type() == NEConnection::Type)
                        {
                            NEConnection* connItem = (NEConnection*)item;
                            NEPort *portFrom = connItem->port1();
                            NEPort *portTo = connItem->port2();
                            if (portFrom && portTo)
                            {
                                NEBlock*blockFrom = portFrom->block();
                                NEBlock*blockTo = portTo->block();
                                if (blockTo->disconnectPtoP(blockFrom, portFrom, portFrom->getIOIndex(), portTo->getIOIndex()))
                                {
                                    delete item;
                                    NEGUI.updateNodeGraphEditor();//Update NodeGraphEditor
                                    return true;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
        case QEvent::GraphicsSceneMouseMove:
        {
            if(conn_list.count() > 0)
            {
                for(int i = 0; i < conn_list.count(); i++)
                {
                    NEConnection *con = conn_list.at(i);
                    if(con->port1() == temp_port){
                        con->setPos1(me->scenePos());
                    }
                    else
                        con->setPos2(me->scenePos());
                    con->updatePath();
                }
                return true;
            }
            else if (conn)
            {
                conn->setPos2(me->scenePos());
                conn->updatePath();
                return true;
            }
            break;
        }
        case QEvent::GraphicsSceneMouseRelease:
        {
            if(conn_list.count() > 0 && me->button() == Qt::LeftButton)
            {
                QGraphicsItem *item = itemAt(me->scenePos());
                if (item && item->type() == NEPort::Type)
                {
                    NEPort *portTo, *portFrom;
                    for(int i = 0; i < conn_list.count(); i++)
                    {
                        NEConnection *con = conn_list.at(i);
                        if(con->port1() == temp_port){
                            portTo = (NEPort*) item;
                            portFrom = con->port2();
                        }
                        else{
                            portFrom = (NEPort*) item;
                            portTo = con->port1();
                        }
                        if (portFrom->block() != portTo->block() && portFrom->isOutput() != portTo->isOutput() &&
                            !portFrom->isConnected(portTo) && portFrom->isConnectable(portTo))
                        {
                            NEBlock *blockFrom = portFrom->block();
                            NEBlock *blockTo = portTo->block();
                            if (blockFrom->connectPtoP(blockTo, portFrom, portFrom->getIOIndex(), portTo->getIOIndex()))
                            {
                                NEBlock *dBlockFrom = con->port1()->block();
                                NEBlock *dBlockTo = con->port2()->block();
                                dBlockFrom->disconnectPtoP(dBlockTo, con->port2(), con->port1()->getIOIndex(), con->port2()->getIOIndex());
                            }
                        }
                        else if(portFrom->isConnected(portTo))
                        {
                            NEBlock *dBlockFrom = con->port1()->block();
                            NEBlock *dBlockTo = con->port2()->block();
                            dBlockFrom->disconnectPtoP(dBlockTo, con->port2(), con->port1()->getIOIndex(), con->port2()->getIOIndex());
                        }
                    }
                }
                NEGUI.updateNodeGraphEditor();
                conn_list.clear();
                return true;
            }
            if (conn && me->button() == Qt::LeftButton)
            {
                QGraphicsItem *item = itemAt(me->scenePos());
                if (item && item->type() == NEPort::Type)
                {
                    NEPort *portFrom = conn->port1();
                    NEPort *portTo = (NEPort*) item;

                    if (portFrom && portTo)
                    {
                        if (portFrom->block() != portTo->block() && portFrom->isOutput() != portTo->isOutput() &&
                            !portFrom->isConnected(portTo) && portFrom->isConnectable(portTo))
                        {
                            NEBlock*blockFrom = portFrom->block();
                            NEBlock*blockTo = portTo->block();
                            if (blockFrom->connectPtoP(blockTo, portFrom, portFrom->getIOIndex(), portTo->getIOIndex()))
                            {
                                conn->setPos2(portTo->scenePos());
                                conn->setPort2(portTo);
                                conn->updatePath();
                                conn = 0;
                                NEGUI.updateNodeGraphEditor();//Update NodeGraphEditor
                                return true;
                            }
                        }
                    }
                }

                delete conn;
                conn = 0;
                return true;
            }
            break;
        }
	}
	return QObject::eventFilter(o, e);
}
