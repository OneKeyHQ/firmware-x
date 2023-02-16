cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_RELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_path(
        KERNELHEADERS_DIR
        include/linux/user.h
        PATHS /usr/src/linux-headers-${KERNEL_RELEASE}
)

set(KERNELHEADERS_INCLUDE ${KERNELHEADERS_DIR}/include)