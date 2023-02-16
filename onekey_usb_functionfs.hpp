#ifndef ONEKEY_USB_FUNCTIONFS_HPP
#define ONEKEY_USB_FUNCTIONFS_HPP

#include <algorithm>
#include <array>
#include <cstdint>
// #include <memory>
#include <string>

#include <errno.h>
#include <linux/usb/ch9.h>
#include <linux/usb/functionfs.h>
#include <sys/eventfd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

// #include <linux/io_uring.h>
#include <libaio.h>
#define IOCB_FLAG_RESFD (1 << 0)

// #include "TickTockVars.hpp"

#include "onekey_usb_common.hpp"
#include "onekey_usb_util.hpp"

// seems rk3566 4.19 kernel not support this
// call ffs related ioctl will hang forever
// or ioctl() return -> 4 -> Interrupted system call
#define SOC_SUPPORT_FFS_IOCTL_CALLES 0

#define RAC_ON_FALSE_FFS \
  {                      \
    RAC_ON_FALSE         \
    return false;        \
  }

#define RAC_ON_FALSE_RW                                                                                      \
  {                                                                                                          \
    RAC_ON_FALSE                                                                                             \
    error_msg =                                                                                              \
        "[" + std::string(__func__) + "]" + "return -> " + std::to_string(errno) + " -> " + strerror(errno); \
    return false;                                                                                            \
  }

#define RAC_ON_FALSE_AIO                                                                                     \
  {                                                                                                          \
    RAC_ON_FALSE                                                                                             \
    error_msg =                                                                                              \
        "[" + std::string(__func__) + "]" + "return -> " + std::to_string(errno) + " -> " + strerror(errno); \
    return false;                                                                                            \
  }

namespace onekey
{
    namespace usb
    {
        namespace functionfs
        {
            static const __le16 MAX_PACKET_SIZE_FS = 64;
            static const __le16 MAX_PACKET_SIZE_HS = 3072;
            static const __le16 MAX_PACKET_SIZE_SS = 3072;

            static constexpr size_t MAX_PAYLOAD_V1 = 4 * 1024;
            static constexpr size_t MAX_PAYLOAD_V2 = 256 * 1024;
            static constexpr size_t MAX_PAYLOAD = MAX_PAYLOAD_V2;
            static constexpr size_t USB_FFS_MAX_TRANSFER_SIZE = 64;
            static constexpr size_t USB_FFS_MIN_TIMOUT_MS = 60000;

            struct func_desc
            {
                struct usb_interface_descriptor intf;
                struct usb_endpoint_descriptor_no_audio source;
                struct usb_endpoint_descriptor_no_audio sink;
            } __attribute__((packed));

            struct ss_func_desc
            {
                struct usb_interface_descriptor intf;
                struct usb_endpoint_descriptor_no_audio source;
                struct usb_ss_ep_comp_descriptor source_comp;
                struct usb_endpoint_descriptor_no_audio sink;
                struct usb_ss_ep_comp_descriptor sink_comp;
            } __attribute__((packed));

            struct desc_v1
            {
                struct usb_functionfs_descs_head_v1
                {
                    __le32 magic;
                    __le32 length;
                    __le32 fs_count;
                    __le32 hs_count;
                } __attribute__((packed)) header;
                struct func_desc fs_descs, hs_descs;
            } __attribute__((packed));

            struct desc_v2
            {
                struct usb_functionfs_descs_head_v2 header;
                __le32 fd_eventfd;
                // The rest of the structure depends on the flags in the header.
                // If some desc part not needed, the related count must be zero.
                // The driver parse the struct byte by byte depend on count.
                __le32 fs_count;
                __le32 hs_count;
                __le32 ss_count;
                __le32 os_count;
                struct func_desc fs_descs, hs_descs;
                // struct ss_func_desc ss_descs;
                struct usb_os_desc_header os_header;
                struct usb_ext_compat_desc os_desc;
            } __attribute__((packed));

