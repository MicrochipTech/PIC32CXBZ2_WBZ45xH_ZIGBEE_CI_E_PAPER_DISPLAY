/*******************************************************************************
  MAC HWI HwdReq Memory Pool Header File

  Company:
    Microchip Technology Inc.

  File Name:
    machwiHwdReqMemPool.h

  Summary:
    This file defines the prototype to access MAC HWI memory Buffers.

  Description:
    This file contains the Types and constants declaration for machwi memory management for 
    machwd requests.
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

#ifndef MACHWIHWDREQMEMPOOL_H
#define MACHWIHWDREQMEMPOOL_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <systemenvironment/include/sysTypes.h>
#include <mac_phy/mac_hwd_phy/include/machwd.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
/******************************************************************************
  Union of possible request types for the requests to machwd module.
******************************************************************************/
typedef union
{
  MACHWD_DataReq_t        data;
  MACHWD_SetTrxStateReq_t setTrxState;
  MACHWD_SetReq_t         set;
  MACHWD_EdReq_t          ed;
} MachwiHwdReqDescr_t;

#endif //MACHWIHWDREQMEMPOOL_H

// eof machwiHwdReqMemPool.h
