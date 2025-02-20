/*******************************************************************************
  HAL MAC Isr Source File

  Company:
    Microchip Technology Inc.

  File Name:
    halMacIsr.c

  Summary:
    This file contains the Implementation of hal timer specifically used by
    the MAC layer for its functionality.

  Description:
    This file provides the interface to access the HAL timer API's required by the BitCloud MAC
    layer.
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


/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                        Includes section.
******************************************************************************/
#include "device.h"
#include <hal/cortexm4/pic32cx/include/halRfCtrl.h>
#include <hal/cortexm4/pic32cx/include/halMacIsr.h>
#if defined(_STATS_ENABLED_)
#include <statStack.h>
#endif
#include <configserver/include/configserver.h>
#include <systemenvironment/include/sysAssert.h>
#include <hal/cortexm4/pic32cx/include/halRfCtrl.h>
#include <mac_phy/mac_hwd_phy/RF231_RF212/PHY/include/phyRtimerIrqDispatcher.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define HAL_NUMBER_OF_TICKS_IN_USEC     (APB_CLK_HZ/RTIMER_FREQUENCY_PRESCALER/1000000UL)

/******************************************************************************
                   Define(s) section
******************************************************************************/
 
// Re defining TC0 Registers for halMacISR.c file 
 #define TC0_SYNCBUSY	TC0_REGS->COUNT16.TC_SYNCBUSY 				//TC_SYNCBUSY
 #define TC0_CTRLA 		TC0_REGS->COUNT16.TC_CTRLA					//TC_CTRLA
 #define TC0_16COUNT	TC0_REGS->COUNT16.TC_COUNT					//TC_COUNT
 #define TC0_CTRLBSET	TC0_REGS->COUNT16.TC_CTRLBSET				//TC_CTRLBSET
 #define TC0_CTRLBCLR	TC0_REGS->COUNT16.TC_CTRLBCLR				//TC_CTRLBCLR
 #define TC0_CC0		TC0_REGS->COUNT16.TC_CC[0]					//TC_CC[0]
 #define TC0_CC1		TC0_REGS->COUNT16.TC_CC[1]					//TC_CC[1]
 #define TC0_INTENSET	TC0_REGS->COUNT16.TC_INTENSET				//TC_INTENSET
 #define TC0_INTENCLR	TC0_REGS->COUNT16.TC_INTENCLR				//TC_INTENCLR
 #define TC0_INTFLAG	TC0_REGS->COUNT16.TC_INTFLAG				//TC_INTFLAG
 #define TC0_WAVE 		TC0_REGS->COUNT16.TC_WAVE					//TC_WAVE
 #define TC0_STATUS		TC0_REGS->COUNT16.TC_STATUS					//TC_STATUS



/******************************************************************************
                        External variables.
******************************************************************************/
/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
                   Global variables section
******************************************************************************/
static RTimerDescr_t rtimer;

static bool rTimerIntervalComplete = true;
static uint16_t nextCycleCnt;
void setRTimerNextCycleCnt(uint16_t remCnt);
void rTimerLoadCnt(HAL_RTimerMode_t mode, uint16_t per);
/******************************************************************************
                    Prototypes section
******************************************************************************/

/******************************************************************************
  MAC timer handler.
******************************************************************************/

/******************************************************************************
                    Implementations section
******************************************************************************/
/******************************************************************************
  Initializes Rtimer.
******************************************************************************/
void ZB_HAL_InitMacIsr(void)
{

  rtimer.mode = HAL_RTIMER_STOPPED_MODE;

  /* Configure the generic clk settings for enabling TC4 and TC5 */
  //GCLK_CLKCTRL = GCLK_CLKCTRL_ID((uint32_t)GCLK_TCC2_GCLK_TC5) | GCLK_CLKCTRL_GEN(GCLK_GEN_3) | GCLK_CLKCTRL_CLKEN;
    //GCLK_PCHCTRL(TCC2_GCLK_ID) = GCLK_PCHCTRL_GEN(GCLK_GEN_3) | GCLK_PCHCTRL_CHEN;

}
/******************************************************************************
 Sets the Sequential CNT variable to be loaded in CC1 when the
 totalCNT > TOP_TIMER_COUNTER_VALUE
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
void setRTimerNextCycleCnt(uint16_t remCnt)
{
  uint16_t tcCnt = 0;
  TC0_CTRLBSET |= TC_CTRLBCLR_CMD_READSYNC;
  while((TC0_SYNCBUSY & TC_SYNCBUSY_CTRLB_Msk) == TC_SYNCBUSY_CTRLB_Msk)
  {
    /* Wait for Write Synchronization */
  }
  tcCnt = TC0_16COUNT;
	
  if(remCnt <tcCnt)
  {
    rTimerIntervalComplete = true;
    phyDispatcheRTimerEvent();       
    return;
  }
 uint16_t loadCnt = tcCnt + remCnt;
 if(loadCnt > TOP_TIMER_COUNTER_VALUE)
 {
   TC0_CC1 = TC_COUNT16_CC_CC(TOP_TIMER_COUNTER_VALUE);
   while ((TC0_SYNCBUSY & TC_SYNCBUSY_CC1_Msk) == TC_SYNCBUSY_CC1_Msk)
   {
     /* Wait for Write Synchronization */
   }
   loadCnt = (uint16_t)(loadCnt - TOP_TIMER_COUNTER_VALUE);
   nextCycleCnt = loadCnt;
 }
 else
 {
   rTimerIntervalComplete = true;
   TC0_CC1 = TC_COUNT16_CC_CC(loadCnt);
   while ((TC0_SYNCBUSY & TC_SYNCBUSY_CC1_Msk) == TC_SYNCBUSY_CC1_Msk)
   {
     /* Wait for Write Synchronization */
   }
 }
  TC0_INTENSET=TC_INTENSET_MC1(1);
  TC0_INTFLAG=TC_INTFLAG_MC1(1);
}

