#include "NEKeyframeInspector.h"
#include "ui_NEKeyframeInspector.h"
#include "Base/NESceneManager.h"
#include "Managers/NEGUIManager.h"
#include "Managers/NEGUIManager_p.h"
#include "Utilities/NEGUIStyle.h"
#include "NENode.h"

NEKeyframeInspector::NEKeyframeInspector(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::NEKeyframeInspector)
{
	ui->setupUi(this);

	// initialize member variables
	m_keyedNode = 0;
	m_selectedKeyframe = -1;

	// connects the button signals with the according select slots
	connect(ui->loadNodeButton, &QPushButton::clicked, this, &NEKeyframeInspector::selectNode);
	connect(ui->removeButton, &QPushButton::clicked, this, &NEKeyframeInspector::removeKeyframe);
	// when an item is selected from the property table, the keyframes for this property are shown
	connect(ui->propertyNameTable, &QTableWidget::itemClicked, this, &NEKeyframeInspector::selectProperty);
	connect(ui->keyframeTable, &QTableWidget::itemClicked, this, &NEKeyframeInspector::selectKeyframe);
	// keyframes can be edited - if this happens, we have to call the corresponding method
	connect(ui->keyframeTable, &QTableWidget::itemChanged, this, &NEKeyframeInspector::changeKeyframe);

	m_ContextMenuProperties = new QMenu("Settings", this);
	m_ContextMenuKeyframes = new QMenu("Settings", this);

	m_ContextMenuProperties->addAction(ui->removeAction);
	m_ContextMenuKeyframes->addAction(ui->removeAction);
	m_ContextMenuKeyframes->addAction(ui->removeAllAction);

	connect(ui->removeAction, &QAction::triggered, this, &NEKeyframeInspector::removeKeyframe);
	connect(ui->removeAllAction, &QAction::triggered, this, &NEKeyframeInspector::removeKeyframeFromAllProperties);
	connect(ui->propertyNameTable, &QTableWidget::itemPressed, this, &NEKeyframeInspector::showPropertyMenu);
	connect(ui->keyframeTable, &QTableWidget::itemPressed, this, &NEKeyframeInspector::showKeyframeMenu);
}

NEKeyframeInspector::~NEKeyframeInspector()
{
	delete ui;
}


void NEKeyframeInspector::showPropertyMenu(QTableWidgetItem *item)
{

	switch(QApplication::mouseButtons())
	{
	case Qt::RightButton:
		if(item)
		{
			m_ContextMenuProperties->exec(QCursor::pos());
		}
		break;
	}
}

void NEKeyframeInspector::showKeyframeMenu(QTableWidgetItem *item)
{
	switch(QApplication::mouseButtons())
	{
	case Qt::RightButton:
		if(item)
		{
			m_ContextMenuKeyframes->exec(QCursor::pos());
		}
		break;
	}
}

void NEKeyframeInspector::changeKeyframe(QTableWidgetItem* item)
{
	if(m_selectedKeyframe == -1)
		return;

	// change the keyframe for all selected properties
	foreach(QTableWidgetItem* selectedProperty, ui->propertyNameTable->selectedItems())
	{
		//// remove old keyframe and store value
		//QVariant tempValue = m_keyedNode->getKeyframeValue(&selectedProperty->text(), m_selectedKeyframe);
		//m_keyedNode->removeKeyframe(m_selectedKeyframe, selectedProperty->text());

		//// now add new keyframe
		//m_keyedNode->addKeyframe(item->text().toInt(), selectedProperty->text(), tempValue);

		m_keyedNode->moveKeyframe(m_selectedKeyframe, (uint)item->text().toUInt(), selectedProperty->text());

	}

	// update tables
	updateKeyframes();

	// update timelinewidget
    NEGUI.P->setTimelineKeyframeMarks(m_keyedNode->getTransformKeyframes());

	// and apply changes to NENode
	m_keyedNode->updateProperties(NESCENE.CurrentFrame(), NESCENE.ElapsedTime());
	NEGUI.updateGLWindows();
}


