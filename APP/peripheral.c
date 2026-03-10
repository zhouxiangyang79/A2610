/********************************** (C) COPYRIGHT *******************************
 * File Name          : peripheral.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/10
 * Description        : ����ӻ�������Ӧ�ó��򣬳�ʼ���㲥���Ӳ�����Ȼ��㲥������������
 *                      ����������Ӳ�����ͨ���Զ������������
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "peripheral.h"
#include "CONFIG.h"
#include "HAL.h"
#include "devinfoservice.h"
#include "gattprofile.h"
#include "parameter.h"
#include <string.h>

uint8_t cmd = 0;
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD 1600

// How often to perform read rssi event
#define SBP_READ_RSSI_EVT_PERIOD 3200

// Parameter update delay
#define SBP_PARAM_UPDATE_DELAY 6400

// PHY update delay
#define SBP_PHY_UPDATE_DELAY 2400

// What is the advertising interval when device is discoverable (units of 625us, 80=50ms)
#define DEFAULT_ADVERTISING_INTERVAL 80

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 6=7.5ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL 6

// Maximum connection interval (units of 1.25ms, 100=125ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL 100

// Slave latency to use parameter update
#define DEFAULT_DESIRED_SLAVE_LATENCY 1

// Supervision timeout value (units of 10ms, 100=1s)
#define DEFAULT_DESIRED_CONN_TIMEOUT 100

// Company Identifier: WCH
#define WCH_COMPANY_ID 0x07D7

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
//void Jump_OTA(void);
/*********************************************************************
 * LOCAL VARIABLES
 */
const uint8_t num[16] = "0123456789ABCDEF";
static uint8_t mac_ascii[12];
static uint8_t Peripheral_TaskID = INVALID_TASK_ID; // Task ID for internal task/event processing

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
        // complete name
        0x11,// length of this data
        GAP_ADTYPE_LOCAL_NAME_COMPLETE, 'B', 'Y', 'D', '-', '0', '0', '0', '0',
        '0', '0', '0', '0', '0', '0', '0', '0',

        // connection interval range
        0x05,// length of this data
        GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE, LO_UINT16(
                DEFAULT_DESIRED_MIN_CONN_INTERVAL), // 100ms
        HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL), LO_UINT16(
                DEFAULT_DESIRED_MAX_CONN_INTERVAL), // 1s
        HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

        // Tx power level
        0x02,// length of this data
        GAP_ADTYPE_POWER_LEVEL, 0
// 0dBm
        };

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertising)
static uint8_t advertData[] =
{
// Flags; this sets the device to use limited discoverable
// mode (advertises for 30 seconds at a time) instead of general
// discoverable mode (advertises indefinitely)
        0x02,// length of this data
        GAP_ADTYPE_FLAGS,
        DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

        // service UUID, to notify central devices what services are included
        // in this peripheral
        0x03,// length of this data
        GAP_ADTYPE_16BIT_MORE, // some of the UUID's, but not all
        LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID) };

// GAP GATT Attributes
uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "BYD-000000000000";

// Connection item list
static peripheralConnItem_t peripheralConnList;

static uint8_t peripheralMTU = ATT_MTU_SIZE;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void
Peripheral_ProcessTMOSMsg(tmos_event_hdr_t* pMsg);
static void
peripheralStateNotificationCB(gapRole_States_t newState, gapRoleEvent_t* pEvent);
static void
performPeriodicTask(void);
static void
simpleProfileChangeCB(uint8_t paramID, uint8_t* pValue, uint16_t len);
static void
peripheralParamUpdateCB(uint16_t connHandle, uint16_t connInterval,
        uint16_t connSlaveLatency, uint16_t connTimeout);
static void
peripheralInitConnItem(peripheralConnItem_t* peripheralConnList);
static void
peripheralRssiCB(uint16_t connHandle, int8_t rssi);
static void
peripheralChar4Notify(uint8_t* pValue, uint16_t len);
static void
peripheralChar1Notify(uint8_t* pValue, uint16_t len);
/*********************************************************************
 * PROFILE CALLBACKS
 */
void print_data(void)
{
    PRINT("cnnnid: %x\n", peripheralConnList.connHandle);
}
void ble_on(void)
{
    uint8_t initial_advertising_enable = TRUE;
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
            &initial_advertising_enable);
}
void ble_off(void)
{

    GAPRole_TerminateLink(peripheralConnList.connHandle);
    uint8_t initial_advertising_enable = FALSE;
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
            &initial_advertising_enable);
}
// GAP Role Callbacks
static gapRolesCBs_t Peripheral_PeripheralCBs =
{ peripheralStateNotificationCB, // Profile State Change Callbacks
        peripheralRssiCB, // When a valid RSSI is read from controller (not used by application)
        peripheralParamUpdateCB };

// Broadcast Callbacks
static gapRolesBroadcasterCBs_t Broadcaster_BroadcasterCBs =
{
NULL, // Not used in peripheral role
        NULL
// Receive scan request callback
        };

// GAP Bond Manager Callbacks
static gapBondCBs_t Peripheral_BondMgrCBs =
{
NULL, // Passcode callback (not used by application)
        NULL
// Pairing / Bonding state Callback (not used by application)
        };

// Simple GATT Profile Callbacks
static simpleProfileCBs_t Peripheral_SimpleProfileCBs =
{ simpleProfileChangeCB
// Characteristic value change callback
        };
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Peripheral_Init
 *
 * @brief   Initialization function for the Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
uint8_t ble_advertising_enable = TRUE;
uint32_t ble_passkey = 0;

