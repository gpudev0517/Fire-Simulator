/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of a Qt Solutions component.
**
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include "qttreepropertybrowser.h"
#include "qtpropertymanager.h"
#include "NENode.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "qtvariantproperty.h"
#include "Manager/NECurveManager.h"

#include "NEObjectController.h"

#include "Dialogs/Curve/NECurveDialog.h"
//#include "Managers/NEGUIManager.h"
#include "Utilities/NEGUIStyle.h"
#include "Dialogs/ColorPreset/NEColorPresetDialog.h"
#include "Dialogs/File/NEFileDialog.h"

#include "Utility/NEColorRamp.h"
#include "Utility/FilePath.h"

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

class QtPropertyEditorView;

class QtTreePropertyBrowserPrivate
{
    QtTreePropertyBrowser* q_ptr;
    Q_DECLARE_PUBLIC(QtTreePropertyBrowser)

public:
    QtTreePropertyBrowserPrivate();
    void init(QWidget* parent);

    void propertyInserted(QtBrowserItem* index, QtBrowserItem* afterIndex);
    void propertyRemoved(QtBrowserItem* index);
    void propertyChanged(QtBrowserItem* index);
    QWidget* createEditor(QtProperty* property, QWidget* parent) const
    {
        return q_ptr->createEditor(property, parent);
    }
    QtProperty* indexToProperty(const QModelIndex& index) const;
    QTreeWidgetItem* indexToItem(const QModelIndex& index) const;
    QtBrowserItem* indexToBrowserItem(const QModelIndex& index) const;
    bool lastColumn(int column) const;
    void disableItem(QTreeWidgetItem* item) const;
    void enableItem(QTreeWidgetItem* item) const;
    bool hasValue(QTreeWidgetItem* item) const;

    void slotCollapsed(const QModelIndex& index);
    void slotExpanded(const QModelIndex& index);

    QColor calculatedBackgroundColor(QtBrowserItem* item) const;

    QtPropertyEditorView* treeWidget() const
    {
        return m_treeWidget;
    }
    bool markPropertiesWithoutValue() const
    {
        return m_markPropertiesWithoutValue;
    }

    QtBrowserItem* currentItem() const;
    void setCurrentItem(QtBrowserItem* browserItem, bool block);
    void editItem(QtBrowserItem* browserItem);

    void slotCurrentBrowserItemChanged(QtBrowserItem* item);
    void slotCurrentTreeItemChanged(QTreeWidgetItem* newItem, QTreeWidgetItem*);
    void slotItemClicked(QTreeWidgetItem* item, int column);

    void showNodePropertyExpression(const NENode* val, const QString P);

    QTreeWidgetItem* editedItem() const;

    QMap<QtBrowserItem*, QTreeWidgetItem*> m_indexToItem;
    QMap<QTreeWidgetItem*, QtBrowserItem*> m_itemToIndex;

    QMap<QString, NEColorRamp*> m_colorPresetListMap;

private:
    void updateItem(QTreeWidgetItem* item);

    QMap<QtBrowserItem*, QColor> m_indexToBackgroundColor;

    QtPropertyEditorView* m_treeWidget;

    bool m_headerVisible;
    QtTreePropertyBrowser::ResizeMode m_resizeMode;
    class QtPropertyEditorDelegate* m_delegate;
    bool m_markPropertiesWithoutValue;
    bool m_browserChangedBlocked;
    QIcon m_expandIcon;

    QFont* m_BoldFont;
    //QFont m_LightBoldFont;
    QBrush* m_LightBrush;
};

// ------------ QtPropertyEditorView
class QtPropertyEditorView : public QTreeWidget
{
    Q_OBJECT
public:
    QtPropertyEditorView(QWidget* parent = 0);

    void setEditorPrivate(QtTreePropertyBrowserPrivate* editorPrivate)
    {
        m_editorPrivate = editorPrivate;
    }

    QTreeWidgetItem* indexToItem(const QModelIndex& index) const
    {
        return itemFromIndex(index);
    }

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void initColorRampList();




private slots:
    void setKey();
    void removeKey();
    void removeAllKeys();
    void setTransformKey();
    void removeTransformKey();
    void showCurveEditor();
    void showExpressionEditor();
    void resetProperty();
    void removeProperty();
    void lockProperty();
    void unlockProperty();
    void hideProperty();
    void SaveColorRampToFile();
    void LoadColorRampFromFile();
    void SaveColorRampToPreset();
    void LoadColorRampFromPreset();
    void loadColorRamp(QAction* action);

private:
    QtTreePropertyBrowserPrivate* m_editorPrivate;
    QTreeWidgetItem* m_lastItem;

    QAction* m_SetKeyAction;
    QAction* m_RemoveKeyAction;
    QAction* m_RemoveAllKeysAction;
    QAction* m_SetTransformKeyAction;
    QAction* m_RemoveTransformKeyAction;
    QAction* m_ShowCurveAction;
    QAction* m_SetExpressionAction;
    QAction* m_ResetPropertyAction;
    QAction* m_RemovePropertyAction;
    QAction* m_LockPropertyAction;
    QAction* m_UnlockPropertyAction;
    QAction* m_HidePropertyAction;
    QAction* m_SaveColorRampToFileAction;
    QAction* m_LoadColorRampFromFileAction;
    QAction* m_SaveColorRampToPresetAction;
//    QAction* m_LoadColorRampFromPresetAction;

    QMenu* m_NENodeContextMenu;
    QMenu* m_NEObjectContextMenu;
    QMenu* m_QObjectContextMenu;

    QMenu* m_NENodeColorRampList;

    QXmlStreamReader xmlReader;             /**< These are required for loading and saving xml files. */
    QXmlStreamWriter xmlWriter;             /**< These are required for loading and saving xml files. */
};

