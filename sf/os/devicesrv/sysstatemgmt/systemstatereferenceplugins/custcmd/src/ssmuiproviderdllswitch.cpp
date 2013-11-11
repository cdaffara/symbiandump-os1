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
// ssmcustomuidswitch.cpp
// 
//

/**
 @file
 @internalComponent
 @released
*/
#include <emulator.h>

extern "C" {

#include "ssmuiproviderdllswitch_stubs.h"

FARPROC vector[MAX_ORDINAL + 1];

void fill_vector(HINSTANCE aDll)
	{
	int i;
	for (i=1; i <= MAX_ORDINAL; i++)
		{
		vector[i] = GetProcAddress(aDll, (LPCSTR)i);
		}
	vector[0] = (FARPROC)1;		// initialised
	}

void init_vector()
	{
	// prevent deadlock with EKA2 scheduler
	__LOCK_HOST;

	HINSTANCE instance;

	// Try loading dll with customised uid data first
	//
	instance = LoadLibraryA("ssmuiproviderdllcustomised.dll");
	if (instance != NULL)
		{
		fill_vector(instance);
		return;
		}

	// Try loading dll with default uid data
	//
	instance = LoadLibraryA("ssmuiproviderdlldefault.dll");
	if (instance != NULL)
		{
		fill_vector(instance);
		return;
		}

	// die
	//
	OutputDebugStringA("Unable to load ssm custom command uid implementation");
	_asm int 3;
	}

__declspec(naked) void common_dispatch()
	{
	_asm cmp	dword ptr vector,0		// initialised?
	_asm jne  	call_though_vector
	_asm push	eax
	_asm push	ecx
	_asm push	edx
	_asm call	init_vector
	_asm pop	edx
	_asm pop 	ecx
	_asm pop 	eax
call_though_vector:
	_asm jmp	[vector+eax*4]
	} //lint !e1065 Suppress Symbol 'common_dispatch(void)' not declared as "C" conflicts
}
