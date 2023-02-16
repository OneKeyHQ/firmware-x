#ifndef ONEKEY_USB_COMMON_HPP
#define ONEKEY_USB_COMMON_HPP

// C headers
#include <cstdint>
#include <cstring>

// C++ headers
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>
#include <vector>
// #include <iostream>
// #include <string>
using namespace std::chrono_literals;

// Project Headers
#include "RunAndCheck.hpp"
#include "StdLog.hpp"
#include "ContainerUtils.hpp"

// Macros

#define INTERFACE_STR        "OneKeyUSB"
#define USB_UDC_DEVICE       "fcc00000.dwc3"
#define USB_CONFIGFS_PATH    "/sys/kernel/config"
#define USB_GADGET_NAME      "onekeyusb"
#define USB_FFS_INSTANT_NAME "usb0"
#define USB_FFS_PATH         "/dev/onekeyusb"
#define USB_FFS_EP(ep)       USB_FFS_PATH "/" #ep
#define USB_FFS_CTRL         USB_FFS_EP(ep0)
#define USB_FFS_IN           USB_FFS_EP(ep1)
#define USB_FFS_OUT          USB_FFS_EP(ep2)

namespace onekey
{
    namespace usb
    {
        // namespace utils
        // {

        // } // namespace utils

    } // namespace usb

} // namespace onekey

#endif // ONEKEY_USB_COMMON_HPP
