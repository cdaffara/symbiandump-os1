// breakpoint.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/ltkutils.h>

#ifdef __WINS__

EXPORT_C void LtkUtils::Breakpoint()
	{
	__BREAKPOINT()
	}

#elif defined(__GCCE__) || defined(__GCC32__)

// This only gets compiled for GCC-based platforms, hence why it uses __attribute__ and not just __NAKED__
#include <u32std.h>

EXPORT_C __attribute__((__naked__)) void LtkUtils::Breakpoint()
	{
	asm(".word 0xE7F123F4 ");
	__JUMP(,lr);
	}

#endif

EXPORT_C TLinAddr LtkUtils::BreakpointAddr()
	{
	return (TLinAddr)&Breakpoint;
	}

void BreakpointCleanupItem(TAny*)
	{
	LtkUtils::Breakpoint();
	}

EXPORT_C void LtkUtils::BreakpointPushL()
	{
	CleanupStack::PushL(TCleanupItem(&BreakpointCleanupItem, NULL));
	}