void rTimerLoadCnt(HAL_RTimerMode_t mode, uint16_t per)
{
  rtimer.period    = (uint16_t)((uint32_t)per * HAL_NUMBER_OF_TICKS_IN_USEC);
  rtimer.mode      = mode;
  TC0_CTRLBSET |= TC_CTRLBCLR_CMD_READSYNC;
  while ((TC0_SYNCBUSY & TC_SYNCBUSY_CTRLB_Msk) == TC_SYNCBUSY_CTRLB_Msk)
  {
     /* Wait for Write Synchronization */
  }

  uint16_t tcCount = TC0_16COUNT;
  uint32_t totalCC1 = (uint16_t)(tcCount + rtimer.period);
  if(totalCC1 > TOP_TIMER_COUNTER_VALUE)
   {
    rTimerIntervalComplete = false;
    uint16_t firstCycleCC1 = (uint16_t)(TOP_TIMER_COUNTER_VALUE);
    TC0_CC1 = TC_COUNT16_CC_CC(firstCycleCC1);
    while ((TC0_SYNCBUSY & TC_SYNCBUSY_CC1_Msk) == TC_SYNCBUSY_CC1_Msk)
    {
      /* Wait for Write Synchronization */
    }
    uint16_t countInFirstCycle = (uint16_t)(TOP_TIMER_COUNTER_VALUE - tcCount);
    nextCycleCnt = (rtimer.period - countInFirstCycle);
	
  }
  else
  {
    TC0_CC1 = TC_COUNT16_CC_CC(totalCC1);
    while ((TC0_SYNCBUSY & TC_SYNCBUSY_CC1_Msk) == TC_SYNCBUSY_CC1_Msk)
    {
      /* Wait for Write Synchronization */
    }
  }
}
/******************************************************************************
  Starts RTimer. Function should be invoked in critical section.
  Parameters:
    mode    - RTimer mode.
    period  - RTimer period.
******************************************************************************/
bool HAL_StartRtimer(HAL_RTimerMode_t mode, uint16_t period)
{
  
  if (HAL_RTIMER_STOPPED_MODE != rtimer.mode)
  {
    return false;
  }
  rTimerLoadCnt(mode, period);
  TC0_INTENSET = TC_INTENSET_MC1(1);
  TC0_INTFLAG = TC_INTFLAG_MC1(1);
  return true;
}

/******************************************************************************
  Stops RTimer. Function should be invoked in critical section.
******************************************************************************/
void HAL_StopRtimer(void)
{
  TC0_INTENCLR = TC_INTENSET_MC1(1);  // disabling interrupt MC0

  rtimer.mode = HAL_RTIMER_STOPPED_MODE;
}

/******************************************************************************
  Returns status of RTimer
******************************************************************************/
bool HAL_IsRtimerStopped(void)
{
  if (rtimer.mode == HAL_RTIMER_STOPPED_MODE)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/******************************************************************************
  MAC timer handler.
******************************************************************************/

void halMacTimerHandler(void)
{
#if defined(_STATS_ENABLED_)
  uint16_t stack_threshold;
  uint16_t stack_left = Stat_GetCurrentStackLeft();
  CS_ReadParameter(CS_STACK_LEFT_THRESHOLD_ID,&stack_threshold);
  SYS_E_ASSERT_FATAL((stack_left>stack_threshold),MAC_TIMER_ISR_STACK_OVERFLOW);
#endif //(_STATS_ENABLED_)

    if (HAL_RTIMER_ONE_SHOT_MODE == rtimer.mode)
    {
     rtimer.mode = HAL_RTIMER_STOPPED_MODE;
    }
    TC0_INTENCLR = TC_INTENSET_MC1(1); // disabling interrupt MC1
		

    if(rTimerIntervalComplete)
    {
      phyDispatcheRTimerEvent();
    }
    else
    {
      setRTimerNextCycleCnt(nextCycleCnt);
    }
    if (HAL_RTIMER_REPEAT_MODE == rtimer.mode)
    {
      rTimerLoadCnt(HAL_RTIMER_REPEAT_MODE, rtimer.period);
      TC0_INTENSET = TC_INTENSET_MC1(1);
      TC0_INTFLAG = TC_INTFLAG_MC1(1);
    }
}
// eof halMacIsr.c