void NEKeyframeInspector::clearTable(QTableWidget* table)
{
	while(table->rowCount() != 0)
		table->removeRow(0);
}

void NEKeyframeInspector::resetEditor()
{
	clearTable(ui->propertyNameTable);
	clearTable(ui->keyframeTable);

	QStringList label;
	label << tr("(none selected)");
	ui->propertyNameTable->setHorizontalHeaderLabels(label);
	ui->keyframeTable->setHorizontalHeaderLabels(label);
}



void NEKeyframeInspector::selectNode()
{
	QTableWidgetItem* item;
	int row = 0;

	m_selectedKeyframe = -1;

	// remove old table content
	clearTable(ui->propertyNameTable);
	clearTable(ui->keyframeTable);

	// set currently selected node as input node
	m_keyedNode = NESCENE.selectedNode();
	if(!m_keyedNode)
		return;

	// set node name as table headline
	QStringList label;
	label << m_keyedNode->objectName();
	ui->propertyNameTable->setHorizontalHeaderLabels(label);

	QStringList label2;
	label2 << "Keyframes";
	ui->keyframeTable->setHorizontalHeaderLabels(label2);


	// Now, list all keyed properties of the node
	QList<QString> keyedProps = m_keyedNode->getKeyedProperties();

	for(int i=0; i<keyedProps.size(); ++i)
	{
		QString prop = keyedProps[i];
		item = new QTableWidgetItem(prop);
		item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		ui->propertyNameTable->insertRow(row);
		ui->propertyNameTable->setItem(row, 0, item);
		row++;
    }
}

void NEKeyframeInspector::updateNodeName()
{
    if(!m_keyedNode)
        return;

    QStringList label;
    label << m_keyedNode->objectName();

    ui->propertyNameTable->setHorizontalHeaderLabels(label);
}

void NEKeyframeInspector::selectProperty()
{
	QTableWidgetItem* item;

	// remove old table content
	clearTable(ui->keyframeTable);

	m_selectedKeyframe = -1;

	// We compute the intersection of keyframes for selected properties
	QSet<uint> intersect = QSet<uint>::fromList(m_keyedNode->getKeyframes());
	foreach(QTableWidgetItem* oItem, ui->propertyNameTable->selectedItems())
	{
		QString itemTxt = oItem->text();
		intersect.intersect(QSet<uint>::fromList(m_keyedNode->getKeyframesForProperty(&itemTxt)));
	}

	QList<uint> keyframes = intersect.toList();
	qSort(keyframes);
	for(int i=0; i < keyframes.size(); ++i)
	{
		item = new QTableWidgetItem();
		item->setText(QString::number(keyframes[i]));
		//item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		ui->keyframeTable->insertRow(i);
		ui->keyframeTable->setItem(i, 0, item);
	}

	// redraw properties, to make sure they are not italic
	for(int i=0; i < ui->propertyNameTable->rowCount(); ++i)
	{
		item = new QTableWidgetItem();
		item->setText(ui->propertyNameTable->item(i, 0)->text());
		item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		ui->propertyNameTable->setItem(i, 0, item);
	}

}

void NEKeyframeInspector::updateKeyframes()
{
	if(!m_keyedNode)
		return;

	QTableWidgetItem* item;

	// we remember which row was selected
	int selectedRow = ui->propertyNameTable->currentRow();

	// remove old table content
	clearTable(ui->propertyNameTable);
	clearTable(ui->keyframeTable);

	// set currently selected node as input node
	m_keyedNode = NESCENE.selectedNode();

	if(!m_keyedNode)
		return;


	// Now, list all keyed properties of the node
	QList<QString> keyedProps = m_keyedNode->getKeyedProperties();

	for(int i=0; i<keyedProps.size(); ++i)
	{
		QString prop = keyedProps[i];
		item = new QTableWidgetItem(prop);
		item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		ui->propertyNameTable->insertRow(i);
		ui->propertyNameTable->setItem(i, 0, item);
	}

	// we select the row again
	if(selectedRow < ui->propertyNameTable->rowCount() && selectedRow >= 0)
	{
		ui->propertyNameTable->setCurrentCell(selectedRow, 0);
		selectProperty();
	}
}

