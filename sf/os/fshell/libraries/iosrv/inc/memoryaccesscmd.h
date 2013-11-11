// memoryaccesscmd.h
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
#ifndef MEMORYACCESSCMD_H
#define MEMORYACCESSCMD_H

#include <fshell/ioutils.h>
#include <fshell/common.mmh>

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
#include <fshell/memoryaccess.h>
#endif

namespace IoUtils
	{

/* CMemoryAccessCommandBase
 * This class is intended to be used by commands that can make use of memory access but don't
 * necessarily require it. By deriving from CMemoryAccessCommandBase instead of CCommandBase,
 * you reduce the number of times "#ifdef FSHELL_MEMORY_ACCESS_SUPPORT" needs sprinkling through
 * your code. The only bits you need to guard are the bits that use iMemAccess and that call 
 * LoadMemoryAccessL (although you could trap that instead if you really wanted). No need to 
 * include memoryaccess.h or close iMemAccess, and you can include this header and derive from
 * this class unconditionally.
 */

class CMemoryAccessCommandBase : public CCommandBase
	{
public:
	IMPORT_C CMemoryAccessCommandBase();
	IMPORT_C CMemoryAccessCommandBase(TUint aFlags);

	IMPORT_C void LoadMemoryAccessL();
	IMPORT_C ~CMemoryAccessCommandBase();

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
protected:
	RMemoryAccess iMemAccess;
#else
private:
	// So that we're the same size regardless
	TUint32 iPadding[3];
#endif
	};

	}

#endif // MEMORYACCESSCMD_H
