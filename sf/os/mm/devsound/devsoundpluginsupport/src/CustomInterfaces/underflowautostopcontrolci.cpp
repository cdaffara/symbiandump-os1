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

#include "underflowautostopcontrolci.h"


// MUX //

TInt CMMFUnderflowAutoStopControlMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceUnderflowAutoStopCtrlDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFUnderflowAutoStopControlMux::Release()
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


void CMMFUnderflowAutoStopControlMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFUnderflowAutoStopControlMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFUnderflowAutoStopControlMux::NewL()
	{
	CMMFUnderflowAutoStopControlMux* self = new (ELeave) CMMFUnderflowAutoStopControlMux;
	return self;
	}


TAny* CMMFUnderflowAutoStopControlMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFUnderflowAutoStopControl* interface = this;
	return interface;
	}


CMMFUnderflowAutoStopControlMux::CMMFUnderflowAutoStopControlMux() :
	iRemoteHandle(-1)
	{
	}


CMMFUnderflowAutoStopControlMux::~CMMFUnderflowAutoStopControlMux()
	{
	}


// from MMMFUnderflowAutoStopControl
TInt CMMFUnderflowAutoStopControlMux::MmuascTurnOffUnderflowAutoStop ()
	{
	TInt result = KErrGeneral;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIUnderflowAutoStopCtrlTurnOffUnderflowAutoStop,
												KNullDesC8);
		}

	return result;
	}



// DEMUX //	

TInt CMMFUnderflowAutoStopControlDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFUnderflowAutoStopControlDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFUnderflowAutoStopControlDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFUnderflowAutoStopControlDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFUnderflowAutoStopControlDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFUnderflowAutoStopControlDeMux::RefreshL()
	{
	// refetch the UnderflowAutoStop Control  custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceUnderflowAutoStopCtrl = static_cast <MMMFUnderflowAutoStopControl*> (iTarget->CustomInterface(KUidUnderflowAutoStopControl));

		if (!iInterfaceUnderflowAutoStopCtrl)
			{
			iInterfaceUnderflowAutoStopCtrl = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFUnderflowAutoStopControlDeMux::NewL()
	{
	CMMFUnderflowAutoStopControlDeMux* self = new (ELeave) CMMFUnderflowAutoStopControlDeMux;
	return self;
	}


CMMFUnderflowAutoStopControlDeMux::CMMFUnderflowAutoStopControlDeMux()
	{
	}


CMMFUnderflowAutoStopControlDeMux::~CMMFUnderflowAutoStopControlDeMux()
	{
	}


TInt CMMFUnderflowAutoStopControlDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the UnderflowAutoStop Control  Hw Device custom interface
	iInterfaceUnderflowAutoStopCtrl = static_cast<MMMFUnderflowAutoStopControl*> (iTarget->CustomInterface(KUidUnderflowAutoStopControl)); 

	if (!iInterfaceUnderflowAutoStopCtrl)
		{
		iInterfaceUnderflowAutoStopCtrl = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFUnderflowAutoStopControlDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFUnderflowAutoStopControlDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIUnderflowAutoStopCtrlTurnOffUnderflowAutoStop:
			{
			result = DoMmuascTurnOffUnderflowAutoStopL();

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
TInt CMMFUnderflowAutoStopControlDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	return KErrNone;
	}


void CMMFUnderflowAutoStopControlDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFUnderflowAutoStopControlDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


TInt CMMFUnderflowAutoStopControlDeMux::DoMmuascTurnOffUnderflowAutoStopL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceUnderflowAutoStopCtrl)
		{
		result = iInterfaceUnderflowAutoStopCtrl->MmuascTurnOffUnderflowAutoStop();
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceUnderflowAutoStopCtrlMux,		CMMFUnderflowAutoStopControlMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceUnderflowAutoStopCtrlDeMux,	CMMFUnderflowAutoStopControlDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

