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

#include "sbcencodertestdevice.h"


/*****************************************************************************/
// Implementation

CMMFHwDevice* CSbcEncoderTestDevice::NewL()
	{
	CSbcEncoderTestDevice* self = new(ELeave) CSbcEncoderTestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*****************************************************************************/
CSbcEncoderTestDevice::~CSbcEncoderTestDevice()
	{
	}

/*****************************************************************************/
CSbcEncoderTestDevice::CSbcEncoderTestDevice()
	{
	}

/*****************************************************************************/
void CSbcEncoderTestDevice::ConstructL()
	{
	}
	
/*****************************************************************************/
TInt CSbcEncoderTestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}

/*****************************************************************************/
TInt CSbcEncoderTestDevice::Stop()
	{
	return 0;
	}
	
/*****************************************************************************/
TInt CSbcEncoderTestDevice::Pause()
	{
	return 0;
	}

/*****************************************************************************/		
TInt CSbcEncoderTestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TAny* CSbcEncoderTestDevice::CustomInterface(TUid aInterfaceId)
	{
	MSbcEncoderIntfc* interface = NULL;
	
	// DevSound initialisation requires something to be returned for both of
	// these two uids: KMmfPlaySettingsCustomInterface, KMmfRecordSettingsCustomInterface
	if ((aInterfaceId == KUidSbcEncoderIntfc) || // This interface
		(aInterfaceId.iUid == KMmfPlaySettingsCustomInterface) ||
		(aInterfaceId.iUid == KMmfRecordSettingsCustomInterface))
		{
		interface = this;
		}
	
	return interface;
	}

/*****************************************************************************/
TInt CSbcEncoderTestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSbcEncoderTestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSbcEncoderTestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSbcEncoderTestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
/*****************************************************************************/	
TInt CSbcEncoderTestDevice::DeleteCodec()
	{
	return 0;
	}

/*****************************************************************************/
CMMFSwCodec& CSbcEncoderTestDevice::Codec()
	{
	return *iCodec;
	}

/*****************************************************************************/
TInt CSbcEncoderTestDevice::GetSupportedSamplingFrequencies ( RArray<TUint>& aSamplingFrequencies )
	{
	TInt retVal = KErrNone;
	// Assume the strategy is to hard code a set of frequencies...
	//
	// Reset the input array
	aSamplingFrequencies.Reset();
	// Populate the array
	TUint value = 0;
	for (TInt i = 0; i < 10; i++)
		{
		retVal = aSamplingFrequencies.Append(value);
		if (retVal != KErrNone)
			{
			break;
			}
		value += 1000;
		}

	return retVal;
	}

TInt CSbcEncoderTestDevice::GetSupportedChannelModes ( RArray<TSbcChannelMode>& aChannelModes )
	{
	TInt retVal = KErrNone;
	aChannelModes.Reset();
	retVal = aChannelModes.Append(ESbcChannelMono);
	if (retVal == KErrNone)
		{
		retVal = aChannelModes.Append(ESbcChannelDual);
		if (retVal == KErrNone)
			{
			retVal = aChannelModes.Append(ESbcChannelStereo);
			if (retVal == KErrNone)
				{
				retVal = aChannelModes.Append(ESbcChannelJointStereo);
				}
			}				
		}
	return retVal;
	}

TInt CSbcEncoderTestDevice::GetSupportedNumOfSubbands ( RArray<TUint>& aNumOfSubbands )
	{
	TInt retVal = KErrNone;
	aNumOfSubbands.Reset();
	TUint val = 100;
	for (TInt i = 0; i < 5; i++)
		{
		retVal = aNumOfSubbands.Append(val);
		if (retVal != KErrNone)
			{
			break;
			}
		val += 100;
		}
	return retVal;
	}
 
TInt CSbcEncoderTestDevice::GetSupportedAllocationMethods ( RArray<TSbcAllocationMethod>& aAllocationMethods )
	{
	TInt retVal = KErrNone;
	aAllocationMethods.Reset();
	retVal = aAllocationMethods.Append(ESbcAllocationSNR);
	if (retVal == KErrNone)
		{
		retVal = aAllocationMethods.Append(ESbcAllocationLoudness);
		}
	return retVal;
	}

TInt CSbcEncoderTestDevice::GetSupportedNumOfBlocks ( RArray<TUint>& aNumOfBlocks )
	{
	TInt retVal = KErrNone;
	aNumOfBlocks.Reset();
	TUint val = 1;
	for (TInt i = 0; i < 10; i++, val++)
		{
		retVal = aNumOfBlocks.Append(val);
		if (retVal != KErrNone)
			{
			break;
			}
		}
	return retVal;
	}

TInt CSbcEncoderTestDevice::GetSupportedBitpoolRange (TUint& aMinSupportedBitpoolSize, TUint& aMaxSupportedBitpoolSize)
	{
	// Arbitrary values
	aMinSupportedBitpoolSize = 1;
	aMaxSupportedBitpoolSize = 2;
	
	return KErrNone;
	}

void CSbcEncoderTestDevice::SetSamplingFrequency ( TUint aSamplingFrequency )
	{
	iSamplingFrequency = aSamplingFrequency;
	}

void CSbcEncoderTestDevice::SetChannelMode (TSbcChannelMode aChannelMode )
	{
	iChannelMode = aChannelMode;
	}

void CSbcEncoderTestDevice::SetNumOfSubbands ( TUint aNumOfSubbands )
	{
	iNumOfSubbands = aNumOfSubbands;
	}

void CSbcEncoderTestDevice::SetAllocationMethod (TSbcAllocationMethod aAllocationMethod )
	{
	iAllocMethod = aAllocationMethod;
	}

void CSbcEncoderTestDevice::SetBitpoolSize (TUint aBitpoolSize )
	{
	iBitpoolSize = aBitpoolSize;
	}

TInt CSbcEncoderTestDevice::ApplyConfig()
	{
	iApplyConfig = ETrue;
	return KErrNone;
	}

TInt CSbcEncoderTestDevice::GetSamplingFrequency ( TUint& aSamplingFrequency )
	{
	TInt retVal = KErrNone;
	aSamplingFrequency = iSamplingFrequency;
	return retVal;
	}

TInt CSbcEncoderTestDevice::GetChannelMode (TSbcChannelMode& aChannelMode )
	{
	TInt retVal = KErrNone;
	aChannelMode = iChannelMode;	
	return retVal;
	}

TInt CSbcEncoderTestDevice::GetNumOfSubbands ( TUint& aNumOfSubbands )
	{
	TInt retVal = KErrNone;
	aNumOfSubbands = iNumOfSubbands;

	return retVal;
	}

TInt CSbcEncoderTestDevice::GetAllocationMethod (TSbcAllocationMethod& aAllocationMethod )
	{
	TInt retVal = KErrNone;
	aAllocationMethod = iAllocMethod;
	return retVal;
	}

TInt CSbcEncoderTestDevice::GetBitpoolSize (TUint& aBitpoolSize )
	{
	TInt retVal = KErrNone;
	aBitpoolSize = iBitpoolSize;
	return retVal;
	}

void CSbcEncoderTestDevice::SetNumOfBlocks ( TUint aNumOfBlocks )
	{
	iNumOfBlocks = aNumOfBlocks;
	}

TInt CSbcEncoderTestDevice::GetNumOfBlocks ( TUint& aNumOfBlocks )
	{
	TInt retVal = KErrNone;
	aNumOfBlocks = iNumOfBlocks;
	return retVal;
	}

