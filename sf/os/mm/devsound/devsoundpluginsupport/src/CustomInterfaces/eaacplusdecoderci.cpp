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

#include "eaacplusdecoderci.h"


// MUX //
/*****************************************************************************/

TInt CMMFEAacPlusDecoderMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceEAacPlusDecoderDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

/*****************************************************************************/
void CMMFEAacPlusDecoderMux::Release()
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
void CMMFEAacPlusDecoderMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}

/*****************************************************************************/
void CMMFEAacPlusDecoderMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFEAacPlusDecoderMux::NewL()
	{
	CMMFEAacPlusDecoderMux* self = new (ELeave) CMMFEAacPlusDecoderMux;
	return self;
	}

/*****************************************************************************/	
TAny* CMMFEAacPlusDecoderMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MEAacPlusDecoderIntfc* interface = this;
	return interface;
	}
	
/*****************************************************************************/
CMMFEAacPlusDecoderMux::CMMFEAacPlusDecoderMux() :
	iRemoteHandle(-1)
	{	
	}

/*****************************************************************************/
CMMFEAacPlusDecoderMux::~CMMFEAacPlusDecoderMux()
	{	
	}

/*****************************************************************************/
// from MEAacPlusDecoderIntfc
void CMMFEAacPlusDecoderMux::SetInputSamplingFrequency(TUint aInputSamplingFrequency)
	{
	if (iRemoteHandle > 0)
		{
		// send the frequency in the sync command
		TPckgBuf<TUint> freqBuffer(aInputSamplingFrequency);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCIEAacPlusDecoderSetInputSamplingFrequency,
										freqBuffer);
		}
	}

void CMMFEAacPlusDecoderMux::SetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType aAudioObjectType)
	{
	if (iRemoteHandle > 0)
		{
		// send the object type in the sync command
		TPckgBuf<MEAacPlusDecoderIntfc::TAudioObjectType> objTypeBuffer(aAudioObjectType);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCIEAacPlusDecoderSetAudioObjectType,
										objTypeBuffer);
		}
	}

void CMMFEAacPlusDecoderMux::SetNumOfChannels(TUint aNumOfChannels)
	{
	if (iRemoteHandle > 0)
		{
		// send the number of channels in the sync command
		TPckgBuf<TUint> numBuffer(aNumOfChannels);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCIEAacPlusDecoderSetNumOfChannels,
										numBuffer);
		}
	}

void CMMFEAacPlusDecoderMux::SetSbr(TBool aSbrEnabled)
	{
	if (iRemoteHandle > 0)
		{
		// send the SBR enabled flag in the sync command
		TPckgBuf<TBool> flagBuffer(aSbrEnabled);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCIEAacPlusDecoderSetSbr,
										flagBuffer);
		}
	}

void CMMFEAacPlusDecoderMux::SetDownSampledMode(TBool aDsmEnabled)
	{
	if (iRemoteHandle > 0)
		{
		// send the DSM enabled flag in the sync command
		TPckgBuf<TBool> flagBuffer(aDsmEnabled);
		// No way of reporting an error message so ignore the return value from SendSlaveSyncCommand
		iUtility->SendSlaveSyncCommand( iRemoteHandle,
										EMMFDevSoundCIEAacPlusDecoderSetDownSampledMode,
										flagBuffer);
		}
	}

TInt CMMFEAacPlusDecoderMux::ApplyConfig()
	{
	TInt retVal = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		retVal = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIEAacPlusDecoderApplyConfig,
												KNullDesC8);
		}
	return retVal;
	}

TInt CMMFEAacPlusDecoderMux::GetInputSamplingFrequency(TUint& aInputSamplingFrequency)
	{
	TInt result = KErrNotReady;

	if (iRemoteHandle > 0)
		{
		TPckgBuf<TUint> freqBuffer;
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIEAacPlusDecoderGetInputSamplingFrequency,
													  KNullDesC8,
													  freqBuffer);
		aInputSamplingFrequency = freqBuffer();
		}
		
	return result;
	}

TInt CMMFEAacPlusDecoderMux::GetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType& aAudioObjectType)
	{
	TInt result = KErrNotReady;

	if (iRemoteHandle > 0) 
		{
		TPckgBuf<MEAacPlusDecoderIntfc::TAudioObjectType> objTypeBuffer;
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIEAacPlusDecoderGetAudioObjectType,
													  KNullDesC8,
													  objTypeBuffer);
		aAudioObjectType = objTypeBuffer();
		}

	return result;
	}

TInt CMMFEAacPlusDecoderMux::GetNumOfChannels(TUint& aNumOfChannels)
	{
	TInt result = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TUint> channelsBuffer;
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIEAacPlusDecoderGetNumOfChannels,
													  KNullDesC8,
													  channelsBuffer);
		aNumOfChannels = channelsBuffer();
		}
		
	return result;
	}

