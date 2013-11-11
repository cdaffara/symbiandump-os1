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

#include "g729encoderconfigci.h"


// MUX //

TInt CMMFG729EncoderIntfcMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceG729EncoderIntfcDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFG729EncoderIntfcMux::Release()
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


void CMMFG729EncoderIntfcMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG729EncoderIntfcMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFG729EncoderIntfcMux::NewL()
	{
	CMMFG729EncoderIntfcMux* self = new (ELeave) CMMFG729EncoderIntfcMux;
	return self;
	}


TAny* CMMFG729EncoderIntfcMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MG729EncoderIntfc* interface = this;
	return interface;
	}


CMMFG729EncoderIntfcMux::CMMFG729EncoderIntfcMux() :
	iRemoteHandle(-1)
	{
	}


CMMFG729EncoderIntfcMux::~CMMFG729EncoderIntfcMux()
	{
	}


// from MG729EncoderIntfc
TInt CMMFG729EncoderIntfcMux::SetVadMode (TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the vadMode in the sync command
		TPckgBuf<TBool> vadModeOn(aVadModeOn);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIG729EncoderIntfcSetVadMode,
												vadModeOn);
		}

	return result;
	}


// from MG729EncoderIntfc
TInt CMMFG729EncoderIntfcMux::GetVadMode (TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		//holds the returned value
		TPckgBuf<TBool> retVadModeOn;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIG729EncoderIntfcGetVadMode,
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

TInt CMMFG729EncoderIntfcDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFG729EncoderIntfcDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFG729EncoderIntfcDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG729EncoderIntfcDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFG729EncoderIntfcDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFG729EncoderIntfcDeMux::RefreshL()
	{
	// refetch the G729 encoder intfc custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceG729EncoderIntfc = static_cast <MG729EncoderIntfc*> (iTarget->CustomInterface(KUidG729EncoderIntfc));

		if (!iInterfaceG729EncoderIntfc)
			{
			iInterfaceG729EncoderIntfc = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFG729EncoderIntfcDeMux::NewL()
	{
	CMMFG729EncoderIntfcDeMux* self = new (ELeave) CMMFG729EncoderIntfcDeMux;
	return self;
	}


CMMFG729EncoderIntfcDeMux::CMMFG729EncoderIntfcDeMux()
	{
	}


CMMFG729EncoderIntfcDeMux::~CMMFG729EncoderIntfcDeMux()
	{
	}


TInt CMMFG729EncoderIntfcDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the G729 encoder intfc Hw Device custom interface
	iInterfaceG729EncoderIntfc = static_cast<MG729EncoderIntfc*> (iTarget->CustomInterface(KUidG729EncoderIntfc)); 

	if (!iInterfaceG729EncoderIntfc)
		{
		iInterfaceG729EncoderIntfc = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFG729EncoderIntfcDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFG729EncoderIntfcDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIG729EncoderIntfcSetVadMode:
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


TInt CMMFG729EncoderIntfcDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNone;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIG729EncoderIntfcGetVadMode:
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


void CMMFG729EncoderIntfcDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFG729EncoderIntfcDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


TInt CMMFG729EncoderIntfcDeMux::DoSetVadModeL(TBool aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG729EncoderIntfc)
		{
		result = iInterfaceG729EncoderIntfc->SetVadMode(aVadModeOn);
		}

	return result;
	}


TInt CMMFG729EncoderIntfcDeMux::DoGetVadModeL(TBool& aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG729EncoderIntfc)
		{
		result = iInterfaceG729EncoderIntfc->GetVadMode(aVadModeOn);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG729EncoderIntfcMux,	CMMFG729EncoderIntfcMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG729EncoderIntfcDeMux,	CMMFG729EncoderIntfcDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

