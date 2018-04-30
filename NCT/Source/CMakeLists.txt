# Source files and their filters
include(CMakeSources.cmake)

find_package(libzmq REQUIRED)
find_package(protobuf REQUIRED)


# Target
add_library(NetChannelTranition SHARED ${NET_CHANNEL_TRANITION_SRC})

# Includes
target_include_directories(NetChannelTranition PRIVATE "./")

# Defines
target_compile_definitions(NetChannelTranition PRIVATE NCT_EXPORTS)


target_link_libraries(NetChannelTranition PRIVATE ${libzmq_LIBRARIES})	
target_link_libraries(NetChannelTranition PRIVATE ${protobuf_LIBRARIES})

# IDE specific
set_property(TARGET NetChannelTranition PROPERTY FOLDER NetChannelTranition)

# Install
install(
	TARGETS NetChannelTranition
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
	
if(WIN32)
	install(
		FILES $<TARGET_PDB_FILE:NetChannelTranition> 
		DESTINATION bin 
		OPTIONAL
	)
endif()