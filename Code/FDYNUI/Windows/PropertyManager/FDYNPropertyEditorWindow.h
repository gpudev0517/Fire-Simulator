#pragma once

class QTreeWidgetItem;
class QRegExpValidator;

class FDYNGUIManager;

class NEObject;

class QQuickWidget;
class FDYNObjectController;
class FDYNPropertyItem;

#ifdef OLD_PROPERTY
namespace Ui {
class FDYNPropertyEditorWindow;
}
#endif

class FDYNPropertyEditorWindow : public QMainWindow
{
    Q_OBJECT

    friend class FDYNGUIManager;
    friend class FDYNGUIManagerPrivate;

public:
    explicit FDYNPropertyEditorWindow(QWidget *parent = 0);
    ~FDYNPropertyEditorWindow();

    //! Initializes the property editor
    void init();

public slots:
    //! Show the properties for the given item
    void showObjectProperty(QObject* object);
    QObject* currentObject();
    void showPrevObjProperty();
    //! Updates the properties for the current object
    void updateProperties();
    //! Removes an object's property editor data
    //!
    void removeObject(QObject* object);
    void loadSelectedObject();
    void loadPreset();
    void savePreset();
    void othersComboBoxActivated(int index);
    void othersComboBoxEditTextChanged(const QString& str);

private slots:
    void addProperty();
    void onFilterTextChanged(const QString& s);

private:

#ifdef OLD_PROPERTY
    void setChildsVisibility(QTreeWidgetItem* item, QString exp);
    Ui::FDYNPropertyEditorWindow *ui;
#else
    void setChildsVisibility(FDYNPropertyItem* item, QString exp);
#endif
    static QObject* m_pPrevObject;
    QRegExpValidator* m_PropNameValidator;
    QRegExpValidator* m_FreeValidator;
    mutable bool f = false;

    QLineEdit* comboLineEdit;
    QVector<QString> othersCBStrs;

    QPropertyAnimation* m_LineEditAnim;
#ifndef OLD_PROPERTY
    FDYNObjectController* m_objController;
public:
    FDYNObjectController* objectController(){
        return m_objController;
    }
#endif
};
