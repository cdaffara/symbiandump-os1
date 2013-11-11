# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# /epoc32/tools/makefile_templates/tools/x86tool.mk
# SBSv2 issues and differences from SBSv1
# 1/ The shell is different
# 2/ The msys shell can't handle absolute paths from root without the drive letter e.g. it can't find /epoc32 but it can find x:/epoc32
# workaround is to go back to TO_ROOT
# 3/ The current directory is at the bld.inf rather than /epoc32/build
# 4/ Backslash is problematic
# 5/ Path variables include the drive letter x: which causes problems for make when used in targets due to :
# 6/ TO_ROOT is not defined
# 7/ Some tool compatibility is problematic e.g. createdir doesn't work with paths containing forward slash
# 
#

# If this environment variable is defined we're definitely building for win32
ifdef ComSpec
WIN32:=1
endif

space:=
space+=
LETTERS:=a b c d e f g h i j k l m n o p q r s t u v w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z

# If win32 isn't defined yet - then we're probably building on sbsv2
# So see if EPOCROOT starts with a drive letter to work out if we're building for win32
ifndef WIN32
  ifneq ($(subst $(space),,$(foreach let,$(LETTERS),$(if $(findstring $(let):,$(EPOCROOT)),1,))),)
    WIN32:=1
  endif
endif

# To guarantee there is a slash at the end of EPOCROOT in case there is not.
# This is needed to ensure compatibility with SBSv1.
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

# Get standard shell definitions
include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

# Raptor includes the drive letter on paths - this makefile can't handle the drive letter
# If we don't do this we get "multiple target patterns" errors in sbsv2
TMPROOT:=$(strip $(foreach let,$(LETTERS),$(if $(findstring $(let):,$(EPOCROOT)),$(subst $(let):,,$(EPOCROOT)))))
ifneq ($(TMPROOT),)
  EPOCROOT:=$(TMPROOT)
endif
TMPROOT:=$(strip $(foreach let,$(LETTERS),$(if $(findstring $(let):,$(EXTENSION_ROOT)),$(subst $(let):,,$(EXTENSION_ROOT)))))
ifneq ($(TMPROOT),)
  EXTENSION_ROOT:=$(TMPROOT)
endif

# Make up for raptor not defining TO_ROOT properly
# This might get broken if they change the current directory (which should really be in /epoc32/build)
# We're using TO_ROOT to handle the fact that the msys shell doesn't seem to handle absolute paths
ifdef WIN32
  ifeq ($(TO_ROOT),)
    TMPROOT:=$(strip $(foreach let,$(LETTERS),$(if $(findstring $(let):,$(TO_BLDINF)),$(subst $(let):,,$(TO_BLDINF)))))
    TO_ROOT:=.$(subst $(space),,$(foreach word,$(subst /,$(space),$(TMPROOT)),/..))
  endif
endif

# Handle inconsistent case for build variant
CFG_PATH:=$(CFG)
ifeq ($(CFG_PATH),DEB)
  CFG_PATH:=deb
endif
ifeq ($(CFG_PATH),REL)
  CFG_PATH:=rel
endif

# Set the following to enable code coverage stats
CODE_COVERAGE:=

# Build
ifdef WIN32
  TARGET_DIR_REL:=$(EPOCROOT)epoc32/release/tools2/rel/
  TARGET_DIR_DEB:=$(EPOCROOT)epoc32/release/tools2/deb/
  GCOVDIR:=$(if $(CODE_COVERAGE),$(EPOCROOT)epoc32/gcc_mingw/libgcc/mingw32/3.4.5/)
else
  TARGET_DIR_REL:=$(EPOCROOT)epoc32/release/tools2/linux-i386/rel/
  TARGET_DIR_DEB:=$(EPOCROOT)epoc32/release/tools2/linux-i386/deb/
endif