            struct ffs_strings_lang
            {
                __le16 code;
                char str1[sizeof(INTERFACE_STR)];
            } __attribute__((packed));

            struct ffs_strings
            {
                struct usb_functionfs_strings_head header;
                struct ffs_strings_lang lang1;
            } __attribute__((packed));

            class handler
            {
                // ******************************
                // BASE
                // ******************************
            public:
                // handler
                std::string error_msg = "";
                std::atomic_bool ffs_initialized = false;
                std::atomic_bool ffs_binded = false;
                std::atomic_bool ffs_enabled = false;

                // ffs
                struct func_desc fs_descriptors = {0};
                struct func_desc hs_descriptors = {0};
                struct ss_func_desc ss_descriptors = {0};
                struct usb_ext_compat_desc os_desc_compat = {0};
                struct usb_os_desc_header os_desc_header = {0};
                struct ffs_strings strings = {0};

                struct desc_v2 v2_descriptor;
                int fd_ffs_ep_crtl = -1;
                int fd_ffs_ep_in = -1;
                int fd_ffs_ep_out = -1;
                int fd_eventfd = -1;

                // virtual bool init() = 0;
                // virtual bool cleanup() = 0;
                // virtual bool is_initialized() = 0;

                std::string get_event_name(const struct usb_functionfs_event* event)
                {
                    static const char* const names[] = {
                        [FUNCTIONFS_BIND] = "BIND",     [FUNCTIONFS_UNBIND] = "UNBIND",
                        [FUNCTIONFS_ENABLE] = "ENABLE", [FUNCTIONFS_DISABLE] = "DISABLE",
                        [FUNCTIONFS_SETUP] = "SETUP",   [FUNCTIONFS_SUSPEND] = "SUSPEND",
                        [FUNCTIONFS_RESUME] = "RESUME",
                    };

                    return std::string(names[event->type]);
                }

                void process_control_event(const struct usb_functionfs_event* event)
                {
                    // todo?
                    switch ( event->type )
                    {
                    case FUNCTIONFS_BIND:
                        bzh_utils::log::StdLog::Debug("FUNCTIONFS_BIND");
                        ffs_binded = true;
                        break;
                    case FUNCTIONFS_UNBIND:
                        bzh_utils::log::StdLog::Debug("FUNCTIONFS_UNBIND");
                        ffs_binded = false;
                        break;
                    case FUNCTIONFS_ENABLE:
                        bzh_utils::log::StdLog::Debug("FUNCTIONFS_ENABLE");
                        ffs_enabled = true;
                        break;
                    case FUNCTIONFS_DISABLE:
                        bzh_utils::log::StdLog::Debug("FUNCTIONFS_DISABLE");
                        ffs_enabled = false;
                        break;
                    case FUNCTIONFS_SETUP:
                        bzh_utils::log::StdLog::Debug("FUNCTIONFS_SETUP");
                        break;
                    case FUNCTIONFS_SUSPEND:
                        bzh_utils::log::StdLog::Debug("FUNCTIONFS_SUSPEND");
                        break;
                    case FUNCTIONFS_RESUME:
                        bzh_utils::log::StdLog::Debug("FUNCTIONFS_RESUME");
                        break;
                    default:
                        break;
                    }
                }

                bool ffs_init()
                {
                    // open eventfd
                    RAC_ADVANCE(fd_eventfd = eventfd(0, 0), (RAC_ret >= 0), RAC_ON_FALSE_FFS);

                    // mount ffs
                    if ( std::filesystem::exists(USB_FFS_PATH) )
                    {
                        RAC_ADVANCE(
                            std::filesystem::remove(USB_FFS_PATH), (RAC_ret == true), RAC_ON_FALSE_FFS
                        );
                    }
                    RAC_ADVANCE(
                        std::filesystem::create_directories(USB_FFS_PATH), (RAC_ret == true), RAC_ON_FALSE_FFS
                    );
                    RAC_ADVANCE(
                        mount(USB_FFS_INSTANT_NAME, USB_FFS_PATH, "functionfs", 0, NULL), (RAC_ret == 0),
                        RAC_ON_FALSE_FFS
                    );

                    // fill desc
                    struct desc_v1 v1_descriptor;
                    v1_descriptor.header.magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC);
                    v1_descriptor.header.length = cpu_to_le32(sizeof(v1_descriptor));
                    v1_descriptor.header.fs_count = cpu_to_le32(3);
                    v1_descriptor.header.hs_count = cpu_to_le32(3);
                    v1_descriptor.fs_descs = fs_descriptors;
                    v1_descriptor.hs_descs = hs_descriptors;

