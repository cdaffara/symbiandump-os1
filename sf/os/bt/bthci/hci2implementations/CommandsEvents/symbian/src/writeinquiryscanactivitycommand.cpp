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

#include <bluetooth/hci/writeinquiryscanactivitycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CWriteInquiryScanActivityCommand* CWriteInquiryScanActivityCommand::NewL(TUint16 aInquiryScanInterval, TUint16 aInquiryScanWindow)
	{
	CWriteInquiryScanActivityCommand* self = new (ELeave) CWriteInquiryScanActivityCommand(aInquiryScanInterval, aInquiryScanWindow);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteInquiryScanActivityCommand* CWriteInquiryScanActivityCommand::NewL()
	{
	CWriteInquiryScanActivityCommand* self = new (ELeave) CWriteInquiryScanActivityCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteInquiryScanActivityCommand::CWriteInquiryScanActivityCommand(TUint16 aInquiryScanInterval, TUint16 aInquiryScanWindow)
	: CHCICommandBase(KWriteInquiryScanActivityOpcode)
	, iInquiryScanInterval(aInquiryScanInterval)
	, iInquiryScanWindow(aInquiryScanWindow)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteInquiryScanActivityCommand::CWriteInquiryScanActivityCommand()
	: CHCICommandBase(KWriteInquiryScanActivityOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CWriteInquiryScanActivityCommand::~CWriteInquiryScanActivityCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteInquiryScanActivityCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutBytes16(iInquiryScanInterval);
	aCommandFrame.PutBytes16(iInquiryScanWindow);
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteInquiryScanActivityCommand::Reset(TUint16 aInquiryScanInterval, TUint16 aInquiryScanWindow)
	{
	iInquiryScanInterval = aInquiryScanInterval;
	iInquiryScanWindow = aInquiryScanWindow;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint16 CWriteInquiryScanActivityCommand::InquiryScanInterval() const
	{
	return iInquiryScanInterval;
	}

EXPORT_C TUint16 CWriteInquiryScanActivityCommand::InquiryScanWindow() const
	{
	return iInquiryScanWindow;
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteInquiryScanActivityCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteInquiryScanActivityCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