# The root to the source code - paths are relative to bld.inf if NOT specified
# If SOURCE_ROOT specified then it's relative to EPOCROOT
ifndef SOURCE_ROOT
  ROOT:=$(subst \,/,$(EXTENSION_ROOT))
  OBJSUBST:=/
else
  ROOT:=$(EPOCROOT)$(subst $(space)/,/,$(SOURCE_ROOT))
  OBJSUBST:=$(EPOCROOT)
endif

# ***
# STATIC LIBRARY
#
ifeq ($(TARGET_TYPE),lib)
  TARGET_FULLNAME_REL:=$(TARGET_DIR_REL)lib$(TARGET_NAME)$(if $(findstring .,$(TARGET_NAME)),,.a)
  TARGET_FULLNAME_DEB:=$(TARGET_DIR_DEB)lib$(TARGET_NAME)$(if $(findstring .,$(TARGET_NAME)),,.a)

  ifeq ($(CFG_PATH),deb)
    TARGET_LIB:=$(TARGET_FULLNAME_DEB)
  endif
  ifeq ($(CFG_PATH),rel)
    TARGET_LIB:=$(TARGET_FULLNAME_REL)
  endif
  TARGET_LIB?=$(TARGET_FULLNAME_REL) $(TARGET_FULLNAME_DEB)
endif

# ***
# EXE
#
ifeq ($(TARGET_TYPE),exe)
  TARGET_FULLNAME_REL:=$(TARGET_DIR_REL)$(TARGET_SUB_DIR)$(TARGET_NAME)$(if $(findstring .,$(TARGET_NAME)),,$(if $(WIN32),.exe))
  TARGET_FULLNAME_DEB:=$(TARGET_DIR_DEB)$(TARGET_SUB_DIR)$(TARGET_NAME)$(if $(findstring .,$(TARGET_NAME)),,$(if $(WIN32),.exe))

  ifeq ($(CFG_PATH),deb)
    TARGET_BLD:=$(TARGET_FULLNAME_DEB)
  endif
  ifeq ($(CFG_PATH),rel)
    TARGET_BLD:=$(TARGET_FULLNAME_REL)
  endif
  TARGET_BLD?=$(TARGET_FULLNAME_REL) $(TARGET_FULLNAME_DEB)

  LIBS+=symexestub
  ifdef STLPORT
    LIBS+=-lstlport.5.1
  endif
endif

# ***
# DLL/SO
#
ifeq ($(TARGET_TYPE),dll)

  TARGET_FULLNAME_REL:=$(TARGET_DIR_REL)$(TARGET_SUB_DIR)lib$(TARGET_NAME)$(if $(findstring .,$(TARGET_NAME)),,$(if $(WIN32),.dll,.so))
  TARGET_FULLNAME_DEB:=$(TARGET_DIR_DEB)$(TARGET_SUB_DIR)lib$(TARGET_NAME)$(if $(findstring .,$(TARGET_NAME)),,$(if $(WIN32),.dll,.so))

  ifeq ($(CFG_PATH),deb)
    TARGET_BLD:=$(TARGET_FULLNAME_DEB)
  endif
  ifeq ($(CFG_PATH),rel)
    TARGET_BLD:=$(TARGET_FULLNAME_REL)
  endif
  TARGET_BLD?=$(TARGET_FULLNAME_REL) $(TARGET_FULLNAME_DEB)

  ifdef STLPORT
    LIBS+=-lstlport.5.1
  endif
endif

# Pick up MINGW compiler from epoc32 on Windows
ifdef WIN32
  AR:=$(EPOCROOT)epoc32/gcc_mingw/bin/ar
  CXX:=$(EPOCROOT)epoc32/gcc_mingw/bin/g++
endif

# Product include files are different for S60
ifdef S60_BUILD
  PRODUCT_INCLUDE:=$(EPOCROOT)epoc32/include/oem/bldvariant.hrh
else
  PRODUCT_INCLUDE:=$(EPOCROOT)epoc32/include/variant/Symbian_OS.hrh
