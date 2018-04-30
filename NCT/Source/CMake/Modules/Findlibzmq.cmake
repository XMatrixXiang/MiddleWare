# Find libzmq dependency
#
# This module defines
#  LIBZMQ_INCLUDE_DIRS
#  LIBZMQ_LIBRARIES
#  LIBZMQ_FOUND

start_find_package(libzmq)

set(libzmq_INSTALL_DIR ${SOURCE_DIR}/../Dependencies/libzmq CACHE PATH "")
gen_default_lib_search_dirs(libzmq)

if(WIN32)
    set(libzmq_LIBNAME libzmq-v140-mt-s-4_2_4)
else()
    set(libzmq_LIBNAME libzmq-v140-mt-s-4_2_4)
endif()

find_imported_includes(libzmq zmq.h)
find_imported_library(libzmq ${libzmq_LIBNAME})

end_find_package(libzmq ${libzmq_LIBNAME})

