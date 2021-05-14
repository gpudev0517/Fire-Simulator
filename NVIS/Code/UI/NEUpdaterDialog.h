#pragma once
#include <QDialog>


#include <pimpl_ptr.h>
#include <kdtoolsglobal.h>

class NEUpdaterDialog : public QDialog
{
    Q_OBJECT
    KDAB_DISABLE_COPY( UpdaterDialog );
public:    
    explicit NEUpdaterDialog( QWidget *parent = 0 );
    ~NEUpdaterDialog();

private:
    class Private;
    kdtools::pimpl_ptr< Private > d;

};

