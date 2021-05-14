#pragma once

class NENode;
class NEExpressionEditor;
class PythonHighlighter;

namespace Ui {
class NEExpressionEditorWindow;
}

class NEExpressionEditorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit NEExpressionEditorWindow(QWidget *parent = 0);
	uint init();

	void reinitStyle(bool dark);

	~NEExpressionEditorWindow();

public slots:
	void showNodePropertyExpression(const NENode* val, const QString prop);
	void saveNodePropertyExpression();
    uint saveNodePropertyExpressionToAFile();
    uint loadNodePropertyExpressionFromAFile();

private:

	PythonHighlighter* m_pHighlighter;
	NEExpressionEditor* expEditor();
	NENode* m_Node;
	QString m_Property;

	Ui::NEExpressionEditorWindow *ui;
};
