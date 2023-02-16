#ifndef ONEKEY_USB_UTIL_HPP
#define ONEKEY_USB_UTIL_HPP

#include <cstdint>

// Little Endian Handling
#include <endian.h>

/*
 * cpu_to_le16/32 are used when initializing structures, a context where a
 * function call is not allowed. To solve this, we code cpu_to_le16/32 in a way
 * that allows them to be used when initializing structures.
 */

#if __BYTE_ORDER == __LITTLE_ENDIAN
  #define cpu_to_le16(x) (x)
  #define cpu_to_le32(x) (x)
#else
    //   #define cpu_to_le16(x) ((((x) >> 8) & 0xffu) | (((x)&0xffu) << 8))
    //   #define cpu_to_le32(x)                                                               \
//     ((((x)&0xff000000u) >> 24) | (((x)&0x00ff0000u) >> 8) | (((x)&0x0000ff00u) << 8) | \
//      (((x)&0x000000ffu) << 24))
  #define cpu_to_le16(x) htole16(x)
  #define cpu_to_le32(x) htole32(x)
#endif

#define le32_to_cpu(x)             le32toh(x)
#define le16_to_cpu(x)             le16toh(x)

#define CONFIG_DESC_MAXPOWER_mA(x) (((x) + 1) / 2)

///////////////////////////////////////////////////////////////////////

// took from TinyUSB
// tusb_common.h
// hid.h
// hid_device.h

//--------------------------------------------------------------------+
// Macros Helper
//--------------------------------------------------------------------+
#define TU_ARRAY_SIZE(_arr) (sizeof(_arr) / sizeof(_arr[0]))
#define TU_MIN(_x, _y)      (((_x) < (_y)) ? (_x) : (_y))
#define TU_MAX(_x, _y)      (((_x) > (_y)) ? (_x) : (_y))

#define TU_U16(_high, _low) ((uint16_t)(((_high) << 8) | (_low)))
#define TU_U16_HIGH(_u16)   ((uint8_t)(((_u16) >> 8) & 0x00ff))
#define TU_U16_LOW(_u16)    ((uint8_t)((_u16)&0x00ff))
#define U16_TO_U8S_BE(_u16) TU_U16_HIGH(_u16), TU_U16_LOW(_u16)
#define U16_TO_U8S_LE(_u16) TU_U16_LOW(_u16), TU_U16_HIGH(_u16)

#define TU_U32_BYTE3(_u32)  ((uint8_t)((((uint32_t)_u32) >> 24) & 0x000000ff)) // MSB
#define TU_U32_BYTE2(_u32)  ((uint8_t)((((uint32_t)_u32) >> 16) & 0x000000ff))
#define TU_U32_BYTE1(_u32)  ((uint8_t)((((uint32_t)_u32) >> 8) & 0x000000ff))
#define TU_U32_BYTE0(_u32)  ((uint8_t)(((uint32_t)_u32) & 0x000000ff)) // LSB

#define U32_TO_U8S_BE(_u32) TU_U32_BYTE3(_u32), TU_U32_BYTE2(_u32), TU_U32_BYTE1(_u32), TU_U32_BYTE0(_u32)
#define U32_TO_U8S_LE(_u32) TU_U32_BYTE0(_u32), TU_U32_BYTE1(_u32), TU_U32_BYTE2(_u32), TU_U32_BYTE3(_u32)

#define TU_BIT(n)           (1UL << (n))
#define TU_GENMASK(h, l)    ((UINT32_MAX << (l)) & (UINT32_MAX >> (31 - (h))))

//------------- ITEM & TAG -------------//
#define HID_REPORT_DATA_0(data)
#define HID_REPORT_DATA_1(data) , data
#define HID_REPORT_DATA_2(data) , U16_TO_U8S_LE(data)
#define HID_REPORT_DATA_3(data) , U32_TO_U8S_LE(data)

#define HID_REPORT_ITEM(data, tag, type, size) \
  (((tag) << 4) | ((type) << 2) | (size)) HID_REPORT_DATA_##size(data)

// Report Item Types
enum
{
    RI_TYPE_MAIN = 0,
    RI_TYPE_GLOBAL = 1,
    RI_TYPE_LOCAL = 2
};

//------------- Main Items - HID 1.11 section 6.2.2.4 -------------//

