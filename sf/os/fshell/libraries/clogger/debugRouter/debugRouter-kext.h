// debugRouter-kext.h
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <u32std.h>

namespace CloggerDebugRouter
	{
	const TInt KCloggerDebugPort = 67;
	
	NONSHARABLE_STRUCT(SCrashDumpArea)
		{
		DChunk* iChunk;
		TBuf8<32> iName;
		TUint32 iReserved[6]; // For use by variant-specific functions
		};

	/* Implementations of these functions should expect to be called in a thread context with the kernel unlocked
	 */
	typedef void (*TRegisterFn)(TAny* /*aAddr*/, TUint /*aSize*/, SCrashDumpArea& /*aCrashDumpArea*/);
	typedef void (*TUnregisterFn)(SCrashDumpArea& /*aCrashDumpArea*/);

	IMPORT_C void DebugPortChanged();
	
	/* If you're going to be calling this, the debug router has to be loaded as a kernel extension.
	 * Call the function after the kext has loaded, and preferably before the user-side cloggerserver has started.
	 * Kernel must be unlocked, no fast mutexes or system lock held, and in a thread context.
	 * 
	 * The functions you pass in must be prepared to get called before this function has returned.
	 */
	IMPORT_C void SetCrashDumpFunctions(TRegisterFn aRegisterFn, TUnregisterFn aUnregisterFn);
	}