QtPropertyEditorView::QtPropertyEditorView(QWidget* parent) :
    QTreeWidget(parent),
    m_editorPrivate(0), m_lastItem(0)
{
    m_SetKeyAction = new QAction("Set key", this);
    m_RemoveKeyAction = new QAction("Remove key", this);
    m_SetTransformKeyAction = new QAction("Set transform key", this);
    m_RemoveAllKeysAction = new QAction( "Remove All keys", this );
    m_RemoveTransformKeyAction = new QAction("Remove transform key", this);
    m_ShowCurveAction = new QAction("Show curve", this);
    m_SetExpressionAction = new QAction("Edit expression", this);
    m_ResetPropertyAction = new QAction("Reset to default", this);
    m_RemovePropertyAction = new QAction("Remove property", this);
    m_LockPropertyAction = new QAction("Lock property", this);
    m_UnlockPropertyAction = new QAction("Unlock property", this);
    m_HidePropertyAction = new QAction("Hide property", this);
    m_SaveColorRampToFileAction = new QAction("Save ColorRamp to file", this);
    m_LoadColorRampFromFileAction = new QAction("Load ColorRamp from file", this);
    m_SaveColorRampToPresetAction = new QAction("ColorRamp to/from preset", this);
//    m_SaveColorRampToPresetAction = new QAction("Save ColorRamp to preset", this);
//    m_LoadColorRampFromPresetAction = new QAction("Load ColorRamp from preset", this);

    this->setAnimated(true);
    this->setIndentation(10);

    QPalette palette = qApp->palette();
    palette.setColor(QPalette::Base, QColor(35, 35, 35));
    this->setPalette(palette);

    //this->setBackground(0, NEGUISTYLE.RealGrayFontBrush());
    //this->setBackgroundRole()

    // Create different menus for different node types

    m_QObjectContextMenu = new QMenu("Settings", this);
    m_QObjectContextMenu->addAction(m_ResetPropertyAction);

    m_NEObjectContextMenu = new QMenu("Settings", this);
    m_NEObjectContextMenu->addAction(m_ResetPropertyAction);
    m_NEObjectContextMenu->addAction(m_RemovePropertyAction);
    m_NEObjectContextMenu->addSeparator();
    m_NEObjectContextMenu->addAction(m_LockPropertyAction);
    m_NEObjectContextMenu->addAction(m_UnlockPropertyAction);
    m_NEObjectContextMenu->addAction(m_HidePropertyAction);


    m_NENodeContextMenu = new QMenu("Settings", this);
    m_NENodeContextMenu->addAction(m_SetKeyAction);
    m_NENodeContextMenu->addAction(m_RemoveKeyAction);
    m_NENodeContextMenu->addAction(m_RemoveAllKeysAction);
    m_NENodeContextMenu->addSeparator();
    m_NENodeContextMenu->addAction(m_SetTransformKeyAction);
    m_NENodeContextMenu->addAction(m_RemoveTransformKeyAction);
    m_NENodeContextMenu->addAction(m_ShowCurveAction);
    m_NENodeContextMenu->addSeparator();
    m_NENodeContextMenu->addAction(m_SetExpressionAction);
    m_NENodeContextMenu->addSeparator();
    m_NENodeContextMenu->addAction(m_ResetPropertyAction);
    m_NENodeContextMenu->addAction(m_RemovePropertyAction);
    m_NENodeContextMenu->addSeparator();
    m_NENodeContextMenu->addAction(m_LockPropertyAction);
    m_NENodeContextMenu->addAction(m_UnlockPropertyAction);
    m_NENodeContextMenu->addAction(m_HidePropertyAction);
    m_NENodeContextMenu->addSeparator();
    m_NENodeContextMenu->addAction(m_SaveColorRampToFileAction);
    m_NENodeContextMenu->addAction(m_LoadColorRampFromFileAction);
    m_NENodeContextMenu->addAction(m_SaveColorRampToPresetAction);
//    m_NENodeContextMenu->addAction(m_LoadColorRampFromPresetAction);

    m_NENodeColorRampList = new QMenu("Load ColorRamp from ...", this);
    m_NENodeContextMenu->addMenu(m_NENodeColorRampList);

    connect(m_SetKeyAction, &QAction::triggered, this, &QtPropertyEditorView::setKey);
    connect(m_RemoveKeyAction, &QAction::triggered, this, &QtPropertyEditorView::removeKey);
    connect(m_RemoveAllKeysAction, &QAction::triggered, this, &QtPropertyEditorView::removeAllKeys);
    connect(m_SetTransformKeyAction, &QAction::triggered, this, &QtPropertyEditorView::setTransformKey);
    connect(m_RemoveTransformKeyAction, &QAction::triggered, this, &QtPropertyEditorView::removeTransformKey);
    connect(m_ShowCurveAction, &QAction::triggered, this, &QtPropertyEditorView::showCurveEditor);
    connect(m_SetExpressionAction, &QAction::triggered, this, &QtPropertyEditorView::showExpressionEditor);
    connect(m_ResetPropertyAction, &QAction::triggered, this, &QtPropertyEditorView::resetProperty);
    connect(m_RemovePropertyAction, &QAction::triggered, this, &QtPropertyEditorView::removeProperty);
    connect(m_LockPropertyAction, &QAction::triggered, this, &QtPropertyEditorView::lockProperty);
    connect(m_UnlockPropertyAction, &QAction::triggered, this, &QtPropertyEditorView::unlockProperty);
    connect(m_HidePropertyAction, &QAction::triggered, this, &QtPropertyEditorView::hideProperty);
    connect(m_SaveColorRampToFileAction, &QAction::triggered, this, &QtPropertyEditorView::SaveColorRampToFile);
    connect(m_LoadColorRampFromFileAction, &QAction::triggered, this, &QtPropertyEditorView::LoadColorRampFromFile);
    connect(m_SaveColorRampToPresetAction, &QAction::triggered, this, &QtPropertyEditorView::SaveColorRampToPreset);
    connect(m_NENodeColorRampList, SIGNAL(triggered(QAction*)), this, SLOT(loadColorRamp(QAction*)));
//    connect(m_LoadColorRampFromPresetAction, &QAction::triggered, this, &QtPropertyEditorView::LoadColorRampFromPreset);

    connect(header(), &QHeaderView::sectionDoubleClicked, this, &QtPropertyEditorView::resizeColumnToContents);
}

void QtPropertyEditorView::initColorRampList(){
    m_editorPrivate->m_colorPresetListMap.clear();
//    QtProperty* prop = m_editorPrivate->m_itemToIndex[m_lastItem]->property();
    QtVariantProperty* prop = static_cast<QtVariantProperty*>(m_editorPrivate->m_itemToIndex[m_lastItem]->property());
    QObject* obj = prop->qObject();

    if(obj)
    {
        NENode* node = qobject_cast<NENode*>(prop->qObject());
        NEColorRamp nodeColorRamp = prop->value().value<NEColorRamp>();
        nodeColorRamp.loadFromPresetList(&m_editorPrivate->m_colorPresetListMap);
        prop->setValue(nodeColorRamp);
    }

    QMap<QString, NEColorRamp*>::iterator iter;
    QString         itemStr;

    for(int i = m_NENodeColorRampList->actions().count(); i > 0; i--){
        m_NENodeColorRampList->removeAction(m_NENodeColorRampList->actions().at(i-1));
    }

    for(iter = m_editorPrivate->m_colorPresetListMap.begin(); iter != m_editorPrivate->m_colorPresetListMap.end(); iter++)
    {
        itemStr = iter.key();
        QAction *rampAction = new QAction(itemStr, this);
        m_NENodeColorRampList->addAction(rampAction);
    }
}

void QtPropertyEditorView::drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;

    bool isKeyed = false;
    bool hasExpression = false;
    bool isKeyedSubproperty = false;
    bool hasValue = true;

    if(m_editorPrivate)
    {
        QtProperty* property = m_editorPrivate->indexToProperty(index);
        if(property)
        {
            QObject* obj = property->qObject();
            // if this obj can be cast do an NENode
            if(NENode* node = qobject_cast<NENode*>(obj))
            {
                QtVariantProperty* vProp = static_cast<QtVariantProperty*>(property);

                if(vProp->hasValue())
                    hasExpression = node->hasUpdateExpression(property->propertyName());

                if(vProp->hasValue() && (uint)vProp->valueType() == getNEVarIds().eulerfId)
                    isKeyed =  node->isKeyed(property->propertyName());

                else if(vProp->hasValue() && (uint)vProp->valueType() >= QVariant::UserType)
                {
                    foreach(QtProperty* subProp, vProp->subProperties())
                    {
                        if(node->isKeyed(property->propertyName() + subProp->propertyName()))
                        {
                            isKeyed = true;
                            break;
                        }
                    }
                }
                else
                    isKeyed =  node->isKeyed(property->propertyName());
            }
            else if(property->hasValue()) // if this is a subproperty (like X in vec3f)
            {
                QObject* obj = property->parentProperty()->qObject();

                if(NENode* node = qobject_cast<NENode*>(obj))
                {
                    QtVariantProperty* vParentProp = static_cast<QtVariantProperty*>(property->parentProperty());
                    if(vParentProp->hasValue() && (uint)vParentProp->valueType() >= QVariant::UserType)
                    {
                        isKeyedSubproperty = node->isKeyed(property->parentProperty()->propertyName() + property->propertyName());
                    }
                }
            }
            hasValue = property->hasValue();
        }
    }
    if(!hasValue && m_editorPrivate->markPropertiesWithoutValue())
    {
        const QColor c = option.palette.color(QPalette::Dark);

        painter->fillRect(option.rect, c);
        opt.palette.setColor(QPalette::AlternateBase, c);
    }
    else if(isKeyed && hasExpression)
    {
        const QColor c =  QColor(150, 120, 0, 255);
        if(c.isValid())
        {
            painter->fillRect(option.rect, c);
            opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
        }
    }
    else if(hasExpression)
    {
        const QColor c =  QColor(0, 120, 0, 255);
        if(c.isValid())
        {
            painter->fillRect(option.rect, c);
            opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
        }
    }
    else if(isKeyed)
    {
        const QColor c =  QColor(150, 0, 0, 255);
        if(c.isValid())
        {
            painter->fillRect(option.rect, c);
            opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
        }
    }
    else if(isKeyedSubproperty)
    {
        const QColor c =  QColor(150, 0, 0, 255);
        if(c.isValid())
        {
            painter->fillRect(option.rect, c);
            opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
        }
    }
    else
    {
        const QColor c =  m_editorPrivate->calculatedBackgroundColor(m_editorPrivate->indexToBrowserItem(index));
        if(c.isValid())
        {
            painter->fillRect(option.rect, c);
            opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
        }
    }
    QTreeWidget::drawRow(painter, opt, index);
    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
    painter->restore();
}

