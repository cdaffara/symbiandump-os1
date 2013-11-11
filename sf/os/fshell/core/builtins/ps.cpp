// ps.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/common.mmh>
#include <fshell/ltkutils.h>
#include <fshell/heaputils.h>
#include "ps.h"

_LIT(KDefaultMatch, "*");

CCommandBase* CCmdPs::NewLC()
	{
	CCmdPs* self = new(ELeave) CCmdPs();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPs::~CCmdPs()
	{
	delete iMatch;
	delete iFormatter;
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	iMemoryAccess.Close();
#endif
	}

CCmdPs::CCmdPs()
	{
	}

const TDesC& CCmdPs::Name() const
	{
	_LIT(KName, "ps");
	return KName;
	}

void CCmdPs::DoRunL()
	{
	if (iMatch == NULL)
		{
		iMatch = KDefaultMatch().AllocL();
		}

	iFormatter = CTextFormatter::NewL(Stdout());

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TInt err = RMemoryAccess::LoadDriver();
	if ((err == KErrNone) || (err == KErrAlreadyExists))
		{
		err = iMemoryAccess.Open();
		}
	if (err)
		{
		PrintWarning(_L("Unable to load memory access device driver: %d"), err);
		}
#endif

#if defined(__WINS__) && !defined(EKA2)
	TFindThread finder(*iMatch);
	RThread process;
#else
	TFindProcess finder(*iMatch);
	RProcess process;
#endif

	
	if (iProcessId)
		{
		User::LeaveIfError(process.Open(iProcessId));
		CleanupClosePushL(process);
		iProcessName = process.Name();
		PrintInfoL(process);
		CleanupStack::PopAndDestroy(&process);
		}
	else
		{
		while (finder.Next(iProcessName) == KErrNone)
			{
			TInt err = process.Open(finder);
			if (err)
				{
				PrintWarning(_L("Unable to open handle to process %S: %d"), &iProcessName, err);
				continue;
				}
			if ((iExcludeDead && (process.ExitType() != EExitPending)) || (iOnlyDead && (process.ExitType() == EExitPending)))
				{
				process.Close();
				continue;
				}
			CleanupClosePushL(process);
			PrintInfoL(process);
			CleanupStack::PopAndDestroy(&process);
			}
		}
		
	Write(iFormatter->Descriptor());
	Complete();
	}

void CCmdPs::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg1, "process_id");
	aArguments.AppendUintL(iProcessId, KArg1);
	}

void CCmdPs::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptMatch, "match");
	aOptions.AppendStringL(iMatch, KOptMatch);

	_LIT(KOptHuman, "human");
	aOptions.AppendBoolL(iHuman, KOptHuman);

	_LIT(KOptPriority, "priority");
	aOptions.AppendBoolL(iPrintPriority, KOptPriority);

	_LIT(KOptExclude, "exclude-dead");
	aOptions.AppendBoolL(iExcludeDead, KOptExclude);

	_LIT(KOptOnlyDead, "only-dead");
	aOptions.AppendBoolL(iOnlyDead, KOptOnlyDead);

	_LIT(KOptHandleCount, "handle-count");
	aOptions.AppendBoolL(iHandleCount, KOptHandleCount);

#if defined(EKA2) || !defined(__WINS__)
	_LIT(KOptThreads, "threads");
	aOptions.AppendBoolL(iPrintThreads, KOptThreads);

	_LIT(KOptStack, "stacks");
	aOptions.AppendBoolL(iPrintStackInfo, KOptStack);

	_LIT(KOptHeap, "heaps");
	aOptions.AppendBoolL(iPrintHeapInfo, KOptHeap);

	_LIT(KOptCpu, "cpu-time");
	aOptions.AppendBoolL(iPrintCpuTime, KOptCpu);

	_LIT(KOptChunk, "chunks");
	aOptions.AppendBoolL(iPrintChunkInfo, KOptChunk);

	_LIT(KOptFileName, "filename");
	aOptions.AppendBoolL(iPrintFileName, KOptFileName);

	_LIT(KOptMemoryInfo, "memory");
	aOptions.AppendBoolL(iPrintMemoryInfo, KOptMemoryInfo);