// Report Item Main group
enum
{
    RI_MAIN_INPUT = 8,
    RI_MAIN_OUTPUT = 9,
    RI_MAIN_COLLECTION = 10,
    RI_MAIN_FEATURE = 11,
    RI_MAIN_COLLECTION_END = 12
};

#define HID_INPUT(x)       HID_REPORT_ITEM(x, RI_MAIN_INPUT, RI_TYPE_MAIN, 1)
#define HID_OUTPUT(x)      HID_REPORT_ITEM(x, RI_MAIN_OUTPUT, RI_TYPE_MAIN, 1)
#define HID_COLLECTION(x)  HID_REPORT_ITEM(x, RI_MAIN_COLLECTION, RI_TYPE_MAIN, 1)
#define HID_FEATURE(x)     HID_REPORT_ITEM(x, RI_MAIN_FEATURE, RI_TYPE_MAIN, 1)
#define HID_COLLECTION_END HID_REPORT_ITEM(x, RI_MAIN_COLLECTION_END, RI_TYPE_MAIN, 0)

//------------- Input, Output, Feature - HID 1.11 section 6.2.2.5 -------------//
#define HID_DATA             (0 << 0)
#define HID_CONSTANT         (1 << 0)

#define HID_ARRAY            (0 << 1)
#define HID_VARIABLE         (1 << 1)

#define HID_ABSOLUTE         (0 << 2)
#define HID_RELATIVE         (1 << 2)

#define HID_WRAP_NO          (0 << 3)
#define HID_WRAP             (1 << 3)

#define HID_LINEAR           (0 << 4)
#define HID_NONLINEAR        (1 << 4)

#define HID_PREFERRED_STATE  (0 << 5)
#define HID_PREFERRED_NO     (1 << 5)

#define HID_NO_NULL_POSITION (0 << 6)
#define HID_NULL_STATE       (1 << 6)

#define HID_NON_VOLATILE     (0 << 7)
#define HID_VOLATILE         (1 << 7)

#define HID_BITFIELD         (0 << 8)
#define HID_BUFFERED_BYTES   (1 << 8)

//------------- Collection Item - HID 1.11 section 6.2.2.6 -------------//
enum
{
    HID_COLLECTION_PHYSICAL = 0,
    HID_COLLECTION_APPLICATION,
    HID_COLLECTION_LOGICAL,
    HID_COLLECTION_REPORT,
    HID_COLLECTION_NAMED_ARRAY,
    HID_COLLECTION_USAGE_SWITCH,
    HID_COLLECTION_USAGE_MODIFIER
};

//------------- Global Items - HID 1.11 section 6.2.2.7 -------------//

// Report Item Global group
enum
{
    RI_GLOBAL_USAGE_PAGE = 0,
    RI_GLOBAL_LOGICAL_MIN = 1,
    RI_GLOBAL_LOGICAL_MAX = 2,
    RI_GLOBAL_PHYSICAL_MIN = 3,
    RI_GLOBAL_PHYSICAL_MAX = 4,
    RI_GLOBAL_UNIT_EXPONENT = 5,
    RI_GLOBAL_UNIT = 6,
    RI_GLOBAL_REPORT_SIZE = 7,
    RI_GLOBAL_REPORT_ID = 8,
    RI_GLOBAL_REPORT_COUNT = 9,
    RI_GLOBAL_PUSH = 10,
    RI_GLOBAL_POP = 11
};

#define HID_USAGE_PAGE(x)         HID_REPORT_ITEM(x, RI_GLOBAL_USAGE_PAGE, RI_TYPE_GLOBAL, 1)
#define HID_USAGE_PAGE_N(x, n)    HID_REPORT_ITEM(x, RI_GLOBAL_USAGE_PAGE, RI_TYPE_GLOBAL, n)

#define HID_LOGICAL_MIN(x)        HID_REPORT_ITEM(x, RI_GLOBAL_LOGICAL_MIN, RI_TYPE_GLOBAL, 1)
#define HID_LOGICAL_MIN_N(x, n)   HID_REPORT_ITEM(x, RI_GLOBAL_LOGICAL_MIN, RI_TYPE_GLOBAL, n)