void QtPropertyEditorView::keyPressEvent(QKeyEvent* event)
{

    switch(event->key())
    {
    case Qt::Key_Space: // Trigger Edit
        event->ignore();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:

        if(!m_editorPrivate->editedItem())
            if(const QTreeWidgetItem* item = currentItem())
                if(item->columnCount() >= 2 && ((item->flags() & (Qt::ItemIsEditable | Qt::ItemIsEnabled)) == (Qt::ItemIsEditable | Qt::ItemIsEnabled)))
                {
                    event->accept();
                    // If the current position is at column 0, move to 1.
                    QModelIndex index = currentIndex();
                    if(index.column() == 0)
                    {
                        index = index.sibling(index.row(), 1);
                        setCurrentIndex(index);
                    }
                    edit(index);
                    return;
                }
        break;
    default:
        break;
    }
    QTreeWidget::keyPressEvent(event);
}

void QtPropertyEditorView::mousePressEvent(QMouseEvent* event)
{
    QTreeWidget::mousePressEvent(event);
    QTreeWidgetItem* item = itemAt(event->pos());
    if(!item)
        return;
    m_lastItem = item;

    m_NENodeContextMenu->hide();

    QtProperty* property = m_editorPrivate->m_itemToIndex[item]->property();
    QObject* obj = property->qObject();
    QMetaProperty metaproperty;
    if(obj)
    {
        const QMetaObject* mobject = obj->metaObject();
        int pindex = -1 ;

        if(mobject)
        {
            pindex = mobject->indexOfProperty(property->propertyName().toUtf8());
            metaproperty = mobject->property(pindex);
        }
    }

    switch(event->buttons())
    {
    case Qt::LeftButton:
        if(item)
        {
            if(metaproperty.isValid() && metaproperty.userType() == getNEVarIds().neBoolId)
            {
                editItem(item, 1);
            }
            else
            {
                if((item != m_editorPrivate->editedItem()) && (event->button() == Qt::LeftButton)
                        && (header()->logicalIndexAt(event->pos().x()) == 1)
                        && ((item->flags() & (Qt::ItemIsEditable | Qt::ItemIsEnabled)) == (Qt::ItemIsEditable | Qt::ItemIsEnabled)))
                {
                    editItem(item, 1);
                }
                else if(!m_editorPrivate->hasValue(item) && m_editorPrivate->markPropertiesWithoutValue() && !rootIsDecorated())
                {
                    if(event->pos().x() + header()->offset() < 20)
                        item->setExpanded(!item->isExpanded());
                }
            }
            showExpressionEditor();
        }
        break;

    case Qt::RightButton:
        if(item)
        {
            if(m_editorPrivate)
            {
                // Show the context menu depending on the property
                QtProperty* prop = m_editorPrivate->m_itemToIndex[item]->property();
                QObject* obj = prop->qObject();
                // Handle static properties
                if(obj)
                {
                    QMetaProperty mprop = obj->metaObject()->property(obj->metaObject()->indexOfProperty(prop->propertyName().toUtf8()));

                    if(mprop.isValid())
                    {
                        // Show the reset option only for the properties with a defined reset function
                        if(mprop.isResettable())
                            m_ResetPropertyAction->setVisible(true);
                        else
                            m_ResetPropertyAction->setVisible(false);
                        // Static properties cannot be removed
                        m_RemovePropertyAction->setVisible(false);
                    }
                    else
                    {
                        // Handle dynamic properties removal
                        m_RemovePropertyAction->setVisible(true);
                    }
                    uint type = mprop.userType();
                    if (type == getNEVarIds().colorRampId)
                    {
                        m_SaveColorRampToFileAction->setVisible(true);
                        m_LoadColorRampFromFileAction->setVisible(true);
                        m_SaveColorRampToPresetAction->setVisible(true);
                        m_NENodeColorRampList->menuAction()->setVisible(true);
                        initColorRampList();
//                        m_LoadColorRampFromPresetAction->setVisible(true);
                    } else
                    {
                        m_SaveColorRampToFileAction->setVisible(false);
                        m_LoadColorRampFromFileAction->setVisible(false);
                        m_SaveColorRampToPresetAction->setVisible(false);
                        m_NENodeColorRampList->menuAction()->setVisible(false);
//                        m_LoadColorRampFromPresetAction->setVisible(false);
                    }
                }
                // Show/Hide lock/unlock actions based on the property state
                if(prop->isEnabled())
                {
                    m_UnlockPropertyAction->setVisible(false);
                    m_LockPropertyAction->setVisible(true);
                }
                else
                {
                    m_UnlockPropertyAction->setVisible(true);
                    m_LockPropertyAction->setVisible(false);
                }

                // Launch a menu based on the object type
                if(qobject_cast<NENode* >(obj))
                    m_NENodeContextMenu->exec(QCursor::pos());
                else if(qobject_cast<NEObject* >(obj))
                    m_NEObjectContextMenu->exec(QCursor::pos());
                else if(qobject_cast<QObject* >(obj))
                    m_QObjectContextMenu->exec(QCursor::pos());
            }
        }
        break;
    }
}

void QtPropertyEditorView::showCurveEditor()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[this->m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        NENode* node = qobject_cast<NENode*>(prop->qObject());
        NECurveDialog* curveDialog = new NECurveDialog(node, prop->propertyName(), nullptr );
        curveDialog->show();
    }
    else
    {
        QtVariantProperty* vProp = static_cast<QtVariantProperty*>(prop);
        QtVariantProperty* vParentProp = static_cast<QtVariantProperty*>(prop->parentProperty());
        if((uint)vParentProp->valueType() == getNEVarIds().vec2fId
                || (uint)vParentProp->valueType() == getNEVarIds().vec3fId)
        {
            NENode* node = qobject_cast<NENode*>(vParentProp->qObject());
            NECurveDialog* curveDialog = new NECurveDialog(node, vParentProp->propertyName() + vProp->propertyName(), nullptr );
            curveDialog->show();
        }
    }
}

void QtPropertyEditorView::showExpressionEditor()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[this->m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        NENode* node = qobject_cast<NENode*>(prop->qObject());
        if(!node)
            return;
        m_editorPrivate->showNodePropertyExpression( node, prop->propertyName() );
        //NEGUI.showNodePropertyExpression(node, prop->propertyName());
        //Log()<<node->objectName()<<" "<< prop->propertyName(); EndLog(NE::kLogWarning);
    }
}

void QtPropertyEditorView::resetProperty()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        QMetaProperty mprop = obj->metaObject()->property(obj->metaObject()->indexOfProperty(prop->propertyName().toUtf8()));
        mprop.reset(obj);
        static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::SaveColorRampToFile()
{
    QtVariantProperty* prop = static_cast<QtVariantProperty*>(m_editorPrivate->m_itemToIndex[m_lastItem]->property());

    QObject* obj = prop->qObject();

    if(obj)
    {
        NENode* node = qobject_cast<NENode*>(prop->qObject());
        if(!node)
            return;
//        std::cout << "QtPropertyEditorView::SaveColorRampToFile " << node->objectName().toStdString() << std::endl;

        QString fileName;
        NEFileDialog dlg(this, NEBASE.SaveDir().path(), tr("Save ColorRamp as"), ColorRampFileFilter(), NEFileDialog::COLORRAMP_SAVE, QFileDialog::AnyFile );
        if (dlg.exec() == QDialog::Accepted)
            fileName = dlg.selectedFile();
        else
            return;

        if (fileName.isEmpty())
            return;

        QString strippedFileName = QFileInfo(fileName).baseName();
        QString path = QFileInfo(fileName).path();
//        std::cout << "QtPropertyEditorView::SaveColorRampToFile filename=" << strippedFileName.toStdString() << " path=" << path.toStdString() << std::endl;

        prop->value().value<NEColorRamp>().saveToFile(strippedFileName, path, &xmlWriter);
//        node->SaveColorRampToFile(strippedFileName, path, &xmlWriter);
    }
}

