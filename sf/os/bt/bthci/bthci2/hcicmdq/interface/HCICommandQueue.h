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

#ifndef HCICOMMANDQUEUE_H
#define HCICOMMANDQUEUE_H

#include <e32def.h>
#include <e32cmn.h>


class CHCICommandQItem;
class MHCICommandQueueClient;
class CHCICommandBase;

/**
An Invalid Command Queue Item Id.
*/
static const TUint KInvalidCommandQueueItemId = 0;

class MHCICommandQueue
	{
public:
	/**
	Adds a pre-allocated command item to the end of the command queue. Ownership of the command item
	is transferred from the caller (even in the event of an ..Add.. method leaving).

	Use this function if you pre-allocated a CHCICommandQItem. Use this for ensuring
	that a command can get accepted by the command queue in low memory conditions.

	@leave KErrHardwareNotAvailable if unable to add the command to the Command Queue due to the controller
									being in a power off state.

	@param aQueItem The command queue item to be added to the queue.
					Ownership is passed.
	@return the command queue ID given to the command which can be used to remove the command 
			if necessary in future.
	*/
	virtual TUint MhcqAddCommandL(CHCICommandQItem* aQueItem) = 0;

	/**
	Adds a command to the end of the command queue. Ownership of the command data is transfered from 
	the caller (even in the event of an ..Add.. method leaving).

	@leave KErrNoMemory This function may leave as it allocates a CHCICommandQItem to wrap around CHCICommandBase.	
	@leave KErrNotSupported if aCommandData is not recognised. E.g. if the caller of the function is 
							asking for a command from a version of the Bluetooth specification
							that the licensee HCI implementation doesn't support.
	@leave KErrHardwareNotAvailable if unable to add the command to the Command Queue due to the controller
									being in a power off state.
	
	@param aCommandData The data to be added to a command queue item which will be be placed on the queue.
						Ownership is passed.
	@param aCmdProgressRecipient The interface which will get notified about the command's progress.
	@return the command queue ID given to the command which can be used to remove the command 
			if necessary in future.
	*/
	virtual TUint MhcqAddCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient) = 0;
	
	/**
	Adds a pre-allocated priority command item to the head of the command queue. Ownership of the command item
	is transferred from the caller (even in the event of an ..Add.. method leaving).

	Use this function if you pre-allocated a CHCICommandQItem. Use this for ensuring
	that a command can get accepted by the command queue in low memory conditions.

	@leave KErrHardwareNotAvailable if unable to add the command to the Command Queue due to the controller
									being in a power off state.

	@param aQueItem The command queue item to be added to the queue.
					Ownership is passed.
	@return the command queue ID given to the command which can be used to remove the command 
			if necessary in future.
	*/	
	virtual TUint MhcqAddPriorityCommandL(CHCICommandQItem* aQueItem) = 0;

	/**
	Adds a priority command to the head of the command queue. Ownership of the command data is transfered from 
	the caller (even in the event of an ..Add.. method leaving).

	@leave KErrNoMemory This function may leave as it allocates a CHCICommandQItem to wrap around CHCICommandBase.	
	@leave KErrNotSupported if aCommandData is not recognised. E.g. if the caller of the function is 
							asking for a command from a version of the Bluetooth specification
							that the licensee HCI implementation doesn't support.
	@leave KErrHardwareNotAvailable if unable to add the command to the Command Queue due to the controller
									being in a power off state.

	@param aCommandData The data to be added to a command queue item which will be be placed on the queue.
						Ownership is passed.
	@param aCmdProgressRecipient The interface which will get notified about the command's progress.
	@return the command queue ID given to the command which can be used to remove the command 
			if necessary in future.
	*/
	virtual TUint MhcqAddPriorityCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient) = 0;

	/**
	Adds a pre-allocated initialisation command item to the end of the command queue. Ownership of the command item
	is transferred from the caller (even in the event of an ..Add.. method leaving).

	Initialisation commands are only accepted and sent after the Command Queue has
	been reset but before it has been started.

	Use this function if you pre-allocated a CHCICommandQItem. Use this for ensuring
	that a command can get accepted by the command queue in low memory conditions.

	@see CHCICmdQController::Reset
	@see CHCICmdQController::Start

	@leave KErrHardwareNotAvailable if unable to add the command to the Command Queue due to the controller
									being in a power off state.

	@panic EInitCmdAfterStart Attempting to add an initialisation command after the queue has been started.

	@param aQueItem The command queue item to be added to the queue.
					Ownership is passed.
	@return the command queue ID given to the command which can be used to remove the command 
			if necessary in future.
	*/
	virtual TUint MhcqAddInitCommandL(CHCICommandQItem* aQueItem) = 0;

	/**
	Adds an initialisation command to the end of the command queue.	Ownership of the command data is transfered from 
	the caller (even in the event of an ..Add.. method leaving).

	Initialisation commands are only accepted and sent after the Command Queue has
	been reset but before it has been started.

	@see CHCICmdQController::Reset
	@see CHCICmdQController::Start

	@leave KErrNoMemory This function may leave as it allocates a CHCICommandQItem to wrap around CHCICommandBase.	
	@leave KErrNotSupported if aCommandData is not recognised. E.g. if the caller of the function is 
							asking for a command from a version of the Bluetooth specification
							that the licensee HCI implementation doesn't support.
	@leave KErrHardwareNotAvailable if unable to add the command to the Command Queue due to the controller
									being in a power off state.

	@panic EInitCmdAfterStart Attempting to add an initialisation command after the queue has been started.

	@param aCommandData The data to be added to a command queue item which will be be placed on the queue.
						Ownership is passed.
	@param aCmdProgressRecipient The interface which will get notified about the command's progress.
	@return the command queue ID given to the command which can be used to remove the command 
			if necessary in future.
	*/
	virtual TUint MhcqAddInitCommandL(CHCICommandBase* aCommandData, MHCICommandQueueClient& aCmdProgressRecipient) = 0;

	/**
	Removes the command, which has ID aCommandId, from the queue of commands that haven't been sent to the 
	Bluetooth controller yet.
	
	@param aCommandId the ID of the command to remove.
	@param aCmdOriginator The client that added the command in the first place. This means one client can't 
			remove the commands of another client.
	@return KErrNotFound if the commmand refered to isn't on the queue of pending commands. This could be
			because the command has already been sent to the contoller or because no command with the
			specified ID was added to the command queue via on of the AddCommand methods above.
	*/
	virtual TInt MhcqRemoveCommand(TUint aCommandId, const MHCICommandQueueClient& aCmdOriginator) = 0;
	
	/**
	Removes from the Command Queue of all commands a given client has added to the queue.
	
	This should always be called by a MHCICommandQueueClient that has added commands to the Command
	Queue before it is deleted.
	
	It should be noted that once this is called:
	 - Commands that are pending and have not yet been sent may never get sent.
	 - Pointers to the CHCICommandQItem may (or may not) be valid.
	
	@param aCmdOriginator The client of commands that are to be purged from the Command Queue.
	*/
	virtual void MhcqRemoveAllCommands(const MHCICommandQueueClient& aCmdOriginator) = 0;
	
	/**
	@return The maximum timeout that can be returned by MHCICmdQueueDecisionInterface::MhcqdiTimeoutRequired
	*/
	virtual TUint MhcqMaxHciCommandTimeout() const = 0;

	/**
	This accesses CHCICmdQueueDecisionPlugin::Interface allowing clients of the
	Command Queue to request additional interfaces from the QDP

	This function will panic if a client attempts to get hold of
       	MHCICmdQueueDeicisionInterface, which is exclusive to the Command Queue.

	@see CHCICmdQueueDecisionPlugin::Interface

	@panic EIllegalRequestForQDPInterface if the client requests MHCICmdQueueDecisionInterface

	@param aUid UID of interface to request from the QDP
	@return Result from CHCICmdQueueDecisionPlugin::Interface
		Note that this can be NULL if there is no QDP in the system
	*/
	virtual TAny* MhcqQdpPluginInterface(TUid aUid) const = 0;
	};

#endif // HCICOMMANDQUEUE_H 

