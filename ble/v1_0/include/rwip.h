/**
****************************************************************************************
*
* @file rwip.h
*
* @brief RW IP SW main module
*
* Copyright (C) RivieraWaves 2009-2023
* Release Identifier: c9ec3d22
*
*
****************************************************************************************
*/
#ifndef _RWIP_H_
#define _RWIP_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @brief Entry points of the RW IP stacks/modules
 *
 * This module contains the primitives that allow an application accessing and running the
 * RW IP protocol stacks / modules.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"          // stack configuration

#include <stdint.h>               // standard integer definitions
#include <stdbool.h>              // standard boolean definitions

#if (EMB_PRESENT)
#include "co_bt_defines.h"        // Bluetooth defines
#endif //(EMB_PRESENT)
#include "co_list.h"              // List defines


/*
 * DEFINES
 ****************************************************************************************
 */
/// Maximum value of a microsecond time stamp
#define RWIP_MAX_US_TIME                 (0xFFFFFFFFL)
/// Maximum value of a Bluetooth clock (in 312.5us half slots)
#define RWIP_MAX_CLOCK_TIME              ((1L<<28) - 1)
/// Invalid target time
#define RWIP_INVALID_TARGET_TIME         (0xFFFFFFFFL)
/// Maximum additional random increment value (range 0-3)
#define RWIP_PRIO_MAX_RAND_INC           (4)

/**
* Inverse an intra-half-slot value (in half-us), from/to following formats:
*   - A: elapsed time from the previous half-slot (in half-us)
*   - B: remaining time to the next half-slot (in half-us)
* The function from A to B or from B to A.
*  ____________________________________________________________________________________________________
*     Half-slot N-1            |             Half-slot N              |             Half-slot N+1
*  ____________________________|______________________________________|________________________________
*                              |<---------- A ---------->|<---- B --->|
*  ____________________________|______________________________________|________________________________
*/
#define HALF_SLOT_INV(x)  (HALF_SLOT_SIZE - (x) - 1)


/// result of sleep state.
enum rwip_sleep_state
{
    /// Some activity pending, can not enter in sleep state
    RWIP_ACTIVE    = 0,
    /// CPU can be put in sleep state (no RTOS), or pend for next event (RTOS)
    RWIP_IDLE,
    /// IP could enter in deep sleep
    RWIP_DEEP_SLEEP,
};


/// Definition of the bits preventing the system from sleeping
enum prevent_sleep
{
    /// Flag indicating that the wake up process is ongoing
    RW_WAKE_UP_ONGOING                 = 0x000001,
    /// Flag indicating the IP is in sleep, to avoid running sleep algorithm while already entering sleep
    RW_DEEP_SLEEP                      = 0x000002,
    /// Flag indicating that an encryption is ongoing
    RW_CRYPT_ONGOING                   = 0x000004,
    /// Flag indicating that controller shall not sleep due to not CSB LPO_Allowed
    RW_CSB_NOT_LPO_ALLOWED             = 0x000008,
    /// Flag indicating the MWS/WLAN Event Generator is in operation
    RW_MWS_WLAN_EVENT_GENERATOR_ACTIVE = 0x000010,
    /// Flag to indicate that platform does not support deep sleep
    RW_PLF_DEEP_SLEEP_DISABLED         = 0x000020,
    /// Flag to indicate that a baseband frame is ongoing
    RW_BB_FRAME_ONGOING                = 0x000040,
    /// Flag to indicate that BLE Hopping computation on-going
    RW_HOP_CALC_ONGOING                = 0x000080,
    /// Flag to indicate that BT is in active mode (ACL, SCO)
    RW_BT_ACTIVE_MODE                  = 0x000100,
    /// Flag to indicate that BLE requires active mode
    RW_BLE_ACTIVE_MODE                 = 0x000200,
    /// Flag indicating that an TX transfer is ongoing on Transport Layer - First Interface
    RW_TL_1_TX_ONGOING                 = 0x000400,
    /// Flag indicating that an TX transfer is ongoing on Transport Layer - Second Interface
    RW_TL_2_TX_ONGOING                 = 0x000800,
    /// Flag indicating that an RX transfer is ongoing on Transport Layer - First Interface
    RW_TL_1_RX_ONGOING                 = 0x001000,
    /// Flag indicating that an RX transfer is ongoing on Transport Layer - Second Interface
    RW_TL_2_RX_ONGOING                 = 0x002000,
    /// Flag indicating that an HW accelerator encryption is ongoing
    RW_CRYPT_ACCELERATOR_SEC_ONGOING   = 0x040000,
    /// Flag to indicate that 15.4 requires active mode
    RW_MAC154_ACTIVE_MODE              = 0x080000,
};