TInt CMMFEAacPlusDecoderMux::GetSbr(TBool& aSbrEnabled)
	{
	TInt result = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TBool> flagBuffer;
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIEAacPlusDecoderGetSbr,
													  KNullDesC8,
													  flagBuffer);
		aSbrEnabled = flagBuffer();
		}
		
	return result;
	}

TInt CMMFEAacPlusDecoderMux::GetDownSampledMode(TBool& aDsmEnabled)
	{
	TInt result = KErrNotReady;
	if (iRemoteHandle > 0)
		{
		TPckgBuf<TBool> flagBuffer;
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIEAacPlusDecoderGetDownSampledMode,
													  KNullDesC8,
													  flagBuffer);
		aDsmEnabled = flagBuffer();
		}
		
	return result;
	}

// DEMUX //	
/*****************************************************************************/
TInt CMMFEAacPlusDecoderDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}

/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::Release()
	{
	TUid key = iDestructorKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}

/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/
void CMMFEAacPlusDecoderDeMux::RefreshL()
	{
	// refetch the EAAC+ decoder custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceEAacPlusDecoder = static_cast <MEAacPlusDecoderIntfc*> (iTarget->CustomInterface(KUidEAacPlusDecoderIntfc));
		if (!iInterfaceEAacPlusDecoder)
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

/*****************************************************************************/
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFEAacPlusDecoderDeMux::NewL()
	{
	CMMFEAacPlusDecoderDeMux* self = new (ELeave) CMMFEAacPlusDecoderDeMux;
	return self;
	}

/*****************************************************************************/	
CMMFEAacPlusDecoderDeMux::CMMFEAacPlusDecoderDeMux()
	{	
	}

/*****************************************************************************/
CMMFEAacPlusDecoderDeMux::~CMMFEAacPlusDecoderDeMux()
	{
	}

/*****************************************************************************/
TInt CMMFEAacPlusDecoderDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the EAAC+ decoder Hw Device custom interface
	iInterfaceEAacPlusDecoder = static_cast<MEAacPlusDecoderIntfc*> (iTarget->CustomInterface(KUidEAacPlusDecoderIntfc)); 
	
	if (!iInterfaceEAacPlusDecoder)
		{
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

/*****************************************************************************/
// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFEAacPlusDecoderDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt retVal = -1;
	switch (data.iCommand)
		{
		case EMMFDevSoundCIEAacPlusDecoderSetInputSamplingFrequency:
			{
			TPckgBuf<TUint> freqBuffer;
			iUtility->ReadFromInputDesL(aMessage, &freqBuffer);
			DoSetInputSamplingFrequency(freqBuffer());
			retVal = KErrNone;
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderSetAudioObjectType:
			{
			TPckgBuf<MEAacPlusDecoderIntfc::TAudioObjectType> audioObjectTypeBuffer;
			iUtility->ReadFromInputDesL(aMessage, &audioObjectTypeBuffer);
			DoSetAudioObjectType(audioObjectTypeBuffer());
			retVal = KErrNone;
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderSetNumOfChannels:
			{
			TPckgBuf<TUint> numChannelsBuffer;
			iUtility->ReadFromInputDesL(aMessage, &numChannelsBuffer);
			DoSetNumOfChannels(numChannelsBuffer());
			retVal = KErrNone;
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderSetSbr:
			{
			TPckgBuf<TBool> flagBuffer;
			iUtility->ReadFromInputDesL(aMessage, &flagBuffer);
			DoSetSbr(flagBuffer());
			retVal = KErrNone;
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderSetDownSampledMode:
			{
			TPckgBuf<TBool> flagBuffer;
			iUtility->ReadFromInputDesL(aMessage, &flagBuffer);
			DoSetDownSampledMode(flagBuffer());
			retVal = KErrNone;
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderApplyConfig:
			{
			retVal = DoApplyConfig();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}		
		};
		
	return retVal;
	}
	
/*****************************************************************************/	
TInt CMMFEAacPlusDecoderDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt retVal = -1;
	switch (data.iCommand)
		{
		case EMMFDevSoundCIEAacPlusDecoderGetInputSamplingFrequency:
			{
			TPckgBuf<TUint> freqBuf;
			retVal = DoGetInputSamplingFrequency(freqBuf());
			iUtility->WriteToOutputDesL(aMessage, freqBuf);		
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderGetAudioObjectType:
			{
			TPckgBuf<MEAacPlusDecoderIntfc::TAudioObjectType> objTypeBuf;
			retVal = DoGetAudioObjectType(objTypeBuf());
			iUtility->WriteToOutputDesL(aMessage, objTypeBuf);		
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderGetNumOfChannels:
			{
			TPckgBuf<TUint> channelsBuf;
			retVal = DoGetNumOfChannels(channelsBuf());
			iUtility->WriteToOutputDesL(aMessage, channelsBuf);		
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderGetSbr:
			{
			TPckgBuf<TBool> flagBuf;
			retVal = DoGetSbr(flagBuf());
			iUtility->WriteToOutputDesL(aMessage, flagBuf);		
			break;
			}
		case EMMFDevSoundCIEAacPlusDecoderGetDownSampledMode:
			{
			TPckgBuf<TBool> flagBuf;
			retVal = DoGetDownSampledMode(flagBuf());
			iUtility->WriteToOutputDesL(aMessage, flagBuf);		
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
void CMMFEAacPlusDecoderDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

/*****************************************************************************/
// EAAC+ Decoder custom interface implementation
void CMMFEAacPlusDecoderDeMux::DoSetInputSamplingFrequency(TUint aInputSamplingFrequency)
	{
	if (iInterfaceEAacPlusDecoder)
		{
		iInterfaceEAacPlusDecoder->SetInputSamplingFrequency(aInputSamplingFrequency);
		}
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::DoSetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType aAudioObjectType)
	{
	if (iInterfaceEAacPlusDecoder)
		{
		iInterfaceEAacPlusDecoder->SetAudioObjectType(aAudioObjectType);
		}		
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::DoSetNumOfChannels(TUint aNumOfChannels)
	{
	if (iInterfaceEAacPlusDecoder)
		{
		iInterfaceEAacPlusDecoder->SetNumOfChannels(aNumOfChannels);
		}		
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::DoSetSbr(TBool aSbrEnabled)
	{
	if (iInterfaceEAacPlusDecoder)
		{
		iInterfaceEAacPlusDecoder->SetSbr(aSbrEnabled);
		}		
	}
	
/*****************************************************************************/	
void CMMFEAacPlusDecoderDeMux::DoSetDownSampledMode(TBool aDsmEnabled)
	{
	if (iInterfaceEAacPlusDecoder)
		{
		iInterfaceEAacPlusDecoder->SetDownSampledMode(aDsmEnabled);
		}		
	}
	
/*****************************************************************************/	
TInt CMMFEAacPlusDecoderDeMux::DoApplyConfig()
	{
	// Initialise the return value to an error that reflects the plugin's not ready
	TInt retVal = KErrNotReady;
	if (iInterfaceEAacPlusDecoder)
		{
		retVal = iInterfaceEAacPlusDecoder->ApplyConfig();
		}
	return retVal;
	}
	
/*****************************************************************************/	
TInt CMMFEAacPlusDecoderDeMux::DoGetInputSamplingFrequency(TUint& aInputSamplingFrequency)
	{
	TInt ret = KErrNotReady;
	if (iInterfaceEAacPlusDecoder)
		{
		ret = iInterfaceEAacPlusDecoder->GetInputSamplingFrequency(aInputSamplingFrequency);
		}
	return ret;
	}
	
/*****************************************************************************/	
TInt CMMFEAacPlusDecoderDeMux::DoGetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType& aAudioObjectType)
	{
	TInt err = KErrNotReady;
	if (iInterfaceEAacPlusDecoder)
		{
		err = iInterfaceEAacPlusDecoder->GetAudioObjectType(aAudioObjectType);
		}
	return err;
	}

/*****************************************************************************/	
TInt CMMFEAacPlusDecoderDeMux::DoGetNumOfChannels(TUint& aNumOfChannels)
	{
	TInt err = KErrNotReady;
	if (iInterfaceEAacPlusDecoder)
		{
		err = iInterfaceEAacPlusDecoder->GetNumOfChannels(aNumOfChannels);
		}
	return err;
	}

/*****************************************************************************/	
TInt CMMFEAacPlusDecoderDeMux::DoGetSbr(TBool& aSbrEnabled)
	{
	TInt err = KErrNotReady;
	if (iInterfaceEAacPlusDecoder)
		{
		err = iInterfaceEAacPlusDecoder->GetSbr(aSbrEnabled);
		}
	return err;
	}
	
/*****************************************************************************/	
TInt CMMFEAacPlusDecoderDeMux::DoGetDownSampledMode(TBool& aDsmEnabled)
	{
	TInt err = KErrNotReady;
	if (iInterfaceEAacPlusDecoder)
		{
		err = iInterfaceEAacPlusDecoder->GetDownSampledMode(aDsmEnabled);
		}
	return err;
	}
	
/*****************************************************************************/	
	
/*****************************************************************************/
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceEAacPlusDecoderMux, CMMFEAacPlusDecoderMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceEAacPlusDecoderDeMux, CMMFEAacPlusDecoderDeMux::NewL),
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


