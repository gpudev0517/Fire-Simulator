#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Base/NEApplication.h"


#include "NEUIApplication.h"
#include "Managers/NEGUIManager.h"


int main(int argc, char* argv[])
{
    std::shared_ptr< NEUIApplication > a = std::shared_ptr< NEUIApplication >( new NEUIApplication( argc, argv ) );

    a->run();

    exit( 0 );
}