void QtPropertyEditorView::LoadColorRampFromFile()
{
    QtVariantProperty* prop = static_cast<QtVariantProperty*>(m_editorPrivate->m_itemToIndex[m_lastItem]->property());
    QObject* obj = prop->qObject();

    if(obj)
    {
        NENode* node = qobject_cast<NENode*>(prop->qObject());
        if(!node)
            return;
//        std::cout << "QtPropertyEditorView::LoadColorRampFromFile " << node->objectName().toStdString() << std::endl;

        QString fileName;
        NEFileDialog dlg(this, NEBASE.SaveDir().path(), tr("Load ColorRamp from"), ColorRampFileFilter(), NEFileDialog::COLORRAMP_OPEN, QFileDialog::ExistingFiles );
        if (dlg.exec() == QDialog::Accepted)
            fileName = dlg.selectedFile();
        else
            return;

        if (fileName.isEmpty())
            return;

        QString strippedFileName = QFileInfo(fileName).baseName();
        QString path = QFileInfo(fileName).path();
//        std::cout << "QtPropertyEditorView::LoadColorRampFromFile filename=" << strippedFileName.toStdString() << " path=" << path.toStdString() << std::endl;


        NEColorRamp cr = prop->value().value<NEColorRamp>();
        if(cr.loadFromFile(strippedFileName, path, &xmlReader))
            prop->setValue(cr);

//        if (node->LoadColorRampFromFile(strippedFileName, path, &xmlReader))
//            static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::SaveColorRampToPreset()
{
    QtVariantProperty* prop = static_cast<QtVariantProperty*>(m_editorPrivate->m_itemToIndex[m_lastItem]->property());
    QObject* obj = prop->qObject();

    if(obj)
    {
        NENode* node = qobject_cast<NENode*>(prop->qObject());
        if(!node)
            return;
//        std::cout << "QtPropertyEditorView::SaveColorRampToPreset " << node->objectName().toStdString() << std::endl;

        NEColorPresetDialog* colorPresetDialog = new NEColorPresetDialog(prop, nullptr);
        colorPresetDialog->exec();
//        colorPresetDialog->show();
        if (colorPresetDialog->isLoadColorRampFromPresetToNode())
            static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::loadColorRamp(QAction* action)
{
    NEColorRamp* colorRamp = m_editorPrivate->m_colorPresetListMap.value(action->text());
    QtVariantProperty* prop = static_cast<QtVariantProperty*>(m_editorPrivate->m_itemToIndex[m_lastItem]->property());
    QObject* obj = prop->qObject();
    if(obj)
    {
//        NENode* node = qobject_cast<NENode*>(prop->qObject());
//        NEColorRamp* nodeColorRamp = node->getCurrentForPreset();
        NEColorRamp nodeColorRamp = prop->value().value<NEColorRamp>();
        nodeColorRamp.setColorRamp(colorRamp->getColorRamp());
        prop->setValue(nodeColorRamp);
//        static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::LoadColorRampFromPreset()
{
    QtVariantProperty* prop = static_cast<QtVariantProperty*>(m_editorPrivate->m_itemToIndex[m_lastItem]->property());
    QObject* obj = prop->qObject();

    if(obj)
    {
        NENode* node = qobject_cast<NENode*>(prop->qObject());
        if(!node)
            return;
//        std::cout << "QtPropertyEditorView::LoadColorRampFromPreset " << node->objectName().toStdString() << std::endl;

        QMap<QString, NEColorRamp*> colorMap;
        NEColorRamp nodeColorRamp = prop->value().value<NEColorRamp>();
        if (nodeColorRamp.loadFromPresetList(&colorMap))
            prop->setValue(nodeColorRamp);
    }
}

void QtPropertyEditorView::removeProperty()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        NEObject* node = qobject_cast<NEObject*>(prop->qObject());
        if(!node)
            return;

        node->removeProperty(prop->propertyName());
        static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::lockProperty()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        NEObject* node = qobject_cast<NEObject*>(prop->qObject());
        if(!node)
            return;

        node->lockProperty(prop->propertyName(), true);
        prop->setEnabled(false);
        static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::unlockProperty()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        NEObject* node = qobject_cast<NEObject*>(prop->qObject());
        if(!node)
            return;

        node->lockProperty(prop->propertyName(), false);
        prop->setEnabled(true);
        static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::hideProperty()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        NEObject* node = qobject_cast<NEObject*>(prop->qObject());
        if(!node)
            return;

        node->hideProperty(prop->propertyName(), true);
        static_cast<NEObjectController*>(parent()->parent())->updateProperties();
    }
}

void QtPropertyEditorView::setKey()
{
    //Log() << "Set key"; EndLog(NE::kLogInfo);
    QtProperty* prop = m_editorPrivate->m_itemToIndex[this->m_lastItem]->property();
    QObject* obj = prop->qObject();
    if(obj)
    {
        // Create the property indices std::vector
        QStringList propNames;
        // Push the queried index of the property
        propNames.push_back(prop->propertyName());

        if(NENode* node = qobject_cast<NENode*>(prop->qObject()))
        {
            node->addKeyframe(NESCENE.CurrentFrame(), propNames, NE::kLinear);
            m_editorPrivate->treeWidget()->viewport()->update();
        }
    }
    else	// For properties with sub properties
    {
        QtVariantProperty* vProp = static_cast<QtVariantProperty*>(prop);
        QtVariantProperty* vParentProp = static_cast<QtVariantProperty*>(prop->parentProperty());
        if((uint)vParentProp->valueType() == getNEVarIds().vec2fId
                || (uint)vParentProp->valueType() == getNEVarIds().vec3fId)
        {
            QStringList propNames;
            propNames.push_back(vParentProp->propertyName() + vProp->propertyName());

            if(NENode* node = qobject_cast<NENode*>(vParentProp->qObject()))
            {
                node->addKeyframe(NESCENE.CurrentFrame(), propNames, NE::kLinear);
                m_editorPrivate->treeWidget()->viewport()->update();
            }
        }
    }
}

void QtPropertyEditorView::removeKey()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[this->m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        // Create the property indices std::vector
        QStringList propNames;
        // Push the queried index of the property
        propNames.push_back(prop->propertyName());
        NENode* node = qobject_cast<NENode*>(prop->qObject());

        node->removeKeyframe(NESCENE.CurrentFrame(), propNames);
        m_editorPrivate->treeWidget()->viewport()->update();
    }
    else	// For properties with sub properties
    {
        QtVariantProperty* vProp = static_cast<QtVariantProperty*>(prop);
        QtVariantProperty* vParentProp = static_cast<QtVariantProperty*>(prop->parentProperty());
        if((uint)vParentProp->valueType() == getNEVarIds().vec2fId
                || (uint)vParentProp->valueType() == getNEVarIds().vec3fId)
        {
            QStringList propNames;
            propNames.push_back(vParentProp->propertyName() + vProp->propertyName());

            NENode* node = qobject_cast<NENode*>(vParentProp->qObject());

            node->removeKeyframe(NESCENE.CurrentFrame(), propNames);
            m_editorPrivate->treeWidget()->viewport()->update();
        }
    }
}


void QtPropertyEditorView::removeAllKeys()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[this->m_lastItem]->property();
    QObject* obj = prop->qObject();

    if(obj)
    {
        // Create the property indices std::vector
        QStringList propNames;
        // Push the queried index of the property
        propNames.push_back(prop->propertyName());
        NENode* node = qobject_cast<NENode*>(prop->qObject());

        QList<uint> keyframes = node->getKeyframesForProperty( &propNames[0] );
        foreach(uint key, keyframes )
        {
            node->removeKeyframe(key, propNames[0]);
            NECURVE.removeCurve(this, propNames[0], key);
        }
        node->removeAllKeyframes();
        m_editorPrivate->treeWidget()->viewport()->update();
    }
    else	// For properties with sub properties
    {
        QtVariantProperty* vProp = static_cast<QtVariantProperty*>(prop);
        QtVariantProperty* vParentProp = static_cast<QtVariantProperty*>(prop->parentProperty());
        if((uint)vParentProp->valueType() == getNEVarIds().vec2fId
                || (uint)vParentProp->valueType() == getNEVarIds().vec3fId)
        {
            QStringList propNames;
            propNames.push_back(vParentProp->propertyName() + vProp->propertyName());

            NENode* node = qobject_cast<NENode*>(vParentProp->qObject());

            QList<uint> keyframes = node->getKeyframesForProperty( &propNames[0] );
            foreach(uint key, keyframes )
            {
                node->removeKeyframe(key, propNames[0]);
                NECURVE.removeCurve(this, propNames[0], key);
            }
            node->removeAllKeyframes();

            m_editorPrivate->treeWidget()->viewport()->update();
        }
    }
}

void QtPropertyEditorView::setTransformKey()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[this->m_lastItem]->property();
    QObject* obj = prop->qObject();
    if(obj)
    {
        //Get the meta-object and access the property index
        const QMetaObject* mObj = obj->metaObject();
        QStringList propNames;

        int propIndex = mObj->indexOfProperty("Position");
        if(propIndex > -1)
            propNames.push_back("Position");

        propIndex = mObj->indexOfProperty("Orientation");
        if(propIndex > -1)
            propNames.push_back("Orientation");

        propIndex = mObj->indexOfProperty("Scale");
        if(propIndex > -1)
            propNames.push_back("Scale");

        propIndex = mObj->indexOfProperty("Scale2D");
        if(propIndex > -1)
            propNames.push_back("Scale2D");

        propIndex = mObj->indexOfProperty("Radius");
        if(propIndex > -1)
            propNames.push_back("Radius");

        NESCENE.setKeyFrameForSelectedNode(propNames, NE::kLinear);
        m_editorPrivate->treeWidget()->viewport()->update();
    }
}

void QtPropertyEditorView::removeTransformKey()
{
    QtProperty* prop = m_editorPrivate->m_itemToIndex[this->m_lastItem]->property();
    QObject* obj = prop->qObject();
    if(obj)
    {
        //Get the meta-object and access the property index
        const QMetaObject* mObj = obj->metaObject();
        QStringList propNames;

        int propIndex = mObj->indexOfProperty("Position");

        if(propIndex > -1)
            propNames.push_back("Position");

        propIndex = mObj->indexOfProperty("Orientation");
        if(propIndex > -1)
            propNames.push_back("Orientation");

        propIndex = mObj->indexOfProperty("Scale");
        if(propIndex > -1)
            propNames.push_back("Scale");

        NESCENE.removeKeyForSelectedNode(propNames);
        m_editorPrivate->treeWidget()->viewport()->update();
    }
}


// ------------ QtPropertyEditorDelegate
class QtPropertyEditorDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    QtPropertyEditorDelegate(QObject* parent = 0)
        : QItemDelegate(parent), m_editorPrivate(0), m_editedItem(0), m_editedWidget(0), m_disablePainting(false)
    {}

    void setEditorPrivate(QtTreePropertyBrowserPrivate* editorPrivate)
    {
        m_editorPrivate = editorPrivate;
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

    void setModelData(QWidget*, QAbstractItemModel*,
                      const QModelIndex&) const {}

    void setEditorData(QWidget*, const QModelIndex&) const
    {
    }

    bool eventFilter(QObject* object, QEvent* event);
    void closeEditor(QtProperty* property);

    QTreeWidgetItem* editedItem() const
    {
        return m_editedItem;
    }

protected:

    void drawDecoration(QPainter* painter, const QStyleOptionViewItem& option,
                        const QRect& rect, const QPixmap& pixmap) const;
    void drawDisplay(QPainter* painter, const QStyleOptionViewItem& option,
                     const QRect& rect, const QString& text) const;

private slots:
    void slotEditorDestroyed(QObject* object);

private:
    int indentation(const QModelIndex& index) const;

    typedef QMap<QWidget*, QtProperty*> EditorToPropertyMap;
    mutable EditorToPropertyMap m_editorToProperty;

    typedef QMap<QtProperty*, QWidget*> PropertyToEditorMap;
    mutable PropertyToEditorMap m_propertyToEditor;
    QtTreePropertyBrowserPrivate* m_editorPrivate;
    mutable QTreeWidgetItem* m_editedItem;
    mutable QWidget* m_editedWidget;
    mutable bool m_disablePainting;
};

int QtPropertyEditorDelegate::indentation(const QModelIndex& index) const
{
    if(!m_editorPrivate)
        return 0;

    QTreeWidgetItem* item = m_editorPrivate->indexToItem(index);
    int indent = 0;
    while(item->parent())
    {
        item = item->parent();
        ++indent;
    }
    if(m_editorPrivate->treeWidget()->rootIsDecorated())
        ++indent;
    return indent * m_editorPrivate->treeWidget()->indentation();
}

void QtPropertyEditorDelegate::slotEditorDestroyed(QObject* object)
{
    if(QWidget* w = qobject_cast<QWidget*>(object))
    {
        const EditorToPropertyMap::iterator it = m_editorToProperty.find(w);
        if(it != m_editorToProperty.end())
        {
            m_propertyToEditor.remove(it.value());
            m_editorToProperty.erase(it);
        }
        if(m_editedWidget == w)
        {
            m_editedWidget = 0;
            m_editedItem = 0;
        }
    }
}

void QtPropertyEditorDelegate::closeEditor(QtProperty* property)
{
    if(QWidget* w = m_propertyToEditor.value(property, 0))
        w->deleteLater();
}

QWidget* QtPropertyEditorDelegate::createEditor(QWidget* parent,
                                                const QStyleOptionViewItem&, const QModelIndex& index) const
{
    if(index.column() == 1 && m_editorPrivate)
    {
        QtProperty* property = m_editorPrivate->indexToProperty(index);
        QTreeWidgetItem* item = m_editorPrivate->indexToItem(index);
        if(property && item && (item->flags() & Qt::ItemIsEnabled))
        {
            QWidget* editor = m_editorPrivate->createEditor(property, parent);
            if(editor)
            {
                editor->setAutoFillBackground(true);
                editor->installEventFilter(const_cast<QtPropertyEditorDelegate*>(this));
                connect(editor, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestroyed(QObject*)));
                m_propertyToEditor[property] = editor;
                m_editorToProperty[editor] = property;
                m_editedItem = item;
                m_editedWidget = editor;
            }
            return editor;
        }
    }
    return 0;
}

