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

#include <bluetooth/hci/rolediscoverycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CRoleDiscoveryCommand* CRoleDiscoveryCommand::NewL(THCIConnectionHandle aConnectionHandle)
	{
	CRoleDiscoveryCommand* self = new (ELeave) CRoleDiscoveryCommand(aConnectionHandle);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRoleDiscoveryCommand* CRoleDiscoveryCommand::NewL()
	{
	CRoleDiscoveryCommand* self = new (ELeave) CRoleDiscoveryCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CRoleDiscoveryCommand::CRoleDiscoveryCommand(THCIConnectionHandle aConnectionHandle)
	: CHCICommandBase(KRoleDiscoveryOpcode)
	, iConnectionHandle(aConnectionHandle)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CRoleDiscoveryCommand::CRoleDiscoveryCommand()
	: CHCICommandBase(KRoleDiscoveryOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CRoleDiscoveryCommand::~CRoleDiscoveryCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CRoleDiscoveryCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	}

// Assign new values to the parameters of this command
EXPORT_C void CRoleDiscoveryCommand::Reset(THCIConnectionHandle aConnectionHandle)
	{
	iConnectionHandle = aConnectionHandle;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CRoleDiscoveryCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}



// Extension function.  Use this to retrieve any extension interfaces from CRoleDiscoveryCommand
// or any class from which it derives.
/*virtual*/ TInt CRoleDiscoveryCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

