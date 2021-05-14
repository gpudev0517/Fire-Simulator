#pragma once

class QTreeWidgetItem;

namespace Ui {
class NEShelfItemEditor;
}

class NEShelfItemEditor : public QDialog
{
    Q_OBJECT

public:
    explicit NEShelfItemEditor(QWidget *parent = 0);
    ~NEShelfItemEditor();
    void populateEditor(QString name, QString help, QString script, QString icon, QTreeWidgetItem* twi);


private slots:
    void acceptDialog();
    void rejectDialog();
    void pickIcon();

private:
    Ui::NEShelfItemEditor *ui;
    QTreeWidgetItem* m_CurrentTreeWidgetItem;
    QString m_LastIconPath;

};

