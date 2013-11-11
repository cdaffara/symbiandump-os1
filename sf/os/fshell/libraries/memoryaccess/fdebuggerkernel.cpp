// fdebuggerkernel.cpp
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
#include "fdebuggerkernel.h"
#include "DynamicDfcSupport.h"
#ifdef __MARM__
#	include <arm/arm.h>
#	if defined(FSHELL_ARM11XX_SUPPORT) || defined(FSHELL_ARM_MEM_MAPPED_DEBUG)
//#		ifndef __MEMMODEL_MULTIPLE__
//#			error "FSHELL_ARM11XX_SUPPORT is only supported on platforms using the multiple memory model!"
//#		endif
#	include <multiple/memmodel.h>
#	endif
#endif
#include "memoryaccess.h"

#if !defined(__EABI__) && defined(FSHELL_ARM_MEM_MAPPED_DEBUG)
#undef FSHELL_ARM_MEM_MAPPED_DEBUG
#endif

#ifdef ASSERT
#undef ASSERT
#endif
#ifdef __WINS__
#define ASSERT(x) __ASSERT_ALWAYS((x), Kern::Fault("Assertion failed: " #x, __LINE__))
#else
#define ASSERT(x) if (!(x)) { Kern::Printf("Assertion failed @ %d: " #x, __LINE__); NKern::Sleep(NKern::TimerTicks(5000)); }
#endif

#define ASSERT_LOCKED() ASSERT(Kern::CurrentThread().iNThread.iHeldFastMutex == &iLock)
#define ASSERT_UNLOCKED() ASSERT(Kern::CurrentThread().iNThread.iHeldFastMutex == NULL)
#define ASSERT_BREAKPOINT_LOCKED() ASSERT(iBreakpointMutex->iCleanup.iThread == &Kern::CurrentThread());
#define ASSERT_BREAKPOINT_UNLOCKED() ASSERT(iBreakpointMutex->iCleanup.iThread != &Kern::CurrentThread());

//#define LOG(args...) Kern::Printf(args)
#define LOG(args...)

void MCR_SetContextIdBrp(TInt aRegister, TUint aContextId);
void MCR_SetBreakpointPair(TInt aRegister, TUint aBvrValue, TUint aBcrValue);
TUint MRC_ReadBcr(TInt aRegister);
TUint32 GetDscr();
void MCR_SetDscr(TUint32 aVal);
TUint32 GetDsar();
TUint32 GetDrar();
TUint32 GetContextId();
void Dsb();
void Isb();
void Imb();

enum TMemMappedDebugAddresses
	{
	EDscrOffset = 0x88,
	EBvrOffset = 0x100,
	EBcrOffset = 0x140,
	ELockAccessOffset = 0xFB0,
	EAuthStatusOffset = 0xFB8,
	};


DDebuggerEventHandler* DDebuggerEventHandler::New(TDfcQue* aQue)
	{
	// This is backwards from the usual constructor, 2nd phase construction pattern because I can't do anything that could
	// error after creating a DKernelEventHandler, because we get called from DLogicalDevice::Install(). So the stuff that would normally
	// be done as 2nd-phase construction is done first

	DMutex* breakpointMutex = NULL;
	TInt err = Kern::MutexCreate(breakpointMutex, _L("FDebuggerBreakpointMutex"), KMutexOrdGeneral5); // No special reason for using 5
	if (err) return NULL;

	DDebuggerEventHandler* self = new DDebuggerEventHandler(aQue);
	if (self)
		{
		self->iBreakpointMutex = breakpointMutex;
		self->Add();
		}
	else
		{
		breakpointMutex->Close(NULL);
		}
	return self;
	}

DDebuggerEventHandler::DDebuggerEventHandler(TDfcQue* aQue)
	: DKernelEventHandler(&Event, this), iNextBreakpointId(1), iHandleCodesegRemovedDfc(&HandleCodesegRemoved, this, aQue, 0)
	{
#if defined(FSHELL_ARM11XX_SUPPORT) || defined(FSHELL_ARM_MEM_MAPPED_DEBUG)
	iFreeHwBreakpoints = 0x3F; // BRPs 0,1,2,3,4,5 (at a minimum) are supported on ARM11xx or later
#endif
	}

/*static*/ TUint DDebuggerEventHandler::Event(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aPrivateData)
	{
	DDebuggerEventHandler* self = static_cast<DDebuggerEventHandler*>(aPrivateData);
	return self->DoEvent(aEvent, a1, a2);
	}

