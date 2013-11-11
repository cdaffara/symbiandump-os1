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
// on Thu, 29 May 2008 15:17:55 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/hostbuffersizecommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CHostBufferSizeCommand* CHostBufferSizeCommand::NewL(TUint16 aHostACLDataPacketLength, TUint8 aHostSynchronousDataPacketLength, TUint16 aHostTotalNumACLDataPackets, TUint16 aHostTotalNumSynchronousDataPackets)
	{
	CHostBufferSizeCommand* self = new (ELeave) CHostBufferSizeCommand(aHostACLDataPacketLength, aHostSynchronousDataPacketLength, aHostTotalNumACLDataPackets, aHostTotalNumSynchronousDataPackets);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHostBufferSizeCommand* CHostBufferSizeCommand::NewL()
	{
	CHostBufferSizeCommand* self = new (ELeave) CHostBufferSizeCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CHostBufferSizeCommand::CHostBufferSizeCommand(TUint16 aHostACLDataPacketLength, TUint8 aHostSynchronousDataPacketLength, TUint16 aHostTotalNumACLDataPackets, TUint16 aHostTotalNumSynchronousDataPackets)
	: CHCICommandBase(KHostBufferSizeOpcode)
	, iHostACLDataPacketLength(aHostACLDataPacketLength)
	, iHostSynchronousDataPacketLength(aHostSynchronousDataPacketLength)
	, iHostTotalNumACLDataPackets(aHostTotalNumACLDataPackets)
	, iHostTotalNumSynchronousDataPackets(aHostTotalNumSynchronousDataPackets)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CHostBufferSizeCommand::CHostBufferSizeCommand()
	: CHCICommandBase(KHostBufferSizeOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CHostBufferSizeCommand::~CHostBufferSizeCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CHostBufferSizeCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutBytes16(iHostACLDataPacketLength);
	aCommandFrame.PutByte(iHostSynchronousDataPacketLength);
	aCommandFrame.PutBytes16(iHostTotalNumACLDataPackets);
	aCommandFrame.PutBytes16(iHostTotalNumSynchronousDataPackets);
	}

// Assign new values to the parameters of this command
EXPORT_C void CHostBufferSizeCommand::Reset(TUint16 aHostACLDataPacketLength, TUint8 aHostSynchronousDataPacketLength, TUint16 aHostTotalNumACLDataPackets, TUint16 aHostTotalNumSynchronousDataPackets)
	{
	iHostACLDataPacketLength = aHostACLDataPacketLength;
	iHostSynchronousDataPacketLength = aHostSynchronousDataPacketLength;
	iHostTotalNumACLDataPackets = aHostTotalNumACLDataPackets;
	iHostTotalNumSynchronousDataPackets = aHostTotalNumSynchronousDataPackets;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint16 CHostBufferSizeCommand::HostACLDataPacketLength() const
	{
	return iHostACLDataPacketLength;
	}

EXPORT_C TUint8 CHostBufferSizeCommand::HostSynchronousDataPacketLength() const
	{
	return iHostSynchronousDataPacketLength;
	}

EXPORT_C TUint16 CHostBufferSizeCommand::HostTotalNumACLDataPackets() const
	{
	return iHostTotalNumACLDataPackets;
	}

EXPORT_C TUint16 CHostBufferSizeCommand::HostTotalNumSynchronousDataPackets() const
	{
	return iHostTotalNumSynchronousDataPackets;
	}



// Extension function.  Use this to retrieve any extension interfaces from CHostBufferSizeCommand
// or any class from which it derives.
/*virtual*/ TInt CHostBufferSizeCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

