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

#include <bluetooth/hci/flowspecificationcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/flowspecificationcompleteevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CFlowSpecificationCommand* CFlowSpecificationCommand::NewL(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency)
	{
	CFlowSpecificationCommand* self = new (ELeave) CFlowSpecificationCommand(aConnectionHandle, aFlags, aFlowDirection, aServiceType, aTokenRate, aTokenBucketSize, aPeakBandwidth, aAccessLatency);
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CFlowSpecificationCommand* CFlowSpecificationCommand::NewL()
	{
	CFlowSpecificationCommand* self = new (ELeave) CFlowSpecificationCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CFlowSpecificationCommand::CFlowSpecificationCommand(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency)
	: CHCICommandBase(KFlowSpecificationOpcode)
	, iConnectionHandle(aConnectionHandle)
	, iFlags(aFlags)
	, iFlowDirection(aFlowDirection)
	, iServiceType(aServiceType)
	, iTokenRate(aTokenRate)
	, iTokenBucketSize(aTokenBucketSize)
	, iPeakBandwidth(aPeakBandwidth)
	, iAccessLatency(aAccessLatency)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

CFlowSpecificationCommand::CFlowSpecificationCommand()
	: CHCICommandBase(KFlowSpecificationOpcode)
	{
	SetExpectsCommandCompleteEvent(EFalse);
	}

// Destructor
CFlowSpecificationCommand::~CFlowSpecificationCommand()
	{
	
	}	

/*virtual*/ void CFlowSpecificationCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (aEvent.EventCode() == EFlowSpecificationCompleteEvent)
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
void CFlowSpecificationCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutConnectionHandle(iConnectionHandle);
	aCommandFrame.PutByte(iFlags);
	aCommandFrame.PutByte(iFlowDirection);
	aCommandFrame.PutByte(iServiceType);
	aCommandFrame.PutBytes32(iTokenRate, 4);
	aCommandFrame.PutBytes32(iTokenBucketSize, 4);
	aCommandFrame.PutBytes32(iPeakBandwidth, 4);
	aCommandFrame.PutBytes32(iAccessLatency, 4);
	}

// Assign new values to the parameters of this command
EXPORT_C void CFlowSpecificationCommand::Reset(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency)
	{
	iConnectionHandle = aConnectionHandle;
	iFlags = aFlags;
	iFlowDirection = aFlowDirection;
	iServiceType = aServiceType;
	iTokenRate = aTokenRate;
	iTokenBucketSize = aTokenBucketSize;
	iPeakBandwidth = aPeakBandwidth;
	iAccessLatency = aAccessLatency;
	}

// Accessor methods for the parameters of the command

EXPORT_C THCIConnectionHandle CFlowSpecificationCommand::ConnectionHandle() const
	{
	return iConnectionHandle;
	}

EXPORT_C TUint8 CFlowSpecificationCommand::Flags() const
	{
	return iFlags;
	}

EXPORT_C TUint8 CFlowSpecificationCommand::FlowDirection() const
	{
	return iFlowDirection;
	}

EXPORT_C TUint8 CFlowSpecificationCommand::ServiceType() const
	{
	return iServiceType;
	}

EXPORT_C TUint32 CFlowSpecificationCommand::TokenRate() const
	{
	return iTokenRate;
	}

EXPORT_C TUint32 CFlowSpecificationCommand::TokenBucketSize() const
	{
	return iTokenBucketSize;
	}

EXPORT_C TUint32 CFlowSpecificationCommand::PeakBandwidth() const
	{
	return iPeakBandwidth;
	}

EXPORT_C TUint32 CFlowSpecificationCommand::AccessLatency() const
	{
	return iAccessLatency;
	}



// Extension function.  Use this to retrieve any extension interfaces from CFlowSpecificationCommand
// or any class from which it derives.
/*virtual*/ TInt CFlowSpecificationCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

