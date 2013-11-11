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

#include <bluetooth/hci/writelocalnamecommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CWriteLocalNameCommand* CWriteLocalNameCommand::NewL(const TDesC8& aLocalName)
	{
	CWriteLocalNameCommand* self = new (ELeave) CWriteLocalNameCommand(aLocalName);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteLocalNameCommand* CWriteLocalNameCommand::NewL()
	{
	CWriteLocalNameCommand* self = new (ELeave) CWriteLocalNameCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteLocalNameCommand::CWriteLocalNameCommand(const TDesC8& aLocalName)
	: CHCICommandBase(KWriteLocalNameOpcode)
	, iLocalName(aLocalName)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteLocalNameCommand::CWriteLocalNameCommand()
	: CHCICommandBase(KWriteLocalNameOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CWriteLocalNameCommand::~CWriteLocalNameCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteLocalNameCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutPaddedString(iLocalName, 248);
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteLocalNameCommand::Reset(const TDesC8& aLocalName)
	{
	iLocalName = aLocalName;
	}

// Accessor methods for the parameters of the command

EXPORT_C TPtrC8 CWriteLocalNameCommand::LocalName() const
	{
	return TPtrC8(iLocalName);
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteLocalNameCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteLocalNameCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

