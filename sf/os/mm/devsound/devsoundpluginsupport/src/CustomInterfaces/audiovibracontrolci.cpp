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

#include "audiovibracontrolci.h"


// MUX //

TInt CMMFAudioVibraControlMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceAudioVibraControlDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFAudioVibraControlMux::Release()
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


void CMMFAudioVibraControlMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFAudioVibraControlMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFAudioVibraControlMux::NewL()
	{
	CMMFAudioVibraControlMux* self = new (ELeave) CMMFAudioVibraControlMux;
	return self;
	}


TAny* CMMFAudioVibraControlMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MAudioVibraControl* interface = this;
	return interface;
	}


CMMFAudioVibraControlMux::CMMFAudioVibraControlMux() :
	iRemoteHandle(-1)
	{
	}


CMMFAudioVibraControlMux::~CMMFAudioVibraControlMux()
	{
	}


// from MAudioVibraControl
TInt CMMFAudioVibraControlMux::StartVibra()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIAudioVibraControlStartVibra,
												KNullDesC8);
		}

	return result;
	}


// from MAudioVibraControl
TInt CMMFAudioVibraControlMux::StopVibra()
	{
	TInt result = KErrBadHandle;

	if (iRemoteHandle > 0)
		{
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIAudioVibraControlStopVibra,
												KNullDesC8);
		}

	return result;
	}



// DEMUX //

TInt CMMFAudioVibraControlDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFAudioVibraControlDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFAudioVibraControlDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFAudioVibraControlDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFAudioVibraControlDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFAudioVibraControlDeMux::RefreshL()
	{
	// refetch the Audio vibra control custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceAudioVibraControl = static_cast <MAudioVibraControl*> (iTarget->CustomInterface(KUidAudioVibraControl));

		if (!iInterfaceAudioVibraControl)
			{
			iInterfaceAudioVibraControl = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFAudioVibraControlDeMux::NewL()
	{
	CMMFAudioVibraControlDeMux* self = new (ELeave) CMMFAudioVibraControlDeMux;
	return self;
	}


CMMFAudioVibraControlDeMux::CMMFAudioVibraControlDeMux()
	{
	}


CMMFAudioVibraControlDeMux::~CMMFAudioVibraControlDeMux()
	{
	}


TInt CMMFAudioVibraControlDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Audio vibra control Hw Device custom interface
	iInterfaceAudioVibraControl = static_cast<MAudioVibraControl*> (iTarget->CustomInterface(KUidAudioVibraControl)); 

	if (!iInterfaceAudioVibraControl)
		{
		iInterfaceAudioVibraControl = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFAudioVibraControlDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFAudioVibraControlDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrGeneral;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIAudioVibraControlStartVibra:
			{
			result = DoStartVibraL();
			break;
			}
		case EMMFDevSoundCIAudioVibraControlStopVibra:
			{
			result = DoStopVibraL();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


TInt CMMFAudioVibraControlDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	return KErrNone;
	}


void CMMFAudioVibraControlDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFAudioVibraControlDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


TInt CMMFAudioVibraControlDeMux::DoStartVibraL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceAudioVibraControl)
		{
		result = iInterfaceAudioVibraControl->StartVibra();
		}

	return result;
	}


TInt CMMFAudioVibraControlDeMux::DoStopVibraL()
	{
	TInt result = KErrNotFound;

	if (iInterfaceAudioVibraControl)
		{
		result = iInterfaceAudioVibraControl->StopVibra();
		}

	return result;
	}

//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceAudioVibraControlMux,		CMMFAudioVibraControlMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceAudioVibraControlDeMux,	CMMFAudioVibraControlDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
