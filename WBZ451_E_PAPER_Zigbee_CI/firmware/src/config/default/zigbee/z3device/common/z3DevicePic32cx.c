/*******************************************************************************
  Z3 Device Pic32cx Source File

  Company:
    Microchip Technology Inc.

  File Name:
    z3DevicePic32cx.c

  Summary:
    This file contains the Z3 Device Pic32cx Implementation.

  Description:
     This file contains the Z3 Device Pic32cx Implementation.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

#include <z3device/stack_interface/zgb_api.h>
#include <z3device/stack_interface/bdb/include/bdb_api.h>
#include <z3device/stack_interface/zcl/include/zcl_api.h>
#include <z3device/stack_interface/nwk/include/nwk_api.h>
#include <z3device/stack_interface/bdb/include/BDB_Unpack.h>
#include <configserver/include/configserver.h>
#include <aps/include/apsCommon.h>
#include <zcl/include/zclAttributes.h>
#include <z3device/common/include/z3Device.h>
#include <zcl/include/clusters.h>
#include <pds/include/wlPdsMemIds.h>
#include <z3device/common/include/zgpAppInterface.h>
#ifdef ZAPPSI_HOST
#include <zsiNotify.h>
#endif
#if defined ZAPPSI_HOST
#include <zsiHalSerialization.h>
#endif
#include <app_zigbee/zigbee_console/console.h>
#include <z3device/common/include/errH.h>
#include "app_zigbee/app_zigbee.h"
#include "app_zigbee/app_zigbee_handler.h"
#include  "app.h"
#include <zdo/include/private/zdoDbg.h>
#include <bdb/include/bdbZllBindings.h>
#include "zllplatform/ZLL/N_Connection/include/N_Connection.h"
#include "zllplatform/ZLL/N_DeviceInfo/include/N_DeviceInfo.h"
#include "zllplatform/ZLL/N_Connection/include/N_Connection_Internal.h"
#include "zllplatform/ZLL/N_LinkTarget/include/N_LinkTarget.h"

#ifdef OTAU_CLIENT
#include <zcl/include/zclOtauClient.h>
#endif
#ifdef _GREENPOWER_SUPPORT_
#if APP_ZGP_DEVICE_TYPE >= APP_ZGP_DEVICE_TYPE_PROXY_BASIC
#include <z3device/common/include/zgpAppInterface.h>
#endif // APP_ZGP_DEVICE_TYPE >= APP_ZGP_DEVICE_TYPE_PROXY_BASIC
#endif // _GREENPOWER_SUPPORT_
#ifndef APP_COMMISSIONING_TOUCHLINK
  #define APP_COMMISSIONING_TOUCHLINK 0
#endif

#ifndef APP_COMMISSIONING_STEERING
  #define APP_COMMISSIONING_STEERING 0
#endif

#ifndef APP_COMMISSIONING_FORMING
  #define APP_COMMISSIONING_FORMING 0
#endif

#ifndef APP_COMMISSIONING_FINDING_AND_BINDING
  #define APP_COMMISSIONING_FINDING_AND_BINDING 0
#endif

//Table 4 �?? Bits of the bdbCommissioningMode attribute
//5.3.2 bdbCommissioningMode attribute
#define BDB_COMMISSIONING_TOUCHLINK       0U //(1)
#define BDB_COMMISSIONING_NWK_STEERING    1U // (2)
#define BDB_COMMISSIONING_NWK_FORMATION   2U // (4)
#define BDB_COMMISSIONING_FINDING_BINDING 3U // (8)

#define TOUCHLINKING_MASK     (1U << BDB_COMMISSIONING_TOUCHLINK)
#define NWK_STEERING_MASK     (1U << BDB_COMMISSIONING_NWK_STEERING)
#define NWK_FORMATION_MASK    (1U << BDB_COMMISSIONING_NWK_FORMATION)
#define FINDING_BINDING_MASK  (1U << BDB_COMMISSIONING_FINDING_BINDING)

#define STEERING_AND_FINDING_BINDING_MASK  (NWK_STEERING_MASK | FINDING_BINDING_MASK)
#define TOUCHLINK_STEERING_FORMING_MASK    (TOUCHLINKING_MASK | NWK_STEERING_MASK | NWK_FORMATION_MASK)

#define APP_MAX_FIND_AND_BIND_ATTEMPTS      (5)
#define VISUALIZATION_PERIOD                (100U)
#define DEFAULT_VISUALIZATION_PERIODS       (1000U / VISUALIZATION_PERIOD)
#define JOIN_NETWORK_VISUALIZATION_PERIODS  (2000U / VISUALIZATION_PERIOD)

#define INTERPAN_PERIOD                     (10000U)

#define IDENTIFY_NON_COLOR_EFFECT false

#define ACTIVITY_TIMER_PERIOD     (10000U)

#define EXT_INT_PIN 0U

// the application task will block on this semaphore until we have new data to process

static AppState_t appState = APP_INITIAL_STATE;
AppDeviceState_t appDeviceState = DEVICE_INITIAL_STATE;
static BDB_SetTargetType_t bdbAppSetTargetType;

static uint8_t epIndex;
static uint16_t visualizationPeriodsCounter;

static void UpdateNetworkIndication(void);
static void QeryingCompleted(void);
static void IdentifyStartIndication(uint16_t timeoutInSec);
static void Connected(void);
static void Disconnected(void);
static void IdentifyStopIndication(void);
static void JoinNetworkIndication(uint16_t groupIdFirst, uint16_t groupIdLast);
static DeviceType_t detectNwkAddrAndDevType(void);

static void NetworkFormationCompleted(BDB_CommissioningStatus_t status);
static void NetworkSteeringCompleted(BDB_CommissioningStatus_t status);
static void TouchlinkCompleted(BDB_CommissioningStatus_t status);
static void FindingBindingCompleted(BDB_CommissioningStatus_t status);
static uint8_t currentCommissioning;
static BDB_CommissioningMode_t storeCommMode;
/* BDB Event Handlers */
static void interPanModeTimerFired(void);
static void ResetToFactoryDefaults(void);
static void Querying(void);
static void Scanning(void);
static void Identifying(void);
static void Joining(void);

static void bdbInitCompleted(void);

void create_app_queue(void);
void APP_EvtUpload(void);
void App_SetCommissioningStatus(bool val);
static uint8_t currentCommissioning;
#if BDB_TOUCHLINK_SUPPORT == 1
static void setTargetTypeDone(BDB_LinkTarget_Status_t status);
#endif
static void FindingBindingInitiatorModeCompleted(void);

