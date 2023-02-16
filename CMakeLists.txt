# cmake settings
cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/aarch64-gnu.toolchain.cmake)
include(${CMAKE_TOOLCHAIN_FILE})

# project
set(PROJ_NAME linux_usb_gadget_pg_pure_cpp)
project(${PROJ_NAME} LANGUAGES CXX)

# source list
set(SOURCE_LIST
    onekey_protocol.hpp
    onekey_usb_common.hpp
    onekey_usb_configfs.hpp
    onekey_usb_functionfs.hpp
    onekey_usb_util.hpp
    onekey_usb_bootloader_mode.hpp
    onekey_usb_firmware_mode.hpp
    onekey_usb.hpp
    main.cpp
)
add_executable(${PROJ_NAME} ${SOURCE_LIST})

# static link
set_target_properties(${PROJ_NAME} PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(${PROJ_NAME} PROPERTIES LINK_SEARCH_END_STATIC 1)
target_link_libraries(${PROJ_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# libraries

# --> LibBzhUtils
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/modules/libBzhUtils/include)

# ---> Linux Header
# include(LinuxKernelHeaders)
# target_include_directories(${PROJ_NAME} PRIVATE ${KERNELHEADERS_INCLUDE})
# target_include_directories(${PROJ_NAME} PRIVATE ${KERNELHEADERS_DIR}/arch/arm64/include)

# ---> LibUsbgx
# note: the build script generates both dynamic and static library
# but LibUsbgxConfig.cmake only gives dynamic somehow
# so we need to patch it
set(LIBUSBGX_INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR}/modules/libusbgx/install)
list(APPEND CMAKE_PREFIX_PATH ${LIBUSBGX_INSTALL_DIR})
find_package(LibUsbgx NO_CMAKE_FIND_ROOT_PATH REQUIRED)
target_include_directories(${PROJ_NAME} PRIVATE LibUsbgx::LibUsbgx)
get_target_property(LibUsbgx_LOC LibUsbgx::LibUsbgx IMPORTED_LOCATION)
string(REGEX REPLACE "\.so$" "\.a" LibUsbgx_STATIC_LIBRARY ${LibUsbgx_LOC})
set_target_properties(LibUsbgx::LibUsbgx PROPERTIES IMPORTED_LOCATION ${LibUsbgx_STATIC_LIBRARY})
target_link_libraries(${PROJ_NAME} PRIVATE LibUsbgx::LibUsbgx)

# pkgconfig
find_package(PkgConfig REQUIRED)

# ---> LibConfig
pkg_check_modules(libconfig REQUIRED libconfig)
target_include_directories(${PROJ_NAME} PUBLIC ${libconfig_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PRIVATE ${libconfig_LIBRARIES})

# ---> LibConfig++
pkg_check_modules(libconfigpp REQUIRED libconfig++)
target_include_directories(${PROJ_NAME} PUBLIC ${libconfigpp_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PRIVATE ${libconfigpp_LIBRARIES})

# ---> LibAIO
# find_library(libaio_LIBRARIES aio REQUIRED)
set(libaio_LIBRARIES -laio)
target_link_libraries(${PROJ_NAME} PRIVATE ${libaio_LIBRARIES})

# ---> ProtoBuf
# include(FindProtobuf)
set(Protobuf_USE_STATIC_LIBS ON)
set(Protobuf_INCLUDE_DIR /usr/include)
find_package(Protobuf REQUIRED)
target_include_directories(${PROJ_NAME} PRIVATE ${Protobuf_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} PRIVATE ${Protobuf_LIBRARY})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Protobuf Generate
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS protobuf/messages.proto)
target_sources(${PROJ_NAME} PRIVATE ${PROTO_SRCS} ${PROTO_HDRS})

# install
install(TARGETS ${PROJ_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
