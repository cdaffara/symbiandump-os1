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
// on Thu, 29 May 2008 15:17:49 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/qossetupcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/qossetupcompleteevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CQOSSetupCommand* CQOSSetupCommand::NewL(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aPeakBandwidth, TUint32 aLatency, TUint32 aDelayVariation)
	{
	CQOSSetupCommand* self = new (ELeave) CQOSSetupCommand(aConnectionHandle, aFlags, aServiceType, aTokenRate, aPeakBandwidth, aLatency, aDelayVariation);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CQOSSetupCommand* CQOSSetupCommand::NewL()
	{
	CQOSSetupCommand* self = new (ELeave) CQOSSetupCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CQOSSetupCommand::CQOSSetupCommand(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aPeakBandwidth, TUint32 aLatency, TUint32 aDelayVariation)
	: CHCICommandBase(KQOSSetupOpcode)
	, iConnectionHandle(aConnectionHandle)
	, iFlags(aFlags)
	, iServiceType(aServiceType)
	, iTokenRate(aTokenRate)
	, iPeakBandwidth(aPeakBandwidth)
	, iLatency(aLatency)
	, iDelayVariation(aDelayVariation)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CQOSSetupCommand::CQOSSetupCommand()
	: CHCICommandBase(KQOSSetupOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CQOSSetupCommand::~CQOSSetupCommand()
	{
	
	}	

/*virtual*/ void CQOSSetupCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == EQOSSetupCompleteEvent)
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
void CQOSSetupCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	aCommandFrame.PutByte(iFlags);
	aCommandFrame.PutByte(iServiceType);
	aCommandFrame.PutBytes32(iTokenRate, 4);
	aCommandFrame.PutBytes32(iPeakBandwidth, 4);
	aCommandFrame.PutBytes32(iLatency, 4);
	aCommandFrame.PutBytes32(iDelayVariation, 4);
	}

// Assign new values to the parameters of this command
EXPORT_C void CQOSSetupCommand::Reset(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aPeakBandwidth, TUint32 aLatency, TUint32 aDelayVariation)
	{
	iConnectionHandle = aConnectionHandle;
	iFlags = aFlags;
	iServiceType = aServiceType;
	iTokenRate = aTokenRate;
	iPeakBandwidth = aPeakBandwidth;
	iLatency = aLatency;
	iDelayVariation = aDelayVariation;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CQOSSetupCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}

EXPORT_C TUint8 CQOSSetupCommand::Flags() const
	{
	return iFlags;
	}

EXPORT_C TUint8 CQOSSetupCommand::ServiceType() const
	{
	return iServiceType;
	}

EXPORT_C TUint32 CQOSSetupCommand::TokenRate() const
	{
	return iTokenRate;
	}

EXPORT_C TUint32 CQOSSetupCommand::PeakBandwidth() const
	{
	return iPeakBandwidth;
	}

EXPORT_C TUint32 CQOSSetupCommand::Latency() const
	{
	return iLatency;
	}

EXPORT_C TUint32 CQOSSetupCommand::DelayVariation() const
	{
	return iDelayVariation;
	}



// Extension function.  Use this to retrieve any extension interfaces from CQOSSetupCommand
// or any class from which it derives.
/*virtual*/ TInt CQOSSetupCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

