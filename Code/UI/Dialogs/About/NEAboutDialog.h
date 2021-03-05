#ifndef NEABOUTDIALOG_H
#define NEABOUTDIALOG_H


namespace Ui {
class NEAboutDialog;
}

class NEAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NEAboutDialog(QWidget *parent = 0);
    ~NEAboutDialog();

private:
    Ui::NEAboutDialog *ui;
};

#endif // NEABOUTDIALOG_H
