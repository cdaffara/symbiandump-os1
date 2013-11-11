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

#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#include "advancedaacencodesettingstestdevice.h"
#include "cidefine.h"

/*****************************************************************************/
// Implementation

CMMFHwDevice* CAdvancedAacEncodeSettingsTestDevice::NewL()
	{
	CAdvancedAacEncodeSettingsTestDevice* self = new(ELeave) CAdvancedAacEncodeSettingsTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*****************************************************************************/
CAdvancedAacEncodeSettingsTestDevice::~CAdvancedAacEncodeSettingsTestDevice()
	{
	}

/*****************************************************************************/
CAdvancedAacEncodeSettingsTestDevice::CAdvancedAacEncodeSettingsTestDevice()
	{
	}

/*****************************************************************************/
void CAdvancedAacEncodeSettingsTestDevice::ConstructL()
	{
	}
	
/*****************************************************************************/
TInt CAdvancedAacEncodeSettingsTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

/*****************************************************************************/
TInt CAdvancedAacEncodeSettingsTestDevice::Stop()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CAdvancedAacEncodeSettingsTestDevice::Pause()
	{
	return 0;
	}

/*****************************************************************************/		
TInt CAdvancedAacEncodeSettingsTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TAny* CAdvancedAacEncodeSettingsTestDevice::CustomInterface(TUid aInterfaceId)
	{
	MMMFAdvancedAACEncodeSettings* interface = NULL;

	// DevSound initialisation requires something to be returned for both of
	// these two uids: KMmfPlaySettingsCustomInterface, KMmfRecordSettingsCustomInterface
	if ((aInterfaceId == KUidAdvancedAACEncoderSettings) || // This interface
		(aInterfaceId.iUid == KMmfPlaySettingsCustomInterface) ||
		(aInterfaceId.iUid == KMmfRecordSettingsCustomInterface))
		{
		interface = this;
		}

	return interface;
	}

/*****************************************************************************/
TInt CAdvancedAacEncodeSettingsTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CAdvancedAacEncodeSettingsTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CAdvancedAacEncodeSettingsTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CAdvancedAacEncodeSettingsTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CAdvancedAacEncodeSettingsTestDevice::DeleteCodec()
	{
	return 0;
	}

/*****************************************************************************/
CMMFSwCodec& CAdvancedAacEncodeSettingsTestDevice::Codec()
	{
	return *iCodec;
	}

/*****************************************************************************/
TInt CAdvancedAacEncodeSettingsTestDevice::SetAACEncodeBitrateMode(TAACBitrateMode aBitrateMode)
	{
	iBitrateMode = aBitrateMode;
	return KErrNone;
	}
	
TInt CAdvancedAacEncodeSettingsTestDevice::GetAACEncodeBitrateMode(TAACBitrateMode& aBitrateMode)
	{
	aBitrateMode = iBitrateMode;
	return KErrNone;
	}
