/*******************************************************************************
  Application Zigbee Handler Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_zigbee_handler.c

  Summary:
    This file contains the Application Zigbee functions for this project.

  Description:
    This file contains the Application Zigbee functions for this project.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <app_zigbee/app_zigbee_handler.h>
#include <app_zigbee/zigbee_console/console.h>
#include <zcl/include/zclAlarmsCluster.h>
#include <zcl/include/zclZllIdentifyCluster.h>
#include <zcl/include/zclZllGroupsCluster.h>
#include <zcl/include/zclZllOnOffCluster.h>
#include <zcl/include/zclZllLevelControlCluster.h>
#include <zcl/include/zclZllColorControlCluster.h>
#include <zcl/include/zclZllScenesCluster.h>
#include <zcl/include/zclIasACECluster.h>
#include <zcl/include/zclIasZoneCluster.h>
#include <zcl/include/zclFanControlCluster.h>
#include <zcl/include/zclThermostatCluster.h>
#include <zcl/include/zclTemperatureMeasurementCluster.h>
#include <zcl/include/zclTimeCluster.h>
#include <zcl/include/zclThermostatUiConfCluster.h>
#include <zcl/include/zclIlluminanceMeasurementCluster.h>
#include <zcl/include/zcloccupancysensingcluster.h>
#include <z3device/combinedInterface/include/ciThermostatCluster.h>
#include <z3device/common/include/z3Device.h>

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
extern void temp_update(int16_t temp_value);
extern void hum_update(int16_t hum_value);
void BSP_Event_Handler(APP_Zigbee_Event_t event);
void Cluster_Event_Handler(APP_Zigbee_Event_t event);
void Zigbee_Event_Handler(APP_Zigbee_Event_t event);

#define MIN_COLOR_LEVEL           0u      //Added to avoid MISRA_c_2014_10.4 Violation
#define MAX_COLOR_LEVEL           0xfeffu

#define MIN_SATURATION_LEVEL      0u
#define MAX_SATURATION_LEVEL      0xfeu
// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Zigbee_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void APP_Zigbee_Handler(APP_Zigbee_Event_t event)
{
    switch(event.eventGroup)
    {
        case EVENT_BSP:
        {
            /* BSP Events Dispatcher */
            BSP_Event_Handler(event);
        }
        break;

        case EVENT_ZIGBEE:
        {
            /* Zigbee Events Dispatcher */
            Zigbee_Event_Handler(event);
        }
        break;

        case EVENT_CLUSTER:
        {
            /* Cluster Events Dispatcher */
            Cluster_Event_Handler(event);
        }
        break;

        default:
            /*default none handler */
        break;
    }
}

/*******************************************************************************
  Function:
    void BSP_Event_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void BSP_Event_Handler(APP_Zigbee_Event_t event)
{
    // User to handle  board Support package events
    switch(event.eventId)
    {
        case CMD_LED_OPEN:
        {
            /* Init/Open LEDs */
            //appSnprintf("Init /Open LEDs\r\n");
        }
        break;

        case CMD_LED_ON:
        {
            /* Turn on the LED */
            //Access - > event.eventData.value;
            //appSnprintf("On\r\n");
        }
        break;

        case CMD_LED_OFF:
        {
            /* Turn off the LED */
            //Access - > event.eventData.value;
            //appSnprintf("Off\r\n");
        }
        break;

        case CMD_LED_TOGGLE:
        {
            /* Toggle the LED */
            //Access - > event.eventData.value;
            //appSnprintf("Toggle\r\n");
        }
        break;
        case CMD_LED_BRIGHTNESS:
        {
            /* Set the given LED brightness */
            //Access - > event.eventData.value;
            //appSnprintf("Led Brightness \r\n");
        }
        break;

        case CMD_LED_COLOR_HS:
        {
            /* LED Hue , Saturation */
            //Access - > event.eventData.colorHS.hue;
            //Access - > event.eventData.colorHS.saturation;
            //appSnprintf("LED Hue , Saturation \r\n");
        }
        break;

        case CMD_LED_COLOR_XY:
        {
            /* Set the LED Color X Y */
            //Access - > event.eventData.colorXY.x;
            //Access - > event.eventData.colorXY.y;
            //appSnprintf("LED X,Y Color \r\n");
        }
        break;

        case CMD_BUTTON_OPEN:
        {
            /* Button Init */
            //appSnprintf("Button Init/Open \r\n");
        }
        break;

        case CMD_BUTTON_READ:
        {
            /* Button Press */
            //Access - > event.eventData.state;
            //appSnprintf("Button Read \r\n");
        }
        break;

        case CMD_SENSOR_OPEN:
        {
            /* Sensor Data */
            //appSnprintf("Sensor Open /Init Event \r\n");
        }        
        break;

        case CMD_SENSOR_READ:
        {
            /* Sensor Data */
            //Access - > event.eventData.data;
            //appSnprintf("Sensor Read Event \r\n");
        }        
        break;
		
		case CMD_BUTTON_LONG_PRESS:
        {
            /* Button long press */
        }
		break;

        default: // Added Comments in all Default case to avoid MISRA 16.4 violations
            /*default none handler */
        break;
    }
}