TUint DDebuggerEventHandler::DoEvent(TKernelEvent aEvent, TAny* a1, TAny* a2)
	{
	//if (aEvent != EEventUserTrace) Kern::Printf("fdbk: Event %d a1=%d a2=%d", aEvent, a1, a2);

	if (aEvent == EEventKillThread)
		{
#ifdef __MARM__
		LOG("Thread %x %O with contextId %x killed", &Kern::CurrentThread().iNThread, &Kern::CurrentThread(), GetContextId());
#endif
		if (iZombieMode == EAllExits || (iZombieMode == EAbnormalExit && Kern::CurrentThread().iExitType != EExitKill))
			{
			// The thread in question is Kern::CurrentThread()
			Zombify();
			}
		RemoveAllHardwareBreakpointsForThread(&Kern::CurrentThread()); // The thread ID could get reused so make sure we clean up
		}
	else if (aEvent == EEventHwExc)
		{
		// Breakpoint?
#ifdef __MARM__
		TArmExcInfo* info = (TArmExcInfo*)a1;
		LOG("fdbk: Exception excCode=%d addr=0x%08x", info->iExcCode, info->iR15);
		SBreakpoint* b = NULL;
		TLinAddr excAddr = info->iR15 & ~1;
		BreakpointLock();
		if (info->iExcCode == 0)
			{
			// EArmExceptionPrefetchAbort - Could be a HW breakpoint
			b = FindHardwareBreakpoint(&Kern::CurrentThread(), excAddr);
			}
		else if (info->iExcCode == 2)
			{
			// EArmExceptionUndefinedOpcode - SW breakpoint?
			b = FindBreakpointByAddress(excAddr);
			}
		
		LOG("fdbk: Found breakpoint %d", b ? b->iBreakpointId : -1);

		if (b == NULL)
			{
			// Not one of ours, we should allow it to blow up (or be handled by someone else)
			// It could of course be a thread hitting a breakpoint we've just removed, in which case I'm not sure what we can do other than let
			// the thread crash... hopefully this won't happen in practice!
			BreakpointUnlock();
			return ERunNext;
			}

		TBool shouldBreak = b->MatchesThread(&Kern::CurrentThread()); // Check if it's the wrong thread
		if (b->iFlags & SBreakpoint::ETempContinue)
			{
			// It's a temporary break-at-next-instruction used while continuing a thread.
			if (shouldBreak)
				{
				// Excellent, we have sucessfully continued from a breakpoint. Restore the original, clear the temp (and any threads waiting on it), resume this thread and we're done
				SBreakpoint* orig = b->iRealBreakpoint;
				if (orig->IsHardware()) ClearBreakpoint(b, ETrue); // Clear HW first 
				TInt err = KErrNone;
				if ((orig->iFlags & SBreakpoint::EDisabled) == SBreakpoint::EDisabledDuringContinue)
					{
					// Only resume it if it hasn't been disabled for some other reason in the meantime
					err = ApplyBreakpoint(orig);
					}

				if (err)
					{
					Kern::Printf("fdbk: failed to re-enable breakpoint id %d", orig->iBreakpointId);
					// What to do?
					}
				else
					{
					// Clear the disabled flag
					orig->iFlags &= ~SBreakpoint::EDisabledDuringContinue;
					}

				if (!orig->IsHardware()) ClearBreakpoint(b, ETrue);
				BreakpointUnlock();
				return (TUint)EExcHandled;
				}
			else
				{
				// Any other threads unlucky enough to hit our temp breakpoint will just have to wait until we see our target thread - otherwise
				// we'll end up with temp breakpoints for the temp breakpoints and the universe will implode.
				BreakpointUnlock();
				Zombify(excAddr);
				return (TUint)EExcHandled;
				}
			}
		TInt id = b->iBreakpointId;
		BreakpointUnlock();
		if (shouldBreak)
			{
			// TODO should we suspend the thread rather than semaphoring it, to be more efficient?
			if (iBreakpointNotifyClient)
				{
				RMemoryAccess::TBreakpointNotification notif;
				notif.iThreadId = Kern::CurrentThread().iId;
				notif.iBreakpointId = id;
				notif.iAddress = excAddr;
				TPckg<RMemoryAccess::TBreakpointNotification> pkg(notif);
				// We shouldn't really pass blobs of data...
				iBreakpointNotifyClient->BreakpointHit(pkg);
				}
			Zombify(excAddr);
			}
		else
			{
			TInt err = ContinueFromBreakpoint(&Kern::CurrentThread(), excAddr);
			if (err) return ERunNext; // If we failed to continue, we shouldn't pretend we've handled it
			}
		return (TUint)DKernelEventHandler::EExcHandled;
#else
		(void)a1;
#endif
		}
	else if (aEvent == EEventRemoveCodeSeg)
		{
		DCodeSeg* codeseg = (DCodeSeg*)a1;
		DProcess* proc = (DProcess*)a2;
		// We can't scan the breakpoint list at this point, because we'd need to call BreakpointLock() and 
		// we're currently holding the codeseg lock. So queue a DFC.
		SRemovedCodeseg* removed = new SRemovedCodeseg;
		if (removed)
			{
			removed->iCodeseg = codeseg;
			removed->iProcess = proc;
			Lock();
			iRemovedCodesegs.Add(&removed->iLink);
			Unlock();
			NKern::ThreadEnterCS();
			iHandleCodesegRemovedDfc.Enque();
			NKern::ThreadLeaveCS();
			}
		}
	else if (aEvent == EEventRemoveThread)
		{
		DThread* thread = (DThread*)a1;
		TCreatorInfo dummy(thread->iId, 0);
		BreakpointLock(); // It's not actually breakpoint related but never mind
		TInt found = iCreatorInfo.FindInUnsignedKeyOrder(dummy);
		if (found != KErrNotFound)
			{
			iCreatorInfo.Remove(found);
			}
		BreakpointUnlock();
		}
	else if (aEvent == EEventRemoveProcess)
		{
		// In case a thread dies before creating its first thread, try to remove the creator info we stashed
		DProcess* proc = (DProcess*)a1;
		TCreatorInfo dummy(proc->iId, 0);
		BreakpointLock();
		TInt found = iCreatorInfo.FindInUnsignedKeyOrder(dummy);
		if (found != KErrNotFound)
			{
			iCreatorInfo.Remove(found);
			}
		BreakpointUnlock();
		}
	else if (aEvent == EEventAddProcess)
		{
		// We need to use EEventAddProcess as well as EEventAddThread, because EEventAddThread doesn't do the special check for the creator not being the current thread, meaning for process creation the main thread always appears to have been created by the loader and not be the person who created the process
		// Remember the process for when we come to add the thread
		DProcess* process = (DProcess*)a1;
		DThread* creator = (DThread*)a2;
		BreakpointLock();
		TCreatorInfo threadInfo(process->iId, creator->iId);
		TInt err = iCreatorInfo.InsertInUnsignedKeyOrder(threadInfo);
		if (err)
			{
			LOG("Failed to InsertInUnsignedKeyOrder err=%d", err);
			}
		BreakpointUnlock();
		}
	else if (aEvent == EEventAddThread)
		{
		DThread* thread = (DThread*)a1;
		DThread* creator = (DThread*)a2;
		TCreatorInfo threadInfo(thread->iId, creator->iId);

		BreakpointLock(); // It's not actually breakpoint related but never mind
		if (thread->Owner() != creator->Owner())
			{
			// This means we're probably the first thread of a new process, and our 'creator' is the loader thread. Use the info we stashed earlier about the process's creator instead
			TCreatorInfo dummy(((DProcess*)thread->Owner())->iId, 0);
			TInt found = iCreatorInfo.FindInUnsignedKeyOrder(dummy);
			if (found != KErrNotFound)
				{
				threadInfo.iCreatorThreadId = iCreatorInfo[found].iCreatorThreadId;
				// We don't need to track the process creator for anything else
				iCreatorInfo.Remove(found);
				}
			}
		TInt err = iCreatorInfo.InsertInUnsignedKeyOrder(threadInfo);
		if (err)
			{
			LOG("Failed to InsertInUnsignedKeyOrder err=%d", err);
			}
		BreakpointUnlock();
		}
	return ERunNext;
	}

TInt DDebuggerEventHandler::Zombify(TLinAddr aBreakpointAddr)
	{
	// The purpose of this code is to prevent dying threads from taking down their address space, so that we can still poke at their memory
	// Currently, it is also used to pause a thread on a breakpoint
	SZombie zom; // zombies go on the stack of the thread that is being halted. That way avoids having to alloc.
	zom.iThread = &Kern::CurrentThread();
	zom.iBlocker = NULL;
	zom.iBreakpointAddress = aBreakpointAddr;
	TBuf8<32> semName;
	semName.Append(_L8("ThreadZombiefier-"));
	semName.AppendNum((TUint)zom.iThread->iId);
	NKern::ThreadEnterCS();
	TInt err = Kern::SemaphoreCreate(zom.iBlocker, semName, 0);
	if (err)
		{
		NKern::ThreadLeaveCS();
		return err;
		}
	Lock();
	iZombies.Add(&zom.iLink);
	iZombieCount++;
	Unlock();
	zom.iThread->Open(); // So no-one is tempted to destroy it
	NKern::ThreadLeaveCS();
	Kern::SemaphoreWait(*zom.iBlocker);
	// The above blocks until a "fdb detach" or equivalent has happened, at which point we'll get signalled on this semaphore and should clean up
	zom.iBlocker->Close(NULL);
	zom.iThread->Close(NULL);
	return KErrNone;
	}

TInt DDebuggerEventHandler::GetZombieMode()
	{
	Lock();
	TInt res = iZombieMode;
	Unlock();
	return res;
	}

TInt DDebuggerEventHandler::SetZombieMode(TInt aMode)
	{
	Lock();
	iZombieMode = (TZombieMode)aMode;
	Unlock();
	if (aMode == EDisabled)
		{
		ClearAllBreakpoints(); // This is not exactly obvious, but unblocking a zombie that's blocked on a breakpoint necessarily means you have to remove the breakpoint
		CompleteZombies();
		}
	
	return KErrNone;
	}

void DDebuggerEventHandler::CompleteZombies()
	{
	ASSERT_UNLOCKED();
	// enter and exit with lock not held
	for (;;)
		{
		Lock();
		SDblQueLink* link = iZombies.IsEmpty() ? NULL : iZombies.First();
		if (!link)
			{
			Unlock();
			break;
			}
		SZombie* zom = _LOFF(link, SZombie, iLink);
		ReleaseZombieAndUnlock(zom);
		}
	}

void DDebuggerEventHandler::UnsuspendThread(SZombie* aZombie)
	{
	Kern::ThreadResume(*aZombie->iThread);
	aZombie->iThread->Close(NULL);
	delete aZombie;
	}

TInt DDebuggerEventHandler::SuspendThread(DThread* aThread)
	{
	// This is the only case where an SZombie goes on the heap - usually they are created in the context of their thread so go on that thread's stack
	SZombie* zom = new SZombie;
	if (!zom) return KErrNoMemory;
	zom->iThread = aThread;
	zom->iThread->Open();
	zom->iBlocker = NULL;
	Lock();
	iZombies.Add(&zom->iLink);
	iZombieCount++;
	Unlock();
	Kern::ThreadSuspend(*zom->iThread, 1);
	return KErrNone;
	}

