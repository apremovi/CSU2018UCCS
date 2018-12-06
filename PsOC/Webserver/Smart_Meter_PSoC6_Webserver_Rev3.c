//Please don't use the file with this filename but paste it into Smart_Meter_PSoC6_Webserver.c because Smart_Meter_PSoC6_Webserver.mk has that name listed as a source
/******************************************************************************
* File Name: cy8ckit_062_demo.c
*
* Version 1.0
*
* This application demonstrates a PSoC 6 device hosting an http webserver
* The PSoC 6 measures the voltage of the ambient light sensor on the CY8CKIT-028
* It then displays that information on the webserver.
* The PSoC 6 also controls the brightness of the RED led on the board. The brightness
* can be controlled by the two capsense buttons, capsense slider, or webpage.
*
* Related Document: CE_Title.pdf
*
* Hardware Dependency: CY8CKIT_062_WiFi with CY8CKIT_028
*
*******************************************************************************
* Copyright (2017), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries () and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ().
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypresss integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death (High Risk Product). By
* including Cypress’s product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
* *******************************************************************************/


#include "Smart_Meter_PSoC6_Webserver.h"
#include <string.h>

/******************************************************
 *               Global Variable Definitions
 ******************************************************/
/* Holds usage data and time stamp*/
static usage_t      usageData;

/* Variables for server*/
static wiced_http_server_t httpServer;

/* ugui instance*/
UG_GUI   gui;

/*Global to hold display row position*/
uint16_t row = TOP_DISPLAY;

/*Flag to indicate is screen is ready for printing usage data*/
uint8_t screenReady = 0;

/*Pointers to webpages*/
static START_OF_HTTP_PAGE_DATABASE(web_pages)
    ROOT_HTTP_PAGE_REDIRECT("/apps/cy8ckit_062_demo/main.html"),
    { "/apps/cy8ckit_062_demo/main.html","text/html",                WICED_RESOURCE_URL_CONTENT,  .url_content.resource_data  = &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_main_html, },
    { "/temp_report.html",               "text/html",                WICED_DYNAMIC_URL_CONTENT,   .url_content.dynamic_data   = {processUsageUpdate, 0 }, },
    { "/images/favicon.ico",             "image/vnd.microsoft.icon", WICED_RESOURCE_URL_CONTENT,  .url_content.resource_data  = &resources_images_DIR_favicon_ico, },
    { "/scripts/general_ajax_script.js", "application/javascript",   WICED_RESOURCE_URL_CONTENT,  .url_content.resource_data  = &resources_scripts_DIR_general_ajax_script_js, },
    { "/styles/buttons.css",             "text/css",                 WICED_RESOURCE_URL_CONTENT,  .url_content.resource_data  = &resources_styles_DIR_buttons_css, },
END_OF_HTTP_PAGE_DATABASE();

wiced_uart_config_t uart_config =
{
    .baud_rate    = 9600,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};


wiced_ring_buffer_t rx_buffer;
uint8_t             rx_data[64];