/*******************************************************************************
  Function:
    void Zigbee_Event_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void Zigbee_Event_Handler(APP_Zigbee_Event_t event)
{
    // User to handle all zigbee stack events  
    switch(event.eventId)
    {
        case EVENT_NETWORK_ESTABLISHED:
        {
            (void) appSnprintf("Network Established\r\n"); // adding (void) in each appSnprintf - to avoid MISRA 17.7 Violation
            LCD_PRINT(0,9,"NTW EST");
        }
        break;
        case EVENT_DISCONNECTED:
        {
           (void) appSnprintf("Disconnected from the Network\r\n");
           LCD_PRINT(0,9,"DISCONNECTED");
        }
        break;
        case EVENT_COMMISSIONING_STARTED:
        {
            (void) appSnprintf("Commissioning Procedure Started - 180 Seconds \r\n");
            LCD_PRINT(0,9,"Comm Started");
            (void) appSnprintf("Commissioning Sequence: ");
            (void) appSnprintf("Formation->");
            (void) appSnprintf("Find & Bind->");
            LCD_PRINT(0,9,"Find & Bind");
            (void) appSnprintf("\r\n");
        }
        break;

        case EVENT_COMMISSIONING_COMPLETE:
        {
            (void) appSnprintf("Commissioning Procedure Complete \r\n");
        }
        break;
        case EVENT_COMMISSIONING_FORMATION_COMPLETE:
        {
            (void) appSnprintf("Nwk Formation: ");
            if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else //BDB_COMMISSIONING_FORMATION_FAILURE
            {   (void) appSnprintf("Failed\r\n");  }
        }
        break;
        case EVENT_COMMISSIONING_STEERING_COMPLETE:
        {
            (void) appSnprintf("Steering: ");
            if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_NO_NETWORK)
            {   (void) appSnprintf("No networks found to join\r\n");   }
            else if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else
            {   (void) appSnprintf("Failed\r\n");  }
        }
        break;
        case EVENT_COMMISSIONING_TOUCHLINK_COMPLETE:
        {
            (void) appSnprintf("Touchlink: Attempt: ");
            if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_NO_SCAN_RESPONSE)
            {   (void) appSnprintf("No scan response\r\n");    }
            else if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else
            {    (void) appSnprintf("Failed\r\n"); }
        }
        break;
        case EVENT_COMMISSIONING_FINDBIND_COMPLETE:
        {
            (void) appSnprintf("Finding & Binding: ");
            if(event.eventData.value == (uint8_t) BDB_COMMISSIONING_NO_IDENTIFY_QUERY_RESPONSE)
            {   (void) appSnprintf("No identify Query Response\r\n");  }
            else if(event.eventData.value == (uint8_t) BDB_COMMISSIONING_BINDING_TABLE_FULL)
            {   (void) appSnprintf("Binding table full\r\n");  }
            else if(event.eventData.value ==(uint8_t) BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else
            {   (void) appSnprintf("Failed\r\n");  }
        }
        break;
        case EVENT_COMMISSIONING_FAILURE:
        {
          switch(event.eventData.value)
          {
              case (uint8_t)BDB_COMMISSIONING_NO_NETWORK:  // since event.eventData.value is APP_Zigbee_Event_t uint8_t variable, 
                                                           // BDB_COMMISSIONING_NO_NETWORK is anonymous enum 
                                                           // without typecast raises the MISRA rule 2012 - 10.3 violations.
              {
                  //(void) appSnprintf("No network found in search\r\n");
              }
              break;
              case (uint8_t)BDB_COMMISSIONING_NOT_SUPPORTED:
              {
                  (void) appSnprintf("Commissioning: One of the BDB commissioning procedure not supported\r\n");
              }
              break;
              case (uint8_t)BDB_COMMISSIONING_NO_SCAN_RESPONSE:
              break;
              case (uint8_t)BDB_COMMISSIONING_NO_IDENTIFY_QUERY_RESPONSE:
              break;
              default:
                /*default none handler */
              break;
          }
        }
        break;

        case EVENT_STARTED_CENTRALIZED_NETWORK:
        {
            (void) appSnprintf("Started Centralized Network\r\n");
        }
        break;
        case EVENT_STARTED_DISTRIBUTED_NETWORK:
        {
            (void) appSnprintf("Started Distributed Network\r\n");
            LCD_PRINT(0,9,"DIST NTW");
        }
        break;
        case EVENT_JOINED_TO_AN_EXISTING_NETWORK:
        {
            (void) appSnprintf("Network Search: Complete: Joined to a Network \r\n");
            LCD_PRINT(0,9,"Joined Ntw");
            (void) appSnprintf("Joined to: Address 0x%04x  MACID 0x%08x%08x ExtendedPANID 0x%08x%08x\r\n", event.eventData.ParentChildInfo.shortAddress, (uint32_t)(event.eventData.ParentChildInfo.extendedAddress >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedAddress & 0xFFFFFFFF), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId & 0xFFFFFFFF));
        }
        break;

        case EVENT_WAKEUP:
        {
            //(void) appSnprintf("Wake up Indication \r\n");
        }
        break;

        case EVENT_LEFT_FROM_NETWORK:
        {
            //(void) appSnprintf("Left from the Network \r\n");
        }
        break;

        case EVENT_CHILD_JOINED:
        {
            (void) appSnprintf("Device joined: Address 0x%04x  MACID 0x%08x%08x ExtendedPANID 0x%08x%08x\r\n", event.eventData.ParentChildInfo.shortAddress, (uint32_t)(event.eventData.ParentChildInfo.extendedAddress >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedAddress & 0xFFFFFFFF), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId & 0xFFFFFFFF));
        }
        break;

        case EVENT_CHILD_REMOVED:
        {
            (void) appSnprintf("Child Left\r\n");
        }
        break;

        case EVENT_NWK_UPDATE:
        {
            //(void) appSnprintf("Network Information updated \r\n");
        }
        break;

        case EVENT_RESET_TO_FACTORY_DEFAULTS:
        {
            //(void) appSnprintf("Reset To Factory New\r\n");
        }
        break;

        case EVENT_NWK_ADDRESS_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf( "->NwkAddrResponse, status = %d, address = %04x\r\n" ,event.eventData.ParentChildInfo.status, event.eventData.ParentChildInfo.shortAddress);}
            else
            {   (void) appSnprintf( "->NwkAddrResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);    }
        }
        break;

        case EVENT_IEEE_ADDRESS_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf("->IeeeAddrResponse, status = %d, address = 0x%08x%08x \r\n", event.eventData.ParentChildInfo.status, (uint32_t)(event.eventData.ParentChildInfo.extendedAddress >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedAddress & 0xFFFFFFFF)); }
            else
            {   (void) appSnprintf( "->IeeeAddrResponse, status = %d \r\n", event.eventData.ParentChildInfo.status);}
        }
        break;

        case EVENT_SIMPLE_DESCRIPTOR_RESPONSE:
        {
            (void) appSnprintf( "->SimpleDescResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_MATCH_DESCRIPTOR_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf( "->MatchDescResponse, status = %d, MatchedEpCount = %d\r\n" ,event.eventData.ParentChildInfo.status, event.eventData.ParentChildInfo.ep);}
            else
            {   (void) appSnprintf( "->MatchDescResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);}
        }
        break;

        case EVENT_ACTIVE_EP_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf( "->ActiveEpResponse, status = %d, EpCount = %d\r\n" ,event.eventData.ParentChildInfo.status, event.eventData.ParentChildInfo.ep);}
            else
            {   (void) appSnprintf( "->ActiveEpResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);   }
        }
        break;

        case EVENT_NODE_DESCRIPTOR_RESPONSE:
        {
            (void) appSnprintf( "->NodeDescResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_LEAVE_RESPONSE:
        {
            (void) appSnprintf( "->LeaveRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_MANAGEMENT_BIND_RESPONSE:
        {
            (void) appSnprintf( "MgmtBindRsp %d\r\n", event.eventData.ParentChildInfo.status); 
        }
        break;

        case EVENT_LQI_RESPONSE:
        {
            (void) appSnprintf( "->MgmtLqiRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_BIND_RESPONSE:
        {
            (void) appSnprintf( "->BindRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_UNBIND_RESPONSE:
        {
            (void) appSnprintf( "->UnBindRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        default:
			/*default none  */
        break;
    }
}

/*******************************************************************************
  Function:
    void Cluster_Event_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void Cluster_Event_Handler(APP_Zigbee_Event_t event)
{
    switch(event.eventId)
    {
        char printData[30];
        case CMD_ZCL_RESET_TO_FACTORY_DEFAULTS:
        {
            /* Command ZCL ResetToFactoryDefaults */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("ZCL ResetToFactoryDefaults\r\n");
        }
        break;
        case CMD_ZCL_IDENTIFY:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf( "->Identify\r\n");
        }
        break;
        case CMD_ZCL_IDENTIFY_QUERY:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("->IdentifyQuery\r\n");
        }
        break;
        case CMD_ZCL_TRIGGER_EFFECT:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_TriggerEffect_t *payload = (ZCL_TriggerEffect_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("->TriggerEffect 0x%x\r\n",  payload->effectIdentifier);
        }
        break;
        case CMD_ZCL_IDENTIFY_QUERY_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("->IdentifyQueryResponse, addr = 0x%04x, timeout = 0x%04x\r\n", addressing->addr.shortAddress, payload->timeout);
        }
        break;
        case CMD_ZCL_ADD_GROUP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_AddGroup_t *payload = (ZCL_AddGroup_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("addGroupInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_VIEW_GROUP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_ViewGroup_t *payload = (ZCL_ViewGroup_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("viewGroupInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_GET_GROUP_MEMBERSHIP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload; 
            (void) appSnprintf("getGroupMembershipInd()\r\n");
        }
        break;
        case CMD_ZCL_REMOVE_GROUP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_RemoveGroup_t *payload = (ZCL_RemoveGroup_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("removeGroupInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_REMOVE_ALL_GROUP:
        {
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
            (void) appSnprintf( "removeAllGroupsInd()\r\n");
        }
        break;
        case CMD_ZCL_ADD_GROUP_IF_IDENTIFYING:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_AddGroupIfIdentifying_t *payload = (ZCL_AddGroupIfIdentifying_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("addGroupIfIdentifyingInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_ADD_GROUP_RESPONSE:
        {
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
            (void) appSnprintf("addGroupResponseInd()\r\n");
        }
        break;
        case CMD_ZCL_VIEW_GROUP_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_ViewGroupResponse_t *payload = (ZCL_ViewGroupResponse_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("viewGroupResponse(): status = 0x%02x\r\n", payload->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_GET_GROUP_MEMBERSHIP_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_GetGroupMembershipResponse_t *payload = (ZCL_GetGroupMembershipResponse_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("getGroupMembershipResponse()\r\n");
            (void) appSnprintf("groupCount = %d\r\n", payload->groupCount);
            for (uint8_t i = 0; i < payload->groupCount; i++)
            {  (void) appSnprintf("groupId = 0x%04x\r\n", payload->groupList[i]);}
        }
        break;
        case CMD_ZCL_REMOVE_GROUP_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_RemoveGroupResponse_t *payload = (ZCL_RemoveGroupResponse_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("removeGroupResponseInd()\r\n");
            (void) appSnprintf("groupId = 0x%04x\r\n", payload->groupId);

        }
        break;
        case CMD_ZCL_ALARM:
        {
            /* Command ZCL OnOffAttributeEventInd */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_Alarm_t *payload = (ZCL_Alarm_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("<-Alarms alarmCode = %d clusterId = 0x%x\r\n", payload->alarmCode, payload->clusterIdentifier);
        }
        break;
        case CMD_ZCL_GET_ALARM_RESPONSE:
        {
            /* Command ZCL OnOffAttributeEventInd */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload; 
            ZCL_getAlarmResponse_t *payload = (ZCL_getAlarmResponse_t *)event.eventData.zclEventData.payload; 
            if ( BC_SUCCESS == payload->status)
            {
                (void) appSnprintf("<-GetAlarmResponse status = 0x%x clusterId = 0x%x alarmCode = 0x%x\r\n", payload->status, payload->clusterIdentifier, payload->alarmCode);
            }
            else
            {
                (void) appSnprintf("<-GetAlarmResponse failed status = 0x%x\r\n", payload->status);
            }
        }
        break;
        case CMD_ZCL_ADD_SCENE_RESP:
        {
            /* ZCL Command Add scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_AddSceneResponse_t *resp = ((ZCL_AddSceneResponse_t*)event.eventData.zclEventData.payload);
            (void) appSnprintf("Add scene response: status = 0x%02x\r\n", resp->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", resp->groupId);
            (void) appSnprintf("sceneId = 0x%02x\r\n", resp->sceneId);
        }
        break;
        case CMD_ZCL_VIEW_SCENE_RESP:
        {
            /* ZCL Command View scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_ViewSceneResponse_t *resp = ((ZCL_ViewSceneResponse_t*)event.eventData.zclEventData.payload);
            (void) appSnprintf("View scene response: status = 0x%02x\r\n", resp->status);
            (void) appSnprintf( "groupId = 0x%04x\r\n", resp->groupId);
            (void) appSnprintf("sceneId = 0x%02x\r\n", resp->sceneId);
            (void) appSnprintf("transitionTime = 0x%04x\r\n", resp->transitionTime);
        }
        break;
        case CMD_ZCL_REMOVE_SCENE_RESP:
        {
            /* ZCL Command remove scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_RemoveSceneResponse_t *resp = ((ZCL_RemoveSceneResponse_t*)event.eventData.zclEventData.payload);
            (void) appSnprintf("Remove scene response: status = 0x%02x\r\n",resp->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", resp->groupId);
            (void) appSnprintf("sceneId = 0x%02x\r\n", resp->sceneId);
        }
        break;
        case CMD_ZCL_REMOVE_ALL_SCENES_RESP:
        {
            /* ZCL Command remove all scenes response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_RemoveAllScenesResponse_t *resp = ((ZCL_RemoveAllScenesResponse_t*)event.eventData.zclEventData.payload);
            (void) appSnprintf("Remove all scenes response: status = 0x%02x\r\n", resp->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", resp->groupId);
        }
        break;
        case CMD_ZCL_STORE_SCENE_RESP:
        {
            /* ZCL Command store scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_StoreSceneResponse_t *resp = ((ZCL_StoreSceneResponse_t*)event.eventData.zclEventData.payload);
            (void) appSnprintf("Store scene response: status = 0x%02x\r\n",resp->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", resp->groupId);
            (void) appSnprintf("sceneId = 0x%02x\r\n", resp->sceneId);
        }
        break;
        case CMD_ZCL_GET_SCENE_MEMBERSHIP_RESP:
        {
            /* ZCL Command get scene membership response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_GetSceneMembershipResponse_t *resp = ((ZCL_GetSceneMembershipResponse_t*)event.eventData.zclEventData.payload);
            (void) appSnprintf("Get scene membership response: status = 0x%02x\r\n", resp->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", resp->groupId);
            (void) appSnprintf("sceneCount = 0x%02x\r\n", resp->sceneCount);
            for (uint8_t i = 0; i < resp->sceneCount; i++)
              (void) appSnprintf("sceneId = 0x%02x\r\n", resp->sceneList[i]);
        }
        break;
        case CMD_ZCL_ENHANCED_ADD_SCENE_RESP:
        {
            /* ZCL Command enhanced Add scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_EnhancedAddSceneResponse_t *resp = (ZCL_EnhancedAddSceneResponse_t *)(event.eventData.zclEventData.payload);
            (void) appSnprintf("Enhanced add scene response: status = 0x%02x\r\n", resp->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", resp->groupId);
            (void) appSnprintf("sceneId = 0x%02x\r\n", resp->sceneId);
        }
        break;
        case CMD_ZCL_ENHANCED_VIEW_SCENE_RESP:
        {
            /* ZCL Command enhanced view scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_EnhancedViewSceneResponse_t *resp = ((ZCL_EnhancedViewSceneResponse_t *)(event.eventData.zclEventData.payload));
            (void) appSnprintf("Enhanced view scene response: status = 0x%02x\r\n", resp->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", resp->groupId);
            (void) appSnprintf("sceneId = 0x%02x\r\n", resp->sceneId);
            (void) appSnprintf("transitionTime = 0x%04x\r\n", resp->transitionTime);
        }
        break;
        case CMD_ZCL_COPY_SCENE_RESP:
        {
            /* ZCL Command copy scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_CopySceneResponse_t *resp = ((ZCL_CopySceneResponse_t*)event.eventData.zclEventData.payload);
            (void) appSnprintf("Copy scene response: status = 0x%02x\r\n", resp->status);
        }
        break;
        case CMD_ZCL_ACE_GET_PANEL_STATUS:
        {
            /* ZCL Command copy scene response received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf(" ->ACEGetPanelStatusCommandInd Received\r\n");
        }
        break;
        case CMD_ZCL_ACE_ARM:
        {
            /* ZCL command IAS ACE arm received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("aceArmCommandInd()\r\n");
        }
        break;
        case CMD_ZCL_ACE_GET_ZONE_INFO:
        {
            /* ZCL ace get zone information received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf("aceGetZoneInformationCommandInd()\r\n");
        }
        break;
        case CMD_ZCL_ACE_GET_ZONE_ID_MAP:
        {
            /* ZCL IAS ACE get zone ID received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf("aceGetZoneIdMapCommandInd()\r\n");
        }
        break;
        case CMD_ZCL_ACE_BYPASS:
        {
            /* ZCL IAS ACE bypass command received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf("aceByPassCommandInd()\r\n");
        }
        break;
        case CMD_ZCL_ACE_GET_BYPASSED_ZONE_LIST:
        {
            /* ZCL IAS ACE get bypassed zone list command received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf("aceGetByPassedZoneListCommandInd()\r\n");
        }
        break;
        case CMD_ZCL_ACE_EMERGENCY:
        {
            /* ZCL IAS ACE emergency command received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf(" ->ACE Emergency Command Received\r\n");
        }
        break;
        case CMD_ZCL_ACE_FIRE:
        {
            /* ZCL IAS ACE fire command received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf(" ->ACE Fire Command Received\r\n");
        }
        break;
        case CMD_ZCL_ACE_PANIC:
        {
            /* ZCL Command IAS ACE PANIC received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf(" ->ACE Panic Command Received\r\n");
        }
        break;
        case CMD_ZCL_ZONE_STATUS_CHANGE_NOTIFY:
        {
            /* ZCL Command zone status change notification received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf(" <-Zone Status Change Notification Received\r\n");
        }
        break;
        case CMD_ZCL_ZONE_ENROLL_REQ:
        {
            /* ZCL Command ZOne enroll request received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("zoneEnrollRequestCommandInd()\r\n");
        }
        break;
        case CMD_ZCL_ATTR_COLOR_CONTROL:
        {
            /* Command ZCL ColorControlAttributeEventInd */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
            //(void) appSnprintf("ZCL ColorControlAttributeEventInd\r\n");
           if(ZCL_CONFIGURE_DEFAULT_ATTRIBUTE_REPORTING_EVENT == event.eventData.zclAttributeData.event)
           {
            switch(event.eventData.zclAttributeData.attributeId)
            {
            default:
                /*default none  */
            break;
        }
           }
        }
        break;
        case CMD_ZCL_ATTR_IDENTIFY:
        {
            /* Command ZCL IdentifyAttributeEventInd */ 
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
            //(void) appSnprintf("ZCL IdentifyAttributeEventInd\r\n");
        }
        break;
        case CMD_ZCL_ATTR_LEVEL_CONTROL:
        {
            /* Command ZCL LevelControlAttributeEventInd */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
        }
        break;
        case CMD_ZCL_ATTR_ONOFF:
        {
            /* Command ZCL OnOffAttributeEventInd */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event; 
        }
        break;
        case CMD_ZCL_ATTR_OCCUPANCY:
        {
            /* Command ZCL OccupancyAttributeEventInd */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
        }
        break;
        case CMD_ZCL_ATTR_ILLUMINANCE_MEASUREMENT:
        {
            /* Command ZCL OnOffAttributeEventInd */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
        }
        break;
        case CMD_ZCL_ATTR_HUMIDITY_MEASUREMENT:
        {
            /* Command ZCL CustomHumidityMeasurementReportInd Prints Report Value */
            //Access - > event.zclAttributeData.zclEventData.addressing;
            //Access - > event.zclAttributeData.zclEventData.attributeId;
            //Access - > event.zclAttributeData.zclEventData.event;           
            //(void) appSnprintf("ZCL HS Humidity Measurement Attr Ind\r\n");
        }
        break;
        case CMD_ZCL_ATTR_TIME:
        {
            /* Command ZCL Time attribute indication */         
            if(event.eventData.zclAttributeData.attributeId == ZCL_TIME_CLUSTER_SERVER_TIME_ATTRIBUTE_ID && 
               event.eventData.zclAttributeData.event == ZCL_WRITE_ATTRIBUTE_EVENT)
            {
	          (void) appSnprintf("Standard and local time updated by client\r\n");
            }
        }
        break;
        case CMD_ZCL_ATTR_THERMOSTAT_UI_CONF:
        {
        }
        break;
        case CMD_ZCL_ATTR_THERMOSTAT:
        {
          /* Command ZCL Thermostat Attribute Indication */
          //Access - > eventItem.eventData.zclAttributeData.addressing;
          //Access - > eventItem.eventData.zclAttributeData.attributeId;
          //Access - > eventItem.eventData.zclAttributeData.event;  
        }
        break;
        case CMD_ZCL_ATTR_TEMPERATURE_MEASUREMENT:
        {
            /* Command ZCL Temperature measurement Attribute Indication */
            //Access - > eventItem.eventData.zclAttributeData.addressing;
            //Access - > eventItem.eventData.zclAttributeData.attributeId;
            //Access - > eventItem.eventData.zclAttributeData.event;
        }
        break;
        case CMD_ZCL_ATTR_FANCONTROL:
        {   
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;   		
            //(void) appSnprintf("<-Attr ID 0x%x event 0x%x\r\n",event.eventData.zclAttributeData.attributeId, event.eventData.zclAttributeData.event);
        }
        break;
        case CMD_ZCL_ATTR_IASZONE:
        {
            /* command zcl IASACE attribute indication */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;               
            //(void) appSnprintf("<-Attr ID 0x%x event 0x%x\r\n",event.eventData.zclAttributeData.attributeId, event.eventData.zclAttributeData.event);
        }
        break;
        case CMD_ZCL_REPORTING_ONOFF:
        {
            /* Command ZCL OnOffReportInd */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.payloadLength;
            //Access - > event.eventData.zclAttributeData.payload;
            ZCL_Report_t *rep = (ZCL_Report_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("<-On/Off Attr Report: Value = 0x%x\r\n", (int)rep->value[0]);
        }
        break;
        case CMD_ZCL_REPORTING_LIGHT_SENSOR:
        {
            /* Command ZCL LightSensorReportInd */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.payloadLength;
            //Access - > event.eventData.zclAttributeData.payload;
            ZCL_Report_t *rep = (ZCL_Report_t *)event.eventData.zclEventData.payload;;
            uint16_t reportValue;
            memcpy(&reportValue, &rep->value[0], sizeof(uint16_t));
            (void) appSnprintf("<-Light Sensor Attr Report: Value = 0x%x\r\n", reportValue);
        }
        break;
        case CMD_ZCL_REPORTING_LEVEL:
        {
            /* Command ZCL LevelReportInd( */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.payloadLength;
            //Access - > event.eventData.zclAttributeData.payload;
            ZCL_Report_t *rep = (ZCL_Report_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("<-Level Control Attr Report: Value = 0x%x\r\n", (int)rep->value[0]);
        }
        break;
        case CMD_ZCL_REPORTING_COLOR_CONTROL:
        {
            /* ZCL Command ZCL STOP MOVE SET received */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            do  //Possibilities of getting more than one report at a time.
            {
                ZCL_Report_t *rep = (ZCL_Report_t *)(event.eventData.zclEventData.payload);
                uint16_t reportValue = 0;
                if(rep->type == ZCL_U8BIT_DATA_TYPE_ID)
                {
                    memcpy(&reportValue, (event.eventData.zclEventData.payload+ sizeof(ZCL_AttributeId_t)+ sizeof(rep->type)), sizeof(uint8_t));
                }
                else
                {
                    memcpy(&reportValue, (event.eventData.zclEventData.payload+ sizeof(ZCL_AttributeId_t)+ sizeof(rep->type)), sizeof(uint16_t));
                }
                (void) appSnprintf("<-Color Control Attr (0x%x) Report: Value = 0x%x\r\n", (unsigned)rep->id, reportValue);
                
                if((event.eventData.zclEventData.payloadLength >= 4))
                {
                    if(rep->type == ZCL_U8BIT_DATA_TYPE_ID)//Attribute data is of 1 byte
                    {
                        event.eventData.zclEventData.payloadLength = event.eventData.zclEventData.payloadLength - sizeof(uint32_t);  // Skip the first report 
                        event.eventData.zclEventData.payload = (event.eventData.zclEventData.payload)+ sizeof(uint32_t);             // Align the pointer to read next        
                    }
                    else // Attribute data is of 2 byte
                    {
                        event.eventData.zclEventData.payloadLength = event.eventData.zclEventData.payloadLength - sizeof(uint32_t) - sizeof(uint8_t); // Skip the first report 
                        event.eventData.zclEventData.payload = (event.eventData.zclEventData.payload) + sizeof(uint32_t) + sizeof(uint8_t);          // Align the pointer to read next            
                    }
               }
            }while(event.eventData.zclEventData.payloadLength);   
        }
        break;
        case CMD_ZCL_REPORTING_OCCUPANCY:
        {
            /* Command ZCL LevelReportInd( */
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.payloadLength;
            //Access - > event.eventData.zclAttributeData.payload;
            ZCL_Report_t *rep = (ZCL_Report_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("<-Occupancy Sensor Attr Report: Value = 0x%x\r\n", (int)rep->value[0]);
        }
        break;
        case CMD_ZCL_REPORTING_THERMOSTAT:
        {
            /* Command ZCL Thermostat Report Indication */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;                
            //(void) appSnprintf("ZCL Report TH Thermostat Ind\r\n");
            ZCL_Report_t *rep = (ZCL_Report_t *)event.eventData.zclEventData.payload;
            int16_t reportValue = 0;
            if(rep->id == ZCL_THERMOSTAT_CLUSTER_LOCAL_TEMPERATURE_SERVER_ATTRIBUTE_ID || (rep->id == ZCL_THERMOSTAT_CLUSTER_PI_COOLING_DEMAND_SERVER_ATTRIBUTE_ID)
            || (rep->id == ZCL_THERMOSTAT_CLUSTER_PI_HEATING_DEMAND_SERVER_ATTRIBUTE_ID))
            {
              if(rep->id == ZCL_THERMOSTAT_CLUSTER_LOCAL_TEMPERATURE_SERVER_ATTRIBUTE_ID)
              {
                memcpy(&reportValue, &rep->value[0], sizeof(int16_t));
                (void) appSnprintf("<-Thermostat Attr (0x%x) Report: Value = %d.%dC\r\n", rep->id, (int)(reportValue/THERMOSTAT_LOCAL_TEMPERATURE_SCALE),(int)(reportValue%THERMOSTAT_LOCAL_TEMPERATURE_SCALE));
              }
              else
              {
                memcpy(&reportValue, &rep->value[0], sizeof(uint8_t));
                (void) appSnprintf("<-Thermostat Attr (0x%x) Report: Value = 0x%x\r\n", rep->id, (uint8_t)reportValue);
              }
            }
            else if(rep->id == ZCL_THERMOSTAT_CLUSTER_OCCUPIED_COOLING_SETPOINT_SERVER_ATTRIBUTE_ID)
            {
              setPoint_t *payload = (setPoint_t*)rep;
    
              (void) appSnprintf("<-Setpoints changed on Thermostat:\r\n");
              memcpy(&reportValue, &rep->value[1], sizeof(int16_t));

              (void) appSnprintf("#cool setpoint: Value = %d.%d\r\n", (int)(reportValue/THERMOSTAT_LOCAL_TEMPERATURE_SCALE),(int)(reportValue%THERMOSTAT_LOCAL_TEMPERATURE_SCALE));
              payload++;

              memcpy(&reportValue, &payload->value, sizeof(int16_t));
              (void) appSnprintf("#heat setpoint: Value = %d.%d\r\n", (int)(reportValue/THERMOSTAT_LOCAL_TEMPERATURE_SCALE),(int)(reportValue%THERMOSTAT_LOCAL_TEMPERATURE_SCALE));
            }
        }
        break;
        case CMD_ZCL_REPORTING_TEMPERATURE_MEASUREMENT:
        {
            /* Command ZCL Thermostat Report Indication */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;              
            ZCL_Report_t *rep = (ZCL_Report_t *)event.eventData.zclEventData.payload;
            int16_t reportValue;
            memcpy(&reportValue, &rep->value[0], sizeof(int16_t));
            (void) appSnprintf( "<-Temperature Measurement Attr Report: Value = %d   0x%02hx\r\n", reportValue ,reportValue);
            temp_update(reportValue);                                 
        }
        break;
        case CMD_ZCL_REPORTING_HUMIDITY_MEASUREMENT:
        {
            /* Command ZCL Humidity Measurement Report Indication */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;              
            ZCL_Report_t *rep = (ZCL_Report_t *)event.eventData.zclEventData.payload;
            uint16_t reportValue;
            memcpy(&reportValue, &rep->value[0], sizeof(uint16_t));
            (void) appSnprintf( "<-Relative Humidity Measurement Attr Report: Value = 0x%x\r\n", reportValue);
            hum_update(reportValue);
        }
        break;
        
        //Custom Clusters       



        default:
           //Default no process 
        break;
    }
}