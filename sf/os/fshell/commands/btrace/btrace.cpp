// btrace.cpp
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

#include "btrace.h"

#if defined(FSHELL_MEMORY_ACCESS_SUPPORT)
#define TEXT_TRACE_MODE_SUPPORTED
#endif

const TInt KMaxCategories = 256;
const TUint KPrimingStopped = 0x102864BC;



CCommandBase* CCmdBtrace::NewLC()
	{
	CCmdBtrace* self = new(ELeave) CCmdBtrace();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCmdBtrace::~CCmdBtrace()
	{
	iPrimaryFilters.Close();
	iSecondaryFilters.Close();
	iBtrace.Close();
	}

CCmdBtrace::CCmdBtrace()
	: iThreshold(50)
	{
	}

void CCmdBtrace::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CCmdBtrace::Name() const
	{
#ifdef BUILDING_ATRACE
	_LIT(KName, "atrace");
#else
	_LIT(KName, "btrace");
#endif
	return KName;
	}

#define CASE_LIT(x) case BTrace:: x : { _LIT(KName, #x); return &KName; }
#define CASE_REALLY_LIT(x) case x : { _LIT(KName, #x); return &KName; }

const TDesC* BtraceCategoryName(TInt aCategory)
	{
	switch (aCategory)
		{
		CASE_LIT(ERDebugPrintf)
		CASE_LIT(EKernPrintf)
		CASE_LIT(EPlatsecPrintf)
		CASE_LIT(EThreadIdentification)
		CASE_LIT(ECpuUsage)
		CASE_LIT(EKernPerfLog)
		CASE_LIT(EClientServer)
		CASE_LIT(ERequests)
		CASE_LIT(EChunks)
		CASE_LIT(ECodeSegs)
		CASE_LIT(EPaging)
		CASE_LIT(EThreadPriority)
		CASE_LIT(EPagingMedia)
		CASE_LIT(EKernelMemory)
		CASE_LIT(EHeap)
		CASE_LIT(EMetaTrace)
		CASE_LIT(ERamAllocator)
		CASE_LIT(EFastMutex)
		CASE_LIT(EProfiling)
		CASE_REALLY_LIT(ExtraBTrace::EPubSub)
		CASE_REALLY_LIT(ExtraBTrace::EFfsTrace)
		CASE_LIT(EPlatformSpecificFirst)
		CASE_LIT(EPlatformSpecificLast)
		CASE_LIT(ESymbianExtentionsFirst)
		CASE_LIT(ESymbianExtentionsLast)
		CASE_LIT(ETest1)
		CASE_LIT(ETest2)
		default:
			{
			_LIT(KUnknown, "?");
			return &KUnknown;
			}
		}
	}

void CCmdBtrace::DoRunL()
	{
#ifdef TEXT_TRACE_MODE_SUPPORTED
	if (iOptions.IsPresent(&iTextTraceMode))
		{
		LoadMemoryAccessL();
		const TInt ESerialOutMask = 3; // As per kernel.h
		TInt err = iMemAccess.SetTextTraceMode(iTextTraceMode, ESerialOutMask);
		LeaveIfErr(err, _L("Couldn't set text trace mode"));
		}
#endif

	TInt i;
	LeaveIfErr(iBtrace.Open(), _L("Couldn't open RBTrace"));

	if (iOptions.IsPresent(&iReset))
		{
		ResetL();
		}

	if (iOptions.IsPresent(&iBufferSize))
		{
		iBtrace.ResizeBuffer(iBufferSize * 1024);
		}

	if (iOptions.IsPresent(&iMode))
		{
		SetMode(iMode);
		}

	iBtrace.SetTimestamp2Enabled(iEnableTimestamp2);

	const TInt numPrimaryFilters = iPrimaryFilters.Count();
	if (numPrimaryFilters > 0)
		{
		RemovePrimaryFilters();
		EmptyBuffer();

		// Enable the specified categories.
		for (i = 0; i < numPrimaryFilters; ++i)
			{
			iBtrace.SetFilter(iPrimaryFilters[i], 1);
			}
		}
	if (iTest)
		{
		TestL();
		return;
		}

	const TInt numSecondaryFilters = iSecondaryFilters.Count();
	if (numSecondaryFilters > 0)
		{
		TUint32* filterArray = new(ELeave) TUint32[iSecondaryFilters.Count()];
		for (i = 0; i < numSecondaryFilters; ++i)
			{
			filterArray[i] = iSecondaryFilters[i];
			}

		TInt err = iBtrace.SetFilter2(filterArray, numSecondaryFilters);
		if (err)
			{
			PrintError(err, _L("Couldn't set secondary filter"));
			}
		else
			{
			// Discard old data.
			EmptyBuffer();
			}

		delete[] filterArray;
		}

	if (numPrimaryFilters > 0 || numSecondaryFilters > 0) 
		{
		BTrace8(BTrace::EMetaTrace, BTrace::EMetaTraceMeasurementEnd, KPrimingStopped, KPrimingStopped);
		}

	if (iFileName.Length())
		{
		RFile file;
		User::LeaveIfError(file.Replace(FsL(), iFileName, EFileWrite));
		file.Close();
		}

	if (iFileName.Length() || iDumpToRdebug)
		{
		if (iFollow)
			{
			if (iThreshold > 100)
				{
				PrintError(KErrArgument, _L("Invalid threshold percentage"));
				User::Leave(KErrArgument);
				}
			Printf(_L("Hit any key to abort waiting and flush buffer...\r\n"));
			TInt bytesRequired = (iBtrace.BufferSize() * iThreshold) / 100;
			TBool finished(EFalse);
			while (!finished)
				{
				TRequestStatus btraceStatus;
				iBtrace.RequestData(btraceStatus, bytesRequired);
				TRequestStatus keyStatus;
				Stdin().WaitForKey(keyStatus);
				if (iVerbose)
					{
					Printf(_L("Waiting for %d bytes in buffer\r\n"), bytesRequired);
					}
				User::WaitForRequest(btraceStatus, keyStatus);
				if (keyStatus == KRequestPending)
					{
					Stdin().WaitForKeyCancel();
					User::WaitForRequest(keyStatus);
					User::LeaveIfError(btraceStatus.Int());
					}
				else
					{
					iBtrace.CancelRequestData();
					User::WaitForRequest(btraceStatus);
					finished = ETrue;
					}

				DumpL();
				}
			}
		else
			{
			DumpL();
			}
		}

	if (iVerbose || (iFileName.Length() == 0))
		{
		Printf(_L("Buffer size: %d\r\n"), iBtrace.BufferSize());
		Printf(_L("Mode: %S (%u)\r\n"), ModeText(iBtrace.Mode()), iBtrace.Mode());
#ifdef TEXT_TRACE_MODE_SUPPORTED
		LoadMemoryAccessL();
		TUint traceMode = 0;
		TInt err = iMemAccess.SetTextTraceMode(traceMode, 0);
		if (err)
			{
			PrintWarning(_L("Couldn't read text trace mode: err=%d"), err);
			}
		else
			{
			_LIT(KHuh, "<?>");
			const TDesC* name = &KHuh;
#define CASE_LIT2(x, y) case x: { _LIT(KName, #y); name = &KName; break; }
			switch (traceMode)
				{
				CASE_LIT2(0, ESerialOutDefault)
				CASE_LIT2(1, ESerialOutNever)
				CASE_LIT2(2, ESerialOutAlways)
				default:
					break;
				}
			Printf(_L("Text trace mode: %S (%d)\r\n"), name, traceMode);
			}
#endif
		Printf(_L("Primary filters:\r\n"));
		TBool foundOne(EFalse);
		for (TInt i = 0; i < KMaxCategories; ++i)
			{
			if (iBtrace.Filter(i) == 1)
				{
				Printf(_L("\t%d (%S)\r\n"), i, BtraceCategoryName(i));
				foundOne = ETrue;
				}
			}
		if (!foundOne)
			{
			Write(_L("\tnone\r\n"));
			}
		Printf(_L("Secondary filters: "));
		TInt globalFilter;
		TUint32* uids;
		TInt ret = iBtrace.Filter2(uids, globalFilter);
		if (ret >= 0)
			{
			if (globalFilter)
				{
				Printf(_L("[Secondary filters accept]\r\n"));
				}
			else
				{
				Printf(_L("[Secondary filters reject]\r\n"));
				}
			if (ret > 0)
				{
				for (TInt i = 0; i < ret; ++i)
					{
					Printf(_L("\t0x%08x\r\n"), uids[i]);
					}
				}
			else
				{
				Write(_L("\tnone\r\n"));
				}
			delete uids;
			}
		else
			{
			PrintError(ret, _L("Unable to read secondary filters"));
			}
		}
	}

void CCmdBtrace::DumpL()
	{
	RFile file;
	if (iFileName.Length())
		{
		if (iVerbose)
			{
			Printf(_L("Opening \"%S\"\r\n"), &iFileName);
			}
		User::LeaveIfError(file.Open(FsL(), iFileName, EFileWrite));
		CleanupClosePushL(file);
		TInt pos = 0;
		User::LeaveIfError(file.Seek(ESeekEnd, pos));
		}

	TUint8* data;
	TInt size;
	TInt err = KErrNone;
	while ((size = iBtrace.GetData(data)) != 0)
		{
		if (file.SubSessionHandle() && (err == KErrNone))
			{
			if (iVerbose)
				{
				Printf(_L("Writing %d bytes to \"%S\"\r\n"), size, &iFileName);
				}
			err = file.Write(TPtrC8(data,size));
			if (err)
				{
				PrintError(err, _L("Couldn't write to file"));
				}
			}

		if (iDumpToRdebug)
			{
			if (iVerbose)
				{
				Printf(_L("Writing %d bytes to LtkUtils::HexDumpToRDebug\r\n"), size);
				}
			LtkUtils::HexDumpToRDebug(TPtrC8(data, size));
			}

		iBtrace.DataUsed();
		}

	User::LeaveIfError(err);
	if (iFileName.Length())
		{
		CleanupStack::PopAndDestroy(&file);
		}
	}

void CCmdBtrace::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFiles, "file_name");
	aArguments.AppendFileNameL(iFileName, KArgFiles);
	}

