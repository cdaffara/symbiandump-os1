# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This file provides the definition of variables that are used by the component
# specific makefile. This is for the BPABI (Base Platform ABI) platform using the
# RVCT compiler.
# Path Settings
# 
#

# Compiler Installation Location
CC_INSTALL_PATH=

#----------------------------------
# Programs used from the ToolChain
#----------------------------------

# C++ Compiler
CC=armcc

# Linker
LD=armlink

# Assembler
ASM=armasm

# Archiver
AR=armar

# Translator to translate the GCC inline assembler code
TRANASM=tranasm.bat

#--------------------
# Option Prefix
#--------------------

# This value will be used by the backend to segregate one option from the other.
# If option prefix is one among '+','*','.'or '?' (these metacharacters have specific predefined meaning
# for pattern matching in Perl) then it should be preceded with '\'.
OPTION_PREFIX=

#------------------
# Compiler Options
#------------------

# Flag whether the compiler requires Unix slashes for absolute paths
UNIX_SLASH_FOR_CC_ABS_PATHS=0

#-------------------
# Debug Mode Options
#-------------------

# Optimization Level in DEBUG mode
DEBUG_OPTIMISATION=-O0

# Option to produce debug information
DEBUG_OPTION=-g

DEBUG_FORMAT_DWARF2=--dwarf2
DEBUG_FORMAT_DWARF3=--dwarf3

# DWARF 2 is default on the ARMV5 build platform.
DEBUG_FORMAT=$(DEBUG_FORMAT_DWARF2)

# Specific compiler options for a UDEB build
SYMBIAN_UDEB_CCFLAGS=$(DEBUG_OPTION)

#-------------------------
# Target Processor Options
#-------------------------

# Option to select the appropriate target processor
ifeq "$(RVCT_VER_MAJOR)" "2"
TARGET_ARCH_OPTION=--cpu 5T
else
TARGET_ARCH_OPTION=--cpu 5TE
endif

# Option to generate the approproate ARM instruction set.
ARM_INSTRUCTION_SET=--arm

# Option to generate the approproate thumb instruction set.
THUMB_INSTRUCTION_SET=--thumb

# Compiler define for thumb instruction set
COMPILER_THUMB_DEFINES=-D__MARM_THUMB__

# Compiler define for interwork
COMPILER_INTERWORK_DEFINES=-D__MARM_INTERWORK__

# Option to specify the floating point conformance.
FPMODE_OPTION=--fpmode ieee_no_fenv

# Compiler option to select softvfp mode
SOFTVFPMODE_OPTION=--fpu softvfp

# Compiler option to select hardware floating point unit
VFP2MODE_OPTION=--fpu vfpv2

# Option to force all enumerations to be stored in as integers.
ENUM_OPTION=--enum_is_int

# Option to disable the generation of unaligned word and halfword accesses on ARMV6
# and ARMV7 processors.
ifeq "$(RVCT_VER_MAJOR)" "2"
NO_UNALIGNED_ACCESS=--memaccess -UL41
else
NO_UNALIGNED_ACCESS=--no_unaligned_access
endif

#---------------------------------
# Options controlling C++ Features
#---------------------------------

# Option for handling Virtual functions and Virtual Tables
EXPORT_VTBL_OPTION=--export_all_vtbl

# Disables unused virtual function elimination (VFE) in C++ mode. --vfe is the default
VFE_OPTION=--no_vfe

# Option to turn on exception handling
EXCEPTIONS=--exceptions --exceptions_unwind
NO_EXCEPTIONS=--no_exceptions --no_exceptions_unwind

#-------------------------------------------------------------------------
# Options controlling the ARM Architecture Procedure Call Standard (AAPCS)
#-------------------------------------------------------------------------

# This Option is for ARM Architecture Procedure Call Standard with the
# qualifier to support calls between the ARM and Thumb instruction sets.
AAPCS_OPTION=--apcs /inter

#-----------------------------------------------------------
# Options controlling generation of Compiler Warnings/Errors
#-----------------------------------------------------------

# Option to control the Compiler warnings
CC_WARNINGS_CONTROL_OPTION=--diag_suppress 161,611,654,997,1152,1300,1464,1488,6318,6331,2523

