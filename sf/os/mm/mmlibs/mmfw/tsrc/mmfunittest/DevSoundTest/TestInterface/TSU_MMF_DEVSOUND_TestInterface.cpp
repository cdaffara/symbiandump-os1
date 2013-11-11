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

#include "TSU_MMF_DEVSOUND_TestInterface.h"
#include "TSU_MMF_DEVSOUND_TestInterface_ImplementationUID.hrh"
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <s32mem.h>

const TUid KUidDevSoundTestCustomInterface = {KMmfDevSoundTestCustomInterfaceMux};

// __________________________________________________________________________
// Implementation

// MUX //

TInt CMMF_TSU_DEVSOUND_TestInterfaceMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = KNullHandle;
	TUid slaveId = {KMmfDevSoundTestCustomInterfaceDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle > KNullHandle)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

void CMMF_TSU_DEVSOUND_TestInterfaceMux::Release()
	{
	// close the slave device if it exists
	if (iRemoteHandle != KNullHandle)
		{
		// we assume the slave is closed correctly
		iUtility->CloseSlave(iRemoteHandle);
		}
	
	TUid key = iKey;
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}

void CMMF_TSU_DEVSOUND_TestInterfaceMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}
	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMF_TSU_DEVSOUND_TestInterfaceMux::NewL()
	{
	CMMF_TSU_DEVSOUND_TestInterfaceMux* bob = new (ELeave) CMMF_TSU_DEVSOUND_TestInterfaceMux;
	return bob;
	}
	
TAny* CMMF_TSU_DEVSOUND_TestInterfaceMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFDevSoundTestCustomInterface* interface = this;
	return interface;
	}
	
CMMF_TSU_DEVSOUND_TestInterfaceMux::CMMF_TSU_DEVSOUND_TestInterfaceMux() : 
	CActive(CActive::EPriorityStandard)
	{	
	CActiveScheduler::Add(this);
	}

CMMF_TSU_DEVSOUND_TestInterfaceMux::~CMMF_TSU_DEVSOUND_TestInterfaceMux()
	{	
	}

/*
EMMFDevSoundCITestSync = 0,
EMMFDevSoundCITestSyncResult,
EMMFDevSoundCITestAsync,
EMMFDevSoundCITestAsyncResult
*/

// test interface
TInt CMMF_TSU_DEVSOUND_TestInterfaceMux::TestSyncCommand(const TDesC8& aDes)
	{
	if (iRemoteHandle == KNullHandle)
		{
		User::Leave(KErrNotReady);
		}
	
	// send EMMFDevSoundCITestSync command to slave
	TInt err = KErrNotSupported;
	err = iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCITestSync, aDes);
	
	// return error to test client
	return err;
	}
	
