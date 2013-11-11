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
// on Mon, 02 Jun 2008 11:36:17 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/sniffsubratingcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CSniffSubratingCommand* CSniffSubratingCommand::NewL(THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout)
	{
	CSniffSubratingCommand* self = new (ELeave) CSniffSubratingCommand(aConnectionHandle, aMaximumLatency, aMinimumRemoteTimeout, aMinimumLocalTimeout);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSniffSubratingCommand* CSniffSubratingCommand::NewL()
	{
	CSniffSubratingCommand* self = new (ELeave) CSniffSubratingCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CSniffSubratingCommand::CSniffSubratingCommand(THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout)
	: CHCICommandBase(KSniffSubratingOpcode)
	, iConnectionHandle(aConnectionHandle)
	, iMaximumLatency(aMaximumLatency)
	, iMinimumRemoteTimeout(aMinimumRemoteTimeout)
	, iMinimumLocalTimeout(aMinimumLocalTimeout)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CSniffSubratingCommand::CSniffSubratingCommand()
	: CHCICommandBase(KSniffSubratingOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CSniffSubratingCommand::~CSniffSubratingCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CSniffSubratingCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	aCommandFrame.PutBytes16(iMaximumLatency);
	aCommandFrame.PutBytes16(iMinimumRemoteTimeout);
	aCommandFrame.PutBytes16(iMinimumLocalTimeout);
	}

// Assign new values to the parameters of this command
EXPORT_C void CSniffSubratingCommand::Reset(THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout)
	{
	iConnectionHandle = aConnectionHandle;
	iMaximumLatency = aMaximumLatency;
	iMinimumRemoteTimeout = aMinimumRemoteTimeout;
	iMinimumLocalTimeout = aMinimumLocalTimeout;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CSniffSubratingCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}

EXPORT_C TBasebandTime CSniffSubratingCommand::MaximumLatency() const
	{
	return iMaximumLatency;
	}

EXPORT_C TBasebandTime CSniffSubratingCommand::MinimumRemoteTimeout() const
	{
	return iMinimumRemoteTimeout;
	}

EXPORT_C TBasebandTime CSniffSubratingCommand::MinimumLocalTimeout() const
	{
	return iMinimumLocalTimeout;
	}



// Extension function.  Use this to retrieve any extension interfaces from CSniffSubratingCommand
// or any class from which it derives.
/*virtual*/ TInt CSniffSubratingCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