void Peripheral_Init()
{
    Peripheral_TaskID = TMOS_ProcessEventRegister(Peripheral_ProcessEvent);
    uint8_t mac[6];
    GetMACAddress(mac);

    mac_ascii[10] = num[mac[0] / 16];
    mac_ascii[11] = num[mac[0] % 16];
    mac_ascii[8] = num[mac[1] / 16];
    mac_ascii[9] = num[mac[1] % 16];
    mac_ascii[6] = num[mac[2] / 16];
    mac_ascii[7] = num[mac[2] % 16];
    mac_ascii[4] = num[mac[3] / 16];
    mac_ascii[5] = num[mac[3] % 16];
    mac_ascii[2] = num[mac[4] / 16];
    mac_ascii[3] = num[mac[4] % 16];
    mac_ascii[0] = num[mac[5] / 16];
    mac_ascii[1] = num[mac[5] % 16];
    tmos_memcpy(scanRspData + 6, mac_ascii, 12);
    tmos_memcpy(attDeviceName + 4, mac_ascii, 12);
//  if (para1.bluetoothswitch == 0)
//  {
//      ble_advertising_enable = FALSE;
//  }
//  else
//  {
//      ble_advertising_enable = TRUE;
//  }

    // Setup the GAP Peripheral Role Profile
    {
        uint8_t initial_advertising_enable = ble_advertising_enable;
        uint16_t desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
        uint16_t desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;

        // Set the GAP Role Parameters
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                &initial_advertising_enable);
        GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                scanRspData);
        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData),
                advertData);
        GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                &desired_min_interval);
        GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                &desired_max_interval);
    }

    // Set the GAP Characteristics
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

    {
        uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

        // Set advertising interval
        GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
        GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);

        // Enable scan req notify
        GAP_SetParamValue(TGAP_ADV_SCAN_REQ_NOTIFY, ENABLE);
    }

    // Setup the GAP Bond Manager
    {
        uint32_t passkey = ble_passkey; // passkey "000000"
        uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
        uint8_t mitm = TRUE;
        uint8_t bonding = TRUE;
        uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
        GAPBondMgr_SetParameter(GAPBOND_PERI_DEFAULT_PASSCODE, sizeof(uint32_t),
                &passkey);
        GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8_t),
                &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8_t),
                &mitm);
        GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8_t),
                &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8_t),
                &bonding);
    }

    // Initialize GATT attributes
    GGS_AddService(GATT_ALL_SERVICES); // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES); // GATT attributes
    DevInfo_AddService(); // Device Information Service
    SimpleProfile_AddService(GATT_ALL_SERVICES); // Simple GATT Profile

    // Setup the SimpleProfile Characteristic Values
    {
        uint8_t charValue1[SIMPLEPROFILE_CHAR1_LEN] = { 1 };
        uint8_t charValue2[SIMPLEPROFILE_CHAR2_LEN] = { 2 };
        uint8_t charValue3[SIMPLEPROFILE_CHAR3_LEN] = { 3 };
        uint8_t charValue4[SIMPLEPROFILE_CHAR4_LEN] = { 4 };
        uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };

        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, SIMPLEPROFILE_CHAR1_LEN,
                charValue1);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR2, SIMPLEPROFILE_CHAR2_LEN,
                charValue2);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, SIMPLEPROFILE_CHAR3_LEN,
                charValue3);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, SIMPLEPROFILE_CHAR4_LEN,
                charValue4);
        SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN,
                charValue5);
    }

    // Init Connection Item
    peripheralInitConnItem(&peripheralConnList);

    // Register callback with SimpleGATTprofile
    SimpleProfile_RegisterAppCBs(&Peripheral_SimpleProfileCBs);

    // Register receive scan request callback
    GAPRole_BroadcasterSetCB(&Broadcaster_BroadcasterCBs);

    // Setup a delayed profile startup
    tmos_set_event(Peripheral_TaskID, SBP_START_DEVICE_EVT);
}

/*********************************************************************
 * @fn      peripheralInitConnItem
 *
 * @brief   Init Connection Item
 *
 * @param   peripheralConnList -
 *
 * @return  NULL
 */
static void peripheralInitConnItem(peripheralConnItem_t* peripheralConnList)
{
    peripheralConnList->connHandle = GAP_CONNHANDLE_INIT;
    peripheralConnList->connInterval = 0;
    peripheralConnList->connSlaveLatency = 0;
    peripheralConnList->connTimeout = 0;
}

