
set(OPTION_DEFAULT OFF)
set(OPTION_CLIENT_DEFAULT ON)
set(OPTION_SERVER_DEFAULT OFF)

if(WIN32)
	set(OPTION_CLIENT_DEFAULT OFF)
	set(OPTION_SERVER_DEFAULT OFF)
endif()

if(${OPTION_CLIENT_DEFAULT} OR ${OPTION_SERVER_DEFAULT})
	set(OPTION_DEFAULT ON)
endif()

define_channel_options(NAME "parallel" TYPE "device"
	DESCRIPTION "Parallel Port Virtual Channel Extension"
	SPECIFICATIONS "[MS-RDPESP]"
	DEFAULT ${OPTION_DEFAULT})

define_channel_client_options(${OPTION_CLIENT_DEFAULT})
define_channel_server_options(${OPTION_SERVER_DEFAULT})

