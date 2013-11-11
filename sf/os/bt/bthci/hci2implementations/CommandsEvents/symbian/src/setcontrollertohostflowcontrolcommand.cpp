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

#include <bluetooth/hci/setcontrollertohostflowcontrolcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CSetControllerToHostFlowControlCommand* CSetControllerToHostFlowControlCommand::NewL(TUint8 aFlowControlEnable)
	{
	CSetControllerToHostFlowControlCommand* self = new (ELeave) CSetControllerToHostFlowControlCommand(aFlowControlEnable);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSetControllerToHostFlowControlCommand* CSetControllerToHostFlowControlCommand::NewL()
	{
	CSetControllerToHostFlowControlCommand* self = new (ELeave) CSetControllerToHostFlowControlCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CSetControllerToHostFlowControlCommand::CSetControllerToHostFlowControlCommand(TUint8 aFlowControlEnable)
	: CHCICommandBase(KSetControllerToHostFlowControlOpcode)
	, iFlowControlEnable(aFlowControlEnable)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CSetControllerToHostFlowControlCommand::CSetControllerToHostFlowControlCommand()
	: CHCICommandBase(KSetControllerToHostFlowControlOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CSetControllerToHostFlowControlCommand::~CSetControllerToHostFlowControlCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CSetControllerToHostFlowControlCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutByte(iFlowControlEnable);
	}

// Assign new values to the parameters of this command
EXPORT_C void CSetControllerToHostFlowControlCommand::Reset(TUint8 aFlowControlEnable)
	{
	iFlowControlEnable = aFlowControlEnable;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint8 CSetControllerToHostFlowControlCommand::FlowControlEnable() const
	{
	return iFlowControlEnable;
	}



// Extension function.  Use this to retrieve any extension interfaces from CSetControllerToHostFlowControlCommand
// or any class from which it derives.
/*virtual*/ TInt CSetControllerToHostFlowControlCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

