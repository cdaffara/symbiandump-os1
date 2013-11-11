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
#include <mmf/server/mmfdevsoundcustomcommands.h>
#include <ecom/ecom.h>
#include <mm/mmpluginutils.h>


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

	}

const TInt KMuxTempBufferSize = 20;

// create a custom interface Mux implementation
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFDevSoundCIMuxUtility::CreateCustomInterfaceMuxL(TUid aInterfaceId)
	{
	// The Uid of the plugin will be the match string
	TInt uidAsInteger = aInterfaceId.iUid;
	TBuf8<KMuxTempBufferSize> tempBuffer;
	tempBuffer.Num(uidAsInteger, EHex); // has value
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
	// open packet = command, interface, inBuff
	TUid commandId(KNullUid);
	commandId.iUid = EMMFDevSoundCustomCommandCIOpenSlave;
	TPckgBuf<TUid> idBuffer(aInterface);
	
	return iCustomChannel->SyncCustomCommand(commandId, idBuffer, aPackageBuf, NULL);
	}
	
void CMMFDevSoundCIMuxUtility::CloseSlave(TInt aHandle)
	{
	// 1. send closeslave custom command to remote side
	// 2. demuxplugin removes its handle for this interface
	TUid commandId(KNullUid);
	commandId.iUid = EMMFDevSoundCustomCommandCICloseSlave;
	TPckgBuf<TInt> handleBuffer(aHandle);
	
	iCustomChannel->SyncCustomCommand(commandId, handleBuffer, KNullDesC8, NULL);
	}


TInt CMMFDevSoundCIMuxUtility::SendSlaveSyncCommand(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf)
	{
	TUid commandId(KNullUid);
	commandId.iUid = EMMFDevSoundCustomCommandCISendSlaveSyncCommand;
		
	TMMFDevSoundCustomInterfaceCommand commandPair;
	commandPair.iCommand = aCommand;
	commandPair.iHandle = aHandle;
	TPckgBuf<TMMFDevSoundCustomInterfaceCommand> comBuffer(commandPair);
	
	return iCustomChannel->SyncCustomCommand(commandId, comBuffer, aPackageBuf, NULL);
	}

TInt CMMFDevSoundCIMuxUtility::SendSlaveSyncCommandResult(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf, TDes8& aResultBuf)
	{
	TUid commandId(KNullUid);
	commandId.iUid = EMMFDevSoundCustomCommandCISendSlaveSyncCommandResult;
		
	TMMFDevSoundCustomInterfaceCommand commandPair;
	commandPair.iCommand = aCommand;
	commandPair.iHandle = aHandle;
	TPckgBuf<TMMFDevSoundCustomInterfaceCommand> comBuffer(commandPair);

	return iCustomChannel->SyncCustomCommand(commandId, comBuffer, aPackageBuf, &aResultBuf);
	}

void CMMFDevSoundCIMuxUtility::SendSlaveAsyncCommand(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf)
	{
	TUid commandId(KNullUid);
	commandId.iUid = EMMFDevSoundCustomCommandCISendSlaveAsyncCommand;
	
	// command and handle comes from client with asynchronous commands
		
	iCustomChannel->AsyncCustomCommand(commandId, aStatus, aComPackage, aPackageBuf, NULL);
	}
	
void CMMFDevSoundCIMuxUtility::SendSlaveAsyncCommandResult(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf, TDes8& aResultBuf)
	{
	TUid commandId(KNullUid);
	commandId.iUid = EMMFDevSoundCustomCommandCISendSlaveAsyncCommandResult;
		
	// command and handle comes from client with asynchronous commands

	iCustomChannel->AsyncCustomCommand(commandId, aStatus, aComPackage, aPackageBuf, &aResultBuf);
	}

CMMFDevSoundCIMuxUtility::CMMFDevSoundCIMuxUtility(MMMFDevSoundCustomInterfaceChannel* aCustomChannel)
: iCustomChannel(aCustomChannel)
	{
	}