# Option to control the Compiler warnings for building Standard C++ application
CC_STDCPP_WARNINGS_CONTROL_OPTION=--diag_suppress 161,611,654,997,1152,1300,1464,1488,6318,6331

# Option to suppress the Compiler errors
CC_ERRORS_CONTROL_OPTION=--diag_error 1267

# Option to suppress the Compiler errors for building Standard C++
CC_STDCPP_ERRORS_CONTROL_OPTION=--diag_error 1267

# Option to modify the Compiler warnings and errors for Symbian OS
SYMBIAN_CC_MESSAGE_OPTION=$(CC_WARNINGS_CONTROL_OPTION) $(CC_ERRORS_CONTROL_OPTION)

#-----------------
# Optional Options
#-----------------

# Compiler option to avoid the generation of the intermediate files. (Optional)
TEMP_FILES_OPTION=

# Library Option
OWN_LIBRARY_OPTION=--library_interface=aeabi_clib

# Option to generate the Object File
COMPILE_ONLY_OPTION=-c

# Option to generate the Preprocessed File
PREPROCESSOR_OPTION=-E

# Other additional Options to be passed to the compiler
EXTRA_CC_OPTION=

#---------------------
# Preprocessor Options
#---------------------

# Prefix Header File passed to the preprocessor
PREFIXFILE=$(EPOCROOT)epoc32\include\rvct\rvct.h

# For .cpp Source files
CPP_LANG_OPTION=--cpp

# For .c Source files
C_LANG_OPTION=--c90

# For .cia Source files
CIA_LANG_OPTION=--cpp -D__CIA__

#-------------------
# Assembler Options
#-------------------

# Option to generate the Assembler instructions
ASSEMBLER_LISTING_OPTION=-S --interleave

# Output option used to pass the output file name
ASM_OUTPUT_OPTION=-o

#------------------
# Translator Options
#------------------

# Flags to be passed to the translator
TRANASM_FLAGS=-n -s

# Output option used to pass the output file name
TRANASM_OUTPUT_OPTION=-o=

# Input option used to pass the input file name
TRANASM_INPUT_OPTION=

#---------------------------
# Include Options and Files
#---------------------------

# Option to specify the location of the header files
INCLUDE_OPTION=-J

# Preinclude file for that compiler that contains all the compiler specific definitions
# required by the Symbian OS source code.
PREINCLUDE_OPTION=--preinclude $(EPOCROOT)epoc32\include\rvct\rvct.h

# Include options required by Function Call Logger
FC_LOGGER_INCLUDE_OPTION=-I
FC_LOGGER_DICTIONARY_FILE_NAME=--dictionary_file_name
FC_LOGGER_GENERATED_C_FILE_NAME=--gen_c_file_name

# Preinclude file to be passed to the Function Call Logger which uses EDG compiler
#PREINCLUDE_OPTION_FCLOGGER=$(FC_LOGGER_INCLUDE_OPTION) $(EPOCROOT)EPOC32\INCLUDE\RVCT --preinclude edg_rvct2_2.h

# Option to control the search for the header files. For example, if we do not want to do a search in the
# standard include directory of C++, then can restrict it by providing the appropriate option.
HEADER_FILES_CONTROL_OPTION=

# Path to pick the header files from the Compiler installation directory
COMPILER_INCLUDE_PATH=

# Fetches the version of the tools from the installation directory
VERSION_OPTION=
VERSION_INFO=

#---------------------
# Release Mode Options
#---------------------

# Optimization Level in RELEASE mode
REL_OPTIMISATION=-O2

# Specific compiler options for a UREL build
SYMBIAN_UREL_CCFLAGS=

#---------------------------------
# Symbol Attribute Setting Options
#---------------------------------

# Option to set the visibility of runtime symbols as DEFAULT (instead of HIDDEN)
RUNTIME_SYMBOL_VISIBILITY_OPTION=--dllimport_runtime

# Option to specify the output of the toolchain
OUTPUT_OPTION=-o

# Options to be passed when building System Target(kernel)
KERNEL_OPTIONS=$(ARM_INSTRUCTION_SET) $(NO_EXCEPTIONS)

# Options to be passed when building in Arm mode
ARM_OPTIONS=$(ARM_INSTRUCTION_SET)

# Options to be passed when building in Thumb mode
THUMB_OPTIONS=$(THUMB_INSTRUCTION_SET)

