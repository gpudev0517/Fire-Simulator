#pragma once

#include "Base/NEApplication.h"

//! NEUIApplictation is a QApplication that also handles file passing to Neutrino.
/*!
    Such a class is required to make Neutrino open scene files with double click on Mac
*/
class NECEXP_NEUI NEUIApplication  : public NEApplication
{
public:
    NEUIApplication(int& argc, char **argv);
    virtual ~NEUIApplication();
    virtual void run() const;

    void setStyle(QStyle *style );


protected:
    bool event( QEvent *e );

};
