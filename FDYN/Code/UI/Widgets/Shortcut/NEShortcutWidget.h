#ifndef NESHORTCUTWIDGET_H
#define NESHORTCUTWIDGET_H

#include <QWidget>

namespace Ui {
class NEShortcutWidget;
}

class NEShortcutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NEShortcutWidget(QWidget *parent = 0);
    ~NEShortcutWidget();

private:
    Ui::NEShortcutWidget *ui;
};

#endif // NESHORTCUTWIDGET_H
