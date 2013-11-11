// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <s32mem.h>

#include "audiobufferprefillci.h"


// MUX //

TInt CMMFSampleBufferingMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceSampleBufferingDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFSampleBufferingMux::Release()
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


void CMMFSampleBufferingMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFSampleBufferingMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFSampleBufferingMux::NewL()
	{
	CMMFSampleBufferingMux* self = new (ELeave) CMMFSampleBufferingMux;
	return self;
	}


TAny* CMMFSampleBufferingMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFSampleBuffering* interface = this;
	return interface;
	}


CMMFSampleBufferingMux::CMMFSampleBufferingMux() :
iRemoteHandle(-1) 
	{
	}


CMMFSampleBufferingMux::~CMMFSampleBufferingMux()
	{
	}


// from MMMFSampleBuffering
TInt CMMFSampleBufferingMux::MmsbEnableSampleBufferingBeforePlayback()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIEnableSampleBufferingBeforePlayback,
												KNullDesC8);
		}

	return result;
	}


// from MMMFSampleBuffering
TInt CMMFSampleBufferingMux::MmsbDisableSampleBufferingBeforePlayback()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIDisableSampleBufferingBeforePlayback,
												KNullDesC8);
		}

	return result;
	}


// from MMMFSampleBuffering
void CMMFSampleBufferingMux::MmsbNotifyPlayStarted(TRequestStatus& aStatus)
	{
	if (iRemoteHandle > 0)
		{
		// package the handle and command, and send in the Async command
		iCmdPkg().iHandle = iRemoteHandle;
		iCmdPkg().iCommand = EMMFDevSoundCINotifyPlayStarted;

		// any return code other than zero is an error
		iUtility->SendSlaveAsyncCommand(iCmdPkg, aStatus, KNullDesC8);
		}
	}


// from MMMFSampleBuffering
void CMMFSampleBufferingMux::MmsbCancelNotifyPlayStarted()
	{
	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		TInt result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
													 EMMFDevSoundCICancelNotifyPlayStarted,
													 KNullDesC8);
		}
	}



// DEMUX //	

TInt CMMFSampleBufferingDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFSampleBufferingDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFSampleBufferingDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFSampleBufferingDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFSampleBufferingDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFSampleBufferingDeMux::RefreshL()
	{
	// refetch the Sample Buffering  custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceSampleBuffering = static_cast <MMMFSampleBuffering*> (iTarget->CustomInterface(KUidSampleBuffering ));

		if (!iInterfaceSampleBuffering)
			{
			iInterfaceSampleBuffering = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFSampleBufferingDeMux::NewL()
	{
	CMMFSampleBufferingDeMux* self = new (ELeave) CMMFSampleBufferingDeMux;
	return self;
	}


CMMFSampleBufferingDeMux::CMMFSampleBufferingDeMux() : CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}


CMMFSampleBufferingDeMux::~CMMFSampleBufferingDeMux()
	{
	Cancel();
	}


TInt CMMFSampleBufferingDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Sample Buffering  Hw Device custom interface
	iInterfaceSampleBuffering = static_cast<MMMFSampleBuffering*> (iTarget->CustomInterface(KUidSampleBuffering)); 

	if (!iInterfaceSampleBuffering)
		{
		iInterfaceSampleBuffering = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFSampleBufferingDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFSampleBufferingDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIEnableSampleBufferingBeforePlayback:
			{
			result = DoMmsbEnableSampleBufferingBeforePlaybackL();
			break;
			}
		case EMMFDevSoundCIDisableSampleBufferingBeforePlayback:
			{
			result = DoMmsbDisableSampleBufferingBeforePlaybackL();
			break;
			}
		case EMMFDevSoundCICancelNotifyPlayStarted:
			{
			DoMmsbCancelNotifyPlayStartedL();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


TInt CMMFSampleBufferingDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	return KErrNone;
	}


void CMMFSampleBufferingDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;

	// decode message
	iUtility->GetAsyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCINotifyPlayStarted:
			{
			DoMmsbNotifyPlayStartedL(aMessage);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}


void CMMFSampleBufferingDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// Sample Buffering  custom interface implementation
TInt CMMFSampleBufferingDeMux::DoMmsbEnableSampleBufferingBeforePlaybackL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceSampleBuffering)
		{
		result = iInterfaceSampleBuffering->MmsbEnableSampleBufferingBeforePlayback();
		}

	return result;
	}


// Sample Buffering custom interface implementation
TInt CMMFSampleBufferingDeMux::DoMmsbDisableSampleBufferingBeforePlaybackL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceSampleBuffering)
		{
		result = iInterfaceSampleBuffering->MmsbDisableSampleBufferingBeforePlayback();
		}

	return result;
	}


// Sample Buffering  custom interface implementation
void CMMFSampleBufferingDeMux::DoMmsbNotifyPlayStartedL(const RMmfIpcMessage& aMessage)
	{
	if (iInterfaceSampleBuffering)
		{
		// make a copy of the received message before jumping to the plugin.
		// It will be used on the reply to the client.
		iStoredMessage = aMessage;

		iInterfaceSampleBuffering->MmsbNotifyPlayStarted(iStatus);

		// check not already active
		ASSERT(!IsActive());

		SetActive();
		}
	}


// Sample Buffering custom interface implementation
void CMMFSampleBufferingDeMux::DoMmsbCancelNotifyPlayStartedL()
	{
	if (iInterfaceSampleBuffering)
		{
		iInterfaceSampleBuffering->MmsbCancelNotifyPlayStarted();
		}
	}


// active object handling functions
void CMMFSampleBufferingDeMux::RunL()
	{
	TInt err = iStatus.Int();

	// complete the client request
	iStoredMessage.Complete(err);
	}


void CMMFSampleBufferingDeMux::DoCancel()
	{
	}

//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSampleBufferingMux,	CMMFSampleBufferingMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSampleBufferingDeMux,	CMMFSampleBufferingDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

