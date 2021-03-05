#include "NEColorPresetDialog.h"
#include "Core/NENode.h"
#include "Utility/NEColorRamp.h"
#include "Windows/PropertyManager/qtvariantproperty.h"

class CustomItem : public QGraphicsEllipseItem
{
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        if(event->button() == Qt::LeftButton) {
            if(event->modifiers() == Qt::ShiftModifier) {
                qDebug() << "Custom item left clicked with shift key.";
                // add the item to the selection
                setSelected(true);
            } else if(event->modifiers() == Qt::AltModifier){
                qDebug() << "Custom item left clicked with alt key.";
                // resize the item
                double radius = boundingRect().width() / 2.0;
                _center = QPointF(boundingRect().topLeft().x() + pos().x() + radius, boundingRect().topLeft().y() + pos().y() + radius);
                QPointF pos = event->scenePos();
                qDebug() << boundingRect() << radius << this->pos() << pos << event->pos();
                double dist = sqrt(pow(_center.x()-pos.x(), 2) + pow(_center.y()-pos.y(), 2));
                if(dist / radius > 0.8) {
                    qDebug() << dist << radius << dist / radius;
                    _isResizing = true;
                } else {
                    _isResizing = false;
                }
            } else {
                qDebug() << "Custom item left clicked.";
                QGraphicsItem::mousePressEvent(event);
                event->accept();
            }
        } else if(event->button() == Qt::RightButton) {
            qDebug() << "Custom item right clicked.";
            event->ignore();
        }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        if(QRect(0, 0, 200, 100).contains(event->scenePos().x(), event->scenePos().y())){
            if(event->modifiers() == Qt::AltModifier && _isResizing){
                QPointF pos = event->scenePos();
                double dist = sqrt(pow(_center.x()-pos.x(), 2) + pow(_center.y()-pos.y(), 2));
                setRect(_center.x()-this->pos().x()-dist, _center.y()-this->pos().y()-dist, dist*2, dist*2);
            } else if(event->modifiers() != Qt::AltModifier) {
                qDebug() << "Custom item moved.";
                QGraphicsItem::mouseMoveEvent(event);
                qDebug()<<"moved"<<pos();
            }
        }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        if(event->modifiers() == Qt::AltModifier && _isResizing) {
            _isResizing = false;
        } else if(event->modifiers() != Qt::ShiftModifier) {
            QGraphicsItem::mouseReleaseEvent(event);
        }
    }

    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return UserType+1;
    }
private:
    QPointF _center;
    bool _isResizing;
};

class CustomScene : public QGraphicsScene
{
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        qDebug() << "Custom scene clicked.";
        QGraphicsScene::mousePressEvent(event);
        if(!event->isAccepted()) {
            if(event->button() == Qt::LeftButton) {
                // add a custom item to the scene
//                QPointF pt = event->scenePos();
//                CustomItem * item = new CustomItem();
//                item->setRect(pt.x()-3, pt.y()-3, 6, 6);
//                item->setFlags(QGraphicsItem::ItemIsSelectable|
//                              QGraphicsItem::ItemIsMovable);
//                item->setPen(QPen(Qt::white));
//                addItem(item);
            } else if(event->button() == Qt::RightButton) {
                // check whether there is an item under the cursor
                QGraphicsItem * itemToRemove = NULL;
                foreach(auto item, items(event->scenePos())) {
                    if(item->type() == QGraphicsItem::UserType+1) {
                        itemToRemove = item;
                        break;
                    }
                }
                if(itemToRemove) {
                    // remove the item from the graphicsScene
                    removeItem(itemToRemove);
                }
            }
        }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        qDebug() << "Custom scene moved.";
        QGraphicsScene::mouseMoveEvent(event);
    }
    void keyPressEvent(QKeyEvent * event) {
        if(event->key() == Qt::Key_Backspace) {
            // remove all selected items
            qDebug() << "selected items" << selectedItems().size();
            while(!selectedItems().isEmpty()) {
                removeItem(selectedItems().front());
            }
        } else {
            QGraphicsScene::keyPressEvent(event);
        }
    }
};

