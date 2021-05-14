#pragma once

namespace Ui {
class NEExpressionInspector;
}

class NENode;
class NEObject;
class NEExpressionInspectorNodeItem;
class NEExpressionInspectorPropertyItem;


class NEExpressionInspector : public QMainWindow
{
    Q_OBJECT

public:
    explicit NEExpressionInspector(QWidget *parent = 0);
    ~NEExpressionInspector();

    void update();
    void recursiveUpdate(QObject *root);

private:
    void addNode(NENode* node);
    void showNodeExpressions(NENode* node);

private slots:
    //! Called when a node is clicked
    void selectNode();
    //! Called when a property with an expression is clicked
    void selectProperty();
    //! Called when a property expression is deleted
    void deletePropertyExpression();
    //! Called when the all expressions of a node are to be deleted
    void deleteAllPropertyExpressionsOfNode();

private:
    Ui::NEExpressionInspector *ui;

    NEExpressionInspectorNodeItem* m_SelectedNodeItem;
    NEExpressionInspectorPropertyItem* m_SelectedPropertyItem;

    int m_SelectedNodePos;

    QAction* m_ExpressionDeleteAction;
    QAction* m_DeleteAllExpressionsAction;
};


