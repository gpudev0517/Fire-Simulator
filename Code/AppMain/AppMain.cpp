#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Base/NEApplication.h"

#include "Utilities/FDYNUIApplication.h"

int main(int argc, char* argv[])
{
    std::shared_ptr< FDYNUIApplication > a = std::shared_ptr< FDYNUIApplication >( new FDYNUIApplication( argc, argv ) );

    a->run();

    exit( 0 );
}