                    struct desc_v2 v2_descriptor;
                    v2_descriptor.header.magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC_V2);
                    v2_descriptor.header.length = cpu_to_le32(sizeof(v2_descriptor));

                    v2_descriptor.header.flags = FUNCTIONFS_HAS_FS_DESC | FUNCTIONFS_HAS_HS_DESC |
                                                 FUNCTIONFS_HAS_SS_DESC | FUNCTIONFS_HAS_MS_OS_DESC |
                                                 FUNCTIONFS_EVENTFD;
                    v2_descriptor.fd_eventfd = cpu_to_le32(fd_eventfd);
                    v2_descriptor.fs_count = cpu_to_le32(3);
                    v2_descriptor.hs_count = cpu_to_le32(3);
                    v2_descriptor.ss_count = cpu_to_le32(0); // must be 0 since we do now provide ss_descs
                    v2_descriptor.os_count = cpu_to_le32(1);
                    v2_descriptor.fs_descs = fs_descriptors;
                    v2_descriptor.hs_descs = hs_descriptors;
                    // v2_descriptor.ss_descs = ss_descriptors;
                    v2_descriptor.os_header = os_desc_header;
                    v2_descriptor.os_desc = os_desc_compat;

                    // open control
                    RAC_ADVANCE(
                        fd_ffs_ep_crtl = open(USB_FFS_CTRL, O_RDWR | O_SYNC), (fd_ffs_ep_crtl >= 0),
                        RAC_ON_FALSE_FFS
                    );

                    // write descriptor v1
                    // RAC_SIMPLE(
                    //     TEMP_FAILURE_RETRY(write(fd_ffs_ep_crtl, &v1_descriptor, sizeof(v1_descriptor))),
                    //     (RAC_ret > 0)
                    // );

                    // write descriptor v2
                    RAC_SIMPLE(
                        TEMP_FAILURE_RETRY(write(fd_ffs_ep_crtl, &v2_descriptor, sizeof(v2_descriptor))),
                        (RAC_ret > 0)
                    );

                    // write strings
                    RAC_ADVANCE(
                        TEMP_FAILURE_RETRY(write(fd_ffs_ep_crtl, &strings, sizeof(strings))), (RAC_ret > 0),
                        RAC_ON_FALSE_FFS
                    );

                    // open in
                    RAC_ADVANCE(
                        fd_ffs_ep_in = open(USB_FFS_IN, O_RDWR), (fd_ffs_ep_in >= 0), RAC_ON_FALSE_FFS
                    );

                    // open out
                    RAC_ADVANCE(
                        fd_ffs_ep_out = open(USB_FFS_OUT, O_RDWR), (fd_ffs_ep_out >= 0), RAC_ON_FALSE_FFS
                    );

                    ffs_initialized = true;

                    return true;
                }

