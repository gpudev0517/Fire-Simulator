#pragma once

class NENode;
class PythonHighlighter;

namespace Ui {
class NEConnectionInspector;
}

class NEConnectableItem;


class NEConnectionInspector : public QMainWindow
{
	Q_OBJECT

public:
	explicit NEConnectionInspector(QWidget *parent = 0);
	~NEConnectionInspector();

	//! Initializes the connection editor
	void init();
	//! Resets the connection editor
	void resetEditor();
	//! Called when an object is deleted
	void deselectObject(QObject* object);
	//! Updates the all node connections table
	void updateAllNodeConnectionsTable();
	//! Updates the node connections table
	void updateAllPropertyConnectionsTable();
	//! Updates the property callbacks table
	void updateAllPropertyCallbacksTable();
	//! Updates the all internal connections table
	void updateAllInternalConnectionsTable();
    //! Updates all node editors when a node is renamed
    void updateNodeRenameToEditors(NENode* node);

public slots:
	// Node connection dialog related functions
	//! Establishes a node connection between the selected nodes
	void connectNodes();
	//! Removes a formerly established node connection
	void removeNodeConnection();
	//! Loads the output types of the selected source node
	void loadSourceOutputTypes(NENode* val = 0);

    //! Custom sorting the labels based on To/From Subsections
    void customSortByColumn(int column );

	//! Loads the input types of the selected target node
	void loadTargetInputTypes(NENode* val = 0);
    //! Loads the input types of the selected target node
    void loadTargetInputTypesForAvailableTarget();
    //! Loads the selected input types when a node is loaded
    void reloadTargetInputTypes();
	//! Show the source connection of the selected node
	void listTargetNodeConnections(QTreeWidgetItem*item);
	//! Show the target connection of the selected node
	void listSourceNodeConnections(QTreeWidgetItem* item);

	// Property connection dialog related functions
	//! Establishes a connection between two formerly selected node
	void connectProperties();
	//! Removes a formerly established property connection
	void removePropertyConnection();
	//! Loads the properties of the currently selected target node
	void loadSourceNodeProperties(NENode* val = 0);
	//! Loads the properties of the currently selected target node
	void loadTargetNodeProperties(NENode* val = 0);
    void loadTargetNodePropertiesForAvailableTarget();
    void reloadTargetProperties();

	//! Shows the target connections of the selected node
	void listTargetPropertyConnections(QTreeWidgetItem*);
	//! Shows the source connections of the selected node
	void listSourcePropertyConnections(QTreeWidgetItem *);
	//! Shows the expression for the selected property
	void showPropertyConnectionExpression(QTreeWidgetItem* item);
	//! Saves the property connection expression
	void savePropertyConnectionExpression();


	//Property callback dialog related functions
	//! Loads the properties of a node
	void loadNodeProperties();
	//! Shows the callback expression of a selected node
	void showCallbackExpression(QTreeWidgetItem* item);
	//! Saves the callback expression of the current node
	void savePropertyCallback();
	//! Remvoves the selected callback expression
	void removePropertyCallback();
	//! List all node properties with a callback
	void listPropertiesWithCallback();
	//! List all internal connections of a node
	void loadInternalConnectionsOfANode();
    void reloadInternalConnectionsOfTheActiveNode();
	//! Swaps the nodes in the IO connection dialog
	void swapIONodes();
	//! Swaps the nodes in the property connection dialog
	void swapPropertyNodes();
	void nodeConnectionClicked(QTreeWidgetItem* item);
	void propertyExpressionEditContentsChanged();
	void propertyCallbackEditContentsChanged();

	void showNodeConnectionsPage();
	void showPropertyConnectionsPage();
	void showInternalConnectionsPage();
	void showPropertyCallbacksPage();

private:

	void updateTitlebarText(QString s);

	Ui::NEConnectionInspector *ui;

	//! The entity shown in the target node connections table
	NENode* m_pIOTargetNode;
	//! The entity shown in the source node connections table
	NENode* m_pIOSourceNode;
	//! Keeps if the connections of the node in the input table are shown
	bool m_TargetIOsShown;

	//! The entity shown in the target property connections table
	NENode* m_pPropTargetNode;
	//! The entity shown in the source property connections table
	NENode* m_pPropSourceNode;
	//! Keeps if the connections of the node in the input table are shown
	bool m_TargetPropertiesShown;

	//! The entity shown in the property callbacks table
	NENode* m_pCallbackNode;

	//! The entity shown in the internal connections table
	NENode* m_pInternalConnectionsNode;

	// Python highlighters for different expression editors
	PythonHighlighter* m_pPythonHighlighter;
	PythonHighlighter* m_pPythonHighlighter2;

    //
    //! Set if target needs to be sorted
    //
    bool m_TargetSort;
};

