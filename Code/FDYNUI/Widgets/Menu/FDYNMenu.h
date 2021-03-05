#pragma once

class FDYNMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FDYNMenu(QWidget *parent = 0);

signals:

public:


public slots:
    void setTitle(const QString & title);

};


