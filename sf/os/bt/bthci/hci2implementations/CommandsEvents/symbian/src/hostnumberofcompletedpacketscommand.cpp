// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// on Wed, 07 Apr 2010 11:59:26 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/hostnumberofcompletedpacketscommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CHostNumberOfCompletedPacketsCommand* CHostNumberOfCompletedPacketsCommand::NewL(TUint8 aNumberOfHandles, const RArray< THCIConnectionHandle >& aConnectionHandle, const RArray< THCINumOfCompletedPackets >& aHostNumOfCompletedPackets)
	{
	CHostNumberOfCompletedPacketsCommand* self = new (ELeave) CHostNumberOfCompletedPacketsCommand(aNumberOfHandles);
	CleanupStack::PushL(self);
	self->ConstructL(aConnectionHandle, aHostNumOfCompletedPackets);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHostNumberOfCompletedPacketsCommand* CHostNumberOfCompletedPacketsCommand::NewL()
	{
	CHostNumberOfCompletedPacketsCommand* self = new (ELeave) CHostNumberOfCompletedPacketsCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CHostNumberOfCompletedPacketsCommand::CHostNumberOfCompletedPacketsCommand(TUint8 aNumberOfHandles)
	: CHCICommandBase(KHostNumberOfCompletedPacketsOpcode)
	, iNumberOfHandles(aNumberOfHandles)
	{
	SetCreditsConsumed(0);
	SetExpectsCommandStatusEvent(EFalse);
	SetExpectsCommandCompleteEvent(EFalse);
	}

CHostNumberOfCompletedPacketsCommand::CHostNumberOfCompletedPacketsCommand()
	: CHCICommandBase(KHostNumberOfCompletedPacketsOpcode)
	{
	SetCreditsConsumed(0);
	SetExpectsCommandStatusEvent(EFalse);
	SetExpectsCommandCompleteEvent(EFalse);
	}

void CHostNumberOfCompletedPacketsCommand::ConstructL(const RArray< THCIConnectionHandle >& aConnectionHandle, const RArray< THCINumOfCompletedPackets >& aHostNumOfCompletedPackets)
	{
	CHCICommandBase::BaseConstructL();
	iConnectionHandle = aConnectionHandle;
	iHostNumOfCompletedPackets = aHostNumOfCompletedPackets;
	}

// Destructor
CHostNumberOfCompletedPacketsCommand::~CHostNumberOfCompletedPacketsCommand()
	{
	iConnectionHandle.Close();
	iHostNumOfCompletedPackets.Close();
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CHostNumberOfCompletedPacketsCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutByte(iNumberOfHandles);
	for(int i=0;i<iNumberOfHandles;++i)
		{
		aCommandFrame.PutConnectionHandle(iConnectionHandle[i]);
		aCommandFrame.PutBytes16(iHostNumOfCompletedPackets[i]);
		}
	}

// Assign new values to the parameters of this command
EXPORT_C void CHostNumberOfCompletedPacketsCommand::Reset(TUint8 aNumberOfHandles, const RArray< THCIConnectionHandle >& aConnectionHandle, const RArray< THCINumOfCompletedPackets >& aHostNumOfCompletedPackets)
	{
	iNumberOfHandles = aNumberOfHandles;
	iConnectionHandle = aConnectionHandle;
	iHostNumOfCompletedPackets = aHostNumOfCompletedPackets;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint8 CHostNumberOfCompletedPacketsCommand::NumberOfHandles() const
	{
	return iNumberOfHandles;
	}

EXPORT_C THCIConnectionHandle CHostNumberOfCompletedPacketsCommand::ConnectionHandle(TInt aIndex) const
	{
	return iConnectionHandle[aIndex];
	}

EXPORT_C THCINumOfCompletedPackets CHostNumberOfCompletedPacketsCommand::HostNumOfCompletedPackets(TInt aIndex) const
	{
	return iHostNumOfCompletedPackets[aIndex];
	}



// Extension function.  Use this to retrieve any extension interfaces from CHostNumberOfCompletedPacketsCommand
// or any class from which it derives.
/*virtual*/ TInt CHostNumberOfCompletedPacketsCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

