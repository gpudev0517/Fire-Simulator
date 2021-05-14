#pragma once

class NEMenu : public QMenu
{
    Q_OBJECT
public:
    explicit NEMenu(QWidget *parent = 0);

signals:

public:


public slots:
    void setTitle(const QString & title);

};


