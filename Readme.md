# 👷‍♂️ Still under construction
# 📦 The full firmware will be dropping soon
# 📡 Stay tuned

## How it works

This demo depends on kernel provided function called configfs and functionfs.

It works in following order:

1. Use libusbgx to configure configfs to set UDC in USB device mode
2. Configure functionfs to create an custom device
3. Mount functionfs control point, and apply configurations.
4. Enable the USB device so the host could find and connect it
5. Work as a service handle all traffic to and from the custom device

Please note, this project mainly for demonstrate the raw USB communication, thus, packet handling and protocol has been kept to minimal level

## How to build

### Requirement

Ubuntu 22.10 (22.04 should also works, but not tested)

If you are building your own rootfs, you have to make sure the toolchain glibc version matches

Target board Linux kernel must support configfs and functionfs related modules.

### Dependencies

#### Apt Setup

```shell
# add arm package repo, mainly for xxx-dev library packages for cross compiling
sudo sed -i 's/^deb/deb [arch=amd64]/g' /etc/apt/sources.list
sudo dpkg --add-architecture arm64
sudo dpkg --add-architecture armhf
sudo tee -a /etc/apt/sources.list.d/source-arm.list > /dev/null <<EOT
deb [arch=armhf,arm64] http://ports.ubuntu.com/ kinetic main restricted
deb [arch=armhf,arm64] http://ports.ubuntu.com/ kinetic-updates main restricted
deb [arch=armhf,arm64] http://ports.ubuntu.com/ kinetic universe
deb [arch=armhf,arm64] http://ports.ubuntu.com/ kinetic-updates universe
deb [arch=armhf,arm64] http://ports.ubuntu.com/ kinetic multiverse
deb [arch=armhf,arm64] http://ports.ubuntu.com/ kinetic-updates multiverse
deb [arch=armhf,arm64] http://ports.ubuntu.com/ kinetic-backports main restricted universe multiverse
EOT
sudo apt update
```

#### Libraries

```shell
# host
sudo apt -y install protobuf-compiler # as it only for protobuf library
# arm64
sudo apt -y install libconfig-dev:arm64
sudo apt -y install binutils:arm64
sudo apt -y libprotobuf-dev:arm64
```

#### Development Tools
```shell
# basic
sudo apt -y install build-essential ninja-build cmake libtool pkg-config clang-format
sudo apt -y install automake autoconf autotools-dev autoconf-archive
sudo apt -y install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu cpp-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

### Cross compile libusbgx

```shell
cd modules/libusbgx
CROSS_COMPILE=aarch64-linux-gnu- autoreconf -i
CROSS_COMPILE=aarch64-linux-gnu- ./configure --prefix=$(pwd)/install
make
# or make -jxx
make install # we install to the same folder, not to the system root
```

### Cross compile demo program

It's a cmake project, use your preferred IDE, open, chose correct toolchain, build, done

I recommended use VS Code for editing and test building, then use Qt Creator for deploy to target board through ssh/sftp and testing

## Demo outputs

### application output

Note: `ffs_h.read_host(buf_read_host, res)` related errors are not important, those are caused by USB read timeout, which is normal if there is no packet from host

```
[Debug] thread_run_control -> true
[AutoLog] Enter: usb_service_routine
[AutoLog] Enter: init
[Warning] Timeout too low, correrted!
[AutoLog] Exit: init
[Debug] wait_read_management_ready
[Debug] wait_read_management_ready end -> wait_result = 2
[Debug] is_read_ctrl_ready
[Debug] is_read_ctrl_ready true
[Debug] FUNCTIONFS_BIND
[Debug] ffs not enabled yet
[Debug] wait_read_management_ready
[Debug] wait_read_management_ready end -> wait_result = 2
[Debug] is_read_ctrl_ready
[Debug] is_read_ctrl_ready true
[Debug] FUNCTIONFS_ENABLE
[Debug] is_read_eventfd_ready
[Debug] is_read_eventfd_ready true
[Debug] eventfd_result -> 2
[Debug] Thread running count -> 1
[Debug] Thread running count -> 2
*****************
RAC FAILED
CALL -> ffs_h.read_host(buf_read_host, res)
CONDITION -> (RAC_ret)
RET  -> 0
[Debug] wait_read_management_ready
[Debug] wait_read_management_ready end -> wait_result = 2
[Debug] is_read_ctrl_ready
[Debug] is_read_ctrl_ready true
[Debug] FUNCTIONFS_DISABLE
[Debug] ffs not enabled yet
[Debug] Thread running count -> 3
[Debug] wait_read_management_ready
[Debug] wait_read_management_ready end -> wait_result = 2
[Debug] is_read_ctrl_ready
[Debug] is_read_ctrl_ready true
[Debug] FUNCTIONFS_ENABLE
[Debug] is_read_eventfd_ready
[Debug] is_read_eventfd_ready true
[Debug] eventfd_result -> 3
[Debug] ==== PACKET DUMP buf_read_host ====
3f 23 23 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

