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

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/commandstatusevent.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hcievents.h>
#include <bluetooth/hci/hciframe.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif


/***************  CHCICommandBase *******************************************/

EXPORT_C THCIOpcode CHCICommandBase::Opcode() const
	{
	return(iOpcode);	
	}

CHCICommandBase::CHCICommandBase(THCIOpcode aOpcode)
	: iOpcode(aOpcode)
	, iCreditsConsumed(1) // Default is 1 consumed credit.
	, iExpectsCommandStatusEvent(ETrue) // Default is command status event is expected
	, iExpectsCommandCompleteEvent(ETrue) //  Default is command complete event is expected
	{
	}

/**
Method to get the number of credits consumed by command.
@return The number of credits consumed by issuing this command.
*/
	
EXPORT_C TUint CHCICommandBase::CreditsConsumed() const
	{
	return iCreditsConsumed;
	}

/**
Method to say whether the command will normally result in a corresponding
Command Status event in response.
@return ETrue if a command status event is expected, otherwise EFalse.
*/
	
EXPORT_C TBool CHCICommandBase::ExpectsCommandStatusEvent() const
	{
	return iExpectsCommandStatusEvent;
	}
	
/**
Method to say whether the command will normally result in a corresponding
Command Complete event in response.
@return ETrue if a command complete event is expected, otherwise EFalse.
*/
	
EXPORT_C TBool CHCICommandBase::ExpectsCommandCompleteEvent() const
	{
	return iExpectsCommandCompleteEvent;
	}

/**
Method to change the number of command credits consumed by the command.
Note: This will typically only be called in a constructor as it is a static
property of a command.
@param aConsumedCredits The number of credits the command consumes.
*/
void CHCICommandBase::SetCreditsConsumed(TUint aConsumedCredits)
	{
	iCreditsConsumed = aConsumedCredits;
	}

/**
Method to change whether a command status event is expected by the command.
Note: This will typically only be called in a constructor as it is a static
property of a command.
@param aExpectsCmdStatus ETrue if a command status event is expected, otherwise EFalse
*/
void CHCICommandBase::SetExpectsCommandStatusEvent(TBool aExpectsCmdStatus)
	{
    iExpectsCommandStatusEvent = aExpectsCmdStatus;
	}

/**
Method to change whether a command complete event is expected by the command.
Note: This will typically only be called in a constructor as it is a static
property of a command.
@param aExpectsCmdComplete ETrue if a command complete event is expected, otherwise EFalse
*/
void CHCICommandBase::SetExpectsCommandCompleteEvent(TBool aExpectsCmdComplete)
	{
	iExpectsCommandCompleteEvent = aExpectsCmdComplete;
	}

/**
Baseclass method to say whether the command is matched by the specified event. Can be called
from the derived class method as a first attempt to establish a match before using more
command-specific logic in the derived class.
*/

/* virtual */ void CHCICommandBase::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	THCIEventCode eventCode(aEvent.EventCode());

	if (eventCode == ECommandCompleteEvent)
		{
		THCICommandCompleteEvent& event = THCICommandCompleteEvent::Cast(aEvent);
		aMatchesCmd			= (event.CommandOpcode() == Opcode());
		aConcludesCmd		= aMatchesCmd;
		aContinueMatching	= !aMatchesCmd;
		}
	else if (eventCode == ECommandStatusEvent)
		{
		TCommandStatusEvent& event = TCommandStatusEvent::Cast(aEvent);
		aMatchesCmd			= (event.CommandOpcode() == Opcode());
		// If the status is an error then this event concludes the command
		aConcludesCmd		= (aMatchesCmd && (event.ErrorCode() != EOK));
		aContinueMatching	= !aMatchesCmd;
		}
	else
		{
		aMatchesCmd			= EFalse;
		aConcludesCmd		= EFalse;
		aContinueMatching	= EFalse;
		}
	}

CHCICommandBase::~CHCICommandBase()
	{
	delete iCommandBaseExtension;
	}

/**
Method to format the command into the format required by the HCTL. Delegates
most of the work to the pure virtual Format implementation in the derived class
*/
EXPORT_C void CHCICommandBase::FormatCommand(CHctlCommandFrame& aCommandFrame)
	{
	// Write the Command Op code into the string.
	aCommandFrame.SetOpcode(iOpcode);

	// Format the rest of the command.
	Format(aCommandFrame);
	aCommandFrame.FinaliseCommand();
	}

/**
This function should be called to initialise the CHCICommandBase completely.
As such it should always be called by the factory function (NewL) of any class
that derives from CHCICommandBase.
*/
void CHCICommandBase::BaseConstructL()
	{
	// gain access to completing event methods
	iCommandBaseExtension = CHCICommandBaseExtension::NewL();
	}

/**
Extension function.  Use this to retrieve any extension interfaces to CHCICommandBase.
It is essential that any class that derives from this class overload and then call
this function.
*/
TInt CHCICommandBase::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	if (aExtensionId == KCompletingEventExpectUid && iCommandBaseExtension)
		{
		if (iCommandBaseExtension->CompletingEventQueryHelper())
			{
			aInterface = static_cast<MHCICompletingEventQuery*>(iCommandBaseExtension->CompletingEventQueryHelper());
			return KErrNone;
			}
		}
		
	return CBase::Extension_(aExtensionId, aInterface, aData);
	}

