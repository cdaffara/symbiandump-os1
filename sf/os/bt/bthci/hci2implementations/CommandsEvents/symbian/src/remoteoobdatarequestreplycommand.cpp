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
// on Sat, 31 May 2008 18:58:48 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/remoteoobdatarequestreplycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CRemoteOOBDataRequestReplyCommand* CRemoteOOBDataRequestReplyCommand::NewL(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aOOBDataC, const TBluetoothSimplePairingRandomizer& aOOBDataR)
	{
	CRemoteOOBDataRequestReplyCommand* self = new (ELeave) CRemoteOOBDataRequestReplyCommand(aBDADDR, aOOBDataC, aOOBDataR);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRemoteOOBDataRequestReplyCommand* CRemoteOOBDataRequestReplyCommand::NewL()
	{
	CRemoteOOBDataRequestReplyCommand* self = new (ELeave) CRemoteOOBDataRequestReplyCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CRemoteOOBDataRequestReplyCommand::CRemoteOOBDataRequestReplyCommand(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aOOBDataC, const TBluetoothSimplePairingRandomizer& aOOBDataR)
	: CHCICommandBase(KRemoteOOBDataRequestReplyOpcode)
	, iBDADDR(aBDADDR)
	, iOOBDataC(aOOBDataC)
	, iOOBDataR(aOOBDataR)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CRemoteOOBDataRequestReplyCommand::CRemoteOOBDataRequestReplyCommand()
	: CHCICommandBase(KRemoteOOBDataRequestReplyOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CRemoteOOBDataRequestReplyCommand::~CRemoteOOBDataRequestReplyCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CRemoteOOBDataRequestReplyCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutDevAddr(iBDADDR);
	aCommandFrame.PutSimplePairingHash(iOOBDataC);
	aCommandFrame.PutSimplePairingRandomizer(iOOBDataR);
	}

// Assign new values to the parameters of this command
EXPORT_C void CRemoteOOBDataRequestReplyCommand::Reset(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aOOBDataC, const TBluetoothSimplePairingRandomizer& aOOBDataR)
	{
	iBDADDR = aBDADDR;
	iOOBDataC = aOOBDataC;
	iOOBDataR = aOOBDataR;
	}

// Accessor methods for the parameters of the command

EXPORT_C TBTDevAddr CRemoteOOBDataRequestReplyCommand::BDADDR() const
	{
	return iBDADDR;
	}

EXPORT_C TBluetoothSimplePairingHash CRemoteOOBDataRequestReplyCommand::OOBDataC() const
	{
	return iOOBDataC;
	}

EXPORT_C TBluetoothSimplePairingRandomizer CRemoteOOBDataRequestReplyCommand::OOBDataR() const
	{
	return iOOBDataR;
	}



// Extension function.  Use this to retrieve any extension interfaces from CRemoteOOBDataRequestReplyCommand
// or any class from which it derives.
/*virtual*/ TInt CRemoteOOBDataRequestReplyCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