[Debug] ==== PACKET PARSE ====
[Debug] msg_raw.size() -> 64
[Debug] msg_id -> MessageType_Initialize
[Debug] msg_size -> 0
[Debug] msg_protobuf_data -> 

[Debug] ==== Create Features ====
[Debug] ==== Get Serialized Features ====
[Debug] feature_serialized_string.size() -> 84
[Debug] ==== PACKET DUMP Features ====
0a 09 6f 6e 65 6b 65 79 2e 73 6f 10 04 18 00 20 
00 28 01 90 01 00 aa 01 01 54 aa 1f 05 46 46 46 
46 46 b2 1f 05 31 2e 32 2e 33 b8 1f 01 d2 1f 05 
31 2e 30 2e 30 fa 1f 1c 54 43 30 31 57 42 44 32 
30 32 32 31 30 31 37 30 38 32 34 35 30 30 30 30 
30 30 36 37 
[Debug] feature_serialized.size() -> 84
[Debug] mpsed.protobuf_data.size() -> 84
[Debug] mpsed.size -> 84
[AutoLog] Enter: first pack
[AutoLog] Exit: first pack
[AutoLog] Enter: subsequential pack
[Debug] calculate processed bytes
[Debug] break if all bytes processed
[Debug] protobuf_data_bytes_composed -> 55
[Debug] protobuf_data_bytes_to_be_composed -> 29
[Debug] header
[Debug] protobuf_data
[Debug] fill 0x00 if last pack not full
[Debug] check size
[Debug] push_back
[AutoLog] Exit: subsequential pack
[AutoLog] Enter: subsequential pack
[Debug] calculate processed bytes
[AutoLog] Exit: subsequential pack
[Debug] mcomposed_list.size() -> 2
[Debug] mcomposed.size() -> 64
[Debug] ==== PACKET DUMP buf_write_host ====
3f 23 23 00 11 00 00 00 54 0a 09 6f 6e 65 6b 65 
79 2e 73 6f 10 04 18 00 20 00 28 01 90 01 00 aa 
01 01 54 aa 1f 05 46 46 46 46 46 b2 1f 05 31 2e 
32 2e 33 b8 1f 01 d2 1f 05 31 2e 30 2e 30 fa 1f 

[Debug] mcomposed.size() -> 64
[Debug] ==== PACKET DUMP buf_write_host ====
3f 1c 54 43 30 31 57 42 44 32 30 32 32 31 30 31 
37 30 38 32 34 35 30 30 30 30 30 30 36 37 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

