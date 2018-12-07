//Please don't use the file with this filename but paste it into Smart_Meter_PSoC6_Webserver.h because Smart_Meter_PSoC6_Webserver.mk has that name listed as a source
/*****************************************************************************
* File Name: CE222494_PSoC6_WICED_WiFi.h
*
* Description: This file contains the function prototypes and constants used in
*  CE222494_PSoC6_WICED_WiFi.c. This driver is intended for AK4954A.
*
******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/

#include <math.h>
#include "cy_tft_display.h"
#include "wiced.h"
#include "http_server.h"
#include "sntp.h"
#include "gedday.h"
#include "resources.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define MAX_SOCKETS                     (10u)                       /* Maximum number of sockets */
#define TOP_DISPLAY                     (0u)                        /* Top of the display*/
#define EXTRA_FONT_SPACE                (3u)                        /* Spacing between printed lines*/
#define TEXT_FONT                       (FONT_8X12)                 /* Font for display text*/
#define HEADING_FONT                    (FONT_8X14)                 /* Font for heading text */
/******************************************************
 *                   Enumerations
 ******************************************************/


/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    int                     usage;
    int                     voltage;
    wiced_iso8601_time_t    timestamp;
} usage_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
wiced_result_t  calculateUsage         ( void* arg );
int32_t         processUsageUpdate       ( const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data );

/** @endcond */
#ifdef __cplusplus
} /*extern "C" */
#endif