#endif
#if !defined(EKA2) && !defined(__WINS__)
	_LIT(KOptCommandLine, "command_line");
	aOptions.AppendBoolL(iPrintCommandLine, KOptCommandLine);
#endif
#if !defined(EKA2)
	_LIT(KOptFlags, "flags");
	aOptions.AppendBoolL(iPrintFlags, KOptFlags);
#endif

	_LIT(KOptAddresses, "addresses");
	aOptions.AppendBoolL(iAddresses, KOptAddresses);

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	_LIT(KOptCodeSegs, "codesegs");
	aOptions.AppendBoolL(iPrintCodesegs, KOptCodeSegs);
#endif
	}

#if defined(__WINS__) && !defined(EKA2)
void CCmdPs::PrintInfoL(RThread& aProcess)
#else
void CCmdPs::PrintInfoL(RProcess& aProcess)
#endif
	{
	TInt processHandleCount = 0;
	
	TBool dead = aProcess.ExitType() != EExitPending;
	iFormatter->AppendFormatL(_L("%u "), TUint(aProcess.Id()));
	if (dead) iFormatter->AppendL(_L("["));
	if (iVerbose)
		{
		iFormatter->AppendL(iProcessName);
		}
	else
		{
		TFullName name = iProcessName;
		LtkUtils::MakeProcessNameFriendly(name);
		iFormatter->AppendL(name);
		}
	if (dead) iFormatter->AppendL(_L("]"));
	iFormatter->AppendL(_L("\r\n"));

	iProcessName.Append(_L("::*"));

	if (iPrintThreads)
		{
		iFormatter->AppendL(_L("\tThreads:\r\n"));
		TFindThread threadFinder(iProcessName);
		RThread thread;
		while (threadFinder.Next(iThreadName) == KErrNone)
			{
			TInt err = thread.Open(threadFinder);
			if (err)
				{
				if ((err != KErrPermissionDenied) || iVerbose)
					{
					PrintWarning(_L("Unable to open handle to thread %S: %d"), &iThreadName, err);
					}
				continue;
				}
			if ((iExcludeDead && (thread.ExitType() != EExitPending)) || (iOnlyDead && (thread.ExitType() == EExitPending)))
				{
				thread.Close();
				continue;
				}

			CleanupClosePushL(thread);
			iThreadName = thread.Name();
#ifdef EKA2
			iFormatter->AppendFormatL(_L("\t\t%Lu %S\r\n"), thread.Id().Id(), &iThreadName);
#else
			iFormatter->AppendFormatL(_L("\t\t%u %S\r\n"), thread.Id(), &iThreadName);
#endif
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			if (iAddresses && iMemoryAccess.Handle())
				{
				TObjectKernelInfo objectInfo;
				TPckg<TObjectKernelInfo> objectInfoPckg(objectInfo);
				TInt err = iMemoryAccess.GetObjectInfoByHandle(EThread, RThread().Id(), thread.Handle(), objectInfoPckg);
				if (err == KErrNone)
					{
					iFormatter->AppendFormatL(_L("\t\t\tAddress: 0x%08x\r\n"), objectInfo.iAddressOfKernelObject);
					}
				}
#endif // FSHELL_MEMORY_ACCESS_SUPPORT
			if (iPrintPriority)
				{
				iFormatter->AppendFormatL(_L("\t\t\tPriority: %d\r\n"), thread.Priority());
				}
			PrintStackInfoL(thread, iThreadName);
			PrintHeapInfoL(thread, iThreadName);
			PrintCpuTimeL(thread, iThreadName);
			if (iHandleCount)
				{
				TInt threadHandleCount;
				thread.HandleCount(processHandleCount, threadHandleCount);
				iFormatter->AppendFormatL(_L("\t\t\tHandle count: %d\r\n"), threadHandleCount);
				}
			CleanupStack::PopAndDestroy(&thread);
			}
		}
	if (iPrintPriority)
		{
		iFormatter->AppendFormatL(_L("\tPriority: %d\r\n"), aProcess.Priority());
		}
#if defined(EKA2) || !defined(__WINS__)
	PrintChunkInfoL(iProcessName);
	if (iPrintFileName)
		{
		TFileName fileName(aProcess.FileName());
		iFormatter->AppendFormatL(_L("\tFile name: %S\r\n"), &fileName);
		}
	if (iPrintMemoryInfo)
		{
		TProcessMemoryInfo memoryInfo;
		TInt err = aProcess.GetMemoryInfo(memoryInfo);
		if (err)
			{
			PrintWarning(_L("Couldn't read memory information: %d"), err);
			}
		else
			{
			iFormatter->AppendFormatL(_L("\tCode base: 0x%08x\r\n"), memoryInfo.iCodeBase);
			PrintSizeL(_L("\tCode size: "), memoryInfo.iCodeSize);
			iFormatter->AppendFormatL(_L("\tConst data base: 0x%08x\r\n"), memoryInfo.iConstDataBase);
			PrintSizeL(_L("\tConst data size: "), memoryInfo.iConstDataSize);
			iFormatter->AppendFormatL(_L("\tInitialised data base: 0x%08x\r\n"), memoryInfo.iInitialisedDataBase);
			PrintSizeL(_L("\tInitialised data size: "), memoryInfo.iInitialisedDataSize);
			iFormatter->AppendFormatL(_L("\tUninitialised data base: 0x%08x\r\n"), memoryInfo.iUninitialisedDataBase);
			PrintSizeL(_L("\tUninitialised data size: "), memoryInfo.iUninitialisedDataSize);
			}
		}
#endif
#if !defined(EKA2) && !defined(__WINS__)
	if (iPrintCommandLine)
		{
		HBufC* cl = HBufC::NewL(aProcess.CommandLineLength());
		TPtr clPtr(cl->Des());
		aProcess.CommandLine(clPtr);
		iFormatter->AppendFormatL(_L("\tCommand line: %S\r\n"), cl);
		delete cl;
		}
#endif
	if (iPrintFlags)
		{
#ifndef EKA2
		iFormatter->AppendFormatL(_L("\tSystem: %d\r\n\tProtected: %d\r\n"), aProcess.System(), aProcess.Protected());
#endif
#if !defined(__WINS__) && !defined(EKA2)
		iFormatter->AppendFormatL(_L("\tLoaded from RAM: %d\r\n"), aProcess.LoadedFromRam());
#endif
		}

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	if (iAddresses && iMemoryAccess.Handle())
		{
		TObjectKernelInfo objectInfo;
		TPckg<TObjectKernelInfo> objectInfoPckg(objectInfo);
		TInt err = iMemoryAccess.GetObjectInfoByHandle(EProcess, RThread().Id(), aProcess.Handle(), objectInfoPckg);
		if (err == KErrNone)
			{
			iFormatter->AppendFormatL(_L("\tAddress: 0x%08x\r\n"), objectInfo.iAddressOfKernelObject);
			}
		}
	if (iPrintCodesegs && iMemoryAccess.Handle())
		{
		PrintCodeSegsL(aProcess);
		}
#endif // FSHELL_MEMORY_ACCESS_SUPPORT
	
	if (iHandleCount)
		{
		if (iPrintThreads)
			{
			iFormatter->AppendFormatL(_L("\tProcess handle count: %d\r\n"), processHandleCount);
			}
		else
			{
			// Summarise the total handle count for this process and all its threads.
			TInt threadHandleCount = 0;
			TFindThread threadFinder(iProcessName);
			RThread thread;
			while (threadFinder.Next(iThreadName) == KErrNone)
				{
				TInt err = thread.Open(threadFinder);
				if (err)
					{
					continue;
					}
				TInt thc;
				thread.HandleCount(processHandleCount, thc);
				thread.Close();
				threadHandleCount += thc;
				}
			iFormatter->AppendFormatL(_L("\tTotal handle count: %d\r\n"), processHandleCount + threadHandleCount);
			}
		}
	}