void DDebuggerEventHandler::Lock()
	{
	NKern::FMWait(&iLock);
	}

void DDebuggerEventHandler::Unlock()
	{
	NKern::FMSignal(&iLock);
	}

DDebuggerEventHandler::~DDebuggerEventHandler()
	{
	// The call to SetZombieMode below is actually pretty pointless: the kernel increments our access count while calling
	// our Event function, which means DKernelEventHandler::Close will never get as far as our destructor
	// while we are blocking on zombies.
	SetZombieMode(EDisabled); // This frees up any outstanding zombies via CompleteZombies()

	iHandleCodesegRemovedDfc.Cancel();

	if (iCodeModifierInited)
		{
#ifdef __EPOC32__
		DebugSupport::CloseCodeModifier();
#endif
		}

	if (iBreakpointMutex)
		{
		iBreakpointMutex->Close(NULL);
		}
#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG
	if (iDebugRegistersChunk)
		{
		iDebugRegistersChunk->Close(NULL);
		}
#endif
	iCreatorInfo.Close();
	}

HBuf* DDebuggerEventHandler::GetZombieThreadIds()
	{
	TInt count = iZombieCount;
	TInt size = count*sizeof(RMemoryAccess::TZombieInfo);
	HBuf* result = HBuf::New(size);
	if (!result) return NULL;
	result->SetLength(size);
	RMemoryAccess::TZombieInfo* ptr = (RMemoryAccess::TZombieInfo*)result->Ptr();
	Lock();
	TInt i = 0;
	for (SDblQueLink* link = iZombies.First(); link != NULL && link != &iZombies.iA && i < count; i++, link=link->iNext)
		{
		SZombie& zom = *_LOFF(link, SZombie, iLink);
		ptr[i].iThreadId = zom.iThread->iId;
		ptr[i].iFlags = 0;
		if (zom.iBlocker == NULL) ptr[i].iFlags |= RMemoryAccess::TZombieInfo::ESuspended;
		else if (zom.iBreakpointAddress != 0) ptr[i].iFlags |= RMemoryAccess::TZombieInfo::EBreakpoint;
		}
	Unlock();
	return result;
	}

DDebuggerEventHandler::SZombie* DDebuggerEventHandler::FindZombie(DThread* aThread)
	{
	// Enter and leave locked
	ASSERT_LOCKED();
	for (SDblQueLink* link = iZombies.First(); link != NULL && link != &iZombies.iA; link=link->iNext)
		{
		SZombie* zom = _LOFF(link, SZombie, iLink);
		if (zom->iThread == aThread)
			{
			return zom;
			}
		}
	return NULL;
	}

TInt DDebuggerEventHandler::ReleaseZombie(DThread* aThread)
	{
	ASSERT_UNLOCKED();
	Lock();
	SZombie* found = FindZombie(aThread);
	if (found)
		{
		if (found->iBreakpointAddress)
			{
			// It's actually paused on a breakpoint, so we need to do a continue instead
			Unlock();
			return ContinueFromBreakpoint(aThread, 0);
			}
		else
			{
			ReleaseZombieAndUnlock(found);
			return KErrNone;
			}
		}
	else
		{
		Unlock();
		return KErrNotFound;
		}
	}

void DDebuggerEventHandler::ReleaseZombieAndUnlock(SZombie* aZombie)
	{
	ASSERT_LOCKED();
	ReleaseZombie(aZombie);
	Unlock();
	}

void DDebuggerEventHandler::ReleaseZombie(SZombie* aZombie)
	{
	ASSERT_LOCKED();
	aZombie->iLink.Deque();
	iZombieCount--;
	if (aZombie->iBlocker)
		{
		Kern::SemaphoreSignal(*aZombie->iBlocker);
		}
	else
		{
		UnsuspendThread(aZombie);
		}
	}

TInt DDebuggerEventHandler::RegisterForBreakpointNotification(MDebuggerEventClient* aClient)
	{
	Lock();
	TInt err = KErrAlreadyExists;
	if (iBreakpointNotifyClient == NULL || iBreakpointNotifyClient == aClient)
		{
		err = KErrNone;
		iBreakpointNotifyClient = aClient;
		}
	Unlock();
	return err;
	}

void DDebuggerEventHandler::UnregisterForBreakpointNotification(MDebuggerEventClient* aClient)
	{
	Lock();
	if (aClient == iBreakpointNotifyClient)
		{
		iBreakpointNotifyClient = NULL;
		}
	Unlock();
	}

TInt DDebuggerEventHandler::SetBreakpoint(DThread* aThread, TLinAddr aAddress, const RMemoryAccess::TPredicate& aCondition)
	{
	TInt codemodifierErr = KErrNone;
	if (!iCodeModifierInited)
		{
#ifdef __EPOC32__
		TUint caps;
		const TInt KMaxBreakpoints = 32;
		codemodifierErr = DebugSupport::InitialiseCodeModifier(caps, KMaxBreakpoints);
#else
		codemodifierErr = KErrNotSupported;
#endif
		if (!codemodifierErr) iCodeModifierInited = ETrue;
		// It's not necessarily fatal that we can't init the code modifier - we may still be able to set a H/W breakpoint
		}

	SBreakpoint* b = new SBreakpoint(aThread, iNextBreakpointId, aAddress, aCondition); // iNextBreakpointId gets incremented later
	if (!b) return KErrNoMemory;

	// Get the codeseg for this address
	Kern::AccessCode();
	b->iCodeSeg = Kern::CodeSegFromAddress(b->iAddress, b->iThread->iOwningProcess);
	Kern::EndAccessCode();

	BreakpointLock();
	
	// Check if there's already a HW breakpoint for this thread and address - we don't allow duplicates, just for sanity's sake
	SBreakpoint* existingHwBreakpoint = FindHardwareBreakpoint(b->iThread, b->iAddress);
	if (existingHwBreakpoint)
		{
		BreakpointUnlock();
		delete b;
		return KErrAlreadyExists;
		}

	TBreakpointPolicy policy = EWhatever;
	// Check if we already have a breakpoint for this address. Because all software breakpoints are global, if we add repeated breakpoints on the same address (but on threads in different processes) DebugSupport will happily allow it and create nested nastyness
	SBreakpoint* existingBreakpoint = FindBreakpointByAddress(b->iAddress);
	if (existingBreakpoint)	policy = EHardwareOnly; // To prevent another SW breakpoint at this location. Multiple HW breakpoints are fine because they're per-thread
	if (codemodifierErr != KErrNone) policy = EHardwareOnly; // If we failed to init code modifier, can only do h/w

	iBreakpoints.Add(&b->iLink);
	TInt err = ApplyBreakpoint(b, policy);
	if (err && existingBreakpoint)
		{
		// If we couldn't set a HW breakpoint, no worries, fall back to relying on the existing SW breakpoint
		b->iRealBreakpoint = existingBreakpoint;
		err = KErrNone;
		}

	if (err < 0)
		{
		b->iLink.Deque();
		delete b;
		}
	else
		{
		iNextBreakpointId++; // Now we know we're actually using it
		}

	BreakpointUnlock();
	if (err)
		{
		return err;
		}
	else
		{
		TInt result = b->iBreakpointId;
		if (b->IsHardware()) result |= RMemoryAccess::TBreakpointInfo::EHardware;
		return result;
		}
	}

