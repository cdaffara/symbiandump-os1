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

#include "advancedaacencodesettingsci.h"


// MUX //
/*****************************************************************************/

TInt CMMFAdvancedAACEncodeSettingsMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	// attempt to open the interface link with the
	// remote slave device
	iRemoteHandle = -1;
	TUid slaveId = {KMmfUidCustomInterfaceAdvancedAACEncodeSettingsDeMux};
		
	TInt handle = iUtility->OpenSlave(slaveId, KNullDesC8);
	if (handle >= 0)
		{
		iRemoteHandle = handle;
		}
		
	return iRemoteHandle;
	}

/*****************************************************************************/
void CMMFAdvancedAACEncodeSettingsMux::Release()
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
void CMMFAdvancedAACEncodeSettingsMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}

/*****************************************************************************/
void CMMFAdvancedAACEncodeSettingsMux::CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/	
MMMFDevSoundCustomInterfaceMuxPlugin* CMMFAdvancedAACEncodeSettingsMux::NewL()
	{
	CMMFAdvancedAACEncodeSettingsMux* self = new (ELeave) CMMFAdvancedAACEncodeSettingsMux;
	return self;
	}

/*****************************************************************************/	
TAny* CMMFAdvancedAACEncodeSettingsMux::CustomInterface(TUid /*aInterfaceId*/)
	{
	MMMFAdvancedAACEncodeSettings* interface = this;
	return interface;
	}
	
/*****************************************************************************/
CMMFAdvancedAACEncodeSettingsMux::CMMFAdvancedAACEncodeSettingsMux() :
	iRemoteHandle(-1)
	{	
	}

/*****************************************************************************/
CMMFAdvancedAACEncodeSettingsMux::~CMMFAdvancedAACEncodeSettingsMux()
	{	
	}

/*****************************************************************************/
// from MMMFAdvancedAACEncodeSettings
TInt CMMFAdvancedAACEncodeSettingsMux::SetAACEncodeBitrateMode(TAACBitrateMode aBitrateMode)
	{
	TInt result = KErrNotReady;
	
	if (iRemoteHandle > 0)
		{
		// send the bitrate mode in the sync command
		TPckgBuf<TAACBitrateMode> mode(aBitrateMode);
		
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommand(iRemoteHandle, 
												EMMFDevSoundCIAdvancedAACEncodeSettings, 
												mode);
		}
	return result;
	}

TInt CMMFAdvancedAACEncodeSettingsMux::GetAACEncodeBitrateMode(TAACBitrateMode& aBitrateMode)
	{
	TInt result = KErrNotReady;
	
	if (iRemoteHandle > 0)
		{
		// send the bitrate mode in the sync command
		TPckgBuf<TAACBitrateMode> mode;//(aBitrateMode);
		
		// any return code other than zero is an error
		result = iUtility->SendSlaveSyncCommandResult(iRemoteHandle, 
													EMMFDevSoundCIAdvancedAACEncodeGetSettings, 
													KNullDesC8,
													mode);
		if (result == KErrNone)
			{
			aBitrateMode = mode();
			}
		}
	return result;
	}

// DEMUX //	
/*****************************************************************************/
TInt CMMFAdvancedAACEncodeSettingsDeMux::OpenInterface(TUid /*aInterfaceId*/)
	{
	return KErrNone;
	}

/*****************************************************************************/	
void CMMFAdvancedAACEncodeSettingsDeMux::Release()
	{
	TUid key = iDestructorKey;
	
	delete this;
	
	// tell ECom to destroy us
	REComSession::DestroyedImplementation(key);
	}
	
/*****************************************************************************/	
void CMMFAdvancedAACEncodeSettingsDeMux::PassDestructorKey(TUid aDestructorKey)
	{
	// store the destructor key
	iDestructorKey = aDestructorKey;
	}
	
/*****************************************************************************/	
void CMMFAdvancedAACEncodeSettingsDeMux::SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget)
	{
	iTarget = aTarget;
	}

/*****************************************************************************/	
void CMMFAdvancedAACEncodeSettingsDeMux::CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aCustomUtility)
	{
	// store a pointer to the utility
	iUtility = aCustomUtility;
	}

/*****************************************************************************/
void CMMFAdvancedAACEncodeSettingsDeMux::RefreshL()
	{
	// refetch the custom interface if we already have a target
	if (iTarget)
		{
		MMMFAdvancedAACEncodeSettings* ptr = NULL;

		ptr = static_cast <MMMFAdvancedAACEncodeSettings*> (iTarget->CustomInterface(KUidAdvancedAACEncoderSettings));
	
		if (!ptr)
			{
			iInterfaceAdvAACEncodeSettings = NULL;
			User::Leave(KErrNotSupported);
			}
		else
			{
			iInterfaceAdvAACEncodeSettings = ptr;
			}	
		}
	}

