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

#include "g729decoderconfigci.h"


// MUX //

TInt CMMFG729DecoderIntfcMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceG729DecoderIntfcDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFG729DecoderIntfcMux::Release()
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


void CMMFG729DecoderIntfcMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG729DecoderIntfcMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFG729DecoderIntfcMux::NewL()
	{
	CMMFG729DecoderIntfcMux* self = new (ELeave) CMMFG729DecoderIntfcMux;
	return self;
	}


TAny* CMMFG729DecoderIntfcMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MG729DecoderIntfc* interface = this;
	return interface;
	}


CMMFG729DecoderIntfcMux::CMMFG729DecoderIntfcMux() :
	iRemoteHandle(-1)
	{
	}


CMMFG729DecoderIntfcMux::~CMMFG729DecoderIntfcMux()
	{
	}


// from MG729DecoderIntfc
TInt CMMFG729DecoderIntfcMux::BadLsfNextBuffer()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIG729DecoderIntfcBadLsfNextBuffer,
												KNullDesC8);
		}

	return result;
	}



// DEMUX //	

TInt CMMFG729DecoderIntfcDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFG729DecoderIntfcDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFG729DecoderIntfcDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFG729DecoderIntfcDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFG729DecoderIntfcDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFG729DecoderIntfcDeMux::RefreshL()
	{
	// refetch the G729 decoder intfc custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceG729DecoderIntfc = static_cast <MG729DecoderIntfc*> (iTarget->CustomInterface(KUidG729DecoderIntfc));

		if (!iInterfaceG729DecoderIntfc)
			{
			iInterfaceG729DecoderIntfc = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFG729DecoderIntfcDeMux::NewL()
	{
	CMMFG729DecoderIntfcDeMux* self = new (ELeave) CMMFG729DecoderIntfcDeMux;
	return self;
	}


CMMFG729DecoderIntfcDeMux::CMMFG729DecoderIntfcDeMux()
	{
	}


CMMFG729DecoderIntfcDeMux::~CMMFG729DecoderIntfcDeMux()
	{
	}


TInt CMMFG729DecoderIntfcDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the G729 decoder intfc Hw Device custom interface
	iInterfaceG729DecoderIntfc = static_cast<MG729DecoderIntfc*> (iTarget->CustomInterface(KUidG729DecoderIntfc));

	if (!iInterfaceG729DecoderIntfc)
		{
		iInterfaceG729DecoderIntfc = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFG729DecoderIntfcDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFG729DecoderIntfcDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIG729DecoderIntfcBadLsfNextBuffer:
			{
			result = DoBadLsfNextBufferL();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


TInt CMMFG729DecoderIntfcDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	return KErrNone;
	}


void CMMFG729DecoderIntfcDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFG729DecoderIntfcDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


TInt CMMFG729DecoderIntfcDeMux::DoBadLsfNextBufferL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceG729DecoderIntfc)
		{
		result = iInterfaceG729DecoderIntfc->BadLsfNextBuffer();
		}

	return result;
	}


//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG729DecoderIntfcMux,	CMMFG729DecoderIntfcMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceG729DecoderIntfcDeMux,	CMMFG729DecoderIntfcDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