/*********************************************************************
 * @fn      Peripheral_ProcessEvent
 *
 * @brief   Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16_t Peripheral_ProcessEvent(uint8_t task_id, uint16_t events)
{
    //  VOID task_id; // TMOS required parameter that isn't used in this function

    if (events & SYS_EVENT_MSG)
    {
        uint8_t* pMsg;

        if ((pMsg = tmos_msg_receive(Peripheral_TaskID)) != NULL)
        {
            Peripheral_ProcessTMOSMsg((tmos_event_hdr_t*) pMsg);
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & SBP_START_DEVICE_EVT)
    {
        // Start the Device
        GAPRole_PeripheralStartDevice(Peripheral_TaskID, &Peripheral_BondMgrCBs,
                &Peripheral_PeripheralCBs);
        return (events ^ SBP_START_DEVICE_EVT);
    }

    if (events & SBP_PERIODIC_EVT)
    {
        // Restart timer
        if (SBP_PERIODIC_EVT_PERIOD)
        {
            tmos_start_task(Peripheral_TaskID, SBP_PERIODIC_EVT,
            SBP_PERIODIC_EVT_PERIOD);
        }
        // Perform periodic application task
        performPeriodicTask();
        return (events ^ SBP_PERIODIC_EVT);
    }

    if (events & SBP_PARAM_UPDATE_EVT)
    {
        // Send connect param update request
        GAPRole_PeripheralConnParamUpdateReq(peripheralConnList.connHandle,
        DEFAULT_DESIRED_MIN_CONN_INTERVAL,
        DEFAULT_DESIRED_MAX_CONN_INTERVAL,
        DEFAULT_DESIRED_SLAVE_LATENCY,
        DEFAULT_DESIRED_CONN_TIMEOUT, Peripheral_TaskID);

        return (events ^ SBP_PARAM_UPDATE_EVT);
    }

    if (events & SBP_PHY_UPDATE_EVT)
    {
        // start phy update
        PRINT("PHY Update %x...\n",
                GAPRole_UpdatePHY(peripheralConnList.connHandle, 0, GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, GAP_PHY_OPTIONS_NOPRE));

        return (events ^ SBP_PHY_UPDATE_EVT);
    }

    if (events & SBP_READ_RSSI_EVT)
    {
        GAPRole_ReadRssiCmd(peripheralConnList.connHandle);
        tmos_start_task(Peripheral_TaskID, SBP_READ_RSSI_EVT,
        SBP_READ_RSSI_EVT_PERIOD);
        return (events ^ SBP_READ_RSSI_EVT);
    }

    // Discard unknown events
    return 0;
}

/*********************************************************************
 * @fn      Peripheral_ProcessGAPMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void Peripheral_ProcessGAPMsg(gapRoleEvent_t* pEvent)
{
    switch (pEvent->gap.opcode)
    {
    case GAP_SCAN_REQUEST_EVENT:
    {
//        PRINT("Receive scan req from %x %x %x %x %x %x  ..\n",
//                pEvent->scanReqEvt.scannerAddr[0],
//                pEvent->scanReqEvt.scannerAddr[1],
//                pEvent->scanReqEvt.scannerAddr[2],
//                pEvent->scanReqEvt.scannerAddr[3],
//                pEvent->scanReqEvt.scannerAddr[4],
//                pEvent->scanReqEvt.scannerAddr[5]);
        break;
    }

    case GAP_PHY_UPDATE_EVENT:
    {
        PRINT("Phy update Rx:%x Tx:%x ..\n", pEvent->linkPhyUpdate.connRxPHYS,
                pEvent->linkPhyUpdate.connTxPHYS);
        break;
    }

    default:
        break;
    }
}

/*********************************************************************
 * @fn      Peripheral_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void Peripheral_ProcessTMOSMsg(tmos_event_hdr_t* pMsg)
{
    switch (pMsg->event)
    {
    case GAP_MSG_EVENT:
    {
        Peripheral_ProcessGAPMsg((gapRoleEvent_t*) pMsg);
        break;
    }

    case GATT_MSG_EVENT:
    {
        gattMsgEvent_t* pMsgEvent;

        pMsgEvent = (gattMsgEvent_t*) pMsg;
        if (pMsgEvent->method == ATT_MTU_UPDATED_EVENT)
        {
            peripheralMTU = pMsgEvent->msg.exchangeMTUReq.clientRxMTU;
            PRINT("mtu exchange: %d\n",
                    pMsgEvent->msg.exchangeMTUReq.clientRxMTU);
        }
        break;
    }

    default:
        break;
    }
}

/*********************************************************************
 * @fn      Peripheral_LinkEstablished
 *
 * @brief   Process link established.
 *
 * @param   pEvent - event to process
 *
 * @return  none
 */
static void Peripheral_LinkEstablished(gapRoleEvent_t* pEvent)
{
    gapEstLinkReqEvent_t* event = (gapEstLinkReqEvent_t*) pEvent;

    // See if already connected
    if (peripheralConnList.connHandle != GAP_CONNHANDLE_INIT)
    {
        GAPRole_TerminateLink(pEvent->linkCmpl.connectionHandle);
        PRINT("Connection max...\n");
    }
    else
    {
        peripheralConnList.connHandle = event->connectionHandle;
        peripheralConnList.connInterval = event->connInterval;
        peripheralConnList.connSlaveLatency = event->connLatency;
        peripheralConnList.connTimeout = event->connTimeout;
        peripheralMTU = ATT_MTU_SIZE;
        // Set timer for periodic event
        tmos_start_task(Peripheral_TaskID, SBP_PERIODIC_EVT,
        SBP_PERIODIC_EVT_PERIOD);

        // Set timer for param update event
        tmos_start_task(Peripheral_TaskID, SBP_PARAM_UPDATE_EVT,
        SBP_PARAM_UPDATE_DELAY);

        // Start read rssi
        tmos_start_task(Peripheral_TaskID, SBP_READ_RSSI_EVT,
        SBP_READ_RSSI_EVT_PERIOD);

        PRINT("Conn %x - Int %x \n", event->connectionHandle,
                event->connInterval);
    }
}

/*********************************************************************
 * @fn      Peripheral_LinkTerminated
 *
 * @brief   Process link terminated.
 *
 * @param   pEvent - event to process
 *
 * @return  none
 */
static void Peripheral_LinkTerminated(gapRoleEvent_t* pEvent)
{
    gapTerminateLinkEvent_t* event = (gapTerminateLinkEvent_t*) pEvent;

    if (event->connectionHandle == peripheralConnList.connHandle)
    {
        peripheralConnList.connHandle = GAP_CONNHANDLE_INIT;
        peripheralConnList.connInterval = 0;
        peripheralConnList.connSlaveLatency = 0;
        peripheralConnList.connTimeout = 0;
        tmos_stop_task(Peripheral_TaskID, SBP_PERIODIC_EVT);
        tmos_stop_task(Peripheral_TaskID, SBP_READ_RSSI_EVT);

        // Restart advertising
        {
            uint8_t advertising_enable = TRUE;
            GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                    &advertising_enable);
        }
    }
    else
    {
        PRINT("ERR..\n");
    }
}

/*********************************************************************
 * @fn      peripheralRssiCB
 *
 * @brief   RSSI callback.
 *
 * @param   connHandle - connection handle
 * @param   rssi - RSSI
 *
 * @return  none
 */
static void peripheralRssiCB(uint16_t connHandle, int8_t rssi)
{
    PRINT("RSSI -%d dB Conn  %x \n", -rssi, connHandle);
}

/*********************************************************************
 * @fn      peripheralParamUpdateCB
 *
 * @brief   Parameter update complete callback
 *
 * @param   connHandle - connect handle
 *          connInterval - connect interval
 *          connSlaveLatency - connect slave latency
 *          connTimeout - connect timeout
 *
 * @return  none
 */
