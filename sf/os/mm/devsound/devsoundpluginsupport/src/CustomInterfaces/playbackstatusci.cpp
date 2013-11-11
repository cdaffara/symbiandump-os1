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
#include "playbackstatusci.h"


// MUX //
/*****************************************************************************/

TInt CMMFPlayBackStatusMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfacePlayBackStatusDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

/*****************************************************************************/
void CMMFPlayBackStatusMux::Release()
	{
	// close the slave device if it exists
	if (iRemoteHandle > 0)
		{
		// we assume the slave is closed correctly
		iUtility->CloseSlave(iRemoteHandle);
		}
	
	TUid key = iDestructorKey;
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}

/*****************************************************************************/	
void CMMFPlayBackStatusMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}

/*****************************************************************************/
void CMMFPlayBackStatusMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFPlayBackStatusMux::NewL()
	{
	CMMFPlayBackStatusMux* self = new (ELeave) CMMFPlayBackStatusMux;
	return self;
	}

/*****************************************************************************/	
TAny* CMMFPlayBackStatusMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFPlaybackStatus* interface = this;
	return interface;
	}
	
/*****************************************************************************/
CMMFPlayBackStatusMux::CMMFPlayBackStatusMux() :
	iRemoteHandle(-1)
	{	
	}

/*****************************************************************************/
CMMFPlayBackStatusMux::~CMMFPlayBackStatusMux()
	{	
	}

/*****************************************************************************/
// from MMMFPlaybackStatus
TInt CMMFPlayBackStatusMux::MmpsGetPlaybackStatusInformation(TMMFPlaybackStatus& aStatus)
	{
	TInt err = KErrNone;
	if (iRemoteHandle > 0)
		{
		// send the status in the sync command
		TPckgBuf<MMMFPlaybackStatus::TMMFPlaybackStatus> status(aStatus);
		
		TPckgBuf<MMMFPlaybackStatus::TMMFPlaybackStatus> retStatus;
		
		// any return code other than zero is an error
		err = iUtility->SendSlaveSyncCommandResult(iRemoteHandle, 
													EMMFDevSoundCIPlayBackStatus, 
													status,
													retStatus);
		// assign return values to aStatus. Do nothing if there is an error
		if(err == KErrNone)
			{
			aStatus = retStatus();
			}
		}
	return err;
	}

TInt CMMFPlayBackStatusMux::MmpsRequestLossOfSyncNotification()
	{
	TInt err = KErrNone;
	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		err = iUtility->SendSlaveSyncCommand(iRemoteHandle, 
												EMMFDevSoundCIPlayBackStatusReqSyncLossNotification, 
												KNullDesC8);
		}
	return err;
	}
	
TInt CMMFPlayBackStatusMux::MmpsCancelLossOfSyncNotification()
	{
	TInt err = KErrNone;
	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		err = iUtility->SendSlaveSyncCommand(iRemoteHandle, 
												EMMFDevSoundCIPlayBackStatusCancelSyncLossNotification, 
												KNullDesC8);
		}
	return err;
	}


// DEMUX //	
/*****************************************************************************/
TInt CMMFPlayBackStatusDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}

/*****************************************************************************/	
void CMMFPlayBackStatusDeMux::Release()
	{
	TUid key = iDestructorKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
/*****************************************************************************/	
void CMMFPlayBackStatusDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}
	
/*****************************************************************************/	
void CMMFPlayBackStatusDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}

/*****************************************************************************/	
void CMMFPlayBackStatusDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/
void CMMFPlayBackStatusDeMux::RefreshL()
	{
	// refetch the play back status custom interface if we already have a target
	if (iTarget)
		{
		iInterfacePlayBackStatus = static_cast<MMMFPlaybackStatus*>(iTarget->
																	CustomInterface(KUidPlaybackStatus));
	
		if (!iInterfacePlayBackStatus)
			{
			User::Leave(KErrNotSupported);
			}
		}
	}
	
/*****************************************************************************/
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFPlayBackStatusDeMux::NewL()
	{
	CMMFPlayBackStatusDeMux* self = new (ELeave) CMMFPlayBackStatusDeMux;
	return self;
	}

/*****************************************************************************/	
CMMFPlayBackStatusDeMux::CMMFPlayBackStatusDeMux()
	{	
	}

/*****************************************************************************/
CMMFPlayBackStatusDeMux::~CMMFPlayBackStatusDeMux()
	{
	}

/*****************************************************************************/
TInt CMMFPlayBackStatusDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Playback status Hw Device custom interface
	MMMFPlaybackStatus* ptr = NULL;

	ptr = static_cast<MMMFPlaybackStatus*> (iTarget->CustomInterface(KUidPlaybackStatus)); 
	
	if (!ptr)
		{
		iInterfacePlayBackStatus = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iInterfacePlayBackStatus = ptr;
		}
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFPlayBackStatusDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

/*****************************************************************************/
// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFPlayBackStatusDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt err = KErrNone;
	switch(data.iCommand)
		{
		case EMMFDevSoundCIPlayBackStatusReqSyncLossNotification:
			{
			err = DoMmpsRequestLossOfSyncNotification();
			break;
			}
		case EMMFDevSoundCIPlayBackStatusCancelSyncLossNotification:
			{
			err = DoMmpsCancelLossOfSyncNotification();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return err;
	}
	
/*****************************************************************************/	
TInt CMMFPlayBackStatusDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIPlayBackStatus:
			{
			TPckgBuf<MMMFPlaybackStatus::TMMFPlaybackStatus> status; 
			iUtility->ReadFromInputDesL(aMessage, &status);

			DoMmpsGetPlaybackStatusInformation(status());
			
			TPckgBuf<MMMFPlaybackStatus::TMMFPlaybackStatus> des(status());
			iUtility->WriteToOutputDesL(aMessage, des);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFPlayBackStatusDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
/*****************************************************************************/	
void CMMFPlayBackStatusDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

/*****************************************************************************/
// Play Back Status custom interface implementation
TInt CMMFPlayBackStatusDeMux::DoMmpsGetPlaybackStatusInformation(MMMFPlaybackStatus::TMMFPlaybackStatus& aStatus)
	{
	TInt err = KErrNotReady;
	if (iInterfacePlayBackStatus)
		{
		err = iInterfacePlayBackStatus->MmpsGetPlaybackStatusInformation(aStatus);
		}
	return err;	
	}

TInt CMMFPlayBackStatusDeMux::DoMmpsRequestLossOfSyncNotification()
	{
	TInt err = KErrNotReady;
	if (iInterfacePlayBackStatus)
		{
		err = iInterfacePlayBackStatus->MmpsRequestLossOfSyncNotification();
		}
	return err;
	}
	
TInt CMMFPlayBackStatusDeMux::DoMmpsCancelLossOfSyncNotification()
	{
	TInt err = KErrNotReady;
	if (iInterfacePlayBackStatus)
		{
		err = iInterfacePlayBackStatus->MmpsCancelLossOfSyncNotification();
		}
	return err;
	}

/*****************************************************************************/
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfacePlayBackStatusMux,	CMMFPlayBackStatusMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfacePlayBackStatusDeMux, CMMFPlayBackStatusDeMux::NewL),
	};

/*****************************************************************************/
//
// ImplementationGroupProxy
//
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


