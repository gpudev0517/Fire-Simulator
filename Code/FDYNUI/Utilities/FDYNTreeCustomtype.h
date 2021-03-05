#ifndef CUSTOMTYPE_H
#define CUSTOMTYPE_H

#include <QObject>

class FDYNTreeCustomType : public QObject
{
    Q_OBJECT

    // Object name
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString icon2 READ icon2 WRITE setIcon2 NOTIFY icon2Changed)
    Q_PROPERTY(QString tooltip READ tooltip WRITE setTooltip NOTIFY tooltipChanged)
    Q_PROPERTY(Qt::ItemFlags flags READ flags WRITE setFlags NOTIFY flagsChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)

public:
    explicit FDYNTreeCustomType(QObject *parent = 0);
    FDYNTreeCustomType(const FDYNTreeCustomType &other);
    ~FDYNTreeCustomType();

    QString name();
    void setName(QString name);

    int status();
    void setStatus(int status);

    QString icon();
    void setIcon(QString iconUrl);

    QString icon2();
    void setIcon2(QString iconUrl);

    Qt::ItemFlags flags();
    void setFlags(Qt::ItemFlags flags);

    QFont font();
    void setFont(QFont font);

    QString tooltip(){
        return mTooltip;
    }

    void setTooltip(QString tooltip){
        mTooltip = tooltip;
        emit tooltipChanged();

    }


signals:
    void nameChanged();
    void statusChanged();
    void iconChanged();
    void icon2Changed();
    void flagsChanged();
    void fontChanged();
    void tooltipChanged();

private:
    QString mName;
    int mStatus;
    QString mIconUrl;
    QString mIconUrl2; //for active, inactive and cache
    QFont mFont;
    Qt::ItemFlags mFlgas;
    QString mTooltip;
};

Q_DECLARE_METATYPE(FDYNTreeCustomType)

#endif // CUSTOMTYPE_H
