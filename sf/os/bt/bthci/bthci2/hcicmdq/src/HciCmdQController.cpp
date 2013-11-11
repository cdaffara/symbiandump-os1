// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <bluetooth/hcicmdqcontroller.h>
#include <bluetooth/hci/hciconsts.h>

#include "HciCmdQTimer.h"
#include "HciCmdQUtil.h"

#include <bluetooth/hardresetinitiator.h>
#include <bluetooth/hcicommandqitem.h>
#include <bluetooth/hcicommandqueueclient.h>
#include <bluetooth/linkmuxnotifier.h>
#include <bluetooth/hci/commandstatusevent.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hcicmdqueuedecisionplugin.h>
#include <bluetooth/hci/hcicmdqueuedecisioninterface.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hctlinterface.h>
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcievents.h>
#include <bluetooth/hci/completingeventquery.h>

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCICMDQ);
#endif

#ifdef _DEBUG
PANICCATEGORY("hcicmdqcon");
#endif

_LIT(KHciCommandQueueComponentName, "cmdq");

EXPORT_C void MHCICmdQueueUtilities::InjectEvent(const THCIEventBase& aEvent)
	{
	MhcquiInjectEvent(aEvent);
	}

EXPORT_C CHCICommandQItem* MHCICmdQueueUtilities::FindOutstandingCommand(THCIOpcode aOpcode)
	{
	return MhcquiFindOutstandingCommand(aOpcode);
	}

EXPORT_C /*static*/ CHCICmdQController* CHCICmdQController::NewL()
	{
	LOG_STATIC_FUNC

	CHCICmdQController* self = new (ELeave) CHCICmdQController();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
*/
EXPORT_C CHCICmdQController::~CHCICmdQController()
	{
	LOG_FUNC

	// All (entry point) command queue clients should have removed any commands they added.
#if defined(_DEBUG) && defined(__FLOG_ACTIVE)
	// In debug to help debugging by logging the commands left on the queues that should be empty.
	if(!iInitCommandQ.IsEmpty())
		{
		LOG(_L("Initialisation Queue is not empty..."))
		LogQueue(iInitCommandQ);
		}
	if(!iNormalCommandQ.IsEmpty())
		{
		LOG(_L("Normal Queue is not empty..."))
		LogQueue(iNormalCommandQ);
		}
	if(!iPriorityCommandQ.IsEmpty())
		{
		LOG(_L("Priority Queue is not empty..."))
		LogQueue(iPriorityCommandQ);
		}
	if(!iWorkaroundCommandQ.IsEmpty())
		{
		LOG(_L("Workaround Queue is not empty..."))
		LogQueue(iWorkaroundCommandQ);
		}
#endif // _DEBUG && __FLOG_ACTIVE
	__ASSERT_ALWAYS(iInitCommandQ.IsEmpty(), PANIC(KHCICmdQPanic, EInitCommandQNotEmptyInDestructor));
	__ASSERT_ALWAYS(iNormalCommandQ.IsEmpty(), PANIC(KHCICmdQPanic, ENormalCommandQNotEmptyInDestructor));
	__ASSERT_ALWAYS(iPriorityCommandQ.IsEmpty(), PANIC(KHCICmdQPanic, EPriorityCommandQNotEmptyInDestructor));
	__ASSERT_ALWAYS(iWorkaroundCommandQ.IsEmpty(), PANIC(KHCICmdQPanic, EWorkaroundCommandQNotEmptyInDestructor));
	
	// Clean up internally managed queues
	TDblQueIter<CHCICommandQItem> sentIter(iSentCommandQ);
	CleanUpQueue(sentIter);
	TDblQueIter<CHCICommandQItem> resendIter(iResendCommandQ);
	CleanUpQueue(resendIter);

	__ASSERT_ALWAYS(iSentCommandQ.IsEmpty(), PANIC(KHCICmdQPanic, ESentCommandQNotEmptyInDestructor));
	__ASSERT_ALWAYS(iResendCommandQ.IsEmpty(), PANIC(KHCICmdQPanic, EResendCommandQNotEmptyInDestructor));
		
	delete iQStarvationTimer;
	delete iSendingCommand;
	delete iQdpPlugin;

	// Delete async CallBacks.	If running, these should be cancelled by the
	// d'tor of CAsyncOneShot.
	delete iAsyncCallBackForReset;
	delete iAsyncCallBackForSend;

	delete iHciUtil;
	CLOSE_LOGGER
	}

/**
Sets the HCTL interface instance to be used by the Command Queue Controller to
send command frames over the HCTL.

@param aHctlInterface An instance implementing the interface to the HCTL.
*/
EXPORT_C void CHCICmdQController::SetHCTLInterface(MHCTLInterface& aHctlInterface)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iHctl, PANIC(KHCICmdQPanic, EHctlInterfaceInitialisedTwice));
	iHctl = &aHctlInterface;
	}

/**
Sets the HCI Command Allocator interface instance to be used by the Command Queue 
Controller to create HCI command frames.

@param aCommandAllocator Implementation of command allocator interface.
*/
EXPORT_C void CHCICmdQController::SetHCICommandAllocator(MHCICommandAllocator& aCommandAllocator)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iCommandAllocator, PANIC(KHCICmdQPanic, EHciCommandAllocatorInterfaceInitialisedTwice));
	iCommandAllocator = &aCommandAllocator;
	}

/**
Sets the Link Mux Notifier interface instance to be used by the Command Queue 
Controller to provide co-ordinated sending to the HCTL.

@param aLinkMuxer An instance implementing the interface to the Link Muxer.
*/
EXPORT_C void CHCICmdQController::SetLinkMuxNotifier(MLinkMuxNotifier& aLinkMuxer)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iLinkMuxer, PANIC(KHCICmdQPanic, ELinkMuxNotifierInitialisedTwice));	
	iLinkMuxer = &aLinkMuxer;
	}

/**
Sets the command queue client that will subsequently receive unmatched events.
	
@param aUnmatchedEventObserver An instance implementing the command queue client interface.
*/
EXPORT_C void CHCICmdQController::SetHCIUnmatchedEventObserver(MHCICommandQueueClient& aUnmatchedEventObserver)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iUnmatchedEventObserver, PANIC(KHCICmdQPanic, EUnmatchedEventObserverInitialisedTwice));	
	iUnmatchedEventObserver = &aUnmatchedEventObserver;
	}

/**
Sets the Hard Reset Initiator interface instance to be used by the QDP. 
This will allow the QDP to request a Hard Reset.

@param aHardResetInitiator An instance implementing the interface to the Hard Reset Initiator.
*/
EXPORT_C void CHCICmdQController::SetHardResetInitiator(MHardResetInitiator& aHardResetInitiator)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iHardResetInitiator, PANIC(KHCICmdQPanic, EHardResetInitiatorInitialisedTwice));	
	iHardResetInitiator = &aHardResetInitiator;
	iQdp->MhcqdiSetHardResetInitiator(aHardResetInitiator);
	}

/**
Sets the physical links state interface instance to be used by the QDP to query the state of
physical links in the stack..

@param aStackInfo An instance implementing the interface to the physical links state information.
*/
EXPORT_C void CHCICmdQController::SetPhysicalLinksState(const MPhysicalLinksState& aStackInfo)
	{
	LOG_FUNC
	iQdp->MhcqdiSetPhysicalLinksState(aStackInfo);
	}

/**
This allows the Command Queue to start accepting commands to be queued. Whilst
in the Initialise state only Initialisation commands will be sent but other
commands will be added to the Normal and Priority queues so they can be sent
when we transition to the Started state.
@see CHCICmdQController::Start
*/
EXPORT_C void CHCICmdQController::Initialise()
	{
	LOG_FUNC
	
	// Can only enter the Initialising state from either the Uninitialised
	// state or the Resetting state
	switch(iCmdQControllerState)
		{
		case EUninitialised:
			iCmdQControllerState = EInitialising;
			break;
			
		case EResetting:
			iCmdQControllerState = EResetInit;
			break;
		
		default:
			PANIC(KHCICmdQPanic, EInvalidStateTransition);
			break;
		};
	}
	
/**
Initiates an asynchronous request for reset processing on the command queue. 
Asynchronous reset processing is performed by CHCICmdQController::DoReset(), to
empty the command queue and perform any other tasks on the Command Queue when 
the stack requests a sub-system reset. This will put the Command Queue into a 
state where no commands can be sent until CHCICmdQController::Initialise() is 
called.
@see CHCICmdQController::Initialise
*/
EXPORT_C void CHCICmdQController::Reset()
	{
	LOG_FUNC
	
	// Mark the tails of the queues.
	StorePurgeMarks();
	
	// Stop the queue starvation timer and reset the priority insert point.
	iQStarvationTimer->Cancel();

	iCmdQControllerState = EResetting;

	// Cancel the sending callback if it is currently active.
	iAsyncCallBackForSend->Cancel();
	
	// Call async CallBack to initiate Reset processing.
	iAsyncCallBackForReset->CallBack();
	}

