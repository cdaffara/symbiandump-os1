// LoggingAllocator.h
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
#ifndef LOGGING_ALLOCATOR_H
#define LOGGING_ALLOCATOR_H

#include <e32cmn.h>
#include <fshell/clogger.h>
#include <fshell/heaputils.h>
using LtkUtils::RAllocatorHelper;

NONSHARABLE_CLASS(RLoggingAllocator) : public RAllocator
	{
public:
	IMPORT_C static TInt Install();
	IMPORT_C static TInt Install(TUint aFlags);
	IMPORT_C static TInt Uninstall();

	// These don't require you to actually link against RLoggingAllocator
	inline static TInt Install_WeakLink(TUint aFlags=0);
	inline static TInt Uninstall_WeakLink();

	// Doesn't require the allocator to be installed, just dumps a stacktrace frame for the given cell
	static IMPORT_C void StaticTraceAlloc(RAllocator* aAllocator, TAny* aCellPtr, TInt aRequestedSize);
	static IMPORT_C void StaticTraceFree(RAllocator* aAllocator, TAny* aCellPtr);

	// Create a new logging allocator without changing the current User::Allocator
	IMPORT_C static TInt New(TUint aFlags, RAllocator* aOrigAllocator, RLoggingAllocator*& aResult);

	enum TFlags
		{
		EScribbleFrees = 1,
		EDeferFree = 2,
		EDeferFreeAndCheckScribbles = EDeferFree | EScribbleFrees,
		EOldFormatLogging = 4, // Use Symbian-defined BTrace::EAlloc and BTrace::ETest1 rather than the new range atrace defines extending THeap
		};
	
	static const TInt KTempDisableLogging = 0x10286F5E;

	static TAny* DisableLogging()
		{
		TAny* result = NULL;
		User::Allocator().DebugFunction(KTempDisableLogging, &result);
		return result;
		}

	static void RestoreLogging(TAny* aAllocator)
		{
		if (aAllocator)
			{
			User::SwitchAllocator((RAllocator*)aAllocator);
			}
		}

protected:
	TAny* Alloc(TInt aSize);
	void Free(TAny* aPtr);
	TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode=0);
	TInt AllocLen(const TAny* aCell) const;
	TInt Compress();
	void Reset();
	TInt AllocSize(TInt& aTotalAllocSize) const;
	TInt Available(TInt& aBiggestBlock) const;
	TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);

private:
	RLoggingAllocator(TUint aFlags);
	static TBool HeavenWalk(LtkUtils::RAllocatorHelper& aHelper, TAny* aPtr, RAllocatorHelper::TCellType aType, TLinAddr aCell, TInt aLen);

	static TBool TraceExistingAllocs(TAny* aContext, RAllocatorHelper::TCellType aType, TLinAddr aCell, TInt aLen);
	inline void TraceAlloc(RAllocator* aAllocator, TAny* aCellPtr, TInt aRequestedSize);
	inline void TraceFree(RAllocator* aAllocator, TAny* aCellPtr, TBool aCellIsAlreadyFreed=EFalse);
	inline void TraceRealloc(TAny *aNewPtr, TAny* aOldPtr, TInt aNewRequestedSize);
	void DoTraceAllocEvent(RAllocator* aAllocator, TAny* aCellPtr, TInt aEvent, TInt aRequestedSize, TAny* aOldPtr=NULL); // RequestedSize only relevant for aEvent==EHeapAlloc or EHeapReAlloc, aOldPtr only for realloc
	void CheckDeferredFrees();
	void Destroy();
	virtual ~RLoggingAllocator(); // Made this virtual to shut up GCCE (even GCC 4 complains about this)

public:
	RAllocator* iA; // The original allocator

private:
	TInt iBreakOnAllocCount; // Useful when running in a debugger
#ifndef __KERNEL_MODE__
	RLibrary iLib; // Needed to keep the plugin DLL loaded past the time when ecom is cleaned up
#endif
	TUint iFlags;
	RFastLock iLock; // For locking around iFlags and iDeferredFrees
	struct SDeferredFreeCell { void* iPtr; TInt iLen; };
	RArray<SDeferredFreeCell> iDeferredFrees;
	TUint iPid;
	RAllocatorHelper iHelper;
	};

inline TInt RLoggingAllocator::Install_WeakLink(TUint aFlags)
	{
	const TInt KInstallOrdinal = 5; // Happily this is the same on bwins and eabi (and probably bmarm if I get round to rebuilding it)
	RLibrary lib;
	TInt err = lib.Load(_L("loggingallocator"));
	if (err) return err;

	typedef TInt (*InstallFn)(TUint);
	InstallFn InstallLoggingAllocator = (InstallFn)lib.Lookup(KInstallOrdinal);
	if (!InstallLoggingAllocator) err = KErrBadLibraryEntryPoint;

	if (!err) err = InstallLoggingAllocator(aFlags);
	lib.Close(); // If the allocator installed itself, it makes sure to keep itself loaded via an RLibrary of its own
	return err;
	}

inline TInt RLoggingAllocator::Uninstall_WeakLink()
	{
	const TInt KUninstallOrdinal = 2; // Happily this is the same on bwins and eabi (and probably bmarm if I get round to rebuilding it)
	RLibrary lib;
	TInt err = lib.Load(_L("loggingallocator"));
	if (err) return err;

	typedef TInt (*UninstallFn)();
	UninstallFn UninstallLoggingAllocator = (UninstallFn)lib.Lookup(KUninstallOrdinal);
	if (!UninstallLoggingAllocator) err = KErrBadLibraryEntryPoint;

	if (!err) err = UninstallLoggingAllocator();
	lib.Close(); // If the allocator installed itself, it makes sure to keep itself loaded via an RLibrary of its own
	return err;
	}

#endif