static void peripheralParamUpdateCB(uint16_t connHandle, uint16_t connInterval,
        uint16_t connSlaveLatency, uint16_t connTimeout)
{
    if (connHandle == peripheralConnList.connHandle)
    {
        peripheralConnList.connInterval = connInterval;
        peripheralConnList.connSlaveLatency = connSlaveLatency;
        peripheralConnList.connTimeout = connTimeout;

        PRINT("Update %x - Int %x \n", connHandle, connInterval);
    }
    else
    {
        PRINT("ERR..\n");
    }
}

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB(gapRole_States_t newState,
        gapRoleEvent_t* pEvent)
{
    switch (newState & GAPROLE_STATE_ADV_MASK)
    {
    case GAPROLE_STARTED:
        PRINT("Initialized..\n");
        break;

    case GAPROLE_ADVERTISING:
        if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT)
        {
            Peripheral_LinkTerminated(pEvent);
            PRINT("Disconnected.. Reason:%x\n", pEvent->linkTerminate.reason);
            PRINT("Advertising..\n");
        }
        else if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
        {
            PRINT("Advertising..\n");
        }
        break;

    case GAPROLE_CONNECTED:
        if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
        {
            Peripheral_LinkEstablished(pEvent);
            PRINT("Connected..\n");
        }
        break;

    case GAPROLE_CONNECTED_ADV:
        if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
        {
            PRINT("Connected Advertising..\n");
        }
        break;

    case GAPROLE_WAITING:
        if (pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT)
        {
            PRINT("Waiting for advertising..\n");
        }
        else if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT)
        {
            Peripheral_LinkTerminated(pEvent);
            PRINT("Disconnected.. Reason:%x\n", pEvent->linkTerminate.reason);
        }
        else if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
        {
            if (pEvent->gap.hdr.status != SUCCESS)
            {
                PRINT("Waiting for advertising..\n");
            }
            else
            {
                PRINT("Error..\n");
            }
        }
        else
        {
            PRINT("Error..%x\n", pEvent->gap.opcode);
        }
        break;

    case GAPROLE_ERROR:
        PRINT("Error..\n");
        break;

    default:
        break;
    }
}

/*********************************************************************
 * @fn      performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets
 *          called every five seconds as a result of the SBP_PERIODIC_EVT
 *          TMOS event. In this example, the value of the third
 *          characteristic in the SimpleGATTProfile service is retrieved
 *          from the profile, and then copied into the value of the
 *          the fourth characteristic.
 *
 * @param   none
 *
 * @return  none
 */
static void performPeriodicTask(void)
{
    // uint8_t buff[30];
//  uint8_t notiData[SIMPLEPROFILE_CHAR4_LEN] = { 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88 };

    // if (cmd != 0) {
    //     buff[0] = 0xAA;
    //     buff[1] = 0x55;
    //     buff[2] = cmd;
    //     buff[3] = para2.runningstate;
    //     buff[4] = para2.errcode;
    //     buff[5] = para2.runningstep;
    //     buff[6] = para2.altitude % 256;
    //     buff[7] = para2.altitude / 256;
    //     buff[8] = para2.runningmode;
    //     if (para2.runningmode == 1) {
    //         buff[9] = para1.setlevel;
    //     } else {
    //         buff[9] = para1.settemp / 10;
    //         buff[10] = para1.setlevel - 1;
    //     }

    //     buff[11] = para2.supplyvoltage % 256;
    //     buff[12] = para2.supplyvoltage / 256;
    //     buff[13] = para2.casetemp % 256;
    //     buff[14] = para2.casetemp / 256;
    //     buff[15] = (para2.cabtemp / 10) % 256;
    //     buff[16] = (para2.cabtemp / 10) / 256;
    //     buff[17] = para1.autotime % 256;
    //     buff[18] = para1.autotime / 256;
    //     buff[19] = para1.runtime % 256;
    //     buff[20] = para1.runtime / 256;
    //     buff[21] = para1.isauto;
    //     buff[22] = para1.language;
    //     buff[23] = para1.tempoffset;
    //     buff[24] = para1.tankvolume;
    //     buff[25] = para1.oilpumptype;
    //     buff[26] = para1.bluetoothswitch;
    //     buff[27] = para1.remotecontrolmatching;
    //     buff[28] = para1.automaticheating;

    //     buff[29] = checksum(buff + 2, 27);
    //     peripheralChar1Notify(buff, 17);
    //     peripheralChar1Notify(buff+17, 13);
    //     for (int i = 0; i < 30; i++) {
    //         PRINT("%X ", buff[i]);
    //     }
    //     PRINT("notify.. \n");
    //     cmd = 0;
    // }
//  peripheralChar4Notify(notiData, SIMPLEPROFILE_CHAR4_LEN);
}

/*********************************************************************
 * @fn      peripheralChar4Notify
 *
 * @brief   Prepare and send simpleProfileChar4 notification
 *
 * @param   pValue - data to notify
 *          len - length of data
 *
 * @return  none
 */