/**
Signals that the Stack has finished intialising the HCI, and that it is
safe to send normal (non-initialisation) commands.
Trying to add intialisation commands after this call will result in a panic.
@see CHCICmdQController::Reset
@panic EInvalidStateTransition if not in the initialising state.
@panic EStartCalledWhenInitQNotEmpty if the method is called when the initialisation queue is not empty.
@panic EObjectNotInitialised if not all inteface references have been populated.
*/
EXPORT_C void CHCICmdQController::Start()
	{
	LOG_FUNC

	// Can only enter the Started state from the Initialising state
	__ASSERT_ALWAYS((iCmdQControllerState == EInitialising), PANIC(KHCICmdQPanic, EInvalidStateTransition));
	__ASSERT_ALWAYS((iInitCommandQ.IsEmpty()), PANIC(KHCICmdQPanic, EStartCalledWhenInitQNotEmpty));
	__ASSERT_ALWAYS(iHctl &&
					iCommandAllocator &&
					iLinkMuxer &&
					iUnmatchedEventObserver &&
					iHardResetInitiator, PANIC(KHCICmdQPanic, EObjectNotInitialised));

	iCmdQControllerState = EStarted;
	if(AnyCmdsToSend())
		{
		iAsyncCallBackForSend->CallBack();
		}
	}

/**
Called by CLinkMuxer only when the command sub-channel is open.
This shall be as a result of this class calling MLinkMuxNotifier::TryToSend.
Only one send should be sent during this function. If another command is suitable
for sending then MLinkMuxNotifier::TryToSend should be called again to get 
permission to do the sending.
*/
EXPORT_C void CHCICmdQController::DoSend()
	{
	LOG_FUNC;

	LOG1(_L("CHCICmdQController::DoSend() - iSendingCommand = 0x%08x"), iSendingCommand);

	if(iSendingCommand)
		{
		// Format and send the command frame.  Operation cannot fail.
		iSendingCommand->FormatFrame();
		TInt err = iHctl->MhiWriteCommand(iSendingCommand->Frame().HCTLPayload());
		LOG1(_L("CHCICmdQController::DoSend() - MhiWriteCommand, Err = %d"), err);

		if(err == KErrNone)
			{
			// Increment the commands sent count.
			iSendingCommand->CommandSent();

			TUint consumed = iSendingCommand->Command().CreditsConsumed();
			// Check if the vendor command has a completion event
			MHCICompletingEventQuery* completingEventInterface = NULL;
			err = iSendingCommand->Command().Extension_(KCompletingEventExpectUid, reinterpret_cast<TAny*&>(completingEventInterface), NULL);
			if( (err == KErrNone && !completingEventInterface->MhceqCompletingEventExpected()) ||
				(err == KErrExtensionNotSupported && consumed == 0))
				{
				// Certain commands (e.g. the Number of Host Complete Packets
				// command) use no credits and do not normally return any
				// event. In this case the command is not moved to the sent
				// queue to prevent it from continually growing.
				// This call will set iSendingCommand to NULL.
				DeleteCommand(iSendingCommand);
				}
			else
				{
				// Decrement command credits, move the command to the sent queue and clear the send buffer.
				__ASSERT_ALWAYS((consumed <= iCommandCredits),
							    PANIC(KHCICmdQPanic, ECommandCreditsCountLessThanZero));
				LOG2(_L("CHCICmdQController::DoSend() - iCommandCredits = %d,  consumed = %d"), iCommandCredits, consumed);

				// If necessary start completion timer.
				TUint timeout(iQdp->MhcqdiTimeoutRequired(*iSendingCommand));
				__ASSERT_ALWAYS((timeout <= MhcqMaxHciCommandTimeout()),
								PANIC(KHCICmdQPanic, ECommandTimeoutTooBig));

				// If a timeout has been specified from the QDP start it.
				if(timeout != MHCICmdQueueDecisionInterface::KNoTimeoutRequired)
					{
					iSendingCommand->StartCompletionTimer(timeout, *this);
					}
				
				iCommandCredits -= consumed;
				iSentCommandQ.AddLast(*iSendingCommand);
				iSendingCommand = NULL;
				}
			}
		else
			{
			// Failed to write the command.  Error the client.
			if(iSendingCommand->Client())
				{
				iSendingCommand->Client()->MhcqcCommandErrored(err, &iSendingCommand->Command());
				}
			DeleteCommand(iSendingCommand); // This will also NULL iSendingCommand
			}
		}
	// Clear the TryToSendBlock and reschedule if required.
	ClearBlock(ETryToSendBlock);
	if(AnyCmdsToSend())
		{
		iAsyncCallBackForSend->CallBack();
		}
	}

/**
This function is called when the timer for aUncompletedCmdId fires.
@panic ETimedOutCommandError if aUncompletedCmdId does not exist on
the sent queue.
@param aUncompletedCmdId the ID of the uncompleted command.
*/
void CHCICmdQController::CompletionTimeoutFired(TUint aUncompletedCmdId)
	{
	LOG_FUNC
	
	CHCICommandQItem* cmd = ScanQueueByQId(iSentCommandQ, aUncompletedCmdId);
	__ASSERT_ALWAYS(cmd != NULL, PANIC(KHCICmdQPanic, ETimedOutCommandError));
	
	ProcessCommandCompletionTimeout(cmd);
 
	if (AnyCmdsToSend())
		{
		iAsyncCallBackForSend->CallBack();
		}
	}

/**
@panic ECanSendDeadlock as a guard against a CanSend block deadlock in
	   UDEB builds. In UREL builds,
	   MHardResetInitiator::MhriStartHardReset will be called.
@panic EResendDeadlock as a guard against a Resend block deadlock in
	   UDEB builds. In UREL builds,
	   MHardResetInitiator::MhriStartHardReset will be called.
@panic EUnknownDeadlock as a guard against priority queue deadlock in
	   UDEB builds, i.e. the priority queue hasn't changed at all.
	   In UREL builds, MHardResetInitiator::MhriStartHardReset will be called.
@param aNextPendingCmdId this was the ID of the next pending
	   command when the timer was started. If this hasn't changed then
	   panic / ResetCycle as above.
*/
void CHCICmdQController::StarvationTimeoutFired(TUint __DEBUG_ONLY(aNextPendingCmdId))
	{
	LOG_FUNC

#ifdef _DEBUG
	// Check for possible dead-lock situation that can occur.
	TDblQue<CHCICommandQItem>* queue = QueueSendingNext();
	
	if (queue)
		{
		if (queue != &iPriorityCommandQ)
			{
			CHCICommandQItem* cmd = queue->First();
	
			if (cmd->CommandQId() == aNextPendingCmdId)
				{
				if (Blocked(ECanSendBlock, ENoBlocks))
					{
					PANIC(KHCICmdQPanic, ECanSendDeadlock);
					}
				else if (Blocked(EResendBlock, ENoBlocks))
					{
					PANIC(KHCICmdQPanic, EResendDeadlock);
					}
				}
			}
		else if (aNextPendingCmdId == KInvalidCommandQueueItemId)
			{
			// This is priority queue, so we can't point to a single culprit.
			// Apparently none of the commands on the queue could be sent.
			PANIC(KHCICmdQPanic, EUnknownDeadlock);
			}
		}
	
	// Always assert in debug builds for starvation timer conditions.
	// This may be removed when more is known about these scenarios.
	__ASSERT_DEBUG(EFalse, PANIC(KHCICmdQPanic, EStarvationTimerFired));
#endif

	// Force possibly stale caching blocks to be recomputed.
	ClearBlock(ECachingCommandBlocks);
	
	iHardResetInitiator->MhriStartHardReset();
	}

/** 
Virtuals from MHCICommandQueue
*/

/*virtual*/ TUint CHCICmdQController::MhcqAddCommandL(CHCICommandQItem* aQueItem)
	{
	LOG_FUNC	
	__ASSERT_ALWAYS(aQueItem != NULL, PANIC(KHCICmdQPanic, EAttemptToAddNullCommand));

	return DoAddCommandL(*aQueItem, iNormalCommandQ, EStarted);
	}

/*virtual*/ TUint CHCICmdQController::MhcqAddCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aCommandData != NULL, PANIC(KHCICmdQPanic, EAttemptToAddNullCommand));
	__ASSERT_ALWAYS(iCommandAllocator != NULL, PANIC(KHCICmdQPanic, EObjectNotInitialised));

	CHCICommandQItem* aQueItem = CHCICommandQItem::NewL(*iCommandAllocator, 
														aCmdProgressRecipient, 
														aCommandData);
	return MhcqAddCommandL(aQueItem);
	}

/*virtual*/ TUint CHCICmdQController::MhcqAddPriorityCommandL(CHCICommandQItem* aQueItem)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aQueItem != NULL, PANIC(KHCICmdQPanic, EAttemptToAddNullCommand));

	return DoAddCommandL(*aQueItem, iPriorityCommandQ, EStarted);
	}

/*virtual*/ TUint CHCICmdQController::MhcqAddPriorityCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aCommandData != NULL, PANIC(KHCICmdQPanic, EAttemptToAddNullCommand));
	__ASSERT_ALWAYS(iCommandAllocator != NULL, PANIC(KHCICmdQPanic, EObjectNotInitialised));
	
	CHCICommandQItem* aQueItem = CHCICommandQItem::NewL(*iCommandAllocator, 
														aCmdProgressRecipient, 
														aCommandData);
	return MhcqAddPriorityCommandL(aQueItem);
	}

