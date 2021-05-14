#include "FDYNPropertyCustomtype.h"

FDYNPropertyCustomType::FDYNPropertyCustomType(QObject *parent) : QObject(parent)
{
    mName = QString("");
    mType = -1;
    mHidden = false;

    mStatus = 1;
    mIconUrl = QString("");
    mIconUrl2 = QString("");
    mFont = QFont();

    mFlgas = Qt::ItemIsSelectable |
                Qt::ItemIsUserCheckable |
                Qt::ItemIsEnabled |
                Qt::ItemIsDragEnabled |
                Qt::ItemIsDropEnabled;

}

FDYNPropertyCustomType::FDYNPropertyCustomType(const FDYNPropertyCustomType &other)
{
    mName = other.mName;
    mType = -1;
    mHidden = false;
    mValue = other.mValue;
    mType = other.mType;
    mStatus = other.mStatus;
    mIconUrl = other.mIconUrl;
    mIconUrl2 = other.mIconUrl2;
    mFont = other.mFont;
    mFlgas = Qt::ItemIsSelectable |
                Qt::ItemIsUserCheckable |
                Qt::ItemIsEnabled |
                Qt::ItemIsDragEnabled |
                Qt::ItemIsDropEnabled;
}

FDYNPropertyCustomType::~FDYNPropertyCustomType()
{
}

QString FDYNPropertyCustomType::name()
{
    return mName;
}

void FDYNPropertyCustomType::setName(QString name)
{
    mName = name;
    emit nameChanged();
}

int FDYNPropertyCustomType::materialIndex()
{
    return mMaterialIndex;
}
void FDYNPropertyCustomType::setMaterialIndex(int idx)
{
    mMaterialIndex = idx;
    emit materialIndexChanged();
}

QVariant FDYNPropertyCustomType::materialList()
{
    return mMaterialList;
}
void FDYNPropertyCustomType::setMaterialList(QVariant variant)
{
    mMaterialList = variant;
    emit materialListChanged();
}

int FDYNPropertyCustomType::index()
{
    return mIndex;
}
void FDYNPropertyCustomType::setIndex(int index)
{
    mIndex = index;
    emit indexChanged();
}

QVariant FDYNPropertyCustomType::value()
{
    return mValue;
}

void FDYNPropertyCustomType::setValue(QVariant value)
{
    mValue = value;
    emit valueChanged();
}

int FDYNPropertyCustomType::type()
{
    return mType;
}

void FDYNPropertyCustomType::setType(int type)
{
    mType = type;
    emit typeChanged();
}

bool FDYNPropertyCustomType::hidden()
{
    return mHidden;
}
void FDYNPropertyCustomType::setHidden(bool hidden)
{
    mHidden = hidden;
    emit hiddenChanged();
}

QVariant FDYNPropertyCustomType::extra()
{
    return mExtra;
}
void FDYNPropertyCustomType::setExtra(QVariant extra)
{
    mExtra = extra;
    emit extraChanged();
}

double FDYNPropertyCustomType::extra1()
{
    return mExtra1;
}
void FDYNPropertyCustomType::setExtra1(double extra)
{
    mExtra1 = extra;
    emit extra1Changed();
}

double FDYNPropertyCustomType::extra2()
{
    return mExtra2;
}
void FDYNPropertyCustomType::setExtra2(double extra)
{
    mExtra2 = extra;
    emit extra2Changed();
}

double FDYNPropertyCustomType::extra3()
{
    return mExtra3;
}
void FDYNPropertyCustomType::setExtra3(double extra)
{
    mExtra3 = extra;
    emit extra3Changed();
}

int FDYNPropertyCustomType::status() {
    return mStatus;
}

void FDYNPropertyCustomType::setStatus(int status) {
    mStatus = status;
    emit statusChanged();
}

QString FDYNPropertyCustomType::icon() {
    return mIconUrl;
}

void FDYNPropertyCustomType::setIcon(QString iconUrl) {
    mIconUrl = iconUrl;
    emit iconChanged();
}

Qt::ItemFlags FDYNPropertyCustomType::flags()
{
    return mFlgas;
}
void FDYNPropertyCustomType::setFlags(Qt::ItemFlags flags)
{
    mFlgas = flags;
    emit flagsChanged();
}

QFont FDYNPropertyCustomType::font() {
    return mFont;
}

void FDYNPropertyCustomType::setFont(QFont font) {
    mFont = font;
    emit fontChanged();
}