#define HID_LOGICAL_MAX(x)        HID_REPORT_ITEM(x, RI_GLOBAL_LOGICAL_MAX, RI_TYPE_GLOBAL, 1)
#define HID_LOGICAL_MAX_N(x, n)   HID_REPORT_ITEM(x, RI_GLOBAL_LOGICAL_MAX, RI_TYPE_GLOBAL, n)

#define HID_PHYSICAL_MIN(x)       HID_REPORT_ITEM(x, RI_GLOBAL_PHYSICAL_MIN, RI_TYPE_GLOBAL, 1)
#define HID_PHYSICAL_MIN_N(x, n)  HID_REPORT_ITEM(x, RI_GLOBAL_PHYSICAL_MIN, RI_TYPE_GLOBAL, n)

#define HID_PHYSICAL_MAX(x)       HID_REPORT_ITEM(x, RI_GLOBAL_PHYSICAL_MAX, RI_TYPE_GLOBAL, 1)
#define HID_PHYSICAL_MAX_N(x, n)  HID_REPORT_ITEM(x, RI_GLOBAL_PHYSICAL_MAX, RI_TYPE_GLOBAL, n)

#define HID_UNIT_EXPONENT(x)      HID_REPORT_ITEM(x, RI_GLOBAL_UNIT_EXPONENT, RI_TYPE_GLOBAL, 1)
#define HID_UNIT_EXPONENT_N(x, n) HID_REPORT_ITEM(x, RI_GLOBAL_UNIT_EXPONENT, RI_TYPE_GLOBAL, n)

#define HID_UNIT(x)               HID_REPORT_ITEM(x, RI_GLOBAL_UNIT, RI_TYPE_GLOBAL, 1)
#define HID_UNIT_N(x, n)          HID_REPORT_ITEM(x, RI_GLOBAL_UNIT, RI_TYPE_GLOBAL, n)

#define HID_REPORT_SIZE(x)        HID_REPORT_ITEM(x, RI_GLOBAL_REPORT_SIZE, RI_TYPE_GLOBAL, 1)
#define HID_REPORT_SIZE_N(x, n)   HID_REPORT_ITEM(x, RI_GLOBAL_REPORT_SIZE, RI_TYPE_GLOBAL, n)

#define HID_REPORT_ID(x)          HID_REPORT_ITEM(x, RI_GLOBAL_REPORT_ID, RI_TYPE_GLOBAL, 1),
#define HID_REPORT_ID_N(x, n)     HID_REPORT_ITEM(x, RI_GLOBAL_REPORT_ID, RI_TYPE_GLOBAL, n),

#define HID_REPORT_COUNT(x)       HID_REPORT_ITEM(x, RI_GLOBAL_REPORT_COUNT, RI_TYPE_GLOBAL, 1)
#define HID_REPORT_COUNT_N(x, n)  HID_REPORT_ITEM(x, RI_GLOBAL_REPORT_COUNT, RI_TYPE_GLOBAL, n)

#define HID_PUSH                  HID_REPORT_ITEM(x, RI_GLOBAL_PUSH, RI_TYPE_GLOBAL, 0)
#define HID_POP                   HID_REPORT_ITEM(x, RI_GLOBAL_POP, RI_TYPE_GLOBAL, 0)

//------------- LOCAL ITEMS 6.2.2.8 -------------//

enum
{
    RI_LOCAL_USAGE = 0,
    RI_LOCAL_USAGE_MIN = 1,
    RI_LOCAL_USAGE_MAX = 2,
    RI_LOCAL_DESIGNATOR_INDEX = 3,
    RI_LOCAL_DESIGNATOR_MIN = 4,
    RI_LOCAL_DESIGNATOR_MAX = 5,
    // 6 is reserved
    RI_LOCAL_STRING_INDEX = 7,
    RI_LOCAL_STRING_MIN = 8,
    RI_LOCAL_STRING_MAX = 9,
    RI_LOCAL_DELIMITER = 10,
};

#define HID_USAGE(x)          HID_REPORT_ITEM(x, RI_LOCAL_USAGE, RI_TYPE_LOCAL, 1)
#define HID_USAGE_N(x, n)     HID_REPORT_ITEM(x, RI_LOCAL_USAGE, RI_TYPE_LOCAL, n)

