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

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <s32mem.h>				

#include "devsoundciutestdevices.hrh"
#include "MMFDevSoundCIMuxDeMuxPluginImp.h"



// MUX //

TInt CMMFDevSoundCIMuxPluginImp::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KUidMmfDevSoundCustomInterfaceDeMuxPlugin};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFDevSoundCIMuxPluginImp::Release()
	{
	// close the slave device if it exists
	if (iRemoteHandle > 0)
		{
		// we assume the slave is closed correctly
		iUtility->CloseSlave(iRemoteHandle);
		}

	TUid key = iKey;
	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFDevSoundCIMuxPluginImp::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFDevSoundCIMuxPluginImp::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the the MMFDevSoundCustomMuxUtility class
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFDevSoundCIMuxPluginImp::NewL()
	{
	CMMFDevSoundCIMuxPluginImp* self = new (ELeave) CMMFDevSoundCIMuxPluginImp;
	return self;
	}


TAny* CMMFDevSoundCIMuxPluginImp::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFDevSoundCIMuxPluginInterface* interface = this;
	return interface;
	}


CMMFDevSoundCIMuxPluginImp::CMMFDevSoundCIMuxPluginImp()
	{
	}


CMMFDevSoundCIMuxPluginImp::~CMMFDevSoundCIMuxPluginImp()
	{
	}


TInt CMMFDevSoundCIMuxPluginImp::DevSoundCIMuxStopHeapFail()
	{
	TInt result = -1;

	if (iRemoteHandle > 0)
		{	
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIMuxDemuxStopHeapFail,
												KNullDesC8); 
		}
	return result;
	}


TInt CMMFDevSoundCIMuxPluginImp::DevSoundCIMuxCauseHeapFail(TInt aFailCount)
	{
	TInt result = -1;

	if (iRemoteHandle > 0)
		{
		TPckgBuf<TInt> failCount(aFailCount);  
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIMuxDemuxCauseHeapFail,
												failCount);  
		}
	return result;
	}

TInt CMMFDevSoundCIMuxPluginImp::DevSoundCIMuxCheckHeapFail()
	{
	TInt result = -1;
	TPckgBuf<TInt> responsePckg;
	
	if (iRemoteHandle > 0)
		{
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
												EMMFDevSoundCIMuxDemuxCheckHeapFail,
												KNullDesC8,
												responsePckg);  
		}
	
	User::LeaveIfError(result);
	return responsePckg();

	}

// DEMUX //

TInt CMMFDevSoundCIDemuxPluginImp::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFDevSoundCIDemuxPluginImp::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFDevSoundCIDemuxPluginImp::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFDevSoundCIDemuxPluginImp::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFDevSoundCIDemuxPluginImp::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the MMFDevSoundCustomDeMuxUtility class
	iUtility = aCustomUtility;
	}


void CMMFDevSoundCIDemuxPluginImp::RefreshL()
	{
	// Nothing to do in this implementation
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFDevSoundCIDemuxPluginImp::NewL()
	{
	CMMFDevSoundCIDemuxPluginImp* self = new (ELeave) CMMFDevSoundCIDemuxPluginImp;
	return self;
	}


CMMFDevSoundCIDemuxPluginImp::CMMFDevSoundCIDemuxPluginImp()
	{
	}


CMMFDevSoundCIDemuxPluginImp::~CMMFDevSoundCIDemuxPluginImp()
	{
	// Nothing to do in this implementation
	}


TInt CMMFDevSoundCIDemuxPluginImp::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// Nothing to do in this implementation
	return KErrNone;
	}


void CMMFDevSoundCIDemuxPluginImp::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// Nothing to do in this implementation
	}

TInt CMMFDevSoundCIDemuxPluginImp::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNotSupported;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIMuxDemuxCauseHeapFail:
			{
			result = KErrNone;
			TPckgBuf<TInt> failCount;				//Create an empty TPckgBuf<INT TYPE> to be empty
			iUtility->ReadFromInputDesL(aMessage, &failCount);  //readFromInputDesL treats this as a descriptor and populates it
			User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext, failCount());
			break;
			}
		case EMMFDevSoundCIMuxDemuxStopHeapFail:
			{
			result = KErrNone;
			User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext,NULL);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return result;
	}


TInt CMMFDevSoundCIDemuxPluginImp::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)  //This method returns an error to Client Side
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNotSupported;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data); 

	switch (data.iCommand)
		{
		case EMMFDevSoundCIMuxDemuxCheckHeapFail:
			{
			result = KErrNone;
			TPckgBuf<TBool> failCheck; 
			TAny *testAlloc = User::Alloc(1);	
			if ( testAlloc == NULL )
				{
				failCheck = EFalse;
				User::__DbgSetAllocFail(RHeap::EUser,RHeap::EFailNext,NULL);
				}
			else
				{
				failCheck = ETrue;
				User::Free(testAlloc);	
				}
			iUtility->WriteToOutputDesL(aMessage, failCheck);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return result;
	}


void CMMFDevSoundCIDemuxPluginImp::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFDevSoundCIDemuxPluginImp::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
