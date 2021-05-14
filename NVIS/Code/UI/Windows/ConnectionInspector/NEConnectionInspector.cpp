#include "NEConnectionInspector.h"
#include "ui_NEConnectionInspector.h"
#include "Base/NESceneManager.h"
#include "Widgets/PythonInput/PythonHighlighter/PythonHighlighter.h"
#include "Utilities/NEGUIStyle.h"
#include "Widgets/Dock/NEDockWidget.h"
#include "NENode.h"
#include "Managers/NEGUIManager.h"
#include "Windows/ScriptEditor/NEConsoleWindow.h"
#include "Windows/NodeGraphEditor/NENodeGraphEditor.h"

/*!
  Used in the list of connectable items
*/
class NEConnectableItem : public QTreeWidgetItem
{
public:
    NEConnectableItem(const char* name) : QTreeWidgetItem(), m_Type(-100), m_nIndex(0)
	{
        setText(0, name);
        setText(1, "0");
	}

	~NEConnectableItem() {}

    int     type() const { return m_Type; }
    void    setType(int type) { m_Type = type; }
    uint    getIndex() const { return m_nIndex; }
    void    setIndex(uint nIndex) { m_nIndex = nIndex; setText(1, QString::number(m_nIndex)); }

private:
	NEConnectableItem(){}
    int     m_Type;
    uint    m_nIndex;
};


/*!
  Used in the list of connections
*/
class NEConnectedItem : public QTreeWidgetItem
{
public:
	~NEConnectedItem() {}
	NEConnectedItem(const QStringList & strings) : QTreeWidgetItem(strings), m_Conn(0), m_Source(0) {}

    NENode::NEConn* connection() { return (NENode::NEConn*)m_Conn; }
	void setConnection(NENode::NEConn* conn) { m_Conn = conn; }

    NENode::NEPropConn* propertyConnection() { return (NENode::NEPropConn*)m_Conn; }
	void setPropertyConnection(NENode::NEPropConn* conn) { m_Conn = conn; }

    NENode::NESrcPropConn* sourcePropertyConnection() { return (NENode::NESrcPropConn*)m_Conn; }
    void setSourcePropertyConnection(NENode::NESrcPropConn* conn) { m_Conn = conn; }

	NENode* source() { return m_Source; }
	void setSource(NENode* source) { m_Source = source; }

private:
	NEConnectedItem() {}
	void* m_Conn;
	NENode* m_Source;
};


/*!
  Used in the list of connectable items
*/
class NECallbackableItem : public QTreeWidgetItem
{
public:
    NECallbackableItem(QString name) : QTreeWidgetItem(0), m_Source(0)
	{
		setText(0, name);
	}
//	NECallbackableItem(const QStringList & strings) : QTreeWidgetItem(0), m_Source(0)
//    {
//        this->set

//    }

	~NECallbackableItem() {}

	NENode* source() { return m_Source; }
	void setSource(NENode* source) { m_Source = source; }

private:
	NECallbackableItem(){}
	NENode* m_Source;
};



NEConnectionInspector::NEConnectionInspector(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::NEConnectionInspector)
{
	ui->setupUi(this);

	// Initialize member variables

	m_pIOTargetNode = 0;
	m_pIOSourceNode = 0;
	m_TargetIOsShown = true;

	m_pPropTargetNode = 0;
	m_pPropSourceNode = 0;
	m_TargetPropertiesShown = true;

	m_pCallbackNode = 0;

	m_pInternalConnectionsNode = 0;

    m_TargetSort = false;

	// Create action group for activating subpages
	QActionGroup* ag = new QActionGroup(this);
	ag->addAction(ui->actionInternal_connections);
	ag->addAction(ui->actionNode_connections);
	ag->addAction(ui->actionProperty_callbacks);
	ag->addAction(ui->actionProperty_connections);

	connect(ui->actionInternal_connections, &QAction::triggered, this, &NEConnectionInspector::showInternalConnectionsPage);
	connect(ui->actionNode_connections, &QAction::triggered, this, &NEConnectionInspector::showNodeConnectionsPage);
	connect(ui->actionProperty_connections, &QAction::triggered, this, &NEConnectionInspector::showPropertyConnectionsPage);
	connect(ui->actionProperty_callbacks, &QAction::triggered, this, &NEConnectionInspector::showPropertyCallbacksPage);

	ui->actionNode_connections->setChecked(true);

	// For the IO connection dialog
	// Connects the button signals to the corresponding slots
	connect(ui->loadSourceNodeConnsButton, SIGNAL(clicked(bool)), this, SLOT(loadSourceOutputTypes()));
	connect(ui->loadTargetNodeConnsButton, SIGNAL(clicked(bool)), this, SLOT(reloadTargetInputTypes()));
	connect(ui->connectNodesButton, &QPushButton::clicked, this, &NEConnectionInspector::connectNodes);
	connect(ui->swapNodesButton, &QPushButton::clicked, this, &NEConnectionInspector::swapIONodes);
	connect(ui->removeNodeConnectionButton, &QPushButton::clicked, this, &NEConnectionInspector::removeNodeConnection);
	connect(ui->sourceNodeOutputsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::listSourceNodeConnections);
	connect(ui->targetNodeInputsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::listTargetNodeConnections);
	// When an item is selected from the output table, only connectable items are listed selectable in the input table
	connect(ui->sourceNodeOutputsTable, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(loadTargetInputTypesForAvailableTarget()));
	connect(ui->allNodeConnectionsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::nodeConnectionClicked);

    ui->allNodeConnectionsTable->setSortingEnabled( true );

    //ui->allNodeConnectionsTable->setSelectionMode( QAbstractItemView::MultiSelection );

    QHeaderView* treeHeader = ui->allNodeConnectionsTable->header();

    if( treeHeader )
    {
        treeHeader->setSortIndicatorShown( true );
        connect( treeHeader, SIGNAL(sectionClicked( int ) ), this, SLOT( customSortByColumn( int ) ) );
        customSortByColumn( treeHeader->sortIndicatorSection() );
    }

    //ui->allNodeConnectionsTable->header()->moveSection( 0, 1 );
	// For the property connection dialog
	// Connects the button signals to the corresponding slots
	connect(ui->loadSourcePropertiesButton, SIGNAL(clicked(bool)), this, SLOT(loadSourceNodeProperties()));
	connect(ui->loadTargetPropertiesButton, SIGNAL(clicked(bool)), this, SLOT(reloadTargetProperties()));
	connect(ui->connectPropertiesButton, &QPushButton::clicked, this, &NEConnectionInspector::connectProperties);
	connect(ui->swapPropertiesButton, &QPushButton::clicked, this, &NEConnectionInspector::swapPropertyNodes);
	connect(ui->removePropertyConnectionButton, &QPushButton::clicked, this, &NEConnectionInspector::removePropertyConnection);
	connect(ui->sourcePropsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::listSourcePropertyConnections);
	connect(ui->targetPropsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::listTargetPropertyConnections);
	// When an item is selected from the output table, only connectable items are listed selectable in the input table
	connect(ui->sourcePropsTable, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(loadTargetNodePropertiesForAvailableTarget()));
	connect(ui->connectedPropsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::showPropertyConnectionExpression);
	// Connect the button to save expression
	connect(ui->savePropertyExpressionButton, &QPushButton::clicked, this, &NEConnectionInspector::savePropertyConnectionExpression);
	// Connect the propery expression editor textChanged signal to propertyExpressionEditContentsChanged to enable save button when text is changed
	connect(ui->propertyExpressionEdit, &QPlainTextEdit::textChanged, this, &NEConnectionInspector::propertyExpressionEditContentsChanged);
	connect(ui->allPropertyConnectionsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::showPropertyConnectionExpression);

	// For the property callbacks dialog
	connect(ui->loadNodePropertiesButton, &QPushButton::clicked, this, &NEConnectionInspector::loadNodeProperties);
	connect(ui->loadNodePropertiesButton, &QPushButton::clicked, this, &NEConnectionInspector::listPropertiesWithCallback);
	connect(ui->saveCallbackExpressionButton, &QPushButton::clicked, this, &NEConnectionInspector::savePropertyCallback);
	connect(ui->removeCallbackExpressionButton, &QPushButton::clicked, this, &NEConnectionInspector::removePropertyCallback);
	connect(ui->nodePropsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::showCallbackExpression);
	connect(ui->activeNodeCallbacksTree, &QTreeWidget::itemClicked, this, &NEConnectionInspector::showCallbackExpression);
	connect(ui->callbackExpressionEdit, &QPlainTextEdit::textChanged, this, &NEConnectionInspector::propertyCallbackEditContentsChanged);
	connect(ui->allCallbackExpressionsTable, &QTreeWidget::itemClicked, this, &NEConnectionInspector::showCallbackExpression);

	//For the internal connections dialog
	connect(ui->loadNodeButton, &QPushButton::clicked, this, &NEConnectionInspector::loadInternalConnectionsOfANode);
}

