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
// on Wed, 07 Apr 2010 11:59:29 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/writestoredlinkkeycommand.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif



// Factory methods

EXPORT_C CWriteStoredLinkKeyCommand* CWriteStoredLinkKeyCommand::NewL(TUint8 aNumKeysToWrite, const RArray< TBTDevAddr >& aBDADDR, const RArray< TBTLinkKey >& aLinkKey)
	{
	CWriteStoredLinkKeyCommand* self = new (ELeave) CWriteStoredLinkKeyCommand(aNumKeysToWrite);
	CleanupStack::PushL(self);
	self->ConstructL(aBDADDR, aLinkKey);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWriteStoredLinkKeyCommand* CWriteStoredLinkKeyCommand::NewL()
	{
	CWriteStoredLinkKeyCommand* self = new (ELeave) CWriteStoredLinkKeyCommand();
	CleanupStack::PushL(self);
	self->CHCICommandBase::BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Constructors

CWriteStoredLinkKeyCommand::CWriteStoredLinkKeyCommand(TUint8 aNumKeysToWrite)
	: CHCICommandBase(KWriteStoredLinkKeyOpcode)
	, iNumKeysToWrite(aNumKeysToWrite)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

CWriteStoredLinkKeyCommand::CWriteStoredLinkKeyCommand()
	: CHCICommandBase(KWriteStoredLinkKeyOpcode)
	{
	SetExpectsCommandStatusEvent(EFalse);
	}

void CWriteStoredLinkKeyCommand::ConstructL(const RArray< TBTDevAddr >& aBDADDR, const RArray< TBTLinkKey >& aLinkKey)
	{
	CHCICommandBase::BaseConstructL();
	iBDADDR = aBDADDR;
	iLinkKey = aLinkKey;
	}

// Destructor
CWriteStoredLinkKeyCommand::~CWriteStoredLinkKeyCommand()
	{
	iBDADDR.Close();
	iLinkKey.Close();
	}	



// Override of pure virtual from CHCICommandBase. This method embodies the knowledge
// of how to format the specifics of this command into the HCTL command frame.
void CWriteStoredLinkKeyCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutByte(iNumKeysToWrite);
	for(int i=0;i<iNumKeysToWrite;++i)
		{
		aCommandFrame.PutDevAddr(iBDADDR[i]);
		aCommandFrame.PutLinkKey(iLinkKey[i]);
		}
	}

// Assign new values to the parameters of this command
EXPORT_C void CWriteStoredLinkKeyCommand::Reset(TUint8 aNumKeysToWrite, const RArray< TBTDevAddr >& aBDADDR, const RArray< TBTLinkKey >& aLinkKey)
	{
	iNumKeysToWrite = aNumKeysToWrite;
	iBDADDR = aBDADDR;
	iLinkKey = aLinkKey;
	}

// Accessor methods for the parameters of the command

EXPORT_C TUint8 CWriteStoredLinkKeyCommand::NumKeysToWrite() const
	{
	return iNumKeysToWrite;
	}

EXPORT_C TBTDevAddr CWriteStoredLinkKeyCommand::BDADDR(TInt aIndex) const
	{
	return iBDADDR[aIndex];
	}

EXPORT_C TBTLinkKey CWriteStoredLinkKeyCommand::LinkKey(TInt aIndex) const
	{
	return iLinkKey[aIndex];
	}



// Extension function.  Use this to retrieve any extension interfaces from CWriteStoredLinkKeyCommand
// or any class from which it derives.
/*virtual*/ TInt CWriteStoredLinkKeyCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	// To add an additional interface implementation specific for this class check the 
	// provided ID and return an appropriate interface.

	// If a specific interface implementation is not provided - forward the call to the base class.
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

