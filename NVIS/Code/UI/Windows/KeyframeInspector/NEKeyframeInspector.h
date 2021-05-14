#pragma once

namespace Ui {
class NEKeyframeInspector;
}

class QTableWidget;
class QTableWidgetItem;
class NENode;

class NEKeyframeInspector : public QMainWindow
{
    Q_OBJECT

public:
    explicit NEKeyframeInspector(QWidget *parent = 0);
    ~NEKeyframeInspector();
    //! Resets the editor
    void resetEditor();
    NENode* activeNode()
    {
        return m_keyedNode;
    }

public slots:
    //!Clears the table widget
    void clearTable(QTableWidget* table);
    //! Determines the currently selected node
    void selectNode();
    // This is called to update the node name when an object is renamed
    void updateNodeName();
    //! Shows the keyframes of the selected property
    void selectProperty();
    //! Shows the properties in the selected keyframe
    void selectKeyframe();
    //! Removes the marked Keyframes
    void removeKeyframe();
    //! Removes the marked Keyframe from all properties
    void removeKeyframeFromAllProperties();
    //! Updates the tables
    void updateKeyframes();
    //! shows the context menu for the property table
    void showPropertyMenu(QTableWidgetItem* item);
    //! shows the context menu for the keyframe table
    void showKeyframeMenu(QTableWidgetItem* item);
    //! changes the keyframe for the given item
    void changeKeyframe(QTableWidgetItem* item);

private:
    //! The selected node
    NENode* m_keyedNode;
    //! The selected keyframe
    int m_selectedKeyframe;
    //! The ui class
    Ui::NEKeyframeInspector* ui;

    //! Context menu for the properties
    QMenu* m_ContextMenuProperties;
    //! Context menu for the keyframes
    QMenu* m_ContextMenuKeyframes;
};
