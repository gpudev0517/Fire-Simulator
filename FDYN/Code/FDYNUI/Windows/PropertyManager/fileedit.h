#pragma once

#include "Utility/FilePath.h"

class FileEdit : public QWidget
{
    Q_OBJECT
public:
    FileEdit(QWidget *parent = 0);
    void setFilePath(const QString &filePath)
    {
        if (theLineEdit->text() != filePath) theLineEdit->setText(filePath);
    }
    QString filePath() const
    {
        return theLineEdit->text();
    }
    void setFilter(const QString &filter) { theFilter = filter; }
    QString filter() const { return theFilter; }
	FilePath::DialogType type() const { return theType; }
    void setType(FilePath::DialogType val)
    {
        theType = val;
    }
signals:
    void filePathChanged(const QString &filePath);
protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
private slots:
    void buttonClicked();
private:

    QLineEdit *theLineEdit;

    QString theFilter;
	FilePath::DialogType theType;
};

