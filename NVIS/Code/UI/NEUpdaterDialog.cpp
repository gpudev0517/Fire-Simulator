/****************************************************************************
** Copyright (C) 2001-2012 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Tools library.
**
** Licensees holding valid commercial KD Tools licenses may use this file in
** accordance with the KD Tools Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the GNU
** Lesser General Public License version 2 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.net if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#include "NEUpdaterDialog.h"
#include "NEUpdater.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QProgressDialog>

#include <QtCore/QEventLoop>


class NEUpdaterDialog::Private : QObject
{
    Q_OBJECT
public:
    Private( NEUpdaterDialog* parent = NULL);
    ~Private();
    void init();

private:
    void createWidgets();
    void createConnections();
    void layoutWidgets();

private:
    NEUpdaterDialog* m_parent;
    QDialogButtonBox* m_buttonBox;
    QPushButton* m_checkForUpdatesBtn;
    QPushButton* m_installUpdatesBtn;
    QLabel* m_notification;
    Updater m_updater;

private Q_SLOTS:
    void checkForUpdates();
    void installUpdates();
};

NEUpdaterDialog::Private::Private( NEUpdaterDialog* parent ) :
        m_parent( parent ),
        m_buttonBox( NULL ),
        m_checkForUpdatesBtn( NULL ),
        m_installUpdatesBtn( NULL ),
        m_notification( NULL )
{

}

void NEUpdaterDialog::Private::createWidgets()
{
    m_buttonBox = new QDialogButtonBox( QDialogButtonBox::Close, Qt::Horizontal, m_parent );

    m_checkForUpdatesBtn = new QPushButton( tr( "Search updates" ), m_buttonBox);

    m_buttonBox->addButton( m_checkForUpdatesBtn, QDialogButtonBox::ActionRole );

    m_installUpdatesBtn = new QPushButton( tr( "Install updates" ), m_buttonBox);
    m_installUpdatesBtn->setEnabled( false );
    m_buttonBox->addButton( m_installUpdatesBtn, QDialogButtonBox::AcceptRole );


    m_notification = new QLabel( m_parent);
    const QString textDisplay = tr( "Currently version %1 of %2 is installed" ).arg( m_updater.getInstalledAppVersion() , m_updater.getAppName() );
    m_notification->setText( textDisplay );
}

void NEUpdaterDialog::Private::createConnections()
{
    connect( m_buttonBox, SIGNAL(rejected()), m_parent, SLOT(reject()) );
    connect( m_checkForUpdatesBtn, SIGNAL(clicked()), this, SLOT(checkForUpdates()) );
    connect( m_installUpdatesBtn, SIGNAL(clicked()), this, SLOT(installUpdates()) );
}

void NEUpdaterDialog::Private::layoutWidgets()
{
    QVBoxLayout* layout = new QVBoxLayout( m_parent );
    layout->addWidget( m_notification, 1);
    layout->addWidget( m_buttonBox, 0);
}

void NEUpdaterDialog::Private::checkForUpdates()
{
    m_checkForUpdatesBtn->setEnabled( false );
    QProgressDialog dialog;
    dialog.setLabelText( tr( "searching updates" ) );
    const bool success = m_updater.checkForUpdates();
    const QString updateFoundText = tr( "\nFound update to version: ");
    if ( success )
    {
        m_notification->setText( m_notification->text() + updateFoundText + m_updater.getUpdateVersion() );
        m_installUpdatesBtn->setEnabled( true );
    }
    else
    {
        m_installUpdatesBtn->setEnabled( false );
    }
    m_checkForUpdatesBtn->setEnabled( true );
}

void NEUpdaterDialog::Private::installUpdates()
{
    const bool success = m_updater.installUpdates();
    if ( success )
    {        
        m_parent->accept();
    }
}

void NEUpdaterDialog::Private::init()
{
    createWidgets();
    createConnections();
    layoutWidgets();
}

NEUpdaterDialog::Private::~Private()
{

}


NEUpdaterDialog::NEUpdaterDialog( QWidget *parent ) :
    QDialog( parent ),
    d( new Private( this ) )
{
    d->init();
}

NEUpdaterDialog::~NEUpdaterDialog()
{

}

#include "NEUpdaterDialog.moc"