# Common compiler options for Arm and Thumb mode
COMMON_OPTIONS=$(DEBUG_FORMAT) $(SYMBIAN_CC_MESSAGE_OPTION)

# Invariant Options which cannot be modified by the user from MMP file
INVARIANT_OPTIONS= $(TARGET_ARCH_OPTION) $(ENUM_OPTION) $(OWN_LIBRARY_OPTION) $(FPMODE_OPTION) $(EXPORT_VTBL_OPTION) $(VFE_OPTION) $(AAPCS_OPTION)

# Common compiler options for compiling programs targeted at Symbian OS
#CCFLAGS=$(SYMBIAN_CC_MESSAGE_OPTION) $(EXCEPTIONS) $(TARGET_ARCH_OPTION) $(ENUM_OPTION) $(OWN_LIBRARY_OPTION) $(FPMODE_OPTION) $(EXPORT_VTBL_OPTION) $(VFE_OPTION) $(AAPCS_OPTION) $(TEMP_FILES_OPTION) $(HEADER_FILES_CONTROL_OPTION) $(COMPILE_ONLY_OPTION) $(EXTRA_CC_OPTION)
CCFLAGS=$(COMMON_OPTIONS) $(INVARIANT_OPTIONS) $(TEMP_FILES_OPTION) $(HEADER_FILES_CONTROL_OPTION) $(COMPILE_ONLY_OPTION) $(EXTRA_CC_OPTION)

#------------------
# Linker Options
#------------------

# Output option used to pass the output file name
LINKER_OUTPUT_OPTION=-o

# Option to generate debug information
LINKER_DEBUG_OPTION=--debug

# Option to *not* generate debug information.
LINKER_NODEBUG_OPTION=

#--------------------------------------------------------------
# Options to generate executables conforming to BPABI standards
#--------------------------------------------------------------

# Option to generate an executable conforming to the Base Platform ABI for the ARM Architecture
BPABI_OPTION=--bpabi

# The `R_ARM_TARGET1' relocation is typically used for entries in the `.init_array' section. It is
# interpreted as either `R_ARM_REL32' or `R_ARM_ABS32', depending on the target. The following option
# override the default export relocations.
TARGET_RELOCATION_OPTION=

#-------------------------------------
# Options to specify the output format
#-------------------------------------

# Option to create a relocatable ELF image. A relocatable image has a dynamic segment that contains
# relocations that can be used to relocate the image post link-time.
RELOCATABLE_IMAGE_OPTION=

# Option to create an ELF shared object.
SHARED_OBJECT_OPTION=--dll

#-------------------------------------------
# Options to set the Start of RO/RW sections
#-------------------------------------------

# Option to set the start of the code section (RO Region)
CODE_SEGMENT_START=

# Option to split object into RO and RW sections
SPLIT_OPTION=

# Option to set the start of the Data section (RW region)
RW_BASE_OPTION=--rw-base

# Option to be passed to the linker to set the start of the data section (RW region)
DATA_SEGMENT_START=$(SPLIT_OPTION) $(RW_BASE_OPTION)

#---------------------------------------------------------
# Options controlling generation of Linker Warnings/Errors
#---------------------------------------------------------

# Option to control the Linker warnings
LD_WARNINGS_CONTROL_OPTION=--diag_suppress 6331,6780

# Option to suppress the Linker errors
LD_ERRORS_CONTROL_OPTION=

# Option to modify the Linker warnings and errors for Symbian OS
SYMBIAN_LD_MESSAGE_OPTION=$(LD_WARNINGS_CONTROL_OPTION) $(LD_ERRORS_CONTROL_OPTION)

# Option to specify whether unresolved symbol references from regular object files can be allowed or not.
UNRESOLVED_SYMBOL_REF_OPTION=

# Option to specify the undefined reference
UNDEFINED_SYMBOL_REF_OPTION=

# Other additional Options to be passed to the Linker
EXTRA_LD_OPTION=--datacompressor=off

ifeq "$(RVCT_VER_MAJOR)" "3"
# Without this linker option, objects with ".directive" sections (i.e. objects
# created by RVCT 2.2) will cause problems.
EXTRA_LD_OPTION += --export_all
endif

#-------------------
#Entry Point Options
#-------------------