void QtPropertyEditorDelegate::updateEditorGeometry(QWidget* editor,
                                                    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect.adjusted(0, 0, 0, -1));
}

#include <QStandardItemModel>

void QtPropertyEditorDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    bool hasValue = true;
    if(m_editorPrivate)
    {
        QtProperty* property = m_editorPrivate->indexToProperty(index);
        if(property)
            hasValue = property->hasValue();
    }
    QStyleOptionViewItem opt = option;
    if((m_editorPrivate && index.column() == 0) || !hasValue)
    {
        QtProperty* property = m_editorPrivate->indexToProperty(index);
        if(property && property->isModified())
        {
            opt.font.setBold(true);
            opt.fontMetrics = QFontMetrics(opt.font);
        }
    }
    QColor c;
    if(!hasValue && m_editorPrivate->markPropertiesWithoutValue())
    {
        c = opt.palette.color(QPalette::Dark);
        opt.palette.setColor(QPalette::Text, opt.palette.color(QPalette::BrightText));
    }
    else
    {
        c = m_editorPrivate->calculatedBackgroundColor(m_editorPrivate->indexToBrowserItem(index));
        if(c.isValid() && (opt.features & QStyleOptionViewItemV2::Alternate))
            c = c.lighter(112);
    }
    if(c.isValid())
        painter->fillRect(option.rect, c);
    opt.state &= ~QStyle::State_HasFocus;
    if(index.column() == 1)
    {
        QTreeWidgetItem* item = m_editorPrivate->indexToItem(index);
        if(m_editedItem && m_editedItem == item)
            m_disablePainting = true;
    }

    QtProperty* property = m_editorPrivate->indexToProperty(index);

    QObject* obj = property->qObject();

    QMetaProperty metaproperty;

    if(obj)
    {
        const QMetaObject* mobject = obj->metaObject();
        int pindex = -1 ;

        if(mobject)
        {
            pindex = mobject->indexOfProperty(property->propertyName().toUtf8());
            metaproperty = mobject->property(pindex);
        }
    }

    if(metaproperty.isValid() && metaproperty.userType() == getNEVarIds().neBoolId)
    {
        QTreeWidgetItem* item = m_editorPrivate->indexToItem(index);

        if(index.column() == 1)
        {
            QString text = item->text(0);
            QRect rect = option.rect;
            QRect textRect(rect);
            painter->drawText(textRect, text);

            QRect buttonRect(rect);
            QStyleOptionButton button;
            button.rect = buttonRect;
            button.text = text;
            button.state = QStyle::State_Raised | QStyle::State_Enabled;

            QApplication::style()->drawControl(QStyle::CE_PushButtonBevel, &button, painter);
        }
        else if(index.column() == 0)
        {
            QString text = item->text(0);
            QRect rect = option.rect;
            QRect textRect(rect);
            painter->drawText(textRect, text);
            QItemDelegate::paint(painter, opt, index);
        }
    }
    else
        QItemDelegate::paint(painter, opt, index);

    m_disablePainting = false;

    opt.palette.setCurrentColorGroup(QPalette::Active);
    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    if(!m_editorPrivate || (!m_editorPrivate->lastColumn(index.column()) && hasValue))
    {
        int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
        painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
    }
    painter->restore();
}

