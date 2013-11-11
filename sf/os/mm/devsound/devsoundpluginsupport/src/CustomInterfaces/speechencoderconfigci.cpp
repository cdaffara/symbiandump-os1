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

#include "speechencoderconfigci.h"


// MUX //

TInt CMMFSpeechEncoderConfigMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceSpeechEncoderConfigDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFSpeechEncoderConfigMux::Release()
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


void CMMFSpeechEncoderConfigMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFSpeechEncoderConfigMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFSpeechEncoderConfigMux::NewL()
	{
	CMMFSpeechEncoderConfigMux* self = new (ELeave) CMMFSpeechEncoderConfigMux;
	return self;
	}


TAny* CMMFSpeechEncoderConfigMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MSpeechEncoderConfig* interface = this;
	return interface;
	}


CMMFSpeechEncoderConfigMux::CMMFSpeechEncoderConfigMux() :
iRemoteHandle(-1)
	{
	}


CMMFSpeechEncoderConfigMux::~CMMFSpeechEncoderConfigMux()
	{
	}


// from MSpeechEncoderConfig
TInt CMMFSpeechEncoderConfigMux::SetVadMode (TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the VadModeOn in the sync command
		TPckgBuf<TBool> vadModeOn(aVadModeOn);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCISpeechEncoderConfigSetVadMode,
												vadModeOn);
		}

	return result;
	}


// from MSpeechEncoderConfig
TInt CMMFSpeechEncoderConfigMux::GetVadMode (TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// holds the returned value.
		TPckgBuf<TBool> retVadModeOn;
		
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCISpeechEncoderConfigGetVadMode,
													  KNullDesC8,
													  retVadModeOn);

		// assign return values to aVadModeOn. Do nothing if there is an error
		if(result == KErrNone)
			{
			aVadModeOn = retVadModeOn();
			}
		}

	return result;
	}



// DEMUX //	

TInt CMMFSpeechEncoderConfigDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFSpeechEncoderConfigDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFSpeechEncoderConfigDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFSpeechEncoderConfigDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFSpeechEncoderConfigDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFSpeechEncoderConfigDeMux::RefreshL()
	{
	// refetch the Speech Encoder Config custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceSpeechEncoderConfig = static_cast <MSpeechEncoderConfig*> (iTarget->CustomInterface(KUidSpeechEncoderConfig));

		if (!iInterfaceSpeechEncoderConfig)
			{
			iInterfaceSpeechEncoderConfig = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFSpeechEncoderConfigDeMux::NewL()
	{
	CMMFSpeechEncoderConfigDeMux* self = new (ELeave) CMMFSpeechEncoderConfigDeMux;
	return self;
	}


CMMFSpeechEncoderConfigDeMux::CMMFSpeechEncoderConfigDeMux()
	{
	}


CMMFSpeechEncoderConfigDeMux::~CMMFSpeechEncoderConfigDeMux()
	{
	}


TInt CMMFSpeechEncoderConfigDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Speech Encoder Config Hw Device custom interface
	iInterfaceSpeechEncoderConfig = static_cast<MSpeechEncoderConfig*> (iTarget->CustomInterface(KUidSpeechEncoderConfig)); 

	if (!iInterfaceSpeechEncoderConfig)
		{
		iInterfaceSpeechEncoderConfig = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFSpeechEncoderConfigDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFSpeechEncoderConfigDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCISpeechEncoderConfigSetVadMode:
			{
			TPckgBuf<TBool> vadModeOn; 
			iUtility->ReadFromInputDesL(aMessage, &vadModeOn);

			result = DoSetVadModeL(vadModeOn());

			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


TInt CMMFSpeechEncoderConfigDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNone;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCISpeechEncoderConfigGetVadMode:
			{
			TPckgBuf<TBool> vadModeOn; 
			iUtility->ReadFromInputDesL(aMessage, &vadModeOn);

			result = DoGetVadModeL(vadModeOn());

			TPckgBuf<TBool> des(vadModeOn());
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


void CMMFSpeechEncoderConfigDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFSpeechEncoderConfigDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// Speech Encoder Config custom interface implementation
TInt CMMFSpeechEncoderConfigDeMux::DoSetVadModeL(TBool aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceSpeechEncoderConfig)
		{
		result = iInterfaceSpeechEncoderConfig->SetVadMode(aVadModeOn);
		}

	return result;
	}


// Speech Encoder Config custom interface implementation
TInt CMMFSpeechEncoderConfigDeMux::DoGetVadModeL(TBool& aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceSpeechEncoderConfig)
		{
		result = iInterfaceSpeechEncoderConfig->GetVadMode(aVadModeOn);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSpeechEncoderConfigMux,	CMMFSpeechEncoderConfigMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSpeechEncoderConfigDeMux,	CMMFSpeechEncoderConfigDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
