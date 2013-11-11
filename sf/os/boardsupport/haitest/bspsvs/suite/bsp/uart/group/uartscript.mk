#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT2:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT2)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

UART_FOLDER=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z\base\serial

SCRIPT_PORT0=$(UART_FOLDER)\base-bsp-serial-driver-PORT0.script
INI_PORT0=$(UART_FOLDER)\base-bsp-serial-driver-PORT0.ini

SCRIPT_PORT0_2400=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-2400.script
INI_PORT0_2400=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-2400.ini

SCRIPT_PORT0_4800=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-4800.script
INI_PORT0_4800=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-4800.ini

SCRIPT_PORT0_9600=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-9600.script
INI_PORT0_9600=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-9600.ini

SCRIPT_PORT0_115200=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-115200.script
INI_PORT0_115200=$(UART_FOLDER)\base-bsp-serial-driver-PORT0-115200.ini

SCRIPT_STRESS_PORT0_2400=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-2400.script
INI_STRESS_PORT0_2400=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-2400.ini

SCRIPT_STRESS_PORT0_4800=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-4800.script
INI_STRESS_PORT0_4800=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-4800.ini

SCRIPT_STRESS_PORT0_9600=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-9600.script
INI_STRESS_PORT0_9600=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-9600.ini

SCRIPT_STRESS_PORT0_115200=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-115200.script
INI_STRESS_PORT0_115200=$(UART_FOLDER)\base-bsp-serial-driver-stress-PORT0-115200.ini

$(UART_FOLDER) :
	@perl -S emkdir.pl "$(UART_FOLDER)"

$(SCRIPT_PORT0)			: ..\scripts\base-bsp-serial-driver-port.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=9600
	@echo $@

$(INI_PORT0)			: ..\testdata\base-bsp-serial-driver-port.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=9600
	@echo $@

$(SCRIPT_PORT0_2400)	: ..\scripts\base-bsp-serial-driver-baud.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=2400
	@echo $@

$(INI_PORT0_2400)		: ..\testdata\base-bsp-serial-driver-baud.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=2400
	@echo $@

$(SCRIPT_PORT0_4800)	: ..\scripts\base-bsp-serial-driver-baud.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=4800
	@echo $@

$(INI_PORT0_4800)		: ..\testdata\base-bsp-serial-driver-baud.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=4800
	@echo $@

$(SCRIPT_PORT0_9600)	: ..\scripts\base-bsp-serial-driver-baud.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=9600
	@echo $@

$(INI_PORT0_9600)		: ..\testdata\base-bsp-serial-driver-baud.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=9600
	@echo $@

$(SCRIPT_PORT0_115200)	: ..\scripts\base-bsp-serial-driver-baud.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=115200
	@echo $@

$(INI_PORT0_115200)		: ..\testdata\base-bsp-serial-driver-baud.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=115200
	@echo $@

$(SCRIPT_STRESS_PORT0_2400)	: ..\scripts\base-bsp-serial-driver-stress.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=2400
	@echo $@

$(INI_STRESS_PORT0_2400)		: ..\testdata\base-bsp-serial-driver-stress.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=2400
	@echo $@

$(SCRIPT_STRESS_PORT0_4800)	: ..\scripts\base-bsp-serial-driver-stress.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=4800
	@echo $@

$(INI_STRESS_PORT0_4800)		: ..\testdata\base-bsp-serial-driver-stress.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=4800
	@echo $@

$(SCRIPT_STRESS_PORT0_9600)	: ..\scripts\base-bsp-serial-driver-stress.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=9600
	@echo $@

$(INI_STRESS_PORT0_9600)		: ..\testdata\base-bsp-serial-driver-stress.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=9600
	@echo $@

$(SCRIPT_STRESS_PORT0_115200)	: ..\scripts\base-bsp-serial-driver-stress.script
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=115200
	@echo $@

$(INI_STRESS_PORT0_115200)		: ..\testdata\base-bsp-serial-driver-stress.ini
	perl uartscript.pl --in="$?" --out="$@" --port=0 --baud=115200
	@echo $@

BLD				:	$(UART_FOLDER) \
					$(SCRIPT_PORT0) $(INI_PORT0) \
					$(SCRIPT_PORT0_2400) $(INI_PORT0_2400) \
					$(SCRIPT_PORT0_4800) $(INI_PORT0_4800) \
					$(SCRIPT_PORT0_9600) $(INI_PORT0_9600) \
					$(SCRIPT_PORT0_115200) $(INI_PORT0_115200) \
					$(SCRIPT_STRESS_PORT0_2400) $(INI_STRESS_PORT0_2400) \
					$(SCRIPT_STRESS_PORT0_4800) $(INI_STRESS_PORT0_4800) \
					$(SCRIPT_STRESS_PORT0_9600) $(INI_STRESS_PORT0_9600) \
					$(SCRIPT_STRESS_PORT0_115200) $(INI_STRESS_PORT0_115200)

CLEAN			:
	-$(ERASE) $(SCRIPT_PORT0)
	-$(ERASE) $(INI_PORT0)
	-$(ERASE) $(SCRIPT_PORT0_2400)
	-$(ERASE) $(INI_PORT0_2400)
	-$(ERASE) $(SCRIPT_PORT0_4800)
	-$(ERASE) $(INI_PORT0_4800)
	-$(ERASE) $(SCRIPT_PORT0_9600)
	-$(ERASE) $(INI_PORT0_9600)
	-$(ERASE) $(SCRIPT_PORT0_115200)
	-$(ERASE) $(INI_PORT0_115200)
	-$(ERASE) $(SCRIPT_STRESS_PORT0_2400)
	-$(ERASE) $(INI_STRESS_PORT0_2400)
	-$(ERASE) $(SCRIPT_STRESS_PORT0_4800)
	-$(ERASE) $(INI_STRESS_PORT0_4800)
	-$(ERASE) $(SCRIPT_STRESS_PORT0_9600)
	-$(ERASE) $(INI_STRESS_PORT0_9600)
	-$(ERASE) $(SCRIPT_STRESS_PORT0_115200)
	-$(ERASE) $(INI_STRESS_PORT0_115200)

CLEANLIB		:

FINAL			:

FREEZE			:

LIB				:

MAKMAKE			:

RELEASABLES		:
	@echo $(SCRIPT_PORT0)
	@echo $(INI_PORT0)
	@echo $(SCRIPT_PORT0_2400)
	@echo $(INI_PORT0_2400)
	@echo $(SCRIPT_PORT0_4800)
	@echo $(INI_PORT0_4800)
	@echo $(SCRIPT_PORT0_9600)
	@echo $(INI_PORT0_9600)
	@echo $(SCRIPT_PORT0_115200)
	@echo $(INI_PORT0_115200)
	@echo $(SCRIPT_STRESS_PORT0_2400)
	@echo $(INI_STRESS_PORT0_2400)
	@echo $(SCRIPT_STRESS_PORT0_4800)
	@echo $(INI_STRESS_PORT0_4800)
	@echo $(SCRIPT_STRESS_PORT0_9600)
	@echo $(INI_STRESS_PORT0_9600)
	@echo $(SCRIPT_STRESS_PORT0_115200)
	@echo $(INI_STRESS_PORT0_115200)

RESOURCE		:

SAVESPACE		:
