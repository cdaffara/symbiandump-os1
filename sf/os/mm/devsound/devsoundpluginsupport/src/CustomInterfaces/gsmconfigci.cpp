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

#include "gsmconfigci.h"


// MUX //

TInt CMMFGsmConfigMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceGsmConfigDeMux};

	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}

	return iRemoteHandle;
	}


void CMMFGsmConfigMux::Release()
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


void CMMFGsmConfigMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFGsmConfigMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


MMMFDevSoundCustomInterfaceMuxPlugin* CMMFGsmConfigMux::NewL()
	{
	CMMFGsmConfigMux* self = new (ELeave) CMMFGsmConfigMux;
	return self;
	}


TAny* CMMFGsmConfigMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFGsmConfig* interface = this;
	return interface;
	}


CMMFGsmConfigMux::CMMFGsmConfigMux() :
iRemoteHandle(-1)
	{
	}


CMMFGsmConfigMux::~CMMFGsmConfigMux()
	{
	}


// from MMMFGsmConfig
TInt CMMFGsmConfigMux::SetConversionFormat(TMMFGsmConversionFormat aConvFormat)
	{
	TInt result = KErrGeneral;

	if (iRemoteHandle > 0)
		{
		// send the convFormat in the sync command
		TPckgBuf<TMMFGsmConversionFormat> convFormat(aConvFormat);

		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle,
												EMMFDevSoundCIGsmConfigSetConversionFormat,
												convFormat);
		}

	return result;
	}


// from MMMFGsmConfig
TInt CMMFGsmConfigMux::ConversionFormat(TMMFGsmConversionFormat& aConvFormat) const
	{
	TInt result = KErrGeneral;

	if (iRemoteHandle > 0)
		{
		// send the retConvFormat in the sync command
		TPckgBuf<TMMFGsmConversionFormat> retConvFormat;

		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle,
													  EMMFDevSoundCIGsmConfigGetConversionFormat,
													  KNullDesC8,
													  retConvFormat);

		// assign return values to aConvFormat. Do nothing if there is an error
		if(result == KErrNone)
			{
			aConvFormat = retConvFormat();
			}
		}

	return result;
	}



// DEMUX //

TInt CMMFGsmConfigDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}


void CMMFGsmConfigDeMux::Release()
	{
	TUid key = iKey;

	delete this;

	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}


void CMMFGsmConfigDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iKey = aDestructorKey;
	}


void CMMFGsmConfigDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}


void CMMFGsmConfigDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}


void CMMFGsmConfigDeMux::RefreshL()
	{
	// refetch the Gsm config custom interface if we already have a target
	if (iTarget)
		{
		iInterfaceGsmConfig = static_cast <MMMFGsmConfig*> (iTarget->CustomInterface(KUidGsmConfig));

		if (!iInterfaceGsmConfig)
			{
			iInterfaceGsmConfig = NULL;
			User::Leave(KErrNotSupported);
			}
		}
	}


MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFGsmConfigDeMux::NewL()
	{
	CMMFGsmConfigDeMux* self = new (ELeave) CMMFGsmConfigDeMux;
	return self;
	}


CMMFGsmConfigDeMux::CMMFGsmConfigDeMux()
	{
	}


CMMFGsmConfigDeMux::~CMMFGsmConfigDeMux()
	{
	}


TInt CMMFGsmConfigDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Gsm Config Hw Device custom interface
	iInterfaceGsmConfig = static_cast<MMMFGsmConfig*> (iTarget->CustomInterface(KUidGsmConfig)); 

	if (!iInterfaceGsmConfig)
		{
		iInterfaceGsmConfig = NULL;
		User::Leave(KErrNotSupported);
		}

	return KErrNone;
	}


void CMMFGsmConfigDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}


// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFGsmConfigDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNotSupported;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIGsmConfigSetConversionFormat:
			{
			TPckgBuf<MMMFGsmConfig::TMMFGsmConversionFormat> convFormat; 
			iUtility->ReadFromInputDesL(aMessage, &convFormat);

			result = DoSetConversionFormatL(convFormat());

			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	return result;
	}


TInt CMMFGsmConfigDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundCIMessageData data;
	TInt result = KErrNotSupported;

	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);

	switch (data.iCommand)
		{
		case EMMFDevSoundCIGsmConfigGetConversionFormat:
			{
			TPckgBuf<MMMFGsmConfig::TMMFGsmConversionFormat> convFormat; 
			iUtility->ReadFromInputDesL(aMessage, &convFormat);
			
			result = DoConversionFormatL(convFormat());

			TPckgBuf<TBool> des(convFormat());
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


void CMMFGsmConfigDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


void CMMFGsmConfigDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}


// Gsm Config custom interface implementation
TInt CMMFGsmConfigDeMux::DoSetConversionFormatL(MMMFGsmConfig::TMMFGsmConversionFormat aConvFormat)
	{
	TInt result = KErrNotFound;

	if (iInterfaceGsmConfig)
		{
		result = iInterfaceGsmConfig->SetConversionFormat(aConvFormat);
		}

	return result;
	}


// Gsm Config custom interface implementation
TInt CMMFGsmConfigDeMux::DoConversionFormatL(MMMFGsmConfig::TMMFGsmConversionFormat& aConvFormat) const
	{
	TInt result = KErrNotFound;

	if (iInterfaceGsmConfig)
		{
		result = iInterfaceGsmConfig->ConversionFormat(aConvFormat);
		}

	return result;
	}


//
// ImplementationTable
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceGsmConfigMux,		CMMFGsmConfigMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceGsmConfigDeMux,	CMMFGsmConfigDeMux::NewL),
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