void CCmdPs::PrintStackInfoL(RThread& aThread, const TDesC& aThreadName)
	{
#ifdef EKA2
	if (iPrintStackInfo)
		{
		TThreadStackInfo stackInfo;
		TInt err = aThread.StackInfo(stackInfo);
		if (err)
			{
			PrintWarning(_L("Unable to get stack info for thread %S: %d"), &aThreadName, err);
			}
		else
			{
			const TInt stackSize = stackInfo.iBase - stackInfo.iLimit;
			iFormatter->AppendFormatL(_L("\t\t\tStack info:\r\n"));
			PrintSizeL(_L("\t\t\t\tSize: "), stackSize);
			iFormatter->AppendFormatL(_L("\t\t\t\tBase: 0x%08x\r\n"), stackInfo.iBase);
			iFormatter->AppendFormatL(_L("\t\t\t\tLimit: 0x%08x\r\n"), stackInfo.iLimit);
			iFormatter->AppendFormatL(_L("\t\t\t\tExpand limit: 0x%08x\r\n"), stackInfo.iExpandLimit);
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			if (iMemoryAccess.Handle())
				{
				const TInt KBufSize = 4096; // The largest amount RMemoryAccess allows us to copy in one go.
				HBufC8* stackBuf = HBufC8::NewLC(KBufSize);
				TPtr8 stackBufPtr(stackBuf->Des());
				TThreadMemoryAccessParamsBuf accessParamsBuf;
				TThreadMemoryAccessParams& accessParams = accessParamsBuf();
				accessParams.iId = (TInt)aThread.Id().Id();
				TInt numBytesRead = 0;
				TInt numUnusedBytes = 0;
				while (numBytesRead < stackSize)
					{
					accessParams.iAddr = (TUint8*)stackInfo.iLimit + numBytesRead;
					accessParams.iSize = Min(KBufSize, stackSize - numBytesRead);
					stackBufPtr.Zero();
					err = iMemoryAccess.GetThreadMem(accessParamsBuf, stackBufPtr);
					if (err)
						{
						PrintWarning(_L("Unable to read stack data for thread %S: %d"), &aThreadName, err);
						break;
						}
					else
						{
						const TInt bufLength = stackBuf->Length();
						for (TInt i = 0; i < bufLength; ++i)
							{
							if ((*stackBuf)[i] != 0x29)
								{
								break;
								}
							++numUnusedBytes;
							}
						numBytesRead += bufLength;
						}
					}
				if (err == KErrNone)
					{
					const TInt numUsedBytes = stackSize - numUnusedBytes;
					iFormatter->AppendFormatL(_L("\t\t\t\tHigh water mark: 0x%08x ("), stackInfo.iBase - numUsedBytes);
					if (iHuman)
						{
						iFormatter->AppendHumanReadableSizeL(numUsedBytes, EUnaligned);
						iFormatter->AppendFormatL(_L(")\r\n"));
						}
					else
						{
						iFormatter->AppendFormatL(_L("%d bytes)\r\n"), numUsedBytes);
						}
					}
				CleanupStack::PopAndDestroy(stackBuf);
				}
#endif // FSHELL_MEMORY_ACCESS_SUPPORT
			}
		}
#endif // EKA2
	}

