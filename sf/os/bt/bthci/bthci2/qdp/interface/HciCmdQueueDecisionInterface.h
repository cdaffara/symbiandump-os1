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
 @publishedPartner
 @released
*/


#ifndef HCICMDQUEUEDECISIONINTERFACE_H
#define HCICMDQUEUEDECISIONINTERFACE_H

#include <e32base.h>
#include <bluetooth/hci/hcitypes.h>

class CHCICommandQItem;
class THCIEventBase;
class MPhysicalLinksState;
class MHardResetInitiator;
class MHCICommandQueue;

/**
The UID of the QDP interface.

If the QDP interface ever has to change, a new UID and associated M- class
will be created. New implementations of CHCICmdQueueDecisionPlugin may 
implement the new interface. Old (non-updated) QDPs will still work as
long as the Command Queue supports the old interface.
*/
const TInt KHCICmdQueueDecisionInterfaceUid = 0x102736F2;

/**
Mixin for the QDP interface.

This interface is to be used via the Command Queue Decision plugin.
@see CHCICmdQueueDecisionPlugin::Interface(TUid)

The QDP is design to help licensees with the following types of workarounds:
	- Automatically resending commands that have completed with an error.
	- Applying a timeout to the period between a command being sent to the 
	controller and the command completing. 
	- Adding 'child' commands before and after an existing command that the 
	Command Queue wishes to send.
	- Blocking the next command to be sent from the Command Queue depending 
	on what had already been sent to the controller.
	- Dealing with unexpected events from the controller.
*/ 
class MHCICmdQueueDecisionInterface
	{
public:
	/**
	Represents that no timer is required for the completion of a command.
	*/
	static const TUint KNoTimeoutRequired = 0;
	
	/**
	Represents the action that the QDP decides the Command Queue should take when the timeout
	for a command has expired.
	@see MhcqdiCommandTimedOut
	*/
	enum TCommandTimedOutAction
		{
		/** 
		Do not inform the command's client of the timeout. 
		This will result in the command being dropped. A QDP would wish to do this
		if it reset the controller in the MhcqdiCommandTimedOut function call.
		*/
		EDropTimeoutEvent = 0,
		/** Continue as if it had received an event with error code EHardwareFail.	*/
		EContinueWithTimeoutEvent = 1,
		/** Attempt to resend the command (command's client is not informed). */
		EResendTimedOutCommand = 2,
		};
	
	/**
	Represents the action that the QDP decides the Command Queue should take when
	a command is matched to an event which signifies an error.
	@see MhcqdiMatchedErrorEventReceived
	*/	
	enum TCommandErroredAction
		{
		/** Continue and report the error to the client who added the command. */
		EContinueWithError = 0,
		/** Attempt to resend the command (command's client is not informed). */
		EResendErroredCommand = 1,
		};
		
	/**
	Represents the action that the QDP decides the Command Queue should take when a
	command is presented to the QDP ready to be sent, in the MhcqdiCanSend function.
	@see MhcqdiCanSend
	*/
	enum TCanSendAction
		{
		/** Send the command, identical to KErrNone*/
		EContinue = KErrNone,
		/** Delay sending the command. */
		EBlock = 1,
		};

public:

	/**
	Queries whether a HCI command requires child commands to be sent before or
	after this parent command is sent for a workaround.
	
	@param aParent The command that might require a workaround.
	@return ETrue if the command does require a workaround, EFalse otherwise.
	*/
	virtual TBool MhcqdiDoesCommandRequireWorkaround(const CHCICommandQItem& aParent) = 0;
	
	
	/**
	Retrieves the next pre-child command for a parent command. A pre-child command is a 
	command that is to be sent to the HCTL before the parent command is sent.
	
	The aid the QDP in selecting the next command to be sent, it is passed the parent
	command and pointers to the previous child-command sent and its associated event.
	Note if the same command needs to be sent twice in a workaround, the QDP needs to
	create two different objects, as ownership of the resultant command is passed to
	the Command Queue.
	
	@param aParent					The parent command that the pre-child command will be for. 
	@param aPreviousWorkaroundCmd	Will be null the first time this function is called for a particular workaround.
									Ownership is not passed.
	@param aPreviousCmdResult		Will be null the first time this function is called for a particular workaround
									or if the previous command has timed out (so there is no event).
									Ownership is not passed.
	@return	The next pre-child command to be sent or NULL if there are none.
			The caller takes ownership of the command.
	*/
	virtual CHCICommandQItem* MhcqdiGetPreChildCommand(const CHCICommandQItem& aParent, 
													   const CHCICommandQItem* aPreviousWorkaroundCmd,
													   const THCIEventBase* aPreviousCmdResult) = 0;
	/**
	Retrieves the next post-child command for a parent command. A post-child command is a 
	command that is to be sent to the HCTL after the parent command has been sent.
	
	The aid the QDP in selecting the next command to be sent, it is passed the parent
	command and pointers to the previous child-command sent and its associated event.
	Note if the same command needs to be sent twice in a workaround, the QDP needs to
	create two different objects, as ownership of the resultant command is passed to
	the Command Queue.
	
	@param aParent				The parent command that the post-child command would be for. 
	@param aPreviousPostChild	Will be NULL the first time this function is called for a particular workaround.
								Ownership is not passed.
	@param aPreviousCmdResult	If aPreviousPostChild is NULL then this is the result of aParent otherwise it is 
								the result of aPreviousPostChild. This parameter can be NULL if the previous
								command timed out (so there is no event).
								Ownership is not passed.
	@return	The next pre-child command to be sent or NULL if there are none.
			The caller takes ownership of the command.
	*/
	virtual CHCICommandQItem* MhcqdiGetPostChildCommand(const CHCICommandQItem& aParent, 
														const CHCICommandQItem* aPreviousPostChild, 
														const THCIEventBase* aPreviousCmdResult) = 0;
	
	/**
	Retrieves the next faked event the QDP wishes to send (following a workaround)
	in order for it to keep the Stack synchronised with what may have been done
	(normally in the case where state cannot returned to how it was prior to the
	workaround, i.e. failures to succesfully perform the post-workaround stage)
	
	For a faked event to be sent, the QDP should create a descriptor (of sufficient
	lifetime) that contains the raw data the event is to have.  It should then use 
	this to create an appropriate THCIEventBase class, which is to be returned as 
	the faked event for the Command Queue to pass on.
	
	This function will be repeatedly called (while the QDP continues to return faked
	events) so as to retrieve the sequence of faked events required. The previous faked
	event will be provided in the call to the QDP so that it does not need to maintain
	state about how far though the sequence of faked events it is.
	
	When there are no (more) faked events required, a NULL pointer should be returned.
	
	@param aParent				The parent command that requires fake events to be generated.
	@param aPreviousFakedEvent	Will be NULL the first time this function is caled for a particular workaround, 
								otherwise it will point to an identical instance of the previously faked event 
								generated by the QDP.
								Ownership is not passed.
	@return A pointer to the faked event that is to be "received", or NULL if no more are required.
			Ownership is passed.
	*/
	virtual THCIEventBase* MhcqdiGetFakedUnsolicitedEvent(const CHCICommandQItem& aParent,
														  const THCIEventBase* aPreviousFakedEvent) = 0;
	
	/**
	Called immediately prior to a command queue item being deleted to allow the QDP to clean up
	the items QDP data member.
	
	@param aDyingCmd The CHCICommandQItem that should have its QDP data member cleaned up.
	*/
	virtual void MhcqdiCommandAboutToBeDeleted(const CHCICommandQItem& aDyingCmd) = 0;
	
	/**
	Called when the Command Queue has determined that aCommand is in a position to be sent.
	This function gives the QDP the opportunity to send, block or reject the command.
	
	If TCanSendAction::EBlock is returned the command will be delayed. MhcqdiCanSend will 
	be called again by the Command Queue after:
		- the sent queue has changed (command completed successfully or with an error)
		- an unsolicited event has been received (e.g. PIN Code Request Event)
	The QDP gets a chance to let the command be sent or to delay it further. There is no
	other way (e.g. by a call back into the Command Queue) for the QDP to change its 
	mind and decide a command can be sent.
	
	The QDP should be careful not to cause a deadlock. This could happen if the decision
	is taken not to send command A if the command P is in the sent queue. However, if the 
	completion of P depends on command A then the Command Queue would deadlock. For example,
	P = Connection Request Command and A = PIN Request Reply Command (in secmode 3).
	
	To aid in avoiding deadlocks the Command Queue:
		- will ASSERT the sent queue is not empty if CanSend returns TCanSendAction::EBlock
		- will run a watchdog timer to ensure the queue is not being starved
	
	CanSend can return a Symbian error code (not including KErrNone) to tell the Command Queue 
	not to send a command. The obtained error code is send back to the command's originator.
	HCI error codes are represented within the Symbian error code range by subtracting the 
	Bluetooth specification defined error code from KHCIErrorBase.
	
	Returning KErrNone will result in the command being sent.
	
	Note that the Command Queue will panic if the value returned is not a Symbian system-wide
	error code, a Symbian HCI error code (excluding EOK), or TCanSendAction::EBlock.
		
	@panic ECanSendDeadlock if queue starvation watchdog timer fires as a result of a CanSend block
							In release urel builds a controller reset will be started
	@panic EResendDeadlock	if queue starvation watchdog timer fires as a result of a Resend block
							In release urel builds a controller reset will be started
	@panic EUnknownDeadlock if queue starvation watchdog timer fires and the cause cannot be determined
							In release urel builds a controller reset will be started
	@panic ECanSendBlockWhenEmpty if the QDP blocks with an empty sent queue
	@panic ECanSendInvalidRejectError if the value returned is not in the ranges expected.
	@param aCommand			The command to decide whether it should be delayed (or not).
	@param aSentCommands	These are the commands that have been sent to the controller but haven't
							been completed yet.
	@return KErrNone if aCommand is to be sent,
			TCanSendAction::EBlock if aCommand is to be delayed,
			A Symbian system-wide or HCI (excluding EOK) error code to reject aCommand.
	*/
	virtual TInt MhcqdiCanSend(CHCICommandQItem& aCommand,
							   const TDblQue<const CHCICommandQItem>& aSentCommands) = 0;
		
	/**
	Called immediately after a command has been sent to the controller allowing
	the QDP to keep its state up-to-date and specify a time-out for the command.
		
	@panic	ECommandTimeoutTooBig if the TUint returned is greater than 
			MHCICommandQueue::MaxHciCommandTimeout().
	@param aSentCmd The sent command.
	@return Timeout required in milliseconds (KNoTimeoutRequired if no timeout is required).
	@see KNoTimeoutRequired
	*/
	virtual TUint MhcqdiTimeoutRequired(const CHCICommandQItem& aSentCmd) = 0;
	
	/**
	Called to notify the QDP of an event that has been matched to a command.
	This function is intended only as a way of the QDP keeping its state up-to-date.
	@param aEvent The event received.
	@param aRelatedCommand The command that caused the event.
	*/
	virtual void MhcqdiMatchedEventReceived(const THCIEventBase& aEvent, 
											const CHCICommandQItem& aRelatedCommand) = 0;
											
	/**
	Called to notify the QDP of an event that has been matched to a command.
	This function is intended as a way of the QDP keeping its state up-to-date.
	@param aErrorEvent The event received which represents an error.
	@param aRelatedCommand The command that caused the event.
	@return The action to be taken by the Command Queue.
			@see TCommandErroredAction
	*/
	virtual TCommandErroredAction MhcqdiMatchedErrorEventReceived(const THCIEventBase& aErrorEvent, 
												 				  const CHCICommandQItem& aRelatedCommand) = 0;
	
	/**
	Called to notify the QDP of an event that does not have a matching command.
	This function is intended as a way of the QDP keeping its state up-to-date.
	
	Note that this function will be called for both events that could legitimately be
	unexpected (unsolicited events), and those that are due to the controller sending
	us something odd e.g. providing a command status event for a command that has 
	not been sent. 
	
	@param aEvent The event received.
	*/
	virtual void MhcqdiUnmatchedEventReceived(const THCIEventBase& aEvent) = 0;
	
	/**
	Called when the timeout for a command has expired.
	
	@param aCommand					The command that has timed out.
	@param aSentCommands			The list of commands currently on the Sent Queue.
	@param aCurrentCommandCredits	The current number of command credits available to the Command Queue.
	@param aCreditsToBeRefunded		The number of credits to be refunded to the command queue.
	@return The action that should be taken by the Command Queue.
			@see TCommandTimedOutAction
	*/
	virtual TCommandTimedOutAction MhcqdiCommandTimedOut(const CHCICommandQItem& aCommand,
														 const TDblQue<const CHCICommandQItem>& aSentCommands,
														 TUint aCurrentCommandCredits,
														 TUint& aCreditsToBeRefunded) = 0;
	
	/**
	Gives the QDP a reference to MPhysicalLinksState, which provides
	information that helps with decision making.
	@param aPhysicalLinksState Interface to query the state of the stack's physical links
	*/
	virtual void MhcqdiSetPhysicalLinksState(const MPhysicalLinksState& aPhysicalLinksState) = 0;

	/**
	Gives the QDP a reference to MHardResetInitiator, which allows it 
	to initiate a hard reset
	@param aHardResetInitiator Hard Reset interface to use
	*/
	virtual void MhcqdiSetHardResetInitiator(const MHardResetInitiator& aHardResetInitiator) = 0;

	/**
	Gives the QDP a reference to MHCICommandQueue, which allows it to 
	add and remove commands when necessary
	@param aHCICommandQueue Interface to Command Queue
	*/
	virtual void MhcqdiSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue) = 0;

	/**
	The QDP should make use of these values when informing the
	command queue of command completion timeouts.  The QDP should
	be aware when choosing command completion timeouts, that
	choosing a value approaching the starvation timeout may result
	in panicing the queue.  By default KMaxHciCommandTimeout is
	less than KQueueStarvationTimeout although this is configurable
	via the CmdQ.ini file.
	@param aQueueStarvationTimeout The time (in milliseconds) in which the Command Queues
								 starvation timer will fire if the pending queue head has
								 remained unchanged.
	@param aMaxHciCommandTimeout The maximum timeout value the qdp may provide for command
		   						 completion.  See MhcqdiTimeoutRequired().
	@see MhcqdiTimeoutRequired
	*/
	virtual void MhcqdiSetTimeouts(TUint aQueueStarvationTimeout,
								   TUint aMaxHciCommandTimeout) = 0;
	/**
	Notifies the QDP of the Command Queue being reset.

	This returns the initial number of command credits for the queue, which is typically one.
	However, some controllers send an event after reset to set credits so it could be zero.
	@return Initial number of command credits for Command Queue
	*/
	virtual TUint MhcqdiReset() = 0;
	};

