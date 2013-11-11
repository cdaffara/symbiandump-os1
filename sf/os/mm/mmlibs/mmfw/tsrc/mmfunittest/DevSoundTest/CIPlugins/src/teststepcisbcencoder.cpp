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

#include "teststepcisbcencoder.h"

/*****************************************************************************/
CTestStepCISbcEncoder::CTestStepCISbcEncoder()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0020-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCISbcEncoder::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
		
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));

	if (interface)
		{
		iTestStepResult = EPass;
		INFO_PRINTF1(_L("Successfully retrieved the interface."));
		}
	else
		{
		iTestStepResult = EFail;
		ERR_PRINTF1(_L("Failed to retrieve the interface"));
		}

	return iTestStepResult;
	}

/****************************************************************************/
CTestStepCISbcEncoderGetSupportedFrequencies::CTestStepCISbcEncoderGetSupportedFrequencies()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0021-HP");
	}

TVerdict CTestStepCISbcEncoderGetSupportedFrequencies::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Getting the SBC Encoder's Supported Sampling Frequencies"));
		
		RArray<TUint> frequencies;
		CleanupClosePushL(frequencies);
		TInt err = interface->GetSupportedSamplingFrequencies(frequencies);
		
		// Data is hard coded in the test HwDevice implementation
		// to add 10 values from 0 to 9000 in intervals of 1000.
		TBool validValues = ValidateArray(frequencies);
		if (err == KErrNone && validValues)
			{			
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully got the frequencies."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF1(_L("Failed to get the frequencies"));
			}
		CleanupStack::PopAndDestroy(); // frequencies
		}
	else
		{
		iTestStepResult = EInconclusive;
		INFO_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}


TBool CTestStepCISbcEncoderGetSupportedFrequencies::ValidateArray(const RArray<TUint>& aArray) const
	{
	TBool iTestStepResult = ETrue;
	
	TInt count = aArray.Count();
	if (count != 10)
		{
		iTestStepResult = EFalse;
		}
	else
		{
		TUint val = 0;
		for (TInt i = 0; i < count; i++)
			{
			if (aArray[i] != val)
				{
				iTestStepResult = EFalse;
				break;
				}
			val += 1000;
			}
		}
		
	return iTestStepResult;
	}

/****************************************************************************/
CTestStepCISbcEncoderGetSupportedSubbands::CTestStepCISbcEncoderGetSupportedSubbands()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0023-HP");
	}

TVerdict CTestStepCISbcEncoderGetSupportedSubbands::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Getting the SBC Encoder's Number Of Subbands"));

		RArray<TUint> subbands;
		CleanupClosePushL(subbands);
		TInt err = interface->GetSupportedNumOfSubbands(subbands);
	
		// Data is hard coded in the test HwDevice implementation
		// to add 5 values from 100 to 500 in intervals of 100.
		TBool validValues = ValidateArray(subbands);
		if (err == KErrNone && validValues)
			{				
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully got the subbands."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF1(_L("Failed to get the subbands"));
			}
		CleanupStack::PopAndDestroy(); // subbands
		}
	else
		{
		iTestStepResult = EInconclusive;
		INFO_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}


TBool CTestStepCISbcEncoderGetSupportedSubbands::ValidateArray(const RArray<TUint>& aArray) const
	{
	TBool iTestStepResult = ETrue;
	
	TInt count = aArray.Count();
	if (count != 5)
		{
		iTestStepResult = EFalse;
		}
	else
		{
		TUint val = 100;
		for (TInt i = 0; i < count; i++)
			{
			if (aArray[i] != val)
				{
				iTestStepResult = EFalse;
				break;
				}
			val += 100;
			}
		}
		
	return iTestStepResult;
	}

/****************************************************************************/
CTestStepCISbcEncoderGetSupportedNumOfBlocks::CTestStepCISbcEncoderGetSupportedNumOfBlocks()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0025-HP");
	}

TVerdict CTestStepCISbcEncoderGetSupportedNumOfBlocks::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Getting the SBC Encoder's Number Of Supported Blocks"));
		
		RArray<TUint> blocks;
		CleanupClosePushL(blocks);
		TInt err = interface->GetSupportedNumOfBlocks(blocks);
		
		// Data is hard coded in the test HwDevice implementation
		// to add 10 values from 1 to 10
		TBool validValues = ValidateArray(blocks);
		if (err == KErrNone && validValues)
			{		
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully got the blocks."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF1(_L("Failed to get the blocks"));
			}
		CleanupStack::PopAndDestroy(); // blocks
		}
	else
		{
		iTestStepResult = EInconclusive;
		INFO_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}


TBool CTestStepCISbcEncoderGetSupportedNumOfBlocks::ValidateArray(const RArray<TUint>& aArray) const
	{
	TBool iTestStepResult = ETrue;
	
	TInt count = aArray.Count();
	if (count != 10)
		{
		iTestStepResult = EFalse;
		}
	else
		{
		TUint val = 1;
		for (TInt i = 0; i < count; i++)
			{
			if (aArray[i] != val)
				{
				iTestStepResult = EFalse;
				break;
				}
			val++;
			}
		}
		
	return iTestStepResult;
	}
	
