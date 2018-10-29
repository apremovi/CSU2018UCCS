#
# $ Copyright Cypress Semiconductor $
#

NAME := App_Smart_Meter_PSoC6_Webserver

$(NAME)_SOURCES    := Smart_Meter_PSoC6_Webserver.c \
					  cy_tft_display.c

$(NAME)_COMPONENTS := daemons/HTTP_server \
                      daemons/device_configuration \
                      protocols/SNTP \
                      graphics/ugui \
                      daemons/Gedday 

$(NAME)_RESOURCES  := apps/Smart_Meter_PSoC6_Webserver/main.html \
                      apps/Smart_Meter_PSoC6_Webserver/data.html \
                      images/favicon.ico \
                      styles/buttons.css \
                      scripts/general_ajax_script.js \
                      scripts/wpad.dat

GLOBAL_DEFINES     := USE_SELF_SIGNED_TLS_CERT


VALID_PLATFORMS    := CY8CKIT_062

ifeq ($(PLATFORM),$(filter $(PLATFORM), CYW9MCU7X9N364))
GLOBAL_DEFINES += APPLICATION_STACK_SIZE=10*1024 WICED_DISABLE_TLS
endif
