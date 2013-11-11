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

#include "MmfDevSoundCIMuxUtility.h"
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>
#include <mmf/server/mmfdevsoundcustominterface.h>
#include <ecom/ecom.h>
#include <mm/mmpluginutils.h>

const TInt KMuxTempBufferSize = 20;


CMMFDevSoundCIMuxUtility* CMMFDevSoundCIMuxUtility::NewL(MMMFDevSoundCustomInterfaceChannel* aCustomChannel)
	{
	CMMFDevSoundCIMuxUtility* self = new (ELeave) CMMFDevSoundCIMuxUtility(aCustomChannel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMMFDevSoundCIMuxUtility::ConstructL()
	{
	// nothing needed in this implementation
	}

CMMFDevSoundCIMuxUtility::~CMMFDevSoundCIMuxUtility()
	{
	iAsyncCustomCommandCleanup.ResetAndDestroy();
	iAsyncCustomCommandCleanup.Close();	
	}

// create a custom interface Mux implementation
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFDevSoundCIMuxUtility::CreateCustomInterfaceMuxL(TUid aInterfaceId)
	{
	// The Uid of the plugin will be used as string for matching the best suitable plug in.
	TInt uidAsInteger = aInterfaceId.iUid;
	TBuf8<KMuxTempBufferSize> tempBuffer;
	tempBuffer.Num(uidAsInteger, EHex);
	TUid interfaceUid = {KUidDevSoundCustomInterfaceMux};
	
	TUid destructorKey;
	MMMFDevSoundCustomInterfaceMuxPlugin* self = 
		static_cast<MMMFDevSoundCustomInterfaceMuxPlugin*>
		(MmPluginUtils::CreateImplementationL(interfaceUid, destructorKey, tempBuffer, KRomOnlyResolverUid));

	// pass the destructor key so class can destroy itself
	self->PassDestructorKey(destructorKey);
	CleanupReleasePushL(*self);

	// attempt to construct the plugin
	self->CompleteConstructL(this);
	CleanupStack::Pop(); // self
		
	return self;
	}

	
// from MMMFDevSoundCustomInterfaceMux interface
TInt CMMFDevSoundCIMuxUtility::OpenSlave(TUid aInterface, const TDesC8& aPackageBuf)
	{
	// 1. send openslave custom command to remote side
	// 2. devsoundsession intercepts this custom command
	//    and creates / opens interface
	TA3FCustomInterfaceCommand command;
	command.iType = EMMFDevSoundCustomCommandCIOpenSlave;
	command.iCommand = 0; // No custom command passed on an OpenSlave
	command.iHandle = aInterface.iUid;
	TPckgBuf<TA3FCustomInterfaceCommand> commandBuffer(command);
	return iCustomChannel->SyncCustomCommand(KUidInterfaceMMFDevSound, commandBuffer, aPackageBuf, NULL);
	}
	
void CMMFDevSoundCIMuxUtility::CloseSlave(TInt aHandle)
	{
	// 1. send closeslave custom command to remote side
	// 2. demuxplugin removes its handle for this interface
	TA3FCustomInterfaceCommand command;
	command.iType = EMMFDevSoundCustomCommandCICloseSlave;
	command.iCommand = 0; // No custom command passed on a CloseSlave
	command.iHandle = aHandle;
	TPckgBuf<TA3FCustomInterfaceCommand> commandBuffer(command);
	iCustomChannel->SyncCustomCommand(KUidInterfaceMMFDevSound, commandBuffer, KNullDesC8, NULL);
	}


TInt CMMFDevSoundCIMuxUtility::SendSlaveSyncCommand(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf)
	{
	TA3FCustomInterfaceCommand command;
	command.iType = EMMFDevSoundCustomCommandCISendSlaveSyncCommand;
	command.iCommand = aCommand;
	command.iHandle = aHandle;
	TPckgBuf<TA3FCustomInterfaceCommand> commandBuffer(command);
	
	return iCustomChannel->SyncCustomCommand(KUidInterfaceMMFDevSound, commandBuffer, aPackageBuf, NULL);
	}

TInt CMMFDevSoundCIMuxUtility::SendSlaveSyncCommandResult(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf, TDes8& aResultBuf)
	{
	TA3FCustomInterfaceCommand command;
	command.iType = EMMFDevSoundCustomCommandCISendSlaveSyncCommandResult;
	command.iCommand = aCommand;
	command.iHandle = aHandle;
	TPckgBuf<TA3FCustomInterfaceCommand> commandBuffer(command);

	return iCustomChannel->SyncCustomCommand(KUidInterfaceMMFDevSound, commandBuffer, aPackageBuf, &aResultBuf);
	}

void CMMFDevSoundCIMuxUtility::SendSlaveAsyncCommand(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf)
	{
	CAsyncCommandCleanup *ciMuxAsyncCommandCleanup=NULL;
	TRAPD(err,ciMuxAsyncCommandCleanup = CAsyncCommandCleanup::NewL(this,iCustomChannel));
	TRequestStatus* status= &aStatus;
	if(err == KErrNone)
		{
		err=iAsyncCustomCommandCleanup.Append(ciMuxAsyncCommandCleanup);
		if(err == KErrNone)
			{
			ciMuxAsyncCommandCleanup->AsyncCustomCommand(EMMFDevSoundCustomCommandCISendSlaveAsyncCommand,aComPackage,aStatus,aPackageBuf,NULL);		
			}
		else
			{
			User::RequestComplete(status,err);
			delete ciMuxAsyncCommandCleanup;
			return;			
			}	
		}
	else
		{
		User::RequestComplete(status,err);
		return;	
		}
	
	}
	
void CMMFDevSoundCIMuxUtility::SendSlaveAsyncCommandResult(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf, TDes8& aResultBuf)
	{
	CAsyncCommandCleanup* ciMuxAsyncCommandCleanup=NULL;
	TRAPD(err,ciMuxAsyncCommandCleanup = CAsyncCommandCleanup::NewL(this,iCustomChannel));
	TRequestStatus* status= &aStatus;
	if(err == KErrNone)
		{
		err=iAsyncCustomCommandCleanup.Append(ciMuxAsyncCommandCleanup);
		if(err == KErrNone)
			{
			ciMuxAsyncCommandCleanup->AsyncCustomCommand(EMMFDevSoundCustomCommandCISendSlaveAsyncCommandResult,aComPackage,aStatus,aPackageBuf,&aResultBuf);
			}
		else
			{
			User::RequestComplete(status,err);
			delete ciMuxAsyncCommandCleanup;
			return;			
			}	
		}
	else
		{
		User::RequestComplete(status,err);
		return;
		}
	}

CMMFDevSoundCIMuxUtility::CMMFDevSoundCIMuxUtility(MMMFDevSoundCustomInterfaceChannel* aCustomChannel)
: iCustomChannel(aCustomChannel)
	{
	}
	
void CMMFDevSoundCIMuxUtility::RemoveAsyncCommand(CAsyncCommandCleanup* aAsyncCustomCommandCleanup)
	{
	TInt index = iAsyncCustomCommandCleanup.Find(aAsyncCustomCommandCleanup);
	__ASSERT_DEBUG( index != KErrNotFound,User::Invariant());
	if(index > KErrNotFound)
		{
		iAsyncCustomCommandCleanup.Remove(index);
		iAsyncCustomCommandCleanup.Compress();
		}
	return;
	}
	
CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup* CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup::NewL(CMMFDevSoundCIMuxUtility* aMuxUtility,MMMFDevSoundCustomInterfaceChannel* aCustomChannel)
	{
	CAsyncCommandCleanup *self= new (ELeave) CAsyncCommandCleanup(aMuxUtility,aCustomChannel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup::ConstructL()
	{
	iCommandBuffer = new (ELeave) TPckgBuf<TA3FCustomInterfaceCommand>;
	}
	
CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup::CAsyncCommandCleanup(CMMFDevSoundCIMuxUtility* aMuxUtility,MMMFDevSoundCustomInterfaceChannel* aCustomChannel)
:CActive(CActive::EPriorityStandard),iMuxUtility(aMuxUtility),iCustomChannel(aCustomChannel)
	{
	CActiveScheduler::Add(this);
	}
	
CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup::~CAsyncCommandCleanup()
	{
	delete iCommandBuffer;
	Cancel();
	}
	
void CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup::AsyncCustomCommand(CMMFDevSoundCIMuxUtility::TMMFDevSoundCustomCommand aType,TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf, TDes8* aResultBuf)
	{
	iClientRequestStatus = &aStatus;
	
	(*iCommandBuffer)().iType = aType;
	(*iCommandBuffer)().iCommand = aComPackage().iCommand;
	(*iCommandBuffer)().iHandle = aComPackage().iHandle;
	*iClientRequestStatus = KRequestPending;
	iCustomChannel->AsyncCustomCommand(KUidInterfaceMMFDevSound,iStatus, *iCommandBuffer, aPackageBuf, aResultBuf);
	SetActive();
	}
	
void CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup::RunL()
	{
	if(iClientRequestStatus)
		{
		User::RequestComplete(iClientRequestStatus,iStatus.Int());		
		}
	iClientRequestStatus = NULL;
	iMuxUtility->RemoveAsyncCommand(this);
	delete this;
	}
	
void CMMFDevSoundCIMuxUtility::CAsyncCommandCleanup::DoCancel()
	{

	}