/**
The UID of the QDP Event modifier interface.

*/
const TInt KHCICmdQueueDecisionEventModifierInterfaceUid = 0x102866F8;

/**
Mixin for extension QDP interface.

This interface is to be used via the Command Queue Decision plugin
@see CHCICmdQueueDecisionPlugin::Interface(TUid)

This interface allows events to be modified before going into the Bluetooth stack. If this interface is implemented,
the functions in this interface will be called instead of MHCICmdQueueDecisionInterface::MhcqdiMatchedEventReceived
and MHCICmdQueueDecisionInterface::MhcqdiUnmatchedEventReceived
*/

class MHCICmdQueueEventModifierInterface
	{
public:
	/**	
	Called to notify the QDP of an event that has been matched to a command, and allows the QDP to change the event data
	@param aEvent The event received.
	@param aRelatedCommand The command that caused the event.
	*/
	virtual void MhcqemiMatchedEventReceived(THCIEventBase& aEvent, 
											  const CHCICommandQItem& aRelatedCommand) = 0;
	
	/**
	Called to notify the QDP of an event that does not have a matching command, and allows the QDP to change the event data
	
	Note that this function will be called for both events that could legitimately be
	unexpected (unsolicited events), and those that are due to the controller sending
	us something odd e.g. providing a command status event for a command that has 
	not been sent. 
	
	@param aEvent The event received.
	*/
	virtual void MhcqemiUnmatchedEventReceived(THCIEventBase& aEvent) = 0;

	};

