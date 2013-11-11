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

#include <bluetooth/hci/pincoderequestreplycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CPINCodeRequestReplyCommand* CPINCodeRequestReplyCommand::NewL(const TBTDevAddr& aBDADDR, TUint8 aPINCodeLength, const TDesC8& aPINCode)
	{
	CPINCodeRequestReplyCommand* self = new (ELeave) CPINCodeRequestReplyCommand(aBDADDR, aPINCodeLength, aPINCode);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPINCodeRequestReplyCommand* CPINCodeRequestReplyCommand::NewL()
	{
	CPINCodeRequestReplyCommand* self = new (ELeave) CPINCodeRequestReplyCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CPINCodeRequestReplyCommand::CPINCodeRequestReplyCommand(const TBTDevAddr& aBDADDR, TUint8 aPINCodeLength, const TDesC8& aPINCode)
	: CHCICommandBase(KPINCodeRequestReplyOpcode)
	, iBDADDR(aBDADDR)
	, iPINCodeLength(aPINCodeLength)
	, iPINCode(aPINCode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CPINCodeRequestReplyCommand::CPINCodeRequestReplyCommand()
	: CHCICommandBase(KPINCodeRequestReplyOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CPINCodeRequestReplyCommand::~CPINCodeRequestReplyCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CPINCodeRequestReplyCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutDevAddr(iBDADDR);
	aCommandFrame.PutByte(iPINCodeLength);
	aCommandFrame.PutPaddedString(iPINCode, 16);
	}

// Assign new values to the parameters of this command
EXPORT_C void CPINCodeRequestReplyCommand::Reset(const TBTDevAddr& aBDADDR, TUint8 aPINCodeLength, const TDesC8& aPINCode)
	{
	iBDADDR = aBDADDR;
	iPINCodeLength = aPINCodeLength;
	iPINCode = aPINCode;
	}

// Accessor methods for the parameters of the command

EXPORT_C TBTDevAddr CPINCodeRequestReplyCommand::BDADDR() const
	{
	return iBDADDR;
	}

EXPORT_C TUint8 CPINCodeRequestReplyCommand::PINCodeLength() const
	{
	return iPINCodeLength;
	}

EXPORT_C TPtrC8 CPINCodeRequestReplyCommand::PINCode() const
	{
	return TPtrC8(iPINCode);
	}



// Extension function.  Use this to retrieve any extension interfaces from CPINCodeRequestReplyCommand
// or any class from which it derives.
/*virtual*/ TInt CPINCodeRequestReplyCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

