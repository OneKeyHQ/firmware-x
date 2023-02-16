#ifndef ONEKEY_USB_CONFIGFS_HPP
#define ONEKEY_USB_CONFIGFS_HPP

#include <usbg/function/hid.h>
#include <usbg/usbg.h>

#include "onekey_usb_common.hpp"
#include "onekey_usb_util.hpp"

#define RAC_ON_FALSE_USBGX                                                                         \
  {                                                                                                \
    RAC_ON_FALSE                                                                                   \
    std::cout << "USBGX_ERR  -> " << usbg_strerror(static_cast<usbg_error>(RAC_ret)) << std::endl; \
    if ( s != nullptr )                                                                            \
    {                                                                                              \
      usbg_cleanup(s);                                                                             \
    }                                                                                              \
    return false;                                                                                  \
  }

namespace onekey
{
    namespace usb
    {
        namespace configfs
        {
            // not used for now, since putting everything inside just add more references layers
        }

        class configfs_handle
        {
        public:
            struct usbg_gadget_attrs g_attrs = {0};
            struct usbg_gadget_strs g_strs = {0};
            struct usbg_config_strs c_strs = {0};
            struct usbg_config_attrs c_attrs = {0};

            usbg_state* s = nullptr;
            usbg_udc* u = nullptr;
            usbg_gadget* g = nullptr;
            usbg_config* c = nullptr;
            usbg_function* f_ffs_data = nullptr;

            configfs_handle()
            {
            }
            ~configfs_handle()
            {
            }

            bool init()
            {
                RAC_ADVANCE(usbg_init(USB_CONFIGFS_PATH, &s), (RAC_ret == USBG_SUCCESS), RAC_ON_FALSE_USBGX);
                RAC_ADVANCE(
                    usbg_create_gadget(s, USB_GADGET_NAME, &g_attrs, &g_strs, &g), (RAC_ret == USBG_SUCCESS),
                    RAC_ON_FALSE_USBGX
                );
                RAC_ADVANCE(
                    usbg_create_function(g, USBG_F_FFS, USB_FFS_INSTANT_NAME, NULL, &f_ffs_data),
                    (RAC_ret == USBG_SUCCESS), RAC_ON_FALSE_USBGX
                );
                RAC_ADVANCE(
                    usbg_create_config(g, 1, NULL, &c_attrs, &c_strs, &c), (RAC_ret == USBG_SUCCESS),
                    RAC_ON_FALSE_USBGX
                );
                RAC_ADVANCE(
                    usbg_add_config_function(c, "data", f_ffs_data), (RAC_ret == USBG_SUCCESS),
                    RAC_ON_FALSE_USBGX
                );

                u = usbg_get_udc(s, USB_UDC_DEVICE);
                if ( u == NULL )
                {
                    bzh_utils::log::StdLog::Error("CALL -> ", usbg_get_udc(s, USB_UDC_DEVICE));
                    bzh_utils::log::StdLog::Error("RET  -> ", "NULL");
                    if ( s != nullptr )
                    {
                        usbg_cleanup(s);
                    }
                }
                return true;
            }
            bool cleanup()
            {
                RAC_ADVANCE(
                    usbg_rm_gadget(g, USBG_RM_RECURSE), (RAC_ret == USBG_SUCCESS), RAC_ON_FALSE_USBGX
                );
                if ( s != nullptr )
                {
                    usbg_cleanup(s);
                }
                return true;
            }
            bool enable()
            {
                RAC_ADVANCE(usbg_enable_gadget(g, u), (RAC_ret == USBG_SUCCESS), RAC_ON_FALSE_USBGX);
                return true;
            }
            bool disable()
            {
                RAC_ADVANCE(usbg_disable_gadget(g), (RAC_ret == USBG_SUCCESS), RAC_ON_FALSE_USBGX);
                return true;
            }
        };

    } // namespace usb

} // namespace onekey

#endif // ONEKEY_USB_CONFIGFS_HPP
