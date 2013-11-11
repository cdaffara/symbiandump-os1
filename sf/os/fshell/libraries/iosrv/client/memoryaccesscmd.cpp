// memoryaccesscmd.cpp
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
#include <fshell/memoryaccesscmd.h>

using namespace IoUtils;

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
__ASSERT_COMPILE(sizeof(RMemoryAccess) == sizeof(TUint32) * 3);
#endif

__ASSERT_COMPILE(sizeof(CMemoryAccessCommandBase) == 62*4); // Otherwise CCommandBase has changed or RMemoryAccess has

EXPORT_C CMemoryAccessCommandBase::CMemoryAccessCommandBase()
	: CCommandBase()
	{
	}

EXPORT_C CMemoryAccessCommandBase::CMemoryAccessCommandBase(TUint aFlags)
	: CCommandBase(aFlags)
	{
	}

EXPORT_C void CMemoryAccessCommandBase::LoadMemoryAccessL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	__ASSERT_COMPILE(_FOFF(CMemoryAccessCommandBase, iMemAccess) == 59*4); // Otherwise something strange has gone wrong in CCommandBase
	if (iMemAccess.Handle()) return; // Already open

	TInt err = RMemoryAccess::LoadDriver();
	if (err != KErrAlreadyExists)
		{
		LeaveIfErr(err, _L("Couldn't load %S.LDD"), &KMemoryAccessName);
		}
	LeaveIfErr(iMemAccess.Open(), _L("Couldn't open RMemoryAccess"));
#else
	LeaveIfErr(KErrNotSupported, _L("Can't load memoryaccess because FSHELL_MEMORY_ACCESS_SUPPORT is not defined");
#endif
	}

EXPORT_C CMemoryAccessCommandBase::~CMemoryAccessCommandBase()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	iMemAccess.Close();
	// Don't unload memaccess, we want to keep loaded so it can monitor stuff
#endif
	}