TInt DDebuggerEventHandler::SetSymbolicBreakpoint(DThread* aThread, HBuf* aCodesegName, TUint32 aOffset, const RMemoryAccess::TPredicate& aCondition)
	{
	// See if the codeseg is currently loaded
	Kern::AccessCode();
	SDblQue* list = Kern::CodeSegList();
	for (SDblQueLink* link = list->First(); link != &list->iA; link = link->iNext)
		{
		DCodeSeg* codeSeg = _LOFF(link, DCodeSeg, iLink);
		if (codeSeg->iRootName == *aCodesegName)
			{
			TUint addr = codeSeg->iRunAddress + aOffset;
			Kern::EndAccessCode();
			TInt res = SetBreakpoint(aThread, addr, aCondition);
			if (res >= KErrNone) delete aCodesegName;
			return res;
			}
		}
	Kern::EndAccessCode();
	// If we get here, codeseg isn't currently loaded so need to create it as a pending breakpoint

	SBreakpoint* b = new SBreakpoint(aThread, iNextBreakpointId++, aOffset, aCondition);
	if (!b) return KErrNoMemory;
	b->iCodeSeg = aCodesegName;
	b->iFlags |= SBreakpoint::EDisabledPendingCodesegLoad;
	BreakpointLock();
	iBreakpoints.Add(&b->iLink);
	BreakpointUnlock();
	return KErrNone;
	}

TInt DDebuggerEventHandler::ApplyBreakpoint(SBreakpoint* aBreakpoint, TBreakpointPolicy aPolicy)
	{
	ASSERT_BREAKPOINT_LOCKED();
	// These magic constants don't appear to be defined anywhere, they're just undefined instructions that will cause an exception that we can then handle
	const TUint32 KArmBreakPoint = 0xE7F123F4;
	const TUint16 KThumbBreakPoint = 0xDE56;
	TUint32 inst = KArmBreakPoint;
	TInt instSize = 4;
	if (aBreakpoint->iFlags & SBreakpoint::EThumb)
		{
		inst = KThumbBreakPoint;
		instSize = 2;
		}

#ifdef __EPOC32__
	// Before modifying stuff, save the original instruction (needed for ReadInstructions())
	TInt err = Kern::ThreadRawRead(aBreakpoint->iThread, (TAny*)aBreakpoint->iAddress, (TAny*)aBreakpoint->iOrigInstruction.Ptr(), instSize);
	if (!err)
		{
		aBreakpoint->iOrigInstruction.SetLength(instSize);
		if (aPolicy != ESoftwareOnly)
			{
			err = ApplyHardwareBreakpoint(aBreakpoint);
			// If we manage to set a hardware breakpoint, we don't need to call ModifyCode
			if (err == KErrNone)
				{
				LOG("HW breakpoint set ok");
				aBreakpoint->iFlags |= SBreakpoint::EHardware;
				}
			else
				{
				Kern::Printf("Failed to set hardware breakpoint - %d", err);
				}
			}

		if (aPolicy != EHardwareOnly && !aBreakpoint->IsHardware())
			{
			// Last check that we don't conflict with another breakpoint
			err = (FindBreakpointByAddress(aBreakpoint->iAddress) == NULL ? KErrNone : KErrAlreadyExists);
			if (!err) err = DebugSupport::ModifyCode(aBreakpoint->iThread, aBreakpoint->iAddress, instSize, inst, DebugSupport::EBreakpointGlobal);
			if (err > 0) err = KErrNone; // Really don't care about what the breakpoint type is
			}
		}
#else
	TInt err = KErrNotSupported;
	(void)aPolicy;
#endif
	return err;
	}

DDebuggerEventHandler::SBreakpoint* DDebuggerEventHandler::FindBreakpointByAddress(/*DThread* aThread,*/ TLinAddr aAddress)
	{
	// Enter and leave holding lock
	// This only finds 'real' breakpoints, ie ones which have a ModifyCode outstanding, or are persistant. (NOT hardware breakpoints)
	ASSERT_BREAKPOINT_LOCKED();
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (/*b->iThread == aThread &&*/ b->iAddress == aAddress && (b->HasModifiedCode() || (b->iFlags & SBreakpoint::EPersistant)))
			{
			return b;
			}
		}
	return NULL;
	}

DDebuggerEventHandler::SBreakpoint* DDebuggerEventHandler::FindHardwareBreakpoint(DThread* aThread, TLinAddr aAddress)
	{
	// Enter and leave holding lock
	ASSERT_BREAKPOINT_LOCKED();
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (b->iThread == aThread && b->iAddress == aAddress && b->IsHardware())
			{
			return b;
			}
		}
	return NULL;
	}

DDebuggerEventHandler::SBreakpoint* DDebuggerEventHandler::FindBreakpointById(TInt aId)
	{
	// Enter and leave holding lock
	ASSERT_BREAKPOINT_LOCKED();
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (b->iBreakpointId == aId)
			{
			return b;
			}
		}
	return NULL;
	}

DDebuggerEventHandler::SBreakpoint* DDebuggerEventHandler::FindBreakpointUsingHardwareContextRegister(TInt aRegister)
	{
	// Enter and leave holding lock
	ASSERT_BREAKPOINT_LOCKED();
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (b->IsHardware() && b->iHardwareBreakpointContextReg == aRegister)
			{
			return b;
			}
		}
	return NULL;
	}

