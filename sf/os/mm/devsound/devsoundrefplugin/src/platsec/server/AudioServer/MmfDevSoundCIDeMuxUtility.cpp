// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "MmfDevSoundCIDeMuxUtility.h"
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>
#include <mmf/server/mmfdevsoundcustomcommands.h>
#include <ecom/ecom.h>
#include <mm/mmpluginutils.h>


CMMFDevSoundCIDeMuxUtility* CMMFDevSoundCIDeMuxUtility::NewL(MMMFDevSoundCustomInterfaceDeMuxInterface* aInterface)
	{
	CMMFDevSoundCIDeMuxUtility* self = new (ELeave) CMMFDevSoundCIDeMuxUtility(aInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMMFDevSoundCIDeMuxUtility::ConstructL()
	{
	// nothing to do in this plugin
	}

CMMFDevSoundCIDeMuxUtility::~CMMFDevSoundCIDeMuxUtility()
	{

	}

const TInt KDeMuxTempBufferSize = 20;

// create a custom interface Mux implementation
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFDevSoundCIDeMuxUtility::CreateCustomInterfaceDeMuxL(TUid aInterfaceId)
	{
	// The Uid of the plugin will be the match string
	TInt uidAsInteger = aInterfaceId.iUid;

	TBuf8<KDeMuxTempBufferSize> tempBuffer;
	tempBuffer.Num(uidAsInteger, EHex); // has value
	TUid interfaceUid = {KUidDevSoundCustomInterfaceDeMux};
	
	TUid destructorKey;
	MMMFDevSoundCustomInterfaceDeMuxPlugin* self = 
		static_cast<MMMFDevSoundCustomInterfaceDeMuxPlugin*>
		(MmPluginUtils::CreateImplementationL(interfaceUid, destructorKey, tempBuffer, KRomOnlyResolverUid));

	// pass the destructor key so class can destroy itself
	self->PassDestructorKey(destructorKey);
	CleanupReleasePushL(*self);

	// attempt to construct the plugin
	self->CompleteConstructL(this);
	CleanupStack::Pop();	// self
	
	return self;
	}

	
// this will leave if the command is not a supported custom interface command
// the integer being returned is not an error code per-se it is the return code
// from the message being handled and so it makes sense here to have the function
// returning an integer but also able to leave if there is a problem
TInt CMMFDevSoundCIDeMuxUtility::ProcessCustomInterfaceCommandL(const RMmfIpcMessage& aMessage)
	{
	// get command manually - this is quicker than extracting all info
	// plus we don't know whether this is async or sync
	TInt command;
	command = aMessage.Int0();
	
	TInt retVal = KErrNotFound;
	
	switch (command)
		{
		case EMMFDevSoundCustomCommandCIOpenSlave:
			{
			// get a local copy of descriptor
			HBufC8* tempBuf = HBufC8::NewL(User::LeaveIfError(InputDesLength(aMessage)));
			CleanupStack::PushL(tempBuf);
			TPtr8 tempPtr(tempBuf->Des());
			ReadFromInputDesL(aMessage, &tempPtr);
			
			// fetch the interface Uid
			TPckgBuf<TUid> idBuffer;
			aMessage.ReadL(1, idBuffer);
			
			retVal = iInterface->DoOpenSlaveL(idBuffer(), tempPtr);		
			CleanupStack::PopAndDestroy(tempBuf);
			break;
			}
		case EMMFDevSoundCustomCommandCICloseSlave:
			{
			// handle is at offset 1
			TPckgBuf<TInt> handleBuffer;
			aMessage.ReadL(1, handleBuffer);
			iInterface->DoCloseSlaveL(handleBuffer());
			retVal = KErrNone; // no return from CloseSlave
			break;
			}
		case EMMFDevSoundCustomCommandCISendSlaveSyncCommand:
			{
			retVal = iInterface->DoSendSlaveSyncCommandL(aMessage);
			break;
			}
		case EMMFDevSoundCustomCommandCISendSlaveSyncCommandResult:
			{
			retVal = iInterface->DoSendSlaveSyncCommandResultL(aMessage);
			break;
			}
		case EMMFDevSoundCustomCommandCISendSlaveAsyncCommand:
			{
			iInterface->DoSendSlaveAsyncCommandL(aMessage);
			retVal = KErrNone; // no return from async
			break;
			}
		case EMMFDevSoundCustomCommandCISendSlaveAsyncCommandResult:
			{
			iInterface->DoSendSlaveAsyncCommandResultL(aMessage);
			retVal = KErrNone;	// no return from async
			break;
			}
		default:
			User::Leave(retVal);
		}
		
	return retVal;
	}
	

// at the moment these two functions are the same but this may change on different platforms
// so separate sync and async message data functions have been defined
void CMMFDevSoundCIDeMuxUtility::GetSyncMessageDataL(const RMmfIpcMessage& aMessage, TMMFDevSoundCIMessageData& aData)
	{
	// data is stored as commandUID, (command, handle), inbuf, outbuf
	TPckgBuf<TMMFDevSoundCustomInterfaceCommand> comBuffer;
	aMessage.ReadL(1, comBuffer);
	
	// get command and handle
	aData.iCommand = comBuffer().iCommand;
	aData.iHandle = comBuffer().iHandle;
	}
	
void CMMFDevSoundCIDeMuxUtility::GetAsyncMessageDataL(const RMmfIpcMessage& aMessage, TMMFDevSoundCIMessageData& aData)
	{
	// data is stored as commandUID, (command, handle), inbuf, outbuf,status 
	TPckgBuf<TMMFDevSoundCustomInterfaceCommand> comBuffer;
	aMessage.ReadL(1, comBuffer);
	
	// get command and handle
	aData.iCommand = comBuffer().iCommand;
	aData.iHandle = comBuffer().iHandle;
	}


TInt CMMFDevSoundCIDeMuxUtility::InputDesLength(const RMmfIpcMessage& aMessage)
	{
	// input descriptor is at offset 2
	TInt len = aMessage.GetDesLength(2);	
	return len;
	}

void CMMFDevSoundCIDeMuxUtility::ReadFromInputDesL(const RMmfIpcMessage& aMessage, TDes8* aBufToFill)
	{
	// check if the descriptor is large enough
	TInt len = User::LeaveIfError(InputDesLength(aMessage));
	if (len > aBufToFill->MaxLength())
		{
		User::Leave(KErrArgument);
		}
	
	// input descriptor is at offset 2
	aMessage.ReadL(2, *aBufToFill);
	}
	
void CMMFDevSoundCIDeMuxUtility::WriteToOutputDesL(const RMmfIpcMessage& aMessage, TDesC8& aBufToWrite)
	{
	// output descriptor is at offset 3
	aMessage.WriteL(3, aBufToWrite);
	}

	
void CMMFDevSoundCIDeMuxUtility::CompleteMessage(const RMmfIpcMessage& aMessage, TInt aError)
	{
	aMessage.Complete(aError);
	}

CMMFDevSoundCIDeMuxUtility::CMMFDevSoundCIDeMuxUtility(MMMFDevSoundCustomInterfaceDeMuxInterface* aInterface)
: iInterface(aInterface)
	{
	}