                bool ffs_cleanup()
                {
                    if ( fd_ffs_ep_in > 0 )
                    {
                        RAC_ADVANCE(ffs_fifo_flush(fd_ffs_ep_in), (RAC_ret == true), RAC_ON_FALSE_FFS);
                        RAC_ADVANCE(ffs_close_pipe(fd_ffs_ep_in), (RAC_ret == true), RAC_ON_FALSE_FFS);
                        RAC_ADVANCE(close(fd_ffs_ep_in), (RAC_ret == 0), RAC_ON_FALSE_FFS);
                        fd_ffs_ep_in = -1;
                    }
                    if ( fd_ffs_ep_out > 0 )
                    {
                        RAC_ADVANCE(ffs_fifo_flush(fd_ffs_ep_out), (RAC_ret == true), RAC_ON_FALSE_FFS);
                        RAC_ADVANCE(ffs_close_pipe(fd_ffs_ep_out), (RAC_ret == true), RAC_ON_FALSE_FFS);
                        RAC_ADVANCE(close(fd_ffs_ep_out), (RAC_ret == 0), RAC_ON_FALSE_FFS);
                        fd_ffs_ep_out = -1;
                    }
                    if ( fd_ffs_ep_crtl > 0 )
                    {
                        RAC_ADVANCE(close(fd_ffs_ep_crtl), (RAC_ret == 0), RAC_ON_FALSE_FFS);
                        fd_ffs_ep_crtl = -1;
                    }

                    RAC_ADVANCE(umount(USB_FFS_PATH), (RAC_ret == 0), RAC_ON_FALSE_FFS);

                    ffs_initialized = false;

                    return true;
                }

                inline bool is_ffs_enabled()
                {
                    return ffs_enabled;
                }

                inline bool is_ffs_binded()
                {
                    return ffs_binded;
                }

                inline int ffs_check_fifo_available(int ffs_ep)
                {
                    return TEMP_FAILURE_RETRY(ioctl(ffs_ep, FUNCTIONFS_FIFO_STATUS)); // get unclaimed bytes
                }

#if SOC_SUPPORT_FFS_IOCTL_CALLES
                bool ffs_fifo_flush(int ffs_ep)
                {
                    int ret = -1;

                    ret = ffs_check_fifo_available(ffs_ep);
                    if ( ret < 0 )
                    {
                        /* ENODEV reported after disconnect */
                        if ( errno != ENODEV )
                        {
                            error_msg = "[" + std::string(__func__) + "]" + "ioctl() return -> " +
                                        std::to_string(errno) + " -> " + strerror(errno);
                            return false;
                        }
                    }
                    else if ( ret > 0 )
                    {
                        // warn("%s: unclaimed = %d\n", t->filename, ret);
                        ret = TEMP_FAILURE_RETRY(ioctl(ffs_ep, FUNCTIONFS_FIFO_FLUSH));
                        if ( ret < 0 )
                        {
                            error_msg = "[" + std::string(__func__) + "]" + "ioctl() return -> " +
                                        std::to_string(errno) + " -> " + strerror(errno);
                            return false;
                        }
                    }
                    return true;
                }

                bool ffs_close_pipe(int ffs_ep)
                {
                    int ret = -1;

                    ret = TEMP_FAILURE_RETRY(ioctl(ffs_ep, FUNCTIONFS_CLEAR_HALT));
                    if ( ret < 0 )
                    {
                        error_msg = "[" + std::string(__func__) + "]" + "ioctl() return -> " +
                                    std::to_string(errno) + " -> " + strerror(errno);
                        return false;
                    }
                    return true;
                }
#else
                bool ffs_fifo_flush(int ffs_ep)
                {
                    return true;
                }
                bool ffs_close_pipe(int ffs_ep)
                {
                    return true;
                }
#endif

                // ******************************
                // BLOCK / TIMEOUT FUNCTIONS
                // ******************************

            public:
                fd_set rfds, wfds;

                inline int wait_read_management_ready(time_t timeout_ms)
                {
                    FD_ZERO(&rfds);
                    FD_SET(fd_ffs_ep_crtl, &rfds);
                    FD_SET(fd_eventfd, &rfds);

                    struct timeval timeout_select = {.tv_sec = 0, .tv_usec = timeout_ms * 1000};

                    return select(
                        (std::max({fd_ffs_ep_crtl, fd_eventfd}) + 1), &rfds, NULL, NULL, &timeout_select
                    );
                }