/// Parameters - Possible Returned Status
enum PARAM_STATUS
{
    /// PARAM status OK
    PARAM_OK,
    /// generic PARAM status KO
    PARAM_FAIL,
    /// PARAM ID unrecognized
    PARAM_ID_NOT_DEFINED,
    /// No space for PARAM
    PARAM_NO_SPACE_AVAILABLE,
    /// Length violation
    PARAM_LENGTH_OUT_OF_RANGE,
    /// PARAM parameter locked
    PARAM_PARAM_LOCKED,
    /// PARAM corrupted
    PARAM_CORRUPT
};

/**
 * External interface type types.
 */
enum rwip_eif_types
{
    /// Host Controller Interface - Controller part
    RWIP_EIF_HCIC,

    /// Host Controller Interface - Host part
    RWIP_EIF_HCIH,

    /// Application Host interface
    RWIP_EIF_AHI,
};


/// Enumeration of External Interface status codes
enum rwip_eif_status
{
    /// EIF status OK
    RWIP_EIF_STATUS_OK,
    /// EIF status KO
    RWIP_EIF_STATUS_ERROR,

#if (BLE_EMB_PRESENT == 0)
    /// External interface detached
    RWIP_EIF_STATUS_DETACHED,
    /// External interface attached
    RWIP_EIF_STATUS_ATTACHED,
#endif // (BLE_EMB_PRESENT == 0)
};

/// Enumeration of RF modulations
enum rwip_rf_mod
{
    MOD_GFSK  = 0x01,
    MOD_DQPSK = 0x02,
    MOD_8DPSK = 0x03,
};

/// Enumeration of txpwr_cs_get search options
enum rwip_txpwr_cs_get_opt
{
    TXPWR_CS_LOWER,
    TXPWR_CS_HIGHER,
    TXPWR_CS_NEAREST,
};

#if RW_DEBUG
/// Assert type
/*@TRACE*/
enum assert_type
{
    ASSERT_TYPE_WARNING = 0,
    ASSERT_TYPE_ERROR   = 1,
};
#endif //RW_DEBUG

/// Rx Status
/*@TRACE*/
enum rwip_rx_status
{
    RWIP_RX_OK,
    RWIP_RX_NO_SYNC,
    RWIP_RX_CRC_ERROR,
    RWIP_RX_OTHER_ERROR,
};

/// Bluetooth protocol type
enum rwip_prot
{
    RWIP_PROT_BT = 0,
	RWIP_PROT_BLE,
    RWIP_PROT_MAC154,
};

/*
 * MACROS
 ****************************************************************************************
 */

/// Radiative Transmit Power from CS value (in dBm)
#define RWIP_TX_PWR_DBM_GET(txpwr_idx) (rwip_rf.txpwr_dbm_get((uint8_t)txpwr_idx) + rwip_tx_path_comp_get()/10)

///Get the nearest higher/lower TX power to the requested value
#define RWIP_TX_PWR_GET(tx_pwr_req, option) (RWIP_TX_PWR_DBM_GET(rwip_rf.txpwr_cs_get(((int8_t)tx_pwr_req - rwip_tx_path_comp_get()/10), (uint8_t)option)))

/// Fetch power control flags
#define RWIP_TX_PWR_DBM_MAX_GET(prot_type, actv_id, phy) (RWIP_TX_PWR_DBM_GET(rwip_rf.txpwr_max_get(prot_type, actv_id, phy)))
#define RWIP_TX_PWR_DBM_MIN_GET() (RWIP_TX_PWR_DBM_GET(rwip_rf.txpwr_min))


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Time information
/*@TRACE*/
typedef struct
{
    /// Integer part of the time (in half-slot)
    uint32_t hs;
    /// Fractional part of the time (in half-us) (range: 0-624)
    uint16_t hus;
} rwip_time_t;

