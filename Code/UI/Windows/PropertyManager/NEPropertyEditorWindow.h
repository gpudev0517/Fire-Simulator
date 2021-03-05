#pragma once

class QTreeWidgetItem;
class QRegExpValidator;

class NEGUIManager;
class NEObjectController;
class NEObject;

namespace Ui {
class NEPropertyEditorWindow;
}

class NEPropertyEditorWindow : public QMainWindow
{
    Q_OBJECT

    friend class NEGUIManager;
    friend class NEGUIManagerPrivate;

public:
    explicit NEPropertyEditorWindow(QWidget *parent = 0);
    ~NEPropertyEditorWindow();

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
    void setChildsVisibility(QTreeWidgetItem* item, QString exp);
    Ui::NEPropertyEditorWindow *ui;
    static QObject* m_pPrevObject;
    QRegExpValidator* m_PropNameValidator;
    QRegExpValidator* m_FreeValidator;
    mutable bool f = false;

    QLineEdit* comboLineEdit;
    QVector<QString> othersCBStrs;

    QPropertyAnimation* m_LineEditAnim;
};
