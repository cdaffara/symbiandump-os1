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

#include <bluetooth/hci/periodicinquirymodecommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CPeriodicInquiryModeCommand* CPeriodicInquiryModeCommand::NewL(TUint16 aMaxPeriodLength, TUint16 aMinPeriodLength, TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses)
	{
	CPeriodicInquiryModeCommand* self = new (ELeave) CPeriodicInquiryModeCommand(aMaxPeriodLength, aMinPeriodLength, aLAP, aInquiryLength, aNumResponses);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPeriodicInquiryModeCommand* CPeriodicInquiryModeCommand::NewL()
	{
	CPeriodicInquiryModeCommand* self = new (ELeave) CPeriodicInquiryModeCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CPeriodicInquiryModeCommand::CPeriodicInquiryModeCommand(TUint16 aMaxPeriodLength, TUint16 aMinPeriodLength, TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses)
	: CHCICommandBase(KPeriodicInquiryModeOpcode)
	, iMaxPeriodLength(aMaxPeriodLength)
	, iMinPeriodLength(aMinPeriodLength)
	, iLAP(aLAP)
	, iInquiryLength(aInquiryLength)
	, iNumResponses(aNumResponses)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CPeriodicInquiryModeCommand::CPeriodicInquiryModeCommand()
	: CHCICommandBase(KPeriodicInquiryModeOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CPeriodicInquiryModeCommand::~CPeriodicInquiryModeCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CPeriodicInquiryModeCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutBytes16(iMaxPeriodLength);
	aCommandFrame.PutBytes16(iMinPeriodLength);
	aCommandFrame.PutBytes32(iLAP, 3);
	aCommandFrame.PutByte(iInquiryLength);
	aCommandFrame.PutByte(iNumResponses);
	}

// Assign new values to the parameters of this command
EXPORT_C void CPeriodicInquiryModeCommand::Reset(TUint16 aMaxPeriodLength, TUint16 aMinPeriodLength, TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses)
	{
	iMaxPeriodLength = aMaxPeriodLength;
	iMinPeriodLength = aMinPeriodLength;
	iLAP = aLAP;
	iInquiryLength = aInquiryLength;
	iNumResponses = aNumResponses;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint16 CPeriodicInquiryModeCommand::MaxPeriodLength() const
	{
	return iMaxPeriodLength;
	}

EXPORT_C TUint16 CPeriodicInquiryModeCommand::MinPeriodLength() const
	{
	return iMinPeriodLength;
	}

EXPORT_C TUint32 CPeriodicInquiryModeCommand::LAP() const
	{
	return iLAP;
	}

EXPORT_C TUint8 CPeriodicInquiryModeCommand::InquiryLength() const
	{
	return iInquiryLength;
	}

EXPORT_C TUint8 CPeriodicInquiryModeCommand::NumResponses() const
	{
	return iNumResponses;
	}



// Extension function.  Use this to retrieve any extension interfaces from CPeriodicInquiryModeCommand
// or any class from which it derives.
/*virtual*/ TInt CPeriodicInquiryModeCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

