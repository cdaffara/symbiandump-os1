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

#include <bluetooth/hci/writenumbroadcastretransmissionscommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CWriteNumBroadcastRetransmissionsCommand* CWriteNumBroadcastRetransmissionsCommand::NewL(TUint8 aNumBroadcastRetransmissions)
	{
	CWriteNumBroadcastRetransmissionsCommand* self = new (ELeave) CWriteNumBroadcastRetransmissionsCommand(aNumBroadcastRetransmissions);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteNumBroadcastRetransmissionsCommand* CWriteNumBroadcastRetransmissionsCommand::NewL()
	{
	CWriteNumBroadcastRetransmissionsCommand* self = new (ELeave) CWriteNumBroadcastRetransmissionsCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteNumBroadcastRetransmissionsCommand::CWriteNumBroadcastRetransmissionsCommand(TUint8 aNumBroadcastRetransmissions)
	: CHCICommandBase(KWriteNumBroadcastRetransmissionsOpcode)
	, iNumBroadcastRetransmissions(aNumBroadcastRetransmissions)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteNumBroadcastRetransmissionsCommand::CWriteNumBroadcastRetransmissionsCommand()
	: CHCICommandBase(KWriteNumBroadcastRetransmissionsOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CWriteNumBroadcastRetransmissionsCommand::~CWriteNumBroadcastRetransmissionsCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteNumBroadcastRetransmissionsCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutByte(iNumBroadcastRetransmissions);
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteNumBroadcastRetransmissionsCommand::Reset(TUint8 aNumBroadcastRetransmissions)
	{
	iNumBroadcastRetransmissions = aNumBroadcastRetransmissions;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint8 CWriteNumBroadcastRetransmissionsCommand::NumBroadcastRetransmissions() const
	{
	return iNumBroadcastRetransmissions;
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteNumBroadcastRetransmissionsCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteNumBroadcastRetransmissionsCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

