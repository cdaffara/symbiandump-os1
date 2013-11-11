// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cmmfdevsoundcifileblocklengthimpluid.hrh"

#include <ecom/implementationproxy.h>
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <s32mem.h>

#include "cmmfdevsoundcifileblocklength.h"

// __________________________________________________________________________
// Implementation

// MUX //

TInt CMMFDevSoundCIFileBlockLengthMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCIFileBlockLengthDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

void CMMFDevSoundCIFileBlockLengthMux::Release()
	{
	// close the slave device if it exists
	if (iRemoteHandle != -1)
		{
		// we assume the slave is closed correctly
		iUtility->CloseSlave(iRemoteHandle);
		}
	
	TUid key = iKey;
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
void CMMFDevSoundCIFileBlockLengthMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}

void CMMFDevSoundCIFileBlockLengthMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}
	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFDevSoundCIFileBlockLengthMux::NewL()
	{
	CMMFDevSoundCIFileBlockLengthMux* self = new (ELeave) CMMFDevSoundCIFileBlockLengthMux;
	return self;
	}
	
TAny* CMMFDevSoundCIFileBlockLengthMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFDevSoundCustomInterfaceFileBlockLength* interface = this;
	return interface;
	}
	
CMMFDevSoundCIFileBlockLengthMux::CMMFDevSoundCIFileBlockLengthMux() :
	iRemoteHandle(-1)
	{	
	}

CMMFDevSoundCIFileBlockLengthMux::~CMMFDevSoundCIFileBlockLengthMux()
	{	
	}

// from MMMFDevSoundCustomInterfaceFileBlockLength

void CMMFDevSoundCIFileBlockLengthMux::SetFileBlockLength(TUint aBlockAlign)
	{
	if (iRemoteHandle == -1)
		{
		return;
		}
	
	// send the BlockAlign in the sync command
	TPckgBuf<TUint> blockAlignBuffer(aBlockAlign);
	
	iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCIBlockLengthSetFileBlockLength, blockAlignBuffer);
	}

// DEMUX //	
	

TInt CMMFDevSoundCIFileBlockLengthDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}
	
void CMMFDevSoundCIFileBlockLengthDeMux::Release()
	{
	TUid key = iKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
void CMMFDevSoundCIFileBlockLengthDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}
	
void CMMFDevSoundCIFileBlockLengthDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}
	
void CMMFDevSoundCIFileBlockLengthDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

void CMMFDevSoundCIFileBlockLengthDeMux::RefreshL()
	{
	// refetch the fileblocklength custom interface if we already have a target
	if (iTarget)
		{
		MMMFDevSoundCustomInterfaceFileBlockLength* ptr = NULL;
		ptr = static_cast<MMMFDevSoundCustomInterfaceFileBlockLength*>(iTarget->CustomInterface(KUidCustomInterfaceDevSoundFileBlockLength));
	
		if (!ptr)
			{
			iFileBlockLengthInterface = NULL;
			User::Leave(KErrNotSupported);
			}
		else
			{
			iFileBlockLengthInterface = ptr;
			}	
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFDevSoundCIFileBlockLengthDeMux::NewL()
	{
	CMMFDevSoundCIFileBlockLengthDeMux* self = new (ELeave) CMMFDevSoundCIFileBlockLengthDeMux;
	return self;
	}
	
CMMFDevSoundCIFileBlockLengthDeMux::CMMFDevSoundCIFileBlockLengthDeMux()
	{
	}

CMMFDevSoundCIFileBlockLengthDeMux::~CMMFDevSoundCIFileBlockLengthDeMux()
	{
	}


TInt CMMFDevSoundCIFileBlockLengthDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the fileblocklength custom interface
	MMMFDevSoundCustomInterfaceFileBlockLength* ptr = NULL;
	ptr = static_cast <MMMFDevSoundCustomInterfaceFileBlockLength*> (iTarget->CustomInterface(KUidCustomInterfaceDevSoundFileBlockLength));
	
	if (!ptr)
		{
		iFileBlockLengthInterface = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iFileBlockLengthInterface = ptr;
		}
	return KErrNone;
	}
	
void CMMFDevSoundCIFileBlockLengthDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFDevSoundCIFileBlockLengthDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	TInt retVal = -1;
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIBlockLengthSetFileBlockLength:
			{
			// we know that offset 2 contains a TInt
			TPckgBuf<TUint> blockAlignBuffer;
			iUtility->ReadFromInputDesL(aMessage, &blockAlignBuffer);
			
			DoSetFileBlockLengthL(blockAlignBuffer());
			retVal = KErrNone;
			break;
			}
	
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
		
	return retVal;
	}
	
TInt CMMFDevSoundCIFileBlockLengthDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	return KErrNotSupported;
	}
	
void CMMFDevSoundCIFileBlockLengthDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
void CMMFDevSoundCIFileBlockLengthDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

void CMMFDevSoundCIFileBlockLengthDeMux::DoSetFileBlockLengthL(TUint aBlockAlign)
	{
	if (!iFileBlockLengthInterface)
		{
		User::Leave(KErrNotReady);
		}
			
	// set the block length		
	iFileBlockLengthInterface->SetFileBlockLength(aBlockAlign);
	}



//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCIFileBlockLengthMux,	CMMFDevSoundCIFileBlockLengthMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCIFileBlockLengthDeMux, CMMFDevSoundCIFileBlockLengthDeMux::NewL),
	};


//
// ImplementationGroupProxy
//
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

#ifndef EKA2
/** DLL Entry point */
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return(KErrNone);
	}
#endif