static void peripheralChar4Notify(uint8_t* pValue, uint16_t len)
{
    attHandleValueNoti_t noti;
    if (len > (peripheralMTU - 3))
    {
        PRINT("Too large noti\n");
        return;
    }
    noti.len = len;
    noti.pValue = GATT_bm_alloc(peripheralConnList.connHandle,
    ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
    if (noti.pValue)
    {
        tmos_memcpy(noti.pValue, pValue, noti.len);
        if (simpleProfile_Notify(peripheralConnList.connHandle,
                &noti) != SUCCESS)
        {
            GATT_bm_free((gattMsg_t*) &noti, ATT_HANDLE_VALUE_NOTI);
        }
    }
}
/*********************************************************************
 * @fn      peripheralChar4Notify
 *
 * @brief   Prepare and send simpleProfileChar4 notification
 *
 * @param   pValue - data to notify
 *          len - length of data
 *
 * @return  none
 */
static void peripheralChar1Notify(uint8_t* pValue, uint16_t len)
{
    attHandleValueNoti_t noti;
    if (len > (peripheralMTU - 3))
    {
        PRINT("Too large noti\n");
        return;
    }
    noti.len = len;
    noti.pValue = GATT_bm_alloc(peripheralConnList.connHandle,
    ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);
    if (noti.pValue)
    {
        tmos_memcpy(noti.pValue, pValue, noti.len);
        if (simpleProfile_Notify_char1(peripheralConnList.connHandle,
                &noti) != SUCCESS)
        {
            GATT_bm_free((gattMsg_t*) &noti, ATT_HANDLE_VALUE_NOTI);
        }
    }
}
/*********************************************************************
 * @fn      simpleProfileChangeCB
 *
 * @brief   Callback from SimpleBLEProfile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *          pValue - pointer to data that was changed
 *          len - length of data
 *
 * @return  none
 */
static uint16_t GetModbusCRC16_Cal(uint8_t* data, uint32_t len) // Modbus-CRC校验-----计算法
{
    uint8_t temp;
    uint16_t wcrc = 0XFFFF; // 16位crc寄存器预置
    uint32_t i = 0, j = 0; // 计数
    for (i = 0; i < len; i++) // 循环计算每个数据
    {
        temp = data[i] & 0X00FF; // 将八位数据与crc寄存器亦或
        wcrc ^= temp; // 将数据存入crc寄存器
        for (j = 0; j < 8; j++) // 循环计算数据的
        {
            if (wcrc & 0X0001) // 判断右移出的是不是1，如果是1则与多项式进行异或。
            {
                wcrc >>= 1; // 先将数据右移一位
                wcrc ^= 0XA001; // 与上面的多项式进行异或
            }
            else // 如果不是1，则直接移出
            {
                wcrc >>= 1; // 直接移出
            }
        }
    }
    return wcrc;
//  return ((wcrc << 8) | (wcrc >> 8)); // 高低位置换
}
extern uint8_t ec11_flage;
uint8_t passkey_checked = 0;


const uint8_t key[] = "password";
void EncryptBuffer(uint8_t* buffer, uint8_t size,const  uint8_t* key)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 8; j++) {
            buffer[8 * i + j] = buffer[8 * i + j] ^ key[j];
        }
    }
}

uint8_t bleDataChick(uint8_t len,uint8_t *buffer)
{
//    uint8_t buf[16];
    uint8_t i=0;
    uint8_t sum_get=0,sum_chick=0;
//    EncryptBuffer(buffer,len/8,key);
    if(*buffer==0xAA && *(buffer+1)==0x55)
    {
        sum_get = *(buffer+len-1);
        for(i=2;i<len-1;i++)
            sum_chick += *(buffer+i);

//        buf[0] = *(buffer+len-1);
//        buf[1] = sum_chick;

//        crc16_get = *(buffer+len-2)+(*(buffer+len-1))*256;
        //crc校验
//        crc16 = GetModbusCRC16_Cal(buffer,len-2);
//        memcpy(buf,buffer,8);
//        buf[8] = crc16_get>>8;
//        buf[9] = crc16_get;
//        buf[10] = crc16>>8;
//        buf[11] = crc16;
//        buf[12] = len;
//        GPIOB_SetBits(GPIO_Pin_9);
//        UART1_SendString(buf,14);
        if(sum_get == sum_chick)  return TRUE;
        else                    return FALSE;
    }
    else return FALSE;
}

