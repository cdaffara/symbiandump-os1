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
// on Wed, 07 Apr 2010 11:59:28 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/writecurrentiaclapcommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CWriteCurrentIACLAPCommand* CWriteCurrentIACLAPCommand::NewL(TUint8 aNumCurrentIAC, const RArray< TUint32 >& aIACLAP)
	{
	CWriteCurrentIACLAPCommand* self = new (ELeave) CWriteCurrentIACLAPCommand(aNumCurrentIAC);
	CleanupStack::PushL(self);
	self->ConstructL(aIACLAP);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteCurrentIACLAPCommand* CWriteCurrentIACLAPCommand::NewL()
	{
	CWriteCurrentIACLAPCommand* self = new (ELeave) CWriteCurrentIACLAPCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteCurrentIACLAPCommand::CWriteCurrentIACLAPCommand(TUint8 aNumCurrentIAC)
	: CHCICommandBase(KWriteCurrentIACLAPOpcode)
	, iNumCurrentIAC(aNumCurrentIAC)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteCurrentIACLAPCommand::CWriteCurrentIACLAPCommand()
	: CHCICommandBase(KWriteCurrentIACLAPOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

void CWriteCurrentIACLAPCommand::ConstructL(const RArray< TUint32 >& aIACLAP)
	{
	CHCICommandBase::BaseConstructL();
	iIACLAP = aIACLAP;
	}

// Destructor
CWriteCurrentIACLAPCommand::~CWriteCurrentIACLAPCommand()
	{
	iIACLAP.Close();
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteCurrentIACLAPCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutByte(iNumCurrentIAC);
	for(int i=0;i<iNumCurrentIAC;++i)
		{
		aCommandFrame.PutBytes32(iIACLAP[i], 3);
		}
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteCurrentIACLAPCommand::Reset(TUint8 aNumCurrentIAC, const RArray< TUint32 >& aIACLAP)
	{
	iNumCurrentIAC = aNumCurrentIAC;
	iIACLAP = aIACLAP;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint8 CWriteCurrentIACLAPCommand::NumCurrentIAC() const
	{
	return iNumCurrentIAC;
	}

EXPORT_C TUint32 CWriteCurrentIACLAPCommand::IACLAP(TInt aIndex) const
	{
	return iIACLAP[aIndex];
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteCurrentIACLAPCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteCurrentIACLAPCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

