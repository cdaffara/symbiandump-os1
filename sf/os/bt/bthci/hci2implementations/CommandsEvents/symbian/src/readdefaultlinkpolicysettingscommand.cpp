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

#include <bluetooth/hci/readdefaultlinkpolicysettingscommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CReadDefaultLinkPolicySettingsCommand* CReadDefaultLinkPolicySettingsCommand::NewL()
	{
	CReadDefaultLinkPolicySettingsCommand* self = new (ELeave) CReadDefaultLinkPolicySettingsCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CReadDefaultLinkPolicySettingsCommand::CReadDefaultLinkPolicySettingsCommand()
	: CHCICommandBase(KReadDefaultLinkPolicySettingsOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CReadDefaultLinkPolicySettingsCommand::~CReadDefaultLinkPolicySettingsCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CReadDefaultLinkPolicySettingsCommand::Format(CHctlCommandFrame& /*aCommandFrame*/) const
	{
	
	}

// Assign new values to the parameters of this command
EXPORT_C void CReadDefaultLinkPolicySettingsCommand::Reset()
	{
	
	}

// Accessor methods for the parameters of the command



// Extension function.  Use this to retrieve any extension interfaces from CReadDefaultLinkPolicySettingsCommand
// or any class from which it derives.
/*virtual*/ TInt CReadDefaultLinkPolicySettingsCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