#ifdef ZAPPSI_HOST
static void resetTimerFired(void);
#endif // ZAPPSI_HOST

static bool commissioningInProgress = false;
#if defined(ZIGBEE_END_DEVICE)
static uint32_t sleepPeriodConfigured;
static bool parentLost = false;
#endif


static void visualizationTimerFired(void);

static void networkEventsHandler(SYS_EventId_t eventId, SYS_EventData_t data);

static SYS_EventReceiver_t networkEventsListener  = { .func = networkEventsHandler};

#if defined(ZIGBEE_END_DEVICE)
static void restartActivity(void);

static HAL_AppTimer_t activityTimer =
{
  .mode     = TIMER_ONE_SHOT_MODE,
  .interval = ACTIVITY_TIMER_PERIOD,
  .callback = restartActivity
};

#endif 
static HAL_AppTimer_t visualizationTimer =
{
  .mode     = TIMER_REPEAT_MODE,
  .interval = VISUALIZATION_PERIOD,
  .callback = visualizationTimerFired
};

static HAL_AppTimer_t interPanModeTimer =
{
  .mode     = TIMER_ONE_SHOT_MODE,
  .interval = INTERPAN_PERIOD,
  .callback = interPanModeTimerFired
};
#ifdef ZAPPSI_HOST
static HAL_AppTimer_t resetTimer =
{
  .interval = 150,
  .mode     = TIMER_ONE_SHOT_MODE,
  .callback = resetTimerFired,
};
#endif

static BdbEventSubscriber_t s_BDB_EventsCallback =
{
  .connected = Connected,
  .disconnected = Disconnected,
  .identifyStartIndication = IdentifyStartIndication,
  .identifyStopIndication = IdentifyStopIndication,
  .updateNetworkIndication = UpdateNetworkIndication,
  .queryingCompleted = QeryingCompleted,
  .joinNetworkIndication = JoinNetworkIndication,
  .SteeringNetwork = NULL,
  .tclkProcedureOngoing = NULL,
  .tclkProcedureCompleted = NULL,
  .addingBindingLinks = NULL,
  .scanIndication = NULL,
  .touchlinkCompleted = TouchlinkCompleted,
  .networkFormationCompleted = NetworkFormationCompleted,
  .SteeringCompleted = NetworkSteeringCompleted,
  .findingBindingCompleted = FindingBindingCompleted,
  .ResetToFactoryDefaults = ResetToFactoryDefaults,
  .querying = Querying,  
  .scanning = Scanning,
  .identifying = Identifying,
  .joining = Joining,
  .findingBindingInitiatorModeCompleted = FindingBindingInitiatorModeCompleted,
};

static BDB_InvokeCommissioningReq_t commissioningReq;
static bool findAndBind = false;
static uint8_t joinInterval = APP_JOIN_ATTEMPT_INTERVAL;
static uint8_t joinAttempts = APP_JOIN_ATTEMPTS_AMOUNT;
static void joinRetryTimerFired(void);

static HAL_AppTimer_t joinRetryTimer =
{
  .interval = APP_JOIN_ATTEMPT_INTERVAL* 1000,
  .mode     = TIMER_ONE_SHOT_MODE,
  .callback = joinRetryTimerFired,
};
static uint8_t touchlinkAttempts = APP_JOIN_ATTEMPTS_AMOUNT;

static BDB_InitiatorReq_t appInitiatorReq;

AppBindReq_t* deviceBindReqs[APP_ENDPOINTS_AMOUNT];
BDB_CommissioningMode_t autoCommissionsEnableMask = ((APP_COMMISSIONING_TOUCHLINK << 0) | (APP_COMMISSIONING_STEERING << 1) |
  (APP_COMMISSIONING_FORMING << 2 ) | (APP_COMMISSIONING_FINDING_AND_BINDING << 3));


extern TaskHandle_t zigbeeTaskHandle;

/**************************************************************************
\brief Create Application queue for zigbee and usart events
***************************************************************************/
void create_app_queue(void)
{
#ifdef H3_INDEPENDENT   
  g_appQueue = xQueueCreate( 64, sizeof(APP_Msg_T) );
 //   APP_Initialize();
#endif    
}

/**************************************************************************
\brief Zigbee API CALL
***************************************************************************/
void ZIGBEE_API_CALL(Stack_API_Request_t *request)
{
  (void)xTaskAbortDelay(zigbeeTaskHandle);
  if ( OSAL_RESULT_TRUE != (bool)(OSAL_QUEUE_Send(&zigbeeRequestQueueHandle, &request,10)))
  {
    while(true)   // ERROR , Should not hang here, handle with assert
    {
    }
  }
}

/**************************************************************************
\brief Post Zigbee API CALLs into queue
***************************************************************************/
void APP_EvtUpload(void)
{
    APP_Msg_T   appMsg;
    APP_Msg_T   *p_appMsg;

    appMsg.msgId=(uint8_t)APP_MSG_ZB_STACK_EVT;

    p_appMsg = &appMsg;
#ifdef H3_INDEPENDENT
    OSAL_QUEUE_Send(&g_appQueue, p_appMsg, 0);
#else
    (void)OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0);
#endif

}
/**************************************************************************
\brief Join retry timer fired callback
***************************************************************************/
static void joinRetryTimerFired(void)
{
  if(commissioningReq.mode == BDB_COMMISSIONING_NWK_STEERING)
  {
    autoCommissionsEnableMask |= NWK_STEERING_MASK;
  }
  if(commissioningReq.mode == BDB_COMMISSIONING_TOUCHLINK)
  {
    autoCommissionsEnableMask |= TOUCHLINKING_MASK;
  }
  appState = APP_START_NETWORK_STATE;
  //OSAL_SEM_Post(&SEM_ZB_APP_TASK);
  APP_EvtUpload();
}
/**************************************************************************//**
\brief Checks if initialization has been done
******************************************************************************/
static void visualizationTimerFired(void)
{
  if (visualizationPeriodsCounter != 0U)
  {
    visualizationPeriodsCounter--;
    LED_TOGGLE();
  }
  else
  {
    (void)HAL_StopAppTimer(&visualizationTimer);
    LED_OFF();
  }
}
#if defined(ZIGBEE_END_DEVICE)
static void restartActivity(void)
{
  if (ZDO_IN_NETWORK_STATUS != ZDO_GetNwkStatus())
  {
    (void)HAL_StartAppTimer(&activityTimer);
  }
  else
  {
    (void)HAL_StopAppTimer(&activityTimer);
  }
}
#endif
/**************************************************************************//**
\brief Register Endpoint

\returns true, if Register Endpoint is Successful, false otherwise
******************************************************************************/
bool APP_RegisterEndpoint(ZCL_DeviceEndpoint_t *endpoint, AppBindReq_t* bindReq)
{
  if (APP_ENDPOINTS_AMOUNT == epIndex)
  {
    return false;
  }
  ZCL_RegisterEndpoint(endpoint);
  deviceBindReqs[epIndex++] = bindReq;
  return true;
}

