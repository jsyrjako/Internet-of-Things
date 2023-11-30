APPLICATION = iot2023sensor

# Set CoAP server IPv6 address here or override on command line using e.g. 'make COAP_SERVER_IPV6_ADDR=2001:db8::1'
COAP_SERVER_IPV6_ADDR ?= "2001:14ba:a819:5a01:4159:583e:cc58:2240"

BOARD ?= iotlab-m3

USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif,
USEMODULE += gnrc_ipv6_default
USEMODULE += gcoap
USEMODULE += od
USEMODULE += fmt

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += ztimer_sec
USEMODULE += ztimer_msec
USEMODULE += ztimer_usec
USEMODULE += ztimer_no_periph_rtt
USEMODULE += isl29020
USEMODULE += lps331ap

USEMODULE += nanocoap_sock
USEMODULE += nanocoap_resources

FEATURES_REQUIRED += periph_i2c

RIOTBASE ?= ../../../include/RIOT

CFLAGS += -DCOAP_SERVER_IPV6_ADDR=\"$(COAP_SERVER_IPV6_ADDR)\"

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTBASE)/makefiles/default-radio-settings.inc.mk
