#pragma once

#include "qtvariantproperty.h"

class FileEdit;


class NEVariantFactory : public QtVariantEditorFactory
{
    Q_OBJECT
public:
    NEVariantFactory(QObject *parent = 0)
        : QtVariantEditorFactory(parent)
    {
    }

    virtual ~NEVariantFactory();
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