/*virtual*/ TUint CHCICmdQController::MhcqAddInitCommandL(CHCICommandQItem* aQueItem)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aQueItem != NULL, PANIC(KHCICmdQPanic, EAttemptToAddNullCommand));
	__ASSERT_ALWAYS((iCmdQControllerState != EStarted), PANIC(KHCICmdQPanic, EInitCmdAfterStart));

	TUint ret = DoAddCommandL(*aQueItem, iInitCommandQ, EInitialising);
	// Mark as an Initialisation command.
	aQueItem->SetInitialisationCmd();
	return ret;
	}

/*virtual*/ TUint CHCICmdQController::MhcqAddInitCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aCommandData != NULL, PANIC(KHCICmdQPanic, EAttemptToAddNullCommand));
	__ASSERT_ALWAYS(iCommandAllocator != NULL, PANIC(KHCICmdQPanic, EObjectNotInitialised));

	CHCICommandQItem* aQueItem = CHCICommandQItem::NewL(*iCommandAllocator, 
														aCmdProgressRecipient, 
														aCommandData);
	return MhcqAddInitCommandL(aQueItem);
	}

/*virtual*/ TInt CHCICmdQController::MhcqRemoveCommand(TUint aCommandId,
													   const MHCICommandQueueClient& aCmdOriginator)
	{
	LOG_FUNC

	CHCICommandQItem* pendingCmd = NULL;
	CHCICommandQItem* lockedCmd = NULL;
	TInt retVal = KErrNotFound;
	
	pendingCmd = ScanQueueByQId(iNormalCommandQ, aCommandId);
	if(!pendingCmd)
		{
		pendingCmd = ScanQueueByQId(iInitCommandQ, aCommandId);
		if (!pendingCmd)
			{
			pendingCmd = ScanQueueByQId(iPriorityCommandQ, aCommandId);
			if (!pendingCmd)
				{
				pendingCmd = ScanQueueByQId(iWorkaroundCommandQ, aCommandId);
				if(!pendingCmd)
					{
					lockedCmd = ScanQueueByQId(iSentCommandQ, aCommandId);
					if(!lockedCmd)
						{
						lockedCmd = ScanQueueByQId(iResendCommandQ, aCommandId);
						}
					}
				}
			}
		}
	
	if(lockedCmd)
		{
		__ASSERT_ALWAYS(lockedCmd->Client() == &aCmdOriginator, PANIC(KHCICmdQPanic, ETryingToDeleteCommandNotOwned));
		retVal = KErrLocked;
		}
	else if(pendingCmd && pendingCmd->IsParent())
		{
		__ASSERT_ALWAYS(pendingCmd->Client() == &aCmdOriginator, PANIC(KHCICmdQPanic, ETryingToDeleteCommandNotOwned));
		retVal = KErrLocked;
		}
	else if(pendingCmd)
		{
		__ASSERT_ALWAYS(pendingCmd->Client() == &aCmdOriginator, PANIC(KHCICmdQPanic, ETryingToDeleteCommandNotOwned));
		__ASSERT_ALWAYS(!pendingCmd->IsPreChild(), PANIC(KHCICmdQPanic, ETryingToDeletePreChildCommandById));
		__ASSERT_ALWAYS(!pendingCmd->IsPostChild(), PANIC(KHCICmdQPanic, ETryingToDeletePostChildCommandById));		
		DeleteCommand(pendingCmd);
		retVal = KErrNone;
		}

	return retVal;
	}

void CHCICmdQController::HandleCommandRemoval(CHCICommandQItem*& aCmd, TBool aCanDelete)
	{
	LOG_FUNC
	ASSERT_DEBUG(aCmd);
	if(aCanDelete && !aCmd->IsParent())
		{
		// The client should never be the originator of child commands
		__ASSERT_ALWAYS(!aCmd->IsPreChild(), PANIC(KHCICmdQPanic, ETryingToDeletePreChildCommand));
		__ASSERT_ALWAYS(!aCmd->IsPostChild(), PANIC(KHCICmdQPanic, ETryingToDeletePostChildCommand));
		DeleteCommand(aCmd);
		}
	else
		{
		aCmd->DetachClient();
		}
	}

void CHCICmdQController::HandleCommandRemoval(CHCICommandQItem& aCmd, TBool aCanDelete)
	{
	LOG_FUNC
	CHCICommandQItem* cmd = &aCmd;
	HandleCommandRemoval(cmd, aCanDelete);
	}

void CHCICmdQController::RemoveAllCommands(TDblQueIter<CHCICommandQItem> aIter,
											const MHCICommandQueueClient& aCmdOriginator,
											TBool aCanDelete)
	{
	LOG_FUNC
	
	for (CHCICommandQItem* item(aIter++);item != NULL;item = aIter++)
		{
		if(item->Client() == &aCmdOriginator)
			{
			HandleCommandRemoval(*item, aCanDelete);
			}
		}
	}

/*virtual*/ void  CHCICmdQController::MhcqRemoveAllCommands(const MHCICommandQueueClient& aCmdOriginator)
	{
	LOG_FUNC

	TDblQueIter<CHCICommandQItem> initIter(iInitCommandQ);
	RemoveAllCommands(initIter, aCmdOriginator, ETrue);

	TDblQueIter<CHCICommandQItem> normalIter(iNormalCommandQ);
	RemoveAllCommands(normalIter, aCmdOriginator, ETrue);

	TDblQueIter<CHCICommandQItem> priorityIter(iPriorityCommandQ);
	RemoveAllCommands(priorityIter, aCmdOriginator, ETrue);

	TDblQueIter<CHCICommandQItem> workaroundIter(iWorkaroundCommandQ);
	RemoveAllCommands(workaroundIter, aCmdOriginator, ETrue);

	TDblQueIter<CHCICommandQItem> sentIter(iSentCommandQ);
	RemoveAllCommands(sentIter, aCmdOriginator, EFalse);

	TDblQueIter<CHCICommandQItem> resendIter(iResendCommandQ);
	RemoveAllCommands(resendIter, aCmdOriginator, EFalse);

	if(iSendingCommand)
		{
		if(iSendingCommand->Client() == &aCmdOriginator)
			{
			HandleCommandRemoval(iSendingCommand, ETrue);
			}
		}
	}

void CHCICmdQController::CleanUpQueue(TDblQueIter<CHCICommandQItem> aIter)
	{
	LOG_FUNC
	
	for (CHCICommandQItem* item(aIter++);item != NULL;item = aIter++)
		{
		DeleteCommand(item);
		}
	}

/*virtual*/ TUint CHCICmdQController::MhcqMaxHciCommandTimeout() const
	{
	LOG_FUNC
	
	return iMaxHciCommandTimeout;
	}

/*virtual*/ TAny* CHCICmdQController::MhcqQdpPluginInterface(TUid aUid) const
	{
	LOG_FUNC

	__ASSERT_ALWAYS((aUid != TUid::Uid(KHCICmdQueueDecisionInterfaceUid))
					&& (aUid != TUid::Uid(KHCICmdQueueDecisionEventModifierInterfaceUid)),
					PANIC(KHCICmdQPanic, EIllegalRequestForQdpInterface));
	return (iQdpPlugin) ? iQdpPlugin->Interface(aUid) : NULL;
	}

/** 
Virtuals from MHCICommandEventObserver
*/

/*virtual*/ void CHCICmdQController::MhceoEventNotification(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	
	ProcessEvent(aEvent, ETrue);
	}

void CHCICmdQController::ProcessEvent(const THCIEventBase& aEvent, TBool aSendToQdp)
	{
	LOG_FUNC
	
	// Process command credits.
	THCIEventCode eventCode(aEvent.EventCode());
	if (eventCode == ECommandCompleteEvent)
		{

		const THCICommandCompleteEvent& event(THCICommandCompleteEvent::Cast(aEvent));
		UpdateCommandCredits(event.NumHCICommandPackets());

		if (event.CommandOpcode() == KNopOpcode)
			{
			// This is a special command complete event that does not
			// complete an event. Command_Opcode, 0x0000 is a NOP, and
			// can be used to change the number of outstanding HCI
			// command packets that the Host can send before waiting.

			// Nothing else to do for this event, so throw it away
			return;
			}
		}
	else if (eventCode == ECommandStatusEvent)
		{
		TCommandStatusEvent& event = TCommandStatusEvent::Cast(aEvent);
		UpdateCommandCredits(event.NumHCICommandPackets());

		if (event.CommandOpcode() == KNopOpcode)
			{
			// Same as above for NOP command complete event.
			
			// Nothing else to do for this event, so throw it away
			return;
			}
		}

	// Iterate through sent queue to try and match the event to a sent command.
	CHCICommandQItem* cmd(0);
	TDblQueIter<CHCICommandQItem> qIter(iSentCommandQ);
	TBool matchesCmd(EFalse);
	TBool concludesCmd(EFalse);
	TBool continueMatching(EFalse);
	TBool eventMatched(EFalse);

	while ((qIter != NULL) && (!eventMatched || continueMatching))
		{
		cmd = qIter++;		
		cmd->Command().Match(aEvent, matchesCmd, concludesCmd, continueMatching);
		if (matchesCmd)
			{
			eventMatched = ETrue;
			
			// If the event concludes the command then remove it from the sent queue
			if (concludesCmd)
				{
				cmd->iLink.Deque();
				}
			
			if (aSendToQdp)
				{
				// Notify QDP.
				if (iQdpEventModifier)
					{
					THCIEventCode origEventCode = aEvent.EventCode();
					iQdpEventModifier->MhcqemiMatchedEventReceived(const_cast<THCIEventBase&>(aEvent), *cmd);
					__ASSERT_ALWAYS(origEventCode == aEvent.EventCode(), PANIC(KHCICmdQPanic, EQdpTryingToChangeEventCode));
					}
				else
					{
					iQdp->MhcqdiMatchedEventReceived(aEvent, *cmd);
					}
				}
		
			// Process the matched event.
			if (aEvent.ErrorCode() == EOK)
				{
				ProcessMatchedEvent(aEvent, cmd, concludesCmd);
				}
			else
				{
				ProcessMatchedErrorEvent(aEvent, cmd, concludesCmd, aSendToQdp);
				}
			}
		}

	if (!eventMatched)
		{
		// Process the unmatched event.
		ProcessUnmatchedEvent(aEvent, aSendToQdp);
		}
	
	// Reschedule if there are commands queued.
	if (AnyCmdsToSend())
		{
		iAsyncCallBackForSend->CallBack();
		}

	}

