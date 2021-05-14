#pragma once


struct NEShelfData
{
    QString script;
    QString help;
    QString icon;
};

class NEShelfBar : public QToolBar
{
	Q_OBJECT
public:
    NEShelfBar(QWidget* parent);
    uint addShelfButton(QString name, QString script, QString help, QString icon);
    uint addShelfButton(QString name, const NEShelfData& data);
    uint removeShelfButton(QString name);

    QMap<QString, NEShelfData>& shelfData();

private:
    QMap<QString, NEShelfData> m_ShelfData;
    QMap<QAction*, NEShelfData*> m_ActionToShelfData;
    QMap<NEShelfData*, QAction*> m_ShelfDataToAction;
    uint createAction(QString name, QString icon);
    void populateShelf();

private slots:
    void processAction();

};




