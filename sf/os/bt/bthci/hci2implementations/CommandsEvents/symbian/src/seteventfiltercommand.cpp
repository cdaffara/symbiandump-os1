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
// on Thu, 29 May 2008 15:17:48 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/seteventfiltercommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CSetEventFilterCommand* CSetEventFilterCommand::NewL(TUint8 aFilterType, TUint8 aFilterConditionType, const TDesC8& aCondition)
	{
	CSetEventFilterCommand* self = new (ELeave) CSetEventFilterCommand(aFilterType, aFilterConditionType, aCondition);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSetEventFilterCommand* CSetEventFilterCommand::NewL()
	{
	CSetEventFilterCommand* self = new (ELeave) CSetEventFilterCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CSetEventFilterCommand::CSetEventFilterCommand(TUint8 aFilterType, TUint8 aFilterConditionType, const TDesC8& aCondition)
	: CHCICommandBase(KSetEventFilterOpcode)
	, iFilterType(aFilterType)
	, iFilterConditionType(aFilterConditionType)
	, iCondition(aCondition)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CSetEventFilterCommand::CSetEventFilterCommand()
	: CHCICommandBase(KSetEventFilterOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CSetEventFilterCommand::~CSetEventFilterCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CSetEventFilterCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutByte(iFilterType);
	aCommandFrame.PutByte(iFilterConditionType);
	aCommandFrame.PutString(iCondition);
	}

// Assign new values to the parameters of this command
EXPORT_C void CSetEventFilterCommand::Reset(TUint8 aFilterType, TUint8 aFilterConditionType, const TDesC8& aCondition)
	{
	iFilterType = aFilterType;
	iFilterConditionType = aFilterConditionType;
	iCondition = aCondition;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint8 CSetEventFilterCommand::FilterType() const
	{
	return iFilterType;
	}

EXPORT_C TUint8 CSetEventFilterCommand::FilterConditionType() const
	{
	return iFilterConditionType;
	}

EXPORT_C TPtrC8 CSetEventFilterCommand::Condition() const
	{
	return TPtrC8(iCondition);
	}



// Extension function.  Use this to retrieve any extension interfaces from CSetEventFilterCommand
// or any class from which it derives.
/*virtual*/ TInt CSetEventFilterCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