void QtPropertyEditorDelegate::drawDecoration(QPainter* painter, const QStyleOptionViewItem& option,
                                              const QRect& rect, const QPixmap& pixmap) const
{
    if(m_disablePainting)
        return;

    QItemDelegate::drawDecoration(painter, option, rect, pixmap);
}

void QtPropertyEditorDelegate::drawDisplay(QPainter* painter, const QStyleOptionViewItem& option,
                                           const QRect& rect, const QString& text) const
{
    if(m_disablePainting)
        return;

    QItemDelegate::drawDisplay(painter, option, rect, text);
}

QSize QtPropertyEditorDelegate::sizeHint(const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const
{
    return QItemDelegate::sizeHint(option, index) + QSize(3, 4);
}

bool QtPropertyEditorDelegate::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::FocusOut)
    {
        QFocusEvent* fe = static_cast<QFocusEvent*>(event);
        if(fe->reason() == Qt::ActiveWindowFocusReason)
            return false;
    }
    return QItemDelegate::eventFilter(object, event);
}

//  -------- QtTreePropertyBrowserPrivate implementation
QtTreePropertyBrowserPrivate::QtTreePropertyBrowserPrivate() :
    m_treeWidget(0),
    m_headerVisible(true),
    m_resizeMode(QtTreePropertyBrowser::Stretch),
    m_delegate(0),
    m_markPropertiesWithoutValue(false),
    m_browserChangedBlocked(false)
{
}

// Draw an icon indicating opened/closing branches
static QIcon drawIndicatorIcon(const QPalette& palette, QStyle* style)
{
    QPixmap pix(14, 14);
    pix.fill(Qt::transparent);
    QStyleOption branchOption;
    QRect r(QPoint(0, 0), pix.size());
    branchOption.rect = QRect(2, 2, 9, 9); // ### hardcoded in qcommonstyle.cpp
    branchOption.palette = palette;
    branchOption.state = QStyle::State_Children;

    QPainter p;
    // Draw closed state
    p.begin(&pix);
    style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &p);
    p.end();
    QIcon rc = pix;
    rc.addPixmap(pix, QIcon::Selected, QIcon::Off);
    // Draw opened state
    branchOption.state |= QStyle::State_Open;
    pix.fill(Qt::transparent);
    p.begin(&pix);
    style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &p);
    p.end();

    rc.addPixmap(pix, QIcon::Normal, QIcon::On);
    rc.addPixmap(pix, QIcon::Selected, QIcon::On);
    return rc;
}

#include "Utilities/NEGUIStyle.h"

void QtTreePropertyBrowserPrivate::init(QWidget* parent)
{
    //For drawing top level items using bold font
    m_BoldFont = const_cast<QFont*>(NEGUISTYLE.BoldFont());

    m_LightBrush = const_cast<QBrush*>(NEGUISTYLE.GrayFontBrush());

    QHBoxLayout* layout = new QHBoxLayout(parent);
    layout->setMargin(0);
    m_treeWidget = new QtPropertyEditorView(parent);
    m_treeWidget->setEditorPrivate(this);
    m_treeWidget->setIconSize(QSize(18, 18));
    layout->addWidget(m_treeWidget);

    m_treeWidget->setColumnCount(2);
    QStringList labels;
    labels.append(QApplication::translate("QtTreePropertyBrowser", "Property", 0));
    labels.append(QApplication::translate("QtTreePropertyBrowser", "Value", 0));
    m_treeWidget->setHeaderLabels(labels);
    m_treeWidget->setAlternatingRowColors(true);
    m_treeWidget->setEditTriggers(QAbstractItemView::EditKeyPressed);
    m_delegate = new QtPropertyEditorDelegate(parent);
    m_delegate->setEditorPrivate(this);
    m_treeWidget->setItemDelegate(m_delegate);
    m_treeWidget->header()->setSectionsMovable(false);
    m_treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);

    m_expandIcon = drawIndicatorIcon(q_ptr->palette(), q_ptr->style());

    QObject::connect(m_treeWidget, SIGNAL(collapsed(const QModelIndex&)), q_ptr, SLOT(slotCollapsed(const QModelIndex&)));
    QObject::connect(m_treeWidget, SIGNAL(expanded(const QModelIndex&)), q_ptr, SLOT(slotExpanded(const QModelIndex&)));
    QObject::connect(m_treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), q_ptr, SLOT(slotCurrentTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(m_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), q_ptr, SLOT(slotItemClicked(QTreeWidgetItem*, int)));
}

QtBrowserItem* QtTreePropertyBrowserPrivate::currentItem() const
{
    if(QTreeWidgetItem* treeItem = m_treeWidget->currentItem())
        return m_itemToIndex.value(treeItem);
    return 0;
}

void QtTreePropertyBrowserPrivate::setCurrentItem(QtBrowserItem* browserItem, bool block)
{
    const bool blocked = block ? m_treeWidget->blockSignals(true) : false;
    if(browserItem == 0)
        m_treeWidget->setCurrentItem(0);
    else
        m_treeWidget->setCurrentItem(m_indexToItem.value(browserItem));
    if(block)
        m_treeWidget->blockSignals(blocked);
}

QtProperty* QtTreePropertyBrowserPrivate::indexToProperty(const QModelIndex& index) const
{
    QTreeWidgetItem* item = m_treeWidget->indexToItem(index);
    QtBrowserItem* idx = m_itemToIndex.value(item);
    if(idx)
        return idx->property();
    return 0;
}

QtBrowserItem* QtTreePropertyBrowserPrivate::indexToBrowserItem(const QModelIndex& index) const
{
    QTreeWidgetItem* item = m_treeWidget->indexToItem(index);
    return m_itemToIndex.value(item);
}

QTreeWidgetItem* QtTreePropertyBrowserPrivate::indexToItem(const QModelIndex& index) const
{
    return m_treeWidget->indexToItem(index);
}

bool QtTreePropertyBrowserPrivate::lastColumn(int column) const
{
    return m_treeWidget->header()->visualIndex(column) == m_treeWidget->columnCount() - 1;
}

void QtTreePropertyBrowserPrivate::disableItem(QTreeWidgetItem* item) const
{
    Qt::ItemFlags flags = item->flags();
    if(flags & Qt::ItemIsEnabled)
    {
        flags &= ~Qt::ItemIsEnabled;
        item->setFlags(flags);
        m_delegate->closeEditor(m_itemToIndex[item]->property());
        const int childCount = item->childCount();
        for(int i = 0; i < childCount; i++)
        {
            QTreeWidgetItem* child = item->child(i);
            disableItem(child);
        }
    }
}

void QtTreePropertyBrowserPrivate::enableItem(QTreeWidgetItem* item) const
{
    Qt::ItemFlags flags = item->flags();
    flags |= Qt::ItemIsEnabled;
    item->setFlags(flags);
    const int childCount = item->childCount();
    for(int i = 0; i < childCount; i++)
    {
        QTreeWidgetItem* child = item->child(i);
        QtProperty* property = m_itemToIndex[child]->property();
        if(property->isEnabled())
        {
            enableItem(child);
        }
    }
}

bool QtTreePropertyBrowserPrivate::hasValue(QTreeWidgetItem* item) const
{
    QtBrowserItem* browserItem = m_itemToIndex.value(item);
    if(browserItem)
        return browserItem->property()->hasValue();
    return false;
}



void QtTreePropertyBrowserPrivate::propertyInserted(QtBrowserItem* index, QtBrowserItem* afterIndex)
{
    QTreeWidgetItem* afterItem = m_indexToItem.value(afterIndex);
    QTreeWidgetItem* parentItem = m_indexToItem.value(index->parent());

    QTreeWidgetItem* newItem = 0;
    if(parentItem)
    {
        newItem = new QTreeWidgetItem(parentItem, afterItem);
    }
    else
    {
        newItem = new QTreeWidgetItem(m_treeWidget, afterItem);
    }
    m_itemToIndex[newItem] = index;
    m_indexToItem[index] = newItem;

    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    //m_treeWidget->setItemExpanded(newItem, true);
    newItem->setExpanded(true);
    newItem->setBackground(0, QColor(35, 35, 35));
    newItem->setBackground(1, QColor(25, 25, 25));

    QTreeWidgetItem* parent = newItem->parent();

    if(parent)
    {
        QTreeWidgetItem* grandParent = parent->parent();

        if(!grandParent)
        {
            newItem->setFont(0, *m_BoldFont);
            newItem->setForeground(0, *m_LightBrush);

        }
    }

    if(!parent)
    {
        newItem->setFont(0, *m_BoldFont);
        newItem->setBackground(0, *NEGUISTYLE.windowBrush());
    }
    else
        newItem->setExpanded(false);


    updateItem(newItem);
}

