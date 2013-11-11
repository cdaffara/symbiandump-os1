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
 @internalComponent
*/

#include <bluetooth/hcicommandqitem.h>
#include "HciCmdQTimer.h"
#include "HciCmdQUtil.h"

#include <bluetooth/hci/hcicommandallocator.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hcicmdqcontroller.h>
#include <bluetooth/hci/command.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCICMDQ);
#endif

/**
Static factory method for HCI Command Queue items.

@param aCommandAllocator the HCI commmand frame allocator.
@param aCmdProgressRecipient the HCI event notification client.
@param aCommand the HCI command to be processed. Ownership of the command is transferred to
	the created HCI Command Queue item object.
@return ownership of a new HCI Command Queue item object
*/
EXPORT_C /*static*/ CHCICommandQItem* CHCICommandQItem::NewL(MHCICommandAllocator& aCommandAllocator,
															 MHCICommandQueueClient& aCmdProgressRecipient,
															 CHCICommandBase* aCommand)
	{
	LOG_STATIC_FUNC
	
	CleanupStack::PushL(aCommand); // Ensure we don't leak the command until ownership is taken.
	CHCICommandQItem* self = new (ELeave) CHCICommandQItem(aCmdProgressRecipient, aCommand);
	CleanupStack::Pop(aCommand);
	CleanupStack::PushL(self);
	self->ConstructL(aCommandAllocator);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHCICommandQItem::~CHCICommandQItem()
	{
	LOG_FUNC

	iLink.Deque();
	delete iCompletionTimer;
	delete iCommand;
	delete iCmdFrame;
	delete iQdpData;
	}

/**
Const getter for the Command encapsulated in the object.

@return Reference to the CHCICommandBase instance holding the command parameters.
*/
EXPORT_C CHCICommandBase& CHCICommandQItem::Command() const
	{
	LOG_FUNC

	return *iCommand;
	}

/**
Getter for the client to be notified of the progress of the command

@return Reference to the MHCICommandQueueClient instance to be notified 
	of the progress of the command
*/
EXPORT_C MHCICommandQueueClient* CHCICommandQItem::Client() const
	{
	LOG_FUNC
	
	return iCmdProgressRecipient;
	}

/**
Getter for the pointer to the QDP specific data associated with the queue item.

@return A pointer to an arbitrary data structure used by the QDP.
*/
EXPORT_C TAny* CHCICommandQItem::QdpData() const
	{
	LOG_FUNC
	
	return iQdpData;
	}

/**
Setter for the pointer to the QDP specific data associated with the queue item.

@param aQdpData A pointer to some data used by the QDP to be associated with this queue item.
*/
EXPORT_C void CHCICommandQItem::SetQdpData(TAny* aQdpData)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iQdpData, PANIC(KHCICmdQPanic, EPossibleQdpDataLeak));
	iQdpData = aQdpData;	
	}

/**
Return if a Command Status event has been received that matches the command associated
with this command queue item.

@return whether this command has received a command status event.
*/
EXPORT_C TBool CHCICommandQItem::ReceivedCmdStatusEvent() const
	{
	LOG_FUNC
	return ((iCommandState & EReceivedCmdStatusEvent));
	}

/**
Return if the command associated with this queue item was added to the command
queue as an initialisation command.

@return whether this command is an initialisation command.
*/
EXPORT_C TBool CHCICommandQItem::IsInitialisation() const
	{
	LOG_FUNC
	return ((iCommandState & EInitialisationCmd));
	}

/**
Return if the command associated with this queue item is a Parent command, i.e.
one that has pre and/or post child commands.

@return whether this command is a Parent command.
*/
EXPORT_C TBool CHCICommandQItem::IsParent() const
	{
	LOG_FUNC
	return (iType == EParent);
	}

/**
Return if the command associated with this queue item is a Pre-Child command, i.e.
one that has Parent command.

@return whether this command is a Pre-Child command.
*/
EXPORT_C TBool CHCICommandQItem::IsPreChild() const
	{
	LOG_FUNC
	return (iType == EPreChild);
	}

/**
Return if the command associated with this queue item is a Post-Child command, i.e.
one that has Parent command.

@return whether this command is a Post-Child command.
*/
EXPORT_C TBool CHCICommandQItem::IsPostChild() const
	{
	LOG_FUNC
	return (iType == EPostChild);
	}

/**
Return if the command associated with this queue item is a Normal command, i.e.
one that doesn't require a workaround and isn't itself a Child command.

@return whether this command is a Normal command.
*/
EXPORT_C TBool CHCICommandQItem::IsNormal() const
	{
	LOG_FUNC
	return (iType == ENormal);
	}

