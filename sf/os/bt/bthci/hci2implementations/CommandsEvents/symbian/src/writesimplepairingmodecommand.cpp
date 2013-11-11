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

#include <bluetooth/hci/writesimplepairingmodecommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CWriteSimplePairingModeCommand* CWriteSimplePairingModeCommand::NewL(TUint8 aSimplePairingMode)
	{
	CWriteSimplePairingModeCommand* self = new (ELeave) CWriteSimplePairingModeCommand(aSimplePairingMode);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteSimplePairingModeCommand* CWriteSimplePairingModeCommand::NewL()
	{
	CWriteSimplePairingModeCommand* self = new (ELeave) CWriteSimplePairingModeCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteSimplePairingModeCommand::CWriteSimplePairingModeCommand(TUint8 aSimplePairingMode)
	: CHCICommandBase(KWriteSimplePairingModeOpcode)
	, iSimplePairingMode(aSimplePairingMode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteSimplePairingModeCommand::CWriteSimplePairingModeCommand()
	: CHCICommandBase(KWriteSimplePairingModeOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CWriteSimplePairingModeCommand::~CWriteSimplePairingModeCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteSimplePairingModeCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutByte(iSimplePairingMode);
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteSimplePairingModeCommand::Reset(TUint8 aSimplePairingMode)
	{
	iSimplePairingMode = aSimplePairingMode;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint8 CWriteSimplePairingModeCommand::SimplePairingMode() const
	{
	return iSimplePairingMode;
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteSimplePairingModeCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteSimplePairingModeCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