void CCmdBtrace::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptPrimaryFilter, "filter");
	aOptions.AppendUintL(iPrimaryFilters, KOptPrimaryFilter);

	_LIT(KOptSecondaryFilter, "secondary");
	aOptions.AppendUintL(iSecondaryFilters, KOptSecondaryFilter);

	_LIT(KOptMode, "mode");
	aOptions.AppendUintL(iMode, KOptMode);

	_LIT(KOptBufferSize, "buffer");
	aOptions.AppendUintL(iBufferSize, KOptBufferSize);

	_LIT(KOptDump, "dump");
	aOptions.AppendBoolL(iDumpToRdebug, KOptDump);

	_LIT(KOptFollow, "follow");
	aOptions.AppendBoolL(iFollow, KOptFollow);

	_LIT(KOptThreshold, "threshold");
	aOptions.AppendUintL(iThreshold, KOptThreshold);

	_LIT(KOptTimestamp2, "timestamp2");
	aOptions.AppendBoolL(iEnableTimestamp2, KOptTimestamp2);

	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptTest, "test");
	aOptions.AppendBoolL(iTest, KOptTest);

	_LIT(KOptReset, "reset");
	aOptions.AppendBoolL(iReset, KOptReset);

#ifdef TEXT_TRACE_MODE_SUPPORTED
	_LIT(KOptTextTraceMode, "text-trace-mode");
	aOptions.AppendUintL(iTextTraceMode, KOptTextTraceMode);