endif

# Define macros we need
CXXDEFS_COMMON:=$(foreach def,$(MACROS),-D$(def)) -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__X86__ -D_UNICODE -D__SUPPORT_CPP_EXCEPTIONS__ -D__TOOLS2__ -D'__PRODUCT_INCLUDE__="$(PRODUCT_INCLUDE)"'
CXXDEFS_DEB:=$(CXXDEFS_COMMON) -D_DEBUG
CXXDEFS_REL:=$(CXXDEFS_COMMON)

# Setup the command line options for the compiler
PREINC=$(EPOCROOT)epoc32/include/x86tool/x86tool.h
CXXOPT_DEB:=-fshort-wchar -x c++ -O0 -g3 -Wall -c -fmessage-length=0 -include $(PREINC) -masm=intel
OPTIMISE:=-fdefer-pop -fmerge-constants -fthread-jumps -floop-optimize -fif-conversion -fif-conversion2 -fguess-branch-probability -fcprop-registers -fforce-mem -foptimize-sibling-calls -fstrength-reduce -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop -frerun-loop-opt -fgcse -fgcse-lm -fgcse-sm -fgcse-las -fdelete-null-pointer-checks -fexpensive-optimizations -fregmove -fschedule-insns -fschedule-insns2 -fsched-interblock -fsched-spec -fcaller-saves -fpeephole2 -freorder-blocks -freorder-functions -fstrict-aliasing -funit-at-a-time -falign-functions -falign-jumps -falign-loops -falign-labels -fcrossjumping
CXXOPT_REL:=-fshort-wchar -x c++ -Wall -c -fmessage-length=0 -include $(PREINC) $(if $(CODE_COVERAGE),-O0,$(OPTIMISE)) -masm=intel

# Allow specification of additional build include file
ifdef BUILDINC
  CXXOPT_DEB+= -include $(BUILDINC)
  CXXOPT_REL+= -include $(BUILDINC)
endif

# Extra options needed for cia files
ASMOPT:=-fomit-frame-pointer

# Linker options for DLL
ifndef DLL_WIN_LINKER_OPTS
  DLL_WIN_LINKER_OPTS:= $(if $(CODE_COVERAGE),-lgcov) -Wl,-export-all-symbols
endif
ifndef DLL_LIN_LINKER_OPTS
  DLL_LIN_LINKER_OPTS:= -Wl,-export-all-symbols $(if $(CODE_COVERAGE),-lgcov) -ldl
endif

# Source files to scan for in a directory
# Note that CPP and cpp will match on the same files - so a sort is necessary on wildcard results
SOURCE_EXT:=CPP cpp c cia