void CHCICmdQController::MhcquiInjectEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	
	ProcessEvent(aEvent, EFalse);
	}

CHCICommandQItem* CHCICmdQController::MhcquiFindOutstandingCommand(THCIOpcode aOpcode)
	{
	LOG_FUNC
	
	return ScanQueueByOpcode(iSentCommandQ, aOpcode);
	}


/**
Constructor.
*/
CHCICmdQController::CHCICmdQController()
  : iMaxHciCommandTimeout(KDefaultMaxHciCommandTimeout),
	iQueueStarvationTimeout(2 * iMaxHciCommandTimeout),
	iInitCommandQ(_FOFF(CHCICommandQItem,iLink)),
	iNormalCommandQ(_FOFF(CHCICommandQItem,iLink)),
	iPriorityCommandQ(_FOFF(CHCICommandQItem,iLink)),
	iWorkaroundCommandQ(_FOFF(CHCICommandQItem,iLink)),
	iSentCommandQ(_FOFF(CHCICommandQItem,iLink)),
	iResendCommandQ(_FOFF(CHCICommandQItem,iLink)),
	iCmdQControllerState(EUninitialised),
	iNextCommandQItemId(1)

	{
	CONNECT_LOGGER
	LOG_FUNC
	}

void CHCICmdQController::ConstructL()
	{
	LOG_FUNC

	// Add Async Callbacks
	TCallBack resetCallBack(AsyncCallBackForReset, this);

	iAsyncCallBackForReset =
		new (ELeave)CAsyncCallBack(resetCallBack, CActive::EPriorityStandard);

	TCallBack startCallBack(AsyncCallBackForSend, this);

	iAsyncCallBackForSend =
		new (ELeave)CAsyncCallBack(startCallBack, CActive::EPriorityStandard);

	iQStarvationTimer = CHCICmdQStarvationTimer::NewL();

	// Create HCI Utility library
	iHciUtil = CHciUtil::NewL(KHciCommandQueueComponentName);

	// If we can't open the ini file then this will be treated in the same way
	// as not reading a valid UID from the ini file.
	TRAP_IGNORE(iHciUtil->OpenIniFileL());
	
	_LIT(KSection, "QDP");
	_LIT(KQdpUidTag, "EcomUid");

	TUid qdpImplUid = TUid::Null();
	TRAPD(err, qdpImplUid = iHciUtil->GetUidFromFileL(KSection, KQdpUidTag));
	
	if (err == KErrNone)
		{
		// Valid UID found, load it
		iQdpPlugin = CHCICmdQueueDecisionPlugin::NewL(qdpImplUid);
		}
	else
		{
		// No UID found in ini file, attempt to load single instance of 
		// implementation
		iQdpPlugin = CHCICmdQueueDecisionPlugin::NewL();
		}

	iQdp = reinterpret_cast<MHCICmdQueueDecisionInterface*>(iQdpPlugin->Interface(TUid::Uid(KHCICmdQueueDecisionInterfaceUid)));
	iQdpEventModifier = reinterpret_cast<MHCICmdQueueEventModifierInterface*>(iQdpPlugin->Interface(TUid::Uid(KHCICmdQueueDecisionEventModifierInterfaceUid)));

	MHCICmdQueueUtilityUser* qdpUtilityUser = reinterpret_cast<MHCICmdQueueUtilityUser*>(iQdpPlugin->Interface(TUid::Uid(KHCICmdQueueUtilityUserUid)));
	
	if (qdpUtilityUser)
		{
		qdpUtilityUser->MhcquuSetUtilitiesProvider(*this);
		}
	
	// Read timeout values
	_LIT(KMaxHciCommandTimeoutTag, "MaxHciCommandTimeout");
	_LIT(KQueueStarvationTimeoutTag, "QueueStarvationTimeout");

	// Don't worry if we cannot read timeouts from file, already initialised
	// to default values
	TRAP_IGNORE(iMaxHciCommandTimeout = iHciUtil->GetValueFromFileL(KSection, KMaxHciCommandTimeoutTag));
	TRAP_IGNORE(iQueueStarvationTimeout = iHciUtil->GetValueFromFileL(KSection, KQueueStarvationTimeoutTag));
	
	__ASSERT_DEBUG(iMaxHciCommandTimeout < iQueueStarvationTimeout, PANIC(KHCICmdQPanic, EQStarvationTimerNotGreaterThanMaxCmdTimer));
	iQdp->MhcqdiSetTimeouts(iQueueStarvationTimeout, iMaxHciCommandTimeout);
	iQdp->MhcqdiSetHCICommandQueue(*this);

	// Reset the QDP.
	iCommandCredits = iQdp->MhcqdiReset();
	}

/**
Tests for the specified TQueueStateBits block(s) being set.
*/		
inline TBool CHCICmdQController::Blocked(TUint aBlocksToCheck, TUint aBlocksToBypass)
	{
	return Blocked(iCommandQState, aBlocksToCheck, aBlocksToBypass);
	}

/**
Tests for the specified TQueueStateBits block(s) being set.
*/		
inline TBool CHCICmdQController::Blocked(TUint aBlockStatus, TUint aBlocksToCheck, TUint aBlocksToBypass)
	{
	return (aBlockStatus & (ValidBlocks(aBlocksToCheck) & ValidBlocks(~aBlocksToBypass)));
	}

/**
Clears the specified TQueueStateBits block(s).
*/
inline void	CHCICmdQController::ClearBlock(TUint aBlocks)
	{
	__ASSERT_DEBUG(!InvalidBlocks(aBlocks), PANIC(KHCICmdQPanic, EIllegalBlock));
	iCommandQState &= ValidBlocks(~aBlocks);
	}
	
/**
Sets the specified TQueueStateBits block(s).
*/
inline void	CHCICmdQController::SetBlock(TUint aBlocks)
	{
	__ASSERT_DEBUG(!InvalidBlocks(aBlocks), PANIC(KHCICmdQPanic, EIllegalBlock));
	iCommandQState |= ValidBlocks(aBlocks);
	}
	
/**
Masks out invalid TQueueStateBits block bit(s).
*/
inline TUint CHCICmdQController::ValidBlocks(TUint aBlocks)
	{
	return (aBlocks & EAllBlocks);
	}
	
/**
Masks out valid TQueueStateBits block bit(s).
*/
inline TUint CHCICmdQController::InvalidBlocks(TUint aBlocks)
	{
	return (aBlocks & ~EAllBlocks);
	}

/**
Check for queue block bypass conditions. Currently the only such condition
is to allow a non-credit consuming command (e.g. the Number of Host Complete
Packets command) to bypass the insufficient credits block.
*/
inline TUint CHCICmdQController::CmdBypassBlocks(const CHCICommandQItem& aCmd)
	{
	TUint bypassBlocks = ENoBlocks;
	if (aCmd.Command().CreditsConsumed() == 0)
		{
		bypassBlocks |= EInsufficientCreditBlock;
		}
	return bypassBlocks;
	}
	
/**
Updates the command credits counter and maintains the EInsufficientCreditBlock
*/
inline void CHCICmdQController::UpdateCommandCredits(TUint8 aCommandCredits)
	{
	iCommandCredits = aCommandCredits;
	
	if (iCommandCredits > 0)
		{
		// We clear block flags when sending priority commands, so there may be some
		// commands blocked on insufficient credits, but the block flag not set
		// anymore. Hence we want to schedule the callback irrespectably of the status
		// of EInsufficientCreditBlock.
		if (AnyCmdsToSend())
			{
			iAsyncCallBackForSend->CallBack();
			}
		ClearBlock(EInsufficientCreditBlock);
		}
	else
		{
		SetBlock(EInsufficientCreditBlock);	
		}
	}
	
