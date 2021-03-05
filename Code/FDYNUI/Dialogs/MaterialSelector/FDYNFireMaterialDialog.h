#pragma once

#include <QDialog>
#include <QItemSelection>
#include <QTableWidgetItem>

#include "Utility/NEFireMaterial.h"

namespace Ui {
class FDYNFireMaterialDialog;
}

struct FireMaterial
{
    QString m_Name;
    int m_ID; //  Id assigned by the database
    int m_Index; // Index in m_MaterialList
    float m_C;
    float m_N;
    float m_H;
    float m_Cl;
    float m_O;
    float m_CombuHeat;
    float m_RadFraction;
    QList<FireMaterialInfo> m_Infos;

    FireMaterial()
    {
    }
    FireMaterial( const NEFireMaterial& mtl )
    {
       copy(mtl);
    }
    void operator =(const NEFireMaterial& mtl )
    {
        copy( mtl );
    }
    operator NEFireMaterial()
    {
        NEFireMaterial fm;

        fm.setName( m_Name );
        fm.setUniqueID( m_ID );
        fm.setIndex( m_Index );
        fm.setC( m_C );
        fm.setN( m_N );
        fm.setH( m_H );
        fm.setCI( m_Cl );
        fm.setO( m_O );
        fm.setCombuHeat( m_CombuHeat );
        fm.setRadFraction( m_RadFraction );
        fm.setInfos(m_Infos);

        return fm;
    }
    void copy(const NEFireMaterial& mtl)
    {
        m_Name = mtl.name();
        m_ID = mtl.uniqueID();
        m_Index = mtl.index();
        m_C = mtl.C();
        m_N = mtl.N();
        m_H = mtl.H();
        m_Cl = mtl.CI();
        m_O = mtl.O();
        m_CombuHeat = mtl.combuHeat();
        m_RadFraction = mtl.radFraction();
        m_Infos = mtl.Infos();
    }
};

class NENode;

class FDYNFireMaterialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNFireMaterialDialog(QWidget *parent = 0);
    virtual ~FDYNFireMaterialDialog();

    QStringList materialNameList();

    void showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type);

    void setDefaultMaterialProps(FireMaterial &mat);
    void setLineEdits(FireMaterial &mat);
    void addRowToTable(FireMaterial &mat);
    void updateControlsState();
    void clear();

public slots:
    void editedLineEdit();
    void materialInfoChanged(int row, int column);

    FireMaterial createNewMaterial();
    FireMaterial createNullMaterial();
    void addNewMaterial();
    void addMaterial(FireMaterial& mat);
    void updateMaterial(FireMaterial& mat);
    void updateMaterial(int);
    void removeMaterial();

    void updateRow(const QItemSelection&, const QItemSelection&);

    void addT2();
    void drawGraph(int type);

    virtual void accept();
    virtual void reject();

signals:
    void sendAddFireMaterial(FireMaterial s);
    void sendRemoveFireMaterial(FireMaterial s);
    void sendAssignFireMaterial(NENode *, FireMaterial s);

private:
    Ui::FDYNFireMaterialDialog *ui;

public:
    FireMaterial *m_CurMaterial;
    QList<FireMaterial> m_MaterialList;
};
