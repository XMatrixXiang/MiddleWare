# Find protobuf dependency
#
# This module defines
#  PROTOBUF_INCLUDE_DIRS
#  PROTOBUF_LIBRARIES
#  PROTOBUF_FOUND

start_find_package(protobuf)

set(protobuf_INSTALL_DIR ${SOURCE_DIR}/../Dependencies/protobuf CACHE PATH "")
gen_default_lib_search_dirs(protobuf)

if(WIN32)
    set(protobuf_LIBNAME libprotobuf)
else()
    set(protobuf_LIBNAME libprotobuf)
endif()

find_imported_includes(protobuf google/protobuf/any.h)
find_imported_library(protobuf ${protobuf_LIBNAME})

end_find_package(protobuf ${protobuf_LIBNAME})

