APPLICATION = iot2023sensor

BOARD ?= iotlab-m3

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
# This application dumps received packets to STDIO using the pktdump module
USEMODULE += gnrc_pktdump
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

USEMODULE += ztimer_msec
USEMODULE += ztimer_usec
USEMODULE += ztimer_sec
USEMODULE += ztimer_no_periph_rtt
USEMODULE += lps331ap
USEMODULE += isl29020
#USEMODULE += ipv6_addr
#USEMODULE += lwip_ipv6_autoconfig
#USEMODULE += MQTTClient
#USEPKG += paho-mqtt

FEATURES_REQUIRED += periph_i2c

RIOTBASE ?= ../../../include/RIOT

# CFLAGS += -Werror

# EXTERNAL_MODULE_DIRS += $(CURDIR)/../../include/Eclipse-Paho-MQTT-C/

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
#include $(RIOTMAKE)/default-radio-settings.inc.mk
