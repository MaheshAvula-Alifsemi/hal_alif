/**
 ****************************************************************************************
 *
 * @file gap.h
 *
 * @brief Header file - Generic Access Profile (GAP)
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef GAP_H_
#define GAP_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @addtogroup GAP_API Generic Access Profile (GAP)
 * @brief Generic Access Profile
 *
 * The GAP module is responsible for providing an API to the application in order to
 * configure the device in the desired mode (discoverable, connectable, etc.) and perform
 * required actions (scanning, connection, pairing, etc.). To achieve this, the GAP
 * interfaces with both the @ref GAPC_API @ref GAPM_API, @ref GATT_API and @ref L2CAP_API
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "compiler.h"
#include "rwip_config.h"
#include "co_bt_defines.h"
#include "co_math.h"

/**
 * @defgroup GAP_DEFAULT_API Default Type and Enumeration Definition
 * @ingroup GAP_API
 * @{
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/**
 * @defgroup GAP_ENUM_API Enumerations
 * @ingroup GAP_DEFAULT_API
 * @{
 */

/// Invalid Procedure Token
#define GAP_INVALID_TOKEN                       (0x0000)
/// BD address length
#define GAP_BD_ADDR_LEN                         (6)
/// Key length
#define GAP_KEY_LEN                             (16)
/// Invalid index, can be use for all 8-bit indexes
#define GAP_INVALID_IDX                         (0xFF)
/// Invalid activity index
#define GAP_INVALID_ACTV_IDX                    (0xFF)
/// Invalid connection index
#define GAP_INVALID_CONIDX                      (0xFF)
/// Invalid connection handle
#define GAP_INVALID_CONHDL                      (0xFFFF)
/// Undefined connection handle - on peripheral side when BT Classic connection is under establishment
#define GAP_UNDEFINED_CONHDL                    (0xFFFE)
/// Invalid L2CAP channel identifier
#define GAP_INVALID_L2CAP_CID                   (0xFFFF)
/// Manufacturer: RivieraWaves SAS
#define GAP_RIVIERAWAVES_VENDOR_ID              (0x0060)
/// Invalid encryption key size
#define GAP_INVALID_ENC_KEY_SIZE                (0)
/// Minimum Encryption key size
#define GAP_MIN_ENC_KEY_SIZE                    (7)
/// Maximum Encryption Key size
#define GAP_MAX_ENC_KEY_SIZE                    (16)

/// Random number length
#define GAP_RAND_NB_LEN                         (0x08)
/// AES Data result length
#define GAP_AES_LEN                             (16)
/// P256 Key Len
#define GAP_P256_KEY_LEN                        (0x20)
/// Length of 128-bit UUID in octets
#define GAP_UUID_128_LEN                        (16)
/// Size of AD Length Field
#define GAP_AD_LEN_SIZE                         (1)
/// Size of AD Type Field
#define GAP_AD_TYPE_SIZE                        (1)
/// Size of AD Header (LTV format)
#define GAP_AD_HEADER_SIZE                      (GAP_AD_LEN_SIZE + GAP_AD_TYPE_SIZE)

/// Maximum length of device name characteristic
#define GAP_DEV_NAME_LEN_MAX                    (248)
/// Length of appearance characteristic
#define GAP_APPEARANCE_LEN                      (2)
/// Length of peripheral preferred connection parameters characteristic
#define GAP_PH_PREF_PARAM_LEN                   (8)
/// Length of central address resolution characteristic
#define GAP_CT_ADDR_RESOL_LEN                   (1)
/// Length of resolvable private address only characteristic
#define GAP_RSLV_PRIV_ADDR_ONLY_LEN             (1)
/// Length of encrypted data key material characteristic
#define GAP_ENC_DATA_KEY_MATERIAL_LEN           (24)


/// Maximum BT Service name UTF-8 string including trailing zero
#define GAP_BT_SDP_MAX_SERVICE_NAME_LENGTH      (50)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Device identification vendor identifier source
enum gap_vendor_id_source
{
    /// Bluetooth SIG assigned Device ID Vendor ID value from the Assigned Numbers document.
    GAP_BLUETOOTH_SIG_VENDOR_ID_SOURCE = (0x0001),
    /// USB Implementer's Forum assigned Vendor ID value
    GAP_USB_FORUM_VENDOR_ID_SOURCE = (0x0002),
};

/// GAP Connection role
enum gap_con_role
{
    /// Central of the connection
    GAP_CON_CENTRAL,
    /// Peripheral of the connection
    GAP_CON_PERIPHERAL,
};