/**
Selects the active command queue (iNormalCommandQ or iInitCommandQ) based on
current state.
*/
inline TDblQue<CHCICommandQItem>* CHCICmdQController::ActiveRegularQueue()
	{
	if (iCmdQControllerState == EStarted)
		{
		return &iNormalCommandQ;
		}
	else if ((iCmdQControllerState == EResetInit) || (iCmdQControllerState == EInitialising))
		{
		return &iInitCommandQ;
		}
		
	return NULL;
	}

/**
Returns the pointer to the queue which contains the command that would be scheduled next.
Does not take the Resend queue into account.
*/
TDblQue<CHCICommandQItem>* CHCICmdQController::QueueSendingNext()
	{
	if (!iWorkaroundCommandQ.IsEmpty())
		{
		// Workaround has highest priority (well, second to resend which we don't care about).
		return &iWorkaroundCommandQ;
		}
	else if ((iCmdQControllerState == EStarted) && !iPriorityCommandQ.IsEmpty())
		{
		// Priority comes after Workaround, but we look at it only in EStarted.
		return &iPriorityCommandQ;
		}
	else
		{
		// Normal or Initialisation or NULL, depending on the state we're in.
		return ActiveRegularQueue();
		}
	}

/**
Returns the item at the head of the specified queue, or NULL if the queue is empty.
*/	
inline CHCICommandQItem* CHCICmdQController::FirstQueueItem(TDblQue<CHCICommandQItem>& aQueue)
	{
	return (aQueue.IsEmpty() ? NULL : aQueue.First());
	}

/**
Returns the item at the tail of the specified queue, or NULL if the queue is empty.
*/	
inline CHCICommandQItem* CHCICmdQController::LastQueueItem(TDblQue<CHCICommandQItem>& aQueue)
	{
	return (aQueue.IsEmpty() ? NULL : aQueue.Last());
	}
	
/**
Saves the purge marks for the queues that need them. When a reset is performed,
the queues will be purged up to those marks.
*/
void CHCICmdQController::StorePurgeMarks()
	{
	iInitQPurgeMark	 = LastQueueItem(iInitCommandQ);
	iNormalQPurgeMark = LastQueueItem(iNormalCommandQ);
	iPriorityQPurgeMark = LastQueueItem(iPriorityCommandQ);
	iWorkaroundQPurgeMark = LastQueueItem(iWorkaroundCommandQ);
	}

/**
Purge the queues up to previously saved purge marks.
*/
void CHCICmdQController::PurgeAllQueues()
	{
	// Purge the queues to the recorded tail markers.
	PurgeQueue(iInitCommandQ, iInitQPurgeMark);
	PurgeQueue(iNormalCommandQ, iNormalQPurgeMark);
	PurgeQueue(iPriorityCommandQ, iPriorityQPurgeMark);
	PurgeQueue(iWorkaroundCommandQ, iWorkaroundQPurgeMark);
	
	iInitQPurgeMark = iNormalQPurgeMark = iPriorityQPurgeMark = iWorkaroundQPurgeMark = NULL;
	
	// Purge the sent and resend queues entirely.
	PurgeQueue(iSentCommandQ, LastQueueItem(iSentCommandQ));
	PurgeQueue(iResendCommandQ, LastQueueItem(iResendCommandQ));
	}


/**
Removes and deletes items from the head of the queue up to and including the marked
item.
*/	
void CHCICmdQController::PurgeQueue(TDblQue<CHCICommandQItem>& aQueue,
									CHCICommandQItem* aMark)
	{
	LOG_FUNC

	// A null aMark implies that there are no commands to purge.
	if(aMark)
		{
		TDblQueIter<CHCICommandQItem> qIter(aQueue);
		TBool found = EFalse;

		// Ensure that the mark is in the queue.
		while(qIter && !found)
			{
			if(aMark == qIter++)
				{
				found = ETrue;
				}
			}

		// If the mark was found remove comands upto and including
		// the mark.
		if(found)
			{
			// Reset the queue iterator.
			qIter.SetToFirst();
			found = EFalse;
			CHCICommandQItem* item = NULL;

			while(qIter && !found)
				{
				item = qIter++;
				if(item == aMark)
					{
					// Mark found.  Exit the loop after this element has been removed.
					found = ETrue;
					}
				DeleteCommand(item);
				}
			}
		}
	}

/**
Returns the first item with the specified opcode which is closest to the head of the 
specified queue, or NULL if there is no such opcode on the queue.
*/	
inline CHCICommandQItem* CHCICmdQController::ScanQueueByOpcode(TDblQue<CHCICommandQItem>& aQueue, THCIOpcode aOpcode)
	{
	LOG_FUNC

	CHCICommandQItem* item = NULL;		
	TDblQueIter<CHCICommandQItem> qIter(aQueue);
	
	while (((item = qIter) != NULL) && (item->Command().Opcode() != aOpcode))
		{
		qIter++;
		}
	
	return item;
	}

/**
Returns the item with the specified command queue ID from the specified queue, or NULL 
if there is no such command on the queue.
*/
inline CHCICommandQItem* CHCICmdQController::ScanQueueByQId(TDblQue<CHCICommandQItem>& aQueue, TUint aCmdId)
	{
	LOG_FUNC

	CHCICommandQItem* item = NULL;		
	TDblQueIter<CHCICommandQItem> qIter(aQueue);
	
	while (((item = qIter) != NULL) && (item->CommandQId() != aCmdId))
		{
		qIter++;
		}
	
	return item;
	}

/**
Performs command queue EResetting state processing. EResetting state is
reached by a call to CHCICmdQController::Reset().
*/	
void CHCICmdQController::DoReset()
	{
	LOG_FUNC

	// Should only be reached if Reset has been called
	__ASSERT_ALWAYS((iCmdQControllerState == EResetInit) || 
					(iCmdQControllerState == EResetting), 
					PANIC(KHCICmdQPanic, EInvalidStateTransition));

	// Delete the parent command of possibly ongoing workaround.
	// Wee need to do it here separately, because the command
	// is not reachable any other way if it's in post-workaround phase
	// - it's already been removed from its queue.
	DeleteCommand(iParentCommand);

	PurgeAllQueues();

	// Reset the QDP.
	iCommandCredits = iQdp->MhcqdiReset();
	
	// Delete any command waiting to be sent.
	DeleteCommand(iSendingCommand);
	
	// Clear the block flags and execute state transition.
	iCommandQState = 0;
	
	// Initialise could have been called during async break so check which
	// state we should move into
	if (iCmdQControllerState == EResetInit)
		{
		iCmdQControllerState = EInitialising;
		}
	else
		{
		// Acording to the ASSERT this must be EResetting
		iCmdQControllerState = EUninitialised;
		}
		
	// Reschedule if there are commands queued and we're ready to send.
	if ((iCmdQControllerState == EInitialising) && CmdsQueued(&iInitCommandQ))
		{
		iAsyncCallBackForSend->CallBack();
		}
	}

/**
The main queue processing routine. Schedules the next command and asks
HCTL to send.
*/
void CHCICmdQController::TryToSend()
	{
	LOG_FUNC

	__ASSERT_DEBUG((iCmdQControllerState == EInitialising) || (iCmdQControllerState == EStarted),
				   PANIC(KHCICmdQPanic, ETryToSendWhileUnoperational));
	
	if ((iCmdQControllerState != EInitialising) && (iCmdQControllerState != EStarted))
		{
		// This is just a safety net. We shouldn't have been scheduled to run
		// if we're not ready. But even if we've been mistakenly scheduled or
		// the state changed in the meantime, this return makes it harmless.
		return;
		}
	
	TBool stopProcessing(ProcessResendQueue());
	if (!stopProcessing)
		{
		stopProcessing = ProcessWorkaroundQueue();
		if (!stopProcessing)
			{
			if (iCmdQControllerState == EStarted)
				{
				stopProcessing = ProcessPriorityQueue();
				}
			if (!stopProcessing)
				{
				TDblQue<CHCICommandQItem>* queue = ActiveRegularQueue();
				if (queue != NULL)
					{
					ProcessRegularQueue(*queue);
					}
				}
			}
		}
	}

/**
Returns ETrue if our current state allows commands to be queued.
*/
inline TBool CHCICmdQController::CanAddCommands()
	{
	switch (iCmdQControllerState)
		{
	case EStarted:
	case EResetInit:
	case EInitialising:
		return ETrue;
	default:
		return EFalse;
		}
	}

/**
Tests if there are items queued on the specified queue.
*/
inline TBool CHCICmdQController::CmdsQueued(TDblQue<CHCICommandQItem>* aQueue)
	{
	if (aQueue != NULL)
		{
		return (!aQueue->IsEmpty());
		}
	return EFalse;
	}

/**
Tests if there are items queued on any of the queues active in current state.
*/
inline TBool CHCICmdQController::AnyCmdsToSend()
	{
	TDblQue<CHCICommandQItem>* activeQ = ActiveRegularQueue();

	// Need to check those two no matter which state we're in.
	TBool commonQueuesEmpty = iWorkaroundCommandQ.IsEmpty() && iResendCommandQ.IsEmpty();

	if (activeQ == &iInitCommandQ)
		{
		return !iInitCommandQ.IsEmpty() || !commonQueuesEmpty;
		}
	else
		{
		return !iNormalCommandQ.IsEmpty() || !iPriorityCommandQ.IsEmpty() || !commonQueuesEmpty;
		}
	}