                inline int wait_read_ctrl_ready(time_t timeout_ms)
                {
                    FD_ZERO(&rfds);
                    FD_SET(fd_ffs_ep_crtl, &rfds);

                    struct timeval timeout_select = {.tv_sec = 0, .tv_usec = timeout_ms * 1000};

                    return select((std::max({fd_ffs_ep_crtl}) + 1), &rfds, NULL, NULL, &timeout_select);
                }

                inline int wait_read_eventfd_ready(time_t timeout_ms)
                {
                    FD_ZERO(&rfds);
                    FD_SET(fd_eventfd, &rfds);

                    struct timeval timeout_select = {.tv_sec = 0, .tv_usec = timeout_ms * 1000};

                    return select((std::max({fd_eventfd}) + 1), &rfds, NULL, NULL, &timeout_select);
                }

                inline bool is_read_ctrl_ready()
                {
                    return FD_ISSET(fd_ffs_ep_crtl, &rfds);
                }

                inline bool is_read_eventfd_ready()
                {
                    return FD_ISSET(fd_eventfd, &rfds);
                }

                // read control
                bool read_ctrl(struct usb_functionfs_event* event, size_t len = sizeof(usb_functionfs_event))
                {
                    uint8_t* buffer = reinterpret_cast<uint8_t*>(event);

                    RAC_ADVANCE(
                        TEMP_FAILURE_RETRY(read(fd_ffs_ep_crtl, buffer, len)), (RAC_ret >= 0), RAC_ON_FALSE_RW
                    );
                    return true;
                }
                inline bool read_ctrl(std::array<uint8_t, sizeof(usb_functionfs_event)> &buffer)
                {
                    return read_ctrl(
                        reinterpret_cast<struct usb_functionfs_event*>(buffer.data()), buffer.size()
                    );
                }

                // read event
                bool read_eventfd(uint64_t* ev_cnt, size_t len = sizeof(uint64_t))
                {
                    uint8_t* buffer = reinterpret_cast<uint8_t*>(ev_cnt);

                    RAC_ADVANCE(
                        TEMP_FAILURE_RETRY(read(fd_eventfd, buffer, len)), (RAC_ret >= 0), RAC_ON_FALSE_RW
                    );
                    return true;
                }

                // ******************************
                // Async IO FUNCTIONS
                // ******************************

            private:
                // private this to force user set through aio_set_timeout function
                time_t aio_timeout_ms = -1;

            public:
                std::atomic_bool aio_initialized = false;
                io_context_t* ctx;
                struct iocb *iocb_in, *iocb_out;

                bool aio_init()
                {
                    ctx = new io_context_t;
                    memset(ctx, 0, sizeof(ctx));
                    iocb_in = new struct iocb;
                    iocb_out = new struct iocb;

                    RAC_ADVANCE(io_setup(1, ctx), (RAC_ret >= 0), RAC_ON_FALSE_AIO);

                    aio_initialized = true;
                    return true;
                }

                bool aio_cleanup()
                {
                    io_destroy(*ctx);
                    delete iocb_in;
                    delete iocb_out;

                    aio_initialized = false;
                    return true;
                }

                void aio_set_timeout(time_t timeout)
                {
                    if ( -1 < timeout < USB_FFS_MIN_TIMOUT_MS )
                    {
                        // if timeout is too low, syscalls may return empty buffer even res is correct!
                        bzh_utils::log::StdLog::Warning("Timeout too low, correrted!");
                        aio_timeout_ms = USB_FFS_MIN_TIMOUT_MS;
                    }
                    else
                    {
                        aio_timeout_ms = timeout;
                    }
                }

