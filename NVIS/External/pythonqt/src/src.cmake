cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_definitions(-DPYTHONQT_EXPORTS)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
set (MAIN_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/PythonQt.h
	${CMAKE_CURRENT_LIST_DIR}/PythonQtStdDecorators.h
	${CMAKE_CURRENT_LIST_DIR}/PythonQtClassInfo.h
	${CMAKE_CURRENT_LIST_DIR}/PythonQtImporter.h
	${CMAKE_CURRENT_LIST_DIR}/PythonQtObjectPtr.h
	${CMAKE_CURRENT_LIST_DIR}/PythonQtProperty.h 
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSignal.h   
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSlot.h     
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSlotDecorator.h
	${CMAKE_CURRENT_LIST_DIR}/PythonQtStdIn.h        
	${CMAKE_CURRENT_LIST_DIR}/PythonQtStdOut.h       
	${CMAKE_CURRENT_LIST_DIR}/PythonQtMisc.h         
	${CMAKE_CURRENT_LIST_DIR}/PythonQtMethodInfo.h   
	${CMAKE_CURRENT_LIST_DIR}/PythonQtImportFileInterface.h 
	${CMAKE_CURRENT_LIST_DIR}/PythonQtConversion.h       
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSignalReceiver.h   
	${CMAKE_CURRENT_LIST_DIR}/PythonQtInstanceWrapper.h  
	${CMAKE_CURRENT_LIST_DIR}/PythonQtClassWrapper.h 
	${CMAKE_CURRENT_LIST_DIR}/PythonQtCppWrapperFactory.h
	${CMAKE_CURRENT_LIST_DIR}/PythonQtQFileImporter.h    
	${CMAKE_CURRENT_LIST_DIR}/PythonQtQFileImporter.h    
	${CMAKE_CURRENT_LIST_DIR}/PythonQtVariants.h         
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSystem.h 
	${CMAKE_CURRENT_LIST_DIR}/PythonQtUtils.h 
	${CMAKE_CURRENT_LIST_DIR}/PythonQtBoolResult.h
)

set (GUI_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/gui/PythonQtScriptingConsole.h  
)

set (MAIN_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/PythonQtStdDecorators.cpp
	${CMAKE_CURRENT_LIST_DIR}/PythonQt.cpp             
	${CMAKE_CURRENT_LIST_DIR}/PythonQtClassInfo.cpp    
	${CMAKE_CURRENT_LIST_DIR}/PythonQtImporter.cpp     
	${CMAKE_CURRENT_LIST_DIR}/PythonQtObjectPtr.cpp    
	${CMAKE_CURRENT_LIST_DIR}/PythonQtProperty.cpp     
	${CMAKE_CURRENT_LIST_DIR}/PythonQtStdIn.cpp        
	${CMAKE_CURRENT_LIST_DIR}/PythonQtStdOut.cpp       
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSignal.cpp       
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSlot.cpp         
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSlotDecorator.cpp
	${CMAKE_CURRENT_LIST_DIR}/PythonQtMisc.cpp         
	${CMAKE_CURRENT_LIST_DIR}/PythonQtMethodInfo.cpp   
	${CMAKE_CURRENT_LIST_DIR}/PythonQtConversion.cpp   
	${CMAKE_CURRENT_LIST_DIR}/PythonQtSignalReceiver.cpp
	${CMAKE_CURRENT_LIST_DIR}/PythonQtInstanceWrapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/PythonQtQFileImporter.cpp  
	${CMAKE_CURRENT_LIST_DIR}/PythonQtClassWrapper.cpp   
	${CMAKE_CURRENT_LIST_DIR}/PythonQtBoolResult.cpp     
)		

set (GUI_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/gui/PythonQtScriptingConsole.cpp
)

source_group( "src\\Sources" FILES ${MAIN_SOURCES} )
source_group( "src\\Headers" FILES ${MAIN_HEADERS} )
source_group( "src\\Sources\\gui" FILES ${GUI_SOURCES} )
source_group( "src\\Headers\\gui" FILES ${GUI_HEADERS} )



