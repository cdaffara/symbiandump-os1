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
// on Thu, 29 May 2008 15:17:47 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/createaclconnectioncommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/connectioncompleteevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CCreateACLConnectionCommand* CCreateACLConnectionCommand::NewL(const TBTDevAddr& aBDADDR, TUint16 aPacketType, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset, TUint8 aAllowRoleSwitch)
	{
	CCreateACLConnectionCommand* self = new (ELeave) CCreateACLConnectionCommand(aBDADDR, aPacketType, aPageScanRepetitionMode, aReserved, aClockOffset, aAllowRoleSwitch);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCreateACLConnectionCommand* CCreateACLConnectionCommand::NewL()
	{
	CCreateACLConnectionCommand* self = new (ELeave) CCreateACLConnectionCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CCreateACLConnectionCommand::CCreateACLConnectionCommand(const TBTDevAddr& aBDADDR, TUint16 aPacketType, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset, TUint8 aAllowRoleSwitch)
	: CHCICommandBase(KCreateACLConnectionOpcode)
	, iBDADDR(aBDADDR)
	, iPacketType(aPacketType)
	, iPageScanRepetitionMode(aPageScanRepetitionMode)
	, iReserved(aReserved)
	, iClockOffset(aClockOffset)
	, iAllowRoleSwitch(aAllowRoleSwitch)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CCreateACLConnectionCommand::CCreateACLConnectionCommand()
	: CHCICommandBase(KCreateACLConnectionOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CCreateACLConnectionCommand::~CCreateACLConnectionCommand()
	{
	
	}	

/*virtual*/ void CCreateACLConnectionCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == EConnectionCompleteEvent)
		{
		TConnectionCompleteEvent& event = TConnectionCompleteEvent::Cast(aEvent);
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
void CCreateACLConnectionCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutDevAddr(iBDADDR);
	aCommandFrame.PutBytes16(iPacketType);
	aCommandFrame.PutByte(iPageScanRepetitionMode);
	aCommandFrame.PutByte(iReserved);
	aCommandFrame.PutBytes16(iClockOffset);
	aCommandFrame.PutByte(iAllowRoleSwitch);
	}

// Assign new values to the parameters of this command
EXPORT_C void CCreateACLConnectionCommand::Reset(const TBTDevAddr& aBDADDR, TUint16 aPacketType, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset, TUint8 aAllowRoleSwitch)
	{
	iBDADDR = aBDADDR;
	iPacketType = aPacketType;
	iPageScanRepetitionMode = aPageScanRepetitionMode;
	iReserved = aReserved;
	iClockOffset = aClockOffset;
	iAllowRoleSwitch = aAllowRoleSwitch;
	}

// Accessor methods for the parameters of the command

EXPORT_C TBTDevAddr CCreateACLConnectionCommand::BDADDR() const
	{
	return iBDADDR;
	}

EXPORT_C TUint16 CCreateACLConnectionCommand::PacketType() const
	{
	return iPacketType;
	}

EXPORT_C THCIPageScanRepetitionMode CCreateACLConnectionCommand::PageScanRepetitionMode() const
	{
	return iPageScanRepetitionMode;
	}

EXPORT_C THCIReservedOctet CCreateACLConnectionCommand::Reserved() const
	{
	return iReserved;
	}

EXPORT_C THCIClockOffset CCreateACLConnectionCommand::ClockOffset() const
	{
	return iClockOffset;
	}

EXPORT_C TUint8 CCreateACLConnectionCommand::AllowRoleSwitch() const
	{
	return iAllowRoleSwitch;
	}



// Extension function.  Use this to retrieve any extension interfaces from CCreateACLConnectionCommand
// or any class from which it derives.
/*virtual*/ TInt CCreateACLConnectionCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

