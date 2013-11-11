// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// For the Winscw Emulator only, selects between NGA and Non-NGA version of wsgraphicdrawer.
// The default is the non-GCE based version.
// To select the NGA version do one of:
// 1. Add a line to the epoc.ini file in \epoc32\data like this:
// symbian_graphics_use_gce ON
// or
// 2. Start epoc.exe with these parameters, (the "--" IS necessary):
// -Dsymbian_graphics_use_gce=ON --
// or
// 3. epoc.exe can be told to switch to a different initialisation file than epoc.ini, with the -M parameter.
// Progress chaining to the real Wserv is logged in epocwind.out.
// 
//



#include <emulator.h>

#include <e32svr.h>
#include <u32hal.h>


extern "C" {

#include "wsgraphicdrawer_stubs.h"

FARPROC vector[MAX_ORDINAL+1];


#ifdef _DEBUG
void Stop(char* aErrorMessage)
#else
void Stop(char* /*aErrorMessage*/)
#endif
	{
	int err = GetLastError();
#ifdef _DEBUG
	RDebug::Printf("%S, (last error = %i)", aErrorMessage, err);
#endif
	_asm int 3;
	}

void fill_vector(HINSTANCE aDll)
	{
	int i;
	FARPROC address = NULL;
	for (i=1;i<=MAX_ORDINAL;i++)
		{
		address = GetProcAddress(aDll, (LPCSTR)i);
		if (address == NULL)
			{
			Stop("... has too few exported functions");
			}
		vector[i] = address;
		}

	address = GetProcAddress(aDll, (LPCSTR)i);
	if (address != NULL)
		{
		Stop("... has too many exported functions");
		}
	vector[0] = (FARPROC)1;		// initialised
	}

// redirects DLL calls to GCE or non-GCE implementation
void init_vector()
	{
	// ask HAL which configuration to use
	TBool nga = EFalse;
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalBoolProperty,  (TAny*)"symbian_graphics_use_gce",  &nga);
	const char* library = nga ? "wsgraphicdrawer_nga.dll" : "wsgraphicdrawer_nonnga.dll";

#ifdef _DEBUG
	RDebug::Printf("Redirecting wsgraphicdrawer.dll to \"%s\" ...\n", library);
#endif
	
	Emulator::Escape();		// prevent deadlock between EKA2 scheduler and MS kernel
	// try to load selected DLL
	HINSTANCE instance = LoadLibraryA(library);
	Emulator::Reenter();

	if (instance == NULL)
		{
		Stop("... unable to load");
		}
	else
		{
		fill_vector(instance);
#ifdef _DEBUG
		RDebug::Printf("... DLL loaded successfully");
#endif
		}
	}

__declspec(naked) void common_dispatch()
	{
	_asm cmp	dword ptr vector,0		// initialised?
	_asm je  	do_init_vector
call_though_vector:
	_asm jmp	[vector+eax*4]

do_init_vector:
	_asm push	eax
	_asm push	ecx
	_asm push	edx
	_asm call	init_vector
	_asm pop	edx
	_asm pop 	ecx
	_asm pop 	eax
	_asm jmp	call_though_vector
	}

}
