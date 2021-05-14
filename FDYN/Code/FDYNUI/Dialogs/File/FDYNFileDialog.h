#pragma once

class FDYNFileWidget : public QFileDialog
{
    Q_OBJECT
public:
    FDYNFileWidget(QWidget * parent);
    ~FDYNFileWidget();

protected:
    void showEvent(QShowEvent* ev);
};

class FDYNFileDialog : public QDialog
{
    Q_OBJECT
public:
    enum DialogMode { OPEN, SAVE, IMPORT, REFER, PLOT3D, COLORRAMP_OPEN, COLORRAMP_SAVE };

    FDYNFileDialog(QWidget *parent, QString wDir, QString caption = "",  QString filter = "",
                   DialogMode mode = OPEN, QFileDialog::FileMode flags = QFileDialog::ExistingFile);
    ~FDYNFileDialog();

    void setExtension(QString strExt);
    QString extension();
    QString dataPath();

    //import
    float xScale();
    float yScale();
    float zScale();
    bool isSwapXY();
    bool isTwoSideLighting();
    bool isSolidWireframe();
    //refer
    float uniformScale();
    bool isReload();
    bool isSwapAxis();
    bool isActive();
    bool isMeshesAsOneOBject();

    QString selectedFile();
    QStringList selectedFiles();
    QString directory() { return fileDialog->directory().path(); }
    void addWidgets(QWidget *widgets);

private:
    FDYNFileWidget *fileDialog;

    //save
    QCheckBox *useExistingCache;
    QCheckBox *copyCache;
    //import & refer
    QLabel *scaleLabel;
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *zLabel;
    QLineEdit *xScaleEdit;
    QLineEdit *yScaleEdit;
    QLineEdit *zScaleEdit;
    QCheckBox *swapXY;
    QCheckBox *twoSideLighting;
    QCheckBox *solidWireframe;
    QCheckBox *active;
    QCheckBox *meshesAsOneObject;

    QLabel    *uniformScaleLabel;
    QLineEdit *uniformScaleEdit;
    QCheckBox *reload;
    QCheckBox *swapAxis;

    QLabel      *dataDir;
    QLineEdit   *dataDirEdit;
    QPushButton *refButton;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;

    QHBoxLayout *hboxLayout;
    QHBoxLayout *hboxLayout0;
    QHBoxLayout *hboxLayout1;
    QHBoxLayout *hboxLayout2;
    QHBoxLayout *hboxLayout3;

public slots:
    void setDataPath();
    void accept();

protected:
    void showEvent(QShowEvent*);
};