/**
Return the total number of times the command associated with this queue item has
been sent.

@return Number of times this command has been sent.
*/
EXPORT_C TUint CHCICommandQItem::SentCount() const
	{
	LOG_FUNC
	return iSentCounter;
	}

/**
This isn't exported since only the CHCICmdQController will use the Frame.
Getter for the command frame object that is associated with this queue item.
@return A reference to the command frame that this queue item "has".
*/
CHctlCommandFrame& CHCICommandQItem::Frame() const
	{
	LOG_FUNC
	return *iCmdFrame;
	}

/**
This isn't exported since only the CHCICmdQController will use the ID.
@return the command queue ID for this object
*/
TUint CHCICommandQItem::CommandQId() const
	{
	LOG_FUNC
	return iCommandQId;
	}

/**
This isn't exported since only the CHCICmdQController will use the ID.
@param aId the command queue ID for this object
*/
void CHCICommandQItem::SetCommandQId(TUint aId)
	{
	LOG_FUNC
	iCommandQId = aId;
	}

/**
This isn't exported since only the CHCICmdQController will use the 
EReceivedCmdStatusEvent flag.
*/
void CHCICommandQItem::SetReceivedCmdStatusEvent(TBool aReceivedCmdStatusEvent)
	{
	LOG_FUNC
	
	if (aReceivedCmdStatusEvent)
		{
		iCommandState |= EReceivedCmdStatusEvent;
		}
	else
		{
		iCommandState &= ~EReceivedCmdStatusEvent;
		}
	}

/**
Marks the command item as being an initialisation command.
*/ 
void CHCICommandQItem::SetInitialisationCmd()
	{
	LOG_FUNC
	
	iCommandState |= EInitialisationCmd;
	}

/**
This isn't exported since only the CHCICmdQController will use the 
EDuplicatedOpcode flag.
@return whether this object duplicates an item on the command queue.
*/	
TBool CHCICommandQItem::DuplicatedOpcode() const
	{
	LOG_FUNC
	return ((iCommandState & EDuplicatedOpcode));
	}
	

/**
This isn't exported since only the CHCICmdQController will use the 
EDuplicatedOpcode flag.
*/
void CHCICommandQItem::SetDuplicatedOpcode(TBool aDuplicatedOpcode)
	{
	LOG_FUNC
	
	if (aDuplicatedOpcode)
		{
		iCommandState |= EDuplicatedOpcode;
		}
	else
		{
		iCommandState &= ~EDuplicatedOpcode;
		}
	}

/**
Starts iCompletionTimer going.
@param aMilliseconds the amount of time in milliseconds to run the timer for.
@param aController the object to inform if the timer expires.
*/
void CHCICommandQItem::StartCompletionTimer(TUint aMilliseconds, CHCICmdQController& aController)
	{
	LOG_FUNC
	
	iCompletionTimer->Restart(aMilliseconds, CommandQId(), aController);
	}

/**
Cancels iCompletionTimer.
*/
void CHCICommandQItem::CancelCompletionTimer()
	{
	LOG_FUNC
	
	iCompletionTimer->Cancel();
	}

/**
Instructs this object to format iCmdFrame with the parameters in iCommand.
*/
void CHCICommandQItem::FormatFrame()
	{
	LOG_FUNC
	
	iCommand->FormatCommand(*iCmdFrame);
	}

/**
Detaches the client callback interface from the command on the queue.
*/
void CHCICommandQItem::DetachClient()
	{
	LOG_FUNC
	
	iCmdProgressRecipient = NULL;
	}

/**
@return Type of the command item (child, parent, normal, etc)
*/
CHCICommandQItem::TType CHCICommandQItem::Type() const
	{
	LOG_FUNC
	
	return iType;
	}

/**
@param Type of the command item (child, parent, normal, etc)
*/
void CHCICommandQItem::SetType(CHCICommandQItem::TType aType)
	{
	LOG_FUNC
	
	iType = aType;
	}
	
/**
This isn't exported since only the CHCICmdQController will set the 
send status flags.
*/
void CHCICommandQItem::CommandSent()
	{
	LOG_FUNC
	iSentCounter++;
	}

/**
Constructor
*/
CHCICommandQItem::CHCICommandQItem(MHCICommandQueueClient& aCmdProgressRecipient, 
								   CHCICommandBase* aCommand)
  : iCommand(aCommand),
	iCmdProgressRecipient(&aCmdProgressRecipient)
	{
	LOG_FUNC
	}

/**
Constructor
*/
void CHCICommandQItem::ConstructL(MHCICommandAllocator& aCommandAllocator)
	{
	LOG_FUNC
	
	iCmdFrame = aCommandAllocator.MhcaNewFrameL();
	iCompletionTimer = CHCICmdQCompletionTimer::NewL();
	}
