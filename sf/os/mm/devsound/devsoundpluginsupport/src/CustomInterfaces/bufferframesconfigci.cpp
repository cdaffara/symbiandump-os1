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
#include "bufferframesconfigci.h"

// Helper class to pass data over process boundary
class TFrameInfo
	{
public:
	TInt iFrameCount;
	TInt iSamplesPerFrame;	
	};

// MUX //
/*****************************************************************************/

TInt CMMFBufferFramesConfigMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceBufferFramesConfigDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

/*****************************************************************************/
void CMMFBufferFramesConfigMux::Release()
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
void CMMFBufferFramesConfigMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}

/*****************************************************************************/
void CMMFBufferFramesConfigMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFBufferFramesConfigMux::NewL()
	{
	CMMFBufferFramesConfigMux* self = new (ELeave) CMMFBufferFramesConfigMux;
	return self;
	}

/*****************************************************************************/	
TAny* CMMFBufferFramesConfigMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFBufferFramesConfig* interface = this;
	return interface;
	}
	
/*****************************************************************************/
CMMFBufferFramesConfigMux::CMMFBufferFramesConfigMux() :
	iRemoteHandle(-1)
	{	
	}

/*****************************************************************************/
CMMFBufferFramesConfigMux::~CMMFBufferFramesConfigMux()
	{	
	}

/*****************************************************************************/
// from MMMFBufferFramesConfig
TInt CMMFBufferFramesConfigMux::MmbfcSetNumberOfFramesPerInputBuffer(TInt aFrameCount, TInt aSamplesPerFrame)
	{
	if (iRemoteHandle > 0)
		{
		TFrameInfo info;
		info.iFrameCount = aFrameCount;
		info.iSamplesPerFrame = aSamplesPerFrame;
	
		TPckgBuf<TFrameInfo> pckgBuf(info);
		// send the frame information in the sync command
		// any return code other than zero is an error
		return iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCIInputBufferFramesConfig, 
												pckgBuf);
		}
	else
		{
		return KErrNotReady;
		}
	}
	
TInt CMMFBufferFramesConfigMux::MmbfcSetNumberOfFramesPerOutputBuffer(TInt aFrameCount, TInt aSamplesPerFrame)
	{
	if (iRemoteHandle > 0)
		{
		TFrameInfo info;
		info.iFrameCount = aFrameCount;
		info.iSamplesPerFrame = aSamplesPerFrame;
		
		TPckgBuf<TFrameInfo> pckgBuf(info);
		// send the frame information in the sync command
		// any return code other than zero is an error
		return iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCIOutputBufferFramesConfig, 
												pckgBuf);
		}
	else
		{
		return KErrNotReady;
		}
	}



// DEMUX //	
/*****************************************************************************/
TInt CMMFBufferFramesConfigDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}

/*****************************************************************************/	
void CMMFBufferFramesConfigDeMux::Release()
	{
	TUid key = iDestructorKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
/*****************************************************************************/	
void CMMFBufferFramesConfigDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}
	
/*****************************************************************************/	
void CMMFBufferFramesConfigDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}

/*****************************************************************************/	
void CMMFBufferFramesConfigDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/
void CMMFBufferFramesConfigDeMux::RefreshL()
	{
	// refetch the buffer frames configuration custom interface if we already have a target
	if (iTarget)
		{
		MMMFBufferFramesConfig* ptr = NULL;

		ptr = static_cast <MMMFBufferFramesConfig*> (iTarget->CustomInterface(KUidBufferFramesConfig));
	
		if (!ptr)
			{
			iInterfaceBufferFramesConfig = NULL;
			User::Leave(KErrNotSupported);
			}
		else
			{
			iInterfaceBufferFramesConfig = ptr;
			}	
		}
	}

/*****************************************************************************/
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFBufferFramesConfigDeMux::NewL()
	{
	CMMFBufferFramesConfigDeMux* self = new (ELeave) CMMFBufferFramesConfigDeMux;
	return self;
	}

/*****************************************************************************/	
CMMFBufferFramesConfigDeMux::CMMFBufferFramesConfigDeMux()
	{	
	}

/*****************************************************************************/
CMMFBufferFramesConfigDeMux::~CMMFBufferFramesConfigDeMux()
	{
	}

/*****************************************************************************/
TInt CMMFBufferFramesConfigDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the buffer frames configuration Hw Device custom interface
	MMMFBufferFramesConfig* ptr = NULL;

	ptr = static_cast<MMMFBufferFramesConfig*> (iTarget->CustomInterface(KUidBufferFramesConfig)); 
	
	if (!ptr)
		{
		iInterfaceBufferFramesConfig = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iInterfaceBufferFramesConfig = ptr;
		}
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFBufferFramesConfigDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

/*****************************************************************************/
// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFBufferFramesConfigDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;

	TInt retVal = KErrNone;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIInputBufferFramesConfig:
			{
			TPckgBuf<TFrameInfo> info; 
			iUtility->ReadFromInputDesL(aMessage, &info);
			
			retVal = DoMmbfcSetNumberOfFramesPerInputBuffer(info().iFrameCount, info().iSamplesPerFrame);
			break;
			}
		case EMMFDevSoundCIOutputBufferFramesConfig:
			{
			TPckgBuf<TFrameInfo> info; 
			iUtility->ReadFromInputDesL(aMessage, &info);
			retVal = DoMmbfcSetNumberOfFramesPerOutputBuffer(info().iFrameCount, info().iSamplesPerFrame);
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
TInt CMMFBufferFramesConfigDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFBufferFramesConfigDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
/*****************************************************************************/	
void CMMFBufferFramesConfigDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

/*****************************************************************************/
// Buffer frames config custom interface implementation
TInt CMMFBufferFramesConfigDeMux::DoMmbfcSetNumberOfFramesPerInputBuffer(TInt aFrameCount,
																		TInt aSamplesPerFrame)
	{
	if (!iInterfaceBufferFramesConfig)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceBufferFramesConfig->MmbfcSetNumberOfFramesPerInputBuffer(aFrameCount,
																					aSamplesPerFrame);
		}		
	}

TInt CMMFBufferFramesConfigDeMux::DoMmbfcSetNumberOfFramesPerOutputBuffer(TInt aFrameCount,
																			TInt aSamplesPerFrame)
	{
	if (!iInterfaceBufferFramesConfig)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceBufferFramesConfig->MmbfcSetNumberOfFramesPerOutputBuffer(aFrameCount,
																					aSamplesPerFrame);
		}		
	}

/*****************************************************************************/
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceBufferFramesConfigMux, CMMFBufferFramesConfigMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceBufferFramesConfigDeMux, CMMFBufferFramesConfigDeMux::NewL),
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