/**
Returns the next Command Queue Item Id
*/
inline TUint CHCICmdQController::NextCommandQueueItemId()
	{
	__ASSERT_DEBUG(iNextCommandQItemId != KInvalidCommandQueueItemId,
				   PANIC(KHCICmdQPanic, EInvalidCommandQueueItemId));
	iNextCommandQItemId++;
	if(iNextCommandQItemId == KInvalidCommandQueueItemId)
		{
		iNextCommandQItemId++;
		}
	return iNextCommandQItemId;
	}
	
/**
Processes the resend queue.
Returns ETrue if the scheduling loop shouldn't process lower priority queues
(which is when it schedules a command for resend) and EFalse if the loop should
go on.
*/
TBool CHCICmdQController::ProcessResendQueue()
	{
	LOG_FUNC

	if (!CmdsQueued(&iResendCommandQ))
		{
		return EFalse;
		}
	
	CHCICommandQItem* cmd = iResendCommandQ.First();
	TUint bypassBlocks = CmdBypassBlocks(*cmd);
	if (Blocked(EResendQueueBlocks, bypassBlocks))
		{
		// We're the highest priority queue, but allow the other queues to try
		// and send if we have commands queued but are blocked - "Only one
		// active resend at a time" is still fulfilled.
		return EFalse;
		}

	if (OkToSendCommand(cmd, bypassBlocks))
		{
		// Process command.
		SendCommand(*cmd);
		SetBlock(EResendBlock);
		return ETrue;
		}

	return EFalse;
	}

/**
Processes the workaround queue.
Returns ETrue if the scheduling loop shouldn't process lower priority queues
(which is when the queue is non-empty) and EFalse if the loop should go on.
*/
TBool CHCICmdQController::ProcessWorkaroundQueue()
	{
	LOG_FUNC

	if (!CmdsQueued(&iWorkaroundCommandQ))
		{
		return EFalse;
		}

	CHCICommandQItem* cmd = iWorkaroundCommandQ.First();
	switch (cmd->Type())
		{
	case CHCICommandQItem::EIndeterminate:
		// Start of workaround sequence, set up the first command.
		cmd->SetType(CHCICommandQItem::EParent);
		SetUpNextWorkaroundCmd(NULL, NULL);
		cmd = iWorkaroundCommandQ.First();
		UpdateStarvationTimer();
		break;
		
	case CHCICommandQItem::EPreChild:
	case CHCICommandQItem::EPostChild:
	case CHCICommandQItem::EParent:
		// In the middle of a workaround.
		// Nothing to do here really, command setup has been done by calling
		// SetUpNextWorkaroundCmd() in Process*Event on completion of the previous
		// command in the workaround sequence.
		break;
	
	default:
		__ASSERT_DEBUG(EFalse, PANIC(KHCICmdQPanic, EInvalidCmdQueueItemType));
		break;
		}

#ifdef _DEBUG
	// We should only ever have at maximum one parent and one child on the 
	// workaround queue at any time.
	TDblQueIter<CHCICommandQItem> iter(iWorkaroundCommandQ);
	TUint count = 0;
	while(iter++)
		{
		count++;
		}
	__ASSERT_DEBUG(count <= 2, PANIC(KHCICmdQPanic, ETooManyItemsOnWorkaroundQueue));
#endif // _DEBUG
	
	// Now that we're sure we have pre-workaraound set up we can check blocks.
	TUint bypassBlocks = CmdBypassBlocks(*cmd);
	if (Blocked(ESendQueueBlocks, bypassBlocks))
		{
		// Don't allow lower priority queues to send until we're empty.
		return ETrue;
		}
	
	if (OkToSendCommand(cmd, bypassBlocks))
		{
		SendCommand(*cmd); 
		UpdateStarvationTimer();
		SetBlock(EWorkaroundBlock);
		}

	// Don't allow lower priority queues to send until we're empty.
	return ETrue;
	}

/**
Processes the priority queue.
Returns ETrue if the scheduling loop shouldn't process lower priority queues
(which is when the queue is non-empty) and EFalse if the loop should go on.
*/
TBool CHCICmdQController::ProcessPriorityQueue()
	{
	LOG_FUNC

	if (!CmdsQueued(&iPriorityCommandQ))
		{
		return EFalse;
		}
	else if (Blocked(ESendQueueBlocks, ECachingCommandBlocks))
		{
		// Check blocks excluding ECachingCommandBlocks
		// The blocks we're checking don't depend on particular commands, so we can
		// safely return here.
		// Don't allow lower priority queues to send until we're empty.
		return ETrue;
		}

	// Search the queue for the first non-blocking command.
	// Note that this is still just heurisitics, because even if we find a non-blocking
	// command, it may still yield a blocking workaround.
	CHCICommandQItem* cmd = NULL;
	TDblQueIter<CHCICommandQItem> i(iPriorityCommandQ);
	while ((cmd = i++) != NULL)
		{
		// For every command we clear the cached blocks as they apply to the last command
		// we attempted to send, using OkToSendCommand. As the priority queue can send out
		// of order, i.e. not just the head command, the cached blocks need to be
		// re-evaluated for each command until we find the one we intend to send.
		ClearBlock(ECachingCommandBlocks);
		if (OkToSendCommand(cmd, CmdBypassBlocks(*cmd)))
			{
			// Found available command.
			if (iQdp->MhcqdiDoesCommandRequireWorkaround(*cmd))
				{
				// Start of workaround sequence.
				// Move onto the workaround queue and kick the scheduling loop.
				cmd->iLink.Deque();
				iWorkaroundCommandQ.AddLast(*cmd);
				iAsyncCallBackForSend->CallBack();
				}
			else
				{
				// Non-workaround.
				cmd->SetType(CHCICommandQItem::ENormal);
				SendCommand(*cmd);
				UpdateStarvationTimer();
				}
			break;
			}
		}

	// At this point we are in one of the following states:
	//  - All priority commands are either blocked or have errored and been deleted in 
	//    OkToSendCommand. If any commands are errored then the scheduling loop will 
	//    also have been kick. 
	//  - A priority command has been added to the workaround queue and the scheduling 
	//    loop has been kick.
	//  - A priority command has been sent.
	//
	// In any one of these conditions we want to stop processing further queues so can
	// always return true.
	return ETrue;
	}

/**
Processes a regular queue - normal or initialization command queue.
*/
void CHCICmdQController::ProcessRegularQueue(TDblQue<CHCICommandQItem>& aQueue)
	{
	LOG_FUNC

	if (!CmdsQueued(&aQueue))
		{
		return;
		}

	CHCICommandQItem* cmd = aQueue.First();
	TUint bypassBlocks = CmdBypassBlocks(*cmd);
	if (Blocked(ESendQueueBlocks, bypassBlocks))
		{
		return;
		}

	if (cmd->Type() == CHCICommandQItem::EIndeterminate)
		{
		// It's the first time we're looking into cmd. See if it needs a workaround.
		if (iQdp->MhcqdiDoesCommandRequireWorkaround(*cmd))
			{
			// Start of workaround sequence.
			// Move it onto the workaround queue and kick the scheduling loop.
			cmd->iLink.Deque();
			iWorkaroundCommandQ.AddLast(*cmd);
			iAsyncCallBackForSend->CallBack();
			
			return;
			}
		else
			{
			// Non-workaround.
			cmd->SetType(CHCICommandQItem::ENormal);
			}
		}
	if (OkToSendCommand(cmd, bypassBlocks))
		{
		SendCommand(*cmd); 
		UpdateStarvationTimer();
		}
	}