/**
Utilities class for the QDP, implemented by the command queue controller.

This class is provided for the QDP to make unsolicited calls on the stack.
In order for the QDP to be provided with this interface, it must implement
MHCICmdQueueUtilities

Functions in this class are implemented as exports forwarded to virtuals to
allow for future functions to be added without breaking BC.
*/

class MHCICmdQueueUtilities
	{
public:
	/**
	Injects an event into the command queue controller
	@param aEvent Event data to be sent to the command queue controller
	*/
	IMPORT_C void InjectEvent(const THCIEventBase& aEvent);
	/**
	Finds an outstanding command in the "sent queue".
	@param aOpcode Opcode of the command to be found
	@return returns NULL if not found, or a pointer to the found item
	*/
	IMPORT_C CHCICommandQItem* FindOutstandingCommand(THCIOpcode aOpcode);
private:
	virtual void MhcquiInjectEvent(const THCIEventBase& aEvent) = 0;
	virtual CHCICommandQItem* MhcquiFindOutstandingCommand(THCIOpcode aOpcode) = 0;
	};

/**
The UID of the QDP utility user interface.
*/
const TInt KHCICmdQueueUtilityUserUid = 0x102866F9;

/**
Mixin for Command Queue Utility User.

This interface is to be used via the Command Queue Decision plugin
@see CHCICmdQueueDecisionPlugin::Interface(TUid)

This interface allows for a MHCICmdQueueUtilities to be provided to the QDP
*/

class MHCICmdQueueUtilityUser
	{
public:
	/**
	Passes an MHCICmdQueueUtilities (implemented by the command queue controller) to the QDP
	@param aProvider Reference to the provider
	*/
	virtual void MhcquuSetUtilitiesProvider(MHCICmdQueueUtilities& aProvider) = 0;
	};

#endif //HCICMDQUEUEDECISIONINTERFACE_H
