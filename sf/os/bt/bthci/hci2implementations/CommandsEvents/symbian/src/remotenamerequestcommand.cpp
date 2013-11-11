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
// on Tue, 26 Aug 2008 13:38:02 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/remotenamerequestcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/remotenamereqcompleteevent.h>
#include <bluetooth/hci/remotehostsupportedfeaturesnotificationevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CRemoteNameRequestCommand* CRemoteNameRequestCommand::NewL(const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset)
	{
	CRemoteNameRequestCommand* self = new (ELeave) CRemoteNameRequestCommand(aBDADDR, aPageScanRepetitionMode, aReserved, aClockOffset);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRemoteNameRequestCommand* CRemoteNameRequestCommand::NewL()
	{
	CRemoteNameRequestCommand* self = new (ELeave) CRemoteNameRequestCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CRemoteNameRequestCommand::CRemoteNameRequestCommand(const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset)
	: CHCICommandBase(KRemoteNameRequestOpcode)
	, iBDADDR(aBDADDR)
	, iPageScanRepetitionMode(aPageScanRepetitionMode)
	, iReserved(aReserved)
	, iClockOffset(aClockOffset)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CRemoteNameRequestCommand::CRemoteNameRequestCommand()
	: CHCICommandBase(KRemoteNameRequestOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CRemoteNameRequestCommand::~CRemoteNameRequestCommand()
	{
	
	}	

/*virtual*/ void CRemoteNameRequestCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == ERemoteNameReqCompleteEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = ETrue;
		aContinueMatching = EFalse;
		}
	else if (aEvent.EventCode() == ERemoteHostSupportedFeaturesNotificationEvent)
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
void CRemoteNameRequestCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutDevAddr(iBDADDR);
	aCommandFrame.PutByte(iPageScanRepetitionMode);
	aCommandFrame.PutByte(iReserved);
	aCommandFrame.PutBytes16(iClockOffset);
	}

// Assign new values to the parameters of this command
EXPORT_C void CRemoteNameRequestCommand::Reset(const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset)
	{
	iBDADDR = aBDADDR;
	iPageScanRepetitionMode = aPageScanRepetitionMode;
	iReserved = aReserved;
	iClockOffset = aClockOffset;
	}

// Accessor methods for the parameters of the command

EXPORT_C TBTDevAddr CRemoteNameRequestCommand::BDADDR() const
	{
	return iBDADDR;
	}

EXPORT_C THCIPageScanRepetitionMode CRemoteNameRequestCommand::PageScanRepetitionMode() const
	{
	return iPageScanRepetitionMode;
	}

EXPORT_C THCIReservedOctet CRemoteNameRequestCommand::Reserved() const
	{
	return iReserved;
	}

EXPORT_C THCIClockOffset CRemoteNameRequestCommand::ClockOffset() const
	{
	return iClockOffset;
	}



// Extension function.  Use this to retrieve any extension interfaces from CRemoteNameRequestCommand
// or any class from which it derives.
/*virtual*/ TInt CRemoteNameRequestCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