void application_start( void )
{
    /*Maximum socket connections*/
    uint16_t                maxSockets = MAX_SOCKETS;

    /*Semaphore used for sleeping application thread*/
    wiced_semaphore_t       semaphore;

    /*Hold results from WICED API*/
    wiced_result_t          result;

    /*Indicates if there is data in DCT or not*/
    wiced_bool_t*           device_configured;
    /*Location for device_configured to point to when clearing DCT*/
    wiced_bool_t            clearDCT = 0;

    /*IP address for device when connected to another network*/
    wiced_ip_address_t      ipv4Address;
    uint32_t                ipAddress;
    char                    buffer[50];
    /*UART STUFF*/
    ring_buffer_init(&rx_buffer, rx_data, 64 );
    wiced_uart_init( STDIO_UART, &uart_config, &rx_buffer );
    /* information on configuration access point*/
    wiced_config_soft_ap_t*  configAp;

    /* Initialize the device and WICED framework */
    wiced_init( );

    /*Initialize the semaphore used to sleep the application thread*/
    wiced_rtos_init_semaphore( &semaphore );

    /*Initialize the TFT LCD*/
    Cy_TFT_Init();
    /*Connect ugui display driver*/
    UG_Init( &gui, Cy_TFT_displayDriver, 320, 240 );

    /*Setup display for this demo*/
    /*Fill screen with black*/
    UG_FillScreen( C_BLACK );
    UG_SetBackcolor( C_BLACK );
    UG_SetForecolor( C_WHITE );
    UG_FontSelect( &HEADING_FONT );

    /*Print welcome message*/
    UG_PutString( 10, row,  "CSU Meter Project");
    /*Increment row for next line*/
    row = (HEADING_FONT.char_height + EXTRA_FONT_SPACE);

    /*Check if there is a network stored in DCT, if not display instructions*/
    wiced_dct_read_lock( (void**) &device_configured, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t, device_configured), sizeof(wiced_bool_t) );

    if ( *device_configured != WICED_TRUE)
    {

        /*Read out details of the configuration AP, for printing out*/
        wiced_dct_read_lock( (void**) &configAp, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t, config_ap_settings), sizeof(wiced_config_soft_ap_t) );

        /*Insert instruction on LCD for logging into WiCED network*/
        UG_FontSelect( &TEXT_FONT );

        UG_PutString( 0, row, "Using another device connect to");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "the following WiFi network:" );
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        sprintf(buffer, "SSID    :  %s", configAp->SSID.value);
        UG_PutString( 0, row, buffer );
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        sprintf(buffer, "Password:  %s\n", configAp->security_key);
        UG_PutString( 0, row, buffer );
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "Open a web browser and go to " );
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "http://192.168.0.1" );
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "On the page click the Wi-Fi Setup");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "button. Select your WiFi network");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "type in password, press Connect");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);

        /*Print message out UART as well*/
        WPRINT_APP_INFO(("*******************************************************\n"));
        WPRINT_APP_INFO(("Using another device connect to the following WiFi network \n\r"));
        WPRINT_APP_INFO(("SSID    :  %s\n", configAp->SSID.value));
        WPRINT_APP_INFO(("Password:  %s\n", configAp->security_key));
        WPRINT_APP_INFO(("Open a web browser and go to  http://192.168.0.1 \n\r"));
        WPRINT_APP_INFO(("On the page click the Wi-Fi Setup button. Select your WiFi network \n\r"));
        WPRINT_APP_INFO(("type in the password, press connect \n\r"));
        WPRINT_APP_INFO(("*******************************************************\n"));

        /* Configure the device */
        wiced_configure_device( NULL );

        /*Unlock read of DCT*/
        wiced_dct_read_unlock( configAp, WICED_FALSE );
    }

    /*Unlock read of DCT*/
    wiced_dct_read_unlock( device_configured, WICED_FALSE );

    /*Display message that we are connecting to the network entered on configuration webpage*/
    UG_FontSelect( &TEXT_FONT );
    UG_PutString( 0, row,  "Connecting please wait");
    row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);

    /* Bring up the network interface */
    result = wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    if(result == WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("\n\r"));
        WPRINT_APP_INFO( ( "Connected to network \n\r" ) );
        UG_PutString( 0, row, "Connected to network");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
    }
    else
    {

        /*Failed to connect to network try again*/
        WPRINT_APP_INFO(("Could not connect to network \n\r"));
        WPRINT_APP_INFO(("Hold SW2 for 1 sec to clear saved network \n\r" ) );
        WPRINT_APP_INFO(("Then reset device and try again\n\r" ) );

        /*Clear the screen*/
        UG_FillScreen( C_BLACK );
        row = TOP_DISPLAY;

        UG_FontSelect( &HEADING_FONT );

        /*Print welcome message*/
        UG_PutString( 10, row,  "CSU Meter Project");
        /*Increment row for next line*/
        row = (HEADING_FONT.char_height + EXTRA_FONT_SPACE);

        UG_FontSelect( &TEXT_FONT );

        /*print failure messages*/
        UG_PutString( 0, row, "Could not connect to network");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "Hold SW2 for 1 sec to clear saved");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
        UG_PutString( 0, row, "network, reset device and try again");
        row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);

        /*Wait for user to either press SW2 or reset the device*/
        do
        {
            /*is the button pressed*/
            if ( !wiced_gpio_input_get( WICED_BUTTON1 ) )
            {
                /*Delay for a second and check again*/
                wiced_rtos_delay_milliseconds( 1000 );

                if ( !wiced_gpio_input_get( WICED_BUTTON1 ) )
                {
                    /*Set flag to indicate DCT needs to be cleared*/
                    clearDCT = 0;
                    device_configured = &clearDCT;

                    WPRINT_APP_INFO(( "Clearing saved network\n" ));
                    UG_PutString( 0, row, "Clearing saved network");
                    row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);

                    /*Clear out the DCT*/
                    wiced_dct_write( (void**) &device_configured, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t, device_configured), sizeof(wiced_bool_t) );
                    wiced_rtos_delay_milliseconds( 1000 );

                    WPRINT_APP_INFO(( "Clearing done, reset the device\n" ));
                    UG_PutString( 0, row, "Clearing done, reset the device");

                    /*Wait for user to reset device*/
                    while(1);
                }
            }

        } while(1);
    }

    /*Print info that we are connected and starting the server*/
    WPRINT_APP_INFO(("\n\r"));
    WPRINT_APP_INFO(("Connecting to time server\n\r"));
    WPRINT_APP_INFO(("This may take a minute\n\r"));
    WPRINT_APP_INFO(("\n\r"));

    UG_PutString( 0, row,  "Connecting to time server");
    row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
    UG_PutString( 0, row,  "This may take a minute");
    row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);

    /* Disable roaming to other access points */
    wiced_wifi_set_roam_trigger( -99 ); /* -99dBm ie. extremely low signal level */

    /* Start automatic time synchronization and synchronize once every day. */
    /* this is what takes a while */
    sntp_start_auto_time_sync( 1 * DAYS );

    /* Start web server to display current light sensor voltage  & PWM duty cycle */
    wiced_http_server_start( &httpServer, 80, maxSockets, web_pages, WICED_STA_INTERFACE, DEFAULT_URL_PROCESSOR_STACK_SIZE );

    /*Tell user we are ready to go*/
    WPRINT_APP_INFO( ( "Website Ready \n\r" ) );

    /*Display instructions on LCD*/
    UG_FillScreen( C_BLACK );
    UG_FontSelect( &HEADING_FONT );
    row = TOP_DISPLAY;
    UG_PutString( 10, row,  "CSU Meter Project");

    UG_FontSelect( &TEXT_FONT );
    row = (HEADING_FONT.char_height + EXTRA_FONT_SPACE);

    /*Grab IP address to tell user which IP address to connect to*/
    wiced_ip_get_ipv4_address(WICED_STA_INTERFACE, &ipv4Address);

    /*Print message to connect to that ip address*/
    ipAddress = (uint32_t)ipv4Address.ip.v4;
    sprintf(buffer, "%u.%u.%u.%u",  (unsigned char) ( ( ipAddress >> 24 ) & 0xff ),
                                    (unsigned char) ( ( ipAddress >> 16 ) & 0xff ),
                                    (unsigned char) ( ( ipAddress >> 8 ) & 0xff ),
                                    (unsigned char) ( ( ipAddress >> 0 ) & 0xff ));

    UG_PutString( 0, row, "On a device connected to the");
    row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
    UG_PutString( 0, row, "same network open a web browser");
    row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);
    UG_PutString( 0, row, "and go to: ");
    UG_PutString( 120, row, buffer);
    row += (TEXT_FONT.char_height + EXTRA_FONT_SPACE);

    WPRINT_APP_INFO(("\n\r"));
    WPRINT_APP_INFO(("*******************************************************\n"));
    WPRINT_APP_INFO(("On a device connected the same network \n\r"));
    WPRINT_APP_INFO(("Open a web browser and go to: \n\r"));
    WPRINT_APP_INFO(("%u.%u.%u.%u\n", (unsigned char) ( ( ipAddress >> 24 ) & 0xff ),
                                      (unsigned char) ( ( ipAddress >> 16 ) & 0xff ),
                                      (unsigned char) ( ( ipAddress >> 8 )  & 0xff ),
                                      (unsigned char) ( ( ipAddress >> 0 )  & 0xff ) ) );
    WPRINT_APP_INFO(("\n\r"));
    WPRINT_APP_INFO(("Webpage presents meter data in plain text \n\r"));
    WPRINT_APP_INFO(("*******************************************************\n"));
    wiced_uart_transmit_bytes( STDIO_UART, "TestXmiti\n\r", sizeof( "TestXmiti\n\r" ) - 1 );
    UG_FontSelect( &HEADING_FONT );

    UG_PutString( 0, 90, "Usage (kW):");


    /*Indicate to threads it is okay to print data*/
    screenReady = 1;

    /* Sleep this thread */
    wiced_rtos_get_semaphore( &semaphore, WICED_WAIT_FOREVER );
}

