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

#include "g711decoderconfigci.h"


// MUX //

TInt CMMFG711DecoderIntfcMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceG711DecoderIntfcDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFG711DecoderIntfcMux::Release()
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


void CMMFG711DecoderIntfcMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG711DecoderIntfcMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFG711DecoderIntfcMux::NewL()
	{
	CMMFG711DecoderIntfcMux* self = new (ELeave) CMMFG711DecoderIntfcMux;
	return self;
	}


TAny* CMMFG711DecoderIntfcMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MG711DecoderIntfc* interface = this;
	return interface;
	}


CMMFG711DecoderIntfcMux::CMMFG711DecoderIntfcMux() :
iRemoteHandle(-1)
	{
	}


CMMFG711DecoderIntfcMux::~CMMFG711DecoderIntfcMux()
	{
	}


// from MG711DecoderIntfc
TInt CMMFG711DecoderIntfcMux::SetDecoderMode(TDecodeMode aDecodeMode)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the decodeMode in the sync command
		TPckgBuf<TDecodeMode> decodeMode(aDecodeMode);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIG711DecoderIntfcSetDecoderMode,
												decodeMode);
		}

	return result;
	}


// from MG711DecoderIntfc
TInt CMMFG711DecoderIntfcMux::GetDecoderMode(TDecodeMode& aDecodeMode)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the decodeMode in the sync command
		TPckgBuf<TDecodeMode> retDecodeMode;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIG711DecoderIntfcGetDecoderMode,
													  KNullDesC8,
													  retDecodeMode);

		// assign return values to aDecodeMode. Do nothing if there is an error
		if(result == KErrNone)
			{
			aDecodeMode = retDecodeMode();
			}
		}

	return result;
	}


// from MG711DecoderIntfc
TInt CMMFG711DecoderIntfcMux::SetComfortNoiseGeneration(TBool aCng)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the cng in the sync command
		TPckgBuf<TBool> cng(aCng);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIG711DecoderIntfcSetComfortNoiseGeneration,
												cng);
		}

	return result;
	}


// from MG711DecoderIntfc
TInt CMMFG711DecoderIntfcMux::GetComfortNoiseGeneration(TBool& aCng)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// holds the returned value.
		TPckgBuf<TBool> retCng;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIG711DecoderIntfcGetComfortNoiseGeneration,
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


// from MG711DecoderIntfc
TInt CMMFG711DecoderIntfcMux::SetPacketLossConcealment(TBool aPlc)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the plc in the sync command
		TPckgBuf<TBool> plc(aPlc);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIG711DecoderIntfcSetPacketLossConcealment,
												plc);
		}

	return result;
	}


// from MG711DecoderIntfc
TInt CMMFG711DecoderIntfcMux::GetPacketLossConcealment(TBool& aPlc)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the plc in the sync command
		TPckgBuf<TBool> retPlc;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIG711DecoderIntfcGetPacketLossConcealment,
													  KNullDesC8,
													  retPlc);

		// assign return values to aPlc. Do nothing if there is an error
		if(result == KErrNone)
			{
			aPlc = retPlc();
			}
		}

	return result;
	}



// DEMUX //	

