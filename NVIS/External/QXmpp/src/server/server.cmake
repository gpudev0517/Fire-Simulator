# Headers
set (QXMPP_SERVER_HEADERS
    server/QXmppDialback.h 
    server/QXmppIncomingClient.h 
    server/QXmppIncomingServer.h 
    server/QXmppOutgoingServer.h 
    server/QXmppPasswordChecker.h 
    server/QXmppServer.h 
    server/QXmppServerExtension.h 
    server/QXmppServerPlugin.h
)

# Source files
set (QXMPP_SERVER_SOURCES
    server/QXmppDialback.cpp 
    server/QXmppIncomingClient.cpp 
    server/QXmppIncomingServer.cpp 
    server/QXmppOutgoingServer.cpp 
    server/QXmppPasswordChecker.cpp 
    server/QXmppServer.cpp 
    server/QXmppServerExtension.cpp
)

source_group( "Headers\\server" FILES ${QXMPP_SERVER_HEADERS} )
source_group( "Sources\\server" FILES ${QXMPP_SERVER_SOURCES} )
