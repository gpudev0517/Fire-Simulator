#ifndef CUSTOMTYPE_H
#define CUSTOMTYPE_H

#include <QObject>

class FDYNPropertyCustomType : public QObject
{
    Q_OBJECT

    // Object name
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool hidden READ hidden WRITE setHidden NOTIFY hiddenChanged)
    Q_PROPERTY(QVariant extra READ extra WRITE setExtra NOTIFY extraChanged)
    Q_PROPERTY(double extra1 READ extra1 WRITE setExtra1 NOTIFY extra1Changed)
    Q_PROPERTY(double extra2 READ extra2 WRITE setExtra2 NOTIFY extra2Changed)
    Q_PROPERTY(double extra3 READ extra3 WRITE setExtra3 NOTIFY extra3Changed)
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(Qt::ItemFlags flags READ flags WRITE setFlags NOTIFY flagsChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(int materialIndex READ materialIndex WRITE setMaterialIndex NOTIFY materialIndexChanged)
    Q_PROPERTY(QVariant materialList READ materialList WRITE setMaterialList NOTIFY materialListChanged)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)

public:
    explicit FDYNPropertyCustomType(QObject *parent = 0);
    FDYNPropertyCustomType(const FDYNPropertyCustomType &other);
    ~FDYNPropertyCustomType();

    QString name();
    void setName(QString name);

    int materialIndex();
    void setMaterialIndex(int idx);

    QVariant materialList();
    void setMaterialList(QVariant variant);

    int index();
    void setIndex(int index);

    QVariant value();
    void setValue(QVariant value);

    int type();
    void setType(int type);

    int status();
    void setStatus(int status);

    QString icon();
    void setIcon(QString iconUrl);

    Qt::ItemFlags flags();
    void setFlags(Qt::ItemFlags flags);

    QFont font();
    void setFont(QFont font);

    bool hidden();
    void setHidden(bool hidden);

    QVariant extra();
    void setExtra(QVariant extra);

    double extra1();
    void setExtra1(double extra);

    double extra2();
    void setExtra2(double extra);

    double extra3();
    void setExtra3(double extra);

signals:
    void nameChanged();
    void valueChanged();
    void typeChanged();
    void statusChanged();
    void iconChanged();
    void flagsChanged();
    void fontChanged();
    void hiddenChanged();
    void extraChanged();
    void extra1Changed();
    void extra2Changed();
    void extra3Changed();
    void materialListChanged();
    void materialIndexChanged();
    void indexChanged();

private:
    QString mName;
    QVariant mValue;
    QVariant mExtra;
    double mExtra1;
    double mExtra2;
    double mExtra3;
    int mType;
    bool mHidden;
    int mStatus;
    QString mIconUrl;
    QString mIconUrl2; //for active, inactive and cache
    QFont mFont;
    Qt::ItemFlags mFlgas;
    int mMaterialIndex;
    QVariant mMaterialList;
    int mIndex;
};

Q_DECLARE_METATYPE(FDYNPropertyCustomType)

#endif // CUSTOMTYPE_H
