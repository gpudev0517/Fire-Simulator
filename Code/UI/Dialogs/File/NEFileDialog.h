#ifndef NEFileDialog_H
#define NEFileDialog_H

class NEFileWidget : public QFileDialog
{
    Q_OBJECT
public:
    NEFileWidget(QWidget * parent);
    ~NEFileWidget();

protected:
    void showEvent(QShowEvent* ev);
};

class NEFileDialog : public QDialog
{
    Q_OBJECT
public:
    enum DialogMode { OPEN, SAVE, IMPORT, REFER, PLOT3D, COLORRAMP_OPEN, COLORRAMP_SAVE };

    NEFileDialog(QWidget *parent, QString wDir, QString caption = "",  QString filter = "", DialogMode mode = OPEN, QFileDialog::FileMode flags = QFileDialog::ExistingFile);
    ~NEFileDialog();

    void setExtension(QString strExt);
    QString extension();
    QString dataPath();
    //open
    int openMPThreads();
    bool isOptimizeNetCache();
    bool isCompressFluidCaches();
    //save
    bool isUseExistingCache();
    bool isCopyCache();
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
    void addWidgets(QWidget *widgets);

private:
    NEFileWidget *fileDialog;
    //open
    QLabel *threadsLabel;
    QSpinBox *threadsSpin;
    QCheckBox *optimizeNetCache;
    QCheckBox *compressFluidCaches;
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

#endif // NEFileDialog_H
