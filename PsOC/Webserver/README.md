Setup

1. Navigate to WICED-Studio-6.2\43xxx_Wi-Fi\apps\demo and create a new folder called "Smart_Meter_PSoC6_Webserver"

Copy the following files to that folder:

  Smart_Meter_PSoC6_Webserver.c
  
  Smart_Meter_PSoC6_Webserver.h
  
  Smart_Meter_PSoC6_Webserver.mk
  
  cy_tft_display.c
  
  cy_tft_display.h


2. Navigate to WICED-Studio-6.2\43xxx_Wi-Fi\resources\apps and again create a new folder called "Smart_Meter_PSoC6_Webserver"

Copy the following files to that folder:

  data.html
  
  main.html


3. Open WICED 6.2


4. Plug the CY8CKIT-062-WiFi-BT kit board into your computer’s USB port.


5. Ensure that LED2 on the CY8CKIT-062-WiFi-BT is off. If it is not, press SW3 “mode select.”


6. In the Make Target window, right-click and select New, and give it the following Target Name:

  demo.Smart_Meter_PSoC6_Webserver-CY8CKIT_062 download_apps download run


7. Double-click the newly created make target to build the code, program the kit, and run the example. Note that this may take a few minutes.


8. After the project is downloaded, the TFT LCD prints out instructions. The device starts a WiFi Access Point (AP). This AP allows you to select your own WiFi network to be used for the rest of the demo.


9. Using another device (laptop, PC, mobile phone, tablet), connect to the following WiFi network:

  ssid: WICED Config
  
  password: 12345678


10. Once connected, open a web browser and type 192.168.0.1. The device configuration webpage will appear.


11. Click the Wi-Fi Setup button; a page appears listing the available WiFi networks.


12. Select the network you wish to connect to, enter your password, and click Connect. After this, the device turn off its AP and connects to the network selected, connects to a time server, and servers an HTTP page.

Note: If for some reason you enter an incorrect password, the Terminal/LCD indicates a failure to connect. Pressing and holding SW2 for 1 second erases the saved password, at which point you must reset the device and follow all the steps again.


13. Next, on a device connected to the same network as selected in step 13, open a web browser and type the IP address shown on the LCD output. The HTTP page will appear:

Note: If your WiFi network does not have internet access, then the data will start at 1970-01-01 and the time will start at 00:00:00.


Mac Address (in \WICED-Studio-6.2\43xxx_Wi-Fi\generated_mac_address.txt)
/* 
 * The MAC address of the Wi-Fi device may be configured in one of several places as 
 * described in the document WICED-AN800-R Factory Programming Application Note.     
 * Please read this document for further information.                                
 */

#define NVRAM_GENERATED_MAC_ADDRESS        "macaddr=00:A0:50:1b:ee:dd"
#define DCT_GENERATED_MAC_ADDRESS          "\x00\xA0\x50\xdd\x1b\xee"
#define DCT_GENERATED_ETHERNET_MAC_ADDRESS "\x00\xA0\x50\xdd\x1b\xef"
