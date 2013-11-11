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
// on Thu, 29 May 2008 15:17:53 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/inquirycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/inquiryresultevent.h>
#include <bluetooth/hci/inquiryresultwithrssievent.h>
#include <bluetooth/hci/extendedinquiryresultevent.h>
#include <bluetooth/hci/inquirycompleteevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CInquiryCommand* CInquiryCommand::NewL(TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses)
	{
	CInquiryCommand* self = new (ELeave) CInquiryCommand(aLAP, aInquiryLength, aNumResponses);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CInquiryCommand* CInquiryCommand::NewL()
	{
	CInquiryCommand* self = new (ELeave) CInquiryCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CInquiryCommand::CInquiryCommand(TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses)
	: CHCICommandBase(KInquiryOpcode)
	, iLAP(aLAP)
	, iInquiryLength(aInquiryLength)
	, iNumResponses(aNumResponses)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CInquiryCommand::CInquiryCommand()
	: CHCICommandBase(KInquiryOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CInquiryCommand::~CInquiryCommand()
	{
	
	}	

/*virtual*/ void CInquiryCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == EInquiryResultEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = EFalse;
		aContinueMatching = EFalse;
		}
	else if (aEvent.EventCode() == EInquiryResultwithRSSIEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = EFalse;
		aContinueMatching = EFalse;
		}
	else if (aEvent.EventCode() == EExtendedInquiryResultEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = EFalse;
		aContinueMatching = EFalse;
		}
	else if (aEvent.EventCode() == EInquiryCompleteEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = ETrue;
		aContinueMatching = EFalse;
		}
	else if (aEvent.EventCode() == ECommandCompleteEvent)
		{
		THCICommandCompleteEvent& event = THCICommandCompleteEvent::Cast(aEvent);
		if (event.CommandOpcode() == KInquiryCancelOpcode)
			{
			aMatchesCmd = ETrue;
			aConcludesCmd = ETrue;
			aContinueMatching = ETrue;
			}
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
void CInquiryCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutBytes32(iLAP, 3);
	aCommandFrame.PutByte(iInquiryLength);
	aCommandFrame.PutByte(iNumResponses);
	}

// Assign new values to the parameters of this command
EXPORT_C void CInquiryCommand::Reset(TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses)
	{
	iLAP = aLAP;
	iInquiryLength = aInquiryLength;
	iNumResponses = aNumResponses;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint32 CInquiryCommand::LAP() const
	{
	return iLAP;
	}

EXPORT_C TUint8 CInquiryCommand::InquiryLength() const
	{
	return iInquiryLength;
	}

EXPORT_C TUint8 CInquiryCommand::NumResponses() const
	{
	return iNumResponses;
	}



// Extension function.  Use this to retrieve any extension interfaces from CInquiryCommand
// or any class from which it derives.
/*virtual*/ TInt CInquiryCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

