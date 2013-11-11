// threadListboxdata.cpp
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
#include "KernLbxModel.h"
#include "Utils.h"
#include <fshell/clogger.h>
#include <fshell/memoryaccess.h>
#include <fshell/qr3dll.h>
#include "QResources3.hrh"
#include <fshell/heaputils.h>

CThreadsListBoxData::CThreadsListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CThreadsListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId)
	{
	TThreadKernelInfo& info = *(TThreadKernelInfo*)aInfo;
	name.Copy(info.iFullName);
	PrettyName(iType, name);

	TUint tid = info.iThreadId;
	TExitType exitType = EExitPending;
	RThread thread;
	if (Model().MemAccess().RThreadForceOpen(thread, tid) == KErrNone)
		{
		exitType = thread.ExitType();
		}
	if (exitType == EExitPending)
		{
		more.Format(_L("Tid %i"), tid);
		}
	else
		{
		TExitCategoryName cat = thread.ExitCategory();
		more.Format(_L("Tid %i %S %i"), tid, &cat, thread.ExitReason());
		}
	thread.Close();
	itemId = tid;
	}

void CThreadsListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	_LIT8(KThreadDesc,"Thread;FullName;Tid;Priority;(PriorityHumanReadable);ParentPid;StackSize;(StackSizeHumanReadable);HeapSize;(HeapSizeHumanReadable);MinHeapSize;(MinHumanReadable);UnusedPages;LostBytes;(LostBytesHumanReadable);ExitStatus;ExitCategory;ExitReason;KernObjAddr");
	_LIT8(KThreadFmt,"Thread;%S;%i;%i;(%S);%i;%i;(%S);%i;(%S);%i;(%S);%i;%i;(%S);%S;%S;%i;%x");
	//                                                    ^ HeapSizeHumanReadable
	if (i == 0) clogger.Log(KThreadDesc);
	TThreadKernelInfo& info = *(TThreadKernelInfo*)iInfo;

	TBuf<16> priority16 = ToString((TThreadPriority)info.iThreadPriority);
	TPtr8 priority = priority16.Collapse();
	
	LtkUtils::RProxyAllocatorHelper allocHelper;
	User::LeaveIfError(allocHelper.Open(Model().MemAccess(), info.iThreadId));
	TBuf8<16> heapSize, minHeap, lostBytes, stackSize;
	HR(heapSize, allocHelper.CommittedSize());
	HR(stackSize, info.iUserStackSize);
	HR(minHeap, allocHelper.MinCommittedSize());
	TInt lostBytesCount = allocHelper.CommittedFreeSpace();
	HR(lostBytes, lostBytesCount);
	TInt parentPid = 0; //TODO
	TPtrC exitStatus;
	TInt exitReason = 0;
	TExitCategoryName exitCategory;
	RThread handle;
	if (Model().MemAccess().RThreadForceOpen(handle, info.iThreadId) == KErrNone)
		{
		exitStatus.Set(ToString(handle.ExitType()));
		exitCategory = handle.ExitCategory();
		exitReason = handle.ExitReason();
		handle.Close();
		}
	TBuf8<16> exitStatus8;
	exitStatus8.Copy(exitStatus);
	TPtrC8 exitCategory8 = exitCategory.Collapse();
	clogger.Log(KThreadFmt, &info.iFullName, info.iThreadId, info.iThreadPriority, &priority, parentPid, info.iUserStackSize, &stackSize, allocHelper.CommittedSize(), &heapSize, allocHelper.MinCommittedSize(), &minHeap, allocHelper.CountUnusedPages(), lostBytesCount, &lostBytes, &exitStatus8, &exitCategory8, exitReason, info.iAddressOfKernelObject);
	allocHelper.Close();
	}

void CThreadsListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* aTemp)
	{
	TThreadKernelInfo& info = *(TThreadKernelInfo*)iInfo;
	RThread thread;
	RProcess process;
	TInt err = Model().MemAccess().RThreadForceOpen(thread, info.iThreadId);
	if (!err)
		{
		err = thread.Process(process);
		}
	TUint processId = err;
	if (!err)
		{
		processId = (TUint)process.Id();
		}
	CleanupClosePushL(thread);
	CleanupClosePushL(process);

	_LIT(KInfo, "Thread info");
	aTitle.Copy(KInfo);
	inf.Copy(iInfo->iFullName);
	inf.Append(Klflf);
	
	_LIT(KThread, "Tid: %d Pid: %d Kern Priority: %d\nPriority: %d (%S)\nProc Priority: %d (%S)");
	_LIT(KCritical, "\n%S");
	_LIT(KCreator, "\nCreator thread id: %d (%S)");
	_LIT(KKern, "\nKernel object address: 0x%08x");
	_LIT(KStack, "\n\nUser stack size: %S");
	_LIT(KHeap, "\nAllocator: 0x%08x\nHeap size: %S (%d)\nMin/Max: %S/%S\nUnused: %S Usable: %S");
	_LIT(KDied, "\n\nDied with: %S %S %i");

	TThreadPriority threadPriority = (TThreadPriority)err;
	TProcessPriority processPriority = (TProcessPriority)err;
	TPtrC priority = _L("?");
	TPtrC priorityP = _L("?");
	if (!err)
		{
		threadPriority = thread.Priority();
		priority.Set(ToString(threadPriority));
		processPriority = process.Priority();
		priorityP.Set(ToString(processPriority));
		}
	LtkUtils::RProxyAllocatorHelper allocHelper;
	User::LeaveIfError(allocHelper.Open(Model().MemAccess(), info.iThreadId));
	TBuf<16> heapSize, minHeap, maxHeap, unusedMem, reclaimableMem, stackSize;
	HR(heapSize, allocHelper.CommittedSize());
	HR(stackSize, info.iUserStackSize);
	HR(minHeap, allocHelper.MinCommittedSize());
	HR(maxHeap, allocHelper.MaxCommittedSize());
	HR(unusedMem, allocHelper.CommittedFreeSpace());
	HR(reclaimableMem, allocHelper.CountUnusedPages() * 4096);
	inf.AppendFormat(KThread, info.iThreadId, processId, info.iThreadPriority, threadPriority, &priority, processPriority, &priorityP);

	TUint creatorTid = Model().MemAccess().GetThreadCreatorId(info.iThreadId);
	if (creatorTid != 0)
		{
		RThread creator;
		TInt err = Model().MemAccess().RThreadForceOpen(creator, creatorTid);
		*aTemp = _L("?");
		if (err == KErrNone)
			{
			*aTemp = creator.FullName();
			PrettyName(iType, *aTemp);
			creator.Close();
			}
		inf.AppendFormat(KCreator, creatorTid, aTemp);
		}

	User::TCritical crit = User::ENotCritical;
	if (!err)
		{
		crit = User::Critical(thread);
		}
	if (crit != User::ENotCritical)
		{
		TPtrC critical = ToString(crit);
		inf.AppendFormat(KCritical, &critical);
		}
	inf.AppendFormat(KKern, info.iAddressOfKernelObject);

	inf.AppendFormat(KStack, &stackSize);
	inf.AppendFormat(KHeap, allocHelper.AllocatorAddress(), &heapSize, allocHelper.CommittedSize(), &minHeap, &maxHeap, &unusedMem, &reclaimableMem);
	allocHelper.Close();
	if (err == KErrNone && thread.ExitType() != EExitPending)
		{
		TPtrC exitStatus;
		TInt exitReason = 0;
		TExitCategoryName exitCategory;
		exitStatus.Set(ToString(thread.ExitType()));
		if (thread.ExitType() == EExitPanic)
			{
			exitCategory = thread.ExitCategory();
			}
		exitReason = thread.ExitReason();
		inf.AppendFormat(KDied, &exitStatus, &exitCategory, exitReason);
		}
	CleanupStack::PopAndDestroy(2, &thread);
	}

TBool CThreadsListBoxData::SupportsCommand(TInt aCommand)
	{
	switch (aCommand)
		{
		case ECmdHandleInfo:
		case ECmdKill:
		case ECmdDumpHeap:
		case ECmdSetCritical:
		case ECmdSetPriority:
		case ECmdPoll:
			return ETrue;
		default:
			return EFalse;
		}
	}