TInt DDebuggerEventHandler::SetBreakpointEnabled(TInt aBreakpointId, TBool aFlag)
	{
	BreakpointLock();
	SBreakpoint* b = FindBreakpointById(aBreakpointId);
	if (!b)
		{
		BreakpointUnlock();
		return KErrNotFound;
		}

	TInt err = KErrNone;
	if (aFlag)
		{
		if (b->iFlags & SBreakpoint::EDisabled == SBreakpoint::EDisabledByUser)
			{
			// Provided it's not disabled for any reason other than the user asked for it, re-enable it
			err = ApplyBreakpoint(b);
			if (err == KErrNone) b->iFlags |= ~SBreakpoint::EDisabledByUser;
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		if (!(b->iFlags & SBreakpoint::EDisabled))
			{
			// If it's not already disabled for any reason, disable it
			UnapplyBreakpoint(b);
			}
		b->iFlags |= SBreakpoint::EDisabledByUser;
		}
	BreakpointUnlock();
	return KErrNone;
	}

TInt DDebuggerEventHandler::ClearBreakpoint(TInt aBreakpointId)
	{
	BreakpointLock();
	SBreakpoint* b = FindBreakpointById(aBreakpointId);
	if (!b)
		{
		BreakpointUnlock();
		return KErrNotFound;
		}

	ClearBreakpoint(b);
	BreakpointUnlock();
	return KErrNone;
	}

void DDebuggerEventHandler::ClearBreakpoint(SBreakpoint* aBreakpoint, TBool aResumeAllBlocked /*=EFalse*/)
	{
	// enter and leave locked
	ASSERT_BREAKPOINT_LOCKED();
	TLinAddr addr = aBreakpoint->iAddress;
	aBreakpoint->iLink.Deque();
	UnapplyBreakpoint(aBreakpoint);
	delete aBreakpoint;

	if (aResumeAllBlocked)
		{
		Lock();
		for (SDblQueLink* link = iZombies.First(); link != NULL && link != &iZombies.iA; link=link->iNext)
			{
			SZombie* zom = _LOFF(link, SZombie, iLink);
			if (zom->iBreakpointAddress == addr)
				{
				ReleaseZombie(zom);
				}
			}
		Unlock();
		}
	}

void DDebuggerEventHandler::UnapplyBreakpoint(SBreakpoint* aBreakpoint)
	{
#ifdef __EABI__
	if (aBreakpoint->HasModifiedCode())
		{
		DebugSupport::RestoreCode(aBreakpoint->iThread, aBreakpoint->iAddress);
		HandleRestoreCode(aBreakpoint->iAddress);
		}
	if (aBreakpoint->IsHardware())
		{
#ifdef __SMP__
		TInt num = NKern::NumberOfCpus();
		TUint32 origAffinity = 0;
		for (TInt i = 0; i < num; i++)
			{
			TUint32 affinity = NKern::ThreadSetCpuAffinity(&Kern::CurrentThread().iNThread, i);
			if (i == 0) origAffinity = affinity;
			DoClearHardwareBreakpoint(aBreakpoint);
			}
		NKern::ThreadSetCpuAffinity(&Kern::CurrentThread().iNThread, origAffinity);
#else
		DoClearHardwareBreakpoint(aBreakpoint);
#endif // __SMP__
		iFreeHwBreakpoints |= (1 << aBreakpoint->iHardwareBreakpointId);
		}
#else
	(void)aBreakpoint;
#endif // __EABI__
	}

#ifdef __EABI__
void DDebuggerEventHandler::DoClearHardwareBreakpoint(SBreakpoint* aBreakpoint)
	{
	TUint32 bcr = ReadBcr(aBreakpoint->iHardwareBreakpointId);
	bcr = bcr & ~1; // Clear the enabled bit
	SetBreakpointPair(aBreakpoint->iHardwareBreakpointId, 0, bcr);
	}
#endif

void DDebuggerEventHandler::ClearAllBreakpoints()
	{
	ASSERT_UNLOCKED();
	ASSERT_BREAKPOINT_UNLOCKED();
	// enter and exit with lock not held
	BreakpointLock();
	for (;;)
		{
		SDblQueLink* link = iBreakpoints.IsEmpty() ? NULL : iBreakpoints.First();
		if (!link)
			{
			break;
			}
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		ClearBreakpoint(b);
		}
	BreakpointUnlock();
	}

TInt DDebuggerEventHandler::ContinueFromBreakpoint(DThread* aThread, TLinAddr aExceptionAddress)
	{
	// First find the SZombie
	SZombie* zombie = NULL;
	TLinAddr breakpointAddr = 0;
	// If aExceptionAddress is non-zero, it means we're being called from inside the exception handler and we haven't actually created a zombie
	if (aExceptionAddress)
		{
		breakpointAddr = aExceptionAddress;
		}
	else
		{
		Lock();
		zombie = FindZombie(aThread);
		if (!zombie)
			{
			Unlock();
			return KErrNotFound;
			}
		else if (zombie->iBreakpointAddress == 0)
			{
			// It's a zombie but not one on a breakpoint
			Unlock();
			return KErrNotReady;
			}
		// Deque early while we still hold lock, twiddle the pointers so the deque in ReleaseZombieAndUnlock won't break
		zombie->iLink.Deque();
		zombie->iLink.iPrev = &zombie->iLink;
		zombie->iLink.iNext = &zombie->iLink;
		breakpointAddr = zombie->iBreakpointAddress;
		Unlock();
		}

	BreakpointLock();
	SBreakpoint* breakpoint = FindHardwareBreakpoint(aThread, breakpointAddr);
	if (breakpoint == NULL) breakpoint = FindBreakpointByAddress(breakpointAddr);
	// This could be null if someone has already cleared the breakpoint
	if (!breakpoint)
		{
		BreakpointUnlock();
		if (zombie)
			{
			Lock();
			ReleaseZombieAndUnlock(zombie);
			}
		return KErrNone;
		}
	// the above logic doesn't handle if someone has cleared a persistant breakpoint that aThread is on - but if
	// the user has done that then they're on their own.
	// (Mitigated by us not listing persistant breakpoints in the GetBreakpoints fn)
	if (breakpoint->iFlags & SBreakpoint::EPersistant)
		{
		// It's a persistant breakpoint, so we don't actually want to clear the breakpoint we just need to modify the PC to step over the break instruction
		TLinAddr breakAddress = breakpoint->iAddress;
		TInt err = SetProgramCounter(aThread, breakAddress + 4);
		BreakpointUnlock();
		if (!err && zombie)
			{
			Lock();
			ReleaseZombieAndUnlock(zombie);
			}
		return err;
		}

	TInt err = MoveBreakpointToNextInstructionForThread(aThread, breakpoint); // When this gets hit it will take care of restoring the breakpoint back to what it should be
	if (err)
		{
		Kern::Printf("fdbk: Error returned from MoveBreakpointToNextInstructionForThread %d", err);
		}

	BreakpointUnlock();
	if (!err && zombie)
		{
		Lock();
		ReleaseZombieAndUnlock(zombie);
		}
	return err;
	}

HBuf* DDebuggerEventHandler::GetBreakpoints()
	{
	TInt count = 0;
	BreakpointLock();
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (b->iFlags & SBreakpoint::EPersistant) continue;
		count++;
		}
	TInt size = count*sizeof(RMemoryAccess::TBreakpointInfo);
	HBuf* result = HBuf::New(size);
	if (!result)
		{
		BreakpointUnlock();
		return NULL;
		}
	result->SetLength(size);
	RMemoryAccess::TBreakpointInfo* ptr = (RMemoryAccess::TBreakpointInfo*)result->Ptr();
	RMemoryAccess::TBreakpointInfo* end = ptr + count;
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA && ptr < end; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (b->iFlags & SBreakpoint::EPersistant) continue; // We don't talk about persistant breakpoints, since they're an implementation detail of LtkUtils::Breakpoint() really
		ptr->iThreadId = b->iThread->iId;
		ptr->iBreakpointId = b->iBreakpointId;
		ptr->iAddress = b->iAddress;
		ptr->iFlags = 0;
		if (b->iFlags & SBreakpoint::EDisabledPendingCodesegLoad) ptr->iFlags |= RMemoryAccess::TBreakpointInfo::EPending;
		if (!(b->iFlags & SBreakpoint::EDisabledByUser)) ptr->iFlags |= RMemoryAccess::TBreakpointInfo::EEnabled;
		if (b->IsHardware()) ptr->iFlags |= RMemoryAccess::TBreakpointInfo::EHardware;
		ptr->iCondition = b->iCondition;
		ptr++;
		}
	BreakpointUnlock();
	return result;
	}

TInt DDebuggerEventHandler::RegisterPersistantBreakpoint(DThread* /*aThread*/, TLinAddr aAddress)
	{
	RMemoryAccess::TPredicate condition; // Default args means 'always pass'
	SBreakpoint* b = new SBreakpoint(NULL, iNextBreakpointId, aAddress, condition); //  The thread is null to say any thread should trigger the breakpoint (ie truly global)
	if (!b) return KErrNoMemory;
	b->iFlags |= SBreakpoint::EPersistant;

	BreakpointLock();
	SBreakpoint* existingBreakpoint = FindBreakpointByAddress(b->iAddress);
	if (existingBreakpoint)
		{
		BreakpointUnlock();
		delete b;
		return KErrAlreadyExists;
		}

	// No need to call ModifyCode because persistant breakpoints are ones that hard-code the invalid instruction
	iBreakpoints.Add(&b->iLink);
	BreakpointUnlock();
	iNextBreakpointId++;
	return KErrNone;
	}

TInt DDebuggerEventHandler::SetProgramCounter(DThread* aThread, TLinAddr aAddress)
	{
	// This can hold the breakpoint lock or not, doesn't care

#if !defined(__WINS__) && !defined(FSHELL_9_1_SUPPORT) // win32 ekern doesn't even export this API let alone implement it
	TUint32 regs[32];
	TUint32 valid = 0;
	NKern::ThreadGetUserContext(&aThread->iNThread, &regs[0], valid);
	if (!(valid & (1<<15))) return KErrNotSupported; // If we can't read it, we can't set it
	regs[15] = aAddress;
	NKern::ThreadSetUserContext(&aThread->iNThread, &regs[0]);
	return KErrNone;
#else
	(void)aThread;
	(void)aAddress;
	return KErrNotSupported;
#endif
	}