/****************************************************************************/
CTestStepCISbcEncoderGetSupportedChannelModes::CTestStepCISbcEncoderGetSupportedChannelModes()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0022-HP");
	}

TVerdict CTestStepCISbcEncoderGetSupportedChannelModes::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Getting the SBC Encoder's Supported Channel Modes"));
		
		RArray<MSbcEncoderIntfc::TSbcChannelMode> modes;
		CleanupClosePushL(modes);
		TInt err = interface->GetSupportedChannelModes(modes);
				
		// Data is hard coded in the test HwDevice implementation
		// to add the 4 available values
		TBool validValues = ValidateArray(modes);
		if (err == KErrNone && validValues)
			{
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully got the supported modes."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF1(_L("Failed to get the supported modes"));
			}
		CleanupStack::PopAndDestroy(); // modes
		}
	else
		{
		iTestStepResult = EInconclusive;
		INFO_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}


TBool CTestStepCISbcEncoderGetSupportedChannelModes::ValidateArray(const RArray<MSbcEncoderIntfc::TSbcChannelMode>& aArray) const
	{
	TBool iTestStepResult = ETrue;
	
	TInt count = aArray.Count();
	if (count != 4)
		{
		iTestStepResult = EFalse;
		}
	else
		{
        if ((aArray[0] != MSbcEncoderIntfc::ESbcChannelMono) ||
		    (aArray[1] != MSbcEncoderIntfc::ESbcChannelDual) ||
		    (aArray[2] != MSbcEncoderIntfc::ESbcChannelStereo) ||
		    (aArray[3] != MSbcEncoderIntfc::ESbcChannelJointStereo))
		    {
        	iTestStepResult = EFalse;
        	}
   		}
		
	return iTestStepResult;
	}
	
/****************************************************************************/
CTestStepCISbcEncoderGetSupportedAllocationMethods::CTestStepCISbcEncoderGetSupportedAllocationMethods()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0024-HP");
	}

TVerdict CTestStepCISbcEncoderGetSupportedAllocationMethods::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Getting the SBC Encoder's Supported Allocation Methods"));
		
		RArray<MSbcEncoderIntfc::TSbcAllocationMethod> allocMethods;
		CleanupClosePushL(allocMethods);
		TInt err = interface->GetSupportedAllocationMethods(allocMethods);

		// Data is hard coded in the test HwDevice implementation
		// to add the 2 available allocation methods
		TBool validValues = ValidateArray(allocMethods);
		if (err == KErrNone && validValues)
			{
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully got the allocation methods."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF1(_L("Failed to get the allocation methods"));
			}
		CleanupStack::PopAndDestroy(); // allocMethods
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}


TBool CTestStepCISbcEncoderGetSupportedAllocationMethods::ValidateArray(const RArray<
																MSbcEncoderIntfc::TSbcAllocationMethod>&
																aArray) const
	{
	TBool iTestStepResult = ETrue;
	
	TInt count = aArray.Count();
	if (count != 2)
		{
		iTestStepResult = EFalse;
		}
	else
		{
        if ((aArray[0] != MSbcEncoderIntfc::ESbcAllocationSNR) ||
		    (aArray[1] != MSbcEncoderIntfc::ESbcAllocationLoudness))
		    {
        	iTestStepResult = EFalse;
        	}
		}
		
	return iTestStepResult;
	}

/*****************************************************************************/
CTestStepCISbcEncoderGetSupportedBitpoolRange::CTestStepCISbcEncoderGetSupportedBitpoolRange()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0026-HP");
	}

TVerdict CTestStepCISbcEncoderGetSupportedBitpoolRange::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Getting the SBC Encoder's Supported Bitpool Range"));
		TUint minSize = 0;
		TUint maxSize = 0;
		TInt err = interface->GetSupportedBitpoolRange(minSize, maxSize);

		// Hard coded min and max values in the test device
		if ((err == KErrNone) &&
			(minSize == 1) &&
			(maxSize == 2))
			{
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully got the bitpool range."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF1(_L("Failed to get the bitpool range"));
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}

/*****************************************************************************/
CTestStepCISbcEncoderGetSetFrequency::CTestStepCISbcEncoderGetSetFrequency(TBool aIsGetTest)
								: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0034-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0027-HP");		
		}
	}

TVerdict CTestStepCISbcEncoderGetSetFrequency::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the SBC Encoder's Sampling Frequency"));
		
		TUint frequency = 666;
		interface->SetSamplingFrequency(frequency);
		
		TUint testFreq = 0;
		TInt err = interface->GetSamplingFrequency(testFreq);
		if (err == KErrNone && (frequency == testFreq))
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got the frequency."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the frequency."));			
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get the frequency"));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set the frequency"));
				}
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}
	
/*****************************************************************************/
CTestStepCISbcEncoderGetSetChannelMode::CTestStepCISbcEncoderGetSetChannelMode(TBool aIsGetTest)
										: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0035-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0028-HP");		
		}
	}