void Ble_Send_Packet(void)
{
    uint8_t buff[50];
//    uint16_t crc;
    uint16_t time_minute=0;

    memset(buff,0,sizeof(buff));
    buff[0] = 0xAA;
    buff[1] = 0x66;
    buff[2] = cmd;

    if(heaterDatas.faultType == 0)
    {
        if(runState.disIndex==DISINDEX_PWROFF_ANIMATION || runState.disIndex==DISINDEX_OFFING || runState.disIndex==DISINDEX_PWROFFED)
        {
            buff[3] = 0;//para2.runningstate;//运行状态 0停机   1开机    2故障
        }
        else{
            buff[3] = 1;
        }
    }else{
        buff[3] = 2;
    }

    buff[4] = heaterDatas.faultType;        //故障代码  00~10
    buff[5] = heaterDatas.runningStep;      //运行步骤  00关机  01自检  02点火  03燃烧  04停机
    if(heaterDatas.altitude==0)
    {
        if(heaterDatas.cabAltitude<0)
        {
            buff[6] = 0;
            buff[7] = 0;
        }else
        {
            buff[6] = (heaterDatas.cabAltitude*10)/256;     //海拔高度H
            buff[7] = (heaterDatas.cabAltitude*10)%256;     //海拔高度L
        }
    }else
    {
        buff[6] = heaterDatas.altitude/256;     //海拔高度H
        buff[7] = heaterDatas.altitude%256;     //海拔高度L
    }

    if(baseParameters.runMode == CTRL_RUNMODE_TEMP)
    {
      buff[8] = 2;                          //运行模式  1手动  2恒温
      buff[10] = GetRunningAutoSetLevel();
    }else if(baseParameters.runMode == CTRL_RUNMODE_LEVEL)
    {
      buff[8] = 1;
      buff[10] = GetRunningSetLevel();           //档位
    }else if(baseParameters.runMode == CTRL_RUNMODE_VENTILATE)
    {
      buff[8] = 3;
      buff[10] = GetRunningVentilateLevel();           //档位
    }else if(baseParameters.runMode == CTRL_RUNMODE_FASTHEAT)
    {
      buff[8] = 4;
      buff[10] = 11;
    }
    if(GetConfigData_tempUnit()==TEMPER_UINT_C)
    {
        buff[9] = runState.setTemper;           //温度摄氏度
    }else if(GetConfigData_tempUnit()==TEMPER_UINT_F)
    {
        buff[9] = runState.setHTemper;          //温度华氏度
    }
//    if(baseParameters.runMode == CTRL_RUNMODE_TEMP)
//    {
//        buff[10] = GetRunningAutoSetLevel();
//    }else if (baseParameters.runMode == CTRL_RUNMODE_TEMP && GetDisSetTemperModeStandby())
//    {
//        buff[10] = GetRunningAutoSetLevel();
//    }else if(baseParameters.runMode == CTRL_RUNMODE_LEVEL)
//    {
//        buff[10] = GetRunningSetLevel();           //档位
//    }else if(baseParameters.runMode == CTRL_RUNMODE_VENTILATE)
//    {
//        buff[10] = GetRunningVentilateLevel();           //档位
//    }
    buff[11] = heaterDatas.supplyVoltage/256;//供电电压H x10
    buff[12] = heaterDatas.supplyVoltage%256;//供电电压 L
    buff[13] = ((heaterDatas.shellTemp/10)>>8)&0x00FF;//((uint16_t)((heaterDatas.shellTemp)/10))/256;//壳体温度H
    buff[14] = (heaterDatas.shellTemp/10)&0x00FF;//((uint16_t)((heaterDatas.shellTemp)/10))%256;//壳体温度L
    buff[15] = (heaterDatas.version/10)/256;//主板版本H
    buff[16] = (heaterDatas.version/10)%256;//主板版本L
    buff[17] = (heaterDatas.oilPumpMass/10)/256;//泵油频率H
    buff[18] = (heaterDatas.oilPumpMass/10)%256;//泵油频率L

    time_minute = (baseParameters.sttime.tm_hour*60) + baseParameters.sttime.tm_min;
    buff[19] = time_minute/256;             //当前时间H
    buff[20] = time_minute%256;             //当前时间L
    time_minute = baseParameters.timerClockHour*60 + baseParameters.timerClockMinute;
    buff[21] = time_minute/256;             //定时时间H
    buff[22] = time_minute%256;             //定时时间L
    buff[23] = baseParameters.totalHeatTime/256;  //定时时长H
    buff[24] = baseParameters.totalHeatTime%256;  //定时时L
    buff[25] = baseParameters.timerClockEn;//自动启停开关
#if GERMAN==2
    if(baseParameters.languageType==SPEECHLANGUARE_ENGLISH)
        buff[26] = 5;
    else buff[26] = 3;
#else
    if(baseParameters.languageType==SPEECHLANGUARE_ENGLISH)
        buff[26] = 0;
    else if(baseParameters.languageType==SPEECHLANGUARE_CHINESE)
        buff[26] = 4;//德语
    else if(baseParameters.languageType==SPEECHLANGUARE_RUSSIAN)
        buff[26] = 2;
    else buff[26] = 3;
#endif
    buff[27] = GetConfigData_tempUnit();    //温度单位 0摄氏度  1华氏度
    if(baseParameters.bleOnOff==0)
        buff[28] = 20;
    else if(baseParameters.bleOnOff==1)
        buff[28] = 21;
    if(baseParameters.rf433OnOff==0)
        buff[29] = 20;
    else if(baseParameters.bleOnOff==1)
        buff[29] = 21;
    buff[30] = 0xFF;
    buff[31] = baseParameters.temperModeStandby;
    buff[32] = (heaterDatas.cabTemp>>8)&0x00FF;//((uint16_t)heaterDatas.cabTemp)/256;
    buff[33] = heaterDatas.cabTemp&0x00FF;//((uint16_t)heaterDatas.cabTemp)%256;
    buff[34] = baseParameters.temperSensorOffset;//((uint16_t)baseParameters.temperSensorOffset)/256;
    switch(heaterDatas.faultType)
    {
    case Fault_Power:
        buff[35] = 1;   //显示故障码
        break;
#if EN02==1
    case Fault_LOWBATTERY://电池供电电池电量低
        buff[35] = 2;   //显示故障码
        break;
#endif
    case Fault_Ignition:
        buff[35] = 3;   //显示故障码
        break;
    case Fault_OilPump:
        buff[35] = 4;   //显示故障码
        break;
    case Fault_ShellTemper_Protect:
        buff[35] = 5;   //显示故障码
        break;
    case Fault_WindMotor:
        buff[35] = 6;   //显示故障码
        break;
    case Fault_IntAirTemper_Senser:
        buff[35] = 7;
        break;
    case Fault_Flameout:
        buff[35] = 8;   //显示故障码
        break;
    case Fault_ShellTemper_Protect_Senser:
        buff[35] = 9;   //显示故障码
        break;
    case Fault_Start:
        buff[35] = 10;   //显示故障码
        break;
    default:
//        buff[35] = heaterDatas.faultType;
        break;
    }
    if(runState.disIndex == DISINDEX_CO_OVERWARNING)
    {
        buff[35] = 11;   //显示故障码
    }
    buff[36] = GetConfigData_BledDuty()+200;
    if(heaterDatas.sensorType==0x01)
    {
        buff[37] = 0x01;//CO数值类型
        buff[38] = (GetRunningData_CONumber()>>8)&0x00FF;//传感器数值
        buff[39] = GetRunningData_CONumber()&0x00FF;//传感器数值
    }else
    {
        buff[37] = 0;
        buff[38] = 0;
        buff[39] = 0;
    }
    buff[40] = DEVICE_TYPE&0xFF;//设备零件号
    buff[41] = (DEVICE_TYPE>>8)&0xFF;//设备零件号
    buff[42] = (DEVICE_TYPE>>16)&0xFF;//设备零件号
    buff[43] = (DEVICE_TYPE>>24)&0xFF;//设备零件号
    if(runState.mainBoardType == MAIN_BOARD_TYPE_VENTILATE_FASTHART)
        buff[44] = MAIN_BOARD_TYPE_VENTILATE_FASTHART;//主板类型
    else if(runState.mainBoardType == MAIN_BOARD_TYPE_NONE_VENTILATE)
        buff[44] = MAIN_BOARD_TYPE_NONE_VENTILATE;//主板类型
    buff[45] = VERSION;//开关软件版本
    buff[47] = 0;

    EncryptBuffer(buff,6,key);
    peripheralChar1Notify(buff,48);
}