/// API functions of the RF driver that are used by the BLE or BT software
struct rwip_rf_api
{
    /// Function called upon HCI reset command reception
    void (*reset)(void);
    /// Function called to enable/disable force AGC mechanism (true: en / false : dis)
    void (*force_agc_enable)(bool);
    /// Function called when TX power has to be decreased
    bool (*txpwr_dec)(uint8_t  *);
    /// Function called when TX power has to be increased
    bool (*txpwr_inc)(uint8_t *);
    /// Function called when TX power has to be set to max
    void (*txpwr_max_set)(uint8_t *);
    /// Function called to convert a TX power CS power field into the corresponding value in dBm
    int8_t (*txpwr_dbm_get)(uint8_t);
    /// Function called to convert a power in dBm into a control structure tx power field
    uint8_t (*txpwr_cs_get)(int8_t, uint8_t);
    /// Function called to convert the RSSI read from the control structure into a real RSSI
    int8_t (*rssi_convert)(uint8_t);
    /// Function used to read a RF register
    uint32_t (*reg_rd)(uint32_t);
    /// Function used to write a RF register
    void (*reg_wr)(uint32_t, uint32_t);
    /// Function called to put the RF in deep sleep mode
    void (*sleep)(void);
    /// Index of minimum TX power
    uint8_t txpwr_min;
    /** Function called to provide the index of maximum TX power according to activity and PHY
     *  The parameters are respectively
     *  - protocol type (see #rwip_prot)
     *  - activity index
     *      - BT: [0, EM_BT_CS_INDEX_MAX]
     *          - Connection activity [0 to MAX_NB_ACTIVE_ACL-1]
     *          - Non-connection activity [MAX_NB_ACTIVE_ACL to EM_BT_CS_INDEX_MAX-1] in order
     *            for Inquiry / Inquiry scan / Active broadcast / Synchronization train / Synchronization scan / CPB transmission / CPB reception
     *      - BLE: [0 to BLE_ACTIVITY_MAX]
     *          - Specific activity [0 to BLE_ACTIVITY_MAX-1]
     *          - If BLE_ACTIVITY_MAX (the parameter phy will be ignored),
     *            the function should return the maximum TX power of all activities and all PHYs.
     *  - phy
     *      - BT: see Packet Type Table defines
     *      - BLE: see #le_phy_value
     **/
    uint8_t (*txpwr_max_get)(uint8_t, uint8_t, uint8_t);
    /// RSSI high threshold ('real' signed value in dBm)
    int8_t rssi_high_thr;
    /// RSSI low threshold ('real' signed value in dBm)
    int8_t rssi_low_thr;
    /// RF wakeup delay (in slots)
    uint8_t wakeup_delay;
    #if defined(CFG_MAC154_EMB)
    /// Function called to set Energy Detection Threshold
    void (*ed_thr_set)(int8_t);
    #endif //CFG_MAC154_EMB
};

/// API functions of the parameters that are used by the BLE or BT software
struct rwip_param_api
{
    /**
     * Get a parameter value
     * @param[in]      param_id     Parameter identifier
     * @param[in/out]  lengthPtr    Pointer to the length of the parameter (input: contain max length, output contain the effective param length, in bytes)
     * @param[out]     buf          Pointer to the buffer be filled with the parameter value
     * @return  status              0: success | >0 : error
     */
    uint8_t (*get) (uint8_t param_id, uint8_t * lengthPtr, uint8_t *buf);

    /**
     * Set a parameter value
     * @param[in]      param_id     Parameter identifier
     * @param[in/out]  length       Length of the parameter (in bytes)
     * @param[out]     buf          Pointer to the buffer containing the parameter value
     * @return  status              0: success | >0 : error
     */
    uint8_t (*set) (uint8_t param_id, uint8_t length, uint8_t *buf);

    /**
     * Delete a parameter
     * @param[in]      param_id     Parameter identifier
     * @return  status              0: success | >0 : error
     */
    uint8_t (*del) (uint8_t param_id);
};

/// Internal API for priority
struct rwip_prio
{
    ///value
    uint8_t value;
    ///Increment
    uint8_t increment;
};
/**
 ****************************************************************************************
 * @brief Function called when packet transmission/reception is finished.

 * @param[in]  dummy  Dummy data pointer returned to callback when operation is over.
 * @param[in]  status Ok if action correctly performed, else reason status code.
 *****************************************************************************************
 */
