// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
*/

#ifndef HCICMDQCONTROLLER_H
#define HCICMDQCONTROLLER_H

#include <e32base.h>

#include <bluetooth/hcicommandeventobserver.h>
#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bttypes.h> // For THCIConnHandle
#include <bluetooth/hci/hcicmdqueuedecisioninterface.h>

class CHCICmdQueueDecisionPlugin;
class CHCICommandQItem;
class MHCICmdQueueDecisionInterface;
class MHCICmdQueueEventModifierInterface;
class MHCICommandAllocator;
class MHCICommandQueueClient;
class MHardResetInitiator;
class MHCTLInterface;
class MLinkMuxNotifier;
class MPhysicalLinksState;
class CHCICmdQStarvationTimer;
class CHciUtil;

// Callback interface for the timer.
NONSHARABLE_CLASS(MHCITimerClient)
	{
public:
	virtual void CompletionTimeoutFired(TUint aUncompletedCmdId) = 0;
	virtual void StarvationTimeoutFired(TUint aNextPendingCmdId) = 0;
	};

NONSHARABLE_CLASS(CHCICmdQController) : public CBase,
										public MHCICommandEventObserver, 
										public MHCICommandQueue,
										public MHCITimerClient,
										public MHCICmdQueueUtilities
	{
public: // exported
	IMPORT_C static CHCICmdQController* NewL();
	IMPORT_C ~CHCICmdQController();

	IMPORT_C void SetHCTLInterface(MHCTLInterface& aHctlInterface);
	IMPORT_C void SetHCICommandAllocator(MHCICommandAllocator& aCommandAllocator);
	IMPORT_C void SetLinkMuxNotifier(MLinkMuxNotifier& aLinkMuxer);
	IMPORT_C void SetHCIUnmatchedEventObserver(MHCICommandQueueClient& aUnmatchedEventObserver);
	IMPORT_C void SetHardResetInitiator(MHardResetInitiator& aHardResetInitiator);
	IMPORT_C void SetPhysicalLinksState(const MPhysicalLinksState& aStackInfo);
											 
	IMPORT_C void Initialise();
	IMPORT_C void Reset();
	IMPORT_C void Start();
	IMPORT_C void DoSend();

public: // un-exported virtuals from MHCITimerClient
	virtual void CompletionTimeoutFired(TUint aUncompletedCmdId);
	virtual void StarvationTimeoutFired(TUint aWasNextPendingCmdId);
	
public: // virtuals from MHCICommandQueue
	virtual TUint MhcqAddCommandL(CHCICommandQItem* aQueItem);
	virtual TUint MhcqAddCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient);
	virtual TUint MhcqAddPriorityCommandL(CHCICommandQItem* aQueItem);
	virtual TUint MhcqAddPriorityCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient);
	virtual TUint MhcqAddInitCommandL(CHCICommandQItem* aQueItem);
	virtual TUint MhcqAddInitCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient);
	virtual TInt  MhcqRemoveCommand(TUint aCommandId, const MHCICommandQueueClient& aCmdOriginator);
	virtual void  MhcqRemoveAllCommands(const MHCICommandQueueClient& aCmdOriginator);
	virtual TUint MhcqMaxHciCommandTimeout() const;
	virtual TAny* MhcqQdpPluginInterface(TUid aUid) const;
		
private: // virtuals from MHCICommandEventObserver	
	virtual void MhceoEventNotification(const THCIEventBase& aEvent);

	// Helpers
	void RemoveAllCommands(TDblQueIter<CHCICommandQItem> aIter,
						   const MHCICommandQueueClient& aCmdOriginator,
						   TBool aCanDelete);
	void HandleCommandRemoval(CHCICommandQItem& aCmd, TBool aCanDelete);
	void HandleCommandRemoval(CHCICommandQItem*& aCmd, TBool aCanDelete);
	void CleanUpQueue(TDblQueIter<CHCICommandQItem> aIter);

private: // virtuals from MHCICmdQueueUtilities
	void MhcquiInjectEvent(const THCIEventBase& aEvent);
	CHCICommandQItem* MhcquiFindOutstandingCommand(THCIOpcode aOpcode);
	