# Source code assumed to be all cpp/cia files in supplied directories
SOURCE_FILES:=$(foreach dir,$(SOURCE_DIRS),$(sort $(foreach ext,$(SOURCE_EXT),$(wildcard $(ROOT)/$(dir)/*.$(ext))))) \
  $(foreach src,$(SOURCE),$(ROOT)/$(if $(SOURCE_FOLDER),$(SOURCE_FOLDER)/)$(src)) \
  $(foreach src,$(SOURCE_MOD),$(subst \,/,$(EXTENSION_ROOT))/$(src)) \
  $(foreach id,$(SOURCE_IDS),$(foreach src,$($(id)_SOURCE),$(subst $(space)/,/,$(if $($(id)_SOURCE_ROOT),$(EPOCROOT)$($(id)_SOURCE_ROOT),$(ROOT))/$(src))))

# Include folders
CXXINC:=$(foreach inc,$(INCLUDES) $(SOURCE_DIRS),-I$(ROOT)/$(inc)) \
  $(foreach inc,$(SYS_INCLUDES),-I$(EPOCROOT)$(inc)) -I$(ROOT) \
  $(foreach id,$(SOURCE_IDS),$(foreach inc,$($(id)_SOURCE_INC),-I$(subst $(space)/,/,$(if $($(id)_SOURCE_ROOT),$(EPOCROOT)$($(id)_SOURCE_ROOT),$(ROOT)))/$(inc)))

# Add standard include paths?
ifndef NO_STD_INCLUDE
  CXXINC+= -I$(EPOCROOT)epoc32/include/x86tool -I$(EPOCROOT)epoc32/include
  ifdef S60_BUILD
    CXXINC+= -I$(EPOCROOT)epoc32/include/oem
  endif
endif

# Support for building JNI
ifdef JAVA_JNI
  CXXINC+= -I"$(JAVA_HOME)/include"
  ifdef WIN32
    CXXINC+= -I"$(JAVA_HOME)/include/win32"
  else
    CXXINC+= -I"$(JAVA_HOME)/include/linux"
  endif
endif


# STL Port support needed? Note STL and Symbian won't mix!
ifdef STLPORT
  CXXINC+= -I$(EPOCROOT)epoc32/include/tools/stlport 
endif

# ***
# DEBUG
#

# Object files are the same name as the source files with a .o extension
OBJECTFILES_DEB:=$(foreach src,$(SOURCE_FILES),deb$(src).o)

# Compile
$(OBJECTFILES_DEB) : $(PREINC) $(SOURCE_FILES)
	@echo ***
	@echo Making: $@
	$(call createdir,$(dir $@))
	$(CXX) $(CXXDEFS_DEB) $(CXXINC) -I$(subst deb$(OBJSUBST),$(OBJSUBST),$(dir $@)) $(CXXOPT_DEB) $(if $(findstring .cia,$@),$(ASMOPT),$(if $(CODE_COVERAGE),-fprofile-arcs -ftest-coverage)) -o$@ $(subst deb$(OBJSUBST),$(OBJSUBST),$(basename $@))

# Link
$(TARGET_FULLNAME_DEB) : $(OBJECTFILES_DEB) $(foreach lib,$(LIBS),$(TARGET_DIR_DEB)lib$(lib).a) $(foreach lib,$(SHARED_LIBS),$(TARGET_DIR_DEB)lib$(lib)$(if $(WIN32),.a,.so))
ifeq ($(TARGET_TYPE),lib)
	@echo ***
	@echo Creating lib: $@
	$(AR) -r $(TARGET_FULLNAME_DEB) $(OBJECTFILES_DEB)
endif
ifeq ($(TARGET_TYPE),exe)
	@echo ***
	@echo Creating exe: $@
	$(CXX) -L$(TARGET_DIR_DEB) $(if $(GCOVDIR),-L$(GCOVDIR)) \
		-o$(TARGET_FULLNAME_DEB) $(OBJECTFILES_DEB) \
		$(foreach lib,$(LIBS) $(SHARED_LIBS), -l$(lib)) \
		$(if $(CODE_COVERAGE),-lgcov) $(if $(WIN32),,-ldl)
endif
ifeq ($(TARGET_TYPE),dll)
  ifdef WIN32
	@echo ***
	@echo Creating Windows dll: $@
	$(CXX) -L$(TARGET_DIR_DEB) $(if $(GCOVDIR),-L$(GCOVDIR)) \
		-shared -o$(TARGET_FULLNAME_DEB) $(OBJECTFILES_DEB) \
		$(foreach lib,$(LIBS) $(SHARED_LIBS), -l$(lib)) \
		-Wl,-out-implib,$(@D)/$(basename $(@F)).a \
		$(DLL_WIN_LINKER_OPTS)
  else
	@echo ***
	@echo Creating Linux shared object: $@
	$(CXX) -L$(TARGET_DIR_DEB) \
		-shared -o$(TARGET_FULLNAME_DEB) $(OBJECTFILES_DEB) \
		$(foreach lib,$(LIBS) $(SHARED_LIBS), -l$(lib)) \
		$(DLL_LIN_LINKER_OPTS)
  endif
endif

# ***
# RELEASE
#

# Object files are the same name as the source files with a .o extension
OBJECTFILES_REL:=$(foreach src,$(SOURCE_FILES),rel$(src).o)

# Compile
$(OBJECTFILES_REL) : $(PREINC) $(SOURCE_FILES)
	@echo ***
	@echo Making: $@
	$(call createdir,$(dir $@))
	$(CXX) $(CXXDEFS_REL) $(CXXINC) -I$(subst rel$(OBJSUBST),$(OBJSUBST),$(dir $@)) $(CXXOPT_REL) $(if $(findstring .cia,$@),$(ASMOPT),$(if $(CODE_COVERAGE),-fprofile-arcs -ftest-coverage)) -o$@ $(subst rel$(OBJSUBST),$(OBJSUBST),$(basename $@))

# Link
$(TARGET_FULLNAME_REL) : $(OBJECTFILES_REL) $(foreach lib,$(LIBS),$(TARGET_DIR_REL)lib$(lib).a) $(foreach lib,$(SHARED_LIBS),$(TARGET_DIR_REL)lib$(lib)$(if $(WIN32),.a,.so))
ifeq ($(TARGET_TYPE),lib)
	@echo ***
	@echo Creating lib: $@
	$(AR) -r $(TARGET_FULLNAME_REL) $(OBJECTFILES_REL)
endif
ifeq ($(TARGET_TYPE),exe)
	@echo ***
	@echo Creating exe: $@
	$(CXX) -L$(TARGET_DIR_REL) $(if $(GCOVDIR),-L$(GCOVDIR)) \
		-o$(TARGET_FULLNAME_REL) $(OBJECTFILES_REL) \
		$(foreach lib,$(LIBS) $(SHARED_LIBS), -l$(lib)) \
		$(if $(CODE_COVERAGE),-lgcov) $(if $(WIN32),,-ldl)
endif
ifeq ($(TARGET_TYPE),dll)
  ifdef WIN32
	@echo ***
	@echo Creating Windows dll: $@	
	$(CXX) -L$(TARGET_DIR_REL) $(if $(GCOVDIR),-L$(GCOVDIR)) \
		-shared -o$(TARGET_FULLNAME_REL) $(OBJECTFILES_REL) \
		$(foreach lib,$(LIBS) $(SHARED_LIBS), -l$(lib)) \
		-Wl,-out-implib,$(@D)/$(basename $(@F)).a \
		$(DLL_WIN_LINKER_OPTS)
  else
	@echo ***
	@echo Creating Linux shared object: $@
	$(CXX) -L$(TARGET_DIR_REL) \
		-shared -o$(TARGET_FULLNAME_REL) $(OBJECTFILES_REL) \
		$(foreach lib,$(LIBS) $(SHARED_LIBS), -l$(lib)) \
		$(DLL_LIN_LINKER_OPTS)
  endif
endif

do_nothing:

#
# The targets invoked by abld...
#

MAKMAKE : do_nothing
FREEZE : do_nothing
RESOURCE : do_nothing
CLEANLIB : do_nothing
FINAL : do_nothing

SAVESPACE :
ifeq ($(CFG_PATH),deb)
	$(call remove,$(OBJECTFILES_DEB))
endif
ifeq ($(CFG_PATH),rel)
	$(call remove,$(OBJECTFILES_REL))
endif

LIB : $(TARGET_LIB)

BLD : $(TARGET_BLD)

CLEAN :
	$(call remove,$(foreach file,$(TARGET_LIB) $(TARGET_BLD),$(TO_ROOT)$(file)))
ifeq ($(CFG_PATH),deb)
	$(call remove,$(OBJECTFILES_DEB))
endif
ifeq ($(CFG_PATH),rel)
	$(call remove,$(OBJECTFILES_REL))
endif

RELEASABLES :
	@echo $(TARGET_LIB) $(TARGET_BLD)
