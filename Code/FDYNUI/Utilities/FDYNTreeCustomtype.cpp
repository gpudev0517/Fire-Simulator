#include "FDYNTreeCustomtype.h"

FDYNTreeCustomType::FDYNTreeCustomType(QObject *parent) : QObject(parent)
{
    mName = QString("");
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

FDYNTreeCustomType::FDYNTreeCustomType(const FDYNTreeCustomType &other)
{
    mName = other.mName;
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

FDYNTreeCustomType::~FDYNTreeCustomType()
{
}

QString FDYNTreeCustomType::name()
{
    return mName;
}

void FDYNTreeCustomType::setName(QString name)
{
    mName = name;
    emit nameChanged();
}

int FDYNTreeCustomType::status() {
    return mStatus;
}

void FDYNTreeCustomType::setStatus(int status) {
    mStatus = status;
    emit statusChanged();
}

QString FDYNTreeCustomType::icon() {
    return mIconUrl;
}

void FDYNTreeCustomType::setIcon(QString iconUrl) {
    mIconUrl = iconUrl;
    emit iconChanged();
}

QString FDYNTreeCustomType::icon2() {
    return mIconUrl2;
}

void FDYNTreeCustomType::setIcon2(QString iconUrl) {
    mIconUrl2 = iconUrl;
    emit icon2Changed();
}

Qt::ItemFlags FDYNTreeCustomType::flags()
{
    return mFlgas;
}
void FDYNTreeCustomType::setFlags(Qt::ItemFlags flags)
{
    mFlgas = flags;
    emit flagsChanged();
}

QFont FDYNTreeCustomType::font() {
    return mFont;
}

void FDYNTreeCustomType::setFont(QFont font) {
    mFont = font;
    emit fontChanged();
}