/**
Process workaround related commands, and is invoked every time a
workaround completes.  As the QDP interface needs to be passed the
concluding event of each workaround command, and events are used
synchronously and have a limited lifetime, this means workaround
handling has to be done at the same time as the event processing.

This processing ensures that the head of the workaround queue contains
the next command in the workaround. It also tidies up and deletes
commands as necessary. Child commands are deleted once their events
have been reported to the QDP. The parent command lives until the end
of the workaround.
*/
void CHCICmdQController::SetUpNextWorkaroundCmd(CHCICommandQItem* aPreviousCmd,
												const THCIEventBase* aPreviousCmdResult)
	{
	LOG_FUNC

	// Update blocks.
	ClearBlock(EWorkaroundBlock);

	// Get parent
	if (iParentCommand == NULL)
		{
		iParentCommand = iWorkaroundCommandQ.First();
		}

	__ASSERT_ALWAYS(iParentCommand, PANIC(KHCICmdQPanic, EObjectNotInitialised));

	// Determine next command in the workaround sequence.
	CHCICommandQItem* child = NULL;
	CHCICommandQItem::TType type;

	if (iParentCommand->SentCount() == 0)
		{
		child = iQdp->MhcqdiGetPreChildCommand(*iParentCommand, aPreviousCmd, aPreviousCmdResult);
		if (child != NULL)
			{
			// Pre-workaround.
			type = CHCICommandQItem::EPreChild;
			}
		else
			{
			// Transitioning to post-workaround.
			type = CHCICommandQItem::EParent;
			}
		}
	else
		{
		__ASSERT_ALWAYS(aPreviousCmd, PANIC(KHCICmdQPanic, EObjectNotInitialised));
		if (aPreviousCmd->Type() == CHCICommandQItem::EParent)
			{
			// Transitioning to post-workaround.
			child = iQdp->MhcqdiGetPostChildCommand(*iParentCommand, NULL, aPreviousCmdResult);
			}
		else
			{
			// Post-workaround.
			child = iQdp->MhcqdiGetPostChildCommand(*iParentCommand, aPreviousCmd, aPreviousCmdResult);
			}
			
		type = CHCICommandQItem::EPostChild;
		}
	
	// Delete previous command (if not parent).
	if ((aPreviousCmd != NULL) &&
		(aPreviousCmd->Type() != CHCICommandQItem::EParent))
		{
		DeleteCommand(aPreviousCmd);
		}

	if (child != NULL)
		{
		// Enqueue child to head of queue.
		child->SetType(type);
		iWorkaroundCommandQ.AddFirst(*child);
		}
	else if (type != CHCICommandQItem::EParent)
		{
		// Give the QDP a chance to update state within the stack if required
		THCIEventBase* fakedEvent = NULL;
		while ((fakedEvent = iQdp->MhcqdiGetFakedUnsolicitedEvent(*iParentCommand, fakedEvent)) != NULL)
			{
			iUnmatchedEventObserver->MhcqcCommandEventReceived(*fakedEvent, NULL);
			}
			
		// End of workaround sequence, delete parent.
		DeleteCommand(iParentCommand);
		}
	}

/**
A helper for restarting the Starvation Timer during command addition/sendout.
Determines the next command that will be sent and restarts the timer with
the ID of that command.
Caveat: this always restarts (or cancels, hence Update rather than just Restart in the name)
the timer, so use it only when you know that the command that will be sent next has really changed.
*/
void CHCICmdQController::UpdateStarvationTimer()
	{
	// Determine the id of the next command to be executed.
	// Don't care about resend queue, the starvation timer shouldn't be restarted on resends.	

	TUint cmdId = KInvalidCommandQueueItemId;

	if (!iWorkaroundCommandQ.IsEmpty())
		{
		cmdId = iWorkaroundCommandQ.First()->CommandQId();
		}
	else if ((iCmdQControllerState == EStarted) && !iPriorityCommandQ.IsEmpty())
		{
		// Only send off priority queue in EStarted (i.e. not in EInitialising)
		// Never know which priority command will be sent next.
		cmdId = KInvalidCommandQueueItemId;
		}
	else
		{
		TDblQue<CHCICommandQItem>* queue = ActiveRegularQueue();
		if (queue && !queue->IsEmpty())
			{
			cmdId = queue->First()->CommandQId();
			}
		else
			{
			// No command to send.
			iQStarvationTimer->Cancel();
			return;
			}
		}
	iQStarvationTimer->Restart(iQueueStarvationTimeout, cmdId, *this);
	}

/**
Dequeues and deletes aItem from its queue.
*/
void CHCICmdQController::DeleteCommand(CHCICommandQItem* &aItem)
	{
	LOG_FUNC

	if (!aItem)
		{
		return;
		}
	aItem->iLink.Deque();
	iQdp->MhcqdiCommandAboutToBeDeleted(*aItem);
	delete aItem;
	aItem = NULL;
	UpdateStarvationTimer();
	}

/**
Helper to be called on command addition.
Returns ETrue if the command that will be sent next changed after we added a command to aQueue.
Ignores the Resend queue.
*/
TBool CHCICmdQController::NextCommandChanged(const TDblQue<CHCICommandQItem> &aQueue)
	{
	LOG_FUNC

	if (&aQueue != QueueSendingNext())
		{
		// We added a command to aQueue, but it's still another queue
		// that sends next, so we didn't change the next command to be sent.
		return EFalse;
		}
	else // aQueue is sending next.
		{
		if (&aQueue == &iPriorityCommandQ)
			{
			// PriorityQ is not FIFO, so any added command can potentially
			// be sent next irrespectably of whether the queue was empty or not.
			return ETrue;
			}
		else
			{
			// Rest of queues is FIFO, so the next command to be sent changed
			// if the queue had been empty when we added the command.
			return aQueue.First() == aQueue.Last();
			}
		}
	}

/**
The general command addition routine. Adds aQueItem to aQueue and schedules
the command processing loop if current state is equal to aActiveState.
*/
TUint CHCICmdQController::DoAddCommandL(CHCICommandQItem& aQueItem,
		TDblQue<CHCICommandQItem> &aQueue, TCmdQControllerStates aActiveState)
	{
	LOG_FUNC	

	// Ensure we are in a state that allows commands to be added.
	if(!CanAddCommands())
		{
		delete &aQueItem;
		User::Leave(KErrHardwareNotAvailable);
		}
		
	// Assign a unique CommandQId.
	aQueItem.SetCommandQId(NextCommandQueueItemId());

	aQueue.AddLast(aQueItem);
	// Restart queue starvation timer if this is the next command to be sent.
	if (NextCommandChanged(aQueue))
		{
		// If this is priority queue, we don't really know which command should be executed
		// next, so we feed the timer with KInvalidCommandQueueItemId in that case.
		TUint cmdId = KInvalidCommandQueueItemId;
		if (&aQueue != &iPriorityCommandQ)
			{
			cmdId = aQueItem.CommandQId();
			}
		iQStarvationTimer->Restart(iQueueStarvationTimeout, cmdId, *this);
		}

	// Only schedule if we're in the state in which given queue should be considered for scheduling.
	if (iCmdQControllerState == aActiveState)
		{
		iAsyncCallBackForSend->CallBack();
		}

	return aQueItem.CommandQId();
	}

/**
Evaluates various blocking conditions to determine if the command can be sent.
aCmdQItem is passed by reference to pointer as it can be deleted and NULLified in case of error.
@return whether the command can be sent.
*/
TBool CHCICmdQController::OkToSendCommand(CHCICommandQItem*& aCmdQItem, TUint aBypassBlocks)
	{
	TInt result = KErrNone;
	LOG_FUNC
	
	__ASSERT_ALWAYS(aCmdQItem, PANIC(KHCICmdQPanic, ENullCmdQItemPtr));
	
	if((iCommandCredits < aCmdQItem->Command().CreditsConsumed()) && 
	   (!(aBypassBlocks & EInsufficientCreditBlock))) 
		{
		// Insufficient HCI credits. Block and suspend processing queue.
		SetBlock(EInsufficientCreditBlock);
		}
	else 
		{
		CHCICommandQItem* const duplicate = ScanQueueByOpcode(iSentCommandQ,
															  aCmdQItem->Command().Opcode());
		if (duplicate && !(aBypassBlocks & EDuplicatedOpcodeBlock))
			{
			// Duplicate command pending. Block and suspend processing queue.
			SetBlock(EDuplicatedOpcodeBlock);

			// Need to mark the duplicate so that EDuplicatedOpcodeBlock is cleared
			// on its completion.
			duplicate->SetDuplicatedOpcode(ETrue);
			}
		else
			{
			const TDblQue<const CHCICommandQItem>* sentQ = (reinterpret_cast<const TDblQue<const CHCICommandQItem>*>(&iSentCommandQ));

			result = iQdp->MhcqdiCanSend(*aCmdQItem, *sentQ);
			// KErrNone if aCommand is to be sent, EBlock if Command
			// is to be delayed.
			if(result < 0)
				{
				if(aCmdQItem->Client())
					{
					aCmdQItem->Client()->MhcqcCommandErrored(result, &aCmdQItem->Command());	
					}
				
				// Dequeue, delete & set to NULL.
				DeleteCommand(aCmdQItem);
				
				if(AnyCmdsToSend())
					{
					iAsyncCallBackForSend->CallBack();
					}
				}
			else if(result == MHCICmdQueueDecisionInterface::EBlock && !(aBypassBlocks & ECanSendBlock))
				{
				// QDP should never block when the sent queue is empty (would permanently block the queue.
				__ASSERT_ALWAYS((!iSentCommandQ.IsEmpty()), PANIC(KHCICmdQPanic, ECanSendBlockWhenEmpty));
				SetBlock(ECanSendBlock);
				}
			}
		}

	return ((!Blocked(ECachingCommandBlocks, aBypassBlocks))&&(result==KErrNone));
	}
	
/**
Submits a request to the link muxer for permission to send a command. 
*/
void CHCICmdQController::SendCommand(CHCICommandQItem& aCmdQItem)
	{
	LOG_FUNC

	__ASSERT_DEBUG(!iSendingCommand, PANIC(KHCICmdQPanic, ETryToSendWhilstSending));
 
	iSendingCommand = &aCmdQItem;
	// Dequeue command.
	iSendingCommand->iLink.Deque();

	// Initiate send request to HCTL, and block the queue.
	iLinkMuxer->TryToSend();
	SetBlock(ETryToSendBlock);
	}

