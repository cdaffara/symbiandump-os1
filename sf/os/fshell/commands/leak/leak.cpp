// leak.cpp
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

#include "leak.h"
#include <fshell/common.mmh>
#ifdef FSHELL_QR3_SUPPORT_LOGGINGALLOCATOR
#include <fshell/loggingallocator.h>
#endif

const TInt KMinChunkSize = 4 * 1024;
const TInt KMaxChunkSize = 512 * 1024 * 1024;

CCommandBase* CCmdLeak::NewLC()
	{
	CCmdLeak* self = new(ELeave) CCmdLeak();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdLeak::~CCmdLeak()
	{
#ifdef FSHELL_QR3_SUPPORT_LOGGINGALLOCATOR
	if (iLoggingAllocator)
		{
		// Hmm how do I clean up a logging allocator created with RLoggingAllocator::New()?
		}
#endif
	iChunk.Close();
	if (iChunkHeap)
		{
		iChunkHeap->Reset();
		iChunkHeap->Close();
		}
	}

CCmdLeak::CCmdLeak()
	{
	}

const TDesC& CCmdLeak::Name() const
	{
	_LIT(KName, "leak");
	return KName;
	}

void CCmdLeak::DoRunL()
	{
	if ((iAmount < 0) || (iIncrementAmount < 0))
		{
		User::Leave(KErrArgument);
		}

	if (iArguments.IsPresent(0))
		{
		if (iIncrementAmount == 0)
			{
			iIncrementAmount = iAmount;
			}
		}
	else
		{
		iAmount = KMaxTInt;
		if (iIncrementAmount == 0)
			{
			iIncrementAmount = KMinChunkSize;
			}
		}

	if (iUseLoggingAllocator) iUseHeap = ETrue; // Using the logging allocator implies the --heap option

	if (iUseHeap)
		{
		iChunkHeap = UserHeap::ChunkHeap(NULL, KMinChunkSize, 256*1024*1024);
		if (!iChunkHeap) LeaveIfErr(KErrNoMemory, _L("Couldn't create chunk heap"));
		iAllocatorToUse = iChunkHeap;

#ifdef FSHELL_QR3_SUPPORT_LOGGINGALLOCATOR
		if (iUseLoggingAllocator)
			{
			LeaveIfErr(RLoggingAllocator::New(0, iChunkHeap, iLoggingAllocator), _L("Couldn't create logging allocator"));
			iAllocatorToUse = iLoggingAllocator;
			}
#endif
		}
	else
		{
		if (iIncrementAmount & 4095) LeaveIfErr(KErrArgument, _L("increment-amount must be a multiple of 4096"));
		TInt err = iChunk.CreateLocal(KMinChunkSize, KMaxChunkSize);
		LeaveIfErr(err, _L("Failed to create local chunk"));
		}

	RIoConsoleWriteHandle stdout = Stdout();
	stdout.SetCursorHeight(0);
	TInt err = KErrNone;
	do
		{
		err = LeakStep(Min(iIncrementAmount, iAmount - iCurrentLeak));
		if (iVerbose)
			{
			Write(_L("\r"));
			stdout.ClearToEndOfLine();
			Printf(_L("Allocated %d: %d"), iCurrentLeak, err);
			}
		if (err && iRetry && iIncrementAmount > 4)
			{
			// Keep going with a smaller increment
			err = KErrNone;
			iIncrementAmount /= 2;
			continue;
			}

		if (iRate > 0)
			{
			User::After(iRate * 1000);
			}
		}
		while ((err == KErrNone) && (iCurrentLeak < iAmount));

	if (err == KErrNoMemory)
		{
		Printf(_L("Out of memory. "));
		}
	else if (err)
		{
		Printf(_L("Error: %d. "), err);
		}

	Printf(_L("\r\nLeaked %d bytes. Press any key to exit (and free the memory).\r\n"), iCurrentLeak);

	RIoConsoleReadHandle stdin = Stdin();
	stdin.ReadKey();
	}

void CCmdLeak::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptIncrementAmount, "increment-amount");
	aOptions.AppendIntL(iIncrementAmount, KOptIncrementAmount);

	_LIT(KOptRate, "rate");
	aOptions.AppendUintL(iRate, KOptRate);

	_LIT(KOptUseHeap, "heap");
	aOptions.AppendBoolL(iUseHeap, KOptUseHeap);

	_LIT(KOptRetry, "retry");
	aOptions.AppendBoolL(iRetry, KOptRetry);

#ifdef FSHELL_QR3_SUPPORT_LOGGINGALLOCATOR
	_LIT(KOptUseLoggingAllocator, "logging-allocator");
	aOptions.AppendBoolL(iUseLoggingAllocator, KOptUseLoggingAllocator);
#endif
	}

void CCmdLeak::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgAmount, "amount");
	aArguments.AppendIntL(iAmount, KArgAmount);
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdLeak)
#endif

TInt CCmdLeak::LeakStep(TInt aAmount)
	{
	TInt err = KErrNone;
	if (iAllocatorToUse)
		{
		TAny* cell = iAllocatorToUse->Alloc(aAmount);
		if (!cell) err = KErrNoMemory;
		}
	else
		{
		if (aAmount < 4096) err = KErrNoMemory; // implies we're retrying - unlike the others that keep retrying down to 4 bytes, we have to stop at page granularity
		else err = iChunk.Adjust(iCurrentLeak + aAmount);
		}

	if (!err) iCurrentLeak += aAmount;
	return err;
	}
