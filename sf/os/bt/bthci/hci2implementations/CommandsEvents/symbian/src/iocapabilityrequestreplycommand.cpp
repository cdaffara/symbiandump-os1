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
// This file was generated automatically from the template commandsource.tmpl
// on Mon, 02 Jun 2008 11:36:14 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/iocapabilityrequestreplycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/iocapabilityresponseevent.h>
#include <bluetooth/hci/remoteoobdatarequestevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CIOCapabilityRequestReplyCommand* CIOCapabilityRequestReplyCommand::NewL(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements)
	{
	CIOCapabilityRequestReplyCommand* self = new (ELeave) CIOCapabilityRequestReplyCommand(aBDADDR, aIOCapability, aOOBDataPresent, aAuthenticationRequirements);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CIOCapabilityRequestReplyCommand* CIOCapabilityRequestReplyCommand::NewL()
	{
	CIOCapabilityRequestReplyCommand* self = new (ELeave) CIOCapabilityRequestReplyCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CIOCapabilityRequestReplyCommand::CIOCapabilityRequestReplyCommand(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements)
	: CHCICommandBase(KIOCapabilityRequestReplyOpcode)
	, iBDADDR(aBDADDR)
	, iIOCapability(aIOCapability)
	, iOOBDataPresent(aOOBDataPresent)
	, iAuthenticationRequirements(aAuthenticationRequirements)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CIOCapabilityRequestReplyCommand::CIOCapabilityRequestReplyCommand()
	: CHCICommandBase(KIOCapabilityRequestReplyOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CIOCapabilityRequestReplyCommand::~CIOCapabilityRequestReplyCommand()
	{
	
	}	

/*virtual*/ void CIOCapabilityRequestReplyCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == EIOCapabilityResponseEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = EFalse;
		aContinueMatching = EFalse;
		}
	else if (aEvent.EventCode() == ERemoteOOBDataRequestEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = EFalse;
		aContinueMatching = EFalse;
		}
	// Command Status Event and default Command Complete Event matching
	// is implemented in the base class.  If we haven't matched already
	// then we should try the default matching.
	if (!aMatchesCmd)
		{
		CHCICommandBase::Match(aEvent, aMatchesCmd, aConcludesCmd, aContinueMatching);
		}
	}
	

// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CIOCapabilityRequestReplyCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutDevAddr(iBDADDR);
	aCommandFrame.PutByte(iIOCapability);
	aCommandFrame.PutByte(iOOBDataPresent);
	aCommandFrame.PutByte(iAuthenticationRequirements);
	}

// Assign new values to the parameters of this command
EXPORT_C void CIOCapabilityRequestReplyCommand::Reset(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements)
	{
	iBDADDR = aBDADDR;
	iIOCapability = aIOCapability;
	iOOBDataPresent = aOOBDataPresent;
	iAuthenticationRequirements = aAuthenticationRequirements;
	}

// Accessor methods for the parameters of the command

EXPORT_C TBTDevAddr CIOCapabilityRequestReplyCommand::BDADDR() const
	{
	return iBDADDR;
	}

EXPORT_C TUint8 CIOCapabilityRequestReplyCommand::IOCapability() const
	{
	return iIOCapability;
	}

EXPORT_C TUint8 CIOCapabilityRequestReplyCommand::OOBDataPresent() const
	{
	return iOOBDataPresent;
	}

EXPORT_C TUint8 CIOCapabilityRequestReplyCommand::AuthenticationRequirements() const
	{
	return iAuthenticationRequirements;
	}



// Extension function.  Use this to retrieve any extension interfaces from CIOCapabilityRequestReplyCommand
// or any class from which it derives.
/*virtual*/ TInt CIOCapabilityRequestReplyCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

