APPLICATION = iot2023joonajanne

BOARD ?= iotlab-m3

USEMODULE += lps331ap
USEMODULE += isl29020

FEATURES_REQUIRED += periph_uart

RIOTBASE ?= $(CURDIR)/../../RIOT

include $(RIOTBASE)/Makefile.include