TVerdict CTestStepCISbcEncoderGetSetChannelMode::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the SBC Encoder's Channel Mode"));
		MSbcEncoderIntfc::TSbcChannelMode channelMode = MSbcEncoderIntfc::ESbcChannelStereo;
		interface->SetChannelMode(channelMode);
		
		MSbcEncoderIntfc::TSbcChannelMode testChannelMode = MSbcEncoderIntfc::ESbcChannelMono;
		TInt err = interface->GetChannelMode(testChannelMode);
		if ((err == KErrNone) && (channelMode == testChannelMode))
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got the channel mode."));	
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the channel mode."));	
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get channel mode"));	
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set channel mode"));	
				}
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}

/*****************************************************************************/
CTestStepCISbcEncoderGetSetNumOfSubbands::CTestStepCISbcEncoderGetSetNumOfSubbands(TBool aIsGetTest)
										: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0036-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0029-HP");
		}
	}

TVerdict CTestStepCISbcEncoderGetSetNumOfSubbands::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the SBC Encoder's Number of Subbands"));
		TUint numOfSubbands = 6;
		interface->SetNumOfSubbands(numOfSubbands);
		
		TUint testSubbands = 0;
		TInt err = interface->GetNumOfSubbands(testSubbands);
		if (err == KErrNone && (testSubbands == numOfSubbands))
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got subbands."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set subbands."));
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get subbands."));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set subbands."));
				}
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}
	
/*****************************************************************************/
CTestStepCISbcEncoderGetSetNumOfBlocks::CTestStepCISbcEncoderGetSetNumOfBlocks(TBool aIsGetTest)
										: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0037-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0030-HP");
		}
	}

TVerdict CTestStepCISbcEncoderGetSetNumOfBlocks::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the SBC Encoder's Number of Blocks"));
		TUint numOfBlocks = 6;
		interface->SetNumOfBlocks(numOfBlocks);
		TUint testBlocks = 0;
		TInt err = interface->GetNumOfBlocks(testBlocks);
		if ((err == KErrNone) && (numOfBlocks == testBlocks))
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got the number of blocks."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the number of blocks."));
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get the number of blocks."));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set the number of blocks."));
				}
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}
	
/*****************************************************************************/
CTestStepCISbcEncoderGetSetAllocationMethod::CTestStepCISbcEncoderGetSetAllocationMethod(TBool aIsGetTest)
										: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0038-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0031-HP");
		}
	}

TVerdict CTestStepCISbcEncoderGetSetAllocationMethod::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the SBC Encoder's Allocation Method"));
		MSbcEncoderIntfc::TSbcAllocationMethod allocationMethod = MSbcEncoderIntfc::ESbcAllocationLoudness;
		interface->SetAllocationMethod(allocationMethod);
		
		MSbcEncoderIntfc::TSbcAllocationMethod testAllocationMethod;
		TInt err = interface->GetAllocationMethod(testAllocationMethod);
		if ((err == KErrNone) && (allocationMethod == testAllocationMethod))
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got the allocation method."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the allocation method."));
				}			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get the allocation method."));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set the allocation method."));
				}
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}
	
/*****************************************************************************/
CTestStepCISbcEncoderGetSetBitpoolSize::CTestStepCISbcEncoderGetSetBitpoolSize(TBool aIsGetTest)
										: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0039-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0032-HP");
		}
	}

TVerdict CTestStepCISbcEncoderGetSetBitpoolSize::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the SBC Encoder's Bitpool Size"));
		TUint bitpoolSize = 6;
		interface->SetBitpoolSize(bitpoolSize);
		
		TUint testBitpoolSize = 0;
		TInt err = interface->GetBitpoolSize(testBitpoolSize);
		if ((err == KErrNone) && (bitpoolSize == testBitpoolSize))
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{			
				INFO_PRINTF1(_L("Successfully got the bitpool size."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the bitpool size."));
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get the bitpool size."));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set the bitpool size."));		
				}
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}

	return iTestStepResult;
	}

/*****************************************************************************/
CTestStepCISbcEncoderApplyConfig::CTestStepCISbcEncoderApplyConfig()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0033-HP");
	}

TVerdict CTestStepCISbcEncoderApplyConfig::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test SBC Encoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidSbcEncoderTestDevice};
	#else
	TFourCC testUID('T','0','0','3');
	#endif
	
 	MSbcEncoderIntfc* interface = NULL;
	interface = static_cast<MSbcEncoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidSbcEncoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Applying the config to the SBC Encoder"));
		TInt err = interface->ApplyConfig();
		if (err == KErrNone)
			{
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("Successfully applied the config."));
			}
		else
			{
			iTestStepResult = EFail;
			ERR_PRINTF2(_L("Failed to apply the config, error: %d"), err);
			}
		}
	else
		{
		iTestStepResult = EInconclusive;
		ERR_PRINTF1(_L("Failed to retrieve the interface."));
		}
	return iTestStepResult;
	}