/// GAP device supported roles
enum gap_role
{
    /// No role set yet
    GAP_ROLE_NONE = 0x00,
    /// Low Energy observer role
    GAP_ROLE_LE_OBSERVER = 0x01,
    /// Low Energy broadcaster role
    GAP_ROLE_LE_BROADCASTER = 0x02,
    /// Low Energy central role
    GAP_ROLE_LE_CENTRAL = (0x04 | GAP_ROLE_LE_OBSERVER),
    /// Low Energy peripheral role
    GAP_ROLE_LE_PERIPHERAL = (0x08 | GAP_ROLE_LE_BROADCASTER),
    /// All LE roles: both peripheral and central
    GAP_ROLE_LE_ALL = (GAP_ROLE_LE_CENTRAL | GAP_ROLE_LE_PERIPHERAL),
    /// Supports BT Classic only
    GAP_ROLE_BT_CLASSIC = 0x10,
    /// Supports full BTDM configuration (BT-Classic and all LE roles)
    GAP_ROLE_BTDM = (GAP_ROLE_LE_ALL | GAP_ROLE_BT_CLASSIC),
};

/// IO Capability Values
enum gap_io_cap
{
    /// Display Only
    GAP_IO_CAP_DISPLAY_ONLY = 0x00,
    /// Display Yes No
    GAP_IO_CAP_DISPLAY_YES_NO,
    /// Keyboard Only
    GAP_IO_CAP_KB_ONLY,
    /// No Input No Output
    GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
    /// Keyboard Display (LE Only)
    GAP_IO_CAP_KB_DISPLAY,
    GAP_IO_CAP_LAST
};

/// Authentication mask
enum gap_auth_mask
{
    /// No Flag set
    GAP_AUTH_NONE = 0,
    /// Bond authentication
    GAP_AUTH_BOND = (1 << 0),
    /// Man In the middle protection
    GAP_AUTH_MITM = (1 << 2),
    /// Secure Connection
    GAP_AUTH_SEC_CON = (1 << 3),
    /// Key Notification
    GAP_AUTH_KEY_NOTIF = (1 << 4),
    /// Cross Transport Key Derivation with h7()
    GAP_AUTH_CT2 = (1 << 5),
};

/// Security Link Level
enum gap_sec_lvl
{
    /// Service accessible through an un-encrypted link\n
    /// Security Level 1
    ///
    /// @note Attribute value is accessible on an unencrypted link.
    GAP_SEC_NOT_ENC = 0u,
    /// Service require an unauthenticated pairing (just work pairing)\n
    /// Security Level 2
    ///
    /// @note Attribute value is accessible on an encrypted link or modified with using write signed procedure
    /// on unencrypted link if bonded using an unauthenticated pairing.
    GAP_SEC_UNAUTH  = 1u,

    /// Service require an authenticated pairing (Legacy pairing with pin code or OOB)\n
    /// Security Level 3
    ///
    /// @note Attribute value is accessible on an encrypted link or modified with using write signed procedure
    /// on unencrypted link if bonded using an authenticated pairing.
    GAP_SEC_AUTH = 2u,

    /// Service require a secure connection pairing\n
    /// Security Level 4
    ///
    /// @note Attribute value is accessible on an encrypted link or modified with using write signed procedure
    /// on unencrypted link if bonded using a secure connection pairing.
    GAP_SEC_SECURE_CON = 3u,
};

/// Security Defines
enum gap_sec_req
{
    /// No security (no authentication and encryption) (Mode 1 - Level 1)
    GAP_NO_SEC = 0x00,
    /// Unauthenticated pairing with encryption (Mode 1 - Level 2)
    GAP_SEC1_NOAUTH_PAIR_ENC,
    /// Authenticated pairing with encryption (Mode 1 - Level 3)
    GAP_SEC1_AUTH_PAIR_ENC,
    /// Unauthenticated pairing with data signing (Mode 2 - Level 1)
    GAP_SEC2_NOAUTH_DATA_SGN,
    /// Authentication pairing with data signing (Mode 2 - Level 2)
    GAP_SEC2_AUTH_DATA_SGN,
    /// Secure Connection pairing with encryption (Mode 1 - Level 4)
    GAP_SEC1_SEC_CON_PAIR_ENC,
};

/// Address type
enum gap_addr_type
{
    /// Public device address
    GAP_ADDR_PUBLIC = 0x00,
    /// Random device Address
    GAP_ADDR_RAND = 0x01,
};

/// Authentication Requirements
enum gap_auth
{
    /// No MITM No Bonding
    GAP_AUTH_REQ_NO_MITM_NO_BOND = (GAP_AUTH_NONE),
    /// No MITM Bonding
    GAP_AUTH_REQ_NO_MITM_BOND = (GAP_AUTH_BOND),
    /// MITM No Bonding
    GAP_AUTH_REQ_MITM_NO_BOND = (GAP_AUTH_MITM),
    /// MITM and Bonding
    GAP_AUTH_REQ_MITM_BOND = (GAP_AUTH_MITM | GAP_AUTH_BOND),
    /// SEC_CON and No Bonding
    GAP_AUTH_REQ_SEC_CON_NO_BOND = (GAP_AUTH_SEC_CON | GAP_AUTH_MITM),
    /// SEC_CON and Bonding
    GAP_AUTH_REQ_SEC_CON_BOND = (GAP_AUTH_SEC_CON | GAP_AUTH_MITM | GAP_AUTH_BOND),