NEConnectionInspector::~NEConnectionInspector()
{
	delete ui;
}

void NEConnectionInspector::init()
{
	// Initialize the expression editor
	ui->propertyExpressionEdit->init(NEGUI.consoleWindow()->outputWindow());
	ui->propertyExpressionEdit->setFont(*NEGUISTYLE.MonoNormalFont());
	m_pPythonHighlighter = new PythonHighlighter(ui->propertyExpressionEdit->document());

	ui->callbackExpressionEdit->init(NEGUI.consoleWindow()->outputWindow());
	ui->callbackExpressionEdit->setFont(*NEGUISTYLE.MonoNormalFont());
	m_pPythonHighlighter2 = new PythonHighlighter(ui->callbackExpressionEdit->document());
}

void NEConnectionInspector::connectNodes()
{
	// check whether both nodes have been selected
	if(m_pIOTargetNode && m_pIOSourceNode)
	{
		// checks whether both input and output types have been selected
		if(ui->targetNodeInputsTable->currentItem() && ui->sourceNodeOutputsTable->currentItem())
		{
            NEConnectableItem* outputItem = (NEConnectableItem*)ui->targetNodeInputsTable->currentItem();
            NEConnectableItem* inputItem = (NEConnectableItem*)ui->sourceNodeOutputsTable->currentItem();

			// check whether input and output type are the same
            if(inputItem->type() == outputItem->type())
			{
				// performs the connection command in the python shell
                uint type = outputItem->type();
                uint sIndex = inputItem->getIndex();
                uint tIndex = outputItem->getIndex();
                int order = ui->orderSpinBox1->value();
                QString cmd = m_pIOSourceNode->objectName().append(tr(".connectTo(")).append(m_pIOTargetNode->objectName()).append(tr(",%1,%2,%3,%4)").arg(type).arg(sIndex).arg(tIndex).arg(order));
//				EvalPy(cmd);
                getPyMain().evalScript(cmd);
			}
			// updates the connection table
			m_TargetIOsShown ? listTargetNodeConnections(ui->targetNodeInputsTable->currentItem()) : listSourceNodeConnections(ui->sourceNodeOutputsTable->currentItem());
			// Update the connection tables
			updateAllNodeConnectionsTable();
		}
	}
    NEGUI.updateNodeGraphEditor();//Update NodeGraphEditor
}

void NEConnectionInspector::removeNodeConnection()
{
	// get the selected connection
    NEConnectedItem* item;
    if(!ui->connectedNodesTable->selectedItems().isEmpty())
	{
        item = static_cast<NEConnectedItem*>(ui->connectedNodesTable->currentItem());

		if(!m_TargetIOsShown)
		{
			NENode::NEConn* conn = item->connection();
			//call the outputs(source) disconnect from method
            QString cmd = conn->target->objectName().append(tr(".disconnectFrom(")).append(item->source()->objectName()).append(tr(",%1,%2,%3)").arg(conn->type).arg(conn->sIndex).arg(conn->tIndex));
			// m_outputNode->disconnectFrom(conn->target, conn->type);
//			EvalPy(cmd);
            getPyMain().evalScript(cmd);
			listSourceNodeConnections(ui->sourceNodeOutputsTable->currentItem());
		}
		else
		{
			NENode::NEConn* conn = item->connection();
			//call the outputs(source) disconnect from method
            QString cmd = m_pIOTargetNode->objectName().append(tr(".disconnectFrom(")).append(conn->source->objectName()).append(tr(",%1,%2,%3)").arg(conn->type).arg(conn->sIndex).arg(conn->tIndex));
			//conn->target->disconnectFrom(m_inputNode, conn->type);
            getPyMain().evalScript(cmd);
			listTargetNodeConnections(ui->targetNodeInputsTable->currentItem());
		}
	}

    else if(!ui->allNodeConnectionsTable->selectedItems().isEmpty())
	{
        item = static_cast<NEConnectedItem*>(ui->allNodeConnectionsTable->currentItem());

        //QList< QTreeWidgetItem *>& items = ui->allNodeConnectionsTable->selectedItems();

        //for( int i = 0; i < items.size(); ++i )
        {
            //item = static_cast< NEConnectedItem *> ( items.value( i ) );
            NENode::NEConn* conn = item->connection();
            //call the outputs(source) disconnect from method
            QString cmd = conn->target->objectName().append(tr(".disconnectFrom(")).append(conn->source->objectName()).append(tr(",%1,%2,%3)").arg(conn->type).arg(conn->sIndex).arg(conn->tIndex));
            getPyMain().evalScript(cmd);
            if(!m_TargetIOsShown)
            {
                // m_outputNode->disconnectFrom(conn->target, conn->type);
                listSourceNodeConnections(ui->sourceNodeOutputsTable->currentItem());
            }
            else
            {
                //conn->target->disconnectFrom(m_inputNode, conn->type);
                listTargetNodeConnections(ui->targetNodeInputsTable->currentItem());
            }
        }
	}

	// disconnection procedure differs wether the selected connection is an input (target) or an output (source)
    // In both cases the disconnectFrom of the source is called

    updateAllNodeConnectionsTable();
    NEGUI.updateNodeGraphEditor();//Update NodeGraphEditor
}

void NEConnectionInspector::loadSourceOutputTypes(NENode *val)
{
	NEConnectableItem* item;
	// remove old table content
	ui->sourceNodeOutputsTable->clear();
	// Select the node that is passed as an argument
	if(val)
		m_pIOSourceNode = val;
	else
		//Otherwise, set currently selected node as output node
		m_pIOSourceNode = NESCENE.selectedNode();

	if(!m_pIOSourceNode)
    {
        ui->sourceNodeOutputsTable->setHeaderLabel("(none selected)");
		return;
    }
	// set node name as table headline
    ui->sourceNodeOutputsTable->setHeaderLabel(m_pIOSourceNode->objectName());

    uint outputCount = m_pIOSourceNode->numOutputs();
    int enumOutputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
    QMetaEnum outputEnum = NE::Instance().metaObject()->enumerator(enumOutputIndex);
    for (uint j = 0; j < outputCount; j++)
    {
        uint outputs = m_pIOSourceNode->outputs(j);
        for (int i = 0; i < outputEnum.keyCount(); ++i)
        {
            // add an entry if the node accepts the according output
            const char* name = outputEnum.key(i);
            uint ioType = (uint)outputEnum.keyToValue(name);
            if (outputs & ioType)
            {
                item = new NEConnectableItem(name);
                item->setType(ioType);
                item->setIndex(j);
                ui->sourceNodeOutputsTable->addTopLevelItem(item);
            }
        }
    }
    ui->sourceNodeOutputsTable->resizeColumnToContents(0);
    ui->sourceNodeOutputsTable->resizeColumnToContents(1);
}

void NEConnectionInspector::customSortByColumn( int column )
{
    // here you can get the order
    Qt::SortOrder order = ui->allNodeConnectionsTable->header()->sortIndicatorOrder();

    if( column == 1 || column == 2 )
        m_TargetSort = true;
    else
        m_TargetSort = false;

    updateAllNodeConnectionsTable();
    // and sort the items
    ui->allNodeConnectionsTable->sortItems( column, order );

}

