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
#include "MmfDevSoundCIMuxUtility.h" // included for command definitions
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>
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
	TPckgBuf<TA3FCustomInterfaceCommand> commandBuf;
	MmfMessageUtil::ReadL(aMessage, 1, commandBuf);
	CMMFDevSoundCIMuxUtility::TMMFDevSoundCustomCommand commandType = commandBuf().iType;
	TInt handle = commandBuf().iHandle;
	TInt retVal = KErrNotFound;
	
	switch (commandType)
		{
		case CMMFDevSoundCIMuxUtility::EMMFDevSoundCustomCommandCIOpenSlave:
			{
			// get a local copy of descriptor
			RBuf8 tempBuf;
			tempBuf.CleanupClosePushL();
			TInt len = InputDesLength(aMessage);
			if (len < 0)
				{
				User::Leave(KErrBadDescriptor);
				}
			else
				{
				tempBuf.CreateL(len);
				}
			ReadFromInputDesL(aMessage, &tempBuf);
			
			TUid interfaceUid(KNullUid);
			interfaceUid.iUid = handle;
			TPckgBuf<TUid> idBuffer(interfaceUid);
			
			retVal = iInterface->DoOpenSlaveL(idBuffer(), tempBuf);		
			CleanupStack::PopAndDestroy(&tempBuf);
			break;
			}
		case CMMFDevSoundCIMuxUtility::EMMFDevSoundCustomCommandCICloseSlave:
			{
			TPckgBuf<TInt> handleBuffer(handle);
			iInterface->DoCloseSlaveL(handleBuffer());
			retVal = KErrNone; // no return from CloseSlave
			break;
			}
		case CMMFDevSoundCIMuxUtility::EMMFDevSoundCustomCommandCISendSlaveSyncCommand:
			{
			retVal = iInterface->DoSendSlaveSyncCommandL(aMessage);
			break;
			}
		case CMMFDevSoundCIMuxUtility::EMMFDevSoundCustomCommandCISendSlaveSyncCommandResult:
			{
			retVal = iInterface->DoSendSlaveSyncCommandResultL(aMessage);
			break;
			}
		case CMMFDevSoundCIMuxUtility::EMMFDevSoundCustomCommandCISendSlaveAsyncCommand:
			{
			iInterface->DoSendSlaveAsyncCommandL(aMessage);
			retVal = KErrNone; // no return from async
			break;
			}
		case CMMFDevSoundCIMuxUtility::EMMFDevSoundCustomCommandCISendSlaveAsyncCommandResult:
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
	// data is stored as destination, custom command info, inbuf, outbuf
	TPckgBuf<TA3FCustomInterfaceCommand> comBuffer;
	aMessage.ReadL(1, comBuffer);
	
	// get command and handle
	aData.iCommand = comBuffer().iCommand;
	aData.iHandle = comBuffer().iHandle;
	}
	
void CMMFDevSoundCIDeMuxUtility::GetAsyncMessageDataL(const RMmfIpcMessage& aMessage, TMMFDevSoundCIMessageData& aData)
	{
	// data is stored as destination, custom command info, inbuf, outbuf,status 
	TPckgBuf<TA3FCustomInterfaceCommand> comBuffer;
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
	TInt len = InputDesLength(aMessage);
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

