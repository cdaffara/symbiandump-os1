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

#include "g711encoderconfigci.h"


// MUX //

TInt CMMFG711EncoderIntfcMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceG711EncoderIntfcDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFG711EncoderIntfcMux::Release()
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


void CMMFG711EncoderIntfcMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG711EncoderIntfcMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFG711EncoderIntfcMux::NewL()
	{
	CMMFG711EncoderIntfcMux* self = new (ELeave) CMMFG711EncoderIntfcMux;
	return self;
	}


TAny* CMMFG711EncoderIntfcMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MG711EncoderIntfc* interface = this;
	return interface;
	}


CMMFG711EncoderIntfcMux::CMMFG711EncoderIntfcMux() :
iRemoteHandle(-1)
	{
	}


CMMFG711EncoderIntfcMux::~CMMFG711EncoderIntfcMux()
	{
	}


// from MG711EncoderIntfc
TInt CMMFG711EncoderIntfcMux::SetEncoderMode(TEncodeMode aEncodeMode)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the encodeMode in the sync command
		TPckgBuf<TEncodeMode> encodeMode(aEncodeMode);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIG711EncoderIntfcSetEncoderMode,
												encodeMode);
		}

	return result;
	}


// from MG711EncoderIntfc
TInt CMMFG711EncoderIntfcMux::SetVadMode (TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the vadMondeOn in the sync command
		TPckgBuf<TBool> vadModeOn(aVadModeOn);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIG711EncoderIntfcSetVadMode,
												vadModeOn);
		}

	return result;
	}


// from MG711EncoderIntfc
TInt CMMFG711EncoderIntfcMux::GetVadMode (TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// holds the returned value.
		TPckgBuf<TBool> retVadModeOn;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIG711EncoderIntfcGetVadMode,
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

TInt CMMFG711EncoderIntfcDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFG711EncoderIntfcDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFG711EncoderIntfcDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG711EncoderIntfcDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFG711EncoderIntfcDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFG711EncoderIntfcDeMux::RefreshL()
	{
	// refetch the G711 encoder intfc custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceG711EncoderIntfc = static_cast <MG711EncoderIntfc*> (iTarget->CustomInterface(KUidG711EncoderIntfc));

		if (!iInterfaceG711EncoderIntfc)
			{
			iInterfaceG711EncoderIntfc = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFG711EncoderIntfcDeMux::NewL()
	{
	CMMFG711EncoderIntfcDeMux* self = new (ELeave) CMMFG711EncoderIntfcDeMux;
	return self;
	}


CMMFG711EncoderIntfcDeMux::CMMFG711EncoderIntfcDeMux()
	{
	}


CMMFG711EncoderIntfcDeMux::~CMMFG711EncoderIntfcDeMux()
	{
	}


TInt CMMFG711EncoderIntfcDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the G711 encoder intfc Hw Device custom interface
	iInterfaceG711EncoderIntfc = static_cast<MG711EncoderIntfc*> (iTarget->CustomInterface(KUidG711EncoderIntfc)); 

	if (!iInterfaceG711EncoderIntfc)
		{
		iInterfaceG711EncoderIntfc = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFG711EncoderIntfcDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFG711EncoderIntfcDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIG711EncoderIntfcSetEncoderMode:
			{
			TPckgBuf<MG711EncoderIntfc::TEncodeMode> encodeMode; 
			iUtility->ReadFromInputDesL(aMessage, &encodeMode);

			result = DoSetEncoderMode(encodeMode());

			break;
			}
		case EMMFDevSoundCIG711EncoderIntfcSetVadMode:
			{
			TPckgBuf<TBool> vadModeOn; 
			iUtility->ReadFromInputDesL(aMessage, &vadModeOn);

			result = DoSetVadMode(vadModeOn());

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
TInt CMMFG711EncoderIntfcDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNone;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIG711EncoderIntfcGetVadMode:
			{
			TPckgBuf<TBool> vadModeOn; 
			iUtility->ReadFromInputDesL(aMessage, &vadModeOn);

			result = DoGetVadMode(vadModeOn());

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


void CMMFG711EncoderIntfcDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFG711EncoderIntfcDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// G711 encoder intfc custom interface implementation
TInt CMMFG711EncoderIntfcDeMux::DoSetEncoderMode(MG711EncoderIntfc::TEncodeMode aEncodeMode)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711EncoderIntfc)
		{
		result = iInterfaceG711EncoderIntfc->SetEncoderMode(aEncodeMode);
		}

	return result;
	}


// G711 encoder intfc custom interface implementation
TInt CMMFG711EncoderIntfcDeMux::DoSetVadMode(TBool aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711EncoderIntfc)
		{
		result = iInterfaceG711EncoderIntfc->SetVadMode(aVadModeOn);
		}

	return result;
	}


// G711 encoder intfc custom interface implementation
TInt CMMFG711EncoderIntfcDeMux::DoGetVadMode(TBool& aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711EncoderIntfc)
		{
		result = iInterfaceG711EncoderIntfc->GetVadMode(aVadModeOn);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG711EncoderIntfcMux,	CMMFG711EncoderIntfcMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG711EncoderIntfcDeMux,	CMMFG711EncoderIntfcDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