void NEConnectionInspector::loadTargetInputTypes(NENode *val)
{
	NEConnectableItem* item;
	// remove old table content
	ui->targetNodeInputsTable->clear();
	// set currently selected node as input node
	if(val)
		m_pIOTargetNode = val;
//    else
//        m_pIOTargetNode = NESCENE.selectedNode();

	if(!m_pIOTargetNode)
    {
        ui->targetNodeInputsTable->setHeaderLabel("(none selected)");
		return;
    }
	// set node name as table headline
    ui->targetNodeInputsTable->setHeaderLabel(m_pIOTargetNode->objectName());

    uint inputCount = m_pIOTargetNode->numInputs();
    int enumInputIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
    QMetaEnum inputEnum = NE::Instance().metaObject()->enumerator(enumInputIndex);
    for (uint j = 0; j < inputCount; j++)
    {
        uint inputs = m_pIOTargetNode->inputs(j);
        for(int i = 0; i < inputEnum.keyCount(); ++i)
        {
            // add an entry if the node accepts the according input
            const char* name = inputEnum.key(i);
            uint ioType = (uint)inputEnum.keyToValue(name);
            if (inputs & ioType)
            {
                item = new NEConnectableItem(name);
                item->setType(ioType);
                item->setIndex(j);

                // If an output node is selected, only show entries that are compatible with the selected output type
                if(m_pIOSourceNode)
                {
                    NEConnectableItem* oItem = static_cast<NEConnectableItem*>(ui->sourceNodeOutputsTable->currentItem());

                    if(oItem)
                    {
                        if(item->type() != oItem->type())
                        {
                            item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
                            item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
                            item->setForeground(1, *NEGUISTYLE.GrayFontBrush());
                        }
                    }
                }
                ui->targetNodeInputsTable->addTopLevelItem(item);
            }
        }
    }
    ui->targetNodeInputsTable->resizeColumnToContents(0);
    ui->targetNodeInputsTable->resizeColumnToContents(1);
}

void NEConnectionInspector::loadTargetInputTypesForAvailableTarget()
{
    if(m_pIOTargetNode)
        loadTargetInputTypes();
}

void NEConnectionInspector::reloadTargetInputTypes()
{
    m_pIOTargetNode = NESCENE.selectedNode();
    loadTargetInputTypes();
}

void NEConnectionInspector::listTargetNodeConnections(QTreeWidgetItem *item)
{
	NEConnectableItem* cItem = static_cast<NEConnectableItem*>(item);
	// store that an output type has been selected
	m_TargetIOsShown = true;
	// remove old table content
	ui->connectedNodesTable->clear();

	uint type = 0;

	// return immediately if item is not valid
	if(item)
		type = cItem->type();
	else
		return;

	// set the table headline
	ui->nodeConnectionsLabel->setText("To " + m_pIOTargetNode->objectName());
	ui->connectedNodesTable->setHeaderLabel("From");

	QMap<uint, NENode::NEConn>& connMap = m_pIOTargetNode->sourceConnections();
	QMap<uint, NENode::NEConn>::iterator i = connMap.begin();

	// add an entry for each output connection of the specified type
	for(; i!=connMap.end(); ++i)
	{
        if((i.value().type & type) == (int)type && (i.value().tIndex == cItem->getIndex()))
		{
			QStringList strList;
            strList << i.value().source->objectName();
            strList << ui->targetNodeInputsTable->currentItem()->text(0);
            strList << QString::number( i.value().sIndex );
            strList << QString::number( i.value().tIndex );
            strList << QString::number( i.value().order );
			NEConnectedItem* item = new NEConnectedItem(strList);

			item->setConnection(&(i.value()));
			item->setSource(this->m_pIOSourceNode);

			ui->connectedNodesTable->addTopLevelItem(item);
		}
	}

	ui->connectedNodesTable->resizeColumnToContents(0);
	ui->connectedNodesTable->resizeColumnToContents(1);
    ui->connectedNodesTable->resizeColumnToContents(2);
    ui->connectedNodesTable->resizeColumnToContents(3);
    ui->connectedNodesTable->resizeColumnToContents(4);
}

void NEConnectionInspector::listSourceNodeConnections(QTreeWidgetItem* item)
{
	NEConnectableItem* cItem = static_cast<NEConnectableItem*>(item);
	// store that an output type has been selected
	m_TargetIOsShown = false;
	// remove old table content
	ui->connectedNodesTable->clear();

	uint type = 0;

	// return immediately if item is not valid
	if(item)
		type = cItem->type();
	else
		return;

	// set the table headline
	ui->nodeConnectionsLabel->setText("From " + m_pIOSourceNode->objectName());
	ui->connectedNodesTable->setHeaderLabel("To");

	QMap<uint, NENode::NEConn>& connMap = m_pIOSourceNode->connections();
	QMap<uint, NENode::NEConn>::iterator i = connMap.begin();

	// add an entry for each output connection of the specified type
	for(; i!=connMap.end(); ++i)
	{
        if((i.value().type & type) == (int)type && (i.value().sIndex == cItem->getIndex()))
		{
			QStringList strList;
            strList << i.value().target->objectName();
            strList << ui->sourceNodeOutputsTable->currentItem()->text(0);
            strList << QString::number( i.value().sIndex );
            strList << QString::number( i.value().tIndex );
            strList << QString::number( i.value().order );
            NEConnectedItem* item = new NEConnectedItem(strList);

			item->setConnection(&(i.value()));
			item->setSource(this->m_pIOSourceNode);

			ui->connectedNodesTable->addTopLevelItem(item);
		}
	}

	ui->connectedNodesTable->resizeColumnToContents(0);
	ui->connectedNodesTable->resizeColumnToContents(1);
    ui->connectedNodesTable->resizeColumnToContents(2);
    ui->connectedNodesTable->resizeColumnToContents(3);
    ui->connectedNodesTable->resizeColumnToContents(4);
}


