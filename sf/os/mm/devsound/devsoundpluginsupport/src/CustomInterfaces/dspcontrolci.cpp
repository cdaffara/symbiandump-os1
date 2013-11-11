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

#include "dspcontrolci.h"


// MUX //

TInt CMMFDspControlMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceDspControlDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFDspControlMux::Release()
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


void CMMFDspControlMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFDspControlMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFDspControlMux::NewL()
	{
	CMMFDspControlMux* self = new (ELeave) CMMFDspControlMux;
	return self;
	}


TAny* CMMFDspControlMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFDSPControl* interface = this;
	return interface;
	}


CMMFDspControlMux::CMMFDspControlMux() :
	iRemoteHandle(-1)
	{
	}


CMMFDspControlMux::~CMMFDspControlMux()
	{
	}


// from MMMFDSPControl
TInt CMMFDspControlMux::MmdspcGetAudioPlaybackInfo (TInt64& aSamplesPlayed,
													TInt64& aSystemTime,
													TUint& aBytesDecoderConsumed,
													TUint& aBytesDecoderDecoded)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// holds the returned value
		TPckgBuf<TAudioPlaybackInfo> returnVal;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIDspCtrlGetAudioPlaybackInfo,
													  KNullDesC8,
													  returnVal);

		// assign returned value to the individual parameter. Do nothing if there is an error
		if(result == KErrNone)
			{
			aSamplesPlayed = returnVal().iSamplesPlayed;
			aSystemTime = returnVal().iSystemTime;
			aBytesDecoderConsumed = returnVal().iBytesDecoderConsumed;
			aBytesDecoderDecoded = returnVal().iBytesDecoderDecoded;
			}
		}

	return result;
	}


// from MMMFDSPControl
TInt CMMFDspControlMux::MmdspcAcceptRecordBuffersInvalidFollowingStop()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIDspCtrlAcceptRecordBuffersInvalidFollowingStop,
												KNullDesC8);
		}

	return result;
	}


// from MMMFDSPControl
TInt CMMFDspControlMux::MmdspcAcceptPlaybackBuffersInvalidFollowingStop()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIDspCtrlAcceptPlaybackBuffersInvalidFollowingStop,
												KNullDesC8);
		}

	return result;
	}



// DEMUX //	

TInt CMMFDspControlDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFDspControlDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFDspControlDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFDspControlDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFDspControlDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFDspControlDeMux::RefreshL()
	{
	// refetch the DSP Control  custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceDspControl = static_cast <MMMFDSPControl*> (iTarget->CustomInterface(KUidDSPControl));

		if (!iInterfaceDspControl)
			{
			iInterfaceDspControl = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFDspControlDeMux::NewL()
	{
	CMMFDspControlDeMux* self = new (ELeave) CMMFDspControlDeMux;
	return self;
	}


CMMFDspControlDeMux::CMMFDspControlDeMux()
	{
	}


CMMFDspControlDeMux::~CMMFDspControlDeMux()
	{
	}


TInt CMMFDspControlDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the DSP Control  Hw Device custom interface
	iInterfaceDspControl = static_cast<MMMFDSPControl*> (iTarget->CustomInterface(KUidDSPControl)); 

	if (!iInterfaceDspControl)
		{
		iInterfaceDspControl = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFDspControlDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFDspControlDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIDspCtrlAcceptRecordBuffersInvalidFollowingStop:
			{
			result = DoMmdspcAcceptRecordBuffersInvalidFollowingStopL();

			break;
			}
		case EMMFDevSoundCIDspCtrlAcceptPlaybackBuffersInvalidFollowingStop:
			{
			result = DoMmdspcAcceptPlaybackBuffersInvalidFollowingStopL();

			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFDspControlDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIDspCtrlGetAudioPlaybackInfo:
			{
			TPckgBuf<TAudioPlaybackInfo> pckData;

			iUtility->ReadFromInputDesL(aMessage, &pckData);

			result = DoMmdspcGetAudioPlaybackInfoL(pckData().iSamplesPlayed,
												   pckData().iSystemTime,
												   pckData().iBytesDecoderConsumed,
												   pckData().iBytesDecoderDecoded);

			TPckgBuf<TAudioPlaybackInfo> des(pckData());
			iUtility->WriteToOutputDesL(aMessage, des);

			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


void CMMFDspControlDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFDspControlDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


TInt CMMFDspControlDeMux::DoMmdspcAcceptRecordBuffersInvalidFollowingStopL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceDspControl)
		{
		result = iInterfaceDspControl->MmdspcAcceptRecordBuffersInvalidFollowingStop();
		}

	return result;
	}


TInt CMMFDspControlDeMux::DoMmdspcAcceptPlaybackBuffersInvalidFollowingStopL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceDspControl)
		{
		result = iInterfaceDspControl->MmdspcAcceptPlaybackBuffersInvalidFollowingStop();
		}

	return result;
	}


TInt CMMFDspControlDeMux::DoMmdspcGetAudioPlaybackInfoL(TInt64& aSamplesPlayed,
														TInt64& aSystemTime,
														TUint& aBytesDecoderConsumed,
														TUint& aBytesDecoderDecoded)
	{
	TInt result = KErrNotFound;

	if (iInterfaceDspControl)
		{
		result = iInterfaceDspControl->MmdspcGetAudioPlaybackInfo(aSamplesPlayed,
																  aSystemTime,
																  aBytesDecoderConsumed,
																  aBytesDecoderDecoded);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceDspControlMux,		CMMFDspControlMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceDspControlDeMux,	CMMFDspControlDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