/*****************************************************************************/
MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFAdvancedAACEncodeSettingsDeMux::NewL()
	{
	CMMFAdvancedAACEncodeSettingsDeMux* self = new (ELeave) CMMFAdvancedAACEncodeSettingsDeMux;
	return self;
	}

/*****************************************************************************/	
CMMFAdvancedAACEncodeSettingsDeMux::CMMFAdvancedAACEncodeSettingsDeMux()
	{	
	}

/*****************************************************************************/
CMMFAdvancedAACEncodeSettingsDeMux::~CMMFAdvancedAACEncodeSettingsDeMux()
	{
	}

/*****************************************************************************/
TInt CMMFAdvancedAACEncodeSettingsDeMux::DoOpenSlaveL(TUid /*aInterface*/, const TDesC8& /*aPackageBuf*/)
	{
	// fetch the Hw Device custom interface
	MMMFAdvancedAACEncodeSettings* ptr = NULL;

	ptr = static_cast<MMMFAdvancedAACEncodeSettings*> (iTarget->CustomInterface(KUidAdvancedAACEncoderSettings)); 
	
	if (!ptr)
		{
		iInterfaceAdvAACEncodeSettings = NULL;
		User::Leave(KErrNotSupported);
		}
	else
		{
		iInterfaceAdvAACEncodeSettings = ptr;
		}
	return KErrNone;
	}
	
/*****************************************************************************/	
void CMMFAdvancedAACEncodeSettingsDeMux::DoCloseSlaveL(TInt /*aHandle*/)
	{
	// nothing to do
	}

/*****************************************************************************/
// original RMessage is supplied so that remote demux plugin can extract necessary details
// using DeMux utility
TInt CMMFAdvancedAACEncodeSettingsDeMux::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	TInt retVal = KErrNone;
	
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIAdvancedAACEncodeSettings:
			{
			TPckgBuf<MMMFAdvancedAACEncodeSettings::TAACBitrateMode> mode; 
			iUtility->ReadFromInputDesL(aMessage, &mode);

			retVal = DoSetAACEncodeBitrateMode(mode());

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
TInt CMMFAdvancedAACEncodeSettingsDeMux::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	TInt retVal = KErrNone;
	TMMFDevSoundCIMessageData data;
	
	// decode message
	iUtility->GetSyncMessageDataL(aMessage, data);
	
	switch (data.iCommand)
		{
		case EMMFDevSoundCIAdvancedAACEncodeGetSettings:
			{
			MMMFAdvancedAACEncodeSettings::TAACBitrateMode mode; 
			retVal = DoGetAACEncodeBitrateMode(mode);
			
			if (retVal == KErrNone)
				{
				TPckgBuf<MMMFAdvancedAACEncodeSettings::TAACBitrateMode> pckg(mode);
				iUtility->WriteToOutputDesL(aMessage, pckg);
				}

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
void CMMFAdvancedAACEncodeSettingsDeMux::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}
	
/*****************************************************************************/	
void CMMFAdvancedAACEncodeSettingsDeMux::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& /*aMessage*/)
	{
	// not used in this interface
	}

/*****************************************************************************/
// Play Back Status custom interface implementation
TInt CMMFAdvancedAACEncodeSettingsDeMux::DoSetAACEncodeBitrateMode(MMMFAdvancedAACEncodeSettings::
																	TAACBitrateMode aBitrateMode)
	{
	if (!iInterfaceAdvAACEncodeSettings)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceAdvAACEncodeSettings->SetAACEncodeBitrateMode(aBitrateMode);
		}		
	}

TInt CMMFAdvancedAACEncodeSettingsDeMux::DoGetAACEncodeBitrateMode(MMMFAdvancedAACEncodeSettings::
																	TAACBitrateMode& aBitrateMode)
	{
	if (!iInterfaceAdvAACEncodeSettings)
		{
		return KErrNotReady;
		}
	else
		{
		return iInterfaceAdvAACEncodeSettings->GetAACEncodeBitrateMode(aBitrateMode);
		}		
	}

/*****************************************************************************/
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceAdvancedAACEncodeSettingsMux,	CMMFAdvancedAACEncodeSettingsMux::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCustomInterfaceAdvancedAACEncodeSettingsDeMux, CMMFAdvancedAACEncodeSettingsDeMux::NewL),
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