void NEConnectionInspector::updateAllNodeConnectionsTable()
{
	ui->allNodeConnectionsTable->clear();

	// Loop over all nodes
	foreach(QObject* obj, NESCENE.children())
	{
		NENode* node = qobject_cast<NENode*>(obj);
		if(!node)
			continue;

        //
        // If we need to look at all source -> target connections
        //
        if( m_TargetSort == false )
        {
            NEConnectedItem* item = 0;

            if(node->connections().size())
            {
                item = new NEConnectedItem(QStringList(node->objectName()));
                item->setBackground(0, *NEGUISTYLE.windowBrush());
                item->setBackground(1, *NEGUISTYLE.windowBrush());
                item->setBackground(2, *NEGUISTYLE.windowBrush());
                item->setBackground(3, *NEGUISTYLE.windowBrush());
                item->setBackground(4, *NEGUISTYLE.windowBrush());
                item->setBackground(5, *NEGUISTYLE.windowBrush());
                item->setFlags(item->flags()^Qt::ItemIsSelectable);
                ui->allNodeConnectionsTable->addTopLevelItem(item);
            }

            if(node->connections().size())
            {	//Loop over all connections
                QMap<uint,NENode::NEConn>::iterator iter = node->connections().begin();
                for(;iter != node->connections().end(); ++iter)
                {
                    int enumIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
                    QMetaEnum ioEnum = NE::Instance().metaObject()->enumerator(enumIndex);
                    QStringList names;
                    names << node->objectName();
                    names << iter->target->objectName();
                    //
                    // Output the input connection number order - loop through target's incoming sources
                    // and find the source which matches that of the current connections's source - and output that order
                    // - that is the incoming order of connection
                    //
                    bool bExistOrder = false;
                    QString strOrder;
                    QMap<uint,NENode::NEConn>::const_iterator targetConnectionsIter = iter->target->sourceConnections().begin();
                    for( ;targetConnectionsIter != iter->target->sourceConnections().end(); ++targetConnectionsIter )
                    {
                        if( targetConnectionsIter->source == iter->source )
                        {
                            if (bExistOrder)
                                strOrder += ", " + QString::number(targetConnectionsIter->order);
                            else
                                strOrder = QString::number(targetConnectionsIter->order);
//                            names << QString::number(targetConnectionsIter->order);
                            bExistOrder = true;
                        }
                    }
                    if (!bExistOrder)
                        strOrder = "0";
                    names << strOrder;
                    names << QString(ioEnum.valueToKey((int)iter->type));
                    names << QString::number(iter->sIndex);
                    names << QString::number(iter->tIndex);
                    NEConnectedItem* subItem = new NEConnectedItem(names);
                    subItem->setSource( node );
                    subItem->setConnection( &(iter.value()) );
                    item->addChild(subItem);
                    item->setExpanded(true);
                }
            }
        }
        //
        // If we need to look at all target <- source connections
        //
        else
        {
            NEConnectedItem* item = 0;

            if( node->sourceConnections().size() )
            {
                item = new NEConnectedItem( QStringList(node->objectName()) );
                item->setBackground( 0, *NEGUISTYLE.windowBrush() );
                item->setBackground( 1, *NEGUISTYLE.windowBrush() );
                item->setBackground( 2, *NEGUISTYLE.windowBrush() );
                item->setBackground( 3, *NEGUISTYLE.windowBrush() );
                item->setBackground( 4, *NEGUISTYLE.windowBrush() );
                item->setBackground( 5, *NEGUISTYLE.windowBrush() );
                item->setFlags( item->flags()^Qt::ItemIsSelectable );
                ui->allNodeConnectionsTable->addTopLevelItem( item );
            }

            if(node->sourceConnections().size())
            {	//Loop over all connections
                QMap<uint,NENode::NEConn>::iterator iter = node->sourceConnections().begin();
                for(;iter != node->sourceConnections().end(); ++iter)
                {
                    int enumIndex = NE::Instance().metaObject()->indexOfEnumerator("NEIoType");
                    QMetaEnum ioEnum = NE::Instance().metaObject()->enumerator(enumIndex);
                    QStringList names;
                    names << iter->source->objectName();
                    names << node->objectName();
                    names << QString::number(iter->order);
                    names << QString(ioEnum.valueToKey((int)iter->type));
                    names << QString::number(iter->sIndex);
                    names << QString::number(iter->tIndex);
                    NEConnectedItem* subItem = new NEConnectedItem(names);
                    subItem->setSource(node);
                    subItem->setConnection(&(iter.value()));
                    item->addChild(subItem);
                    item->setExpanded(true);
                }
            }
        }
	}


	ui->allNodeConnectionsTable->resizeColumnToContents(0);
	ui->allNodeConnectionsTable->resizeColumnToContents(1);
    ui->allNodeConnectionsTable->resizeColumnToContents(2);
    ui->allNodeConnectionsTable->resizeColumnToContents(3);
    ui->allNodeConnectionsTable->resizeColumnToContents(4);
    ui->allNodeConnectionsTable->resizeColumnToContents(5);
}

/*!
	Determines the selected node and shows the input it accepts in the input table
*/
void NEConnectionInspector::loadTargetNodeProperties(NENode *val)
{
	NEConnectableItem* item;
	// remove old table content
	ui->targetPropsTable->clear();
	// set currently selected node as input node

	if(val)
		m_pPropTargetNode = val;

	if(!m_pPropTargetNode)
    {
        ui->targetPropsTable->setHeaderLabel("(none selected)");
        return;
    }
	// set node name as table headline
	ui->targetPropsTable->setHeaderLabel(m_pPropTargetNode->objectName());

	// Now, list all connectable properties of the node
	const QMetaObject* metaObject = m_pPropTargetNode->metaObject();
	for(int i=0; i<metaObject->propertyCount(); ++i)
	{
		QMetaProperty metaProp = metaObject->property(i);
        if(!metaProp.isDesignable(val))
            continue;
		item = new NEConnectableItem(metaProp.name());
		item->setFont(0, *NEGUISTYLE.ItalicFont());
		// Distinguish between user type and the default QVariant types
		if(metaProp.type() == QVariant::UserType)
        {
            item->setType(metaProp.userType());


            if(metaProp.userType() == (int)getNEVarIds().vec2fId)
            {
                NEConnectableItem* xItem = new NEConnectableItem("x");
                NEConnectableItem* yItem = new NEConnectableItem("y");
                xItem->setType(QMetaType::Double);
                yItem->setType(QMetaType::Double);
                xItem->setFont(0, *NEGUISTYLE.ItalicFont());
                yItem->setFont(0, *NEGUISTYLE.ItalicFont());
                item->addChild(xItem);
                item->addChild(yItem);
            }
            else if(metaProp.userType() == (int)getNEVarIds().vec3fId)
            {
                NEConnectableItem* xItem = new NEConnectableItem("x");
                NEConnectableItem* yItem = new NEConnectableItem("y");
                NEConnectableItem* zItem = new NEConnectableItem("z");
                xItem->setType(QMetaType::Double);
                yItem->setType(QMetaType::Double);
                zItem->setType(QMetaType::Double);
                xItem->setFont(0, *NEGUISTYLE.ItalicFont());
                yItem->setFont(0, *NEGUISTYLE.ItalicFont());
                zItem->setFont(0, *NEGUISTYLE.ItalicFont());
                item->addChild(xItem);
                item->addChild(yItem);
                item->addChild(zItem);
            }
        }
		else
			item->setType(metaProp.type());


		// If an output node is selected, only show entries that are compatible with the selected output type
		if(m_pPropSourceNode)
		{
            NEConnectableItem* oItem = static_cast<NEConnectableItem*>(ui->sourcePropsTable->currentItem());

			if(oItem)
			{
				if(item->type() != oItem->type())
				{
					item->setFlags(item->flags() ^ Qt::ItemIsSelectable);
					item->setForeground(0, *NEGUISTYLE.GrayFontBrush());
				}
			}
		}
		ui->targetPropsTable->addTopLevelItem(item);
    }
}

void NEConnectionInspector::loadTargetNodePropertiesForAvailableTarget()
{
    if(m_pPropTargetNode)
        loadTargetNodeProperties();
}

void NEConnectionInspector::reloadTargetProperties()
{
    m_pPropTargetNode = NESCENE.selectedNode();
    loadTargetNodeProperties();
}


/*!
	Determines the selected node and shows which outputs are possible
*/
void NEConnectionInspector::loadSourceNodeProperties(NENode *val)
{
	NEConnectableItem* item;
	// remove old table content
	ui->sourcePropsTable->clear();

	if(val)
		m_pPropSourceNode = val;
	else
		m_pPropSourceNode = NESCENE.selectedNode();

	if(!m_pPropSourceNode)
    {
        ui->sourcePropsTable->setHeaderLabel(("(none selected)"));
		return;
    }
	// set node name as table headline
	ui->sourcePropsTable->setHeaderLabel(m_pPropSourceNode->objectName());

	// Now, list all properties of the node
	const QMetaObject* metaObject = m_pPropSourceNode->metaObject();
	for(int i=0; i<metaObject->propertyCount(); ++i)
	{
		QMetaProperty metaProp = metaObject->property(i);
        if(!metaProp.isDesignable(val))
            continue;
		item = new NEConnectableItem(metaProp.name());
		item->setFont(0, *NEGUISTYLE.ItalicFont());
		// Distinguish between user type and the default QVariant types
		if(metaProp.type() == QVariant::UserType)
        {
            item->setType(metaProp.userType());

            if(metaProp.userType() == (int)getNEVarIds().vec2fId)
            {
                NEConnectableItem* xItem = new NEConnectableItem("x");
                NEConnectableItem* yItem = new NEConnectableItem("y");
                xItem->setType(QMetaType::Double);
                yItem->setType(QMetaType::Double);
                xItem->setFont(0, *NEGUISTYLE.ItalicFont());
                yItem->setFont(0, *NEGUISTYLE.ItalicFont());
                item->addChild(xItem);
                item->addChild(yItem);
            }
            else if(metaProp.userType() == (int)getNEVarIds().vec3fId)
            {
                NEConnectableItem* xItem = new NEConnectableItem("x");
                NEConnectableItem* yItem = new NEConnectableItem("y");
                NEConnectableItem* zItem = new NEConnectableItem("z");
                xItem->setType(QMetaType::Double);
                yItem->setType(QMetaType::Double);
                zItem->setType(QMetaType::Double);
                xItem->setFont(0, *NEGUISTYLE.ItalicFont());
                yItem->setFont(0, *NEGUISTYLE.ItalicFont());
                zItem->setFont(0, *NEGUISTYLE.ItalicFont());
                item->addChild(xItem);
                item->addChild(yItem);
                item->addChild(zItem);
            }
        }
		else
			item->setType(metaProp.type());

		ui->sourcePropsTable->addTopLevelItem(item);
	}
}

