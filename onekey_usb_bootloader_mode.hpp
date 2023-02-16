#ifndef ONEKEY_USB_BOOTLOADER_MODE_HPP
#define ONEKEY_USB_BOOTLOADER_MODE_HPP

#include "onekey_protocol.hpp"
#include "onekey_usb_common.hpp"
#include "onekey_usb_configfs.hpp"
#include "onekey_usb_functionfs.hpp"

#define RAC_ON_FALSE_BOOTLOADER \
  {                             \
    RAC_ON_FALSE                \
    return false;               \
  }

namespace onekey
{
    namespace usb
    {
        namespace bootloader
        {
            // configfs

            static const struct usbg_gadget_attrs g_attrs = {
                .bcdUSB = 0x0210,
                .bDeviceClass = 0x00, // USB_CLASS_PER_INTERFACE
                .bDeviceSubClass = 0x00,
                .bDeviceProtocol = 0x00,
                .bMaxPacketSize0 = 64, /* Max allowed ep0 packet size */
                .idVendor = 0x1209,
                .idProduct = 0x53C0,
                .bcdDevice = 0x0200, /* Verson of device */
            };

            static const struct usbg_gadget_strs g_strs = {
                .manufacturer = (char*)"OneKey",             /* Manufacturer */
                .product = (char*)"OneKey",                  /* Product string */
                .serial = (char*)"000000000000000000000000", /* Serial number */
            };

            static const struct usbg_config_strs c_strs = {
                .configuration = (char*)"OneKeyUSB",
            };

            static const struct usbg_config_attrs c_attrs = {
                .bmAttributes = 0x80,                      // bus powered
                .bMaxPower = CONFIG_DESC_MAXPOWER_mA(500), // 500ma
            };

            // functionfs

            static const struct functionfs::func_desc fs_descriptors = {
                .intf =
                    {
                        .bLength = sizeof(fs_descriptors.intf),
                        .bDescriptorType = USB_DT_INTERFACE,
                        .bInterfaceNumber = 0,
                        .bNumEndpoints = 2,
                        .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
                        .bInterfaceSubClass = 0,
                        .bInterfaceProtocol = 0,
                        .iInterface = 1, /* first string from the provided table */
                    },
                .source =
                    {
                        .bLength = sizeof(fs_descriptors.source),
                        .bDescriptorType = USB_DT_ENDPOINT,
                        .bEndpointAddress = 1 | USB_DIR_IN,
                        .bmAttributes = USB_ENDPOINT_XFER_INT,
                        .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_FS),
                        .bInterval = 1,
                    },
                .sink =
                    {
                        .bLength = sizeof(fs_descriptors.sink),
                        .bDescriptorType = USB_DT_ENDPOINT,
                        .bEndpointAddress = 1 | USB_DIR_OUT,
                        .bmAttributes = USB_ENDPOINT_XFER_INT,
                        .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_FS),
                        .bInterval = 1,
                    },
            };

