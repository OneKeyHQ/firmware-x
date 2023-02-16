#ifndef ONEKEY_USB_H
#define ONEKEY_USB_H

#include <cstdint>
#include <cstring>

// #include <array>
// #include <iostream>
// #include <string>

#include <linux/usb/ch9.h>
#include <linux/usb/functionfs.h>

#include <usbg/function/hid.h>
#include <usbg/usbg.h>

#define RAC_CHECK_LOGIC                                                          \
  if ( ret != USBG_SUCCESS )                                                     \
  {                                                                              \
    usbg_error uerr = static_cast<usbg_error>(ret);                              \
    bzh_utils::log::StdLog::Error("CALL -> ", func_call_str);                                    \
    bzh_utils::log::StdLog::Error("RET  -> ", usbg_error_name(uerr), ": ", usbg_strerror(uerr)); \
    if ( s != nullptr )                                                          \
    {                                                                            \
      usbg_cleanup(s);                                                           \
    }                                                                            \
    return false;                                                                \
  }
#include "RunAndCheck.hpp"

#include "StdLog.hpp"
#include "misc_macros.h"

#include "onekey_usb_common.hpp"

class ONEKEY_USB_FIRMWARE
{
public:
    bool is_inited = false;

private:
    usbg_state* s = nullptr;
    usbg_gadget* g = nullptr;
    usbg_config* c = nullptr;
    usbg_function* f_hid = nullptr;
    int ret = -EINVAL;
    int usbg_ret;

public:
    static constexpr struct usbg_gadget_attrs g_attrs = {
        .bcdUSB = 0x0200,
        .bDeviceClass = 0x00, // USB_CLASS_PER_INTERFACE
        .bDeviceSubClass = 0x00,
        .bDeviceProtocol = 0x00,
        .bMaxPacketSize0 = 64, /* Max allowed ep0 packet size */
        .idVendor = 0x1209,
        .idProduct = 0x53C1,
        .bcdDevice = 0x0200, /* Verson of device */
    };

    static constexpr struct usbg_gadget_strs g_strs = {
        .manufacturer = (char*)"OneKey",             /* Manufacturer */
        .product = (char*)"OneKey",                  /* Product string */
        .serial = (char*)"71CD44D932F174C92E5530FA", /* Serial number */
    };

    static constexpr struct usbg_config_strs c_strs = {
        // .configuration = (char *)"1xFFS",
        .configuration = (char*)"1xHID",
    };

    static constexpr struct usbg_config_attrs c_attrs = {
        .bmAttributes = 0x80, // bus powered
        .bMaxPower = 0xFA,    // 500ma
    };

    // this dummy is only for compute the size of the array
    // we could not use it since c++ not allow non-const item outside a function
    // and the api only take non-const array
    // I know this is ugly, but there is no better workarounds, until we rewrite the libusbgx in C++
    static constexpr char dummy[] = {TUD_HID_REPORT_DESC_FIDO_U2F(64)};
    char report_desc[sizeof(dummy)];

    struct usbg_f_hid_attrs f_attrs = {
        .protocol = 0,
        .report_desc =
            {
                .desc = report_desc,
                .len = sizeof(report_desc),
            },
        .report_length = 64,
        .subclass = 0,
    };

    ONEKEY_USB_FIRMWARE()
    {
        // another part of the workaround
        char temp_report_desc[] = {
            TUD_HID_REPORT_DESC_FIDO_U2F(64),
        };
        memcpy(report_desc, temp_report_desc, sizeof(report_desc));

        // init();
    }

    ~ONEKEY_USB_FIRMWARE()
    {
        cleanup();
    }

    bool init()
    {
        RAC_CUSTOM(usbg_init("/sys/kernel/config", &s), RAC_CHECK_LOGIC);
        RAC_CUSTOM(usbg_create_gadget(s, "onekeyusb", &g_attrs, &g_strs, &g), RAC_CHECK_LOGIC);
        RAC_CUSTOM(usbg_create_function(g, USBG_F_HID, "usb0", &f_attrs, &f_hid), RAC_CHECK_LOGIC);
        RAC_CUSTOM(usbg_create_config(g, 1, "The only one", NULL, &c_strs, &c), RAC_CHECK_LOGIC);
        RAC_CUSTOM(usbg_add_config_function(c, "some_name", f_hid), RAC_CHECK_LOGIC);
        RAC_CUSTOM(usbg_enable_gadget(g, DEFAULT_UDC), RAC_CHECK_LOGIC);

        return true;
    }

    bool cleanup()
    {
        if ( is_inited )
        {
            usbg_cleanup(s);
            is_inited = false;
        }
        return true;
    }
};

class ONEKEY_USB_BOOTLOADER
{
    // static void usb_init_all(secbool usb21_landing)
    // {
    //     usb_dev_info_t dev_info = {
    //         .device_class = 0x00,
    //         .device_subclass = 0x00,
    //         .device_protocol = 0x00,
    //         .vendor_id = 0x1209,
    //         .product_id = 0x53C0,
    //         .release_num = 0x0200,
    //         .manufacturer = "OneKey",
    //         .product = "ONEKEY",
    //         .serial_number = "000000000000000000000000",
    //         .interface = "ONEKEY Interface",
    //         .usb21_enabled = sectrue,
    //         .usb21_landing = usb21_landing,
    //     };

    //     static uint8_t rx_buffer[USB_PACKET_SIZE];

    //     static const usb_webusb_info_t webusb_info = {
    //         .iface_num = USB_IFACE_NUM,
    //         .ep_in = USB_EP_DIR_IN | 0x01,
    //         .ep_out = USB_EP_DIR_OUT | 0x01,
    //         .subclass = 0,
    //         .protocol = 0,
    //         .max_packet_len = sizeof(rx_buffer),
    //         .rx_buffer = rx_buffer,
    //         .polling_interval = 1,
    //     };

    //     usb_init(&dev_info);

    //     ensure(usb_webusb_add(&webusb_info), NULL);

    //     usb_start();
    // }

public:
public:
    ONEKEY_USB_BOOTLOADER()
    {
    }
    ~ONEKEY_USB_BOOTLOADER()
    {
    }
};

#endif // ONEKEY_USB_H
