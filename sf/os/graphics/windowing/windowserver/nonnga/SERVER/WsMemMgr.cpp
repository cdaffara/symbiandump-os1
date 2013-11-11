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
//

#include "WsMemMgr.h"
#include "inifile.h"
#include "panics.h"
#include "wstop.h"

CWsMemoryManager * CWsMemoryManager::iStatic = NULL;

CWsMemoryManager * CWsMemoryManager::Static()
	{
	return iStatic;
	}

CWsMemoryManager * CWsMemoryManager::NewLC()
	{
	CWsMemoryManager * self = new (ELeave) CWsMemoryManager;
	CleanupStack::PushL(self);
	self->ConstructL();
	iStatic = self;
	return iStatic;
	}

CWsMemoryManager::CWsMemoryManager()
	{
	iImpl = User::SwitchAllocator(this);
	}
	
CWsMemoryManager::~CWsMemoryManager()
	{
	WS_ASSERT_ALWAYS(this == User::SwitchAllocator(iImpl),EWsPanicMemoryManager);
	iStatic = 0;
	if (iReserve!=NULL)
		{
		Free(iReserve);
		iReserve = NULL;
		}	
	}

void CWsMemoryManager::ConstructL()
	{
	_LIT(KMemMgrReserve, "MEMORYRESERVE");
	const TInt KDefaultMemMgrReserve = 1024;
	
	if (!WsIniFile->FindVar(KMemMgrReserve, iReserveSize))
		iReserveSize = KDefaultMemMgrReserve;

	if (iReserveSize > 0)
		iReserve = Alloc(iReserveSize);
	}

/**
Implementing RAllocator
*/

/**
Alloc and ReAlloc attempt to obtain memory through CWsTop::ReleaseMemory when they run low.
ReleaseMemory looks for blocks of memory that the window server doesn't need urgently and frees
them.
*/
TAny* CWsMemoryManager::Alloc(TInt aSize)
	{
	TBool keepTrying = ETrue;
	do
		{
		if(iReleasing)
			return iImpl->Alloc(aSize); //fallback on RAllocator
		
		if(TAny* ret = iImpl->Alloc(aSize)) //normal case
			return ret;
		
		if(iReserveEnabled && iReserve && (aSize < iReserveSize))
			{
			Free(iReserve);
			iReserve = NULL;
			}
		else
			{
			iReleasing = ETrue;
			keepTrying = CWsTop::ReleaseMemory();
			if(keepTrying)
 				{
 				const TInt reclaimed = Compress(); //Try to give back to the OS
 				}			
			iReleasing = EFalse;
			}
		
		} while(keepTrying);

	return NULL;
	}

TAny* CWsMemoryManager::ReAlloc(TAny* aPtr, TInt aSize, TInt aMode)
	{
	TBool keepTrying = ETrue;
	do
		{
		if(iReleasing)
			return iImpl->ReAlloc(aPtr, aSize, aMode); //fallback on RAllocator
		
		if(TAny* ret = iImpl->ReAlloc(aPtr, aSize, aMode)) //normal case
			return ret;
		
		if(iReserveEnabled && iReserve && (aSize < iReserveSize))
			{
			Free(iReserve);
			iReserve = NULL;
			}
		else
			{
			iReleasing = ETrue;
			keepTrying = CWsTop::ReleaseMemory();
			if(keepTrying)
 				{
 				const TInt reclaimed = Compress(); //Try to give back to the OS
 				}	
			iReleasing = EFalse;
			}

		} while(keepTrying);

	return NULL;
	}

/**
The rest of these functions just call the default implementation
*/
void CWsMemoryManager::Free(TAny* aPtr)
	{
	return iImpl->Free(aPtr);
	}

TInt CWsMemoryManager::AllocLen(const TAny* aCell) const
	{
	return iImpl->AllocLen(aCell);
	}

TInt CWsMemoryManager::Compress()
	{
	return iImpl->Compress();
	}

void CWsMemoryManager::Reset()
	{
	iImpl->Reset();
	}

TInt CWsMemoryManager::AllocSize(TInt& aTotalAllocSize) const
	{
	return iImpl->AllocSize(aTotalAllocSize);
	}

TInt CWsMemoryManager::Available(TInt& aBiggestBlock) const
	{
	return iImpl->Available(aBiggestBlock);
	}

TInt CWsMemoryManager::DebugFunction(TInt aFunc, TAny* a1, TAny* a2)
	{
	return iImpl->DebugFunction(aFunc,a1,a2);
	}

TInt CWsMemoryManager::Count() const
	{
	return iImpl->Count();
	}
/** This is a fairly dumb way to enable and disable the reserve, but we normally
get away with it because wserv is high priority.  A better approach would be to
use placement new into the reserve memory and manage it directly.  This would also
allow us to track misbehaving code which allocated during OOM drawing and didn't
free at the end.
*/
void CWsMemoryManager::EnableReserve()
	{
	WS_ASSERT_DEBUG(!iReserveEnabled, EWsPanicMemoryManager);
	iReserveEnabled = ETrue;
	}

void CWsMemoryManager::DisableReserve()
	{
	WS_ASSERT_DEBUG(iReserveEnabled, EWsPanicMemoryManager);
	iReserveEnabled = EFalse;
	if((!iReserve) && (iReserveSize > 0))
		iReserve = Alloc(iReserveSize);
	}