# Option to pass the explicit symbol for beginning execution of the program
LINKER_ENTRY_OPTION=--entry

# Symbol used to denote the Start of the grouped archives.
START_GROUP_SYMBOL=(

# Symbol used to denote the End of the grouped archives.
END_GROUP_SYMBOL=)

#-------------------------
# Library Specific Options
#-------------------------

# Option to control the search in the standard libraries
STDLIB_OPTION=--no_scanlib

# Additional static libraries that should automatically be supplied to the linker.
ifeq "$(RVCT_VER_MAJOR)" "2"
STATIC_LIBS_LIST=armlib\h_t__uf.l(switch8.o)
else
STATIC_LIBS_LIST=armlib\h_5.l(switch8.o)
endif

# Option to pass the path from where the runtime libraries should be picked up
STATIC_LIBS_PATH=--libpath

# Fetches the library path from the installation directory
STATIC_LIBRARY_PATH=
STATIC_LIBS=

# Additional runtime libraries that should be supplied to the linker.
RTVER:=$(RVCT_VER_MAJOR)_$(RVCT_VER_MINOR)
RUNTIME_LIBS_LIST=drtaeabi.dso dfpaeabi.dso dfprvct$(RTVER).dso drtrvct$(RTVER).dso


# The library that contains operator new and operator delete.
SYM_NEW_LIB=scppnwdl.dso

# Option to pass the path from where the runtime libraries should be picked up
RUNTIME_LIBS_PATH=

# Option to turn on implicit symbol versioning
SYMVER_OPTION=--symver_soname

# Option to specify the shared object name that is stored in the executable
SO_NAME_OPTION=--soname

# Option to generate the map file that provides information about the linking
LINKER_SYMBOLS_OPTION=--symbols

# Option to specify the Symbols file
LINKER_SYMBOLS_FILE_OPTION=--list

# Option to produce the Map file that provides information about linking
LINKER_SYMBOLS_MAP_OPTION=$(LINKER_SYMBOLS_OPTION) $(LINKER_SYMBOLS_FILE_OPTION)

# Option to specify the linker script file
LINKER_SCRIPT_FILE_OPTION=

#Via file prefixes
VIA_FILE_PREFIX=
VIA_FILE_SUFFIX=

# option to take object file names from other(via) file, here in case of ARMV5 it is necessary to specify space after the "--via " option.
COMMANDFILE_OPTION:=--via 

# Linker options which can be customized from BSF
LD_OPTIONS=$(SYMBIAN_LD_MESSAGE_OPTION)$(UNRESOLVED_SYMBOL_REF_OPTION) $(STDLIB_OPTION) $(EXTRA_LD_OPTION)

# Linker option common to all link commands for UREL build
SYMBIAN_UREL_LINK_FLAGS=$(BPABI_OPTION) $(RELOCATABLE_IMAGE_OPTION) $(TARGET_RELOCATION_OPTION) $(LD_OPTIONS)

# Linker option common to all link commands for UDEB build
SYMBIAN_UDEB_LINK_FLAGS=$(BPABI_OPTION) $(RELOCATABLE_IMAGE_OPTION) $(TARGET_RELOCATION_OPTION) $(LD_OPTIONS)


#-----------------
# Archiver Options
#-----------------

# ARCHIVER variable should be set since there is a dependency for this variable in e32test.
ARCHIVER= $(AR)

ARCHIVER_CREATE_OPTION=--create

# Archiver options which can be set from BSF files
AR_OPTIONS=

#--------------------------------------
# Compiler and Platform specific macros
#--------------------------------------

# Macro definitions required to identify the compiler. Allows for conditional compilation based on compiler
PLATFORM_DEFINES=

COMPILER_DEFINES=

# Compiler target option
COMPILER_PLAT=ARMCC

#--------------------------------------
# Function Call Logger Options
#--------------------------------------
FC_LOGGER_OPTION=--wchar_t_keyword --microsoft_version=1300 --diag_suppress 66,161,611,654,815,830,997,1152,1300,1390

# Defines for Function Call Logger
FC_LOGGER_DEFINES=-D__ARMCC_VERSION=220435

#------------
# OE Options
#------------

# Options to export all the globol symbols and import undefined references, required by OE
OE_OPTIONS=--no_hide_all

