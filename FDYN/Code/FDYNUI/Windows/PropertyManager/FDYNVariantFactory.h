#pragma once

#include "Windows/PropertyManager/qtvariantproperty.h"

class FileEdit;


class FDYNVariantFactory : public QtVariantEditorFactory
{
    Q_OBJECT
public:
    FDYNVariantFactory(QObject *parent = 0)
        : QtVariantEditorFactory(parent)
    {
    }

    virtual ~FDYNVariantFactory();
protected:
    virtual void connectPropertyManager(QtVariantPropertyManager *manager);
    virtual QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent);
    virtual void disconnectPropertyManager(QtVariantPropertyManager *manager);
private slots:
    void slotPropertyChanged(QtProperty *property, const QVariant &value);
    void slotPropertyAttributeChanged(QtProperty *property, const QString &attribute, const QVariant &value);
    void slotSetValue(const QString &value);
    void slotRampChanged();
    void slotEditorDestroyed(QObject *object);
private:
    QMap<QtProperty *, QList<QWidget *> > theCreatedEditors;
    QMap<QWidget *, QtProperty *> theEditorToProperty;
};
