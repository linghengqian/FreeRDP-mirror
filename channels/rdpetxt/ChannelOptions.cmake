set(OPTION_DEFAULT OFF)
set(OPTION_CLIENT_DEFAULT OFF)
set(OPTION_SERVER_DEFAULT ON)

define_channel_options(NAME "rdpetxt" TYPE "dynamic"
	DESCRIPTION "Text Input Virtual Channel Extension"
	SPECIFICATIONS "[MS-RDPETXT]"
	DEFAULT ${OPTION_DEFAULT})

define_channel_client_options(${OPTION_CLIENT_DEFAULT})
define_channel_server_options(${OPTION_SERVER_DEFAULT})