typedef void (*rwip_eif_callback) (void*, uint8_t);

/**
 * Transport layer communication interface.
 */
struct rwip_eif_api
{
    /**
     *************************************************************************************
     * @brief Starts a data reception.
     *
     * @param[out] bufptr      Pointer to the RX buffer
     * @param[in]  size        Size of the expected reception
     * @param[in]  callback    Pointer to the function called back when transfer finished
     * @param[in]  dummy       Dummy data pointer returned to callback when reception is finished
     *************************************************************************************
     */
    void (*read) (uint8_t *bufptr, uint32_t size, rwip_eif_callback callback, void* dummy);

    /**
     *************************************************************************************
     * @brief Starts a data transmission.
     *
     * @param[in]  bufptr      Pointer to the TX buffer
     * @param[in]  size        Size of the transmission
     * @param[in]  callback    Pointer to the function called back when transfer finished
     * @param[in]  dummy       Dummy data pointer returned to callback when transmission is finished
     *************************************************************************************
     */
    void (*write)(uint8_t *bufptr, uint32_t size, rwip_eif_callback callback, void* dummy);

    /**
     *************************************************************************************
     * @brief Enable Interface flow.
     *************************************************************************************
     */
    void (*flow_on)(void);

    /**
     *************************************************************************************
     * @brief Disable Interface flow.
     *
     * @return True if flow has been disabled, False else.
     *************************************************************************************
     */
    bool (*flow_off)(void);
};

#if (EMB_PRESENT)
/// BLE channel assessment data
struct rwip_ch_assess_data
{
    /// Timestamp of last received packet for each frequency
    uint32_t timestamp[AFH_NB_CHANNEL_MAX];

    /// Channel quality level for each frequency
    int8_t level[AFH_NB_CHANNEL_MAX];
};
#endif //EMB_PRESENT

typedef struct rwip_aes_client rwip_aes_client_t;

/**
 ****************************************************************************************
 * @brief Callback function that handle AES result
 *
 * @param[in] p_client Pointer to AES client
 * @param[in] status   AES execution status (see #co_error enumeration)
 * @param[in] p_result Pointer to array that contains 16 bytes AES Result)
 *
 ****************************************************************************************
 */
typedef void (*rwip_aes_result_handler_cb)(rwip_aes_client_t* p_client, uint8_t status, const uint8_t* p_aes_res);

/// Structure definition of AES client
struct rwip_aes_client
{
    /// List Header for AES queuing
    co_list_hdr_t  hdr;
    /// Callback that handle AES result
    rwip_aes_result_handler_cb cb_result;
    /// AES Encryption key must be 16 bytes
    const uint8_t* p_key;
    /// 16 bytes value array to encrypt or decrypt using AES
    const uint8_t* p_val;
    /// True to encrypt, False to decrypt
    bool cipher;
};

/*
 * VARIABLE DECLARATION
*****************************************************************************************
 */

/// API for RF driver
extern struct rwip_rf_api rwip_rf;
/// API for parameters
extern struct rwip_param_api rwip_param;
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)
/// API for dual mode priority
extern struct rwip_prio rwip_priority[RWIP_PRIO_IDX_MAX];
#if (RW_WLAN_COEX || RW_MWS_COEX)
/// API for COEX configuration
extern const uint8_t rwip_coex_cfg[RWIP_COEX_CFG_MAX];
#endif //(RW_WLAN_COEX || RW_MWS_COEX)
/// Programming delay, margin for programming the baseband in advance of each activity (in half-slots)
extern uint8_t rwip_prog_delay;
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT || mAC154_EMB_PRESENT)

/*
 * MACROS
 ****************************************************************************************
 */

