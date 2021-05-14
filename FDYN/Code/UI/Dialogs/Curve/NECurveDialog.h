#ifndef NECurveDialog_H
#define NECurveDialog_H

#include "ui_NECurveDialog.h"

class NECurve;
class Spline;
class NEKey;
class NENode;
class NECurveManager;
class NEEnumCurveScene;
class NECurveScene;
class NEAbstractDialog;


class NECurveDialog : public QDialog, public Ui::NECurveDialog
{
	Q_OBJECT

public:
	NECurveDialog(NENode* node, QString propertyName, QWidget *parent = 0,  Qt::WindowFlags flags = 0);
	~NECurveDialog();
	//! Returns the QGraphicsScene shown in this dialog
    QGraphicsScene* getScene();
protected:
	void showEvent(QShowEvent* event);
private slots:
	void selectedPropertyChanged(QTableWidgetItem* item);
	void onScenePosChanged(QPointF point);
	//! Toggles the current Keyframe mode (e.g. Add, Move, Delete)
	void toggleKeyframeMode();
public slots:
	//! Called if the user clicks the Apply button
	void apply();
	//! Called if the user accepts the Dialog by clicking Okay
	void accept();
	//! Called to set and activate the curve type dropdown
	void setCurveType(NEKey* key, NECurve* curve);
	//! Fits the current curve of the scene into the view
	void fitInView();
	//! Called if the currently selected Key is moved
	void currentKeyMoved(NEKey* key);
	//! Called if the user changes the values in the x_keyframe/y_keyframe line edits
	void keyValueChanged();
private:
	void initializeScene(QString popertyName);
	NECurveScene *scene;
	QActionGroup* splinesActionGroup;
	NENode* m_node;
	QString selectedProperty;
	NEKey* m_currentKey;
};

#endif // NECurveDialog_H