/*******************************************************************************
  Function:
    void bdbInitCompleted(void)

  Remarks:
   bdb Initialisationn complete

*/
static void bdbInitCompleted(void)
{
}

/**************************************************************************//**
\brief Performs operations with network address which depends
  static or dinamyc on addressing type

\returns device type.
*******************************************************************************/
static DeviceType_t detectNwkAddrAndDevType(void)
{
  DeviceType_t deviceType;
#if (1 == CS_NWK_UNIQUE_ADDR)
  ShortAddr_t nwkAddr;

  // Read NWK address as dipswitch's state.
  nwkAddr = NWK_NODE_ADDRESS;
  if (0 == nwkAddr)
    deviceType = DEVICE_TYPE_COORDINATOR;
  else
    deviceType = DEVICE_TYPE_ROUTER;
  // Set parameter to config server
  CS_WriteParameter(CS_NWK_ADDR_ID, &nwkAddr);
#else // (1 != CS_NWK_UNIQUE_ADDR)
  deviceType = (DeviceType_t)APP_DEVICE_TYPE;
#endif // (1 == CS_NWK_UNIQUE_ADDR)
  return deviceType;
}

/**************************************************************************//**
\brief Callback on completion FindingAndBinding as Initiator
******************************************************************************/
static void FindingBindingInitiatorModeCompleted(void)
{
//  LCD_PRINT(0, 2,"                            ");
  LCD_PRINT(0, 9,"FBSuccessful");

  if (epIndex < APP_ENDPOINTS_AMOUNT)
  {
    appIdentifyStop();
  }
}
/**************************************************************************//**
\brief Callback on Querying
******************************************************************************/
static void Querying(void)
{
//  LCD_PRINT(0, 2,"                            ");
  LCD_PRINT(0, 9,"Querying");
}
/**************************************************************************//**
\brief Callback on Scanning in Progress
******************************************************************************/
static void Scanning(void)
{
//  LCD_PRINT(0, 2,"                            ");
  LCD_PRINT(0, 9,"Scanning....");  
}
/**************************************************************************//**
\brief Callback on Identifying in Progress
******************************************************************************/
static void Identifying(void)
{
//  LCD_PRINT(0, 2,"                            ");
//  LCD_PRINT(0, 2,"Identifying....");  
}
/**************************************************************************//**
\brief Callback on Joining in Progress
******************************************************************************/
static void Joining(void)
{
//  LCD_PRINT(0, 2,"                            ");
//  LCD_PRINT(0, 2,"Joining....");  
}
/**************************************************************************//**
\brief Callback on completion of Nwk formation
\param status - Commissioning Status
******************************************************************************/
static void NetworkFormationCompleted(BDB_CommissioningStatus_t status)
{
  APP_Zigbee_Event_t event;
  if (BDB_COMMISSIONING_SUCCESS == status)
  {
//    LCD_PRINT(0, 2,"                      ");
    LCD_PRINT(0, 9,"Network Formed");
    currentCommissioning = NWK_FORMATION_MASK;
  }
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_COMMISSIONING_FORMATION_COMPLETE;
  event.eventData.value = (uint8_t)status;
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief Callback on completion of Finding and Binding as Target
\param status - Commissioning STatus
******************************************************************************/
static void FindingBindingCompleted(BDB_CommissioningStatus_t status)
{
  APP_Zigbee_Event_t event;
//  LCD_PRINT(0, 2,"                            ");
  LCD_PRINT(0, 9,"Ident Comp");

  currentCommissioning = FINDING_BINDING_MASK;
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_COMMISSIONING_FINDBIND_COMPLETE;
  event.eventData.value = (uint8_t)status;
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief Callback on completion of Nwk Steering
\param status - Commissioning Status
******************************************************************************/
static void NetworkSteeringCompleted(BDB_CommissioningStatus_t status)
{
  APP_Zigbee_Event_t event;
  if (BDB_COMMISSIONING_SUCCESS == status)
  {
//    LCD_PRINT(0, 2,"                        ");
    LCD_PRINT(0, 9,"Steering    Sucessful");
    currentCommissioning = NWK_STEERING_MASK;
  }
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_COMMISSIONING_STEERING_COMPLETE;
  event.eventData.value = (uint8_t)status;
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief Callback on completion of Touchlink commssioning
\param status - Commissioning STatus - To be validated
******************************************************************************/
static void TouchlinkCompleted(BDB_CommissioningStatus_t status)
{
  APP_Zigbee_Event_t event;
  if (BDB_COMMISSIONING_SUCCESS == status)
  {
//    LCD_PRINT(0, 2,"                      ");
//    LCD_PRINT(0, 2,"Touchlink Completed");
    currentCommissioning = TOUCHLINKING_MASK;
  }
  else if(touchlinkAttempts != 0U)
  {
      touchlinkAttempts--;
      commissioningReq.mode = BDB_COMMISSIONING_TOUCHLINK;
      (void)HAL_StartAppTimer(&joinRetryTimer);
  }
  else if(!(bool)touchlinkAttempts)
  {
    commissioningReq.mode = storeCommMode; 
    (void)HAL_StartAppTimer(&joinRetryTimer);
  }
  else
  {
       //add else for avoid misra rule 15.7
  }
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_COMMISSIONING_TOUCHLINK_COMPLETE;
  event.eventData.value = (uint8_t)status;
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief Eanbles the touchlink target type, if touchlinking is supported on a router device
******************************************************************************/
static void setTouchlinkTargetType(void)
{
#if BDB_TOUCHLINK_SUPPORT == 1
  DeviceType_t deviceType;
  BDB_NodeCommissioningCapability_t commissioningCapability;
  CS_ReadParameter(CS_DEVICE_TYPE_ID, &deviceType);
  commissioningCapability = BDB_GetCommissioningCapability();
  if ((DEVICE_TYPE_ROUTER == deviceType) && (bool)(commissioningCapability.touchlinkComissioningSupported))
  {
    (void)memset(&bdbAppSetTargetType,0,sizeof(BDB_SetTargetType_t));
    bdbAppSetTargetType.targetType = 1;
    bdbAppSetTargetType.BDB_SetTargetTypeConf = setTargetTypeDone;
    BDB_SetTargetType(&bdbAppSetTargetType);
//    LCD_PRINT(0, 2,"                      ");
//    LCD_PRINT(0, 3,"                      ");
//    LCD_PRINT(0, 2," Touchlink Target");
  }
#endif
}

/**************************************************************************//**
\brief Callback on reception of ResetToFactoryBNew command
\param status - Commissioning STatus
******************************************************************************/
static void ResetToFactoryDefaults(void)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_RESET_TO_FACTORY_DEFAULTS;
  (void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief InterPan mode timer fired
******************************************************************************/
static void interPanModeTimerFired(void)
{
  setTouchlinkTargetType();
}
/**************************************************************************
\brief Check the starting of a network is required or not on startup
***************************************************************************/
static bool startNetwork(BDB_CommissioningMode_t commMode)
{
  if ((NWK_FORMATION_MASK == (NWK_FORMATION_MASK & commMode)) || (NWK_STEERING_MASK == (NWK_STEERING_MASK & commMode)) || (TOUCHLINKING_MASK == (TOUCHLINKING_MASK & commMode)) )
  {
    return true;
  }
  return false;
}

/**************************************************************************//**
\brief Listen different events from network

\param[in] eventId - id of raised event;
\param[in] data    - event's data.
******************************************************************************/
static void networkEventsHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
  static bool resetToFactoryNew = false;
  
  switch(eventId)
  {
    case (uint8_t)BC_EVENT_LEAVE_COMMAND_RECEIVED:
    {
      resetToFactoryNew = true;
      SYS_SubscribeToEvent((uint8_t)BC_EVENT_NETWORK_LEFT, &networkEventsListener);
    }
    break;
    
    case (uint8_t)BC_EVENT_NETWORK_LEFT:
    {
      APP_Zigbee_Event_t event;
      event.eventGroup = EVENT_ZIGBEE;
      event.eventId = EVENT_LEFT_FROM_NETWORK;
	  (void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
      APP_Zigbee_Handler(event);
      SYS_UnsubscribeFromEvent((uint8_t)BC_EVENT_NETWORK_LEFT, &networkEventsListener);
      if (resetToFactoryNew)
      {
#ifdef _ENABLE_PERSISTENT_SERVER_
        (void)PDS_DeleteAll(false);
#endif
        SystemReset();
      }
      else //need to be checked
      {
        if (startNetwork((APP_COMMISSIONING_TOUCHLINK << 0) | (APP_COMMISSIONING_STEERING << 1) | (APP_COMMISSIONING_FORMING << 2 )))
        {
          appState = APP_START_NETWORK_STATE;
          APP_EvtUpload();
        }
      }
    }
    break;

    case (uint8_t)BC_EVENT_NETWORK_ENTERED:      /* The device is in the network */
    { /* To handle finding and binding only enabled case */
      if (APP_START_WAIT_STATE == appState)
      {
        if (findAndBind)
        {
          commissioningReq.mode = BDB_COMMISSIONING_FINDING_BINDING;
          appState = APP_FINDING_BINDING_STATE;
        }
        else
        {
          appState = APP_IN_NETWORK_IDLE_STATE;
        }
        APP_EvtUpload();
      }

    }
    break;
    default:
        /* TO DO */
    break;
  }
  (void)eventId;
  (void)data;
}


/**************************************************************************
\brief To Handle steeringfailure from application
***************************************************************************/
static void handleSteeringFailure(void)
{
  APP_Zigbee_Event_t event;
  if (commissioningReq.mode == BDB_COMMISSIONING_NWK_STEERING)
  {
    if ((bool)joinInterval && (bool)joinAttempts)
    {     
      (void)HAL_StartAppTimer(&joinRetryTimer);
      commissioningInProgress = true;
    }
    else if (!(bool)joinInterval && (bool)joinAttempts)
    {
      commissioningReq.mode = BDB_COMMISSIONING_NWK_STEERING;
      /* Try to rejoin joinAttempts time with no delay */
      appState = APP_START_NETWORK_STATE;
      APP_EvtUpload();
    }
    else
    {
//      LCD_PRINT(0, 2,"                            ");
      LCD_PRINT(0, 9, "No Ntw Found");
      event.eventGroup = EVENT_ZIGBEE;
      event.eventId = EVENT_COMMISSIONING_FAILURE;
      event.eventData.value = (uint8_t)BDB_COMMISSIONING_NO_NETWORK;
      APP_Zigbee_Handler(event);
      appState = APP_START_NETWORK_STATE;
      APP_EvtUpload();
      return;
    }

    if ( (bool)joinAttempts && (joinAttempts !=  0xffU))
    {
      joinAttempts--;
    }
  }
}

/*************************************************************//**
\brief CallBack on finishing the SetTartgetType
*****************************************************************/
#if BDB_TOUCHLINK_SUPPORT == 1
static void setTargetTypeDone(BDB_LinkTarget_Status_t status)
{
  /* Do Nothing */
  (void)status;
}
#endif

/**************************************************************************
\brief determine enabled commissioning modes
***************************************************************************/
BDB_CommissioningMode_t determineCommissionMode(void)
{
  BDB_CommissioningMode_t retMode = 0xffU;

  if ((autoCommissionsEnableMask & TOUCHLINKING_MASK) != 0U)
  {
    retMode = BDB_COMMISSIONING_TOUCHLINK;
    autoCommissionsEnableMask &= (uint8_t)~TOUCHLINKING_MASK;
  }
   else if ((autoCommissionsEnableMask & NWK_STEERING_MASK) != 0U)
  {
    retMode = BDB_COMMISSIONING_NWK_STEERING;
    autoCommissionsEnableMask &= (uint8_t)~NWK_STEERING_MASK;
  }
  else if ((autoCommissionsEnableMask & NWK_FORMATION_MASK) != 0U)
  {
    retMode = BDB_COMMISSIONING_NWK_FORMATION;
    autoCommissionsEnableMask &=  (uint8_t)~NWK_FORMATION_MASK;
  }
  else if ((autoCommissionsEnableMask & FINDING_BINDING_MASK) != 0U)
  {
    retMode = BDB_COMMISSIONING_FINDING_BINDING;
    autoCommissionsEnableMask &=  (uint8_t)~FINDING_BINDING_MASK;
  }
  else
  {  
      //add else for avoid misra 15.7
  }
    return retMode;
}

/**************************************************************************//**
\brief Performs join retries if the steering is nwk failed
*******************************************************************************/
static void commissioningDone(BDB_InvokeCommissioningConf_t *conf)
{
  APP_Zigbee_Event_t event;
  commissioningInProgress = false;
  switch (conf->status)
  {
    //!< commissioning sub procedure is successful
    case BDB_COMMISSIONING_SUCCESS:
    {
      switch (currentCommissioning)
      {
        case TOUCHLINKING_MASK:
        {
          ShortAddr_t nwkAddr;
          APP_Zigbee_Event_t event_t;
          ExtPanId_t       csExtPanId;
          appState = APP_IN_NETWORK_STATE;
//          LCD_PRINT(0, 2,"                            ");
//          LCD_PRINT(0, 2, "Touchlink successful");
          CS_ReadParameter(CS_NWK_PARENT_ADDR_ID, &nwkAddr);
          const ExtAddr_t* parentExtAddr =  NWK_GetExtByShortAddress(nwkAddr);
          CS_ReadParameter(CS_EXT_PANID_ID, &csExtPanId);
          event_t.eventGroup = EVENT_ZIGBEE;
          event_t.eventId = EVENT_JOINED_TO_AN_EXISTING_NETWORK;
          event_t.eventData.ParentChildInfo.shortAddress = nwkAddr;
          if (parentExtAddr != NULL)
          {
             (void)memcpy(&event_t.eventData.ParentChildInfo.extendedAddress, parentExtAddr, 8);
          }
          else
          {
            event_t.eventData.ParentChildInfo.extendedAddress = 0;
          }
          event_t.eventData.ParentChildInfo.extendedPanId = csExtPanId;
          APP_Zigbee_Handler(event_t);
        }
        break;
        case NWK_STEERING_MASK:
        {
          ShortAddr_t nwkAddr;
          APP_Zigbee_Event_t event_t;
          ExtPanId_t       csExtPanId;
//          LCD_PRINT(0, 2, "                          ");
          LCD_PRINT(0, 9, "Steering success");
          if (appState != APP_IN_NETWORK_PERMITJOIN_STATE)
          {
            CS_ReadParameter(CS_NWK_PARENT_ADDR_ID, &nwkAddr);
            const ExtAddr_t* parentExtAddr =  NWK_GetExtByShortAddress(nwkAddr);
            CS_ReadParameter(CS_EXT_PANID_ID, &csExtPanId);
            event_t.eventGroup = EVENT_ZIGBEE;
            event_t.eventId = EVENT_JOINED_TO_AN_EXISTING_NETWORK;
            event_t.eventData.ParentChildInfo.shortAddress = nwkAddr;
            if (parentExtAddr != NULL)
            {
                (void)memcpy(&event_t.eventData.ParentChildInfo.extendedAddress, parentExtAddr, 8);
            }
            else
            {
               event_t.eventData.ParentChildInfo.extendedAddress = 0;
            }
            event_t.eventData.ParentChildInfo.extendedPanId = csExtPanId;
            APP_Zigbee_Handler(event_t);
          }
          appState = APP_IN_NETWORK_STATE;
        }
        break;
        case NWK_FORMATION_MASK:
        {
          ShortAddr_t nwkAddr;
          APP_Zigbee_Event_t event_t;
//          LCD_PRINT(0, 2,"                            ");
          LCD_PRINT(0, 9, "Nwk Formation");
          appState = APP_IN_NETWORK_PERMITJOIN_STATE;
          CS_ReadParameter(CS_NWK_ADDR_ID, &nwkAddr);
          event_t.eventGroup = EVENT_ZIGBEE;
          if (nwkAddr == 0U)
          {
            event_t.eventId = EVENT_STARTED_CENTRALIZED_NETWORK;
          }
          else
          {
            event_t.eventId = EVENT_STARTED_DISTRIBUTED_NETWORK;
          }
		  (void)memset(&event_t.eventData, 0, sizeof(APP_Zigbee_EventData));
          APP_Zigbee_Handler(event_t);
        }
        break;
        case FINDING_BINDING_MASK:
        {
          currentCommissioning = 0;
          if (epIndex < APP_ENDPOINTS_AMOUNT)
          {
            appState = APP_FINDING_BINDING_STATE;
          }
          else
          {
            appState = APP_IN_NETWORK_IDLE_STATE;
            event.eventGroup = EVENT_ZIGBEE;
            event.eventId = EVENT_COMMISSIONING_COMPLETE;
	        event.eventData.value = (uint8_t)appState;
            APP_Zigbee_Handler(event);
          }
        }
        break;
        default: // Shall not come here
          appState = APP_START_WAIT_STATE;
          //appSnprintf("CommissioningStatus = In wait State\r\n");
        break;
      }
      currentCommissioning = 0;
      APP_EvtUpload();
    }
    break;
    //!< The initiator is not address assignment capable during touchlink
    case BDB_COMMISSIONING_NOT_AA_CAPABLE:
    //!< A network could not be formed during network formation
    case BDB_COMMISSIONING_FORMATION_FAILURE:
    //!< A binding table entry could not be created due to insufficient space in 
    //the binding table during finding & binding.
    case BDB_COMMISSIONING_BINDING_TABLE_FULL:
    //!< The Trust Center link key exchange procedure has failed attempting to 
    //join a centralized security network
    case BDB_COMMISSIONING_TCLK_EX_FAILURE:
    //!< The given commissioning is not supported
    case BDB_COMMISSIONING_NOT_SUPPORTED:
      event.eventGroup = EVENT_ZIGBEE;
      event.eventId = EVENT_COMMISSIONING_FAILURE;
      event.eventData.value = (uint8_t)(conf->status);
      APP_Zigbee_Handler(event);
      break;
    //!< A network has not been found during network steering or touchlink
    case BDB_COMMISSIONING_NO_NETWORK:
      handleSteeringFailure();
      break;

    //!< No response to an identify query command has been received during finding & binding
    case BDB_COMMISSIONING_NO_IDENTIFY_QUERY_RESPONSE:
      event.eventGroup = EVENT_ZIGBEE;
      event.eventId = EVENT_COMMISSIONING_FAILURE;
      event.eventData.value = (uint8_t)(conf->status);
      APP_Zigbee_Handler(event);
      currentCommissioning = 0;
      if (epIndex < APP_ENDPOINTS_AMOUNT)
      {
         appState = APP_FINDING_BINDING_STATE;
      }
      else
      {
         appState = APP_IN_NETWORK_IDLE_STATE;
         event.eventGroup = EVENT_ZIGBEE;
         event.eventId = EVENT_COMMISSIONING_COMPLETE;
         APP_Zigbee_Handler(event);
      }
      APP_EvtUpload();
      break;

    //!< No response to a scan request inter-PAN command has been received during touchlink
    case BDB_COMMISSIONING_NO_SCAN_RESPONSE:
      event.eventGroup = EVENT_ZIGBEE;
      event.eventId = EVENT_COMMISSIONING_FAILURE;
      event.eventData.value = (uint8_t)(conf->status);
      commissioningInProgress = true;
      APP_Zigbee_Handler(event);
      /* If the router device couldn't find any target during touchlinking, let it continue as Target */
      setTouchlinkTargetType();
      break;

    default:
        /* TO DO */
      break;
  }
}
/**************************************************************************//**
\brief Reset the report configuration of the device's reportable attributes

\param[in] none
******************************************************************************/
void resetReportConfig(void)
{
  APS_RegisterEndpointReq_t *endpointDescriptor = NULL;
  uint8_t clusterCount = 0;
  while (NULL != (endpointDescriptor = APS_NextEndpoint(endpointDescriptor)))
  {
    for(clusterCount = 0; clusterCount < endpointDescriptor->simpleDescriptor->AppInClustersCount; clusterCount++)
    {
      ZCL_Cluster_t *cluster = ZCL_GetCluster(endpointDescriptor->simpleDescriptor->endpoint,
                                              endpointDescriptor->simpleDescriptor->AppInClustersList[clusterCount],
                                              ZCL_CLUSTER_SIDE_SERVER);
      ZclAttribute_t *attr = NULL;

      if(cluster == NULL)
      {
        continue;
      }
      attr = (ZclAttribute_t *)cluster->attributes;
      // For all the attributes
      for (uint8_t attrIndex = 0; attrIndex < cluster->attributesAmount; attrIndex++)
      {
        // check if the attribute is reportable and the 'reporting configured' is already set
        if ((bool)(attr->properties & ZCL_REPORTABLE_ATTRIBUTE) && (bool)(attr->properties & ZCL_REPORTING_CONFIGURED))
        {
          // Resetting the attribute's reporting configuration property
          attr->properties &= (uint8_t)(~ZCL_REPORTING_CONFIGURED);
        }
        attr = jumpToNextAttribute(attr);
      }
    }
  }
}
/**************************************************************************//**
\brief Network update notification
\param notify - update notification parameters
******************************************************************************/
void ZDO_MgmtNwkUpdateNotf_CB(ZDO_MgmtNwkUpdateNotf_t *notify)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_ZIGBEE;
  switch(notify->status)
  {
    case (uint8_t)ZDO_CHILD_JOINED_STATUS:
    {
       ExtPanId_t csExtPanId;
       const ExtAddr_t* parentExtAddr =  NWK_GetExtByShortAddress(notify->childAddr.shortAddr);
       CS_ReadParameter(CS_EXT_PANID_ID, &csExtPanId);
       event.eventId = EVENT_CHILD_JOINED;
       event.eventData.ParentChildInfo.shortAddress = notify->childAddr.shortAddr;
       (void)memcpy(&event.eventData.ParentChildInfo.extendedAddress, parentExtAddr, 8);
       event.eventData.ParentChildInfo.extendedPanId = csExtPanId;
       APP_Zigbee_Handler(event);
    }
    break;

    case (uint8_t)ZDO_CHILD_REMOVED_STATUS:
    {
      event.eventId = EVENT_CHILD_REMOVED;
      event.eventData.data = notify->childInfo.shortAddr;
      APP_Zigbee_Handler(event);
    }
    break;

    case (uint8_t)ZDO_NWK_UPDATE_STATUS:
    {
      event.eventId = EVENT_NWK_UPDATE;
	  (void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
      APP_Zigbee_Handler(event);
    }
    break;

    default:
        /* TO DO */
    break;
  }
}

/*******************************************************************************
  Description: just a stub.

  Parameters: none.

  Returns: nothing.
*******************************************************************************/
void ZDO_WakeUpInd_CB(void)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_WAKEUP;
  (void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief ZDO Binding indication function

\param[out] bindInd - ZDO bind indication parameters structure pointer.
                      For details go to ZDO_BindInd_t declaration
******************************************************************************/
void ZDO_BindIndication_CB(ZDO_BindInd_t *bindInd)
{
  (void)bindInd;
}

/**************************************************************************//**
\brief ZDO Unbinding indication function

\param[out] unbindInd - ZDO unbind indication parameters structure pointer.
                        For details go to ZDO_UnbindInd_t declaration
******************************************************************************/
void ZDO_UnbindIndication_CB(ZDO_UnbindInd_t *unbindInd)
{
  (void)unbindInd;
}

/**************************************************************************//**
\brief ZDO_ResetNetwork Confirmation function

\param[out] conf - Confirmation structure with status
******************************************************************************/
void ZDO_ResetNetworkConf_CB(ZDO_ResetNetworkConf_t *conf)
{
  sysAssert(ZDO_SUCCESS_STATUS == conf->status, ZDOINIT_ZDOINITRESETNETWORKCONF_0);
  APP_EvtUpload();
}

/**************************************************************************//**
\brief Check the commssioning in progress status

\returns true, if commissioning is in progress, false otherwise
******************************************************************************/
bool isCommssioiningInProgress(void)
{
  return commissioningInProgress;
}

/**************************************************************************//**
\brief set/reset the commissioning in progress status

\returns true, if commissioning is in progress, false otherwise
******************************************************************************/
void App_SetCommissioningStatus(bool val)
{
    commissioningInProgress = val;
}

/**************************************************************************//**
\brief Callback function for user button short press action.
*****************************************************************************/
static void userButtonShortPressAction(void)
{
    /* Add custom action when user button short pressed here. */
}

/**************************************************************************//**
\brief Application and stack parameters initialization
*****************************************************************************/
static void initApp(void)
{
  ExtAddr_t extAddr;
  DeviceType_t deviceType;
  //Reads the UID set in configuration.h
  CS_ReadParameter(CS_UID_ID,&extAddr);
  if (extAddr == 0U || extAddr > APS_MAX_UNICAST_EXT_ADDRESS)
  {
    //BSP_ReadUid(&extAddr); //Will read the UID from chip
    CS_WriteParameter(CS_UID_ID, &extAddr); //Writes the read UID to the ram
  }

  CS_WriteParameter(CS_PERMIT_DURATION_ID, &(uint8_t){0});

  deviceType = detectNwkAddrAndDevType();
  // Set parameters to config server
  CS_WriteParameter(CS_DEVICE_TYPE_ID, &deviceType);
  

  errHInit();
  /* BDB Advance features like parallel Finding & Binding are enabled which is non Compliance */
  /* Few additional prints are added for demopurpose */
  BDB_EnableAutoCommissioning();
  BDB_EventsSubscribe(&s_BDB_EventsCallback);
  BDB_Init(bdbInitCompleted);
  

  //BSP_OpenLeds();
  appDeviceInit();
#ifdef _ZCL_REPORTING_SUPPORT_
    resetReportConfig();
    ZCL_StartReporting();
#endif
#ifdef _GREENPOWER_SUPPORT_
#if APP_ZGP_DEVICE_TYPE >= APP_ZGP_DEVICE_TYPE_PROXY_BASIC
  ZGP_AppInit();
#endif //#if APP_ZGP_DEVICE_TYPE >= APP_ZGP_DEVICE_TYPE_PROXY_BASIC
#endif //_GREENPOWER_SUPPORT_

  epIndex = 0;
  SYS_SubscribeToEvent((uint8_t)BC_EVENT_LEAVE_COMMAND_RECEIVED, &networkEventsListener);
  SYS_SubscribeToEvent((uint8_t)BC_EVENT_NETWORK_ENTERED, &networkEventsListener);

  commissioningReq.BDB_InvokeCommissioningConf = commissioningDone;

  /* Check the NFN Status of the device */
  if (BDB_GetBdbNodeIsOnANetwork())
  {
#if defined(ZIGBEE_END_DEVICE)
  if (DEVICE_TYPE_END_DEVICE == deviceType)
  {
#if BDB_TOUCHLINK_SUPPORT == 1
    if (!N_DeviceInfo_IsFactoryNew() && !N_Connection_IsConnected())
#else
    if(!N_Connection_IsConnected())
#endif   //#if BDB_TOUCHLINK_SUPPORT == 1
    {
      N_Connection_ReconnectUrgent();
    }
  }
#endif   //#if defined(ZIGBEE_END_DEVICE)
    appState = APP_IN_NETWORK_IDLE_STATE;    
    APP_EvtUpload();
//    LCD_PRINT(0, 1,"                            ");
//    LCD_PRINT(0, 1, "NFN Node");
    return;
  }

  storeCommMode = autoCommissionsEnableMask;
  if ((autoCommissionsEnableMask & FINDING_BINDING_MASK) == FINDING_BINDING_MASK)
  {
    findAndBind = true;
  }
  else
  {
    findAndBind = false;
  }
  /* Next action taken based on commissoining mode selected */
  if (startNetwork(autoCommissionsEnableMask))
  {
    APP_Zigbee_Event_t event;
    event.eventGroup = EVENT_ZIGBEE;
    event.eventId = EVENT_COMMISSIONING_STARTED;
	(void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
    APP_Zigbee_Handler(event);
    appState = APP_START_NETWORK_STATE;
  }
  else
  {
    appState = APP_START_WAIT_STATE;
  }
#if defined(ZIGBEE_END_DEVICE)
    restartActivity();
#endif	
  APP_EvtUpload();
}

/************************BDB Evenet Handling ******************************/
/**************************************************************************//**
\brief Callback on succeessfully connected to a network

******************************************************************************/
static void Connected(void)
{
  ExtPanId_t       csExtPanId;
  ExtPanId_t       csNwkExtPanId;
  APP_Zigbee_Event_t event;
#if (MICROCHIP_APPLICATION_SUPPORT == 1)
    CS_ReadParameter(CS_EXT_PANID_ID, &csExtPanId);
    // CS_EXT_PANID is 0 on default, after joining any network its ext panId shall be retained
    if (!(bool)csExtPanId)
    {
      CS_ReadParameter(CS_NWK_EXT_PANID_ID, &csNwkExtPanId);
      CS_WriteParameter(CS_EXT_PANID_ID, &csNwkExtPanId);
    }
#endif

#if defined(ZIGBEE_END_DEVICE)
  DeviceType_t deviceType;

  CS_ReadParameter(CS_DEVICE_TYPE_ID, &deviceType);

  if ((DEVICE_TYPE_END_DEVICE == deviceType ) && parentLost)
  {
    // On reconnection, end device will restore its sleep period value
    // because it was set to zero on previous disconnected event
    if (sleepPeriodConfigured != 0U)
    {
      CS_WriteParameter(CS_END_DEVICE_SLEEP_PERIOD_ID, &sleepPeriodConfigured);
      sleepPeriodConfigured = 0;
    }
    parentLost = false;
  }
#endif

  /* Start OTAU functionalty */
  appDeviceTaskHandler();
//  LCD_PRINT(0, 2, "         ");
  LCD_PRINT(0, 9,"NFN");

  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_NETWORK_ESTABLISHED;
  (void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief Callback on disconnection from a network

******************************************************************************/
static void Disconnected(void)
{
#if defined(ZIGBEE_END_DEVICE)
  DeviceType_t deviceType;

  CS_ReadParameter(CS_DEVICE_TYPE_ID, &deviceType);

  if (DEVICE_TYPE_END_DEVICE == deviceType)
  {
    uint32_t sleepPeriod;
    parentLost = true;
    // When disconnected event happens, then device won't not in network.
    // so settting end device sleep time to zero so that it will not do 
    // slow polling i.e polling in sleep period interval and taking backup of 
    // sleep period value
    CS_ReadParameter(CS_END_DEVICE_SLEEP_PERIOD_ID, &sleepPeriod);
    if (sleepPeriod != 0U)
    {
      sleepPeriodConfigured = sleepPeriod;
      sleepPeriod = 0;
      CS_WriteParameter(CS_END_DEVICE_SLEEP_PERIOD_ID, &sleepPeriod);
    }
    N_Connection_Reconnect();
  }
#endif
  
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_DISCONNECTED;
  (void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
  APP_Zigbee_Handler(event);
}

/**************************************************************************//**
\brief Callback on completion of Nwk Update
******************************************************************************/
static void UpdateNetworkIndication(void)
{

}

/**************************************************************************//**
\brief Callback on completion of Querying
******************************************************************************/
static void QeryingCompleted(void)
{
//  LCD_PRINT(0, 2,"                            ");
//  LCD_PRINT(0, 2,"Querying Completed");
}

/**************************************************************************//**
\brief Callback on Start of Identification as Target
\param timeoutInSec - identifying time secs
******************************************************************************/
static void IdentifyStartIndication(uint16_t timeoutInSec)
{
  (void)HAL_StopAppTimer(&interPanModeTimer);
  (void)HAL_StartAppTimer(&interPanModeTimer);
//  LCD_PRINT(0, 2,"                      ");
//  LCD_PRINT(0, 2,"Indentify Started");
  
  if (!(bool)visualizationPeriodsCounter)
  {
    // Converting to ms
    visualizationPeriodsCounter = (uint16_t)((timeoutInSec == 0xFFFFU) ? DEFAULT_VISUALIZATION_PERIODS : ((timeoutInSec * 1000U)/VISUALIZATION_PERIOD));
    (void)HAL_StartAppTimer(&visualizationTimer);
    LED_ON();
    appIdentifyStart(visualizationPeriodsCounter, IDENTIFY_NON_COLOR_EFFECT, 0u);
  }
  else
  {
    // Converting to ms
    visualizationPeriodsCounter = (uint16_t)(MAX(visualizationPeriodsCounter, ((timeoutInSec * 1000U)/VISUALIZATION_PERIOD)));
  }
}

/**************************************************************************//**
\brief Stops identifying.
******************************************************************************/
static void IdentifyStopIndication(void)
{
  visualizationPeriodsCounter = 0;
  (void)HAL_StopAppTimer(&visualizationTimer);
  appIdentifyStop();
  LED_OFF();
}
/**************************************************************************//**
\brief Indication that the device has been linked

\param[in] groupIdFirst - the first group identifier assigned to
  the device during linking; zero if no groups where assigned;
\param[in] groupIdLast  - the last group identifier assigned to
  the device during linking; zero if no groups where assigned
******************************************************************************/
static void JoinNetworkIndication(uint16_t groupIdFirst, uint16_t groupIdLast)
{
  if (!(bool)visualizationPeriodsCounter)
  {
    visualizationPeriodsCounter = JOIN_NETWORK_VISUALIZATION_PERIODS;
    (void)HAL_StartAppTimer(&visualizationTimer);
    LED_ON();
  }
  else
  {
    visualizationPeriodsCounter = MAX(visualizationPeriodsCounter, JOIN_NETWORK_VISUALIZATION_PERIODS);
  }
  (void)groupIdFirst;
  (void)groupIdLast;
}

/**************************************************************************//**
\brief Application task handler
******************************************************************************/
void process_ZB_evt(void)
{

  switch (appState)
  {
      // node is in initial state
      case APP_INITIAL_STATE:                 // Initial (after RESET) state
        appSnprintf("Zigbee Device Initialized: Type Help for Commands\r\n");
        initApp();                            // Init application
        break;

      case APP_START_WAIT_STATE:              //do nothing wait for user input to start commissioning
        // appSnprintf("App in wait State\r\n");
        break;

      case APP_START_NETWORK_STATE:
      {
//        LCD_PRINT(0, 2,"                            ");
        LCD_PRINT(0, 9, "Started Comm");
        commissioningInProgress = true;
        commissioningReq.mode = determineCommissionMode();
        BDB_InvokeCommissioning(&commissioningReq);
      }
      break;

      case APP_IN_NETWORK_STATE:              // Normal operating
      {
        if (findAndBind)
        {         
         commissioningReq.mode = BDB_COMMISSIONING_FINDING_BINDING;
         appState = APP_FINDING_BINDING_STATE;
        }
        else
        {  
          APP_Zigbee_Event_t event;
          event.eventGroup = EVENT_ZIGBEE;
          event.eventId = EVENT_COMMISSIONING_COMPLETE;
		  (void)memset(&event.eventData, 0, sizeof(APP_Zigbee_EventData));
          APP_Zigbee_Handler(event);
          appState = APP_IN_NETWORK_IDLE_STATE;
        }
        APP_EvtUpload();
      }
      break;

      case APP_IN_NETWORK_PERMITJOIN_STATE:
      {
        commissioningReq.mode = BDB_COMMISSIONING_NWK_STEERING;   //To do PermitJoin
        commissioningInProgress = true;
        BDB_InvokeCommissioning(&commissioningReq);
      }
      break;

      case APP_FINDING_BINDING_STATE:
      {
        AppBindReq_t *currBindReq = deviceBindReqs[epIndex++];
        
        appInitiatorReq.initiatorEndpoint = currBindReq->srcEndpoint;
        appInitiatorReq.clientClustersCount = currBindReq->remoteServersCnt;
        appInitiatorReq.serverClustersCount = currBindReq->remoteClientsCnt;

        appInitiatorReq.clientClustersList = currBindReq->remoteServers;
        appInitiatorReq.serverClustersList = currBindReq->remoteClients;

        appInitiatorReq.callback = currBindReq->callback;
        appInitiatorReq.groupId = currBindReq->groupId;

        commissioningReq.initiatorReq = &appInitiatorReq;
        commissioningInProgress = true;
        BDB_InvokeCommissioning(&commissioningReq);
      }
      break;
      case APP_IN_NETWORK_IDLE_STATE:
        break;
      default:
          /* TO DO */
        break;
  }
}


/**************************************************************************//**
\brief Backing up up required paramaters for stack

\param[in]
  bool None
******************************************************************************/
void SYS_BackupStackParams(uint32_t expectedSleepTime)
{
  ZDO_BackupZdoParams();
  CS_BackupNwkParams();
  HAL_BackupRunningTimers(expectedSleepTime);
}


#if defined WIN
/**************************************************************************//**
\brief Indicates reset Application.

\param[out]
\return none
******************************************************************************/
void restartApp(void)
{
  appState = APP_INITIAL_STATE;
  appDeviceState = DEVICE_INITIAL_STATE;
  idenityResetSubscribers();
  HAL_ResetAllTimers();
  BDB_ResetFsm();
  SYS_EnableTask(APL_TASK_ID);
  SYS_EnableTask(ZLL_TASK_ID);
  SYS_EnableTask(ZCL_TASK_ID);
  SYS_SysInit();
}
#endif
