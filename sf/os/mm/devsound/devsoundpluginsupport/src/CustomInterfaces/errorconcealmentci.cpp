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

#include "errorconcealmentci.h"


// MUX //

TInt CMMFErrorConcealmentIntfcMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceErrorConcealmentIntfcDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFErrorConcealmentIntfcMux::Release()
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


void CMMFErrorConcealmentIntfcMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFErrorConcealmentIntfcMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFErrorConcealmentIntfcMux::NewL()
	{
	CMMFErrorConcealmentIntfcMux* self = new (ELeave) CMMFErrorConcealmentIntfcMux;
	return self;
	}


TAny* CMMFErrorConcealmentIntfcMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFErrorConcealmentIntfc* interface = this;
	return interface;
	}


CMMFErrorConcealmentIntfcMux::CMMFErrorConcealmentIntfcMux() :
	iRemoteHandle(-1)
	{
	}


CMMFErrorConcealmentIntfcMux::~CMMFErrorConcealmentIntfcMux()
	{
	}


// from MErrorConcealmentIntfc
TInt CMMFErrorConcealmentIntfcMux::ConcealErrorForNextBuffer()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIErrorConcealmentIntfcConcealErrorForNextBuffer,
												KNullDesC8);
		}

	return result;
	}


// from MErrorConcealmentIntfc
TInt CMMFErrorConcealmentIntfcMux::SetFrameMode(TBool aFrameModeOn)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// send the cng in the sync command
		TPckgBuf<TBool> frameModeOn(aFrameModeOn);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIErrorConcealmentIntfcSetFrameMode,
												frameModeOn);
		}

	return result;
	}


// from MErrorConcealmentIntfc
TInt CMMFErrorConcealmentIntfcMux::FrameModeRqrdForEC(TBool& aFrameModeRqrd)
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// holds the returned value.
		TPckgBuf<TBool> retFrameModeRqrd;

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIErrorConcealmentIntfcFrameModeRqrdForEC,
													  KNullDesC8,
													  retFrameModeRqrd);

		// assign return values to aFrameModeRqrd. Do nothing if there is an error
		if(result == KErrNone)
			{
			aFrameModeRqrd = retFrameModeRqrd();
			}
		}

	return result;
	}



// DEMUX //	

TInt CMMFErrorConcealmentIntfcDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFErrorConcealmentIntfcDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFErrorConcealmentIntfcDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFErrorConcealmentIntfcDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFErrorConcealmentIntfcDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFErrorConcealmentIntfcDeMux::RefreshL()
	{
	// refetch the Error Concealment intfc custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceErrorConcealmentIntfc = static_cast <MMMFErrorConcealmentIntfc*> (iTarget->CustomInterface(KUidErrorConcealmentIntfc));

		if (!iInterfaceErrorConcealmentIntfc)
			{
			iInterfaceErrorConcealmentIntfc = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFErrorConcealmentIntfcDeMux::NewL()
	{
	CMMFErrorConcealmentIntfcDeMux* self = new (ELeave) CMMFErrorConcealmentIntfcDeMux;
	return self;
	}


CMMFErrorConcealmentIntfcDeMux::CMMFErrorConcealmentIntfcDeMux()
	{
	}


CMMFErrorConcealmentIntfcDeMux::~CMMFErrorConcealmentIntfcDeMux()
	{
	}


TInt CMMFErrorConcealmentIntfcDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Error Concealment intfc Hw Device custom interface
	iInterfaceErrorConcealmentIntfc = static_cast<MMMFErrorConcealmentIntfc*> (iTarget->CustomInterface(KUidErrorConcealmentIntfc)); 

	if (!iInterfaceErrorConcealmentIntfc)
		{
		iInterfaceErrorConcealmentIntfc = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFErrorConcealmentIntfcDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFErrorConcealmentIntfcDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIErrorConcealmentIntfcConcealErrorForNextBuffer:
			{
			result = DoConcealErrorForNextBufferL();

			break;
			}
		case EMMFDevSoundCIErrorConcealmentIntfcSetFrameMode:
			{
			TPckgBuf<TBool> frameModeOn; 
			iUtility->ReadFromInputDesL(aMessage, &frameModeOn);
			
			result = DoSetFrameModeL(frameModeOn());

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
TInt CMMFErrorConcealmentIntfcDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIErrorConcealmentIntfcFrameModeRqrdForEC:
			{
			TPckgBuf<TBool> frameModeRqrd; 

			iUtility->ReadFromInputDesL(aMessage, &frameModeRqrd);

			result = DoFrameModeRqrdForECL(frameModeRqrd());

			TPckgBuf<TBool> des(frameModeRqrd());
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


void CMMFErrorConcealmentIntfcDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFErrorConcealmentIntfcDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// Error Concealment intfc custom interface implementation
TInt CMMFErrorConcealmentIntfcDeMux::DoConcealErrorForNextBufferL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceErrorConcealmentIntfc)
		{
		result = iInterfaceErrorConcealmentIntfc->ConcealErrorForNextBuffer();
		}

	return result;
	}


// Error Concealment intfc custom interface implementation
TInt CMMFErrorConcealmentIntfcDeMux::DoSetFrameModeL(TBool aFrameModeRqrd)
	{
	TInt result = KErrNotFound;

	if (iInterfaceErrorConcealmentIntfc)
		{
		result = iInterfaceErrorConcealmentIntfc->SetFrameMode(aFrameModeRqrd);
		}
		
	return result;
	}


// Error Concealment intfc custom interface implementation
TInt CMMFErrorConcealmentIntfcDeMux::DoFrameModeRqrdForECL(TBool& aFrameModeRqrd)
	{
	TInt result = KErrNotFound;

	if (iInterfaceErrorConcealmentIntfc)
		{
		result = iInterfaceErrorConcealmentIntfc->FrameModeRqrdForEC(aFrameModeRqrd);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceErrorConcealmentIntfcMux,		CMMFErrorConcealmentIntfcMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceErrorConcealmentIntfcDeMux,	CMMFErrorConcealmentIntfcDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