void DDebuggerEventHandler::HandleRestoreCode(TLinAddr aAddress)
	{
	ASSERT_BREAKPOINT_LOCKED();
	// This function is to update any shadow breakpoints that were relying on this address having been modified
	// We can't promote any breakpoint that's marked as pending, we need to start tracking the breakpoint address
	// as an offset to the codeseg run address before we can do that.
	SBreakpoint* newRealBreakpoint = NULL;
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (b->iAddress == aAddress && !b->IsHardware() && !(b->iFlags & SBreakpoint::EDisabled) && (b->iRealBreakpoint))
			{
			if (newRealBreakpoint == NULL)
				{
				// b is the new real one
				b->iRealBreakpoint = NULL;
				TInt err = ApplyBreakpoint(b, ESoftwareOnly);
				if (err)
					{
					// Oh dear...
					b->iFlags |= SBreakpoint::EDisabledPendingCodesegLoad;
					b->iCodeSeg = NULL; // This saves me having to think what state to put it in - NULL means the breakpoint is dead forever
					}
				else
					{
					newRealBreakpoint = b;
					}
				}
			else
				{
				b->iRealBreakpoint = newRealBreakpoint;
				}
			}
		}
	}

void DDebuggerEventHandler::BreakpointLock()
	{
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iBreakpointMutex);
	}

void DDebuggerEventHandler::BreakpointUnlock()
	{
	Kern::MutexSignal(*iBreakpointMutex);
	NKern::ThreadLeaveCS();
	}

TBool DDebuggerEventHandler::SBreakpoint::HasModifiedCode() const
	{
	return !(iFlags & EDisabled)
		&& !(iFlags & EPersistant)
		&& !IsHardware()
		&& iRealBreakpoint == NULL;
	}

TBool DDebuggerEventHandler::SBreakpoint::IsHardware() const
	{
	return (iFlags & EHardware);
	}

void DDebuggerEventHandler::HandleCodesegRemoved(TAny* aPtr)
	{
	static_cast<DDebuggerEventHandler*>(aPtr)->DoHandleCodesegRemoved();
	}

void DDebuggerEventHandler::DoHandleCodesegRemoved()
	{
	for (;;)
		{
		Lock();
		SDblQueLink* link = iRemovedCodesegs.GetFirst();
		Unlock();
		if (!link) break;
		SRemovedCodeseg* r = _LOFF(link, SRemovedCodeseg, iLink);

		// Check for breakpoints that have been removed by the oh-so-helpful CodeModifier
		BreakpointLock();
		for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
			{
			SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
			if (b->iCodeSeg == r->iCodeseg && b->iThread->iOwningProcess == r->iProcess && b->HasModifiedCode())
				{
				DCodeSeg* codeSeg = static_cast<DCodeSeg*>(b->iCodeSeg);
				TBool hadModifiedCode = b->HasModifiedCode();
				b->iFlags |= SBreakpoint::EDisabledPendingCodesegLoad;
				HBuf* codesegName = HBuf::New(codeSeg->iRootName.Length());
				if (codesegName)
					{
					// If it's null, we'll just never be able to reenable the breakpoint
					codesegName->Copy(codeSeg->iRootName);
					}
				TInt codesegOffset = b->iAddress - codeSeg->iRunAddress;
				b->iCodeSeg = codesegName;
				if (hadModifiedCode)
					{
					HandleRestoreCode(b->iAddress);
					}
				b->iAddress = codesegOffset; // Pending breakpoints use iAddress to store the codeseg offset
				}
			}
		BreakpointUnlock();
		delete r;
		}
	}

DDebuggerEventHandler::SBreakpoint::SBreakpoint(DThread* aThread, TInt aBreakpointId, TLinAddr aAddress, const RMemoryAccess::TPredicate& aCondition)
	: iThread(aThread), iBreakpointId(aBreakpointId), iAddress(aAddress&~1), iCodeSeg(NULL), iFlags(0), iRealBreakpoint(NULL), iHardwareBreakpointId(-1), iHardwareBreakpointContextReg(-1), iMatch(NULL), iCondition(aCondition)
	{
	if (aAddress & 1) iFlags |= EThumb;
	if (iThread)
		{
		iThread->Open();
		iFlags |= EThreadSpecific;
		}
	}

DDebuggerEventHandler::SBreakpoint::~SBreakpoint()
	{
	if (iThread) iThread->Close(NULL);
	delete iMatch;
	if (iFlags & SBreakpoint::EDisabledPendingCodesegLoad) delete (HBuf*)iCodeSeg;
	}

TInt DDebuggerEventHandler::ReadInstructions(DThread* aThread, TLinAddr aAddress, TInt aLength, TDes8& aData)
	{
	if (aLength > aData.MaxSize()) return KErrArgument;
	TInt err = Kern::ThreadRawRead(aThread, (TAny*)aAddress, (TAny*)aData.Ptr(), aLength);
	if (err) return err;
	aData.SetLength(aLength);

	// Now check if any of that data had breakpoints in that mean we've not read the real instruction values
	BreakpointLock();
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA; link=link->iNext)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		if (b->iAddress >= aAddress && b->iAddress < aAddress + aLength && b->iOrigInstruction.Length())
			{
			TInt size = b->iFlags & SBreakpoint::EThumb ? 2 : 4;
			TPtr8 instrBuf((TUint8*)aData.Ptr() + (b->iAddress - aAddress), size, size);
			instrBuf.Copy(b->iOrigInstruction);
			}
		}
	BreakpointUnlock();
	return KErrNone;
	}

TInt DDebuggerEventHandler::ApplyHardwareBreakpoint(SBreakpoint* aBreakpoint)
	{
	ASSERT_BREAKPOINT_LOCKED();

	// First, check the context registers and see if we have one for this thread
	TInt contextReg = 0;
	SBreakpoint* breakUsingFour = FindBreakpointUsingHardwareContextRegister(4);
	SBreakpoint* breakUsingFive = FindBreakpointUsingHardwareContextRegister(5);
	if (breakUsingFour && aBreakpoint->iThread == breakUsingFour->iThread) contextReg = 4;
	if (breakUsingFive && aBreakpoint->iThread == breakUsingFive->iThread) contextReg = 5;

	if (contextReg == 0 && breakUsingFour == NULL)
		{
		// Noone is using 4
		contextReg = 4;
		}

	if (contextReg == 0 && breakUsingFive == NULL)
		{
		contextReg = 5;
		}

	LOG("fdbk: Using context reg %d", contextReg);
	if (contextReg == 0) return KErrOverflow; // No free context registers

	// Now find a free breakpoint reg
	TInt breakreg = -1;
	if (iFreeHwBreakpoints & 8) breakreg = 3;
	if (iFreeHwBreakpoints & 4) breakreg = 2;
	if (iFreeHwBreakpoints & 2) breakreg = 1;
	if (iFreeHwBreakpoints & 1) breakreg = 0;

	if (breakreg == -1) return KErrOverflow;
	aBreakpoint->iHardwareBreakpointId = breakreg;

	TInt err = KErrNone;
#ifdef __SMP__
	// Have to apply to every CPU in turn
	const TInt num = NKern::NumberOfCpus();
	TUint32 origAffinity = 0;
	LOG("Applying breakpoint to all %d CPUs...", num);
	for (TInt i = 0; i < num; i++)
		{
		TUint32 affinity = NKern::ThreadSetCpuAffinity(&Kern::CurrentThread().iNThread, i);
		if (i == 0) origAffinity = affinity;
		err = DoApplyHardwareBreakpoint(aBreakpoint, contextReg);
		if (err)
			{
			// Disable the breakpoint on any CPUs we successfully applied it on
			for (TInt j = 0; j < i; j++)
				{
				NKern::ThreadSetCpuAffinity(&Kern::CurrentThread().iNThread, j);
				DoClearHardwareBreakpoint(aBreakpoint);
				}
			break;
			}
		}
	// Restore original affinity
	NKern::ThreadSetCpuAffinity(&Kern::CurrentThread().iNThread, origAffinity);
#else
	err = DoApplyHardwareBreakpoint(aBreakpoint, contextReg);
#endif
	if (err == KErrNone)
		{
		iFreeHwBreakpoints &= ~(1<<breakreg);
		}
	return err;
	}

