// fdebuggerkernel.h
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
#ifndef FDEBUGGERKERNEL_H
#define FDEBUGGERKERNEL_H

#include <nkern.h>
#include <kernel.h>
#include <kern_priv.h>

#include <fshell/common.mmh>
#include "memoryaccess.h"
class TDynDfcQueWrapper;

class MDebuggerEventClient
	{
public:
	virtual void BreakpointHit(TDes& aPkg) =0;
	};

NONSHARABLE_CLASS(DDebuggerEventHandler) : public DKernelEventHandler
	{
public:
	static DDebuggerEventHandler* New(TDfcQue* aQue);

	TInt GetZombieMode();
	TInt SetZombieMode(TInt aMode);
	HBuf* GetZombieThreadIds();
	TInt ReleaseZombie(DThread* aThread);
	TInt SuspendThread(DThread* aThread);

	TInt RegisterForBreakpointNotification(MDebuggerEventClient* aClient);
	void UnregisterForBreakpointNotification(MDebuggerEventClient* aClient);

	TInt SetBreakpoint(DThread* aThread, TLinAddr aAddress, const RMemoryAccess::TPredicate& aCondition);
	TInt SetSymbolicBreakpoint(DThread* aThread, HBuf* aCodesegName, TUint32 aOffset, const RMemoryAccess::TPredicate& aCondition);
	TInt RegisterPersistantBreakpoint(DThread* aThread, TLinAddr aAddress);
	TInt SetBreakpointEnabled(TInt aBreakpointId, TBool aEnabled);
	TInt ClearBreakpoint(TInt aBreakpointId);
	TInt ContinueFromBreakpoint(DThread* aThread, TLinAddr aExceptionAddress);
	HBuf* GetBreakpoints();

	TUint GetCreatorThread(TUint aThreadId); // Returns 0 if not known

private:
	DDebuggerEventHandler(TDfcQue* aQue);
	~DDebuggerEventHandler();
	static TUint Event(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aPrivateData);
	TUint DoEvent(TKernelEvent aEvent, TAny* a1, TAny* a2);
	void CompleteZombies();
	void Lock();
	void Unlock();
	void BreakpointLock();
	void BreakpointUnlock();
	struct SZombie;
	SZombie* FindZombie(DThread* aThread);
	void ReleaseZombie(SZombie* aZombie);
	void ReleaseZombieAndUnlock(SZombie* aZombie);
	void UnsuspendThread(SZombie* aZombie);
	TInt Zombify(TLinAddr aBreakpointAddr=0);
	struct SBreakpoint;
	SBreakpoint* FindBreakpointByAddress(/*DThread* aThread,*/ TLinAddr aAddress);
	SBreakpoint* FindBreakpointById(TInt aId);
	SBreakpoint* FindBreakpointUsingHardwareContextRegister(TInt aRegister);
	SBreakpoint* FindHardwareBreakpoint(DThread* aThread, TLinAddr aAddress);
	enum TBreakpointPolicy
		{
		EWhatever,
		EHardwareOnly,
		ESoftwareOnly,
		};
	TInt ApplyBreakpoint(SBreakpoint* aBreakpoint, TBreakpointPolicy aPolicy=EWhatever);
	void UnapplyBreakpoint(SBreakpoint* aBreakpoint);
	void ClearBreakpoint(SBreakpoint* aBreakpoint, TBool aResumeAllBlocked=EFalse);
	TInt SetProgramCounter(DThread* aThread, TLinAddr aAddress);
	void ClearAllBreakpoints();
	void RemoveAllHardwareBreakpointsForThread(DThread* aThread);
	void HandleRestoreCode(TLinAddr aAddress);
	static void HandleCodesegRemoved(TAny* aPtr);
	void DoHandleCodesegRemoved();
	TInt ReadInstructions(DThread* aThread, TLinAddr aAddress, TInt aLength, TDes8& aData);
	TInt ApplyHardwareBreakpoint(SBreakpoint* aBreakpoint);
	TInt DoApplyHardwareBreakpoint(SBreakpoint* aBreakpoint, TInt aContextReg);
	static TUint32 GetArmContextIdForThread(DThread* aThread);
	TInt MoveBreakpointToNextInstructionForThread(DThread* aThread, SBreakpoint* aBreakpoint);
#ifdef __EABI__
	void DoClearHardwareBreakpoint(SBreakpoint* aBreakpoint);
	void SetDscr(TUint32 aVal);
	void SetBreakpointPair(TInt aRegister, TUint aBvrValue, TUint aBcrValue);
	TUint ReadBcr(TInt aRegister);
	void SetContextIdBrp(TInt aRegister, TUint aContextId);

	// Ones from the-TLA-that-dare-not-speak-its-name
	TInt ReadKernelRegisterValue(DThread* aThread, TInt aRegister, TUint32& aResult);
	TInt DoReadMemory(DThread* aThread, TLinAddr aAddress, TInt aLength, TDes8& aResult);
	TBool IsPreviousInstructionMovePCToLR(DThread *aThread);
	TInt CurrentInstructionArm(DThread* aThread, TUint32& aInstruction);
	TInt CurrentInstructionThumb(DThread* aThread, TUint32& aInstruction);
	TUint32 PCAfterInstructionExecutes(DThread *aThread, TUint32 aCurrentPC, TUint32 /*aStatusRegister*/, TInt aInstSize, TUint32 &aNewRangeEnd, TBool &aChangingModes);
	TUint32 ShiftedRegValue(DThread *aThread, TUint32 aInstruction, TUint32 aCurrentPC, TUint32 aStatusRegister);
	TInt CurrentPC(DThread* aThread, TUint32& aPC);
	TInt CurrentCPSR(DThread* aThread, TUint32& aCPSR);
	TInt ReadMem32(DThread* aThread, const TUint32 aAddress, TUint32& aValue);
	TInt ReadMem16(DThread* aThread, const TUint32 aAddress, TUint16& aValue);
	TInt RegisterValue(DThread *aThread, const TUint32 aKernelRegisterId, TUint32 &aValue);
#endif

#ifdef FSHELL_ARM_MEM_MAPPED_DEBUG
	TUint32 ReadRegister(TInt aRegisterOffset);
	void WriteRegister(TInt aRegisterOffset, TUint32 aValue);
#endif

private:
	enum TZombieMode
		{
		EDisabled = 0,
		EAbnormalExit = 1,
		EAllExits = 2,
		};
	TZombieMode iZombieMode;
	
	struct SZombie
		{
		DThread* iThread;
		SDblQueLink iLink;
		DSemaphore* iBlocker; // Null if thread manually suspended via SuspendThread
		TLinAddr iBreakpointAddress; // > 0 if the thread is blocked on a breakpoint
		};
	SDblQue iZombies;
	TInt iZombieCount;
	NFastMutex iLock; // For iZombies and other non-breakpoint-related structures

	struct SBreakpoint
		{
		SBreakpoint(DThread* aThread, TInt aBreakpointId, TLinAddr aAddress, const RMemoryAccess::TPredicate& aCondition);
		void SetThreadMatchPattern(HBuf* aMatch);
		TBool MatchesThread(DThread* aThread) const;
		~SBreakpoint(); // Note this DOESNT deque from the breakpoint list - do that first!

		TBool HasModifiedCode() const;
		TBool IsHardware() const;

		DThread* iThread;
		TInt iBreakpointId;
		TLinAddr iAddress; // If the breakpoint is pending, this is actually the codeseg offset
		TBuf8<4> iOrigInstruction;
		SDblQueLink iLink;
		TAny* iCodeSeg; // If the breakpoint is pending, an HBuf* which is the name of the codeseg. For a persistant breakpoint, NULL. For anything that HasModifiedCode(), the DCodeSeg* that the address is in
		enum TFlags
			{
			EDisabledPendingCodesegLoad = 1,
			EDisabledByUser = 2,
			EDisabledDuringContinue = 4,
			EDisabled = EDisabledPendingCodesegLoad|EDisabledByUser|EDisabledDuringContinue,

			EPersistant = 8, // Meaning the actual code has an invalid instruction, so hasn't been ModifyCoded, so we should resume to the next instruction.
			EThumb = 16,
			EHardware = 32, // Implies EThreadSpecific
			ETempContinue = 64,
			EThreadSpecific = 128, // Only match a single specified thread.
			};
		TUint iFlags;
		SBreakpoint* iRealBreakpoint; // If iFlags&ETempContinue, points to the breakpoint we're temping for. Otherwise, this indicates that the breakpoint hasn't called ModifyCode because (currently) there is another breakpoint (in a different thread) at the same address that has done it for us.
		TInt iHardwareBreakpointId; // Only relevant if iFlags&EHardware
		TInt iHardwareBreakpointContextReg; // Ditto. Value is either 4 or 5
		//TInt iContinueCount;
		HBuf* iMatch;
		RMemoryAccess::TPredicate iCondition;

	private:
		SBreakpoint(); // No copying allowed
		};
	TBool iCodeModifierInited;
	SDblQue iBreakpoints;
	TInt iNextBreakpointId;
	MDebuggerEventClient* iBreakpointNotifyClient;
	/* Rules for the breakpoint lock: Any manipulation of iBreakpoints, or of any breakpoint on the iBreakpoints list, must be protected by this lock.
	 * Ditto iFreeHwBreakpoints. SBreakpoint objects not on the list can be manipulated without needing the lock held
	 */
	DMutex* iBreakpointMutex;
	TDfc iHandleCodesegRemovedDfc;
	struct SRemovedCodeseg
		{
		DProcess* iProcess;
		DCodeSeg* iCodeseg;
		SDblQueLink iLink;
		};
	SDblQue iRemovedCodesegs; // Protected by iLock
	TUint32 iFreeHwBreakpoints;
	DPlatChunkHw* iDebugRegistersChunk;

	class TCreatorInfo
		{
	public:
		TCreatorInfo(TUint aThreadId, TUint aCreatorThreadId)
			: iThreadId(aThreadId), iCreatorThreadId(aCreatorThreadId)
			{}

		TUint iThreadId;
		TUint iCreatorThreadId;
		};
	RArray<TCreatorInfo> iCreatorInfo;
	};

#endif // FDEBUGGERKERNEL_H