/*!
	Connects an output node to an input node. When the user clicks on connect, not called internally.
*/
void NEConnectionInspector::connectProperties()
{
	// check whether both nodes have been selected
	if(m_pPropTargetNode && m_pPropSourceNode)
	{
		// checks whether both, input and output types, have been selected
		if(ui->targetPropsTable->currentItem() && ui->sourcePropsTable->currentItem())
		{
            NEConnectableItem* targetItem = (NEConnectableItem*)ui->targetPropsTable->currentItem();
            NEConnectableItem* sourceItem = (NEConnectableItem*)ui->sourcePropsTable->currentItem();

            QString sourceProp = sourceItem->text(0);
            QString targetProp = targetItem->text(0);

            // For handling sub-properties
            if(sourceItem->parent())
            {
                NEConnectableItem* parentItem = (NEConnectableItem*)sourceItem->parent();
                sourceProp.prepend(parentItem->text(0));
            }
            if(targetItem->parent())
            {
                NEConnectableItem* parentItem = (NEConnectableItem*)targetItem->parent();
                targetProp.prepend(parentItem->text(0));
            }


			// check whether input and output type are the same
            if(targetItem->type() == sourceItem->type())
			{	// If it's a node based connection
				int value = ui->orderSpinBox2->value();
				QString valstr = QString("%1").arg(value);
				QString cmd = m_pPropSourceNode->objectName() + ".connectTo(" + m_pPropTargetNode->objectName() +
					", \"" + sourceProp + "\", \"" + targetProp + "\", " + valstr + ")";
                getPyMain().evalScript(cmd);

			}
			// updates the connection table
			m_TargetPropertiesShown ? listTargetPropertyConnections(ui->targetPropsTable->currentItem()) : listSourcePropertyConnections(ui->sourcePropsTable->currentItem());
			// Update the connection tables
			updateAllPropertyConnectionsTable();
		}
	}
}

/*!
	Removes a connection from a node based on a selection in the connection table
*/
void NEConnectionInspector::removePropertyConnection()
{
    NEConnectedItem* item = 0;
    if(!ui->connectedPropsTable->selectedItems().isEmpty())
	{
        item = static_cast<NEConnectedItem*>(ui->connectedPropsTable->currentItem());
		if(item)
		{
			if(!m_TargetPropertiesShown)
			{
				NENode::NEPropConn* conn = item->propertyConnection();
                QString cmd = item->source()->objectName() + ".disconnectFrom(" + conn->target->objectName() + ", \"" + conn->srcProp + "\"" + ", \"" + conn->trgProp + "\")";
                getPyMain().evalScript(cmd);
				//m_outputNode->disconnectFrom(conn->target, conn->srcProp, conn->trgProp);
				listSourcePropertyConnections(ui->sourcePropsTable->currentItem());
			}
			else
			{
				NENode::NEPropConn* conn = item->propertyConnection();
                QString cmd = conn->target->objectName() + ".disconnectFrom(" + item->source()->objectName() + ", \"" + conn->srcProp + "\"" + ", \"" + conn->trgProp + "\")";
                getPyMain().evalScript(cmd);
				//conn->target->disconnectFrom(m_inputNode, conn->srcProp, conn->trgProp);
				listTargetPropertyConnections(ui->targetPropsTable->currentItem());
			}
			updateAllPropertyConnectionsTable();
			ui->propertyExpressionEdit->clear();
		}
	}
    else if(!ui->allPropertyConnectionsTable->selectedItems().isEmpty())
	{

        item = static_cast<NEConnectedItem*>(ui->allPropertyConnectionsTable->currentItem());
		if(!item || !item->source())
			return;

		if(!m_TargetPropertiesShown)
		{
			NENode::NEPropConn* conn = item->propertyConnection();
            QString cmd;
            cmd = item->source()->objectName() + ".disconnectFrom(" + conn->target->objectName() + ", \"" + conn->srcProp + "\"" + ", \"" + conn->trgProp + "\")";
            //std::cout << cmd.toStdString() << std::endl;
            getPyMain().evalScript( cmd);
			//m_outputNode->disconnectFrom(conn->target, conn->srcProp, conn->trgProp);
			listSourcePropertyConnections(ui->sourcePropsTable->currentItem());
		}
		else
		{
			NENode::NEPropConn* conn = item->propertyConnection();
			QString cmd = item->source()->objectName() + ".disconnectFrom(" + conn->target->objectName() + ", \"" + conn->srcProp + "\"" + ", \"" + conn->trgProp + "\")";
            getPyMain().evalScript(cmd);
			//conn->target->disconnectFrom(m_inputNode, conn->srcProp, conn->trgProp);
			listTargetPropertyConnections(ui->targetPropsTable->currentItem());
		}
		updateAllPropertyConnectionsTable();
		ui->propertyExpressionEdit->clear();
	}
	// check whether the item is valid
}


/*!
	Updates the connection table for a selected output type
*/
void NEConnectionInspector::listSourcePropertyConnections(QTreeWidgetItem* item)
{
	// store that an output type has been selected
	m_TargetPropertiesShown = false;
	// remove old table content
	ui->connectedPropsTable->clear();

	// return immediately if item is not valid
	if(!item)
		return;

	// set the table headline
	ui->propertyConnectionsLabel->setText("From " + m_pPropSourceNode->objectName());
	ui->connectedPropsTable->setHeaderLabel("To");

	QMap<QString, QMap<NENode*, NENode::NEPropConn>>& connMap = m_pPropSourceNode->propertyConnections();
	QMap<QString, QMap<NENode*, NENode::NEPropConn>>::iterator i = connMap.begin();

	for(; i!=connMap.end(); ++i)
	{
		QMap<NENode*, NENode::NEPropConn>::iterator j = i->begin();
		for(; j!=i->end(); ++j)
		{
            QString txt = ui->sourcePropsTable->currentItem()->text(0);

            if(ui->sourcePropsTable->currentItem()->parent())
                txt.prepend(ui->sourcePropsTable->currentItem()->parent()->text(0));

            if(txt == j.value().srcProp)
			{
				QStringList strList;
				strList << j.value().target->objectName() << j.value().srcProp + "->" + j.value().trgProp;
				NEConnectedItem* item = new NEConnectedItem(strList);

				item->setPropertyConnection(&(j.value()));
				item->setSource(this->m_pPropSourceNode);

				ui->connectedPropsTable->addTopLevelItem(item);
			}
		}
	}
	// Clear the property expression editor
	ui->propertyExpressionEdit->clear();

	ui->connectedPropsTable->resizeColumnToContents(0);
	ui->connectedPropsTable->resizeColumnToContents(1);
}

/*!
	Updates the connection table for a selected input type
*/
void NEConnectionInspector::listTargetPropertyConnections(QTreeWidgetItem *item)
{
	// store that an input type has been selected
	m_TargetPropertiesShown = true;
	// remove old table content
	ui->connectedPropsTable->clear();

	// return immediately if item is not valid
	if(!item)
		return;
	// set the table headline
	ui->propertyConnectionsLabel->setText("To " + m_pPropTargetNode->objectName());
	ui->connectedPropsTable->setHeaderLabel(tr("From"));

    QMap<QString, NENode::NESrcPropConn>& connMap = m_pPropTargetNode->sourcePropertyConnections();
    QMap<QString, NENode::NESrcPropConn>::iterator i = connMap.begin();

	for(; i!=connMap.end(); ++i)
	{
        QString txt = ui->targetPropsTable->currentItem()->text(0);

        if(ui->targetPropsTable->currentItem()->parent())
            txt.prepend(ui->targetPropsTable->currentItem()->parent()->text(0));


        if(txt == i.value().trgProp)
		{
			QStringList strList;
			strList << i.value().target->objectName() << i.value().srcProp + "->" + i.value().trgProp;
			NEConnectedItem* item = new NEConnectedItem(strList);

            item->setSourcePropertyConnection(&(i.value()));
			item->setSource(this->m_pPropTargetNode);

			ui->connectedPropsTable->addTopLevelItem(item);
		}
	}
	// Clear the property expression editor
	ui->propertyExpressionEdit->clear();

	ui->connectedPropsTable->resizeColumnToContents(0);
	ui->connectedPropsTable->resizeColumnToContents(1);
}