# OE Glue code libs
OE_EXE_LIBS=libcrt0.lib
OE_EXE_LIBS_WCHAR=libwcrt0.lib

# OE Import Libs
OE_IMPORT_LIBS=euser.lib

# The library that contains operator new and operator delete.
OE_NEW_LIB=stdnew.dso

#-----------------------------------------------------------------------------------------------------
#This section defines the interface with Symbian tools
#This SHOULD NOT be changed as any changes in this section will not be picked up by the tools back end
#-----------------------------------------------------------------------------------------------------

# Programs used from the ToolChain
export  CC
export  LD
export  ASM
export  AR
export  TRANASM

# Preprocessor Options
export  PREFIXFILE
export  CPP_LANG_OPTION
export  C_LANG_OPTION
export  CIA_LANG_OPTION

# Option Prefix
export OPTION_PREFIX

# Compiler Options
export	PREPROCESSOR_OPTION
export  ASSEMBLER_LISTING_OPTION
export  ASM_OUTPUT_OPTION
export  INCLUDE_OPTION
export  PREINCLUDE_OPTION
export  HEADER_FILES_CONTROL_OPTION
export  COMPILER_INCLUDE_PATH
export  SYMBIAN_UREL_CCFLAGS
export  SYMBIAN_UDEB_CCFLAGS
export  REL_OPTIMISATION
export  DEBUG_OPTIMISATION
export  ARM_OPTIONS
export  THUMB_OPTIONS
export  COMMON_OPTIONS
export  INVARIANT_OPTIONS
export  KERNEL_OPTIONS
export  COMPILER_THUMB_DEFINES
export  COMPILER_INTERWORK_DEFINES
export  SOFTVFPMODE_OPTION
export  VFP2MODE_OPTION
export  RUNTIME_SYMBOL_VISIBILITY_OPTION
export  OUTPUT_OPTION
export  CCFLAGS
export  UNIX_SLASH_FOR_CC_ABS_PATH
export  EXCEPTIONS
export  NO_EXCEPTIONS
export 	VERSION_INFO
export  NO_UNALIGNED_ACCESS
export  DEBUG_FORMAT

# Linker Options
export  LINKER_OUTPUT_OPTION
export  LINKER_DEBUG_OPTION
export  LINKER_NODEBUG_OPTION
export  CODE_SEGMENT_START
export  DATA_SEGMENT_START
export  UNDEFINED_SYMBOL_REF_OPTION
export  LINKER_ENTRY_OPTION
export  START_GROUP_SYMBOL
export  END_GROUP_SYMBOL
export  SHARED_OBJECT_OPTION
export  STDLIB_OPTION
export  STATIC_LIBS_LIST
export  STATIC_LIBS_PATH
export 	STATIC_LIBRARY_PATH
export  STATIC_LIBS
export  RUNTIME_LIBS_LIST
export  SYM_NEW_LIB
export  RUNTIME_LIBS_PATH
export  SYMVER_OPTION
export  SO_NAME_OPTION
export  LINKER_SYMBOLS_MAP_OPTION
export  COMMANDFILE_OPTION
export	VIA_FILE_PREFIX
export	VIA_FILE_SUFFIX
export  SYMBIAN_UREL_LINK_FLAGS
export  SYMBIAN_UDEB_LINK_FLAGS
export  LD_OPTIONS

# Archiver Options
export  ARCHIVER
export  ARCHIVER_CREATE_OPTION
export  AR_OPTIONS

# Compiler Specific Defines
export  COMPILER_DEFINES
export  COMPILER_PLAT

# Platform Specific Defines
export  PLATFORM_DEFINES

# Translator Options
export  TRANASM_FLAGS
export  TRANASM_OUTPUT_OPTION
export  TRANASM_INPUT_OPTION

# Function Call Logger options
export PREINCLUDE_OPTION_FCLOGGER
export FC_LOGGER_OPTION
export FC_LOGGER_DEFINES
export FC_LOGGER_INCLUDE_OPTION
export FC_LOGGER_DICTIONARY_FILE_NAME
export FC_LOGGER_GENERATED_C_FILE_NAME

#OE Options
export OE_OPTIONS
export OE_EXE_LIBS
export OE_EXE_LIBS_WCHAR
export OE_IMPORT_LIBS
export OE_NEW_LIB

#-----------------------------------------------------------------------------------------------------