TInt CMMFG711DecoderIntfcDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFG711DecoderIntfcDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFG711DecoderIntfcDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG711DecoderIntfcDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFG711DecoderIntfcDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFG711DecoderIntfcDeMux::RefreshL()
	{
	// refetch the G711 decoder intfc custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceG711DecoderIntfc = static_cast <MG711DecoderIntfc*> (iTarget->CustomInterface(KUidG711DecoderIntfc));

		if (!iInterfaceG711DecoderIntfc)
			{
			iInterfaceG711DecoderIntfc = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFG711DecoderIntfcDeMux::NewL()
	{
	CMMFG711DecoderIntfcDeMux* self = new (ELeave) CMMFG711DecoderIntfcDeMux;
	return self;
	}


CMMFG711DecoderIntfcDeMux::CMMFG711DecoderIntfcDeMux()
	{
	}


CMMFG711DecoderIntfcDeMux::~CMMFG711DecoderIntfcDeMux()
	{
	}


TInt CMMFG711DecoderIntfcDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the G711 decoder intfc Hw Device custom interface
	iInterfaceG711DecoderIntfc = static_cast<MG711DecoderIntfc*> (iTarget->CustomInterface(KUidG711DecoderIntfc)); 

	if (!iInterfaceG711DecoderIntfc)
		{
		iInterfaceG711DecoderIntfc = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFG711DecoderIntfcDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFG711DecoderIntfcDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIG711DecoderIntfcSetDecoderMode:
			{
			TPckgBuf<MG711DecoderIntfc::TDecodeMode> decodeMode; 
			iUtility->ReadFromInputDesL(aMessage, &decodeMode);

			result = DoSetDecoderModeL(decodeMode());

			break;
			}
		case EMMFDevSoundCIG711DecoderIntfcSetComfortNoiseGeneration:
			{
			TPckgBuf<TBool> cng; 
			iUtility->ReadFromInputDesL(aMessage, &cng);

			result = DoSetComfortNoiseGenerationL(cng());
			break;
			}
		case EMMFDevSoundCIG711DecoderIntfcSetPacketLossConcealment:
			{
			TPckgBuf<TBool> plc; 
			iUtility->ReadFromInputDesL(aMessage, &plc);
			
			result = DoSetPacketLossConcealmentL(plc());
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
TInt CMMFG711DecoderIntfcDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIG711DecoderIntfcGetDecoderMode:
			{
			TPckgBuf<MG711DecoderIntfc::TDecodeMode> decodeMode; 
			iUtility->ReadFromInputDesL(aMessage, &decodeMode);

			result = DoGetDecoderModeL(decodeMode());

			TPckgBuf<TBool> des(decodeMode());
			iUtility->WriteToOutputDesL(aMessage, des);

			break;
			}
		case EMMFDevSoundCIG711DecoderIntfcGetComfortNoiseGeneration:
			{
			TPckgBuf<TBool> cng; 
			iUtility->ReadFromInputDesL(aMessage, &cng);

			result = DoGetComfortNoiseGenerationL(cng());

			TPckgBuf<TBool> des(cng());
			iUtility->WriteToOutputDesL(aMessage, des);

			break;
			}
		case EMMFDevSoundCIG711DecoderIntfcGetPacketLossConcealment:
			{
			TPckgBuf<TBool> plc; 
			iUtility->ReadFromInputDesL(aMessage, &plc);

			result = DoGetPacketLossConcealmentL(plc());

			TPckgBuf<TBool> des(plc());
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


void CMMFG711DecoderIntfcDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFG711DecoderIntfcDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// G711 decoder intfc custom interface implementation
TInt CMMFG711DecoderIntfcDeMux::DoSetDecoderModeL(MG711DecoderIntfc::TDecodeMode aDecodeMode)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711DecoderIntfc)
		{
		result = iInterfaceG711DecoderIntfc->SetDecoderMode(aDecodeMode);
		}

	return result;
	}


// G711 decoder intfc custom interface implementation
TInt CMMFG711DecoderIntfcDeMux::DoGetDecoderModeL(MG711DecoderIntfc::TDecodeMode& aDecodeMode)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711DecoderIntfc)
		{
		result = iInterfaceG711DecoderIntfc->GetDecoderMode(aDecodeMode);
		}

	return result;
	}


// G711 decoder intfc custom interface implementation
TInt CMMFG711DecoderIntfcDeMux::DoSetComfortNoiseGenerationL(TBool aCng)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711DecoderIntfc)
		{
		result = iInterfaceG711DecoderIntfc->SetComfortNoiseGeneration(aCng);
		}

	return result;
	}


// G711 decoder intfc custom interface implementation
TInt CMMFG711DecoderIntfcDeMux::DoGetComfortNoiseGenerationL(TBool& aCng)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711DecoderIntfc)
		{
		result = iInterfaceG711DecoderIntfc->GetComfortNoiseGeneration(aCng);
		}
		
	return result;
	}


// G711 decoder intfc custom interface implementation
TInt CMMFG711DecoderIntfcDeMux::DoSetPacketLossConcealmentL(TBool aPlc)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711DecoderIntfc)
		{
		result = iInterfaceG711DecoderIntfc->SetPacketLossConcealment(aPlc);
		}

	return result;
	}


// G711 decoder intfc custom interface implementation
TInt CMMFG711DecoderIntfcDeMux::DoGetPacketLossConcealmentL(TBool& aPlc)
	{
	TInt result = KErrNotFound;

	if (iInterfaceG711DecoderIntfc)
		{
		result = iInterfaceG711DecoderIntfc->GetPacketLossConcealment(aPlc);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG711DecoderIntfcMux,	CMMFG711DecoderIntfcMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG711DecoderIntfcDeMux,	CMMFG711DecoderIntfcDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
