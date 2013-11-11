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
// on Thu, 29 May 2008 15:17:52 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/readstoredlinkkeycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/returnlinkkeysevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CReadStoredLinkKeyCommand* CReadStoredLinkKeyCommand::NewL(const TBTDevAddr& aBDADDR, TUint8 aReadAllFlag)
	{
	CReadStoredLinkKeyCommand* self = new (ELeave) CReadStoredLinkKeyCommand(aBDADDR, aReadAllFlag);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CReadStoredLinkKeyCommand* CReadStoredLinkKeyCommand::NewL()
	{
	CReadStoredLinkKeyCommand* self = new (ELeave) CReadStoredLinkKeyCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CReadStoredLinkKeyCommand::CReadStoredLinkKeyCommand(const TBTDevAddr& aBDADDR, TUint8 aReadAllFlag)
	: CHCICommandBase(KReadStoredLinkKeyOpcode)
	, iBDADDR(aBDADDR)
	, iReadAllFlag(aReadAllFlag)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CReadStoredLinkKeyCommand::CReadStoredLinkKeyCommand()
	: CHCICommandBase(KReadStoredLinkKeyOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CReadStoredLinkKeyCommand::~CReadStoredLinkKeyCommand()
	{
	
	}	

/*virtual*/ void CReadStoredLinkKeyCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == EReturnLinkKeysEvent)
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
void CReadStoredLinkKeyCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutDevAddr(iBDADDR);
	aCommandFrame.PutByte(iReadAllFlag);
	}

// Assign new values to the parameters of this command
EXPORT_C void CReadStoredLinkKeyCommand::Reset(const TBTDevAddr& aBDADDR, TUint8 aReadAllFlag)
	{
	iBDADDR = aBDADDR;
	iReadAllFlag = aReadAllFlag;
	}

// Accessor methods for the parameters of the command

EXPORT_C TBTDevAddr CReadStoredLinkKeyCommand::BDADDR() const
	{
	return iBDADDR;
	}

EXPORT_C TUint8 CReadStoredLinkKeyCommand::ReadAllFlag() const
	{
	return iReadAllFlag;
	}



// Extension function.  Use this to retrieve any extension interfaces from CReadStoredLinkKeyCommand
// or any class from which it derives.
/*virtual*/ TInt CReadStoredLinkKeyCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

