#pragma once

#include <QDialog>

#include "Utility/NESurfaceMaterial.h"

namespace Ui {
class FDYNSurfaceMaterialDialog;
}

struct SurfaceMaterial
{
    QString m_Name;
    int m_ID; //  Id assigned by the database
    int m_Index; // Index in m_MaterialList
    float m_Conductivity;
    float m_Heat;
    float m_Density;
    float m_Thickness;
    float m_Emissivity;

    SurfaceMaterial()
    {
    }
    SurfaceMaterial( const NESurfaceMaterial& mtl )
    {
       copy(mtl);
    }
    void operator =(const NESurfaceMaterial& mtl )
    {
        copy( mtl );
    }
    operator NESurfaceMaterial()
    {
        NESurfaceMaterial sm;

        sm.setName( m_Name );
        sm.setUniqueID( m_ID );
        sm.setIndex( m_Index );
        sm.setConductivity( m_Conductivity );
        sm.setDensity( m_Density );
        sm.setEmissivity( m_Emissivity );
        sm.setHeat( m_Heat );
        sm.setThickness( m_Thickness );

        return sm;
    }
    void copy(const NESurfaceMaterial& mtl)
    {
        m_Name = mtl.name();
        m_ID = mtl.uniqueID();
        m_Index = mtl.index();
        m_Conductivity = mtl.conductivity();
        m_Density = mtl.density();
        m_Emissivity = mtl.emissivity();
        m_Heat = mtl.heat();
        m_Thickness = mtl.thickness();
    }
};

class NENode;

class FDYNSurfaceMaterialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNSurfaceMaterialDialog(QWidget *parent = 0);
    virtual ~FDYNSurfaceMaterialDialog();

    QStringList materialNameList();

    void showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type, QVariant vName);

    void setDefaultMaterialProps(SurfaceMaterial &mat);
    void addRowToTable(SurfaceMaterial &material);
    void clear();

signals:
    void sendAddSurfaceMaterial(SurfaceMaterial s);
    void sendRemoveSurfaceMaterial(SurfaceMaterial s);
    void sendAssignSurfaceMaterial(NENode *, SurfaceMaterial s, QString materialKind);
    void sendSurfaceMaterialChanged(SurfaceMaterial s);

public slots:

    SurfaceMaterial createNewMaterial();
    SurfaceMaterial createNullMaterial();
    void addNewMaterial();
    void addMaterial(SurfaceMaterial& mat);
    void updateMaterial(SurfaceMaterial& mat);
    void duplicateMaterial();
    void removeMaterial();
    void addDefaultMaterials();

    void updateRow(const QItemSelection&, const QItemSelection&);
    void updateItem(QTableWidgetItem *);

    virtual void accept();
    virtual void reject();

private:
    Ui::FDYNSurfaceMaterialDialog *ui;
    int m_prevSelectedIndex;
    QString m_CurrentMaterialKind;

public:
    SurfaceMaterial *m_CurMaterial;
    QList<SurfaceMaterial> m_MaterialList;
};