/*
 * Updates usage information on the web page
 */
int32_t processUsageUpdate( const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data )
{
    char                    tempCharBuffer[128];
    int                     tempCharBufferLength;
    char c;
    //c[1] = '\0';
    char d[2];
    d[1] = '\0';
    char * commaA;
    char * commaB;
    char recdstring[64];
    char * StartOfData;
    char * EndOfData;
    char usage[8];
    char voltage[8];
    int recdcount = 0;
    uint32_t expected_data_size = 1;
    int recstart = 0;
    UNUSED_PARAMETER( url_path );
    UNUSED_PARAMETER( http_data );
    //WPRINT_APP_INFO(("Updating:\n\r"));
    wiced_uart_transmit_bytes( STDIO_UART, "TestXmit\n\r", sizeof( "TestXmit\n\r" ) - 1 );
    while ( wiced_uart_receive_bytes( STDIO_UART, &c, &expected_data_size, WICED_NEVER_TIMEOUT ) == WICED_SUCCESS )
        {
            d[0] = c;
            //wiced_uart_transmit_bytes( STDIO_UART, "gotchar\n\r", sizeof( "gotchar\n\r" ) - 1 );
            wiced_uart_transmit_bytes( STDIO_UART, &c, 1 );
            if(d[0] == '[')
            {
                recstart = 1;
            }
            if(recstart){
                strcat(recdstring,d);
            }
            recdcount++;

            if((recdcount==50)||((d[0] == ']')&&recstart))
                break;
            expected_data_size = 1;
        }

    wiced_uart_transmit_bytes( STDIO_UART, "outofloop\n\r", sizeof( "outofloop\n\r" ) - 1 );
    //wiced_uart_transmit_bytes( STDIO_UART, &recdstring, sizeof( recdstring ) - 1 );
    wiced_uart_transmit_bytes( STDIO_UART, "\n\r", sizeof( "\n\r" ) - 1 );
    /*/parse recieved data
    /commaA = strchr(recdstring,',');
    commaB = strrchr(recdstring,',');
    commaA-recdstring+1;
    commaB-recdstring+1;
    strncpy(usage,recdstring+1,(commaA-recdstring+1));
    */
    //StartOfData = strchr(&recdstring,'[');
    //EndOfData = strchr(&recdstring,']');
    //commaA = strchr(&recdstring,',');
    //commaB = strrchr(&recdstring,',');

    //int i = 0;
    //int length = commaA-StartOfData;
    //while (i < length)
    //{
    //    usage[i] = StartOfData[i+1];
    //    i++;
    //}
    //usage[i] = '\0';
    //wiced_uart_transmit_bytes( STDIO_UART, &usage, sizeof( usage ) - 1 );
    //wiced_uart_transmit_bytes( STDIO_UART, "\n\r", sizeof( "\n\r" ) - 1 );

    //commaA-recdstring+1;
    //Fill the variables to check that it's working
    usageData.usage = 123;
    usageData.voltage = 32;
    usageData.curr_time_year=2018;
    usageData.curr_time_month=12;
    usageData.curr_time_day=1;
    usageData.curr_time_hr=1;
    usageData.curr_time_min=47;
    usageData.curr_time_sec=33;

    /*make sure it is okay to print data*/
    if(screenReady)
    {
        UG_FontSelect( &HEADING_FONT );

        /*Print on LCD the usage data*/
        UG_PutString( 230, 115, usageData.usage);
    }

    /* Write the time to the web page */
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_time_start );
    tempCharBufferLength = sprintf(tempCharBuffer, "%d:%d:%d", usageData.curr_time_hr, usageData.curr_time_min, usageData.curr_time_sec);
    wiced_http_response_stream_write(stream, tempCharBuffer, tempCharBufferLength );
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_time_end );

    /* Write the date to the webpage */
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_date_start );
    tempCharBufferLength = sprintf(tempCharBuffer, "%d-%d-%d", usageData.curr_time_year, usageData.curr_time_month, usageData.curr_time_day);
    wiced_http_response_stream_write(stream, tempCharBuffer, tempCharBufferLength );
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_date_end );

    /* Write the usage data to the webpage */
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_usage_start );
    tempCharBufferLength = sprintf(tempCharBuffer, "%s", recdstring);
    wiced_http_response_stream_write(stream, tempCharBuffer, tempCharBufferLength );
    wiced_http_response_stream_write_resource(  stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_usage_end);

    /* Write the voltage to the webpage */
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_voltage_start );
    tempCharBufferLength = sprintf(tempCharBuffer, "%d", usageData.voltage);
    wiced_http_response_stream_write(stream, tempCharBuffer, tempCharBufferLength );
    wiced_http_response_stream_write_resource(  stream, &resources_apps_DIR_Smart_Meter_PSoC6_Webserver_DIR_data_html_voltage_end);

    return 0;
}
