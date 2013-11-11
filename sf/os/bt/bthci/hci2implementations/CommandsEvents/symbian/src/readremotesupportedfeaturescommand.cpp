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
// on Thu, 29 May 2008 15:17:50 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/readremotesupportedfeaturescommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/readremsuppfeatcompleteevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CReadRemoteSupportedFeaturesCommand* CReadRemoteSupportedFeaturesCommand::NewL(THCIConnectionHandle aConnectionHandle)
	{
	CReadRemoteSupportedFeaturesCommand* self = new (ELeave) CReadRemoteSupportedFeaturesCommand(aConnectionHandle);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CReadRemoteSupportedFeaturesCommand* CReadRemoteSupportedFeaturesCommand::NewL()
	{
	CReadRemoteSupportedFeaturesCommand* self = new (ELeave) CReadRemoteSupportedFeaturesCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CReadRemoteSupportedFeaturesCommand::CReadRemoteSupportedFeaturesCommand(THCIConnectionHandle aConnectionHandle)
	: CHCICommandBase(KReadRemoteSupportedFeaturesOpcode)
	, iConnectionHandle(aConnectionHandle)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CReadRemoteSupportedFeaturesCommand::CReadRemoteSupportedFeaturesCommand()
	: CHCICommandBase(KReadRemoteSupportedFeaturesOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CReadRemoteSupportedFeaturesCommand::~CReadRemoteSupportedFeaturesCommand()
	{
	
	}	

/*virtual*/ void CReadRemoteSupportedFeaturesCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == EReadRemSuppFeatCompleteEvent)
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
void CReadRemoteSupportedFeaturesCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	}

// Assign new values to the parameters of this command
EXPORT_C void CReadRemoteSupportedFeaturesCommand::Reset(THCIConnectionHandle aConnectionHandle)
	{
	iConnectionHandle = aConnectionHandle;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CReadRemoteSupportedFeaturesCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}



// Extension function.  Use this to retrieve any extension interfaces from CReadRemoteSupportedFeaturesCommand
// or any class from which it derives.
/*virtual*/ TInt CReadRemoteSupportedFeaturesCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