void QtTreePropertyBrowserPrivate::propertyRemoved(QtBrowserItem* index)
{
    QTreeWidgetItem* item = m_indexToItem.value(index);

    if(m_treeWidget->currentItem() == item)
    {
        m_treeWidget->setCurrentItem(0);
    }

    delete item;

    m_indexToItem.remove(index);
    m_itemToIndex.remove(item);
    m_indexToBackgroundColor.remove(index);
}

void QtTreePropertyBrowserPrivate::propertyChanged(QtBrowserItem* index)
{
    QTreeWidgetItem* item = m_indexToItem.value(index);

    updateItem(item);
}

void QtTreePropertyBrowserPrivate::updateItem(QTreeWidgetItem* item)
{
    QtProperty* property = m_itemToIndex[item]->property();
    QIcon expandIcon;
    if(property->hasValue())
    {
        QString toolTip = property->toolTip();
        if(!toolTip.isEmpty())
            item->setToolTip(0, toolTip);
        item->setToolTip(1, property->valueText());
        item->setIcon(1, property->valueIcon());
        item->setText(1, property->valueText());
    }
    else if(markPropertiesWithoutValue() && !m_treeWidget->rootIsDecorated())
    {
        expandIcon = m_expandIcon;
    }
    item->setIcon(0, expandIcon);
    item->setFirstColumnSpanned(!property->hasValue());
    //item->setToolTip(0, property->propertyName());
    item->setStatusTip(0, property->statusTip());
    item->setWhatsThis(0, property->whatsThis());


    if(property->visiblePropertyName().isEmpty())
        item->setText(0, property->propertyName());
    else
        item->setText(0, property->visiblePropertyName());

    bool wasEnabled = item->flags() & Qt::ItemIsEnabled;
    bool isEnabled = wasEnabled;
    if(property->isEnabled())
    {
        QTreeWidgetItem* parent = item->parent();

        if(!parent || (parent->flags() & Qt::ItemIsEnabled))
            isEnabled = true;
        else
            isEnabled = false;
    }
    else
    {
        isEnabled = false;
    }
    if(wasEnabled != isEnabled)
    {
        if(isEnabled)
            enableItem(item);
        else
            disableItem(item);
    }
    m_treeWidget->viewport()->update();
}

QColor QtTreePropertyBrowserPrivate::calculatedBackgroundColor(QtBrowserItem* item) const
{
    QtBrowserItem* i = item;
    const QMap<QtBrowserItem*, QColor>::const_iterator itEnd = m_indexToBackgroundColor.constEnd();
    while(i)
    {
        QMap<QtBrowserItem*, QColor>::const_iterator it = m_indexToBackgroundColor.constFind(i);
        if(it != itEnd)
            return it.value();
        i = i->parent();
    }
    return QColor();
}

void QtTreePropertyBrowserPrivate::slotCollapsed(const QModelIndex& index)
{
    QTreeWidgetItem* item = indexToItem(index);
    QtBrowserItem* idx = m_itemToIndex.value(item);
    if(item)
        emit q_ptr->collapsed(idx);
}

void QtTreePropertyBrowserPrivate::slotExpanded(const QModelIndex& index)
{
    QTreeWidgetItem* item = indexToItem(index);
    QtBrowserItem* idx = m_itemToIndex.value(item);
    if(item)
        emit q_ptr->expanded(idx);
}

void QtTreePropertyBrowserPrivate::slotCurrentBrowserItemChanged(QtBrowserItem* item)
{
    if(!m_browserChangedBlocked && item != currentItem())
        setCurrentItem(item, true);
}

void QtTreePropertyBrowserPrivate::slotCurrentTreeItemChanged(QTreeWidgetItem* newItem, QTreeWidgetItem*)
{
    QtBrowserItem* browserItem = newItem ? m_itemToIndex.value(newItem) : 0;
    m_browserChangedBlocked = true;
    q_ptr->setCurrentItem(browserItem);
    m_browserChangedBlocked = false;
}

QTreeWidgetItem* QtTreePropertyBrowserPrivate::editedItem() const
{
    return m_delegate->editedItem();
}

void QtTreePropertyBrowserPrivate::editItem(QtBrowserItem* browserItem)
{
    if(QTreeWidgetItem* treeItem = m_indexToItem.value(browserItem, 0))
    {
        m_treeWidget->setCurrentItem(treeItem, 1);
        m_treeWidget->editItem(treeItem, 1);
    }
}

void QtTreePropertyBrowserPrivate::slotItemClicked(QTreeWidgetItem* item, int column)
{
    //	QtProperty *property = m_itemToIndex[item]->property();
    //Log() <<property->propertyName(); EndLog(NE::kLogInfo);
    //m_treeWidget->createAndShowContextMenu()
}

void QtTreePropertyBrowserPrivate::showNodePropertyExpression(const NENode *val, const QString P)
{
    if( q_ptr )
    {
        NEObjectController *objController = qobject_cast< NEObjectController *>( q_ptr->parent() );
        qDebug() << "Before Emit signal to show property exp " ;

        if( objController )
        {
            qDebug() << "Emit signal to show property exp " ;
            emit objController->showNodePropertyExpression( val, P );
        }
    }
}


/*!
    \class QtTreePropertyBrowser

    \brief The QtTreePropertyBrowser class provides QTreeWidget based
    property browser.

    A property browser is a widget that enables the user to edit a
    given set of properties. Each property is represented by a label
    specifying the property's name, and an editing widget (e.g. a line
    edit or a combobox) holding its value. A property can have zero or
    more subproperties.

    QtTreePropertyBrowser provides a tree based view for all nested
    properties, i.e. properties that have subproperties can be in an
    expanded (subproperties are visible) or collapsed (subproperties
    are hidden) state. For example:

    \image qttreepropertybrowser.png

    Use the QtAbstractPropertyBrowser API to add, insert and remove
    properties from an instance of the QtTreePropertyBrowser class.
    The properties themselves are created and managed by
    implementations of the QtAbstractPropertyManager class.

    \sa QtGroupBoxPropertyBrowser, QtAbstractPropertyBrowser
*/

/*!
    \fn void QtTreePropertyBrowser::collapsed(QtBrowserItem *item)

    This signal is emitted when the \a item is collapsed.

    \sa expanded(), setExpanded()
*/

/*!
    \fn void QtTreePropertyBrowser::expanded(QtBrowserItem *item)

    This signal is emitted when the \a item is expanded.

    \sa collapsed(), setExpanded()
*/

/*!
    Creates a property browser with the given \a parent.
*/
QtTreePropertyBrowser::QtTreePropertyBrowser(QWidget* parent)
    : QtAbstractPropertyBrowser(parent)
{
    d_ptr = new QtTreePropertyBrowserPrivate;
    d_ptr->q_ptr = this;

    d_ptr->init(this);
    connect(this, SIGNAL(currentItemChanged(QtBrowserItem*)), this,
            SLOT(slotCurrentBrowserItemChanged(QtBrowserItem*)));
}

/*!
    Destroys this property browser.

    Note that the properties that were inserted into this browser are
    \e not destroyed since they may still be used in other
    browsers. The properties are owned by the manager that created
    them.

    \sa QtProperty, QtAbstractPropertyManager
*/
QtTreePropertyBrowser::~QtTreePropertyBrowser()
{
    delete d_ptr;
}

/*!
    \property QtTreePropertyBrowser::indentation
    \brief indentation of the items in the tree view.
*/
int QtTreePropertyBrowser::indentation() const
{
    return d_ptr->m_treeWidget->indentation();
}

void QtTreePropertyBrowser::setIndentation(int i)
{
    d_ptr->m_treeWidget->setIndentation(i);
}

/*!
  \property QtTreePropertyBrowser::rootIsDecorated
  \brief whether to show controls for expanding and collapsing root items.
*/
bool QtTreePropertyBrowser::rootIsDecorated() const
{
    return d_ptr->m_treeWidget->rootIsDecorated();
}