/*!
	This function populates the global connections table by iterating all NENode children in the NESceneManager
*/
void NEConnectionInspector::updateAllPropertyConnectionsTable()
{
	ui->allPropertyConnectionsTable->clear();

	// Loop over all nodes
	foreach(QObject* obj, NESCENE.children())
	{
		NENode* node = qobject_cast<NENode*>(obj);
		if(!node)
			continue;

		NEConnectedItem* item = 0;

		if(node->propertyConnections().size())
		{
			item = new NEConnectedItem(QStringList(node->objectName()));
            item->setBackground(0, *NEGUISTYLE.windowBrush());
            item->setBackground(1, *NEGUISTYLE.windowBrush());
            item->setBackground(2, *NEGUISTYLE.windowBrush());
			item->setFlags(item->flags()^Qt::ItemIsSelectable);
			ui->allPropertyConnectionsTable->addTopLevelItem(item);
		}

		if(node->propertyConnections().size())
		{	//Loop over all property connections
			for(QMap<QString, QMap<NENode*, NENode::NEPropConn>>::iterator i = node->propertyConnections().begin(); i != node->propertyConnections().end(); ++i)
            {
                QMap<NENode*,NENode::NEPropConn>::iterator j = i->begin();
                for(; j != i->end(); ++j)
                {
                    QStringList names;
                    names << node->objectName() << j->target->objectName() << j->srcProp + "->" + j->trgProp;
					NEConnectedItem* subItem = new NEConnectedItem(names);
					subItem->setSource(node);
					subItem->setPropertyConnection(&(j.value()));
                    item->addChild(subItem);
                    item->setExpanded(true);
                }
            }
		}
	}

	ui->allPropertyConnectionsTable->resizeColumnToContents(0);
	ui->allPropertyConnectionsTable->resizeColumnToContents(1);
	ui->allPropertyConnectionsTable->resizeColumnToContents(2);
}


void NEConnectionInspector::loadNodeProperties()
{
	// If the node was already selected, do nothing
	if(m_pCallbackNode == NESCENE.selectedObject())
		return;
	// Otherwise, clear the callback expression editor
	else
		ui->callbackExpressionEdit->clear();

	// remove old table content
	ui->nodePropsTable->clear();

	// set currently selected node as output node
	m_pCallbackNode = NESCENE.selectedNode();

	// If nothing (0) is selected, return
	if(!m_pCallbackNode)
		return;
	// set node name as table headline
	ui->nodePropsTable->setHeaderLabel(m_pCallbackNode->objectName());

	// Now, list all properties of the node
	const QMetaObject* metaObject = m_pCallbackNode->metaObject();
	for(int i=0; i<metaObject->propertyCount(); ++i)
	{
		// Add all properties to the tree
		QMetaProperty metaProp = metaObject->property(i);

        if(!metaProp.isDesignable(m_pCallbackNode))
            continue;

        NECallbackableItem* item = new NECallbackableItem(metaProp.name());
        item->setSource(m_pCallbackNode);
        item->setFont(0, *NEGUISTYLE.ItalicFont());

        if(metaProp.type() == QVariant::UserType)
        {
            if(metaProp.userType() == (int)getNEVarIds().vec2fId)
            {
                NECallbackableItem* xItem = new NECallbackableItem("x");
                NECallbackableItem* yItem = new NECallbackableItem("y");
                xItem->setFont(0, *NEGUISTYLE.ItalicFont());
                yItem->setFont(0, *NEGUISTYLE.ItalicFont());
                xItem->setSource(m_pCallbackNode);
                yItem->setSource(m_pCallbackNode);
                item->addChild(xItem);
                item->addChild(yItem);
            }
            if(metaProp.userType() == (int)getNEVarIds().vec3fId)
            {
                NECallbackableItem* xItem = new NECallbackableItem("x");
                NECallbackableItem* yItem = new NECallbackableItem("y");
                NECallbackableItem* zItem = new NECallbackableItem("z");
                xItem->setFont(0, *NEGUISTYLE.ItalicFont());
                yItem->setFont(0, *NEGUISTYLE.ItalicFont());
                zItem->setFont(0, *NEGUISTYLE.ItalicFont());
                xItem->setSource(m_pCallbackNode);
                yItem->setSource(m_pCallbackNode);
                zItem->setSource(m_pCallbackNode);
                item->addChild(xItem);
                item->addChild(yItem);
                item->addChild(zItem);
            }
        }
		ui->nodePropsTable->addTopLevelItem(item);
	}
}

void NEConnectionInspector::showCallbackExpression(QTreeWidgetItem *item)
{
	ui->callbackExpressionEdit->clear();

	NECallbackableItem* item2 = static_cast<NECallbackableItem*>(item);
	if(!item2->source())
		return;

	m_pCallbackNode = item2->source();

    QString propname = item2->text(0);
    if(item2->parent())
    {
        NECallbackableItem* parentItem = static_cast<NECallbackableItem*>(item2->parent());
        if(parentItem->source())
            propname.prepend(parentItem->text(0));
    }

    if(item2->source()->hasCallbackExpression(propname))
    {
        QString expr = item2->source()->propertyCallbackExpression(propname);
        ui->callbackExpressionEdit->insertPlainText(expr);
    }
}

void NEConnectionInspector::savePropertyCallback()
{
	NECallbackableItem* item2 = 0;
	if(!ui->nodePropsTable->selectedItems().isEmpty())
		item2 = static_cast<NECallbackableItem*>(ui->nodePropsTable->currentItem());
	else if(!ui->activeNodeCallbacksTree->selectedItems().isEmpty())
		item2 = static_cast<NECallbackableItem*>(ui->activeNodeCallbacksTree->currentItem());
	else if(!ui->allCallbackExpressionsTable->selectedItems().isEmpty())
		item2 = static_cast<NECallbackableItem*>(ui->allCallbackExpressionsTable->currentItem());

	if(!item2 || !item2->source())
		return;

    QString propname = item2->text(0);
    if(item2->parent())
        propname.prepend(item2->parent()->text(0));

	QString txt = ui->callbackExpressionEdit->toPlainText();
	if(txt.isEmpty())
        m_pCallbackNode->removePropertyCallbackExpression(propname);
	else
        m_pCallbackNode->setPropertyCallbackExpression(propname, txt);

	// Disable the button after saving the expression
	ui->saveCallbackExpressionButton->setDisabled(true);
}

void NEConnectionInspector::removePropertyCallback()
{
	NECallbackableItem* item2 = static_cast<NECallbackableItem*>(ui->nodePropsTable->currentItem());

	if(!item2)
		item2 = static_cast<NECallbackableItem*>(ui->activeNodeCallbacksTree->currentItem());
	if(!item2)
		item2 = static_cast<NECallbackableItem*>(ui->allCallbackExpressionsTable->currentItem());
	if(!item2 || !item2->source())
		return;

	m_pCallbackNode->removePropertyCallbackExpression(item2->text(0));
	ui->callbackExpressionEdit->clear();
}

void NEConnectionInspector::listPropertiesWithCallback()
{
	// remove old table content
	ui->activeNodeCallbacksTree->clear();

	// return if the node does not have any property callback set
	if(!m_pCallbackNode || !m_pCallbackNode->propertyCallbackExpressionsMap().size())
		return;

    QMap<int, QString>::iterator i = m_pCallbackNode->propertyCallbackExpressionsMap().begin();

	for(; i!=m_pCallbackNode->propertyCallbackExpressionsMap().end(); ++i)
	{
        const char* pName = m_pCallbackNode->metaObject()->property(i.key()).name();
        NECallbackableItem* item = new NECallbackableItem(pName);
		item->setSource(m_pCallbackNode);
		ui->activeNodeCallbacksTree->addTopLevelItem(item);
	}
	ui->activeNodeCallbacksTree->resizeColumnToContents(0);
	ui->activeNodeCallbacksTree->resizeColumnToContents(1);
}

