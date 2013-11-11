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

#include "aacdecoderconfigci.h"


// MUX //

TInt CMMFAacDecoderConfigMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceAacDecoderConfigDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFAacDecoderConfigMux::Release()
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


void CMMFAacDecoderConfigMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFAacDecoderConfigMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFAacDecoderConfigMux::NewL()
	{
	CMMFAacDecoderConfigMux* self = new (ELeave) CMMFAacDecoderConfigMux;
	return self;
	}


TAny* CMMFAacDecoderConfigMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MAacDecoderConfig* interface = this;
	return interface;
	}


CMMFAacDecoderConfigMux::CMMFAacDecoderConfigMux() :
iRemoteHandle(-1)
	{
	}


CMMFAacDecoderConfigMux::~CMMFAacDecoderConfigMux()
	{
	}


// from MAacDecoderConfig
TInt CMMFAacDecoderConfigMux::SetAudioConfig(TAudioConfig& aAudioConfig)
	{
	TInt result = -1;

	if (iRemoteHandle > 0)
		{
		// send the status in the sync command
		TPckgBuf<TAudioConfig> audioConfig(aAudioConfig);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIAacDecoderConfigSetAudioConfig,
												audioConfig);
		}

	return result;
	}


// from MAacDecoderConfig
TInt CMMFAacDecoderConfigMux::GetSupportedAudioConfigs(RArray<TAudioConfig>& aSupportedAudioConfigs)
	{
	TInt result = KErrNone;

	if (iRemoteHandle == -1)
		{
		return KErrBadHandle;
		}

	// first clear out the array
	aSupportedAudioConfigs.Reset();

	// now fetch the count from the server
	TInt count = -1;
	count = iUtility->SendSlaveSyncCommand(iRemoteHandle,
										   EMMFDevSoundCIAacDecoderConfigGetSupportedAudioConfig,
										   KNullDesC8);

	// if count is negative then the server side left with an error
	if (count < 0)
		{
		result = KErrNotReady;
		}

	// no point getting the data if the count is zero
	if ( (count != 0) && (result == KErrNone) )
		{
		// allocate a temporary buffer to hold the audio configuration
		HBufC8* buf = NULL;
		TRAP(result, buf = HBufC8::NewL(count * sizeof(TAudioConfig)));

		if (result != KErrNone)
			{
			return result;
			}

		TPtr8 ptr = buf->Des();

		// fetch the audio configurations - but send over the received count to be sure
		TPckgBuf<TInt> countBuf(count);
		iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
											 EMMFDevSoundCIAacDecoderConfigGetSupportedAudioConfigArray,
											 countBuf,
											 ptr);

		if (result != KErrNone)
			{
			return result;
			}

		// stream data into the pointer
		RDesReadStream stream(ptr);

		TInt err = KErrNone;
		for (TInt i = 0; i < count; i++)
			{
			TAudioConfig audioConfig;

			TRAP(result, audioConfig.iAudioObjectType = static_cast<TAudioConfig::TAudioObjectType>(stream.ReadInt32L());

			err = aSupportedAudioConfigs.Append(audioConfig));

			if ( (err != KErrNone) || (result != KErrNone) )
				{
				// note we don't destroy array because we don't own it
				// but we do reset it if it is incomplete
				aSupportedAudioConfigs.Reset();
				result = KErrCorrupt;
				}
			}

		stream.Close();
		stream.Release();
		delete buf;
		}

	return result;
	}



// DEMUX //

TInt CMMFAacDecoderConfigDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFAacDecoderConfigDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFAacDecoderConfigDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFAacDecoderConfigDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFAacDecoderConfigDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFAacDecoderConfigDeMux::RefreshL()
	{
	// refetch the aac decoder config custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceAacDecoderConfig = static_cast <MAacDecoderConfig*> (iTarget->CustomInterface(KUidAacDecoderConfig));

		if (!iInterfaceAacDecoderConfig)
			{
			iInterfaceAacDecoderConfig = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFAacDecoderConfigDeMux::NewL()
	{
	CMMFAacDecoderConfigDeMux* self = new (ELeave) CMMFAacDecoderConfigDeMux;
	return self;
	}


CMMFAacDecoderConfigDeMux::CMMFAacDecoderConfigDeMux()
	{
	}


CMMFAacDecoderConfigDeMux::~CMMFAacDecoderConfigDeMux()
	{
	iSupportedAudioConfigs.Reset();
	iSupportedAudioConfigs.Close();
	}


TInt CMMFAacDecoderConfigDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Aac Decoder Config Hw Device custom interface
	iInterfaceAacDecoderConfig = static_cast<MAacDecoderConfig*> (iTarget->CustomInterface(KUidAacDecoderConfig)); 

	if (!iInterfaceAacDecoderConfig)
		{
		iInterfaceAacDecoderConfig = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFAacDecoderConfigDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFAacDecoderConfigDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNotSupported;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIAacDecoderConfigSetAudioConfig:
			{
			TPckgBuf<MAacDecoderConfig::TAudioConfig> audioConfig; 
			iUtility->ReadFromInputDesL(aMessage, &audioConfig);

			result = DoSetAudioConfigL(audioConfig());

			break;
			}
		case EMMFDevSoundCIAacDecoderConfigGetSupportedAudioConfig:
			{
			// reset the current AudioConfig array
			iSupportedAudioConfigs.Reset();
			result = DoGetSupportedAudioConfigsL(iSupportedAudioConfigs);

			// send back the array count
			TInt count = iSupportedAudioConfigs.Count();
			result = count;

			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


TInt CMMFAacDecoderConfigDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNotSupported;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIAacDecoderConfigGetSupportedAudioConfigArray:
			{

			DoCopyAudioConfigsBufferToClientL(aMessage);

			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


void CMMFAacDecoderConfigDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFAacDecoderConfigDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// Aac Decoder Config custom interface implementation
TInt CMMFAacDecoderConfigDeMux::DoSetAudioConfigL(MAacDecoderConfig::TAudioConfig& aAudioConfig)
	{
	TInt result = KErrNotFound;

	if (iInterfaceAacDecoderConfig)
		{
		result = iInterfaceAacDecoderConfig->SetAudioConfig(aAudioConfig);
		}

	return result;
	}


// Aac Decoder Config custom interface implementation
TInt CMMFAacDecoderConfigDeMux::DoGetSupportedAudioConfigsL(RArray<MAacDecoderConfig::TAudioConfig>& aSupportedAudioConfigs)
	{
	TInt result = KErrNotFound;

	if (iInterfaceAacDecoderConfig)
		{
		result = iInterfaceAacDecoderConfig->GetSupportedAudioConfigs(aSupportedAudioConfigs);
		}

	return result;
	}


// Aac Decoder Config custom interface implementation
void CMMFAacDecoderConfigDeMux::DoCopyAudioConfigsBufferToClientL(const RMmfIpcMessage& aMessage)
	{
	if (!iInterfaceAacDecoderConfig)
		{
		User::Leave(KErrNotReady);
		}

	// check our count is the same as the client's
	TPckgBuf<TInt> countBuffer;
	iUtility->ReadFromInputDesL(aMessage, &countBuffer);

	TInt count = countBuffer();
	if (count != iSupportedAudioConfigs.Count())
		{
		User::Leave(KErrCorrupt);
		}

	// send back the array - the client has the count already
	const TInt KBufExpandSize8 = 8; //two TInt's
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);

	for (TInt i = 0; i < count; i++)
		{
		stream.WriteInt32L(iSupportedAudioConfigs[i].iAudioObjectType);
		}

	// write the data to the supplied descriptor buffer
	TPtr8 ptrBuf = dataCopyBuffer->Ptr(0);
	iUtility->WriteToOutputDesL(aMessage, ptrBuf);
	stream.Close();

	CleanupStack::PopAndDestroy(2, dataCopyBuffer); // dataCopyBuffer, stream
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceAacDecoderConfigMux,	CMMFAacDecoderConfigMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceAacDecoderConfigDeMux,	CMMFAacDecoderConfigDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
