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

#include "ilbcdecoderconfigci.h"


// MUX //

TInt CMMFIlbcDecoderIntfcMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceIlbcDecoderIntfcDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFIlbcDecoderIntfcMux::Release()
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


void CMMFIlbcDecoderIntfcMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFIlbcDecoderIntfcMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFIlbcDecoderIntfcMux::NewL()
	{
	CMMFIlbcDecoderIntfcMux* self = new (ELeave) CMMFIlbcDecoderIntfcMux;
	return self;
	}


TAny* CMMFIlbcDecoderIntfcMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MIlbcDecoderIntfc* interface = this;
	return interface;
	}


CMMFIlbcDecoderIntfcMux::CMMFIlbcDecoderIntfcMux() :
iRemoteHandle(-1)
	{
	}


CMMFIlbcDecoderIntfcMux::~CMMFIlbcDecoderIntfcMux()
	{
	}


// from MIlbcDecoderIntfc
TInt CMMFIlbcDecoderIntfcMux::SetDecoderMode(TDecodeMode aDecodeMode)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the decodeMode in the sync command
		TPckgBuf<TDecodeMode> decodeMode(aDecodeMode);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIIlbcDecoderIntfcSetDecoderMode,
												decodeMode);
		}

	return result;
	}


// from MIlbcDecoderIntfc
TInt CMMFIlbcDecoderIntfcMux::SetComfortNoiseGeneration(TBool aCng)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the cng in the sync command
		TPckgBuf<TBool> cng(aCng);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIIlbcDecoderIntfcSetComfortNoiseGeneration,
												cng);
		}

	return result;
	}


// from MIlbcDecoderIntfc
TInt CMMFIlbcDecoderIntfcMux::GetComfortNoiseGeneration(TBool& aCng)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// holds the returned value.
		TPckgBuf<TBool> retCng;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIIlbcDecoderIntfcGetComfortNoiseGeneration,
													  KNullDesC8,
													  retCng);

		// assign return values to aCng. Do nothing if there is an error
		if(result == KErrNone)
			{
			aCng = retCng();
			}
		}

	return result;
	}



// DEMUX //	

TInt CMMFIlbcDecoderIntfcDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFIlbcDecoderIntfcDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFIlbcDecoderIntfcDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFIlbcDecoderIntfcDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFIlbcDecoderIntfcDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFIlbcDecoderIntfcDeMux::RefreshL()
	{
	// refetch the Ilbc decoder intfc custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceIlbcDecoderIntfc = static_cast <MIlbcDecoderIntfc*> (iTarget->CustomInterface(KUidIlbcDecoderIntfc));

		if (!iInterfaceIlbcDecoderIntfc)
			{
			iInterfaceIlbcDecoderIntfc = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFIlbcDecoderIntfcDeMux::NewL()
	{
	CMMFIlbcDecoderIntfcDeMux* self = new (ELeave) CMMFIlbcDecoderIntfcDeMux;
	return self;
	}


CMMFIlbcDecoderIntfcDeMux::CMMFIlbcDecoderIntfcDeMux()
	{
	}


CMMFIlbcDecoderIntfcDeMux::~CMMFIlbcDecoderIntfcDeMux()
	{
	}


TInt CMMFIlbcDecoderIntfcDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Ilbc decoder intfc Hw Device custom interface
	iInterfaceIlbcDecoderIntfc = static_cast<MIlbcDecoderIntfc*> (iTarget->CustomInterface(KUidIlbcDecoderIntfc)); 

	if (!iInterfaceIlbcDecoderIntfc)
		{
		iInterfaceIlbcDecoderIntfc = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFIlbcDecoderIntfcDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFIlbcDecoderIntfcDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIIlbcDecoderIntfcSetDecoderMode:
			{
			TPckgBuf<MIlbcDecoderIntfc::TDecodeMode> decodeMode; 
			iUtility->ReadFromInputDesL(aMessage, &decodeMode);

			result = DoSetDecoderMode(decodeMode());

			break;
			}
		case EMMFDevSoundCIIlbcDecoderIntfcSetComfortNoiseGeneration:
			{
			TPckgBuf<TBool> cng; 
			iUtility->ReadFromInputDesL(aMessage, &cng);

			result = DoSetComfortNoiseGenerationL(cng());

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
TInt CMMFIlbcDecoderIntfcDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIIlbcDecoderIntfcGetComfortNoiseGeneration:
			{
			TPckgBuf<TBool> cng; 
			iUtility->ReadFromInputDesL(aMessage, &cng);

			result = DoGetComfortNoiseGenerationL(cng());

			TPckgBuf<TBool> des(cng());
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


void CMMFIlbcDecoderIntfcDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFIlbcDecoderIntfcDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// Ilbc decoder intfc custom interface implementation
TInt CMMFIlbcDecoderIntfcDeMux::DoSetDecoderMode(MIlbcDecoderIntfc::TDecodeMode aDecodeMode)
	{
	TInt result = KErrNotFound;

	if (iInterfaceIlbcDecoderIntfc)
		{
		result = iInterfaceIlbcDecoderIntfc->SetDecoderMode(aDecodeMode);
		}

	return result;
	}


// Ilbc decoder intfc custom interface implementation
TInt CMMFIlbcDecoderIntfcDeMux::DoSetComfortNoiseGenerationL(TBool aCng)
	{
	TInt result = KErrNotFound;

	if (iInterfaceIlbcDecoderIntfc)
		{
		result = iInterfaceIlbcDecoderIntfc->SetComfortNoiseGeneration(aCng);
		}

	return result;
	}


// Ilbc decoder intfc custom interface implementation
TInt CMMFIlbcDecoderIntfcDeMux::DoGetComfortNoiseGenerationL(TBool& aCng)
	{
	TInt result = KErrNotFound;

	if (iInterfaceIlbcDecoderIntfc)
		{
		result = iInterfaceIlbcDecoderIntfc->GetComfortNoiseGeneration(aCng);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceIlbcDecoderIntfcMux,	CMMFIlbcDecoderIntfcMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceIlbcDecoderIntfcDeMux,	CMMFIlbcDecoderIntfcDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
