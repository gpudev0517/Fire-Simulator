set ( PLUGINEXT ".so" )

if( WIN32 AND MSVC )
set ( PLUGINEXT ".dll" )
elseif( APPLE )
set ( PLUGINEXT ".dylib" )
endif()

MACRO(pluginCopyDir DPATH SPATH TARGET_NAME)

if( WIN32 AND MSVC )

    if( ${DEBUG_BUILD} )
        set ( PPATH ${SPATH}/Data/Plugins/Debug/${DPATH}/${TARGET_NAME}${PLUGINEXT} )
        string( REPLACE "/" "\\" RPATH ${PPATH})
        add_custom_command(TARGET ${TARGET_NAME}  POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${RPATH}
            COMMENT "Copying ${TARGET_NAME} to ${RPATH}."
        )
    else()
        #set ( LPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${TARGET_NAME}${PLUGINEXT} )
        set ( PPATH ${SPATH}/Data/Plugins/Release/${DPATH}/${TARGET_NAME}${PLUGINEXT} )
        string( REPLACE "/" "\\" RPATH ${PPATH})
        # add_custom_target(CopyPlugins_${TARGET_NAME} 
        #     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${RPATH}
        #     DEPENDS ${TARGET_NAME})
        add_custom_command(TARGET ${TARGET_NAME}  POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${RPATH}
            COMMENT "Copying ${TARGET_NAME} to ${RPATH}."
        )
    endif()

endif()
ENDMACRO(pluginCopyDir)



# # Copies the given plugin to the destination directory at post-link
# defineTest(pluginCopyDir) {
    # DPATH = $$1
    # SPATH = $$2
    # PPATH = $${DESTDIR}
    # CONFIG(release, debug|release) {
        # QMAKE_LIBDIR += ../../Build$$QT_VERSION/Release
        # DPATH = $$SPATH/Data/Plugins/Release/$$DPATH
    # } else {
        # QMAKE_LIBDIR += ../../Build$$QT_VERSION/Debug
        # DPATH = $$SPATH/Data/Plugins/Debug/$$DPATH
    # }
    # unix {
        # PPATH = $${PPATH}/lib$${TARGET}$${PLUGINEXT}
    # }
    # win32 {
        # PPATH = $${PPATH}/$${TARGET}$${PLUGINEXT}
    # }
    # # Replace slashes in paths with backslashes for Windows
    # win32:DPATH ~= s,/,\\,g
    # win32:PPATH ~= s,/,\\,g

    # QMAKE_POST_LINK += $$sprintf($$QMAKE_MKDIR_CMD, $$DPATH) '"'$${DPATH}'"' $$escape_expand(\\n\\t)
    # QMAKE_POST_LINK += $$QMAKE_COPY '"'$${PPATH}'"' '"'$${DPATH}'"' $$escape_expand(\\n\\t)

    # export(QMAKE_POST_LINK)
    # export(QMAKE_LIBDIR)
# }


if( UNIX AND NOT APPLE )
# INCLUDEPATH +=\
    # $${ROOTPATH}/Code \
    # $${ROOTPATH}/External \
    # $${ROOTPATH}/External/Linux/include \
    # ../../
elseif(APPLE)
# INCLUDEPATH +=\
    # $${ROOTPATH}/Code \
    # $${ROOTPATH}/External \
    # $${ROOTPATH}/External/Mac/include \
    # ../../
elseif( WIN32 AND MSVC )
include_directories (
    ./ 
    ${ROOTPATH}/Code 
    ${ROOTPATH}/Code/Core 
    ${ROOTPATH}/External 
    ${ROOTPATH}/External/include 
    ${ROOTPATH}/External/Windows/include/ 
    ../../
)
endif()
