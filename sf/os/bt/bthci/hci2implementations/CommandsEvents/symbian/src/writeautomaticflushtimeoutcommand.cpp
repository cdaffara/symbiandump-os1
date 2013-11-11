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

#include <bluetooth/hci/writeautomaticflushtimeoutcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CWriteAutomaticFlushTimeoutCommand* CWriteAutomaticFlushTimeoutCommand::NewL(THCIConnectionHandle aConnectionHandle, TUint16 aFlushTimeout)
	{
	CWriteAutomaticFlushTimeoutCommand* self = new (ELeave) CWriteAutomaticFlushTimeoutCommand(aConnectionHandle, aFlushTimeout);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteAutomaticFlushTimeoutCommand* CWriteAutomaticFlushTimeoutCommand::NewL()
	{
	CWriteAutomaticFlushTimeoutCommand* self = new (ELeave) CWriteAutomaticFlushTimeoutCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteAutomaticFlushTimeoutCommand::CWriteAutomaticFlushTimeoutCommand(THCIConnectionHandle aConnectionHandle, TUint16 aFlushTimeout)
	: CHCICommandBase(KWriteAutomaticFlushTimeoutOpcode)
	, iConnectionHandle(aConnectionHandle)
	, iFlushTimeout(aFlushTimeout)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteAutomaticFlushTimeoutCommand::CWriteAutomaticFlushTimeoutCommand()
	: CHCICommandBase(KWriteAutomaticFlushTimeoutOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CWriteAutomaticFlushTimeoutCommand::~CWriteAutomaticFlushTimeoutCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteAutomaticFlushTimeoutCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	aCommandFrame.PutBytes16(iFlushTimeout);
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteAutomaticFlushTimeoutCommand::Reset(THCIConnectionHandle aConnectionHandle, TUint16 aFlushTimeout)
	{
	iConnectionHandle = aConnectionHandle;
	iFlushTimeout = aFlushTimeout;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CWriteAutomaticFlushTimeoutCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}

EXPORT_C TUint16 CWriteAutomaticFlushTimeoutCommand::FlushTimeout() const
	{
	return iFlushTimeout;
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteAutomaticFlushTimeoutCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteAutomaticFlushTimeoutCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