void NEKeyframeInspector::selectKeyframe()
{
	QTableWidgetItem* item;
	//int row = 0;

	//int currentRow = ui->propertyNameTable->currentRow();

	m_selectedKeyframe = ui->keyframeTable->currentItem()->text().toInt();

	// Now, list all properties of the node
	QTableWidgetItem* currentKey = ui->keyframeTable->currentItem();

	//QList<QString> allProps = m_keyedNode->getKeyedProperties();
	QList<QString> keyedProps = m_keyedNode->getKeyedProperties(currentKey->text().toInt()).keys();

	for(int i=0; i < ui->propertyNameTable->rowCount(); ++i)
	{
		item = ui->propertyNameTable->item(i, 0);

		if(!keyedProps.contains(item->text()))
		{
			item->setFont(*NEGUISTYLE.ItalicFont());
			item->setForeground(*NEGUISTYLE.GrayFontBrush());
		} else
		{
			item->setFont(*NEGUISTYLE.NormalFont());
			item->setForeground(QBrush());
		}
	}
}

void NEKeyframeInspector::removeKeyframe()
{
	if(!m_keyedNode)
		return;

	//if keyframes are selected, we remove the selected keyframes from the property
	if(ui->keyframeTable->selectedItems().size() > 0)
	{
		QTableWidgetItem* propertyItem = ui->propertyNameTable->currentItem();

		for(int i = 0; i < ui->keyframeTable->selectedItems().size(); ++i)
		{
			QTableWidgetItem* keyframeItem = ui->keyframeTable->selectedItems()[i];

			m_keyedNode->removeKeyframe(keyframeItem->text().toInt(), propertyItem->text());
		}


		updateKeyframes();
	} else if (ui->propertyNameTable->selectedItems().size() > 0)
	{
		for(int i = 0; i < ui->propertyNameTable->selectedItems().size(); ++i)
		{
			QTableWidgetItem* propertyItem = ui->propertyNameTable->selectedItems()[i];
			QString propText = propertyItem->text();
			QList<uint> keyframes = m_keyedNode->getKeyframesForProperty(&propText);

			// we remove all keyframes for this property
			foreach(uint index, keyframes)
			{
				m_keyedNode->removeKeyframe(index, propertyItem->text());
			}
		}

		updateKeyframes();
	}

	// update timeline widget, if this node is shown
	if(NESCENE.selectedObject() == m_keyedNode)
        NEGUI.P->setTimelineKeyframeMarks(m_keyedNode->getTransformKeyframes());

}

void NEKeyframeInspector::removeKeyframeFromAllProperties()
{
	if(!m_keyedNode)
		return;

	if(ui->keyframeTable->selectedItems().size() > 0)
	{
		// we remove this keyframe from all properties in ui->propertyNameTable
		for(int row = 0; row < ui->propertyNameTable->rowCount(); ++row)
		{
			QTableWidgetItem* propertyItem = ui->propertyNameTable->item(row, 0);

			for(int i = 0; i < ui->keyframeTable->selectedItems().size(); ++i)
			{
				QTableWidgetItem* keyframeItem = ui->keyframeTable->selectedItems()[i];

				m_keyedNode->removeKeyframe(keyframeItem->text().toInt(), propertyItem->text());
			}
		}
		updateKeyframes();
	}

	// update timeline widget, if this node is shown
	if(NESCENE.selectedObject() == m_keyedNode)
        NEGUI.P->setTimelineKeyframeMarks(m_keyedNode->getTransformKeyframes());

}