void NEConnectionInspector::loadInternalConnectionsOfANode()
{
	if(NESCENE.selectedNode() == m_pInternalConnectionsNode)
		return;

	m_pInternalConnectionsNode = NESCENE.selectedNode();

    reloadInternalConnectionsOfTheActiveNode();
}

void NEConnectionInspector::reloadInternalConnectionsOfTheActiveNode()
{
    if(!m_pInternalConnectionsNode)
    {
        ui->incomingInternalConnsLabel->setText("Incoming Internal Connections");
        ui->outgoingInternalConnsLabel->setText("Outgoing Internal Connections");
        ui->outgoingInternalConnsTree->clear();
        ui->incomingInternalConnsTree->clear();
        return;
    }

    QMultiMap<QObject*, NENode::NEIntConn>& inInConnMap = m_pInternalConnectionsNode->incomingInternalConnectionsMap();
    QMultiMap<QObject*, NENode::NEIntConn>& outInConnMap = m_pInternalConnectionsNode->outgoingInternalConnectionsMap();

    ui->outgoingInternalConnsTree->clear();
    ui->incomingInternalConnsTree->clear();

    ui->incomingInternalConnsLabel->setText("Incoming Internal Connections to: " + m_pInternalConnectionsNode->objectName());

    QMultiMap<QObject*, NENode::NEIntConn>::iterator i;
    for(i=inInConnMap.begin(); i!= inInConnMap.end(); ++i)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, i.value().sender->objectName());
        item->setText(1, i.value().signal);
        item->setText(2, i.value().receiver->objectName());
        item->setText(3, i.value().slot);

        ui->incomingInternalConnsTree->addTopLevelItem(item);
    }
    ui->incomingInternalConnsTree->resizeColumnToContents(0);
    ui->incomingInternalConnsTree->resizeColumnToContents(1);
    ui->incomingInternalConnsTree->resizeColumnToContents(2);
    ui->incomingInternalConnsTree->resizeColumnToContents(3);

    ui->outgoingInternalConnsLabel->setText("Outgoing Internal Connections from: " + m_pInternalConnectionsNode->objectName());

    for(i=outInConnMap.begin(); i!= outInConnMap.end(); ++i)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, i.value().sender->objectName());
        item->setText(1, i.value().signal);
        item->setText(2, i.value().receiver->objectName());
        item->setText(3, i.value().slot);

        ui->outgoingInternalConnsTree->addTopLevelItem(item);
    }
    ui->outgoingInternalConnsTree->resizeColumnToContents(0);
    ui->outgoingInternalConnsTree->resizeColumnToContents(1);
    ui->outgoingInternalConnsTree->resizeColumnToContents(2);
    ui->outgoingInternalConnsTree->resizeColumnToContents(3);

}

void NEConnectionInspector::swapIONodes()
{
	NENode* newTarget = m_pIOSourceNode;
	NENode* newSource = m_pIOTargetNode;
	loadTargetInputTypes(newTarget);
	loadSourceOutputTypes(newSource);
	ui->connectedNodesTable->clear();
	ui->nodeConnectionsLabel->setText("(none selected)");
}

void NEConnectionInspector::swapPropertyNodes()
{
	NENode* newTarget = m_pPropSourceNode;
	NENode* newSource = m_pPropTargetNode;
	loadTargetNodeProperties(newTarget);
	loadSourceNodeProperties(newSource);
	ui->connectedPropsTable->clear();
	ui->propertyConnectionsLabel->setText("(none selected)");
}

void NEConnectionInspector::nodeConnectionClicked(QTreeWidgetItem *item)
{

}

void NEConnectionInspector::propertyExpressionEditContentsChanged()
{
	ui->savePropertyExpressionButton->setEnabled(true);
}

void NEConnectionInspector::propertyCallbackEditContentsChanged()
{
	ui->saveCallbackExpressionButton->setEnabled(true);
}

void NEConnectionInspector::showNodeConnectionsPage()
{
	ui->stackedWidget->setCurrentIndex(0);
	updateTitlebarText("(Node-Node)");

}

void NEConnectionInspector::showPropertyConnectionsPage()
{
	ui->stackedWidget->setCurrentIndex(1);
	updateTitlebarText("(Property-Property)");
}

void NEConnectionInspector::showInternalConnectionsPage()
{
	ui->stackedWidget->setCurrentIndex(3);
	updateTitlebarText("(Internal)");
}

void NEConnectionInspector::showPropertyCallbacksPage()
{
	ui->stackedWidget->setCurrentIndex(2);
	updateTitlebarText("(Callbacks)");
}


void NEConnectionInspector::updateTitlebarText(QString s)
{
	NEDockWidget* dw = static_cast<NEDockWidget*>(parentWidget());
	dw->setTitleText("Connection Inspector - " + s);
}

void NEConnectionInspector::updateAllPropertyCallbacksTable()
{
	// List the properties with callback for the current node
	listPropertiesWithCallback();

	ui->allCallbackExpressionsTable->clear();

	// Loop over all nodes
	foreach(QObject* obj, NESCENE.children())
	{
		NENode* node = qobject_cast<NENode*>(obj);
		if(!node)
			continue;

		NECallbackableItem* item = 0;

		if(node->propertyCallbackExpressionsMap().size())
		{
            item = new NECallbackableItem(node->objectName());
            item->setBackground(0, *NEGUISTYLE.windowBrush());
            item->setBackground(1, *NEGUISTYLE.windowBrush());
			item->setFlags(item->flags()^Qt::ItemIsSelectable);
			ui->allCallbackExpressionsTable->addTopLevelItem(item);
		}
		else continue;

        for(QMap<int, QString>::iterator i = node->propertyCallbackExpressionsMap().begin(); i != node->propertyCallbackExpressionsMap().end(); ++i)
		{
            //QStringList names;
            //names << i.key() << i.value();
            const char* pName = m_pCallbackNode->metaObject()->property(i.key()).name();
            NECallbackableItem* subItem = new NECallbackableItem(pName);
            subItem->setText(1, i.value());
			subItem->setSource(node);
			item->addChild(subItem);
			item->setExpanded(true);
		}
	}

	ui->allCallbackExpressionsTable->resizeColumnToContents(0);
	ui->allCallbackExpressionsTable->resizeColumnToContents(1);
}

void NEConnectionInspector::updateAllInternalConnectionsTable()
{
	// Clear the table first
	ui->allInternalConnsTree->clear();
	// Loop over all nodes
	foreach(QObject* obj, NESCENE.children())
	{
		NENode* node = qobject_cast<NENode*>(obj);
		if(!node)
			continue;

		QTreeWidgetItem* item = 0;
		if(node->outgoingInternalConnectionsMap().size())
		{
			item = new QTreeWidgetItem(QStringList(node->objectName()));
            item->setBackground(0, *NEGUISTYLE.windowBrush());
            item->setBackground(1, *NEGUISTYLE.windowBrush());
            item->setBackground(2, *NEGUISTYLE.windowBrush());
            item->setBackground(3, *NEGUISTYLE.windowBrush());
			item->setFlags(item->flags()^Qt::ItemIsSelectable);
			ui->allInternalConnsTree->addTopLevelItem(item);
		}
		else
			continue;

		QMultiMap<QObject*, NENode::NEIntConn>& outInConnMap = node->outgoingInternalConnectionsMap();

		QMultiMap<QObject*, NENode::NEIntConn>::iterator i;
		for(i=outInConnMap.begin(); i!= outInConnMap.end(); ++i)
		{
			QTreeWidgetItem* subItem = new QTreeWidgetItem();
			subItem->setText(0, i.value().sender->objectName());
			subItem->setText(1, i.value().signal);
			subItem->setText(2, i.value().receiver->objectName());
            subItem->setText(3, i.value().slot);

			item->addChild(subItem);
			item->setExpanded(true);
		}
	}
	ui->allInternalConnsTree->resizeColumnToContents(0);
	ui->allInternalConnsTree->resizeColumnToContents(1);
	ui->allInternalConnsTree->resizeColumnToContents(2);
    ui->allInternalConnsTree->resizeColumnToContents(3);
}

