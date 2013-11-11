// proxyallocatorhelper.cpp
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
#include <fshell/heaputils.h>
#include <fshell/common.mmh>
#include <fshell/ltkutils.h>
__ASSERT_COMPILE(sizeof(LtkUtils::RProxyAllocatorHelper) == 11*4);

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
#include <fshell/memoryaccess.h>
#endif

EXPORT_C LtkUtils::RProxyAllocatorHelper::RProxyAllocatorHelper()
	: RAllocatorHelper()
	{
	}

EXPORT_C TInt LtkUtils::RProxyAllocatorHelper::Open(RMemoryAccess& aMem, TUint aThreadId)
	{
	iMemoryAccess = &aMem;
	iThreadId = aThreadId;

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TUint8* allocatorAddress;
	TInt err = iMemoryAccess->GetAllocatorAddress(iThreadId, allocatorAddress);
	if (err == KErrNone)
		{
		if (allocatorAddress == NULL)
			{
			// If the thread has not yet been resumed it's valid for it not to have an allocator yet
			err = KErrNotReady;
			}
		else
			{
			iAllocatorAddress = (TLinAddr)allocatorAddress;
			TInt udeb = EuserIsUdeb();
			if (udeb < 0) return udeb; // error
			err = IdentifyAllocatorType(udeb);
			}
		}
	return err;
#else
	return KErrNotSupported;
#endif
	}

EXPORT_C TInt LtkUtils::RProxyAllocatorHelper::OpenChunkHeap(RMemoryAccess& aMem, TAny* aDChunkPtr)
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	iMemoryAccess = &aMem;

	TChunkKernelInfo chunkInfo;
	TPckg<TChunkKernelInfo> chunkInfoPckg(chunkInfo);
	TInt err = iMemoryAccess->GetObjectInfo(EChunk, (TUint8*)aDChunkPtr, chunkInfoPckg);
	if (err) return err;
	RProcess process;
	err = process.Open(chunkInfo.iControllingOwnerProcessId);
	if (err == KErrNone)
		{
		TPckgBuf<TProcessKernelInfo> processInfo;
		err = iMemoryAccess->GetObjectInfoByHandle(EProcess, RThread().Id(), process.Handle(), processInfo);
		process.Close();
		if (err == KErrNone)
			{
			iThreadId = processInfo().iFirstThreadId;
			return RAllocatorHelper::OpenChunkHeap((TLinAddr)chunkInfo.iBase, chunkInfo.iMaxSize);
			}
		}
	return err;
#else
	(void)aMem;
	(void)aDChunkPtr;
	return KErrNotSupported;
#endif
	}

TInt LtkUtils::RProxyAllocatorHelper::ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TThreadMemoryAccessParams params;
	params.iId = iThreadId;
	params.iAddr = (TUint8*)aLocation;
	params.iSize = aSize;
	TPtr8 ptr((TUint8*)aResult, aSize, aSize);
	return iMemoryAccess->GetThreadMem(params, ptr);
#else
	return KErrNotSupported;
#endif
	}

TInt LtkUtils::RProxyAllocatorHelper::WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize)
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	const TPtrC8 ptr((const TUint8*)aData, aSize);
	return iMemoryAccess->WriteMem(iThreadId, ptr, (TAny*)aLocation);
#else
	return KErrNotSupported;
#endif
	}

TInt LtkUtils::RProxyAllocatorHelper::TryLock()
	{
	return KErrNotSupported;
	}

void LtkUtils::RProxyAllocatorHelper::TryUnlock()
	{
	// Not supported
	}

EXPORT_C void LtkUtils::RProxyAllocatorHelper::Close()
	{
	iMemoryAccess = NULL;
	iThreadId = 0;
	RAllocatorHelper::Close();
	}