uint8_t newValue[SIMPLEPROFILE_CHAR1_LEN];
static void simpleProfileChangeCB(uint8_t paramID, uint8_t* pValue,uint16_t len)
{
    uint16_t cmd_value = 0;
    int16_t value=0;
    uint16_t get_password=0,password=0;
//  uint8_t buf[50];
//  uint16_t crc;
//  uint16_t time_minute=0;
//  unsigned char temp[40] = { 0 };
//  unsigned char ret;
    switch (paramID)
    {
    case SIMPLEPROFILE_CHAR1:
    {
        if(GetDisSetBleOnOff()==0)  return;

        tmos_memcpy(newValue, pValue, len);
        if(bleDataChick(len,newValue))
        {
            password = newValue[2]*256 + newValue[3];
            cmd = newValue[4];
            cmd_value = newValue[5] + newValue[6]*256;
            get_password = baseParameters.blePassword;
            if(runState.bleSetPassword==1)
            {
                baseParameters.blePassword = password;
                //显示suc,延时后跳转运行页面
                vPARA_SAVE();
//              runState.disIndex = DISINDEX_RUNNING;
                runState.bleSetPassword = 0;
                runState.disSetTimeState = DISSETTIME_STATE_HOUR;//显示SUC后直接退出
            }
            else
            {
                if(get_password == password)
                {
                    switch (cmd)
                    {
                    case BLE_CMD_GETDATA://获取数据
        //              GPIOB_SetBits(GPIO_Pin_9);
        //              UART1_SendString(newValue,len+1);
                        break;
                    case BLE_CMD_SET_MODEL://档位、恒温切换
                        runState.bklight_time = GetConfigData_BledTime()*10;
                        if(cmd_value==1)//档位
                        {
                            baseParameters.runMode = CTRL_RUNMODE_LEVEL;
                        }else if(cmd_value==2)//温控
                        {
                            baseParameters.runMode = CTRL_RUNMODE_TEMP;
                        }else if(cmd_value==3)//通风
                        {
                            baseParameters.runMode = CTRL_RUNMODE_VENTILATE;
                        }else if(cmd_value==4)//强热
                        {
                            baseParameters.runMode = CTRL_RUNMODE_FASTHEAT;
                        }
                        break;
                    case BLE_CMD_TURNONOFF://开、关机
                        if(cmd_value==0)
                            TurnOff_Device();
                        else if(cmd_value==1)
                        {
                            if(heaterDatas.runningStep == HEATERRUNSTEP_NULL)
                                TurnOn_Device();
                        }
                        break;
                    case BLE_CMD_CHANGEVALUE:
                        runState.bklight_time = GetConfigData_BledTime()*10;
                        if(baseParameters.runMode==CTRL_RUNMODE_LEVEL)
                        {
                            if(cmd_value>10)        cmd_value = 10;
                            else if(cmd_value==0)   cmd_value = 1;
                            runState.setLevel = cmd_value;
                        }else if(baseParameters.runMode==CTRL_RUNMODE_TEMP)
                        {
                            if(GetConfigData_tempUnit()==TEMPER_UINT_C)
                            {
                                if(cmd_value>CTRL_RUNMODE_TEMP_MAX)         cmd_value = CTRL_RUNMODE_TEMP_MAX;
                                else if(cmd_value<CTRL_RUNMODE_TEMP_MIN)    cmd_value = CTRL_RUNMODE_TEMP_MIN;
                                runState.setTemper = cmd_value;
                            }else if(GetConfigData_tempUnit()==TEMPER_UINT_F)
                            {
                                if(cmd_value>CTRL_RUNMODE_HTEMP_MAX)         cmd_value = CTRL_RUNMODE_HTEMP_MAX;
                                else if(cmd_value<CTRL_RUNMODE_HTEMP_MIN)    cmd_value = CTRL_RUNMODE_HTEMP_MIN;
                                runState.setHTemper = cmd_value;
                                //!!!!!!!!!!!!!!!!!!!!  App设置华氏度开关档位不变
                                runState.setTemper = (int16_t)(((float)(runState.setHTemper-32))/1.8);
                            }
                        }else if(baseParameters.runMode==CTRL_RUNMODE_VENTILATE)
                        {
                            if(cmd_value>10)        cmd_value = 10;
                            else if(cmd_value==0)   cmd_value = 1;
                            runState.ventilateLevel = cmd_value;
                        }
                        CleardisSetTempBlinkCnt();
                        SetDisSetTemperBlink(1);
                        runState.bklight_time = GetConfigData_BledTime()*10;
                        break;
                    case BLE_CMD_SET_TIME://设置时间
                        if (cmd_value <= 1440)
                        {
                            SetDisSetTime(cmd_value/60,cmd_value%60);
                        }
                        break;
                    case BLE_CMD_SET_TIMER://定时开机时间
                        if (cmd_value <= 24 * 60)
                        {
                            SetDisSetTimer(cmd_value/60,cmd_value%60);
                            vPARA_SAVE();
                        }
                        break;
                    case BLE_CMD_SET_TIMER_RUNTIME://定时开机运行时间
                        if (cmd_value <= 600)
                        {
                            SetDisSetTimerRunTime(cmd_value);
                            vPARA_SAVE();
                        }
                        break;
                    case BLE_CMD_SET_TIMER_EN://定时开机使能
                        if (cmd_value >= 0 && cmd_value <= 1)
                        {
                            SetDisSetTimerClockEn(cmd_value);
                            vPARA_SAVE();
                        }
                        break;
                    case BLE_CMD_SET_LANGUAGE://设置语音
                        if (cmd_value < 5)
                        {
#if GERMAN==0
                            if(cmd_value==0)        SetDisSetLanguageType(SPEECHLANGUARE_ENGLISH);
                            else if(cmd_value==2)   SetDisSetLanguageType(SPEECHLANGUARE_RUSSIAN);
                            else if(cmd_value==3)   SetDisSetLanguageType(SPEECHLANGUARE_OFF);
                            else if(cmd_value==1)   SetDisSetLanguageType(SPEECHLANGUARE_CHINESE);
#elif GERMAN==1
                            if(cmd_value==0)        SetDisSetLanguageType(SPEECHLANGUARE_ENGLISH);
                            else if(cmd_value==2)   SetDisSetLanguageType(SPEECHLANGUARE_RUSSIAN);
                            else if(cmd_value==3)   SetDisSetLanguageType(SPEECHLANGUARE_OFF);
                            else if(cmd_value==4)   SetDisSetLanguageType(SPEECHLANGUARE_CHINESE);//德语
#elif GERMAN==2
                            if(cmd_value==3)    SetDisSetLanguageType(SPEECHLANGUARE_OFF);
                            else                SetDisSetLanguageType(SPEECHLANGUARE_ENGLISH);
#endif
                            vPARA_SAVE();
                        }
                        break;
                    case BLE_CMD_SET_TEMPER_UINT:
                        if(cmd_value==TEMPER_UINT_C)
                        {
                            SetConfigData_tempUnit(TEMPER_UINT_C);
                            vPARA_SAVE();
                        }else if(cmd_value==TEMPER_UINT_F)
                        {
                            SetConfigData_tempUnit(TEMPER_UINT_F);
                            vPARA_SAVE();
                        }
                        break;
                    case BLE_CMD_SET_OILCASL_VOLUME:
//                        if (cmd_value <= 10)
//                        {
//                            SetDisSetOilCaseVolume(cmd_value*5);
//                            vPARA_SAVE();
//                        }
                        if(cmd_value == 20)
                        {
                            SetDisSetBleOnOff(0);
                            vPARA_SAVE();
                        }
                        else if(cmd_value == 21)
                        {
                            SetDisSetBleOnOff(1);
                            vPARA_SAVE();
                        }
                        break;
                    case BLE_CMD_SET_OILPUMP_TYPE:
//                        printf("17:%d",cmd_value);
                        if(cmd_value == 20)
                        {
                            SetDisSet433OnOff(0);
                            vPARA_SAVE();
                        }
                        else if(cmd_value == 21)
                        {
                            SetDisSet433OnOff(1);
                            vPARA_SAVE();
                        }
//                        if (cmd_value <= 3)
//                        {
//                            SetDisSetOilPumpType(cmd_value);
//                            switch(cmd_value)
//                            {
//                            case 0:
//                                SetDisSetOilPumpType(OILPUMP_TYPE_16UL);
//                                break;
//                            case 1:
//                                SetDisSetOilPumpType(OILPUMP_TYPE_22UL);
//                                break;
//                            case 2:
//                                SetDisSetOilPumpType(OILPUMP_TYPE_28UL);
//                                break;
//                            case 3:
//                                SetDisSetOilPumpType(OILPUMP_TYPE_32UL);
//                                break;
//                            }
//                            vPARA_SAVE();
//                        }
                        break;
                    case BLE_CMD_SET_AUTO_ONOFF_SW:
                        if (cmd_value >= 0 && cmd_value <= 1)
                        {
                            SetDisSetTemperModeStandby(cmd_value);
                            vPARA_SAVE();
                        }
                        break;
        //          case BLE_CMD_SET_SET_ALTITUDE_UINT:
        //              break;
                    case BLE_CMD_SET_TEMPER_OFFSET:
                        value = (int16_t)cmd_value;
                        if (value >= -9 && value <= 9)
                        {
                            SetDisSetTemperOffSet(value);
                            vPARA_SAVE();
                        }
                        break;
                    case BLE_CMD_SET_BLED_TIME:
                        if(cmd_value>=200)
                        {
                            cmd_value = cmd_value-200;
                            SetConfigData_BledDuty(cmd_value);
                            vPARA_SAVE();
                        }
                        break;
                    }
                    Ble_Send_Packet();
                }
            }
        }
        break;
    }

    case SIMPLEPROFILE_CHAR3:
    {
        uint8_t newValue[SIMPLEPROFILE_CHAR3_LEN];
        tmos_memcpy(newValue, pValue, len);
        PRINT("profile ChangeCB CHAR3..\n");
        PRINT("jump OTA \n");
        mDelaymS(5);
        Jump_OTA();
        break;
    }

    default:
        // should not reach here!
        break;
    }
}
/* OTA 升级标志 */
#define IMAGE_OTA_FLAG       0x03

/* 存放在DataFlash地址，不能占用蓝牙的位置 */
#define OTA_DATAFLASH_ADD    0x00077000 - FLASH_ROM_MAX_SIZE

/* flash的数据临时存储 */
__attribute__((aligned(8)))     uint8_t block_buf[16];

/*********************************************************************
 * @fn      Jump_OTA
 *
 * @brief   跳转OTA升级
 *
 * @return  none
 */
void Jump_OTA(void)
{
    uint16_t i;
    uint32_t ver_flag;

    /* 读取第一块 */
    EEPROM_READ(OTA_DATAFLASH_ADD, (uint32_t * )&block_buf[0], 4);

    /* 擦除第一块 */
    EEPROM_ERASE(OTA_DATAFLASH_ADD, EEPROM_PAGE_SIZE);

    /* 更新Image信息 */
    block_buf[0] = IMAGE_OTA_FLAG;

    /* 编程DataFlash */
    EEPROM_WRITE(OTA_DATAFLASH_ADD, (uint32_t * )&block_buf[0], 4);

    /* 软复位 */
    SYS_ResetExecute();
}

/*********************************************************************
 *********************************************************************/
