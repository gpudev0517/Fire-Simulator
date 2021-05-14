
#ifndef QTPROPERTYBASE_H
#define QTPROPERTYBASE_H

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

class QtPropertyBase
{
public:
    virtual QString propertyName() const { return "QtPropertyBase"; }
    virtual void setEnabled(bool enable) {}
    virtual void setAttribute(const QString &attribute, const QVariant &value) {}
};


#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#endif