TInt DDebuggerEventHandler::DoApplyHardwareBreakpoint(SBreakpoint* aBreakpoint, TInt aContextReg)
	{
#if defined(__EABI__)

#if defined(FSHELL_ARM_MEM_MAPPED_DEBUG)
	if (iDebugRegistersChunk == NULL)
		{
		// Need to setup mapping for debug registers
		TUint drar = GetDrar();
		TUint dsar = GetDsar();
		//Kern::Printf("drar=0x%08x dsar=0x%08x", drar, dsar);
		if ((dsar & 3) != 3)
			{
			Kern::Printf("fdbk: DSAR enabled bits are not set - aborting");
			return KErrNotSupported;
			}
		TPhysAddr physAddr = (drar & ~3) + (dsar & ~3); // We don't check validity of DRAR, TI set it incorrectly to 0x0 (invalid) on 3530
		if (physAddr == 0x52011000) physAddr = 0x54011000; // TI got this wrong too...
		const TInt KDebugRegSize = 0x10000; // 64K so we can reach DBGEN on 3530
		TUint attrib;
		new(&attrib) TMappingAttributes2(/*EMemAttNormalUncached*/ EMemAttStronglyOrdered, EFalse, ETrue);
		TInt err = DPlatChunkHw::New(iDebugRegistersChunk, physAddr, KDebugRegSize, attrib);
		if (err < 0)
			{
			Kern::Printf("fdbk: Error mapping debug registers - %d", err);
			return err;
			}
		LOG("debug registers from 0x%08x mapped at 0x%08x", physAddr, iDebugRegistersChunk->LinearAddress());
		}
#endif

#if defined(FSHELL_ARM11XX_SUPPORT) || defined(FSHELL_ARM_MEM_MAPPED_DEBUG)
	// First. check debug status register (DSCR) to check that monitor mode is enabled (and thus that the BCRs are writeable)
	TUint dscr = GetDscr();
	//Kern::Printf("dscr=0x%08x", dscr);
#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG
	// Check for DBGEN (via authstatus register) too, we need it
	TUint authStatus = *(TUint32*)(iDebugRegistersChunk->LinearAddress() + 0xFB8);
	LOG("authStatus = 0x%08x", authStatus);
	if ((authStatus & 1) == 0)
		{
		// DBGEN not enabled - try the magic barely documented 3530 way of setting it
		LOG("Setting Lock Access Register");
		WriteRegister(ELockAccessOffset, 0xC5ACCE55);

		Kern::Printf("Reading CONTROL_SEC_EMU from 0x48002A64...");
		TUint attrib;
		new(&attrib) TMappingAttributes2(/*EMemAttNormalUncached*/ EMemAttStronglyOrdered, EFalse, ETrue);
		DPlatChunkHw* crazyOmapRegs = NULL;
		DPlatChunkHw::New(crazyOmapRegs, 0x48002000, 0x1000, attrib);
		TUint32 controlsecemu = *(TUint32*)(crazyOmapRegs->LinearAddress() + 0xa64);
		Kern::Printf("CONTROL_SEC_EMU = 0x%08x", controlsecemu);

		Kern::Printf("Trying 3530 approach to enabling DBGEN...");
		TUint32 volatile * dbgenWord = (TUint32*)(iDebugRegistersChunk->LinearAddress() + 0xc030);
		Kern::Printf("DBGEN word = 0x%08x", *dbgenWord);
		*dbgenWord |= (1 << 13); // Set bit 13 of 0x5401d030 is how you do it, apparently
		//*dbgenWord = 0xFFFFFFFF;
		// Now do as ASM says and DSB, poll for DSCR (or just wait a bit) then ISB
		Dsb();
		Kern::Printf("Wasting time waiting for dbgen....");
		Isb();
		Kern::Printf("DBGEN word is now 0x%08x", *dbgenWord);
		authStatus = *(TUint32*)(iDebugRegistersChunk->LinearAddress() + 0xFB8);
		Kern::Printf("authStatus is now 0x%08x", authStatus);
		}
#endif
	if ((dscr & 0xC000) != 0x8000)
		{
		// Set bits [15:14] to b10
		dscr = (dscr & ~0xC000) | 0x8000;
		//Kern::Printf("Setting monitor mode in DSCR...");
		SetDscr(dscr); // If there's a JTAG doing halt-mode debugging this is gonna crash. Oh well.
		}
	dscr = GetDscr();
	//Kern::Printf("dscr from MCR=0x%08x", dscr);		

#else
	// Is ARM, but not ARM11 or A8
	(void)aBreakpoint;
	(void)aContextReg;
	return KErrNotSupported;
#endif
	
	TUint32 contextId = GetArmContextIdForThread(aBreakpoint->iThread);
	
	//Kern::Printf("fdbk: fdb thinks contextId is 0x%08x", contextId);
	//return KErrNotSupported; //DEBUG

	SetContextIdBrp(aContextReg, contextId);

	// Now need to construct the BCR register value
	const TUint32 KArmBcr = 0x001001E5;
	//const TUint32 KArmBcr = 0x000001E5; // DEBUG disable context match
	//TUint32 bcr = ReadBcr(aBreakReg);
	TUint32 bcr = KArmBcr;
	bcr |= aContextReg << 16;
	if (aBreakpoint->iFlags & SBreakpoint::EThumb)
		{
		const TUint32 KByteSelectMask = 0x1E0;
		// These assume a little-endian world
		const TUint32 KMiddleOfWordThumbByteSelect = 0x180; // [8:5] = b1100
		const TUint32 KWordAlignedThumbByteSelect = 0x060; // [8:5] = b0011
		bcr = bcr & ~KByteSelectMask;
		if (aBreakpoint->iAddress & 2)
			{
			bcr |= KMiddleOfWordThumbByteSelect;
			}
		else
			{
			bcr |= KWordAlignedThumbByteSelect;
			}
		}

	SetBreakpointPair(aBreakpoint->iHardwareBreakpointId, aBreakpoint->iAddress & ~3, bcr);
	return KErrNone;
	
#else
	(void)aBreakpoint;
	(void)aContextReg;
	return KErrNotSupported;
#endif
	}

TUint32 DDebuggerEventHandler::GetArmContextIdForThread(DThread* aThread)
	{
#if (defined(FSHELL_ARM11XX_SUPPORT) || defined(FSHELL_ARM_MEM_MAPPED_DEBUG)) && defined(__MARM__)
	// This is according to TScheduler::Reschedule (gulp)
	TUint asid = static_cast<DMemModelProcess*>(aThread->iOwningProcess)->iOsAsid;
	//TUint32 result = (((TUint32)&aThread->iNThread >> 6) << 8) | asid;
	TUint32 result = (TUint32)&aThread->iNThread;
	result = (result << 2) & ~0xFF;
	result |= (asid & 0xFF);
	return result;
#else
	(void)aThread;
	return 0;
#endif
	}

#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG

TUint32 DDebuggerEventHandler::ReadRegister(TInt aRegisterOffset)
	{
	return *(TUint32*)(iDebugRegistersChunk->LinearAddress() + aRegisterOffset);
	}

void DDebuggerEventHandler::WriteRegister(TInt aRegisterOffset, TUint32 aValue)
	{
	TUint32* reg = (TUint32*)(iDebugRegistersChunk->LinearAddress() + aRegisterOffset);
	*reg = aValue;
	}

#endif // FSHELL_ARM_MEM_MAPPED_DEBUG

#ifdef __EABI__

void DDebuggerEventHandler::SetDscr(TUint32 aVal)
	{
#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG
	TUint32* dscrAddr = (TUint32*)(iDebugRegistersChunk->LinearAddress() + EDscrOffset);
	*dscrAddr = aVal;
#else
	MCR_SetDscr(aVal);
#endif
	}