private:
	/**
	Command queue block bit flags.
	*/
	enum
		{
		ENoBlocks					= 0x00000000,
		/**
		Indicates that there is a workaround command on the sent
		queue, so another command cannot be sent
		*/
		EWorkaroundBlock			= 0x00000001,

		/**
		Indicates the current head of the non-priority queues has been
		blocked from MHCICmdQueueDecisionInterface::MhcqdiCanSend()
		*/
		ECanSendBlock				= 0x00000002,

		/**
		Indicates that the current head of the non-priority queues has the
		same opcode as one on the sent queue.
		*/
		EDuplicatedOpcodeBlock		= 0x00000004,

		/**
		Indicates that the current head of the non-priority queues does not have
		enough HCI credits.
		*/
		EInsufficientCreditBlock	= 0x00000008,

		/**
		Indicates that there is a resent command on the sent queue, so
		another command cannot be resent.
		*/
		EResendBlock				= 0x00000010,

		/**
		Indicates that an HCTL send request
		(MLinkMuxNotifier::TryToSend) is in progress, so another
		command cannot be scheduled until we get a DoSend() callback.
		*/
		ETryToSendBlock				= 0x00000020,

		EAllBlocks					= EWorkaroundBlock |
									  ECanSendBlock |
									  EDuplicatedOpcodeBlock |
									  EInsufficientCreditBlock |
									  EResendBlock | ETryToSendBlock,

		/**
		These three are different from the TryToSend, Resend and Workaround
		blocks in that they only cache the effect of the last call to OkToSendCommand
		for the most recently processed non-priority command. This means that
		when they're all 0, OkToSendCommand needs to be called to re-evaluate
		corresponding conditions.
		*/
		ECachingCommandBlocks		= ECanSendBlock |
									  EDuplicatedOpcodeBlock |
									  EInsufficientCreditBlock,

		/**
		Do not include the Resend block for any of the queues waiting to send a command
		for the first time. Regardless of whether the Resend block is set we would need
		a credit to send a command and if we get more than one credit (the first 
		potentially being used by a resent command) then we should use them.
		*/
		ESendQueueBlocks			= EAllBlocks & ~EResendBlock,

		/**
		For the Resend queue we do not want to include the Workaround block as it maybe
		a workaround command being resent.
		*/
		EResendQueueBlocks			= EAllBlocks & ~EWorkaroundBlock,
		};

	/**
	Command queue controller state
	*/
	enum TCmdQControllerStates
		{
		/**
		Power down state state. In this state no commands will be sent or queued.
		Any attempt to queue a new command will fail.
		
		Reached on completion of async Reset() if in EResetting state.
		*/
		EUninitialised	= 0,
		
		/**
		Asynchronous reset pending state.
		
		Reached by calling Reset().
		*/
		EResetting		= 1,

		/**
		Asynchronous reset pending state.
		
		Reached by calling Initialise() while async Reset() outstanding.
		*/
		EResetInit		= 2,
		
		/**
		Initialisation state. In this state only initilisation commands will be 
		sent, normal and priority commands are held on their queues.
		
		Reached by Initialise() or completion of async Reset() if in 
		EResetInit state.
		*/
		EInitialising	= 3,
		
		/**
		Operational state.
		
		Reached by calling Start().
		*/
		EStarted		= 4,
		};

	CHCICmdQController();
	void ConstructL();
	
	inline TBool Blocked(TUint aBlocksToCheck, TUint aBlocksToBypass);
	inline TBool Blocked(TUint aBlockStatus, TUint aBlocksToCheck, TUint aBlocksToBypass);
	inline void	ClearBlock(TUint aBlocks);
	inline void	SetBlock(TUint aBlocks);
	inline TUint ValidBlocks(TUint aBlocks);
	inline TUint InvalidBlocks(TUint aBlocks);
	inline TUint CmdBypassBlocks(const CHCICommandQItem& aCmd);
	
	inline void UpdateCommandCredits(TUint8 aCommandCredits);

	inline TUint NextCommandQueueItemId();
	
	inline TDblQue<CHCICommandQItem>* ActiveRegularQueue();
	inline CHCICommandQItem* FirstQueueItem(TDblQue<CHCICommandQItem>& aQueue);
	inline CHCICommandQItem* LastQueueItem(TDblQue<CHCICommandQItem>& aQueue);

	inline void StorePurgeMarks();
	inline void PurgeAllQueues();
	void PurgeQueue(TDblQue<CHCICommandQItem>& aQueue, CHCICommandQItem* aMark);
	
	inline CHCICommandQItem* ScanQueueByOpcode(TDblQue<CHCICommandQItem>& aQueue, THCIOpcode aOpcode);
	inline CHCICommandQItem* ScanQueueByQId(TDblQue<CHCICommandQItem>& aQueue, TUint aCmdId);
#ifdef _DEBUG
	void LogQueue(TDblQue<CHCICommandQItem>& aQueue);