    GAP_AUTH_REQ_LAST,

    /// Mask of  authentication features without reserved flag
    GAP_AUTH_REQ_MASK = 0x1Fu,
};

/// Pairing level achieved
enum gap_pairing_lvl
{
    /// Unauthenticated pairing achieved but without bond data
    /// (meaning-less for connection confirmation)
    GAP_PAIRING_UNAUTH = 0x00u,
    /// Authenticated pairing achieved but without bond data
    /// (meaning-less for connection confirmation)
    GAP_PAIRING_AUTH = 0x04u,
    /// Secure connection pairing achieved but without bond data
    /// (meaning-less for connection confirmation)
    GAP_PAIRING_SECURE_CON = 0x0Cu,

    /// No pairing performed with peer device
    /// (meaning-less for connection confirmation)
    GAP_PAIRING_NO_BOND = 0x00u,
    /// Peer device bonded through an unauthenticated pairing.
    GAP_PAIRING_BOND_UNAUTH = 0x01u,
    /// Peer device bonded through an authenticated pairing.
    GAP_PAIRING_BOND_AUTH = 0x05u,
    /// Peer device bonded through a secure connection pairing pairing.
    GAP_PAIRING_BOND_SECURE_CON = 0x0Du,

    /// Pairing with bond data present Bit
    GAP_PAIRING_BOND_PRESENT_BIT = 0x01u,
    GAP_PAIRING_BOND_PRESENT_POS = 0x00u,

};

/// Key Distribution Flags
enum gap_kdist
{
    /// No Keys to distribute
    GAP_KDIST_NONE = 0x00,
    /// Encryption key in distribution (LTK)
    GAP_KDIST_ENCKEY = (1 << 0),
    /// IRK (ID key)in distribution
    GAP_KDIST_IDKEY = (1 << 1),
    /// CSRK(Signature key) in distribution
    GAP_KDIST_SIGNKEY= (1 << 2),
    /// Link key derivation (for BR/EDR)
    GAP_KDIST_LINKKEY= (1 << 3),

    GAP_KDIST_LAST =   (1 << 4)
};


/// GAP data types used for Extended Inquiry Response (EIR), Advertising Data (AD), Scan Response Data (SRD), Additional
/// Controller Advertising Data (ACAD), and OOB data blocks.
enum gap_ad_type
{
    /// Flag
    GAP_AD_TYPE_FLAGS = 0x01,
    /// Use of more than 16 bits UUID
    GAP_AD_TYPE_MORE_16_BIT_UUID = 0x02,
    /// Complete list of 16 bit UUID
    GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID = 0x03,
    /// Use of more than 32 bit UUD
    GAP_AD_TYPE_MORE_32_BIT_UUID = 0x04,
    /// Complete list of 32 bit UUID
    GAP_AD_TYPE_COMPLETE_LIST_32_BIT_UUID = 0x05,
    /// Use of more than 128 bit UUID
    GAP_AD_TYPE_MORE_128_BIT_UUID = 0x06,
    /// Complete list of 128 bit UUID
    GAP_AD_TYPE_COMPLETE_LIST_128_BIT_UUID = 0x07,
    /// Shortened device name
    GAP_AD_TYPE_SHORTENED_NAME = 0x08,
    /// Complete device name
    GAP_AD_TYPE_COMPLETE_NAME = 0x09,
    /// Transmit power
    GAP_AD_TYPE_TRANSMIT_POWER = 0x0A,
    /// Class of device
    GAP_AD_TYPE_CLASS_OF_DEVICE = 0x0D,
    /// Simple Pairing Hash C
    GAP_AD_TYPE_SP_HASH_C = 0x0E,
    /// Simple Pairing Randomizer
    GAP_AD_TYPE_SP_RANDOMIZER_R = 0x0F,
    /// Temporary key value
    GAP_AD_TYPE_TK_VALUE = 0x10,
    /// Out of Band Flag
    GAP_AD_TYPE_OOB_FLAGS = 0x11,
    /// Slave connection interval range
    GAP_AD_TYPE_SLAVE_CONN_INT_RANGE = 0x12,
    /// Require 16 bit service UUID
    GAP_AD_TYPE_RQRD_16_BIT_SVC_UUID = 0x14,
    /// Require 32 bit service UUID
    GAP_AD_TYPE_RQRD_32_BIT_SVC_UUID = 0x1F,
    /// Require 128 bit service UUID
    GAP_AD_TYPE_RQRD_128_BIT_SVC_UUID = 0x15,
    /// Service data 16-bit UUID
    GAP_AD_TYPE_SERVICE_16_BIT_DATA = 0x16,
    /// Service data 32-bit UUID
    GAP_AD_TYPE_SERVICE_32_BIT_DATA = 0x20,
    /// Service data 128-bit UUID
    GAP_AD_TYPE_SERVICE_128_BIT_DATA = 0x21,
    /// Public Target Address
    GAP_AD_TYPE_PUB_TGT_ADDR = 0x17,
    /// Random Target Address
    GAP_AD_TYPE_RAND_TGT_ADDR = 0x18,
    /// Appearance
    GAP_AD_TYPE_APPEARANCE = 0x19,
    /// Advertising Interval
    GAP_AD_TYPE_ADV_INTV = 0x1A,
    /// LE Bluetooth Device Address
    GAP_AD_TYPE_LE_BT_ADDR = 0x1B,
    /// LE Role
    GAP_AD_TYPE_LE_ROLE = 0x1C,
    /// Simple Pairing Hash C-256
    GAP_AD_TYPE_SPAIR_HASH = 0x1D,
    /// Simple Pairing Randomizer R-256
    GAP_AD_TYPE_SPAIR_RAND = 0x1E,
    /// Resolvable Set Identifier (Coordinated Set Identification Service)
    GAP_AD_TYPE_RSI = 0x2E,
    /// 3D Information Data
    GAP_AD_TYPE_3D_INFO = 0x3D,
    /// Broadcast Name
    GAP_AD_TYPE_BROADCAST_NAME = 0x30,
    /// Manufacturer specific data
    GAP_AD_TYPE_MANU_SPECIFIC_DATA = 0xFF,
};

