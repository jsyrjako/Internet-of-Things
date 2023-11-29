APPLICATION = iot2023sensor

BOARD ?= iotlab-m3

USEMODULE += ipv6_addr
USEMODULE += lwip_ipv6
USEMODULE += lwip_ipv6_autoconfig
USEMODULE += gnrc_icmpv6_echo     # Additional module that can be dropped
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
USEMODULE += gnrc_pktdump

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netdev_default
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

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

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTBASE)/makefiles/default-radio-settings.inc.mk