void QtTreePropertyBrowser::setRootIsDecorated(bool show)
{
    d_ptr->m_treeWidget->setRootIsDecorated(show);
    QMapIterator<QTreeWidgetItem*, QtBrowserItem*> it(d_ptr->m_itemToIndex);
    while(it.hasNext())
    {
        QtProperty* property = it.next().value()->property();
        if(!property->hasValue())
            d_ptr->updateItem(it.key());
    }
}

/*!
  \property QtTreePropertyBrowser::alternatingRowColors
  \brief whether to draw the background using alternating colors.
  By default this property is set to true.
*/
bool QtTreePropertyBrowser::alternatingRowColors() const
{
    return d_ptr->m_treeWidget->alternatingRowColors();
}

void QtTreePropertyBrowser::setAlternatingRowColors(bool enable)
{
    d_ptr->m_treeWidget->setAlternatingRowColors(enable);
    QMapIterator<QTreeWidgetItem*, QtBrowserItem*> it(d_ptr->m_itemToIndex);
}

/*!
  \property QtTreePropertyBrowser::headerVisible
  \brief whether to show the header.
*/
bool QtTreePropertyBrowser::isHeaderVisible() const
{
    return d_ptr->m_headerVisible;
}

void QtTreePropertyBrowser::setHeaderVisible(bool visible)
{
    if(d_ptr->m_headerVisible == visible)
        return;

    d_ptr->m_headerVisible = visible;
    d_ptr->m_treeWidget->header()->setVisible(visible);
}

/*!
  \enum QtTreePropertyBrowser::ResizeMode

  The resize mode specifies the behavior of the header sections.

  \value Interactive The user can resize the sections.
  The sections can also be resized programmatically using setSplitterPosition().

  \value Fixed The user cannot resize the section.
  The section can only be resized programmatically using setSplitterPosition().

  \value Stretch QHeaderView will automatically resize the section to fill the available space.
  The size cannot be changed by the user or programmatically.

  \value ResizeToContents QHeaderView will automatically resize the section to its optimal
  size based on the contents of the entire column.
  The size cannot be changed by the user or programmatically.

  \sa setResizeMode()
*/

/*!
    \property QtTreePropertyBrowser::resizeMode
    \brief the resize mode of setions in the header.
*/

QtTreePropertyBrowser::ResizeMode QtTreePropertyBrowser::resizeMode() const
{
    return d_ptr->m_resizeMode;
}

void QtTreePropertyBrowser::setResizeMode(QtTreePropertyBrowser::ResizeMode mode)
{
    if(d_ptr->m_resizeMode == mode)
        return;

    d_ptr->m_resizeMode = mode;
    QHeaderView::ResizeMode m = QHeaderView::Stretch;
    switch(mode)
    {
    case QtTreePropertyBrowser::Interactive:
        m = QHeaderView::Interactive;
        break;
    case QtTreePropertyBrowser::Fixed:
        m = QHeaderView::Fixed;
        break;
    case QtTreePropertyBrowser::ResizeToContents:
        m = QHeaderView::ResizeToContents;
        break;
    case QtTreePropertyBrowser::Stretch:
    default:
        m = QHeaderView::Stretch;
        break;
    }
    d_ptr->m_treeWidget->header()->setSectionResizeMode(m);
}

/*!
    \property QtTreePropertyBrowser::splitterPosition
    \brief the position of the splitter between the colunms.
*/

int QtTreePropertyBrowser::splitterPosition() const
{
    return d_ptr->m_treeWidget->header()->sectionSize(0);
}

void QtTreePropertyBrowser::setSplitterPosition(int position)
{
    d_ptr->m_treeWidget->header()->resizeSection(0, position);
}

/*!
    Sets the \a item to either collapse or expanded, depending on the value of \a expanded.

    \sa isExpanded(), expanded(), collapsed()
*/

void QtTreePropertyBrowser::setExpanded(QtBrowserItem* item, bool expanded)
{
    QTreeWidgetItem* treeItem = d_ptr->m_indexToItem.value(item);
    if(treeItem)
        treeItem->setExpanded(expanded);
}

/*!
    Returns true if the \a item is expanded; otherwise returns false.

    \sa setExpanded()
*/

bool QtTreePropertyBrowser::isExpanded(QtBrowserItem* item) const
{
    QTreeWidgetItem* treeItem = d_ptr->m_indexToItem.value(item);
    if(treeItem)
        return treeItem->isExpanded();
    return false;
}

/*!
    Returns true if the \a item is visible; otherwise returns false.

    \sa setItemVisible()
    \since 4.5
*/

bool QtTreePropertyBrowser::isItemVisible(QtBrowserItem* item) const
{
    if(const QTreeWidgetItem* treeItem = d_ptr->m_indexToItem.value(item))
        return !treeItem->isHidden();
    return false;
}

/*!
    Sets the \a item to be visible, depending on the value of \a visible.

   \sa isItemVisible()
   \since 4.5
*/

void QtTreePropertyBrowser::setItemVisible(QtBrowserItem* item, bool visible)
{
    if(QTreeWidgetItem* treeItem = d_ptr->m_indexToItem.value(item))
        treeItem->setHidden(!visible);
}

/*!
    Sets the \a item's background color to \a color. Note that while item's background
    is rendered every second row is being drawn with alternate color (which is a bit lighter than items \a color)

    \sa backgroundColor(), calculatedBackgroundColor()
*/

void QtTreePropertyBrowser::setBackgroundColor(QtBrowserItem* item, const QColor& color)
{
    if(!d_ptr->m_indexToItem.contains(item))
        return;
    if(color.isValid())
        d_ptr->m_indexToBackgroundColor[item] = color;
    else
        d_ptr->m_indexToBackgroundColor.remove(item);
    d_ptr->m_treeWidget->viewport()->update();
}

/*!
    Returns the \a item's color. If there is no color set for item it returns invalid color.

    \sa calculatedBackgroundColor(), setBackgroundColor()
*/

QColor QtTreePropertyBrowser::backgroundColor(QtBrowserItem* item) const
{
    return d_ptr->m_indexToBackgroundColor.value(item);
}

/*!
    Returns the \a item's color. If there is no color set for item it returns parent \a item's
    color (if there is no color set for parent it returns grandparent's color and so on). In case
    the color is not set for \a item and it's top level item it returns invalid color.

    \sa backgroundColor(), setBackgroundColor()
*/

QColor QtTreePropertyBrowser::calculatedBackgroundColor(QtBrowserItem* item) const
{
    return d_ptr->calculatedBackgroundColor(item);
}

/*!
    \property QtTreePropertyBrowser::propertiesWithoutValueMarked
    \brief whether to enable or disable marking properties without value.

    When marking is enabled the item's background is rendered in dark color and item's
    foreground is rendered with light color.

    \sa propertiesWithoutValueMarked()
*/
void QtTreePropertyBrowser::setPropertiesWithoutValueMarked(bool mark)
{
    if(d_ptr->m_markPropertiesWithoutValue == mark)
        return;

    d_ptr->m_markPropertiesWithoutValue = mark;
    QMapIterator<QTreeWidgetItem*, QtBrowserItem*> it(d_ptr->m_itemToIndex);
    while(it.hasNext())
    {
        QtProperty* property = it.next().value()->property();
        if(!property->hasValue())
            d_ptr->updateItem(it.key());
    }
    d_ptr->m_treeWidget->viewport()->update();
}

bool QtTreePropertyBrowser::propertiesWithoutValueMarked() const
{
    return d_ptr->m_markPropertiesWithoutValue;
}

/*!
    \reimp
*/
void QtTreePropertyBrowser::itemInserted(QtBrowserItem* item, QtBrowserItem* afterItem)
{
    d_ptr->propertyInserted(item, afterItem);
}

/*!
    \reimp
*/
void QtTreePropertyBrowser::itemRemoved(QtBrowserItem* item)
{
    d_ptr->propertyRemoved(item);
}

/*!
    \reimp
*/
void QtTreePropertyBrowser::itemChanged(QtBrowserItem* item)
{
    d_ptr->propertyChanged(item);
}

/*!
    Sets the current item to \a item and opens the relevant editor for it.
*/
void QtTreePropertyBrowser::editItem(QtBrowserItem* item)
{
    d_ptr->editItem(item);
}

QTreeWidget* QtTreePropertyBrowser::treeWidget()
{
    return d_ptr->treeWidget();
}

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#include "moc_qttreepropertybrowser.cpp"
#include "qttreepropertybrowser.moc"