                bool write_host(void* buffer, size_t len)
                {
                    // prep
                    io_prep_pwrite(iocb_in, fd_ffs_ep_in, buffer, len, 0);
                    iocb_in->u.c.flags |= IOCB_FLAG_RESFD;
                    iocb_in->u.c.resfd = fd_eventfd;

                    // submit
                    RAC_ADVANCE(io_submit(*ctx, 1, &iocb_in), (RAC_ret >= 0), RAC_ON_FALSE_AIO);

                    // wait and receive
                    struct io_event aio_event = {0};
                    int rec_count = 0;

                    struct timespec timeout_io_getevents = {
                        .tv_sec = 0, .tv_nsec = aio_timeout_ms * 1000 * 1000};

                    RAC_ADVANCE(
                        rec_count = io_getevents(
                            *ctx, 1, 1, &aio_event, ((aio_timeout_ms > 0) ? &timeout_io_getevents : NULL)
                        ),
                        (RAC_ret >= 0), RAC_ON_FALSE_AIO
                    );

                    // check result
                    // the result should always be ONE, not more, not less
                    if ( rec_count != 1 )
                        return false;

                    // we are not actually doing async, result should always match the request we
                    // just submitted
                    if ( (aio_event.obj->aio_lio_opcode != IO_CMD_PWRITE) ||
                         (aio_event.obj->aio_fildes != fd_ffs_ep_in) )
                        return false;

                    return true;
                }
                bool write_host(std::vector<uint8_t> &buffer)
                {
                    bool result;
                    result = write_host(buffer.data(), buffer.size());
                    buffer.clear();
                    return result;
                }

                bool read_host(void* buffer, size_t len, int32_t &aio_event_res)
                {
                    // prep
                    io_prep_pread(iocb_out, fd_ffs_ep_out, buffer, len, 0);
                    iocb_out->u.c.flags |= IOCB_FLAG_RESFD;
                    iocb_out->u.c.resfd = fd_eventfd;

                    // submit
                    RAC_ADVANCE(io_submit(*ctx, 1, &iocb_out), (RAC_ret >= 0), RAC_ON_FALSE_AIO);

                    // wait and receive
                    struct io_event aio_event = {0};
                    int rec_count = 0;

                    struct timespec timeout_io_getevents = {
                        .tv_sec = 0, .tv_nsec = aio_timeout_ms * 1000 * 1000};

                    RAC_ADVANCE(
                        rec_count = io_getevents(
                            *ctx, 1, 1, &aio_event, ((aio_timeout_ms > 0) ? &timeout_io_getevents : NULL)
                        ),
                        (RAC_ret >= 0), RAC_ON_FALSE_AIO
                    );

                    // check result
                    // the result should always be ONE, not more, not less
                    if ( rec_count != 1 )
                        return false;

                    // return result
                    aio_event_res = static_cast<int32_t>(aio_event.res);

                    // we are not actually doing async, result should always match the request we
                    // just submitted
                    if ( (aio_event.obj->aio_lio_opcode != IO_CMD_PREAD) ||
                         (aio_event.obj->aio_fildes != fd_ffs_ep_out) || (aio_event_res < 0) )
                        return false;

                    // bzh_utils::log::StdLog::Debug("====== PACKET read_host DUMP ======");
                    // bzh_utils::container::print_raw_array((uint8_t*)buffer,
                    // functionfs::USB_FFS_MAX_TRANSFER_SIZE, false);

                    return true;
                }
                bool read_host(std::vector<uint8_t> &buffer, int32_t &aio_event_res)
                {
                    buffer.clear();
                    buffer.resize(functionfs::USB_FFS_MAX_TRANSFER_SIZE);

                    bool result = false;
                    result = read_host(buffer.data(), buffer.size(), aio_event_res);
                    if ( aio_event_res >= 0 )
                    {
                        buffer.resize(aio_event_res);
                    }
                    return result;
                }

                // ******************************
                // MISC FUNCTIONS
                // ******************************

                bool init()
                {
                    return ffs_init() && aio_init();
                }
                bool cleanup()
                {
                    return ffs_cleanup() && aio_cleanup();
                }
                bool is_initialized()
                {
                    return ffs_initialized && aio_initialized;
                }
            };

        } // namespace functionfs

    } // namespace usb

} // namespace onekey

#endif // ONEKEY_USB_FUNCTIONFS_HPP