TInt CMMF_TSU_DEVSOUND_TestInterfaceMux::TestSyncCommandResult(const TDesC8& aDes, TDes8& aRes)
	{
	if (iRemoteHandle == KNullHandle)
		{
		User::Leave(KErrNotReady);
		}
	
	// send EMMFDevSoundCITestSyncResult command to slave
	TInt err = KErrNotSupported;
	err = iUtility->SendSlaveSyncCommandResult(iRemoteHandle, EMMFDevSoundCITestSyncResult, aDes, aRes);
	
	// return error to test client
	return err;
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceMux::TestAsyncCommandL(const TDesC8& aDes, TRequestStatus& aStatus)
	{
	if (iRemoteHandle == KNullHandle)
		{
		User::Leave(KErrNotReady);
		}
	
	// check not already active
	if (IsActive())
		{
		User::Leave(KErrNotSupported);
		}
	
	// take a copy of the client status
	iClientStatus = &aStatus;	
	
	// otherwise setup the active object
	SetActive();
	
	// send EMMFDevSoundCITestAsync command to slave
	// this will complete automatically asynchronously
	iComPackage().iHandle = iRemoteHandle;
	iComPackage().iCommand = EMMFDevSoundCITestAsync;
	
	iUtility->SendSlaveAsyncCommand(iComPackage, iStatus, aDes);
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceMux::TestAsyncCommandResultL(const TDesC8& aDes, TDes8& aRes, TRequestStatus& aStatus)
	{
	if (iRemoteHandle == KNullHandle)
		{
		User::Leave(KErrNotReady);
		}
	
	// check not already active
	if (IsActive())
		{
		User::Leave(KErrNotSupported);
		}
	
	// take a copy of the client status
	iClientStatus = &aStatus;
	
	// otherwise setup the active object
	SetActive();
	
	// send EMMFDevSoundCITestAsyncResult command to slave
	// this will complete automatically asynchronously
	iComPackage().iHandle = iRemoteHandle;
	iComPackage().iCommand = EMMFDevSoundCITestAsyncResult;
	
	iUtility->SendSlaveAsyncCommandResult(iComPackage, iStatus, aDes, aRes);
	}

// active object handling functions
void CMMF_TSU_DEVSOUND_TestInterfaceMux::RunL()
	{
	TInt err = iStatus.Int();
	
	// complete the client request
	User::RequestComplete(iClientStatus, err);
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceMux::DoCancel()
	{
	// XXX - do we need to send a cancel message to the DeMux plugin?
	if (iClientStatus->Int() == KRequestPending)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}

// DEMUX //	
	

TInt CMMF_TSU_DEVSOUND_TestInterfaceDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::Release()
	{
	TUid key = iKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::RefreshL()
	{
	// refetch the test custom interface if we already have a target
	if (iTarget)
		{
		MMMFDevSoundTestCustomInterface* ptr = NULL;
		ptr = static_cast <MMMFDevSoundTestCustomInterface*> (iTarget->CustomInterface(KUidDevSoundTestCustomInterface));

		// if we found the interface then the UID must be wrong since this is a test interface
		// don't panic though since this is test code
		if (ptr)
			{	
			// keep the ARMV5 compiler happy by doing something
			RDebug::Print(_L("Found the test interface on DevSound - this must be an error!"));
			}
		}
	}

void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::InterfaceDeleted()
	{
	}

MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMF_TSU_DEVSOUND_TestInterfaceDeMux::NewL()
	{
	CMMF_TSU_DEVSOUND_TestInterfaceDeMux* bob = new (ELeave) CMMF_TSU_DEVSOUND_TestInterfaceDeMux;
	return bob;
	}
	
CMMF_TSU_DEVSOUND_TestInterfaceDeMux::CMMF_TSU_DEVSOUND_TestInterfaceDeMux()
	{
	
	
	}

CMMF_TSU_DEVSOUND_TestInterfaceDeMux::~CMMF_TSU_DEVSOUND_TestInterfaceDeMux()
	{

	}


TInt CMMF_TSU_DEVSOUND_TestInterfaceDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the test custom interface
	MMMFDevSoundTestCustomInterface* ptr = NULL;
	ptr = static_cast <MMMFDevSoundTestCustomInterface*> (iTarget->CustomInterface(KUidDevSoundTestCustomInterface));

	// this test interface will not be supported on DevSound but
	// since this is test code we don't care
	if (ptr)
		{	
		// keep the ARMV5 compiler happy by doing something
		RDebug::Print(_L("Found the test interface on DevSound - this must be an error!"));
		}

	return KErrNone;
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMF_TSU_DEVSOUND_TestInterfaceDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	if (data.iCommand != EMMFDevSoundCITestSync)
		{
		User::Leave(KErrNotSupported);
		}
	
	// check descriptor
	HBufC8* tempBuf = HBufC8::NewL(iUtility->InputDesLength(aMessage));
	CleanupStack::PushL(tempBuf);
	TPtr8 tempDes(tempBuf->Des());
	
	iUtility->ReadFromInputDesL(aMessage, &tempDes);
	
	if (tempDes != KDevSoundCITestIn)
		{
		User::Leave(KErrCorrupt);
		}
	
	CleanupStack::PopAndDestroy(); // tempBuf
	
	return KErrNone;
	}
	
TInt CMMF_TSU_DEVSOUND_TestInterfaceDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	if (data.iCommand != EMMFDevSoundCITestSyncResult)
		{
		User::Leave(KErrNotSupported);
		}
	
	// check descriptor
	HBufC8* tempBuf = HBufC8::NewL(iUtility->InputDesLength(aMessage));
	CleanupStack::PushL(tempBuf);
	TPtr8 tempDes(tempBuf->Des());
	
	iUtility->ReadFromInputDesL(aMessage, &tempDes);
	
	if (tempDes != KDevSoundCITestIn)
		{
		User::Leave(KErrCorrupt);
		}
	
	// write to the output buffer
	TPtrC8 outString(KDevSoundCITestOut);
	iUtility->WriteToOutputDesL(aMessage, outString);
	
	CleanupStack::PopAndDestroy(); // tempBuf
	
	return KErrNone;
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetAsyncMessageDataL(aMessage, data);

	if (data.iCommand != EMMFDevSoundCITestAsync)
		{
		User::Leave(KErrNotSupported);
		}
	
	// check descriptor
	HBufC8* tempBuf = HBufC8::NewL(iUtility->InputDesLength(aMessage));
	CleanupStack::PushL(tempBuf);
	TPtr8 tempDes(tempBuf->Des());
	
	iUtility->ReadFromInputDesL(aMessage, &tempDes);
	
	if (tempDes != KDevSoundCITestIn)
		{
		User::Leave(KErrCorrupt);
		}
		
	// complete the message
	iUtility->CompleteMessage(aMessage, KErrNone);
	
	CleanupStack::PopAndDestroy(); // tempBuf
	}
	
void CMMF_TSU_DEVSOUND_TestInterfaceDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetAsyncMessageDataL(aMessage, data);

	if (data.iCommand != EMMFDevSoundCITestAsyncResult)
		{
		User::Leave(KErrNotSupported);
		}
	
	// check descriptor
	HBufC8* tempBuf = HBufC8::NewL(iUtility->InputDesLength(aMessage));
	CleanupStack::PushL(tempBuf);
	TPtr8 tempDes(tempBuf->Des());
	
	iUtility->ReadFromInputDesL(aMessage, &tempDes);
	
	if (tempDes != KDevSoundCITestIn)
		{
		User::Leave(KErrCorrupt);
		}
	
	// write to the output buffer
	TPtrC8 outString(KDevSoundCITestOut);
	iUtility->WriteToOutputDesL(aMessage, outString);
	
	// complete the message
	iUtility->CompleteMessage(aMessage, KErrNone);
	CleanupStack::PopAndDestroy(); // tempBuf
	}

	


//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfDevSoundTestCustomInterfaceMux,	CMMF_TSU_DEVSOUND_TestInterfaceMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfDevSoundTestCustomInterfaceDeMux,	CMMF_TSU_DEVSOUND_TestInterfaceDeMux::NewL),
	};


//
// ImplementationGroupProxy
//
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