#if defined(EKA2) && defined(FSHELL_MEMORY_ACCESS_SUPPORT)
void CCmdPs::PrintHeapInfoL(RThread& aThread, const TDesC& aThreadName)
	{
	if (iPrintHeapInfo && iMemoryAccess.Handle())
		{
		LtkUtils::RProxyAllocatorHelper allocHelper;
		CleanupClosePushL(allocHelper);
		TInt err = allocHelper.Open(iMemoryAccess, TUint(aThread.Id()));
		if (err)
			{
			PrintWarning(_L("Couldn't open allocator helper for thread %S: %d"), &aThreadName, err);
			}
		else
			{
			TInt committed = allocHelper.CommittedSize();
			TInt alloced = allocHelper.AllocatedSize();
			PrintSizeL(_L("\t\t\tHeap size:   "), committed);
			iFormatter->AppendFormatL(_L("\t\t\tAlloc count: %d\r\n"), allocHelper.AllocationCount());
			PrintSizeL(_L("\t\t\tAlloc size:  "), alloced);
			}
		CleanupStack::PopAndDestroy(&allocHelper);
		}
	}
#else
void CCmdPs::PrintHeapInfoL(RThread&, const TDesC&)
	{
	}
#endif

#ifdef EKA2
void CCmdPs::PrintCpuTimeL(RThread& aThread, const TDesC& aThreadName)
	{
	if (iPrintCpuTime)
		{
		TTimeIntervalMicroSeconds time;
		TInt err = aThread.GetCpuTime(time);
		if (err)
			{
			PrintWarning(_L("Unable to get CPU time for thread %S: %d"), &aThreadName, err);
			}
		else
			{
			iFormatter->AppendFormatL(_L("\t\t\tCPU time: %Lu\r\n"), time.Int64());
			}
		}
	}