/**
Process matched non-error events.
*/
void CHCICmdQController::ProcessMatchedEvent(const THCIEventBase& aEvent, CHCICommandQItem* aCmd, TBool aConcludesCmd)
	{
	LOG_FUNC
	
	// If required by the command, a Command Status Event should always be received before any other event related to a command.
	THCIEventCode eventCode(aEvent.EventCode());	
	if (eventCode == ECommandStatusEvent)
		{
		if (aCmd->Command().ExpectsCommandStatusEvent())
			{
			aCmd->SetReceivedCmdStatusEvent(ETrue);
			}
		else
			{
			// assert in debug that we don't get here.
			}
		}
	else 
		{
		// Non Command Status Event, clear possible CanSend block.
		ClearBlock(ECanSendBlock);
		}
		
	CHCICommandQItem::TType type = aCmd->Type();
	if ((type != CHCICommandQItem::EPreChild) &&
		(type != CHCICommandQItem::EPostChild))
		{
		// Command client is notified of non-workaround command events.
		if(aCmd->Client())
			{
			aCmd->Client()->MhcqcCommandEventReceived(aEvent, &aCmd->Command());
			}
		}
	
	if (aConcludesCmd)
		{
		// Check that if a command status is expected then it has been got before being concluded.

		// Cancel completion timer.
		aCmd->CancelCompletionTimer();
		
		// Update blocks, cleared blocks will be re-evaluated on next Run Queue
		if (aCmd->SentCount() > 1)
			{
			ClearBlock(EResendBlock);
			}
		if (aCmd->DuplicatedOpcode())
			{
			ClearBlock(EDuplicatedOpcodeBlock);
			}
						
		// Process completed command.					
		if (type == CHCICommandQItem::ENormal)
			{
			// Non-workaround command. Delete processed command.
			DeleteCommand(aCmd);
			}
		else
			{
			// Workaround in progress. Set up next workaround command.
			SetUpNextWorkaroundCmd(aCmd, &aEvent);
			}
		}
	}

/**
Process matched error events.
*/
void CHCICmdQController::ProcessMatchedErrorEvent(const THCIEventBase& aEvent, CHCICommandQItem* aCmd, TBool /* aConcludesCmd */, TBool aSendToQdp)
	{
	LOG_FUNC
	
	// Cancel completion timer. 
	aCmd->CancelCompletionTimer(); 
		
	/*
	
	Update blocks. The following blocks are not cleared:
	
	1. The Resend block untouched to avoid interfering with an in-progress resend and 
	   to ensure that only a single resend is active at a time.	
	2. The TryToSend block which is strictly managed by the HCTL flow control methods 
	   (SendCommand and DoSend). 
	3. The Workaround block to prevent possibly bypassing a valid workaround block
	   on completion of a resend.
	
	All other cleared blocks (the caching blocks) will be re-evaluated before sending
	the next command.
	*/
	ClearBlock(ECachingCommandBlocks);
	
	// Remove item from sent queue.
	aCmd->iLink.Deque();

	if (aSendToQdp && iQdp->MhcqdiMatchedErrorEventReceived(aEvent, *aCmd) == MHCICmdQueueDecisionInterface::EResendErroredCommand)
		{
		// Clear the command status event flag and enqueue to the resend queue
		aCmd->SetReceivedCmdStatusEvent(EFalse);
		if (aCmd->SentCount() > 1)
			{
			// Command was previously resent, enqueue to head of resend queue and clear Resend block
			iResendCommandQ.AddFirst(*aCmd);
			ClearBlock(EResendBlock);
			}
		else
			{
			// Command has not previously been resent, enqueue to tail of resend queue.
			iResendCommandQ.AddLast(*aCmd);			
			}
		}
	else
		{
		CHCICommandQItem::TType type = aCmd->Type();

		if ((type != CHCICommandQItem::EPreChild) &&
			(type != CHCICommandQItem::EPostChild))
			{
			// Command error event received, notify client if non-child command
			if(aCmd->Client())
				{
				aCmd->Client()->MhcqcCommandEventReceived(aEvent, &aCmd->Command());	
				}
			}

		// Process completed command.					
		if (type == CHCICommandQItem::ENormal)
			{
			// Non-workaround command. Delete processed command.
			DeleteCommand(aCmd);
			}
		else
			{
			// Workaround in progress. Set up next workaround command.
			SetUpNextWorkaroundCmd(aCmd, &aEvent);
			}											
		}
	}

/**
Processes unmatched events.
*/
void CHCICmdQController::ProcessUnmatchedEvent(const THCIEventBase& aEvent, TBool aSendToQdp)
	{
	LOG_FUNC
	
	if (aSendToQdp)
		{
		// Notify QDP.
		if (iQdpEventModifier)
			{
			THCIEventCode origEventCode = aEvent.EventCode();
			iQdpEventModifier->MhcqemiUnmatchedEventReceived(const_cast<THCIEventBase&>(aEvent));
			__ASSERT_ALWAYS(origEventCode == aEvent.EventCode(), PANIC(KHCICmdQPanic, EQdpTryingToChangeEventCode));
			}
		else
			{
			iQdp->MhcqdiUnmatchedEventReceived(aEvent);
			}
		}

	iUnmatchedEventObserver->MhcqcCommandEventReceived(aEvent, NULL);
	}

/**
Processes timed out commands.
*/
void CHCICmdQController::ProcessCommandCompletionTimeout(CHCICommandQItem* aCmd)
	{
	LOG_FUNC
		
	/*
	
	Update blocks. The following blocks are not cleared:
	
	1. The Resend block untouched to avoid interfering with an in-progress resend and 
	   to ensure that only a single resend is active at a time.	
	2. The TryToSend block which is strictly managed by the HCTL flow control methods 
	   (SendCommand and DoSend). 
	3. The Workaround block to prevent possibly bypassing a valid workaround block
	   on completion of a resend.
	
	All other cleared blocks (the caching blocks) will be re-evaluated before sending
	the next command.
	*/
	ClearBlock(ECachingCommandBlocks);
	
	// Remove item from sent queue.
	aCmd->iLink.Deque();

	TUint refundedCredits = 0;
	MHCICmdQueueDecisionInterface::TCommandTimedOutAction action;
	const TDblQue<const CHCICommandQItem>* sentQ = (reinterpret_cast<const TDblQue<const CHCICommandQItem>*>(&iSentCommandQ));

	action = iQdp->MhcqdiCommandTimedOut(*aCmd, *sentQ, iCommandCredits, refundedCredits);
	
	// Take back any refunded credits
	iCommandCredits += refundedCredits;

	if (action == MHCICmdQueueDecisionInterface::EContinueWithTimeoutEvent)
		{
		CHCICommandQItem::TType type = aCmd->Type();

		if ((type != CHCICommandQItem::EPreChild) &&
			(type != CHCICommandQItem::EPostChild))
			{
			// Command error event received, notify client if non-child command
			if(aCmd->Client())
				{
				aCmd->Client()->MhcqcCommandErrored(CHciUtil::SymbianErrorCode(EHardwareFail), &aCmd->Command());	
				}
			}

		// Process completed command.					
		if (type == CHCICommandQItem::ENormal)
			{
			// Non-workaround command. Delete processed command.
			DeleteCommand(aCmd);
			}
		else
			{
			SetUpNextWorkaroundCmd(aCmd, NULL);
			}											
		}
	else if (action == MHCICmdQueueDecisionInterface::EResendTimedOutCommand)
		{
		// Clear the command status event flag and enqueue to the resend queue
		aCmd->SetReceivedCmdStatusEvent(EFalse);
		if (aCmd->SentCount() > 1)
			{
			// Command was previously resent, enqueue to head of resend queue and clear Resend block
			iResendCommandQ.AddFirst(*aCmd);
			ClearBlock(EResendBlock);
			}
		else
			{
			// Command has not previously been resent, enqueue to tail of resend queue.
			iResendCommandQ.AddLast(*aCmd);
			}
		}
	}

// Static async CallBack methods.
/*static*/ TInt CHCICmdQController::AsyncCallBackForReset(TAny* aCmdQController)
	{
	CHCICmdQController* cmdQController = static_cast<CHCICmdQController*>(aCmdQController);
	cmdQController->DoReset();
	return EFalse;	// Don't call back any more.
	}
	
/*static*/ TInt CHCICmdQController::AsyncCallBackForSend(TAny* aCmdQController)
	{
	CHCICmdQController* cmdQController = static_cast<CHCICmdQController*>(aCmdQController);
	cmdQController->TryToSend();
	return EFalse;	// Don't call back any more.
	}

#ifdef _DEBUG
void CHCICmdQController::LogQueue(TDblQue<CHCICommandQItem>& aQueue)
	{
	TDblQueIter<CHCICommandQItem> iter(aQueue);
	while(CHCICommandQItem* item = iter++)
		{
		THCIOpcode opcode = item->Command().Opcode();
		const TUint16 KOgfMask = 0xfc00;
		TUint8 ogf = (opcode&KOgfMask) >> 10; // OGF starts at bit 10.
		TUint16 ocf = (opcode&~KOgfMask);
		LOG4(_L("Command OGF(0x%02x) OCF(0x%04x) item 0x%08x owned by 0x%08x"), ogf, ocf, item, item->Client());
		}
	}
#endif // _DEBUG


