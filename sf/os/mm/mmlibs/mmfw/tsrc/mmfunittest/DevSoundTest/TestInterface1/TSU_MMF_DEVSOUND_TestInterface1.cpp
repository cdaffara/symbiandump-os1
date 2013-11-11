// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TSU_MMF_DEVSOUND_TestInterface1.h"
#include "TSU_MMF_DEVSOUND_TestInterface1_ImplementationUID.hrh"
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <s32mem.h>
#include "../TestInterface/TSU_MMF_DEVSOUND_TestInterface.h"

const TUid KUidDevSoundTestCustomInterface = {KMmfDevSoundTestCustomInterface1Mux};

// __________________________________________________________________________
// Implementation

// MUX //

TInt CMMF_TSU_DEVSOUND_TestInterface1Mux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = KNullHandle;
	TUid slaveId = {KMmfDevSoundTestCustomInterface1DeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle > KNullHandle)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

void CMMF_TSU_DEVSOUND_TestInterface1Mux::Release()
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
	
void CMMF_TSU_DEVSOUND_TestInterface1Mux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}

void CMMF_TSU_DEVSOUND_TestInterface1Mux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}
	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMF_TSU_DEVSOUND_TestInterface1Mux::NewL()
	{
	CMMF_TSU_DEVSOUND_TestInterface1Mux* bob = new (ELeave) CMMF_TSU_DEVSOUND_TestInterface1Mux;
	return bob;
	}
	
TAny* CMMF_TSU_DEVSOUND_TestInterface1Mux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFDevSoundTestCustomInterface* interface = this;
	return interface;
	}
	
CMMF_TSU_DEVSOUND_TestInterface1Mux::CMMF_TSU_DEVSOUND_TestInterface1Mux() : 
	CActive(CActive::EPriorityStandard)
	{	
	CActiveScheduler::Add(this);
	}

CMMF_TSU_DEVSOUND_TestInterface1Mux::~CMMF_TSU_DEVSOUND_TestInterface1Mux()
	{	
	}

/*
EMMFDevSoundCITestSync = 0,
EMMFDevSoundCITestSyncResult,
EMMFDevSoundCITestAsync,
EMMFDevSoundCITestAsyncResult
*/

// test interface
TInt CMMF_TSU_DEVSOUND_TestInterface1Mux::TestSyncCommand(const TDesC8& aDes)
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
	
TInt CMMF_TSU_DEVSOUND_TestInterface1Mux::TestSyncCommandResult(const TDesC8& aDes, TDes8& aRes)
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
	
void CMMF_TSU_DEVSOUND_TestInterface1Mux::TestAsyncCommandL(const TDesC8& aDes, TRequestStatus& aStatus)
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
	
void CMMF_TSU_DEVSOUND_TestInterface1Mux::TestAsyncCommandResultL(const TDesC8& aDes, TDes8& aRes, TRequestStatus& aStatus)
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
void CMMF_TSU_DEVSOUND_TestInterface1Mux::RunL()
	{
	TInt err = iStatus.Int();
	
	// complete the client request
	User::RequestComplete(iClientStatus, err);
	}
	
void CMMF_TSU_DEVSOUND_TestInterface1Mux::DoCancel()
	{
	// XXX - do we need to send a cancel message to the DeMux plugin?
	if (iClientStatus->Int() == KRequestPending)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}

// DEMUX //	
	

TInt CMMF_TSU_DEVSOUND_TestInterface1DeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}
	
void CMMF_TSU_DEVSOUND_TestInterface1DeMux::Release()
	{
	TUid key = iKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
void CMMF_TSU_DEVSOUND_TestInterface1DeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}
	
void CMMF_TSU_DEVSOUND_TestInterface1DeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}
	
void CMMF_TSU_DEVSOUND_TestInterface1DeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

void CMMF_TSU_DEVSOUND_TestInterface1DeMux::RefreshL()
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

void CMMF_TSU_DEVSOUND_TestInterface1DeMux::InterfaceDeleted()
	{
	}

MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMF_TSU_DEVSOUND_TestInterface1DeMux::NewL()
	{
	CMMF_TSU_DEVSOUND_TestInterface1DeMux* bob = new (ELeave) CMMF_TSU_DEVSOUND_TestInterface1DeMux;
	return bob;
	}
	
CMMF_TSU_DEVSOUND_TestInterface1DeMux::CMMF_TSU_DEVSOUND_TestInterface1DeMux()
	{
	
	
	}

CMMF_TSU_DEVSOUND_TestInterface1DeMux::~CMMF_TSU_DEVSOUND_TestInterface1DeMux()
	{

	}


TInt CMMF_TSU_DEVSOUND_TestInterface1DeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
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
	
void CMMF_TSU_DEVSOUND_TestInterface1DeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMF_TSU_DEVSOUND_TestInterface1DeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
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
	
TInt CMMF_TSU_DEVSOUND_TestInterface1DeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
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
	
void CMMF_TSU_DEVSOUND_TestInterface1DeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage)
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
	
void CMMF_TSU_DEVSOUND_TestInterface1DeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage)
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
	IMPLEMENTATION_PROXY_ENTRY(KMmfDevSoundTestCustomInterface1Mux,	CMMF_TSU_DEVSOUND_TestInterface1Mux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfDevSoundTestCustomInterface1DeMux,	CMMF_TSU_DEVSOUND_TestInterface1DeMux::NewL),
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


