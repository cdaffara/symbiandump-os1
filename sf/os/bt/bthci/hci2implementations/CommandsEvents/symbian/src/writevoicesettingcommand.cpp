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
// on Wed, 05 Dec 2007 14:54:27 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/writevoicesettingcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

#pragma message("Value of iVoiceSetting member is closely related to hardware used. ")
#pragma message("Bits: Input Coding, Input Data, Input Sample Size and Linear_PCM_Bit_pos, should be set...")
#pragma message("...according to hardware capabilities, and OR-ed to create appropriate iVoiceSetting value.")

// Factory methods

EXPORT_C CWriteVoiceSettingCommand* CWriteVoiceSettingCommand::NewL(TUint16 aVoiceSetting)
	{
	CWriteVoiceSettingCommand* self = new (ELeave) CWriteVoiceSettingCommand(aVoiceSetting);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteVoiceSettingCommand* CWriteVoiceSettingCommand::NewL()
	{
	CWriteVoiceSettingCommand* self = new (ELeave) CWriteVoiceSettingCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteVoiceSettingCommand::CWriteVoiceSettingCommand(TUint16 aVoiceSetting)
	: CHCICommandBase(KWriteVoiceSettingOpcode)
	, iVoiceSetting(aVoiceSetting)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteVoiceSettingCommand::CWriteVoiceSettingCommand()
	: CHCICommandBase(KWriteVoiceSettingOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

// Destructor
CWriteVoiceSettingCommand::~CWriteVoiceSettingCommand()
	{
	
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteVoiceSettingCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutBytes16(iVoiceSetting);
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteVoiceSettingCommand::Reset(TUint16 aVoiceSetting)
	{
	iVoiceSetting = aVoiceSetting;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint16 CWriteVoiceSettingCommand::VoiceSetting() const
	{
	return iVoiceSetting;
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteVoiceSettingCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteVoiceSettingCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

