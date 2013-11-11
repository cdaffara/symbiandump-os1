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

#include "gettimestampsci.h"

class TPositionTimestamp
	{
public:
	TTimeIntervalMicroSeconds iBufferPosition;
	TTime iTimestamp;
	};

// MUX //
/*****************************************************************************/

TInt CMMFGetTimestampsMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceGetTimestampsDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

/*****************************************************************************/
void CMMFGetTimestampsMux::Release()
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
void CMMFGetTimestampsMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}

/*****************************************************************************/
void CMMFGetTimestampsMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFGetTimestampsMux::NewL()
	{
	CMMFGetTimestampsMux* self = new (ELeave) CMMFGetTimestampsMux;
	return self;
	}

/*****************************************************************************/	
TAny* CMMFGetTimestampsMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFGetTimestamps* interface = this;
	return interface;
	}
	
/*****************************************************************************/
CMMFGetTimestampsMux::CMMFGetTimestampsMux() :
	iRemoteHandle(-1)
	{	
	}

/*****************************************************************************/
CMMFGetTimestampsMux::~CMMFGetTimestampsMux()
	{	
	}

/*****************************************************************************/
// from MMMFGetTimestamps
TInt CMMFGetTimestampsMux::MmgtSetRecordSystemTimestampsEnabled(TBool aEnable)
	{
	if (iRemoteHandle > 0)
		{
		// send the enable flag in the sync command
		TPckgBuf<TBool> flag(aEnable);

		// any return code other than zero is an error
		return iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCIGetTimestampsEnable, flag);
		}
	else
		{
		return KErrNotReady;
		}
	}

TInt CMMFGetTimestampsMux::MmgtGetSystemTimestampForBuffer(const TTimeIntervalMicroSeconds& aBufferPosition,
															TTime& aTimestamp) const
	{
	TInt err = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		// send the position and timestamp in the sync command
		TPositionTimestamp value;
		value.iBufferPosition = aBufferPosition;
		value.iTimestamp = aTimestamp;
		TPckgBuf<TPositionTimestamp> pckgBuffer(value);

		// any return code other than zero is an error
		err = iUtility->SendSlaveSyncCommandResult(iRemoteHandle, EMMFDevSoundCIGetTimestampsForBuffer, 
													pckgBuffer, pckgBuffer);
		aTimestamp = pckgBuffer().iTimestamp;
		}

	return err;
	}



// DEMUX //	
/*****************************************************************************/
TInt CMMFGetTimestampsDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}

/*****************************************************************************/	
void CMMFGetTimestampsDeMux::Release()
	{
	TUid key = iDestructorKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
/*****************************************************************************/	
void CMMFGetTimestampsDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}
	
/*****************************************************************************/	
void CMMFGetTimestampsDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}

/*****************************************************************************/	
void CMMFGetTimestampsDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/
void CMMFGetTimestampsDeMux::RefreshL()
	{
	// refetch the play back status custom interface if we already have a target
	if (iTarget)
		{
		MMMFGetTimestamps* ptr = NULL;

		ptr = static_cast <MMMFGetTimestamps*> (iTarget->CustomInterface(KUidGetTimestamps));
	
		if (!ptr)
			{
			iInterfaceGetTimestamps = NULL;
			User::Leave(KErrNotSupported);
			}
		else
			{
			iInterfaceGetTimestamps = ptr;
			}	
		}
	}

/*****************************************************************************/
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFGetTimestampsDeMux::NewL()
	{
	CMMFGetTimestampsDeMux* self = new (ELeave) CMMFGetTimestampsDeMux;
	return self;
	}

/*****************************************************************************/	
CMMFGetTimestampsDeMux::CMMFGetTimestampsDeMux()
	{	
	}

/*****************************************************************************/
CMMFGetTimestampsDeMux::~CMMFGetTimestampsDeMux()
	{
	}

/*****************************************************************************/
TInt CMMFGetTimestampsDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Playback status Hw Device custom interface
	MMMFGetTimestamps* ptr = NULL;

	ptr = static_cast<MMMFGetTimestamps*> (iTarget->CustomInterface(KUidGetTimestamps)); 
	
	if (!ptr)
		{
		iInterfaceGetTimestamps = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iInterfaceGetTimestamps = ptr;
		}
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFGetTimestampsDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

/*****************************************************************************/
// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFGetTimestampsDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;

	TInt retVal = KErrNone;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIGetTimestampsEnable:
			{
			TPckgBuf<TBool> flag; 
			iUtility->ReadFromInputDesL(aMessage, &flag);
			retVal = DoMmgtSetRecordSystemTimestampsEnabled(flag());
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return retVal;
	}
	
/*****************************************************************************/	
TInt CMMFGetTimestampsDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;

	TInt retVal = KErrNone;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIGetTimestampsForBuffer:
			{
			TPckgBuf<TPositionTimestamp> value; 
			iUtility->ReadFromInputDesL(aMessage, &value);
			retVal = DoMmgtGetSystemTimestampForBuffer(value().iBufferPosition, value().iTimestamp);
			iUtility->WriteToOutputDesL(aMessage, value);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return retVal;
	}
	
/*****************************************************************************/	
void CMMFGetTimestampsDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
/*****************************************************************************/	
void CMMFGetTimestampsDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

/*****************************************************************************/
// Get Timestamp custom interface implementation
TInt CMMFGetTimestampsDeMux::DoMmgtSetRecordSystemTimestampsEnabled(TBool aEnable)
	{
	if (!iInterfaceGetTimestamps)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceGetTimestamps->MmgtSetRecordSystemTimestampsEnabled(aEnable);
		}	
	}
		
TInt CMMFGetTimestampsDeMux::DoMmgtGetSystemTimestampForBuffer(const TTimeIntervalMicroSeconds& aBufferPosition,
																TTime& aTimestamp)
	{
	if (!iInterfaceGetTimestamps)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceGetTimestamps->MmgtGetSystemTimestampForBuffer(aBufferPosition, aTimestamp);
		}
	}
/*****************************************************************************/
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceGetTimestampsMux, CMMFGetTimestampsMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceGetTimestampsDeMux, CMMFGetTimestampsDeMux::NewL),
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