#endif
	}

#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdBtrace)
#endif

void CCmdBtrace::TestL()
	{
	SetMode(RBTrace::EEnable);
	EmptyBuffer();
	iBtrace.SetFilter(BTrace::ESymbianExtentionsFirst, 1);
	_LIT8(KHello, "Hello world of btrace!!!");
	TInt res = BTraceBig(BTrace::ESymbianExtentionsFirst, 0, 0, KHello().Ptr(), KHello().Size());
	Printf(_L8("Writing text '%S' to btrace returned %d...\r\n"), &KHello, res);

	TUint8* data;
	TInt size;
	TInt err = KErrNotFound;
	while ((size = iBtrace.GetData(data)) != 0 && err == KErrNotFound)
		{
		Printf(_L("Read a buffer of size %d\r\n"), size);
		TPtrC8 ptr(data, size);
		if (ptr.Find(KHello) >= 0)
			{
			Printf(_L("Found text ok.\r\n"));
			err = KErrNone;
			}
		}
	if (err == KErrNotFound)
		{
		Printf(_L("Failed to find the test data\r\n"));
		}
	}

void CCmdBtrace::ResetL()
	{
	SetMode(0);
	EmptyBuffer();
	RemovePrimaryFilters();
	RemoveSecondaryFiltersL();
	}

void CCmdBtrace::SetMode(TUint aMode)
	{
	if (iVerbose)
		{
		Printf(_L("Setting mode to \"%S\" (%u)\r\n"), ModeText(aMode), aMode);
		}

	iBtrace.SetMode(aMode);
	}

void CCmdBtrace::EmptyBuffer()
	{
	if (iVerbose)
		{
		Write(_L("Discarding contents of trace buffer...\r\n"));
		}

	iBtrace.Empty();
	}

void CCmdBtrace::RemovePrimaryFilters()
	{
	if (iVerbose)
		{
		Write(_L("Removing all primary filters...\r\n"));
		}

	for (TInt i = 0; i < KMaxCategories; ++i)
		{
		iBtrace.SetFilter(i, 0);
		}
	}

void CCmdBtrace::RemoveSecondaryFiltersL()
	{
	if (iVerbose)
		{
		Write(_L("Removing all secondary filters...\r\n"));
		}

	LeaveIfErr(iBtrace.SetFilter2((const TUint32*)NULL, 0), _L("Couldn't remove all secondary filters"));
	}

const TDesC* CCmdBtrace::ModeText(TUint aMode) const
	{
	_LIT(KDisabled, "disabled");
	_LIT(KEnabled, "enabled");
	_LIT(KFreeRunning, "enabled and free running");
	_LIT(KUnknown, "unknown");

	switch (aMode)
		{
		case 0:
			return &KDisabled;
		case RBTrace::EEnable:
			return &KEnabled;
		case RBTrace::EEnable | RBTrace::EFreeRunning:
			return &KFreeRunning;
		default:
			return &KUnknown;
		}
	}