NEColorPresetDialog::NEColorPresetDialog(QtVariantProperty* prop, QWidget *parent) : QDialog(parent)
{
    m_prop = prop;
    m_colorPresetListMap.clear();

    m_contentsWidget = new QListWidget;
    m_contentsWidget->setViewMode(QListView::ListMode); //IconMode);
    m_contentsWidget->setIconSize(QSize(_LIST_ICON_WIDTH_, _LIST_ICON_HEIGHT_));
    m_contentsWidget->setMovement(QListView::Static); //Free);
    m_contentsWidget->setMaximumWidth(_LIST_ICON_WIDTH_ * 5);
    m_contentsWidget->setMinimumWidth(_LIST_ICON_WIDTH_ * 3);
    m_contentsWidget->setSpacing(5);
    m_contentsWidget->setWordWrap(true);
    connect(m_contentsWidget, &QListWidget::itemChanged, this, &NEColorPresetDialog::itemChanged);

    m_currentWidget = new QListWidget;
    m_currentWidget->setViewMode(QListView::IconMode); //ListMode);
    m_currentWidget->setIconSize(QSize(_LIST1_ICON_WIDTH_, _LIST1_ICON_HEIGHT_));
    m_currentWidget->setMovement(QListView::Static);
    m_currentWidget->setMinimumSize(_LIST1_ICON_WIDTH_ + 10, _LIST1_ICON_HEIGHT_ * 2);
    m_currentWidget->setMaximumSize(_LIST1_ICON_WIDTH_ + 10, _LIST1_ICON_HEIGHT_ * 2.5);
    m_currentWidget->setWordWrap(true);

    m_graphicView = new QGraphicsView;
    m_graphicScene = new CustomScene;
    m_graphicView->setScene(m_graphicScene);

    initPresetListWidget();
    createValueTable();
    setValueToTable(m_currentNodeColorRamp);
    setValueToGraphicView(m_currentNodeColorRamp);

    QPushButton *addButton = new QPushButton(tr("Add to PresetList"));
    QPushButton *deleteButton = new QPushButton(tr("Delete from PresetList"));
    QPushButton *loadButton = new QPushButton(tr("Load from PresetList"));
    QPushButton *applyButton = new QPushButton(tr("Apply"));
    QPushButton *closeButton = new QPushButton(tr("Close"));

    connect(closeButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(addButton, &QAbstractButton::clicked, this, &NEColorPresetDialog::addPreset);
    connect(deleteButton, &QAbstractButton::clicked, this, &NEColorPresetDialog::deletePreset);
    connect(loadButton, &QAbstractButton::clicked, this, &NEColorPresetDialog::loadPreset);
    connect(applyButton, &QAbstractButton::clicked, this, &NEColorPresetDialog::applyPreset);

    connect(m_graphicScene, &QGraphicsScene::changed, this, &NEColorPresetDialog::graphicsViewChanged);

    QVBoxLayout *horizontalLayout = new QVBoxLayout;
    horizontalLayout->addWidget(m_contentsWidget);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(new QLabel("Current ColorRamp:"));
    buttonsLayout->addWidget(m_graphicView);
    buttonsLayout->addWidget(m_currentWidget);
    buttonsLayout->addSpacing(5);
    buttonsLayout->addWidget(m_valueTable);
    buttonsLayout->addStretch(1);
    buttonsLayout->addSpacing(15);
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addSpacing(15);
    buttonsLayout->addWidget(deleteButton);
    buttonsLayout->addSpacing(15);
    buttonsLayout->addWidget(loadButton);
    buttonsLayout->addSpacing(20);
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addSpacing(15);
    buttonsLayout->addWidget(closeButton);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(horizontalLayout);
//    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Color Preset Dialog"));
    m_loadColorRampFromPreset = false;
}

void NEColorPresetDialog::initPresetListWidget()
{
    createItem(m_prop, m_currentWidget, Qt::AlignLeft, _LIST1_ICON_WIDTH_, _LIST1_ICON_HEIGHT_);
    m_currentNodeColorRamp = new NEColorRamp();
    NEColorRamp cr = m_prop->value().value<NEColorRamp>();
    m_currentNodeColorRamp->setColorRamp(cr.getColorRamp());

    cr.loadFromPresetList(&m_colorPresetListMap);
    m_prop->setValue(cr);

    QMap<QString, NEColorRamp*>::iterator iter;
    QString         itemStr;
    NEColorRamp*    colorRamp;
    for(iter = m_colorPresetListMap.begin(); iter != m_colorPresetListMap.end(); iter++)
    {
        itemStr = iter.key();
        colorRamp = *iter;
        QList<QListWidgetItem *> oldItems = m_contentsWidget->findItems(itemStr, Qt::MatchExactly);
        if (oldItems.size() <= 0)
        {
            QListWidgetItem *colorPresetItem = new QListWidgetItem();
            colorPresetItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            colorPresetItem->setIcon(crateColorRampPresetImage(colorRamp, _LIST_ICON_WIDTH_, _LIST_ICON_HEIGHT_));
            colorPresetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            colorPresetItem->setText(itemStr);
            m_contentsWidget->addItem(colorPresetItem);
        }
    }
}

void NEColorPresetDialog::createItem(QtVariantProperty* prop, QListWidget* listWidget, int alignment, int imageWidth, int imageHeight, QString itemStr)
{
    QListWidgetItem *colorPresetItem = new QListWidgetItem();
    colorPresetItem->setTextAlignment(alignment);
    if (prop)
    {
        NEColorRamp colorRamp = m_prop->value().value<NEColorRamp>();
        colorPresetItem->setIcon(crateColorRampPresetImage(&colorRamp, imageWidth, imageHeight));
        colorPresetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    } else
    {
        colorPresetItem->setIcon(crateColorRampPresetImage(Q_NULLPTR, _LIST_ICON_WIDTH_, _LIST_ICON_HEIGHT_));
        colorPresetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    if (itemStr.isNull() || itemStr.isEmpty())
    {
        if (prop)
            colorPresetItem->setText(QString("Default %1").arg(prop->qObject()->objectName()));
        else
            colorPresetItem->setText(tr("colorPreset_1"));
    } else
        colorPresetItem->setText(itemStr);
    listWidget->addItem(colorPresetItem);
}

void NEColorPresetDialog::addPreset()
{
    QListWidgetItem *colorPresetItem = m_currentWidget->item(0);
    QString         itemStr = colorPresetItem->text();
    itemStr.replace(";", "");
    if (itemStr.isEmpty()) return;
    QList<QListWidgetItem *> oldItems = m_contentsWidget->findItems(itemStr, Qt::MatchExactly);
    if (oldItems.size() <= 0)
    {
        createItem(m_prop, m_contentsWidget, Qt::AlignLeft /*AlignHCenter*/ | Qt::AlignVCenter, _LIST_ICON_WIDTH_, _LIST_ICON_HEIGHT_, itemStr);
        NEColorRamp* cr = new NEColorRamp();
        *cr = m_prop->value().value<NEColorRamp>();
        m_colorPresetListMap.insert(itemStr, cr);
    }
}

void NEColorPresetDialog::deletePreset()
{
    QList<QListWidgetItem *> focusItems = m_contentsWidget->selectedItems();
    if (focusItems.size() > 0)
    {
        for (int i = 0; i < focusItems.size(); i++)
        {
            QMap<QString, NEColorRamp*>::iterator iter;
            QString         itemStr;
            for(iter = m_colorPresetListMap.begin(); iter != m_colorPresetListMap.end(); iter++)
            {
                itemStr = iter.key();
                if (itemStr == focusItems[i]->text())
                {
                    m_colorPresetListMap.erase(iter);
                    break;
                }
            }
            int rowNumber = m_contentsWidget->row(focusItems[i]);
//            std::cout << "NEColorPresetDialog::deletePreset row:" << rowNumber << std::endl;
            delete m_contentsWidget->takeItem(rowNumber);
        }
    }
}

void NEColorPresetDialog::loadPreset()
{
    QListWidgetItem *colorPresetItem = m_contentsWidget->currentItem();
    if (!colorPresetItem) return;
    QString         itemStr = colorPresetItem->text();
    if (itemStr.isEmpty()) return;

    if (m_colorPresetListMap.contains(itemStr))
    {
        NEColorRamp* mapColorRamp = m_colorPresetListMap[itemStr];
        NEColorRamp nodeColorRamp = m_prop->value().value<NEColorRamp>();
        nodeColorRamp.setColorRamp(mapColorRamp->getColorRamp());
        m_prop->setValue(nodeColorRamp);
        m_loadColorRampFromPreset = true;
        updateCurColorRampAndValueTableWithoutName();
    }
}

void NEColorPresetDialog::applyPreset()
{
    if (!m_prop) return;
    QMap<QString, NEColorRamp*>::iterator iter;
    QString         itemStr;
    NEColorRamp*    colorRamp;
    for(iter = m_colorPresetListMap.begin(); iter != m_colorPresetListMap.end(); iter++)
    {
        itemStr = iter.key();
        colorRamp = *iter;
//        std::cout << "NEColorPresetDialog::applyPreset " << itemStr.toStdString() << std::endl;
    }

    NEColorRamp nodeColorRamp = m_prop->value().value<NEColorRamp>();
    nodeColorRamp.saveToPresetList(&m_colorPresetListMap);


    getValueToTable(m_currentNodeColorRamp);
    nodeColorRamp.setColorRamp(m_currentNodeColorRamp->getColorRamp());
    m_prop->setValue(nodeColorRamp);
    m_loadColorRampFromPreset = true;
    updateCurColorRampAndValueTableWithoutName();
}

void NEColorPresetDialog::updateCurColorRampAndValueTableWithoutName()
{
    QListWidgetItem *colorRampItem = m_currentWidget->item(0);
    if (!colorRampItem) return;
    QString         itemStr = colorRampItem->text();
    m_currentWidget->clear();
    createItem(m_prop, m_currentWidget, Qt::AlignLeft, _LIST1_ICON_WIDTH_, _LIST1_ICON_HEIGHT_, itemStr);
    m_currentNodeColorRamp->setColorRamp(m_prop->value().value<NEColorRamp>().getColorRamp());
    setValueToTable(m_currentNodeColorRamp);
}

void NEColorPresetDialog::itemChanged(QListWidgetItem *item)
{
    if (!item) return;
    QList<QListWidgetItem *> oldItems;
    QMap<QString, NEColorRamp*>::iterator iter;
    QString         oldItemStr;
    NEColorRamp*    colorRamp;
//    std::cout << "NEColorPresetDialog::currentTextChanged current: " << item->text().toStdString() << std::endl;
    for(iter = m_colorPresetListMap.begin(); iter != m_colorPresetListMap.end(); iter++)
    {
        oldItemStr = iter.key();
        colorRamp = *iter;
        oldItems = m_contentsWidget->findItems(oldItemStr, Qt::MatchExactly);
        if (oldItems.size() <= 0)
        {
            if (item->text().isNull() || item->text().isEmpty())
                item->setText(oldItemStr);
            else
            {
                if (m_colorPresetListMap.contains(item->text()) || item->text().contains(";"))
                    item->setText(oldItemStr);
                else
                {
                    NEColorRamp *newColorRamp = new NEColorRamp();
                    newColorRamp->setColorRamp(colorRamp->getColorRamp());
                    m_colorPresetListMap.erase(iter);
                    m_colorPresetListMap.insert(item->text(), newColorRamp);
                }
            }
//            std::cout << "NEColorPresetDialog::currentTextChanged old: " << oldItemStr.toStdString() << std::endl;
            break;
        }
    }
}

QIcon NEColorPresetDialog::crateColorRampPresetImage(NEColorRamp* colorRamp, int imageWidth, int imageHeight)
{
    QPixmap pixmap = QPixmap(imageWidth, imageHeight);

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);

    QLinearGradient grad;
    QRect crec(0, 0, imageWidth, imageHeight);
    grad = QLinearGradient(0, 0, imageWidth-1, 0);

    if (colorRamp)
    {
        QVector<QPair<qreal, QColor>>& colorList = colorRamp->getColorRampRef();
        int colorSize = colorList.size();
//        std::cout << "NEColorPresetDialog::crateColorRampPresetImage colorSize=" << colorSize << std::endl;
        QColor  oldColor, curColor;
        for (int i = 0; i < colorSize; i++)
        {
//            std::cout << "NEColorPresetDialog::crateColorRampPresetImage 1: i=" << i << ", r=" << colorList[i].second.red() << ", g=" << colorList[i].second.green() << ", b=" << colorList[i].second.blue() << std::endl;
            if (colorList[i].first < 0) continue;
            if (i == 0 || i == (colorSize - 1))
            {
                curColor = colorList[i].second;
                curColor.setAlpha(255);
                grad.setColorAt(colorList[i].first, curColor);
            } else
            {
//                if (fabs(colorList[i].first - colorList[i-1].first) < 0.05) continue;
                curColor = colorList[i].second;
                curColor.setAlpha(255);
                if ((oldColor.red() == curColor.red()) &&
                    (oldColor.green() == curColor.green()) &&
                    (oldColor.blue() == curColor.blue()))
                    continue;
                grad.setColorAt(colorList[i].first, curColor);
            }
            oldColor = curColor;
        }
    } else
    {
        grad.setColorAt(0, QColor(Qt::blue));
        grad.setColorAt(0.5, QColor(Qt::green));
        grad.setColorAt(1, QColor(Qt::red));
    }

    painter.fillRect(crec, grad);
    painter.drawRect(crec);
    painter.end();

    return QIcon(pixmap);
}

void NEColorPresetDialog::createValueTable()
{
    m_valueTable = new QTableWidget(0, 6);
//    m_valueTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr(" ") << tr("Value") << tr("R") << tr("G")  << tr("B")  << tr("A");
    m_valueTable->setHorizontalHeaderLabels(labels);
    m_valueTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_valueTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_valueTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_valueTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_valueTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_valueTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_valueTable->verticalHeader()->hide();
    m_valueTable->setShowGrid(true);
    m_valueTable->setMinimumSize(_LIST1_ICON_WIDTH_, _LIST1_ICON_HEIGHT_ * 2);

//    connect(m_valueTable, &QTableWidget::cellActivated,
//            this, &NEColorPresetDialog::activeOfItem);
}

void NEColorPresetDialog::setValueToTable(NEColorRamp* colorRamp)
{
    QVector<QPair<qreal, QColor>>& colorList = colorRamp->getColorRampRef();
    int     colorSize = colorList.size();
    QColor  curColor;
    QTableWidgetItem *valueItem;
    m_valueTable->setRowCount(0);
    for (int i = 0; i < colorSize; i++)
    {
//        std::cout << "NEColorPresetDialog::setValueToTable i=" << i << ", r=" << colorList[i].second.red() << ", g=" << colorList[i].second.green() << ", b=" << colorList[i].second.blue() << std::endl;
//        if (colorList[i].first < 0) continue;

        int row = m_valueTable->rowCount();
        m_valueTable->insertRow(row);

        valueItem = new QTableWidgetItem(tr("%1").arg(i));
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_valueTable->setItem(row, 0, valueItem);
        valueItem = new QTableWidgetItem(tr("%1").arg(colorList[i].first));
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_valueTable->setItem(row, 1, valueItem);

        curColor = colorList[i].second;
        valueItem = new QTableWidgetItem(tr("%1").arg(curColor.red()));
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_valueTable->setItem(row, 2, valueItem);
        valueItem = new QTableWidgetItem(tr("%1").arg(curColor.green()));
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_valueTable->setItem(row, 3, valueItem);
        valueItem = new QTableWidgetItem(tr("%1").arg(curColor.blue()));
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_valueTable->setItem(row, 4, valueItem);
        valueItem = new QTableWidgetItem(tr("%1").arg(curColor.alpha()));
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_valueTable->setItem(row, 5, valueItem);
    }
}

void NEColorPresetDialog::setValueToGraphicView(NEColorRamp* colorRamp)
{
    QVector<QPair<qreal, QColor>>& colorList = colorRamp->getColorRampRef();
    int     colorSize = colorList.size();
    QColor  curColor;
    QPolygon poly;
    poly << QPoint(0, 100);
    for (int i = 0; i < colorSize; i++)
    {
        curColor = colorList[i].second;
        poly << QPoint(colorList[i].first * 200, 100 - double(curColor.alpha()) / 255 * 100);
    }
    poly << QPoint(200, 100);


    QGraphicsPolygonItem* polygonItem = new QGraphicsPolygonItem(poly);
    QLinearGradient lgrad(0, 50, 200, 50);

    for (int i = 0; i < colorSize; i++)
    {
        curColor = colorList[i].second;
        lgrad.setColorAt(colorList[i].first,curColor);
    }
    polygonItem->setBrush(lgrad);
    m_graphicScene->addItem(polygonItem);

    for (int i = 0; i < colorSize; i++)
    {
        curColor = colorList[i].second;
        CustomItem *item = new CustomItem;
        item->setPos(colorList[i].first * 200 - 3, (100 - double(curColor.alpha()) / 255 * 100) - 3);
        item->setRect(0, 0, 6, 6);
        item->setFlags(QGraphicsItem::ItemIsSelectable|
                        QGraphicsItem::ItemIsMovable);
        item->setPen(QPen(Qt::white));
        m_graphicScene->addItem(item);
    }
}

void NEColorPresetDialog::graphicsViewChanged(QList<QRectF> region){
    if(region.size() > 1){
        QList<QGraphicsItem*> itemList = m_graphicScene->items();
        QVector<QPair<qreal, QColor>>& colorList = m_currentNodeColorRamp->getColorRampRef();
        colorList.clear();
        int     colorSize = m_valueTable->rowCount();
        QColor  color;
        QTableWidgetItem *valueItem;
        for (int i = 0; i < colorSize; i++)
        {
            qreal posValue = (itemList.at(colorSize - i - 1)->scenePos().x() + 3) / 200.0;
            valueItem = m_valueTable->item(i, 2);
            color.setRed(valueItem->text().toInt());
            valueItem = m_valueTable->item(i, 3);
            color.setGreen(valueItem->text().toInt());
            valueItem = m_valueTable->item(i, 4);
            color.setBlue(valueItem->text().toInt());
            valueItem = m_valueTable->item(i, 5);
            color.setAlpha( (100.0 - itemList.at(colorSize - i - 1)->scenePos().y() - 3) / 100.0 * 255);
            colorList.push_back(QPair<qreal, QColor>(posValue, color));
       }

        QPolygon poly;
        QColor  curColor;
        poly << QPoint(0, 100);
        for (int i = 0; i < colorSize; i++)
        {
            curColor = colorList[i].second;
            poly << QPoint(itemList.at(colorSize - i - 1)->scenePos().x() + 3, itemList.at(colorSize - i - 1)->scenePos().y());
        }
        poly << QPoint(200, 100);


        QGraphicsPolygonItem* polygonItem = (QGraphicsPolygonItem*)itemList.at(itemList.size()-1);
        polygonItem->setPolygon(poly);
        QLinearGradient lgrad(0, 50, 200, 50);

        for (int i = 0; i < colorSize; i++)
        {
            curColor = colorList[i].second;
            lgrad.setColorAt(colorList[i].first,curColor);
        }
        polygonItem->setBrush(lgrad);
        NEColorRamp nodeColorRamp = m_prop->value().value<NEColorRamp>();
        nodeColorRamp.setColorRamp(m_currentNodeColorRamp->getColorRamp());
        m_prop->setValue(nodeColorRamp);
        m_loadColorRampFromPreset = true;
        updateCurColorRampAndValueTableWithoutName();
    }
}

void NEColorPresetDialog::getValueToTable(NEColorRamp* colorRamp)
{
    QVector<QPair<qreal, QColor>>& colorList = colorRamp->getColorRampRef();
    colorList.clear();
    int     colorSize = m_valueTable->rowCount();
    QColor  color;
    QTableWidgetItem *valueItem;
    for (int i = 0; i < colorSize; i++)
    {
        valueItem = m_valueTable->item(i, 1);
        qreal posValue = valueItem->text().toDouble();
        valueItem = m_valueTable->item(i, 2);
        color.setRed(valueItem->text().toInt());
        valueItem = m_valueTable->item(i, 3);
        color.setGreen(valueItem->text().toInt());
        valueItem = m_valueTable->item(i, 4);
        color.setBlue(valueItem->text().toInt());
        valueItem = m_valueTable->item(i, 5);
        color.setAlpha(valueItem->text().toInt());
        colorList.push_back(QPair<qreal, QColor>(posValue, color));
    }
}