/// AD Type Flag - Bit field
enum gap_ad_type_flags
{
    /// Limited discovery flag - AD Flag - bit mask
    GAP_LE_LIM_DISCOVERABLE_FLG_BIT = 0x01,
    /// Limited discovery flag - AD Flag - bit position
    GAP_LE_LIM_DISCOVERABLE_FLG_POS = 0,
    /// General discovery flag - AD Flag - bit mask
    GAP_LE_GEN_DISCOVERABLE_FLG_BIT = 0x02,
    /// General discovery flag - AD Flag - bit position
    GAP_LE_GEN_DISCOVERABLE_FLG_POS = 1,
    /// Legacy BT not supported - AD Flag - bit mask
    GAP_BR_EDR_NOT_SUPPORTED_BIT = 0x04,
    /// Legacy BT not supported - AD Flag - bit position
    GAP_BR_EDR_NOT_SUPPORTED_POS =  2,
    /// Dual mode for controller supported (BR/EDR/LE) - AD Flag - bit mask
    GAP_SIMUL_BR_EDR_LE_CONTROLLER_BIT = 0x08,
    /// Dual mode for controller supported (BR/EDR/LE) - AD Flag - bit position
    GAP_SIMUL_BR_EDR_LE_CONTROLLER_POS = 3,
};


/// @} GAP_ENUM_API

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/**
 * @defgroup GAP_STRUCT_API Structures
 * @ingroup GAP_DEFAULT_API
 * @{
 */

/// Device name
/*@TRACE*/
typedef struct gap_device_name
{
    /// Length of provided value
    uint16_t value_length;
    /// name value starting from offset to maximum length
    uint8_t value[__ARRAY_EMPTY];
} gap_device_name_t;

/// Bluetooth address
/*@TRACE*/
typedef struct gap_addr
{
    /// BD Address of device
    uint8_t addr[GAP_BD_ADDR_LEN];
} gap_addr_t;

/// Address information about a device address
/*@TRACE*/
typedef struct gap_bdaddr
{
    /// BD Address of device
    uint8_t addr[GAP_BD_ADDR_LEN];
    /// Address type of the device 0=public/1=private random (see #gap_addr_type enumeration)
    uint8_t addr_type;
} gap_bdaddr_t;

/// Out of Band Information
/*@TRACE*/
typedef struct gap_oob
{
    /// Confirm Value
    uint8_t conf[GAP_KEY_LEN];
    /// Random Number
    uint8_t rand[GAP_KEY_LEN];
} gap_oob_t;

/// Generic Security key structure
/*@TRACE*/
typedef struct gap_sec_key
{
    /// Key value
    uint8_t key[GAP_KEY_LEN];
} gap_sec_key_t;


/// @} GAP_STRUCT_API
/// @} GAP_DEFAULT_API
/// @} GAP_API

#endif // GAP_H_
