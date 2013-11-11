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

#include "eaacplusdecodertestdevice.h"


/*****************************************************************************/
// Implementation

CMMFHwDevice* CEAacPlusDecoderTestDevice::NewL()
	{
	CEAacPlusDecoderTestDevice* self = new(ELeave) CEAacPlusDecoderTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*****************************************************************************/
CEAacPlusDecoderTestDevice::~CEAacPlusDecoderTestDevice()
	{
	}

/*****************************************************************************/
CEAacPlusDecoderTestDevice::CEAacPlusDecoderTestDevice()
	{
	}

/*****************************************************************************/
void CEAacPlusDecoderTestDevice::ConstructL()
	{
	}
	
/*****************************************************************************/
TInt CEAacPlusDecoderTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

/*****************************************************************************/
TInt CEAacPlusDecoderTestDevice::Stop()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CEAacPlusDecoderTestDevice::Pause()
	{
	return 0;
	}

/*****************************************************************************/		
TInt CEAacPlusDecoderTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
/*****************************************************************************/
TAny* CEAacPlusDecoderTestDevice::CustomInterface(TUid aInterfaceId)
	{
	MEAacPlusDecoderIntfc* interface = NULL;
	
	// DevSound initialisation requires something to be returned for both of
	// these two uids: KMmfPlaySettingsCustomInterface, KMmfRecordSettingsCustomInterface
	if ((aInterfaceId == KUidEAacPlusDecoderIntfc) || // This interface
		(aInterfaceId.iUid == KMmfPlaySettingsCustomInterface) ||
		(aInterfaceId.iUid == KMmfRecordSettingsCustomInterface))
		{
		interface = this;
		}
	
	return interface;	
	}

/*****************************************************************************/
TInt CEAacPlusDecoderTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CEAacPlusDecoderTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CEAacPlusDecoderTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CEAacPlusDecoderTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CEAacPlusDecoderTestDevice::DeleteCodec()
	{
	return 0;
	}

/*****************************************************************************/
CMMFSwCodec& CEAacPlusDecoderTestDevice::Codec()
	{
	return *iCodec;
	}

/*****************************************************************************/

void CEAacPlusDecoderTestDevice::SetInputSamplingFrequency(TUint aInputSamplingFrequency)
	{
	iFrequency = aInputSamplingFrequency;
	}
	
void CEAacPlusDecoderTestDevice::SetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType aAudioObjectType)
	{
	iAudioObjectType = aAudioObjectType;
	}
	
void CEAacPlusDecoderTestDevice::SetNumOfChannels(TUint aNumOfChannels)
	{
	iNumOfChannels = aNumOfChannels;
	}
	
void CEAacPlusDecoderTestDevice::SetSbr(TBool aSbrEnabled)
	{
	iSbrEnabled = aSbrEnabled;
	}
	
void CEAacPlusDecoderTestDevice::SetDownSampledMode(TBool aDsmEnabled)
	{
	iDsmEnabled = aDsmEnabled;
	}
	
TInt CEAacPlusDecoderTestDevice::ApplyConfig()
	{
	iApplyConfig = ETrue;
	return KErrNone;
	}
	
TInt CEAacPlusDecoderTestDevice::GetInputSamplingFrequency ( TUint& aInputSamplingFrequency )
	{
	aInputSamplingFrequency = iFrequency;
	return KErrNone;
	}
	
TInt CEAacPlusDecoderTestDevice::GetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType& aAudioObjectType)
	{
	aAudioObjectType = iAudioObjectType;
	return KErrNone;
	}
	
TInt CEAacPlusDecoderTestDevice::GetNumOfChannels ( TUint& aNumOfChannels )
	{
	aNumOfChannels = iNumOfChannels;
	return KErrNone;
	}
	
TInt CEAacPlusDecoderTestDevice::GetSbr (TBool& aSbrEnabled)
	{
	aSbrEnabled = iSbrEnabled;
	return KErrNone;
	}
	
TInt CEAacPlusDecoderTestDevice::GetDownSampledMode (TBool& aDsmEnabled)
	{
	aDsmEnabled = iDsmEnabled;
	return KErrNone;
	}