void NEConnectionInspector::updateNodeRenameToEditors(NENode* node)
{
    if(node == m_pIOSourceNode)
    {
        ui->sourceNodeOutputsTable->setHeaderLabel(m_pIOSourceNode->objectName());
    }

    if(node == m_pIOTargetNode)
    {
        ui->targetNodeInputsTable->setHeaderLabel(m_pIOTargetNode->objectName());
    }

    if(node == m_pPropSourceNode)
    {
        ui->sourcePropsTable->setHeaderLabel(m_pPropSourceNode->objectName());
    }

    if(node == m_pPropTargetNode)
    {
        ui->targetPropsTable->setHeaderLabel(m_pPropTargetNode->objectName());
    }

    if(node == m_pCallbackNode)
    {
        ui->nodePropsTable->setHeaderLabel(m_pCallbackNode->objectName());
    }

    if(node == m_pInternalConnectionsNode)
    {
        reloadInternalConnectionsOfTheActiveNode();
    }

    if(ui->sourceNodeOutputsTable->currentItem())
        listSourceNodeConnections(ui->sourceNodeOutputsTable->currentItem());
    else if(ui->targetNodeInputsTable->currentItem())
        listTargetNodeConnections(ui->targetNodeInputsTable->currentItem());

    if(ui->sourcePropsTable->currentItem())
        listSourcePropertyConnections(ui->sourcePropsTable->currentItem());
    else if(ui->targetPropsTable->currentItem())
        listTargetPropertyConnections(ui->targetPropsTable->currentItem());

    if(ui->connectedPropsTable->currentItem())
        showPropertyConnectionExpression(ui->connectedPropsTable->currentItem());
    else if(ui->allPropertyConnectionsTable->currentItem())
        showPropertyConnectionExpression(ui->allPropertyConnectionsTable->currentItem());

    if(ui->nodePropsTable->currentItem())
        showCallbackExpression(ui->nodePropsTable->currentItem());
    else if(ui->activeNodeCallbacksTree->currentItem())
        showCallbackExpression(ui->activeNodeCallbacksTree->currentItem());
    else if(ui->allCallbackExpressionsTable->currentItem())
        showCallbackExpression(ui->allCallbackExpressionsTable->currentItem());

    updateAllPropertyConnectionsTable();
    updateAllInternalConnectionsTable();
    updateAllNodeConnectionsTable();
    updateAllPropertyCallbacksTable();
}

void NEConnectionInspector::resetEditor()
{
	updateAllPropertyConnectionsTable();
	ui->connectedPropsTable->clear();
	ui->targetPropsTable->clear();
	ui->sourcePropsTable->clear();

	QString label = tr("(none selected)");
	ui->connectedPropsTable->setHeaderLabel(label);
	ui->targetPropsTable->setHeaderLabel(label);
	ui->sourcePropsTable->setHeaderLabel(label);

}

void NEConnectionInspector::deselectObject( QObject* object )
{
	QString label = tr("(none selected)");

	if(m_pPropTargetNode == object)
	{
		// Deselect the target node
		m_pPropTargetNode = 0;
		// Clear the table header label
		ui->targetPropsTable->setHeaderLabel(label);
		ui->targetPropsTable->clear();

		if(m_pPropSourceNode!=object && !m_TargetPropertiesShown)
			listSourcePropertyConnections(ui->sourcePropsTable->currentItem());
		else
		{
			ui->propertyConnectionsLabel->setText("(none selected)");
			ui->connectedPropsTable->clear();
		}
	}
	if(m_pPropSourceNode == object)
	{
		// Deselect the source node
		m_pPropSourceNode = 0;
		// Clear the table header label
		ui->sourcePropsTable->setHeaderLabel(label);
		ui->sourcePropsTable->clear();

		if(m_pPropTargetNode!=object && m_TargetPropertiesShown)
			listTargetPropertyConnections(ui->targetPropsTable->currentItem());
		else
		{
			ui->propertyConnectionsLabel->setText("(none selected)");
			ui->connectedPropsTable->clear();
		}
	}
	// Update the reliated UI when a node that was set to property connection dialog has been deleted
	if(!m_pPropSourceNode || !m_pPropTargetNode)
	{
		// Update all property callbacks table
		updateAllPropertyConnectionsTable();
		// Clear the property expression editor
		ui->propertyExpressionEdit->clear();
	}

	if(m_pIOTargetNode == object)
	{
		// Deselect the target node
		m_pIOTargetNode = 0;

		// Clear the table header label
		ui->targetNodeInputsTable->setHeaderLabel(label);
		ui->targetNodeInputsTable->clear();
		// Re-load source node connections if it is a different node
		if(m_pIOSourceNode!=object && !m_TargetIOsShown)
			listSourceNodeConnections(ui->sourceNodeOutputsTable->currentItem());
		else
		{
			ui->nodeConnectionsLabel->setText("(none selected)");
			ui->connectedNodesTable->clear();
		}
	}

	if(m_pIOSourceNode == object)
	{
		// Deselect the source node
		m_pIOSourceNode = 0;

		// Clear the table header label
		ui->sourceNodeOutputsTable->setHeaderLabel(label);
		ui->sourceNodeOutputsTable->clear();
		if(m_pIOTargetNode!=object && m_TargetIOsShown)
			listTargetNodeConnections(ui->targetNodeInputsTable->currentItem());
		else
		{
			ui->nodeConnectionsLabel->setText("(none selected)");
			ui->connectedNodesTable->clear();
		}
	}
	// Update the reliated UI when a node that was set to node IO connection dialog has been deleted
	if(!m_pIOSourceNode || !m_pIOTargetNode)
		updateAllNodeConnectionsTable();

	if(m_pCallbackNode == object)
	{
		m_pCallbackNode = 0;
		ui->nodePropsTable->setHeaderLabel(label);
		ui->nodePropsTable->clear();
		ui->activeNodeCallbacksTree->clear();
		ui->callbackExpressionEdit->clear();
		updateAllPropertyCallbacksTable();
	}

	// Update the reliated UI when a node that was set to internal connections dialog has been deleted

	if(m_pInternalConnectionsNode == object)
	{
		m_pInternalConnectionsNode = 0;
        ui->incomingInternalConnsLabel->setText("(none selected)");
        ui->outgoingInternalConnsLabel->setText("(none selected)");
		ui->incomingInternalConnsTree->clear();
		ui->outgoingInternalConnsTree->clear();
	}
}


void NEConnectionInspector::showPropertyConnectionExpression(QTreeWidgetItem *item)
{
	if(!item)
		return;
	ui->propertyExpressionEdit->clear();

	NEConnectedItem* item2 = static_cast<NEConnectedItem*>(item);

	if(!item2->propertyConnection())
		return;

    if(item2->source() == this->m_pPropSourceNode)
    {
        NENode::NEPropConn* conn = item2->propertyConnection();
        ui->propertyExpressionEdit->insertPlainText(conn->expr);
    }
    else if(item2->source() == this->m_pPropTargetNode)
    {
        NENode::NESrcPropConn* conn = item2->sourcePropertyConnection();
        ui->propertyExpressionEdit->insertPlainText(*(conn->expr));
    }
}

void NEConnectionInspector::savePropertyConnectionExpression()
{
	// get the selected connection
	NEConnectedItem* item = static_cast<NEConnectedItem*>(ui->connectedPropsTable->currentItem());
	if(!item)
		item = static_cast<NEConnectedItem*>(ui->allPropertyConnectionsTable->currentItem());

	if(!item || !item->propertyConnection())
		return;


    if(item->source() == this->m_pPropSourceNode)
    {
        NENode::NEPropConn* conn = item->propertyConnection();
        conn->expr = ui->propertyExpressionEdit->toPlainText();
    }
    else if(item->source() == this->m_pPropTargetNode)
    {
        NENode::NESrcPropConn* conn = item->sourcePropertyConnection();
        *(conn->expr) = ui->propertyExpressionEdit->toPlainText();
    }

	ui->savePropertyExpressionButton->setDisabled(true);
}




