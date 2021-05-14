#pragma once

#include "Base/NEApplication.h"

//! NEUIApplictation is a QApplication that also handles file passing .
/*!
    Such a class is required to make to open scene files with double click on Mac
*/
class NECEXP_FDYNUI FDYNUIApplication  : public NEApplication
{
public:
    FDYNUIApplication(int& argc, char **argv);
    virtual ~FDYNUIApplication();
    virtual void run() const;

    void setStyle(QStyle *style );


};