#else
void CCmdPs::PrintCpuTimeL(RThread&, const TDesC&)
	{
	}
#endif

void CCmdPs::PrintChunkInfoL(const TDesC& aProcessName)
	{
	if (iPrintChunkInfo)
		{
		iFormatter->AppendL(_L("\tChunks:\r\n"));
		TFindChunk findChunk(aProcessName);
		while (findChunk.Next(iChunkName) == KErrNone)
			{
			TPtrC shortChunkName(iChunkName.Mid(aProcessName.Length() - 1));
			iFormatter->AppendFormatL(_L("\t\t%S\r\n"), &shortChunkName);
#ifdef EKA2
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			if (iMemoryAccess.Handle())
				{
				TChunkKernelInfo chunkInfo;
				TPckg<TChunkKernelInfo> chunkInfoPckg(chunkInfo);
				TInt err = iMemoryAccess.GetObjectInfo(EChunk, iChunkName, chunkInfoPckg);
				if (err)
					{
					PrintWarning(_L("Unable to get info for chunk %S: %d"), &iChunkName, err);
					}
				else
					{
					iFormatter->AppendFormatL(_L("\t\t\tAddress:   0x%08x\r\n"), chunkInfo.iAddressOfKernelObject);
					PrintSizeL(_L("\t\t\tSize:      "), chunkInfo.iSize);
					PrintSizeL(_L("\t\t\tMax size:  "), chunkInfo.iMaxSize);
					iFormatter->AppendFormatL(_L("\t\t\tType:      %d\r\n"), chunkInfo.iChunkType);
					}
				}
#endif // FSHELL_MEMORY_ACCESS_SUPPORT
#else  // !EKA2
			RChunk chunk;
			TInt err = chunk.Open(findChunk);
			if (err)
				{
				PrintWarning(_L("Unable to open chunk %S: %d"), &iChunkName, err);
				}
			else
				{
				CleanupClosePushL(chunk);
				PrintSizeL(_L("\t\t\tSize:      "), chunk.Size());
				PrintSizeL(_L("\t\t\tMax size:  "), chunk.MaxSize());
				CleanupStack::PopAndDestroy(&chunk);
				}
#endif // EKA2
			}
		}
	}

void CCmdPs::PrintSizeL(const TDesC& aCaption, TInt aSize)
	{
	if (iHuman)
		{
		iFormatter->AppendL(aCaption);
		iFormatter->AppendHumanReadableSizeL(aSize, EUnaligned);
		_LIT(KNewLine, "\r\n");
		iFormatter->AppendL(KNewLine);
		}
	else
		{
		_LIT(KFormat, "%S%d\r\n");
		iFormatter->AppendFormatL(KFormat, &aCaption, aSize);
		}
	}

void ReleaseCodesegMutex(TAny* aMemAccess)
	{
	static_cast<RMemoryAccess*>(aMemAccess)->ReleaseCodeSegMutex();
	}

void CCmdPs::PrintCodeSegsL(RProcess& aProcess)
	{
	TInt count = iMemoryAccess.AcquireCodeSegMutexAndFilterCodesegsForProcess(aProcess.Id());
	LeaveIfErr(count, _L("Couldn't acquire codeseg mutex"));
	CleanupStack::PushL(TCleanupItem(&ReleaseCodesegMutex, &iMemoryAccess));

	iFormatter->AppendFormatL(_L("\t%d code segments:\r\n"), count);

	TCodeSegKernelInfo codeSegInfo;
	TPckg<TCodeSegKernelInfo> pkg(codeSegInfo);
	while (iMemoryAccess.GetNextCodeSegInfo(pkg) == KErrNone)
		{
		// Reuse iChunkName, I'm sure it doesn't mind
		iChunkName.Copy(codeSegInfo.iFileName);
		iFormatter->AppendFormatL(_L("\t\t%S\r\n"), &iChunkName);
		}
	CleanupStack::PopAndDestroy(); // ReleaseCodesegMutex
	}

#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdPs)
#endif