            static const struct functionfs::func_desc hs_descriptors = {
                .intf =
                    {
                        .bLength = sizeof(hs_descriptors.intf),
                        .bDescriptorType = USB_DT_INTERFACE,
                        .bInterfaceNumber = 0,
                        .bNumEndpoints = 2,
                        .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
                        .bInterfaceSubClass = 0,
                        .bInterfaceProtocol = 0,
                        .iInterface = 1, /* first string from the provided table */
                    },
                .source =
                    {
                        .bLength = sizeof(hs_descriptors.source),
                        .bDescriptorType = USB_DT_ENDPOINT,
                        .bEndpointAddress = 1 | USB_DIR_IN,
                        .bmAttributes = USB_ENDPOINT_XFER_INT,
                        .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_FS),
                        // .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_HS),
                        .bInterval = 1,
                    },
                .sink =
                    {
                        .bLength = sizeof(hs_descriptors.sink),
                        .bDescriptorType = USB_DT_ENDPOINT,
                        .bEndpointAddress = 1 | USB_DIR_OUT,
                        .bmAttributes = USB_ENDPOINT_XFER_INT,
                        .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_FS),
                        // .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_HS),
                        .bInterval = 1,
                    },
            };

            static const struct functionfs::ss_func_desc ss_descriptors = {
                .intf =
                    {
                        .bLength = sizeof(ss_descriptors.intf),
                        .bDescriptorType = USB_DT_INTERFACE,
                        .bInterfaceNumber = 0,
                        .bNumEndpoints = 2,
                        .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
                        .bInterfaceSubClass = 0,
                        .bInterfaceProtocol = 0,
                        .iInterface = 1, /* first string from the provided table */
                    },
                .source =
                    {
                        .bLength = sizeof(ss_descriptors.source),
                        .bDescriptorType = USB_DT_ENDPOINT,
                        .bEndpointAddress = 1 | USB_DIR_OUT,
                        .bmAttributes = USB_ENDPOINT_XFER_BULK,
                        .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_SS),
                    },
                .source_comp =
                    {
                        .bLength = sizeof(ss_descriptors.source_comp),
                        .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
                        .bMaxBurst = 4,
                    },
                .sink =
                    {
                        .bLength = sizeof(ss_descriptors.sink),
                        .bDescriptorType = USB_DT_ENDPOINT,
                        .bEndpointAddress = 2 | USB_DIR_IN,
                        .bmAttributes = USB_ENDPOINT_XFER_BULK,
                        .wMaxPacketSize = cpu_to_le16(functionfs::MAX_PACKET_SIZE_SS),
                    },
                .sink_comp =
                    {
                        .bLength = sizeof(ss_descriptors.sink_comp),
                        .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
                        .bMaxBurst = 4,
                    },
            };

            static const struct usb_ext_compat_desc os_desc_compat = {
                .bFirstInterfaceNumber = 0,
                .Reserved1 = cpu_to_le32(1),
                .CompatibleID = {0},
                .SubCompatibleID = {0},
                .Reserved2 = {0},
            };

            static const struct usb_os_desc_header os_desc_header = {
                .interface = cpu_to_le32(1),
                .dwLength = cpu_to_le32(sizeof(os_desc_header) + sizeof(os_desc_compat)),
                .bcdVersion = cpu_to_le32(1),
                .wIndex = cpu_to_le32(4),
                .bCount = cpu_to_le32(1),
                .Reserved = cpu_to_le32(0),
            };

            static const struct functionfs::ffs_strings strings = {
                .header =
                    {
                        .magic = cpu_to_le32(FUNCTIONFS_STRINGS_MAGIC),
                        .length = cpu_to_le32(sizeof(strings)),
                        .str_count = cpu_to_le32(1),
                        .lang_count = cpu_to_le32(1),
                    },
                .lang1 =
                    {
                        .code = cpu_to_le16(0x0409), /* en-us */
                        .str1 = INTERFACE_STR,
                    },
            };

            // class

            class BOOTLOADER
            {
            public:
                // instances
                configfs_handle cfs_h;
                functionfs::handler ffs_h;
                protocol::parser parser_h;
                protocol::composer composer_h;
                time_t usb_timeout_ms = 1000;

                BOOTLOADER()
                {
                }
                ~BOOTLOADER()
                {
                }

                bool init()
                {
                    AUTOLOG_FUNC_ONLY;
                    cfs_h.g_attrs = g_attrs;
                    cfs_h.g_strs = g_strs;
                    cfs_h.c_attrs = c_attrs;
                    cfs_h.c_strs = c_strs;

                    ffs_h.fs_descriptors = fs_descriptors;
                    ffs_h.hs_descriptors = hs_descriptors;
                    // ffs_h.ss_descriptors = ss_descriptors;
                    ffs_h.os_desc_compat = os_desc_compat;
                    ffs_h.os_desc_header = os_desc_header;
                    ffs_h.strings = strings;
                    ffs_h.aio_set_timeout(8000);

                    RAC_ADVANCE(cfs_h.init(), (RAC_ret == true), RAC_ON_FALSE_BOOTLOADER);
                    RAC_ADVANCE(ffs_h.init(), (RAC_ret == true), RAC_ON_FALSE_BOOTLOADER);
                    RAC_ADVANCE(cfs_h.enable(), (RAC_ret == true), RAC_ON_FALSE_BOOTLOADER);
                    return true;
                }

                bool cleanup()
                {
                    AUTOLOG_FUNC_ONLY;
                    RAC_ADVANCE(cfs_h.disable(), (RAC_ret == true), RAC_ON_FALSE_BOOTLOADER);
                    RAC_ADVANCE(ffs_h.cleanup(), (RAC_ret == true), RAC_ON_FALSE_BOOTLOADER);
                    RAC_ADVANCE(cfs_h.cleanup(), (RAC_ret == true), RAC_ON_FALSE_BOOTLOADER);
                    return true;
                }

                // thread main
                void usb_service_routine(std::atomic_bool &run_control, std::atomic_bool &run_status)
                {
                    AUTOLOG_FUNC_ONLY;
                    // buffer
                    std::array<uint8_t, sizeof(usb_functionfs_event)> buf_ctrl;
                    std::vector<uint8_t> buf_write_host, buf_read_host;
                    buf_write_host.resize(functionfs::USB_FFS_MAX_TRANSFER_SIZE);
                    buf_read_host.resize(functionfs::USB_FFS_MAX_TRANSFER_SIZE);
                    int32_t res = -1;

                    if ( !run_control )
                    {
                        bzh_utils::log::StdLog::Error("service_routine called when run_control is false!");
                        return;
                    }

                    struct usb_functionfs_event* p_event =
                        reinterpret_cast<struct usb_functionfs_event*>(buf_ctrl.data());

                    while ( run_control )
                    {
                        if ( !init() )
                        {
                            run_control = false;
                            break;
                        }

                        int wait_result = -1;
                        uint64_t eventfd_result = -1;
                        while ( run_control )
                        {
                            // set status
                            run_status = true;

                            // wait for control or eventfd have event
                            bzh_utils::log::StdLog::Debug("wait_read_management_ready");
                            wait_result = ffs_h.wait_read_management_ready(usb_timeout_ms);
                            if ( wait_result == 0 )
                            {
                                bzh_utils::log::StdLog::Debug("wait_read_management_ready timeout");
                                continue;
                            }
                            if ( wait_result < 0 )
                            {
                                bzh_utils::log::StdLog::Debug("wait_read_management_ready error");
                                run_control = false;
                                break;
                            }
                            bzh_utils::log::StdLog::Debug(
                                "wait_read_management_ready end -> wait_result = " +
                                std::to_string(wait_result)
                            );

                            // pull control
                            bzh_utils::log::StdLog::Debug("is_read_ctrl_ready");
                            if ( ffs_h.is_read_ctrl_ready() )
                            {
                                bzh_utils::log::StdLog::Debug("is_read_ctrl_ready true");
                                RAC_ADVANCE(
                                    ffs_h.read_ctrl(p_event), (RAC_ret == true),
                                    {
                                        RAC_ON_FALSE
                                        run_control = false;
                                        break;
                                    }

                                );
                                ffs_h.process_control_event(p_event);
                            }

                            if ( !ffs_h.ffs_enabled )
                            {
                                bzh_utils::log::StdLog::Debug("ffs not enabled yet");
                                std::this_thread::sleep_for(500ms);
                                continue;
                            }

                            // check if we got event
                            bzh_utils::log::StdLog::Debug("is_read_eventfd_ready");
                            if ( ffs_h.is_read_eventfd_ready() )
                            {
                                bzh_utils::log::StdLog::Debug("is_read_eventfd_ready true");

                                // read to clear the event
                                RAC_ADVANCE(ffs_h.read_eventfd(&eventfd_result), (RAC_ret), {
                                    RAC_ON_FALSE
                                    run_control = false;
                                    return;
                                });

                                bzh_utils::log::StdLog::Debug("eventfd_result -> ", std::to_string(eventfd_result));

                                // pull host
                                res = -1;

                                RAC_ADVANCE(ffs_h.read_host(buf_read_host, res), (RAC_ret), {
                                    RAC_ON_FALSE
                                    continue;
                                });

                                // process request

                                bzh_utils::log::StdLog::Debug("==== PACKET DUMP buf_read_host ====");
                                bzh_utils::container::print_stl_container(buf_read_host);

                                bzh_utils::log::StdLog::Debug("==== PACKET PARSE ====");
                                RAC_SIMPLE(parser_h.parse_legacy(buf_read_host), (RAC_ret));
                                protocol::message_parsed msg_parsed = parser_h.get_parsed();
                                bzh_utils::log::StdLog::Debug(
                                    "msg_id -> ",
                                    std::string(MessageType_Name(static_cast<MessageType>(msg_parsed.id)))
                                );
                                bzh_utils::log::StdLog::Debug("msg_size -> ", std::to_string(msg_parsed.size));
                                bzh_utils::log::StdLog::Debug("msg_protobuf_data -> ");
                                bzh_utils::container::print_stl_container(msg_parsed.protobuf_data);

                                bzh_utils::log::StdLog::Debug("==== Create Features ====");
                                Features feature;
                                feature.set_vendor("onekey.so");
                                feature.set_major_version(4);
                                feature.set_minor_version(0);
                                feature.set_patch_version(0);
                                feature.set_bootloader_mode(true);
                                feature.set_model("T");
                                feature.set_firmware_present(false);
                                feature.set_ble_name("FFFFF");
                                feature.set_ble_ver("1.2.3");
                                feature.set_ble_enable(true);
                                feature.set_se_ver("1.0.0");
                                feature.set_serial_no("TC01WBD202210170824500000067");

                                bzh_utils::log::StdLog::Debug("==== Get Serialized Features ====");
                                std::string feature_serialized_string = feature.SerializeAsString();
                                std::vector<uint8_t> feature_serialized = std::vector<uint8_t>(
                                    feature_serialized_string.begin(), feature_serialized_string.end()
                                );
                                bzh_utils::log::StdLog::Debug(
                                    "feature_serialized_string.size() -> ",
                                    std::to_string(feature_serialized_string.size())
                                );

                                bzh_utils::log::StdLog::Debug("==== PACKET DUMP Features ====");
                                bzh_utils::container::print_stl_container(feature_serialized);
                                bzh_utils::log::StdLog::Debug(
                                    "feature_serialized.size() -> ", std::to_string(feature_serialized.size())
                                );

                                protocol::message_parsed mpsed;

                                for ( uint8_t element : feature_serialized )
                                {
                                    mpsed.protobuf_data.push_back(reinterpret_cast<uint8_t>(element));
                                }
                                mpsed.size = mpsed.protobuf_data.size();
                                mpsed.id = static_cast<uint16_t>(MessageType_Features);
                                bzh_utils::log::StdLog::Debug(
                                    "mpsed.protobuf_data.size() -> ",
                                    std::to_string(mpsed.protobuf_data.size())
                                );
                                bzh_utils::log::StdLog::Debug("mpsed.size -> ", std::to_string(mpsed.size));

                                RAC_SIMPLE(composer_h.compose_legacy(mpsed), (RAC_ret));
                                std::vector<protocol::message_composed> mcomposed_list =
                                    composer_h.get_composed();

                                bzh_utils::log::StdLog::Debug(
                                    "mcomposed_list.size() -> ", std::to_string(mcomposed_list.size())
                                );

                                // put host
                                for ( auto mcomposed : mcomposed_list )
                                {
                                    bzh_utils::log::StdLog::Debug("mcomposed.size() -> ", std::to_string(mcomposed.size()));
                                    bzh_utils::log::StdLog::Debug("==== PACKET DUMP buf_write_host ====");
                                    bzh_utils::container::print_stl_container(mcomposed);
                                    ffs_h.write_host(mcomposed);
                                }

                                // ffs_h.write_host(buf_write_host);
                            }
                            // if not, we continue
                            else
                            {
                                continue;
                            }
                        }
                        bzh_utils::log::StdLog::Debug("loop out");

                        if ( !cleanup() )
                        {
                            run_control = false;
                            break;
                        } // I know it's not needed, just for keep the style
                    }

                    run_status = false;

                    return;
                }
            };

        } // namespace bootloader

    } // namespace usb

} // namespace onekey

#endif // ONEKEY_USB_BOOTLOADER_MODE_HPP