/// Get Event status flag
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)
#define RWIP_PRIO_INC(prio_idx) (rwip_priority[prio_idx].increment + ((rwip_priority[prio_idx].increment != 0) ? CO_MOD(co_rand_byte(), RWIP_PRIO_MAX_RAND_INC) : 0))
/// Adds two RWIP priority values, limit at maximum
#define RWIP_PRIO_ADD_2(prio, inc)      co_min(RWIP_PRIO_MAX, (uint16_t)(prio) + (inc))
#if (RW_WLAN_COEX || RW_MWS_COEX)
#define RWIP_COEX_GET(coex_cfg_idx, bit_field) \
                (uint8_t)(((rwip_coex_cfg[RWIP_COEX_ ## coex_cfg_idx ##_IDX]) >> RWIP_ ## bit_field ## _POS ) & RWIP_COEX_BIT_MASK)
#else //!(RW_WLAN_COEX || RW_MWS_COEX)
#define RWIP_COEX_GET(coex_cfg_idx, bit_field) 0
#endif //(RW_WLAN_COEX || RW_MWS_COEX)
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)

/*
 * FUNCTION DECLARATION
*****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes uart intefaces.
 *
 ****************************************************************************************
 */

// PLF_SPARK_EXTSYS0
// [HCI], AHI and H4TL
void rwip_uart_init(bool init_hci);

/**
 ****************************************************************************************
 * @brief Initializes the RW BT SW.
 *
 ****************************************************************************************
 */
void rwip_init(uint32_t error);

/**
 ****************************************************************************************
 * @brief Reset the RW BT SW.
 *
 ****************************************************************************************
 */
void rwip_reset(void);



#if (BT_EMB_PRESENT)

#if PCA_SUPPORT
/**
 ****************************************************************************************
 * @brief Check if clock dragging limitation
 *
 * @return    true if clock dragging must be used
 ****************************************************************************************
 */
bool rwip_pca_clock_dragging_only(void);
#endif //PCA_SUPPORT
#endif // (BT_EMB_PRESENT)

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)
#if (RW_MWS_COEX)
/**
 ****************************************************************************************
 * @brief Enable/Disable the MWS coexistence interface.
 *
 * @param[in]   en     Coexistence enable setting
 *
 ****************************************************************************************
 */
void rwip_mwscoex_set(bool en);
#endif //RW_MWS_COEX

#if (RW_WLAN_COEX)
/**
 ****************************************************************************************
 * @brief Enable/Disable the Wireless LAN coexistence interface.
 *
 * @param[in]   en     Coexistence enable setting
 *
 ****************************************************************************************
 */
void rwip_wlcoex_set(bool en);
#endif //RW_WLAN_COEX
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Function to implement in platform in order to retrieve each external interface API
 *
 * @param[in] idx External interface index
 *
 * @return External interface api structure
 ****************************************************************************************
 */
extern const struct rwip_eif_api* rwip_eif_get(uint8_t idx);

#if RW_DEBUG
/**
 ****************************************************************************************
 * @brief Raises an assertion message to the control interface (if present)
 *
 * @param[in] file    File name
 * @param[in] line    Line number
 * @param[in] param0  Parameter 0 (custom value given by the assert instruction)
 * @param[in] param1  Parameter 1 (custom value given by the assert instruction)
 * @param[in] type    0: warning / 1: error
 ****************************************************************************************
 */
void rwip_assert(const char * file, int line, int param0, int param1, uint8_t type);
#endif //RW_DEBUG

#if ((EMB_PRESENT && BLE_ISO_PRESENT) || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Request a transfer of data between SDU and PDU buffers
 *
 * @param[in] p_dst_addr    Destination address
 * @param[in] p_src_addr    Source address
 * @param[in] size          Size of the transfer (in octets)
 ****************************************************************************************
 */
void rwip_data_transfer (void* p_dst_addr, const void* p_src_addr, uint16_t size);
#endif // ((EMB_PRESENT && BLE_ISO_PRESENT) || BT_EMB_PRESENT)


/* **************************************************************************************
 * Driver functions
 * **************************************************************************************
 */

#if (EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Retrieved sampled time
 *
 * @return Current time sampled (see #rwip_time_t)
 ****************************************************************************************
 */
rwip_time_t rwip_time_get(void);
#endif // (EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Retrieved time in microsecond
 *
 * @return Current microsecond time sampled
 ****************************************************************************************
 */
uint32_t rwip_us_time_get(void);

#if (BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Set current time
 *
 * @param clock value in half-slots
 ****************************************************************************************
 */
void rwip_time_set(uint32_t clock);

/**
 ****************************************************************************************
 * @brief Adjust current time
 *
 * @param adjustment value in us
 ****************************************************************************************
 */
void rwip_time_adj(int16_t clk_adj_us);
#endif // (BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Set the microsecond target timer
 *
 * @note if target is RWIP_INVALID_TARGET_TIME, no timer is programmed
 *
 * @param[in] us_target      1us Timer target value (in microseconds)
 ****************************************************************************************
 */
void rwip_timer_us_set(uint32_t us_target);

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Set the an alarm target timer
 *
 * @note if target is RWIP_INVALID_TARGET_TIME, no timer is programmed
 *
 * @param[in] target         Half Slot Timer target value
 * @param[in] half_us_delay  Half us timer delay in corresponding half slot (range [0:624])
 ****************************************************************************************
 */
void rwip_timer_alarm_set(uint32_t target, uint32_t half_us_delay);

/**
 ****************************************************************************************
 * @brief Set the an Arbiter target timer
 *
 * @note if target is RWIP_INVALID_TARGET_TIME, no timer is programmed
 *
 * @param[in] target         Half Slot Timer target value
 * @param[in] half_us_delay  Half us timer delay in corresponding half slot (range [0:624])
 ****************************************************************************************
 */
void rwip_timer_arb_set(uint32_t target, uint32_t half_us_delay);
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Start AES encryption
 *
 * The exchange memory must be filled before calling this function.
 * This function expect to be called from a BLE Module
 *
 * @param[in] p_client      Pointer to AES client
 * @param[in] val           16 bytes value array to encrypt using AES
 * @param[in] cipher        True to cipher data; de-cipher data otherwise
 ****************************************************************************************
 */
void rwip_aes(rwip_aes_client_t* p_client);

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Request a Software interrupt to be triggered
 ****************************************************************************************
 */
void rwip_sw_int_req(void);
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Invoke the sleep function.
 *
 * @return  sleep status (@see enum rwip_sleep_state)
 ****************************************************************************************
 */
uint8_t rwip_sleep(void);

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)

#if !RW_DEBUG
/**
 ****************************************************************************************
 * @brief Report HW errors to Host.
 *
 * @param[in] error_base        Base opcode for HW block (BLE/BT/BTDM)
 * @param[in] error_type_stat   Error Type status reported by HW
 ****************************************************************************************
 */
void rwip_hw_error_report(uint8_t error_base, uint32_t error_type_stat);
#endif // !RW_DEBUG

/**
 ****************************************************************************************
 * @brief Handle the common core interrupts.
 ****************************************************************************************
 */
void rwip_isr(void);
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT || MAC154_EMB_PRESENT)

#if ((BT_EMB_PRESENT && BLE_EMB_PRESENT) || (BLE_EMB_PRESENT && MAC154_EMB_PRESENT))
/**
 ****************************************************************************************
 * @brief Handle the btdm-specific core interrupts.
 ****************************************************************************************
 */
void rwip_btdm_isr(void);
#endif // ((BT_EMB_PRESENT && BLE_EMB_PRESENT) || (BLE_EMB_PRESENT && MAC154_EMB_PRESENT))

/**
 ****************************************************************************************
 * @brief Set a bit in the prevent sleep bit field, in order to prevent the system from
 *        going to sleep
 *
 * @param[in] prv_slp_bit   Bit to be set in the prevent sleep bit field
 ****************************************************************************************
 */
void rwip_prevent_sleep_set(uint32_t prv_slp_bit);

/**
 ****************************************************************************************
 * @brief Clears a bit in the prevent sleep bit field, in order to allow the system
 *        going to sleep
 *
 * @param[in] prv_slp_bit   Bit to be cleared in the prevent sleep bit field
 ****************************************************************************************
 */
void rwip_prevent_sleep_clear(uint32_t prv_slp_bit);


#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Get the current clock drift of the system
 *
 * The drift depends on the system activity.
 * Note:
 *  - on dual-mode device the drift cannot exceed 250ppm
 *  - on BLE device the drift cannot exceed 500ppm
 *
 * @return  Current clock drift (in ppm)
 ****************************************************************************************
 */
uint16_t rwip_current_drift_get(void);

/**
 ****************************************************************************************
 * @brief Get the maximum clock drift of the system
 *
 * @return  Maximum clock drift (in ppm)
 ****************************************************************************************
 */
uint16_t rwip_max_drift_get(void);

/**
 ****************************************************************************************
 * @brief Get sleep clock accuracy of the system
 *
 * @return     sca     Sleep clock accuracy, physical low power clock accuracy (@see enum SCA)
 ****************************************************************************************
 */
uint8_t rwip_sca_get(void);
#endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/**
 ****************************************************************************************
 * @brief Process pending events.
 *
 ****************************************************************************************
 */
void rwip_process(void);

#if (RTOS_SUPPORT)
/**
 ****************************************************************************************
 * @brief Pend on RWIP event from RTOS.
 *
 ****************************************************************************************
 */
void rwip_evt_pend(void);

/**
 ****************************************************************************************
 * @brief Post an RWIP event to RTOS if BT Task is inactive.
 *
 ****************************************************************************************
 */
void rwip_evt_post(void);

#endif // (RTOS_SUPPORT)

#if (EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Convert Bluetooth time (Half slot + Half us) provided in parameters to a microsecond time stamp value
 *
 * @param[in] hs   Bluetooth time in half slot
 * @param[in] hus  Bluetooth time in half microseconds
 *
 * @return Time in microseconds
 ****************************************************************************************
 */
uint32_t rwip_bt_time_to_us(uint32_t hs, uint16_t hus);

/**
 ****************************************************************************************
 * @brief Convert microsecond time value to Bluetooth time (Half slot + Half us)
 *
 * @param[in]  us_time  Time in microseconds
 * @param[out] p_hs     Bluetooth time in half slot
 * @param[out] p_hus    Bluetooth time in half microseconds
 ****************************************************************************************
 */
void rwip_us_to_bt_time(uint32_t ref_time, uint32_t* p_hs, uint16_t* p_hus);
#endif // (EMB_PRESENT)

#if (BLE_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Assess a BLE RF channel
 *
 * @param[in]  channel       Channel index (range:[0..37])
 * @param[in]  rx_status     (see #lld_rx_status enumeration)
 * @param[in]  rssi_cs       RSSI read from the CS
 * @param[in]  timestamp     Timestamp of the assessment
 * @param[in]  connected     True: expecting to receive a packet (eg. RX connection) |
 *                           False: not expecting to receive a packet (eg. Channel Scan)
 *
 * @return status            0: success | 1-255: error
 ****************************************************************************************
 */
void rwip_channel_assess_ble(uint8_t channel, uint8_t rx_status, uint8_t rssi_cs, uint32_t timestamp, bool connected);
#endif //BLE_EMB_PRESENT

#if (EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Get channel assessment
 *
 * @return  Pointer to channel assessment data
 ****************************************************************************************
 */
struct rwip_ch_assess_data* rwip_ch_assess_data_get(void);

/**
 ****************************************************************************************
 * @brief Insert status of a channel
 *
 * @param[in]  channel       Channel index (range:[0..79])
 * @param[in]  rx_status     (see #lld_rx_status enumeration)
 * @param[in]  rssi_cs       RSSI read from the CS
 * @param[in]  timestamp     Timestamp of the assessment (in half-slots)
 * @param[in]  connected     True: expecting to receive a packet (eg. RX connection) |
 *                           False: not expecting to receive a packet (eg. Channel Scan)
 *
 * @return status            0: success | 1-255: error
 ****************************************************************************************
 */
void rwip_channel_stat_insert(uint8_t channel, uint8_t rx_status, uint8_t rssi_cs, uint32_t timestamp, bool connected);
#endif // (EMB_PRESENT)

#if (BT_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Assess a BT RF channel
 *
 * @param[in]  channel       Channel index (range:[0..79])
 * @param[in]  rx_status     (see #lld_rx_status enumeration)
 * @param[in]  rssi_cs       RSSI read from the CS
 * @param[in]  timestamp     Timestamp of the assessment (in half-slots)
 *
 * @return status            0: success | 1-255: error
 ****************************************************************************************
 */
void rwip_channel_assess_bt(uint8_t channel, uint8_t rx_status, uint8_t rssi_cs, uint32_t timestamp);

/**
 ****************************************************************************************
 * @brief Get the BT local channel classification based channel assessment
 *
 * @param[out]  ch_class      Pointer to BT channel classification
 ****************************************************************************************
 */
void rwip_ch_class_bt_get(struct bt_ch_class* ch_class);

/**
 ****************************************************************************************
 * @brief Get the BT local channel map based channel assessment
 *
 * @param[out]  ch_map        Pointer to BT channel map
 ****************************************************************************************
 */
void rwip_ch_map_bt_get(struct bt_ch_map* ch_map);

/**
 ****************************************************************************************
 * @brief Re-Introduce some channels in the BT map based on the Host classification
 *
 * @param[in]  nb_ch_to_add  Number of channels to introduce on the channel map
 * @param[in]  host_ch_class Pointer to Host channel classification
 * @param[out] ch_map        Pointer to BT channel map
 ****************************************************************************************
 */
void rwip_activate_channels_ch_map_bt(uint8_t nb_ch_to_add, struct bt_ch_map* host_ch_class, struct bt_ch_map* ch_map);
#endif //BT_EMB_PRESENT

#if (EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief  enable/disable channel assessment scheme
 *
 * @param[in] en              true if enabled, false otherwise
 *****************************************************************************************
 */
void rwip_ch_ass_en_set(bool ch_ass_en);

/**
 ****************************************************************************************
 * @brief  returns status of channel assessment scheme (enabled or disabled)
 *****************************************************************************************
 */
bool rwip_ch_ass_en_get(void);

/**
 ****************************************************************************************
 * @brief Get the LE local channel classification based channel assessment
 *
 * @param[out]  ch_class      Pointer to LE channel classification
 ****************************************************************************************
 */
void rwip_ch_class_ble_get(struct le_ch_class* ch_class);

/**
 ****************************************************************************************
 * @brief Get the LE local channel map based channel assessment
 *
 * @param[out]  ch_map        Pointer to LE channel map
 ****************************************************************************************
 */
void rwip_ch_map_ble_get(struct le_ch_map* ch_map);

/**
 ****************************************************************************************
 * @brief Re-Introduce some channels in the LE map based on the Host classification
 *
 * @param[in]  nb_ch_to_add  Number of channels to introduce on the channel map
 * @param[in]  host_ch_class Pointer to Host channel classification
 * @param[out] ch_map        Pointer to LE channel map
 ****************************************************************************************
 */
void rwip_ch_map_ble_fill(uint8_t nb_ch_to_add, struct le_ch_map* ch_map, struct le_ch_map* host_ch_class);

#if (BLE_CH_SCAN_SUPPORT)
/**
 ****************************************************************************************
 * @brief Get channel map for channel scan
 *
 * @param[out]  ch_map     Channel Map
 ****************************************************************************************
 */
void rwip_ch_map_scan_get(struct le_ch_map* ch_map);
#endif //(BLE_CH_SCAN_SUPPORT)

/**
 ****************************************************************************************
 * @brief Set RF Path Compensation
 *
 * @param[in]  tx_path_comp     TX path compensation value (in 0.1 dB)
 * @param[in]  rx_path_comp     RX path compensation value (in 0.1 dB)
 ****************************************************************************************
 */
void rwip_path_comp_set(int16_t tx_path_comp, int16_t rx_path_comp);

/**
 ****************************************************************************************
 * @brief Get RF Tx Path Compensation
 *
 * @param[out]  tx_path_comp     TX path compensation value (in 0.1 dB)
 ****************************************************************************************
 */
int16_t rwip_tx_path_comp_get(void);

/**
 ****************************************************************************************
 * @brief Get RF Rx Path Compensation
 *
 * @param[out]  rx_path_comp     RX path compensation value (in 0.1 dB)
 ****************************************************************************************
 */
int16_t rwip_rx_path_comp_get(void);

#if (PLF_SPARK_EXTSYS0)
/**
 ****************************************************************************************
 * @brief Store runtime BLE&MAC154&Multimode registers to retention memory to be kept
 *  over warm boot
 ****************************************************************************************
 */
void alif_pm_suspend(void);

/**
 ****************************************************************************************
 * @brief Restore runtime BLE&MAC154&Multimode registers from retention memory
 ****************************************************************************************
 */
void alif_pm_resume(void);

/**
 ****************************************************************************************
 * @brief Initialize RF registers
 *
 * @note Call directly only after warm boot - called by rf_init() otherwise
 ****************************************************************************************
 */
void spark_transceiver_init(void);

#endif // PLF_SPARK_EXTSYS0
#endif // (EMB_PRESENT)
///@} ROOT

#endif // _RWIP_H_
