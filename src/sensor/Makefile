APPLICATION = iot2023joonajanne

BOARD ?= iotlab-m3

USEMODULE += ztimer_msec
USEMODULE += lps331ap
USEMODULE += isl29020
USEMODULE += ipv6_addr
USEMODULE += lwip_ipv6_autoconfig
USEMODULE += MQTTClient-C
#USEPKG += paho-mqtt

FEATURES_REQUIRED += periph_i2c

RIOTBASE ?= ../../include/RIOT

CFLAGS += -Werror

EXTERNAL_MODULE_DIRS += $(CURDIR)/../../include/paho.mqtt.embedded-c/

include $(RIOTBASE)/Makefile.include
