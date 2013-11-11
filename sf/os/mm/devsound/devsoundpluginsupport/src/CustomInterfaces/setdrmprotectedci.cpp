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

#include "setdrmprotectedci.h"


// MUX //
/*****************************************************************************/

TInt CMMFSetDRMProtectedMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceSetDRMProtectedDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

/*****************************************************************************/
void CMMFSetDRMProtectedMux::Release()
	{
	// close the slave device if it exists
	if (iRemoteHandle > 0)
		{
		// we assume the slave is closed correctly
		iUtility->CloseSlave(iRemoteHandle);
		}
	
	TUid key = iDestructorKey;
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}

/*****************************************************************************/	
void CMMFSetDRMProtectedMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}

/*****************************************************************************/
void CMMFSetDRMProtectedMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFSetDRMProtectedMux::NewL()
	{
	CMMFSetDRMProtectedMux* self = new (ELeave) CMMFSetDRMProtectedMux;
	return self;
	}

/*****************************************************************************/	
TAny* CMMFSetDRMProtectedMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFSetDRMProtected* interface = this;
	return interface;
	}
	
/*****************************************************************************/
CMMFSetDRMProtectedMux::CMMFSetDRMProtectedMux() :
	iRemoteHandle(-1)
	{	
	}

/*****************************************************************************/
CMMFSetDRMProtectedMux::~CMMFSetDRMProtectedMux()
	{	
	}

/*****************************************************************************/
// from MMMFSetDRMProtected
TInt CMMFSetDRMProtectedMux::MmsdpMarkDataAsDRMProtected(TBool aDRMProtected)
	{
	if (iRemoteHandle > 0)
		{
		// send the DRM protected flag in the sync command
		TPckgBuf<TBool> flag(aDRMProtected);

		// any return code other than zero is an error
		return iUtility->SendSlaveSyncCommand(iRemoteHandle, EMMFDevSoundCISetDRMProtected, flag);
		}
	else
		{
		return KErrNotReady;
		}
	}



// DEMUX //	
/*****************************************************************************/
TInt CMMFSetDRMProtectedDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}

/*****************************************************************************/	
void CMMFSetDRMProtectedDeMux::Release()
	{
	TUid key = iDestructorKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
/*****************************************************************************/	
void CMMFSetDRMProtectedDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}
	
/*****************************************************************************/	
void CMMFSetDRMProtectedDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}

/*****************************************************************************/	
void CMMFSetDRMProtectedDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/
void CMMFSetDRMProtectedDeMux::RefreshL()
	{
	// refetch the DRM protected custom interface if we already have a target
	if (iTarget)
		{
		MMMFSetDRMProtected* ptr = NULL;

		ptr = static_cast <MMMFSetDRMProtected*> (iTarget->CustomInterface(KUidSetDRMProtected));
	
		if (!ptr)
			{
			iInterfaceSetDRMProtected = NULL;
			User::Leave(KErrNotSupported);
			}
		else
			{
			iInterfaceSetDRMProtected = ptr;
			}	
		}
	}

/*****************************************************************************/
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFSetDRMProtectedDeMux::NewL()
	{
	CMMFSetDRMProtectedDeMux* self = new (ELeave) CMMFSetDRMProtectedDeMux;
	return self;
	}

/*****************************************************************************/	
CMMFSetDRMProtectedDeMux::CMMFSetDRMProtectedDeMux()
	{	
	}

/*****************************************************************************/
CMMFSetDRMProtectedDeMux::~CMMFSetDRMProtectedDeMux()
	{
	}

/*****************************************************************************/
TInt CMMFSetDRMProtectedDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Playback status Hw Device custom interface
	MMMFSetDRMProtected* ptr = NULL;

	ptr = static_cast<MMMFSetDRMProtected*> (iTarget->CustomInterface(KUidSetDRMProtected)); 
	
	if (!ptr)
		{
		iInterfaceSetDRMProtected = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iInterfaceSetDRMProtected = ptr;
		}
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFSetDRMProtectedDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

/*****************************************************************************/
// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFSetDRMProtectedDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;

	TInt retVal = KErrNone;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCISetDRMProtected:
			{
			TPckgBuf<TBool> flag; 
			iUtility->ReadFromInputDesL(aMessage, &flag);
			retVal = DoMmsdpMarkDataAsDRMProtected(flag());
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return retVal;
	}
	
/*****************************************************************************/	
TInt CMMFSetDRMProtectedDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFSetDRMProtectedDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
/*****************************************************************************/	
void CMMFSetDRMProtectedDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

/*****************************************************************************/
// Set DRM protected custom interface implementation
TInt CMMFSetDRMProtectedDeMux::DoMmsdpMarkDataAsDRMProtected(TBool aDRMProtected)
	{
	if (!iInterfaceSetDRMProtected)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceSetDRMProtected->MmsdpMarkDataAsDRMProtected(aDRMProtected);
		}		
	}

/*****************************************************************************/
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSetDRMProtectedMux, CMMFSetDRMProtectedMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceSetDRMProtectedDeMux, CMMFSetDRMProtectedDeMux::NewL),
	};

/*****************************************************************************/
//
// ImplementationGroupProxy
//
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