void DDebuggerEventHandler::SetBreakpointPair(TInt aRegister, TUint aBvrValue, TUint aBcrValue)
	{
	LOG("fdbk: SetBreakpointPair(reg=%d, bvr=0x%08x, bcr=0x%08x)", aRegister, aBvrValue, aBcrValue);
#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG
	// This is the sequence the ARM ARM recommends
	TUint32* bcrAddr = (TUint32*)(iDebugRegistersChunk->LinearAddress() + EBcrOffset + aRegister*4);
	TUint32* bvrAddr = (TUint32*)(iDebugRegistersChunk->LinearAddress() + EBvrOffset + aRegister*4);

	// Disable the breakpoint
	//*bcrAddr = 0;
	//Imb();
	*bvrAddr = aBvrValue;
	*bcrAddr = aBcrValue;
#else
	MCR_SetBreakpointPair(aRegister, aBvrValue, aBcrValue);
#endif
	Imb();
	}

TUint DDebuggerEventHandler::ReadBcr(TInt aRegister)
	{
//#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG
//	TUint32* bcrAddr = (TUint32*)(iDebugRegistersChunk->LinearAddress() + EBcrOffset + aRegister*4);
//	return *bcrAddr;
//#else
	return MRC_ReadBcr(aRegister);
//#endif
	}

void DDebuggerEventHandler::SetContextIdBrp(TInt aRegister, TUint aContextId)
	{
#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG
	// This is according to "ARM 13.3.9. CP14 c80-c85, Breakpoint Control Registers (BCR)"
	const TUint KContextIdBCR = 0x003001E7;
	SetBreakpointPair(aRegister, aContextId, KContextIdBCR);
#else
	LOG("SetContextIdBrp %d 0x%08x", aRegister, aContextId);
	MCR_SetContextIdBrp(aRegister, aContextId);
	Imb();
#endif
	}

#endif

void DDebuggerEventHandler::RemoveAllHardwareBreakpointsForThread(DThread* aThread)
	{
	BreakpointLock();
	for (SDblQueLink* link = iBreakpoints.First(); link != NULL && link != &iBreakpoints.iA;)
		{
		SBreakpoint* b = _LOFF(link, SBreakpoint, iLink);
		link=link->iNext; // Do this before potentially calling ClearBreakpoint, because that will delete the breakpoint, invalidating link
		if (b->IsHardware() && b->MatchesThread(aThread))
			{
			ClearBreakpoint(b);
			}
		}
	BreakpointUnlock();
	}

TInt DDebuggerEventHandler::MoveBreakpointToNextInstructionForThread(DThread* aThread, SBreakpoint* aBreakpoint)
	{
#ifdef SUPPORT_BREAKPOINT_STUFF
	ASSERT_BREAKPOINT_LOCKED();
	TUint32 notUsed = 0;
	TBool aModeChange = EFalse;
	TLinAddr nextAddr = PCAfterInstructionExecutes(aThread, aBreakpoint->iAddress, notUsed, aBreakpoint->iFlags & SBreakpoint::EThumb ? 2 : 4, notUsed, aModeChange);
	LOG("fdbk: Next instruction after %x is %x", aBreakpoint->iAddress, nextAddr);

	RMemoryAccess::TPredicate alwaysPass;
	SBreakpoint* temp = new SBreakpoint(aThread, iNextBreakpointId, nextAddr, alwaysPass);
	if (!temp) return KErrNoMemory;
	iBreakpoints.Add(&temp->iLink);

	const TBool hw = aBreakpoint->IsHardware();
	if (hw) UnapplyBreakpoint(aBreakpoint); // If it's hardware, it's ok to unapply it before adding the temp one because hw breakpoints are thread-specific. For software, we want the new one in place before we remove this one to make sure we don't miss stuff

	TInt err = ApplyBreakpoint(temp, hw ? EHardwareOnly : ESoftwareOnly);
	if (err)
		{
		// This really shouldn't fail if hw - we still have the lock and we know there's a free BRP cos we just unapplied aBreakpoint
		temp->iLink.Deque();
		delete temp;
		return err;
		}

	if (!hw) UnapplyBreakpoint(aBreakpoint);

	aBreakpoint->iFlags |= SBreakpoint::EDisabledDuringContinue;
	temp->iFlags |= SBreakpoint::ETempContinue;
	temp->iRealBreakpoint = aBreakpoint;
	iNextBreakpointId++;

	return KErrNone;
#else
	(void)aThread;
	(void)aBreakpoint;
	return KErrNotSupported;
#endif
	}


void DDebuggerEventHandler::SBreakpoint::SetThreadMatchPattern(HBuf* aMatch)
	{
	ASSERT(iMatch == NULL);
	ASSERT(iThread == NULL);
	ASSERT((iFlags & EThreadSpecific) == 0);
	iMatch = aMatch;
	}

TBool DDebuggerEventHandler::SBreakpoint::MatchesThread(DThread* aThread) const
	{
	TBool match = EFalse;

	if (iFlags & EThreadSpecific)
		{
		match = (aThread == iThread);
		}
	else if (iMatch == NULL)
		{
		match = ETrue;
		}
	else
		{
		TFullName threadName;
		aThread->FullName(threadName);
		match = threadName.MatchF(*iMatch);
		}

	if (match && iCondition.HasConditions())
		{
#ifdef __MARM__
		TUint32 regs[32];
		TUint32 valid = 0;
		NKern::ThreadGetUserContext(&aThread->iNThread, &regs[0], valid);
		match = iCondition.SatisfiesConditions(regs);
#endif
		}
	return match;
	}

//

const TUint32 KOpMask = 0xF;

TBool RMemoryAccess::TPredicate::SatisfiesConditions(TUint32* aRegisterSet) const
	{
	for (TInt slot = 0; slot < KNumSlots; slot++)
		{
		TInt slotShift = slot * 8;
		TUint32 opAndReg = (iOp >> slotShift) & 0xFF;
		TOp op = (TOp)(opAndReg & KOpMask);
		TInt reg = opAndReg >> 4;
		TBool match = Test(op, aRegisterSet[reg], iVals[slot]);
		if (!match) return EFalse;
		}
	return ETrue;
	}

TBool RMemoryAccess::TPredicate::Test(TOp aOperation, TUint32 aCurrentValue, TUint32 aStoredValue)
	{
	LOG("Testing op %d %u against %u", aOperation, aCurrentValue, aStoredValue);

	switch (aOperation)
		{
		case ENothing:
			return ETrue;
		case EEq:
		case ESignedEq:
			return (aCurrentValue == aStoredValue);
		case ENe:
		case ESignedNe:
			return (aCurrentValue != aStoredValue);
		case ELt:
			return (aCurrentValue < aStoredValue);
		case ELe:
			return (aCurrentValue <= aStoredValue);
		case EGt:
			return (aCurrentValue > aStoredValue);
		case EGe:
			return (aCurrentValue >= aStoredValue);
		case ESignedLt:
			return ((TInt32)aCurrentValue < (TInt32)aStoredValue);
		case ESignedLe:
			return ((TInt32)aCurrentValue <= (TInt32)aStoredValue);
		case ESignedGt:
			return ((TInt32)aCurrentValue > (TInt32)aStoredValue);
		case ESignedGe:
			return ((TInt32)aCurrentValue >= (TInt32)aStoredValue);
		}
	return EFalse; // Compiler shutter-upper
	}

TUint DDebuggerEventHandler::GetCreatorThread(TUint aThreadId)
	{
	TCreatorInfo dummy(aThreadId, 0);
	TUint result = 0;
	BreakpointLock();
	TInt found = iCreatorInfo.FindInUnsignedKeyOrder(dummy);
	if (found != KErrNotFound)
		{
		result = iCreatorInfo[found].iCreatorThreadId;
		}
	BreakpointUnlock();
	return result;
	}
