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
// on Thu, 29 May 2008 15:17:51 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/acceptsynchronousconnectionrequestcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/synchronousconnectioncompleteevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CAcceptSynchronousConnectionRequestCommand* CAcceptSynchronousConnectionRequestCommand::NewL(const TBTDevAddr& aBDADDR, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aContentFormat, TUint8 aRetransmissionEffort, TUint16 aPacketType)
	{
	CAcceptSynchronousConnectionRequestCommand* self = new (ELeave) CAcceptSynchronousConnectionRequestCommand(aBDADDR, aTransmitBandwidth, aReceiveBandwidth, aMaxLatency, aContentFormat, aRetransmissionEffort, aPacketType);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAcceptSynchronousConnectionRequestCommand* CAcceptSynchronousConnectionRequestCommand::NewL()
	{
	CAcceptSynchronousConnectionRequestCommand* self = new (ELeave) CAcceptSynchronousConnectionRequestCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CAcceptSynchronousConnectionRequestCommand::CAcceptSynchronousConnectionRequestCommand(const TBTDevAddr& aBDADDR, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aContentFormat, TUint8 aRetransmissionEffort, TUint16 aPacketType)
	: CHCICommandBase(KAcceptSynchronousConnectionRequestOpcode)
	, iBDADDR(aBDADDR)
	, iTransmitBandwidth(aTransmitBandwidth)
	, iReceiveBandwidth(aReceiveBandwidth)
	, iMaxLatency(aMaxLatency)
	, iContentFormat(aContentFormat)
	, iRetransmissionEffort(aRetransmissionEffort)
	, iPacketType(aPacketType)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CAcceptSynchronousConnectionRequestCommand::CAcceptSynchronousConnectionRequestCommand()
	: CHCICommandBase(KAcceptSynchronousConnectionRequestOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CAcceptSynchronousConnectionRequestCommand::~CAcceptSynchronousConnectionRequestCommand()
	{
	
	}	

/*virtual*/ void CAcceptSynchronousConnectionRequestCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == ESynchronousConnectionCompleteEvent)
		{
		TSynchronousConnectionCompleteEvent& event = TSynchronousConnectionCompleteEvent::Cast(aEvent);
		if (event.BDADDR() == BDADDR())
			{
			aMatchesCmd = ETrue;
			aConcludesCmd = ETrue;
			aContinueMatching = EFalse;
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
void CAcceptSynchronousConnectionRequestCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutDevAddr(iBDADDR);
	aCommandFrame.PutBytes32(iTransmitBandwidth, 4);
	aCommandFrame.PutBytes32(iReceiveBandwidth, 4);
	aCommandFrame.PutBytes16(iMaxLatency);
	aCommandFrame.PutBytes16(iContentFormat);
	aCommandFrame.PutByte(iRetransmissionEffort);
	aCommandFrame.PutBytes16(iPacketType);
	}

// Assign new values to the parameters of this command
EXPORT_C void CAcceptSynchronousConnectionRequestCommand::Reset(const TBTDevAddr& aBDADDR, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aContentFormat, TUint8 aRetransmissionEffort, TUint16 aPacketType)
	{
	iBDADDR = aBDADDR;
	iTransmitBandwidth = aTransmitBandwidth;
	iReceiveBandwidth = aReceiveBandwidth;
	iMaxLatency = aMaxLatency;
	iContentFormat = aContentFormat;
	iRetransmissionEffort = aRetransmissionEffort;
	iPacketType = aPacketType;
	}

// Accessor methods for the parameters of the command

EXPORT_C TBTDevAddr CAcceptSynchronousConnectionRequestCommand::BDADDR() const
	{
	return iBDADDR;
	}

EXPORT_C TUint32 CAcceptSynchronousConnectionRequestCommand::TransmitBandwidth() const
	{
	return iTransmitBandwidth;
	}

EXPORT_C TUint32 CAcceptSynchronousConnectionRequestCommand::ReceiveBandwidth() const
	{
	return iReceiveBandwidth;
	}

EXPORT_C TUint16 CAcceptSynchronousConnectionRequestCommand::MaxLatency() const
	{
	return iMaxLatency;
	}

EXPORT_C TUint16 CAcceptSynchronousConnectionRequestCommand::ContentFormat() const
	{
	return iContentFormat;
	}

EXPORT_C TUint8 CAcceptSynchronousConnectionRequestCommand::RetransmissionEffort() const
	{
	return iRetransmissionEffort;
	}

EXPORT_C TUint16 CAcceptSynchronousConnectionRequestCommand::PacketType() const
	{
	return iPacketType;
	}



// Extension function.  Use this to retrieve any extension interfaces from CAcceptSynchronousConnectionRequestCommand
// or any class from which it derives.
/*virtual*/ TInt CAcceptSynchronousConnectionRequestCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

