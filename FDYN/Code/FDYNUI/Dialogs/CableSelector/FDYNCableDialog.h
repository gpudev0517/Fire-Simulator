#pragma once

#include <QDialog>
#include <QStyledItemDelegate>
#include "Geometry/Utility/NECableProperty.h"

namespace Ui {
class FDYNCableDialog;
}

class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ComboBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

struct CableProperty
{
    QString m_Name;
    QString m_CleanName;

    int m_ID; //  Id assigned by the database
    int m_Index; // Index in m_MaterialList

    QString m_Model;
    int m_InsulationType;
    int m_JacketType;
    float m_OutSideDiameterMM;
    float m_OutSideDiameterKgPerMeter;
    float m_JacketThicknessMM;
    float m_Gauges;
    float m_Conductors;
    QString m_Description;
    float m_Grade;
    float m_Part;
    float m_Item;

    QString m_Category;
    QString m_Manufacturer;
    QString m_Issues;

    CableProperty()
    {
    }
    CableProperty( const NECableProperty& mtl );
    void operator =(const NECableProperty& mtl );
    operator NECableProperty();
    void copy(const NECableProperty& mtl);


};

class NENode;

class FDYNCableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNCableDialog(QWidget *parent = 0);
    virtual ~FDYNCableDialog();

    QStringList cablePropertyList();

    void showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type);

    void setDefaultCableProps(CableProperty &fail);
    void addRowToTable(CableProperty &fail);
    void clear();

    void updateFilterCombos(std::vector<QList<QString>> options);

signals:
    void sendAddCableProperty(CableProperty fail);
    void sendRemoveCableProperty(CableProperty fail);
    void sendAssignCableProperty(NENode *node, CableProperty fail);

public slots:

    CableProperty createCableProperty();
    CableProperty createNullCableProperty();
    void addCableProperty();
    void addCableProperty(CableProperty& fail);
    void updateCableProperty(CableProperty& fail);
    void duplicateCableProperty();
    void removeCableProperty();
    void insulationTypeChanged(int _idx);
    void jacketTypeChanged(int _idx);

    void categoryTypeChanged(int _idx);
    void gaugeTypeChanged(int _idx);
    void conductorsTypeChanged(int _idx);

    void updateRow(const QItemSelection&, const QItemSelection&);
    void updateItem(QTableWidgetItem *item);

    void filterByCombos(QString categoryFilter, QString gaugeFilter, QString conductorFilter);
    void filterCategory(QString index);
    void filterGauge(QString index);
    void filterConductor(QString index);

    virtual void accept();
    virtual void reject();

private:
    Ui::FDYNCableDialog *ui;
    int m_prevSelectedIndex;

public:
    CableProperty *m_CurCableProperty;
    QList<CableProperty> m_CablePropertyList;
    QList<int> m_filterdCableIndexList;

    QList<QString> m_CategoryTypes;
    QList<QString> m_InsulationTypes;
};
