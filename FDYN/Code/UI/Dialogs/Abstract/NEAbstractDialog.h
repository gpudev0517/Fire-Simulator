# pragma once

class QVBoxLayout;
class QGridLayout;
class QPushButton;
class QHBoxLayout;
class QComboBox;
class QSignalMapper;

/*!
        This class provides some functionality for adding widget to a dialog generically.
        All dialogs should be derived from this class.
*/
class NEAbstractDialog : public QDialog
{
    Q_OBJECT

public:
    NEAbstractDialog(QWidget* parent, uint DialogType=0);
    //NEAbstractDialog(QWidget* parent, );
    ~NEAbstractDialog();

signals:
    void comboBoxIndexChanged(int widgetIndex);
    //public slots:
    //	//!
    //	virtual void comboBoxChanged(int itemIndex, int widgetIndex);

protected:
    QWidget* getWidget(int index);
    int getPropertyIndex(QWidget* widget);

    QVariant valueOfWidget(int index, uint type);
    void setValueOfWidget(int index, QVariant value);

    //Add widget methods
    void addCheckBox(int index ,int row, int column, QGridLayout* layout);
    //!
    void addLineEdit(int index ,int row, int column, QGridLayout* layout);
    //!
    void addSpinBox(int index , int row, int column, QGridLayout* layout, QIntValidator* validator=NULL);
    //!
    void addDoubleSpinBox(int index ,int row,  int column, QGridLayout* layout, QDoubleValidator* validator=NULL);
    //!
    void addLabel(QString name ,int row,  int column, QGridLayout* layout);
    //!
    void addVector4D(int index ,int row,  int column, QGridLayout* layout,QDoubleValidator* validator=NULL);
    //!
    void addVector3D(int index ,int row,  int column, QGridLayout* layout,QDoubleValidator* validator=NULL);
    //! Creates and returns a combo box filled with the QStrinList.
    QComboBox* addComboBox(const QStringList& itemList, int row, int column, QGridLayout* layout);
    //! Creates and returns a combo box filled with the keys of the QMetaEnum
    QComboBox* addComboBox(const QMetaEnum& metaEnum, int index, int row, int column, QGridLayout* layout);
    //!
    QGridLayout* addGroupBox(QString text, QVBoxLayout* pLayout, QWidget* parent=0);
    //!
    QGridLayout* groupBoxLayout(QString text);
    //! Reads all Q_PROPERTIES of the metaObject and creates widgets for it. Offset can be used to exclude properties of inherited classes
    void addPropertyWidgets(const QObject *object, int row, int column, QGridLayout* layout, int offset=1);
    //! Writes all widget values for the Q_PROPERTIES back to the object
    void writeAllProperties(QObject *object, QGridLayout* layout, int offset=1);

    //void writeProperty();
    //! Removes all widgets of the layout and deletes them.
    void clearLayout(QGridLayout* layout);
    //void createGroupBoxForLayout(QVBoxLayout* layout);
    //!
    QMap<int,QWidget*> m_Widgets;
    //!
    QMap<QString,QGridLayout*> m_Layouts;
    //!
    QSignalMapper* m_SignalMapper;

    QPushButton* m_SubmitButton;
    QPushButton* m_CancelButton;

    QVBoxLayout* m_MainLayout;
    QGridLayout* m_MainContentLayout;
    QHBoxLayout* m_ButtonsLayout;
};
