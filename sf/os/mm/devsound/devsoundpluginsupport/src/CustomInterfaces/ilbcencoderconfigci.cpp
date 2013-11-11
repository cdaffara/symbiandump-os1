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

#include "ilbcencoderconfigci.h"


// MUX //

TInt CMMFIlbcEncoderIntfcMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceIlbcEncoderIntfcDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFIlbcEncoderIntfcMux::Release()
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


void CMMFIlbcEncoderIntfcMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFIlbcEncoderIntfcMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFIlbcEncoderIntfcMux::NewL()
	{
	CMMFIlbcEncoderIntfcMux* self = new (ELeave) CMMFIlbcEncoderIntfcMux;
	return self;
	}


TAny* CMMFIlbcEncoderIntfcMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MIlbcEncoderIntfc* interface = this;
	return interface;
	}


CMMFIlbcEncoderIntfcMux::CMMFIlbcEncoderIntfcMux() :
	iRemoteHandle(-1)
	{
	}


CMMFIlbcEncoderIntfcMux::~CMMFIlbcEncoderIntfcMux()
	{
	}


// from MIlbcEncoderIntfc
TInt CMMFIlbcEncoderIntfcMux::SetEncoderMode(TEncodeMode aEncodeMode)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send encodeMode in the sync command.
		TPckgBuf<TEncodeMode> encodeMode(aEncodeMode);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIIlbcEncoderIntfcSetEncoderMode,
												encodeMode);
		}

	return result;
	}


// from MIlbcEncoderIntfc
TInt CMMFIlbcEncoderIntfcMux::GetEncoderMode(TEncodeMode& aEncodeMode)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send encodeMode in the sync command.
		TPckgBuf<TEncodeMode> retEncodeMode;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIIlbcEncoderIntfcGetEncoderMode,
													  KNullDesC8,
													  retEncodeMode);

		// assign return values to aEncodeMode. Do nothing if there is an error
		if(result == KErrNone)
			{
			aEncodeMode = retEncodeMode();
			}
		}

	return result;
	}


// from MIlbcEncoderIntfc
TInt CMMFIlbcEncoderIntfcMux::SetVadMode (TBool aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send vadModeOn in the sync command.
		TPckgBuf<TBool> vadModeOn(aVadModeOn);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIIlbcEncoderIntfcSetVadMode,
												vadModeOn);
		}

	return result;
	}


// from MIlbcEncoderIntfc
TInt CMMFIlbcEncoderIntfcMux::GetVadMode (TBool& aVadModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// holds the returned value.
		TPckgBuf<TBool> retVadModeOn;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIIlbcEncoderIntfcGetVadMode,
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

TInt CMMFIlbcEncoderIntfcDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFIlbcEncoderIntfcDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFIlbcEncoderIntfcDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFIlbcEncoderIntfcDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFIlbcEncoderIntfcDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFIlbcEncoderIntfcDeMux::RefreshL()
	{
	// refetch the Ilbc encoder intfc custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceIlbcEncoderIntfc = static_cast <MIlbcEncoderIntfc*> (iTarget->CustomInterface(KUidIlbcEncoderIntfc));

		if (!iInterfaceIlbcEncoderIntfc)
			{
			iInterfaceIlbcEncoderIntfc = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFIlbcEncoderIntfcDeMux::NewL()
	{
	CMMFIlbcEncoderIntfcDeMux* self = new (ELeave) CMMFIlbcEncoderIntfcDeMux;
	return self;
	}


CMMFIlbcEncoderIntfcDeMux::CMMFIlbcEncoderIntfcDeMux()
	{
	}


CMMFIlbcEncoderIntfcDeMux::~CMMFIlbcEncoderIntfcDeMux()
	{
	}


TInt CMMFIlbcEncoderIntfcDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Ilbc encoder intfc Hw Device custom interface
	iInterfaceIlbcEncoderIntfc = static_cast<MIlbcEncoderIntfc*> (iTarget->CustomInterface(KUidIlbcEncoderIntfc)); 

	if (!iInterfaceIlbcEncoderIntfc)
		{
		iInterfaceIlbcEncoderIntfc = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFIlbcEncoderIntfcDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFIlbcEncoderIntfcDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIIlbcEncoderIntfcSetEncoderMode:
			{
			TPckgBuf<MIlbcEncoderIntfc::TEncodeMode> encodeMode; 
			iUtility->ReadFromInputDesL(aMessage, &encodeMode);

			result = DoSetEncoderModeL(encodeMode());

			break;
			}
		case EMMFDevSoundCIIlbcEncoderIntfcSetVadMode:
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


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFIlbcEncoderIntfcDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNone;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIIlbcEncoderIntfcGetEncoderMode:
			{
			TPckgBuf<MIlbcEncoderIntfc::TEncodeMode> encodeMode; 
			iUtility->ReadFromInputDesL(aMessage, &encodeMode);

			result = DoGetEncoderModeL(encodeMode());

			TPckgBuf<TBool> des(encodeMode());
			iUtility->WriteToOutputDesL(aMessage, des);

			break;
			}
		case EMMFDevSoundCIIlbcEncoderIntfcGetVadMode:
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


void CMMFIlbcEncoderIntfcDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFIlbcEncoderIntfcDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// Ilbc encoder intfc custom interface implementation
TInt CMMFIlbcEncoderIntfcDeMux::DoSetEncoderModeL(MIlbcEncoderIntfc::TEncodeMode aEncodeMode)
	{
	TInt result = KErrNotFound;

	if (iInterfaceIlbcEncoderIntfc)
		{
		result = iInterfaceIlbcEncoderIntfc->SetEncoderMode(aEncodeMode);
		}

	return result;
	}


// Ilbc encoder intfc custom interface implementation
TInt CMMFIlbcEncoderIntfcDeMux::DoGetEncoderModeL(MIlbcEncoderIntfc::TEncodeMode& aEncodeMode)
	{
	TInt result = KErrNotFound;

	if (iInterfaceIlbcEncoderIntfc)
		{
		result = iInterfaceIlbcEncoderIntfc->GetEncoderMode(aEncodeMode);
		}

	return result;
	}


// Ilbc encoder intfc custom interface implementation
TInt CMMFIlbcEncoderIntfcDeMux::DoSetVadModeL(TBool aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceIlbcEncoderIntfc)
		{
		result = iInterfaceIlbcEncoderIntfc->SetVadMode(aVadModeOn);
		}

	return result;
	}


// Ilbc encoder intfc custom interface implementation
TInt CMMFIlbcEncoderIntfcDeMux::DoGetVadModeL(TBool& aVadModeOn)
	{
	TInt result = KErrNotFound;

	if (iInterfaceIlbcEncoderIntfc)
		{
		result = iInterfaceIlbcEncoderIntfc->GetVadMode(aVadModeOn);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceIlbcEncoderIntfcMux,	CMMFIlbcEncoderIntfcMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceIlbcEncoderIntfcDeMux,	CMMFIlbcEncoderIntfcDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