#define HID_USAGE_MIN(x)      HID_REPORT_ITEM(x, RI_LOCAL_USAGE_MIN, RI_TYPE_LOCAL, 1)
#define HID_USAGE_MIN_N(x, n) HID_REPORT_ITEM(x, RI_LOCAL_USAGE_MIN, RI_TYPE_LOCAL, n)

#define HID_USAGE_MAX(x)      HID_REPORT_ITEM(x, RI_LOCAL_USAGE_MAX, RI_TYPE_LOCAL, 1)
#define HID_USAGE_MAX_N(x, n) HID_REPORT_ITEM(x, RI_LOCAL_USAGE_MAX, RI_TYPE_LOCAL, n)

/// HID Usage Table - Table 1: Usage Page Summary
enum
{
    HID_USAGE_PAGE_DESKTOP = 0x01,
    HID_USAGE_PAGE_SIMULATE = 0x02,
    HID_USAGE_PAGE_VIRTUAL_REALITY = 0x03,
    HID_USAGE_PAGE_SPORT = 0x04,
    HID_USAGE_PAGE_GAME = 0x05,
    HID_USAGE_PAGE_GENERIC_DEVICE = 0x06,
    HID_USAGE_PAGE_KEYBOARD = 0x07,
    HID_USAGE_PAGE_LED = 0x08,
    HID_USAGE_PAGE_BUTTON = 0x09,
    HID_USAGE_PAGE_ORDINAL = 0x0a,
    HID_USAGE_PAGE_TELEPHONY = 0x0b,
    HID_USAGE_PAGE_CONSUMER = 0x0c,
    HID_USAGE_PAGE_DIGITIZER = 0x0d,
    HID_USAGE_PAGE_PID = 0x0f,
    HID_USAGE_PAGE_UNICODE = 0x10,
    HID_USAGE_PAGE_ALPHA_DISPLAY = 0x14,
    HID_USAGE_PAGE_MEDICAL = 0x40,
    HID_USAGE_PAGE_MONITOR = 0x80, // 0x80 - 0x83
    HID_USAGE_PAGE_POWER = 0x84,   // 0x084 - 0x87
    HID_USAGE_PAGE_BARCODE_SCANNER = 0x8c,
    HID_USAGE_PAGE_SCALE = 0x8d,
    HID_USAGE_PAGE_MSR = 0x8e,
    HID_USAGE_PAGE_CAMERA = 0x90,
    HID_USAGE_PAGE_ARCADE = 0x91,
    HID_USAGE_PAGE_FIDO = 0xF1D0,  // FIDO alliance HID usage page
    HID_USAGE_PAGE_VENDOR = 0xFF00 // 0xFF00 - 0xFFFF
};

/// HID Usage Table: FIDO Alliance Page (0xF1D0)
enum
{
    HID_USAGE_FIDO_U2FHID = 0x01,  // U2FHID usage for top-level collection
    HID_USAGE_FIDO_DATA_IN = 0x20, // Raw IN data report
    HID_USAGE_FIDO_DATA_OUT = 0x21 // Raw OUT data report
};

// FIDO U2F Authenticator Descriptor Template
// - 1st parameter is report size, which is 64 bytes maximum in U2F
// - 2nd parameter is HID_REPORT_ID(n) (optional)
#define TUD_HID_REPORT_DESC_FIDO_U2F(report_size, ...)                                                       \
  HID_USAGE_PAGE_N(HID_USAGE_PAGE_FIDO, 2), HID_USAGE(HID_USAGE_FIDO_U2FHID),                                \
      HID_COLLECTION(HID_COLLECTION_APPLICATION), /* Report ID if any */                                     \
      __VA_ARGS__                                 /* Usage Data In */                                        \
      HID_USAGE(HID_USAGE_FIDO_DATA_IN),                                                                     \
      HID_LOGICAL_MIN(0), HID_LOGICAL_MAX_N(0xff, 2), HID_REPORT_SIZE(8), HID_REPORT_COUNT(report_size),     \
      HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), /* Usage Data Out */                                \
      HID_USAGE(HID_USAGE_FIDO_DATA_OUT), HID_LOGICAL_MIN(0), HID_LOGICAL_MAX_N(0xff, 2),                    \
      HID_REPORT_SIZE(8), HID_REPORT_COUNT(report_size), HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), \
      HID_COLLECTION_END

#endif // ONEKEY_USB_UTIL_HPP
