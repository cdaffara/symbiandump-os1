# sf_config level configuration makefile

#Define this platform as default if nothing is defined
ifeq (,$(S60))
S60 += 91
$(warning S60 platform not defined (E.g. S60=91)! Using $(S60))
endif

ifeq (91,$(findstring 91,$(S60)))

MAKEFILE = /sf/config/sf_config/Makefile

sf_all    :: sf_config
sf_config ::
sf_config :: config
config_all    :: sf_all

include include_template.mk

endif