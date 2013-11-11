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
// on Wed, 05 Dec 2007 14:43:51 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/setupsynchronousconnectioncommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/synchronousconnectioncompleteevent.h>
#include <bluetooth/hci/synchronousconnectionchangedevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

#pragma message("Value of iVoiceSetting member is closely related to hardware used. ")
#pragma message("Bits: Input Coding, Input Data, Input Sample Size and Linear_PCM_Bit_pos, should be set...")
#pragma message("...according to hardware capabilities, and OR-ed to create appropriate iVoiceSetting value.")

// Factory methods

EXPORT_C CSetupSynchronousConnectionCommand* CSetupSynchronousConnectionCommand::NewL(THCIConnectionHandle aConnectionHandle, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aVoiceSetting, TUint8 aRetransmissionEffort, TUint16 aPacketType, const TBTDevAddr& aBDADDR)
	{
	CSetupSynchronousConnectionCommand* self = new (ELeave) CSetupSynchronousConnectionCommand(aConnectionHandle, aTransmitBandwidth, aReceiveBandwidth, aMaxLatency, aVoiceSetting, aRetransmissionEffort, aPacketType, aBDADDR);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSetupSynchronousConnectionCommand* CSetupSynchronousConnectionCommand::NewL()
	{
	CSetupSynchronousConnectionCommand* self = new (ELeave) CSetupSynchronousConnectionCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CSetupSynchronousConnectionCommand::CSetupSynchronousConnectionCommand(THCIConnectionHandle aConnectionHandle, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aVoiceSetting, TUint8 aRetransmissionEffort, TUint16 aPacketType, const TBTDevAddr& aBDADDR)
	: CHCICommandBase(KSetupSynchronousConnectionOpcode)
	, iConnectionHandle(aConnectionHandle)
	, iTransmitBandwidth(aTransmitBandwidth)
	, iReceiveBandwidth(aReceiveBandwidth)
	, iMaxLatency(aMaxLatency)
	, iVoiceSetting(aVoiceSetting)
	, iRetransmissionEffort(aRetransmissionEffort)
	, iPacketType(aPacketType)
	, iBDADDR(aBDADDR)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CSetupSynchronousConnectionCommand::CSetupSynchronousConnectionCommand()
	: CHCICommandBase(KSetupSynchronousConnectionOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CSetupSynchronousConnectionCommand::~CSetupSynchronousConnectionCommand()
	{
	
	}	

/*virtual*/ void CSetupSynchronousConnectionCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
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
	else if (aEvent.EventCode() == ESynchronousConnectionChangedEvent)
		{
		aMatchesCmd = ETrue;
		aConcludesCmd = ETrue;
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
void CSetupSynchronousConnectionCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	aCommandFrame.PutBytes32(iTransmitBandwidth, 4);
	aCommandFrame.PutBytes32(iReceiveBandwidth, 4);
	aCommandFrame.PutBytes16(iMaxLatency);
	aCommandFrame.PutBytes16(iVoiceSetting);
	aCommandFrame.PutByte(iRetransmissionEffort);
	aCommandFrame.PutBytes16(iPacketType);
	}

// Assign new values to the parameters of this command
EXPORT_C void CSetupSynchronousConnectionCommand::Reset(THCIConnectionHandle aConnectionHandle, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aVoiceSetting, TUint8 aRetransmissionEffort, TUint16 aPacketType, const TBTDevAddr& aBDADDR)
	{
	iConnectionHandle = aConnectionHandle;
	iTransmitBandwidth = aTransmitBandwidth;
	iReceiveBandwidth = aReceiveBandwidth;
	iMaxLatency = aMaxLatency;
	iVoiceSetting = aVoiceSetting;
	iRetransmissionEffort = aRetransmissionEffort;
	iPacketType = aPacketType;
	iBDADDR = aBDADDR;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CSetupSynchronousConnectionCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}

EXPORT_C TUint32 CSetupSynchronousConnectionCommand::TransmitBandwidth() const
	{
	return iTransmitBandwidth;
	}

EXPORT_C TUint32 CSetupSynchronousConnectionCommand::ReceiveBandwidth() const
	{
	return iReceiveBandwidth;
	}

EXPORT_C TUint16 CSetupSynchronousConnectionCommand::MaxLatency() const
	{
	return iMaxLatency;
	}

EXPORT_C TUint16 CSetupSynchronousConnectionCommand::VoiceSetting() const
	{
	return iVoiceSetting;
	}

EXPORT_C TUint8 CSetupSynchronousConnectionCommand::RetransmissionEffort() const
	{
	return iRetransmissionEffort;
	}

EXPORT_C TUint16 CSetupSynchronousConnectionCommand::PacketType() const
	{
	return iPacketType;
	}

EXPORT_C TBTDevAddr CSetupSynchronousConnectionCommand::BDADDR() const
	{
	return iBDADDR;
	}



// Extension function.  Use this to retrieve any extension interfaces from CSetupSynchronousConnectionCommand
// or any class from which it derives.
/*virtual*/ TInt CSetupSynchronousConnectionCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

