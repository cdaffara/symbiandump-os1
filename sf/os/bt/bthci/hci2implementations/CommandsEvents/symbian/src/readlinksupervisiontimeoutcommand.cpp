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

#include <bluetooth/hci/readlinksupervisiontimeoutcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CReadLinkSupervisionTimeoutCommand* CReadLinkSupervisionTimeoutCommand::NewL(THCIConnectionHandle aConnectionHandle)
	{
	CReadLinkSupervisionTimeoutCommand* self = new (ELeave) CReadLinkSupervisionTimeoutCommand(aConnectionHandle);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CReadLinkSupervisionTimeoutCommand* CReadLinkSupervisionTimeoutCommand::NewL()
	{
	CReadLinkSupervisionTimeoutCommand* self = new (ELeave) CReadLinkSupervisionTimeoutCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CReadLinkSupervisionTimeoutCommand::CReadLinkSupervisionTimeoutCommand(THCIConnectionHandle aConnectionHandle)
	: CHCICommandBase(KReadLinkSupervisionTimeoutOpcode)
	, iConnectionHandle(aConnectionHandle)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CReadLinkSupervisionTimeoutCommand::CReadLinkSupervisionTimeoutCommand()
	: CHCICommandBase(KReadLinkSupervisionTimeoutOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CReadLinkSupervisionTimeoutCommand::~CReadLinkSupervisionTimeoutCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CReadLinkSupervisionTimeoutCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	}

// Assign new values to the parameters of this command
EXPORT_C void CReadLinkSupervisionTimeoutCommand::Reset(THCIConnectionHandle aConnectionHandle)
	{
	iConnectionHandle = aConnectionHandle;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CReadLinkSupervisionTimeoutCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}



// Extension function.  Use this to retrieve any extension interfaces from CReadLinkSupervisionTimeoutCommand
// or any class from which it derives.
/*virtual*/ TInt CReadLinkSupervisionTimeoutCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