#endif // _DEBUG

	inline TBool CanAddCommands();
	inline TBool CmdsQueued(TDblQue<CHCICommandQItem>* aQueue);
	inline TBool AnyCmdsToSend();
	inline TDblQue<CHCICommandQItem>* QueueSendingNext();
	inline TBool NextCommandChanged(const TDblQue<CHCICommandQItem> &aQueue);

	void DoReset();
	void TryToSend();
	TBool ProcessResendQueue();
	TBool ProcessWorkaroundQueue();
	TBool ProcessPriorityQueue();
	void ProcessRegularQueue(TDblQue<CHCICommandQItem>& aQueue);
		
	void SetUpNextWorkaroundCmd(CHCICommandQItem* aPreviousCmd, const THCIEventBase* aPreviousCmdResult);
	void UpdateStarvationTimer();

	TUint DoAddCommandL(CHCICommandQItem& aQueItem,	TDblQue<CHCICommandQItem> &aQueue, TCmdQControllerStates aActiveState);
	void DeleteCommand(CHCICommandQItem* &aItem);
	TBool OkToSendCommand(CHCICommandQItem*& aCmdQItem, TUint aBypassBlocks);
	void SendCommand(CHCICommandQItem& aCmdQItem);
		
	void ProcessMatchedEvent(const THCIEventBase& aEvent, CHCICommandQItem* aCmd, TBool aConcludesCmd);
	void ProcessMatchedErrorEvent(const THCIEventBase& aEvent, CHCICommandQItem* aCmd, TBool aConcludesCmd, TBool aSendToQdp);
	void ProcessUnmatchedEvent(const THCIEventBase& aEvent, TBool aSendToQdp);
	void ProcessCommandCompletionTimeout(CHCICommandQItem* aCmd);
	void ProcessEvent(const THCIEventBase& aEvent, TBool aSendToQdp);

	// Static async CallBack methods.
	static TInt AsyncCallBackForReset(TAny* aCmdQController);
	static TInt AsyncCallBackForSend(TAny* aCmdQController);

private:
	TUint iMaxHciCommandTimeout;
	TUint iQueueStarvationTimeout;

	const static TUint KDefaultMaxHciCommandTimeout = 60000; // 60 seconds.
	
	MLinkMuxNotifier*				iLinkMuxer;
	MHCICommandQueueClient*			iUnmatchedEventObserver;
	MHCTLInterface*					iHctl;
	MHardResetInitiator*			iHardResetInitiator;
	MHCICommandAllocator*			iCommandAllocator;
	MHCICmdQueueDecisionInterface*	iQdp;
	MHCICmdQueueEventModifierInterface*	iQdpEventModifier;
	
	/**
	This timer gets reset every time the next command to send changes.
	If that command stays the same for too long, we probably have a deadlock of some sorts.
	*/
	CHCICmdQStarvationTimer*		iQStarvationTimer;
	
	/**
	The queue of initialisation commands that have not yet been processed.
	*/
	TDblQue<CHCICommandQItem>		iInitCommandQ;
	
	/**
	The queue of normal commands that have not yet been processed.
	*/
	TDblQue<CHCICommandQItem>		iNormalCommandQ;

	/**
	The queue of priority commands that have not yet been processed.
	Note: it's not FIFO - the order of execution of commands is determined
	dynamically.
	*/
	TDblQue<CHCICommandQItem>		iPriorityCommandQ;

	/**
	The queue of workarounds for commands on the other queues.
	A command is moved here when time comes to execute it but QDP deems it to
	be in need of workaround.
	*/
	TDblQue<CHCICommandQItem>		iWorkaroundCommandQ;

	/**
	The queue of commands that have been sent to the controller but have not
	yet had a response.
	*/
	TDblQue<CHCICommandQItem>		iSentCommandQ;
	
	/**
	The queue of commands that are waiting to be re-sent to the controller.
	*/
	TDblQue<CHCICommandQItem>		iResendCommandQ;


	/**
	Recorded tails of the queues for Reset processing.
	*/
	CHCICommandQItem*				iInitQPurgeMark;
	CHCICommandQItem*				iNormalQPurgeMark;
	CHCICommandQItem*				iPriorityQPurgeMark;
	CHCICommandQItem*				iWorkaroundQPurgeMark;
	
	/**
	Currently processing send command. This is maintained to ensure both that the 
	correct command is processed by DoSend, and that there is only ever one outstanding 
	call to MLinkMuxNotifier::TryToSend at any given time
	*/
	CHCICommandQItem*				iSendingCommand;

	/**
	Currently processing parent command. This is maintained to simplify priority
	command processing while a workaround is in progress, and to ensure that 
	the parent's iQdpData is available to the QDP until the workaround completes.
	*/
	CHCICommandQItem*				iParentCommand;
	
	/**
	Number of HCI command packets the queue controller can send to the HCI. This
	corresponds to the Num_HCI_Command_Packets event parameter. This is decremented
	whenever a command is sent to the HCTL, and is updated on receipt of Command Status
	and Command Complete (including NOP) events.
	*/
	TUint8							iCommandCredits;
	
	/**
	The CHCICmdQController state variable
	*/
	TCmdQControllerStates			iCmdQControllerState;
	
	/**
	This is a bit mask of the possible TQueueStateBits states
	*/
	TUint							iCommandQState;
	
	/**
	The QDP that the Command Queue will use.
	*/
	CHCICmdQueueDecisionPlugin*		iQdpPlugin;

	/**
	A counter generating IDs to uniquely identify commands.
	*/
	TUint							iNextCommandQItemId;
	
	/**
	HCI Utility library
	*/
	CHciUtil* iHciUtil;
	
	/**
	Async Callbacks
	*/
	CAsyncCallBack* iAsyncCallBackForReset;
	CAsyncCallBack* iAsyncCallBackForSend;

	};

#endif // HCICMDQCONTROLLER_H 