[Debug] wait_read_management_ready
[Debug] wait_read_management_ready end -> wait_result = 1
[Debug] is_read_ctrl_ready
[Debug] is_read_eventfd_ready
[Debug] is_read_eventfd_ready true
[Debug] eventfd_result -> 3
[Debug] Thread running count -> 4
[Debug] Thread running count -> 5
[Debug] Thread running count -> 6
[Debug] Thread running count -> 7
[Debug] Thread running count -> 8
[Debug] Thread running count -> 9
[Debug] Thread running count -> 10
[Debug] Thread running count -> 11
[Debug] Thread running count -> 12
[Debug] Thread running count -> 13
[Debug] Thread running count -> 14
[Debug] Thread running count -> 15
[Debug] Thread running count -> 16
[Debug] Thread running count -> 17
[Debug] Thread running count -> 18
[Debug] Thread running count -> 19
[Debug] Thread running count -> 20
[Debug] Thread stopping count -> 1
[Debug] Thread stopping count -> 2
[Debug] Thread stopping count -> 3
[Debug] Thread stopping count -> 4
*****************
RAC FAILED
CALL -> ffs_h.read_host(buf_read_host, res)
CONDITION -> (RAC_ret)
RET  -> 0
[Debug] loop out
[AutoLog] Enter: cleanup
[AutoLog] Exit: cleanup
[AutoLog] Exit: usb_service_routine
[Debug] Thread stopping count -> 4
```



### kernel logs

```
[   23.033332] functionfs_init()
[   23.033387] file system registered
[   23.034139] ffs_alloc()
[   23.034539] ffs_fs_mount()
[   23.034544] ffs_fs_parse_opts()
[   23.034549] ffs_data_new()
[   23.034579] ffs_acquire_dev()
[   23.034664] ffs_sb_fill()
[   23.034669] ffs_sb_make_inode()
[   23.034679] ffs_sb_create_file()
[   23.034683] ffs_sb_make_inode()
[   23.035554] ffs_ep0_open()
[   23.035560] ffs_data_opened()
[   23.035577] ffs_ep0_write()
[   23.035586] Buffer from user space:
[   23.035596] : 03 00 00 00 71 00 00 00 2f 00 00 00 03 00 00 00  ....q.../.......
[   23.035598] : 03 00 00 00 03 00 00 00 00 00 00 00 01 00 00 00  ................
[   23.035601] : 09 04 00 00 02 ff 00 00 01 07 05 81 03 40 00 01  .............@..
[   23.035603] : 07 05 01 03 40 00 01 09 04 00 00 02 ff 00 00 01  ....@...........
[   23.035606] : 07 05 81 03 40 00 01 07 05 01 03 40 00 01 01 23  ....@......@...#
[   23.035608] : 00 00 00 01 00 04 00 01 00 00 01 00 00 00 00 00  ................
[   23.035611] : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
[   23.035613] : 00                                               .
[   23.035615] read descriptors
[   23.035669] __ffs_data_got_descs()
[   23.035678] ffs_do_descs()
[   23.035681] __ffs_data_do_entity()
[   23.035684] ffs_do_single_desc()
[   23.035689] interface descriptor
[   23.035692] entity INTERFACE(00)
[   23.035694] __ffs_data_do_entity()
[   23.035697] entity STRING(01)
[   23.035699] __ffs_data_do_entity()
[   23.035702] __ffs_data_do_entity()
[   23.035704] ffs_do_single_desc()
[   23.035707] endpoint descriptor
[   23.035710] entity ENDPOINT(81)
[   23.035712] __ffs_data_do_entity()
[   23.035714] __ffs_data_do_entity()
[   23.035716] ffs_do_single_desc()
[   23.035717] endpoint descriptor
[   23.035719] entity ENDPOINT(01)
[   23.035721] __ffs_data_do_entity()
[   23.035723] __ffs_data_do_entity()
[   23.035726] ffs_do_descs()
[   23.035728] __ffs_data_do_entity()
[   23.035730] ffs_do_single_desc()
[   23.035732] interface descriptor
[   23.035734] entity INTERFACE(00)
[   23.035736] __ffs_data_do_entity()
[   23.035738] entity STRING(01)
[   23.035740] __ffs_data_do_entity()
[   23.035742] __ffs_data_do_entity()
[   23.035744] ffs_do_single_desc()
[   23.035745] endpoint descriptor
[   23.035747] entity ENDPOINT(81)
[   23.035749] __ffs_data_do_entity()
[   23.035751] __ffs_data_do_entity()
[   23.035753] ffs_do_single_desc()
[   23.035755] endpoint descriptor
[   23.035757] entity ENDPOINT(01)
[   23.035759] __ffs_data_do_entity()
[   23.035761] __ffs_data_do_entity()
[   23.035765] ffs_do_os_descs()
[   23.035768] ffs_do_single_os_desc()
[   23.035772] __ffs_data_do_os_desc()
[   23.035791] ffs_ep0_write()
[   23.035798] Buffer from user space:
[   23.035801] : 02 00 00 00 1c 00 00 00 01 00 00 00 01 00 00 00  ................
[   23.035803] : 09 04 4f 6e 65 4b 65 79 55 53 42 00              ..OneKeyUSB.
[   23.035805] read strings
[   23.035821] __ffs_data_got_strings()
[   23.035828] ffs_epfiles_create()
[   23.035834] ffs_sb_create_file()
[   23.035844] ffs_sb_make_inode()
[   23.035855] ffs_sb_create_file()
[   23.035859] ffs_sb_make_inode()
[   23.035865] ffs_ready()
[   23.035888] ffs_epfile_open()
[   23.035891] ffs_data_opened()
[   23.035909] ffs_epfile_open()
[   23.035912] ffs_data_opened()
[   23.036284] udc fcc00000.dwc3: registering UDC driver [onekeyusb]
[   23.036357] configfs-gadget gadget: adding 'Function FS Gadget'/00000000e40a8f5b to config 'config'/0000000013956175
[   23.036364] ffs_do_functionfs_bind()
[   23.036369] functionfs_bind()
[   23.036373] ffs_data_get()
[   23.036377] _ffs_func_bind()
[   23.036382] ffs_do_descs()
[   23.036385] ffs_do_single_desc()
[   23.036389] interface descriptor
[   23.036392] entity INTERFACE(00)
[   23.036395] entity STRING(01)
[   23.036401] : Original  ep desc: 07 05 81 03 40 00 01                             ....@..
[   23.036404] autoconfig
[   23.036414] : Rewritten ep desc: 07 05 81 03 40 00 01                             ....@..
[   23.036416] ffs_do_single_desc()
[   23.036418] endpoint descriptor
[   23.036421] entity ENDPOINT(81)
[   23.036424] : Original  ep desc: 07 05 01 03 40 00 01                             ....@..
[   23.036425] autoconfig
[   23.036429] : Rewritten ep desc: 07 05 01 03 40 00 01                             ....@..
[   23.036431] ffs_do_single_desc()
[   23.036432] endpoint descriptor
[   23.036435] entity ENDPOINT(01)
[   23.036525] ffs_do_descs()
[   23.036527] ffs_do_single_desc()
[   23.036529] interface descriptor
[   23.036532] entity INTERFACE(00)
[   23.036534] entity STRING(01)
[   23.036538] : Original  ep desc: 07 05 81 03 40 00 01                             ....@..
[   23.036541] : Rewritten ep desc: 07 05 81 03 40 00 01                             ....@..
[   23.036544] ffs_do_single_desc()
[   23.036546] endpoint descriptor
[   23.036549] entity ENDPOINT(81)
[   23.036551] : Original  ep desc: 07 05 01 03 40 00 01                             ....@..
[   23.036553] : Rewritten ep desc: 07 05 01 03 40 00 01                             ....@..
[   23.036555] ffs_do_single_desc()
[   23.036557] endpoint descriptor
[   23.036559] entity ENDPOINT(01)
[   23.036562] ffs_do_descs()
[   23.036564] ffs_do_single_desc()
[   23.036565] interface descriptor
[   23.036568] entity INTERFACE(00)
[   23.036572] 00 -> 00
[   23.036574] entity STRING(01)
[   23.036577] 01 -> 05
[   23.036578] ffs_do_single_desc()
[   23.036580] endpoint descriptor
[   23.036582] entity ENDPOINT(81)
[   23.036584] ffs_do_single_desc()
[   23.036586] endpoint descriptor
[   23.036588] entity ENDPOINT(01)
[   23.036590] ffs_do_single_desc()
[   23.036591] interface descriptor
[   23.036593] entity INTERFACE(00)
[   23.036596] 00 -> 00
[   23.036598] entity STRING(01)
[   23.036600] 01 -> 05
[   23.036602] ffs_do_single_desc()
[   23.036603] endpoint descriptor
[   23.036605] entity ENDPOINT(81)
[   23.036607] ffs_do_single_desc()
[   23.036618] endpoint descriptor
[   23.036620] entity ENDPOINT(01)
[   23.036626] adding event 0
[   23.041298] ffs_ep0_read()
[   23.241068] dwc3 fcc00000.dwc3: device reset
[   23.352573] dwc3 fcc00000.dwc3: device reset
[   23.352763] android_work: did not send uevent (0 0           (null))
[   23.457394] android_work: sent uevent USB_STATE=CONNECTED
[   23.460297] configfs-gadget gadget: high-speed config #1: config
[   23.460452] adding event 2
[   23.460906] android_work: sent uevent USB_STATE=CONFIGURED
[   23.541890] ffs_ep0_read()
[   23.542133] ffs_epfile_read_iter()
[   25.704083] dwc3 fcc00000.dwc3: device reset
[   25.704236] configfs-gadget gadget: reset config
[   25.704288] ffs_epfile_async_io_complete()
[   25.704338] adding event 3
[   25.704578] android_work: sent uevent USB_STATE=DISCONNECTED
[   25.704970] ffs_ep0_read()
[   25.809232] dwc3 fcc00000.dwc3: device reset
[   25.809417] android_work: did not send uevent (0 0           (null))
[   25.909335] android_work: sent uevent USB_STATE=CONNECTED
[   25.910608] configfs-gadget gadget: high-speed config #1: config
[   25.910779] adding event 2
[   25.911080] android_work: sent uevent USB_STATE=CONFIGURED
[   26.205550] ffs_ep0_read()
[   26.205692] ffs_epfile_read_iter()
[   26.205991] ffs_epfile_async_io_complete()
[   26.209882] ffs_epfile_write_iter()
[   26.210202] ffs_epfile_async_io_complete()
[   26.210696] ffs_epfile_write_iter()
[   26.211020] ffs_epfile_async_io_complete()
[   26.211434] ffs_epfile_read_iter()
[   86.212822] configfs-gadget fcc00000.dwc3: unregistering UDC driver [onekeyusb]
[   86.213066] configfs-gadget gadget: reset config
[   86.213126] ffs_epfile_async_io_complete()
[   86.213194] adding event 3
[   86.213332] configfs-gadget gadget: unbind function 'Function FS Gadget'/00000000e40a8f5b
[   86.213349] ffs_func_unbind()
[   86.213371] functionfs_unbind()
[   86.213385] ffs_data_put()
[   86.213401] purging event 3
[   86.213409] adding event 1
[   86.213753] android_work: sent uevent USB_STATE=DISCONNECTED
[   86.214076] ffs_epfile_release()
[   86.214089] ffs_data_closed()
[   86.214098] ffs_data_put()
[   86.214142] ffs_epfile_release()
[   86.214151] ffs_data_closed()
[   86.214159] ffs_data_put()
[   86.214189] ffs_ep0_release()
[   86.214197] ffs_data_closed()
[   86.214204] ffs_data_reset()
[   86.214212] ffs_data_clear()
[   86.214218] ffs_closed()
[   86.214233] ffs_epfiles_destroy()
[   86.214291] ffs_data_put()
[   86.230045] ffs_fs_kill_sb()
[   86.230137] ffs_data_closed()
[   86.230149] ffs_data_reset()
[   86.230151] ffs_data_clear()
[   86.230154] ffs_closed()
[   86.230163] ffs_data_put()
[   86.230166] ffs_data_put(): freeing
[   86.230230] ffs_data_clear()
[   86.230233] ffs_closed()
[   86.230236] ffs_release_dev()
[   86.270605] ffs_release_dev()
[   86.270618] functionfs_cleanup()
[   86.270620] unloading

```

