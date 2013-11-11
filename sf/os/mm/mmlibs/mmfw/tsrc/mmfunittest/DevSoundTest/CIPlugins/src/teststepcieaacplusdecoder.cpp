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

#include "teststepcieaacplusdecoder.h"

/*****************************************************************************/
CTestStepCIEAacPlusDecoder::CTestStepCIEAacPlusDecoder()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0008-HP");
	}

/*****************************************************************************/
TVerdict CTestStepCIEAacPlusDecoder::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test EAAC+ Decoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidEAacPlusDecoderTestDevice};
	#else
	TFourCC testUID('T','0','0','2');
	#endif
	
	MEAacPlusDecoderIntfc* interface = NULL;
	interface = static_cast<MEAacPlusDecoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidEAacPlusDecoderIntfc));

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

/*****************************************************************************/
CTestStepCIEAacPlusDecoderGetSetFrequency::CTestStepCIEAacPlusDecoderGetSetFrequency(TBool aIsGetTest) 
											: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0015-HP");		
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0009-HP");
		}
	}

TVerdict CTestStepCIEAacPlusDecoderGetSetFrequency::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test EAAC+ Decoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidEAacPlusDecoderTestDevice};
	#else
	TFourCC testUID('T','0','0','2');
	#endif
	
	MEAacPlusDecoderIntfc* interface = NULL;
	interface = static_cast<MEAacPlusDecoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidEAacPlusDecoderIntfc));
	
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the EAAC+ Decoder's Sampling Frequency"));
		TUint frequency = 666;
		interface->SetInputSamplingFrequency(frequency);
		
		TUint testFrequency;
		TInt err = interface->GetInputSamplingFrequency(testFrequency);
		if (err == KErrNone && testFrequency == frequency)
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
CTestStepCIEAacPlusDecoderGetSetObjType::CTestStepCIEAacPlusDecoderGetSetObjType(TBool aIsGetTest) 
										: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0016-HP");		
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0010-HP");
		}
	}

TVerdict CTestStepCIEAacPlusDecoderGetSetObjType::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test EAAC+ Decoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidEAacPlusDecoderTestDevice};
	#else
	TFourCC testUID('T','0','0','2');
	#endif
	
	MEAacPlusDecoderIntfc* interface = NULL;
	interface = static_cast<MEAacPlusDecoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidEAacPlusDecoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the EAAC+ Decoder's Object Type"));
		
		MEAacPlusDecoderIntfc::TAudioObjectType audioObjectType = MEAacPlusDecoderIntfc::EAacLtp;
		interface->SetAudioObjectType(audioObjectType);
		
		MEAacPlusDecoderIntfc::TAudioObjectType testAudioObjectType;
		interface->GetAudioObjectType(testAudioObjectType);
		if (audioObjectType == testAudioObjectType)
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got the object type."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the object type."));				
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get the object type"));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set the object type"));
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
CTestStepCIEAacPlusDecoderGetSetNumChannels::CTestStepCIEAacPlusDecoderGetSetNumChannels(TBool aIsGetTest) 
											: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0017-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0011-HP");
		}
	}

TVerdict CTestStepCIEAacPlusDecoderGetSetNumChannels::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test EAAC+ Decoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidEAacPlusDecoderTestDevice};
	#else
	TFourCC testUID('T','0','0','2');
	#endif
	
	MEAacPlusDecoderIntfc* interface = NULL;
	interface = static_cast<MEAacPlusDecoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidEAacPlusDecoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the EAAC+ Decoder's Number of Channels"));
		TUint numChannels = 2;
		interface->SetNumOfChannels(numChannels);
		
		TUint testNumChannels;
		interface->GetNumOfChannels(testNumChannels);
		if (testNumChannels == numChannels)
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got the number of channels."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the number of channels."));				
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get the number of channels"));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set the number of channels"));
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
CTestStepCIEAacPlusDecoderGetSetSbr::CTestStepCIEAacPlusDecoderGetSetSbr(TBool aIsGetTest) 
									: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0018-HP");		
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0012-HP");
		}
	}

TVerdict CTestStepCIEAacPlusDecoderGetSetSbr::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test EAAC+ Decoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidEAacPlusDecoderTestDevice};
	#else
	TFourCC testUID('T','0','0','2');
	#endif
	
	MEAacPlusDecoderIntfc* interface = NULL;
	interface = static_cast<MEAacPlusDecoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidEAacPlusDecoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the EAAC+ Decoder's SBR"));
		interface->SetSbr(ETrue);
		
		TBool testSbr = EFalse;
		interface->GetSbr(testSbr);
		if (testSbr)
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{			
				INFO_PRINTF1(_L("Successfully got SBR."));
				}
			else
				{				
				INFO_PRINTF1(_L("Successfully set SBR."));
				}
			}
		else
			{
			iTestStepResult = EFail;
			if (iIsGetTest)
				{
				ERR_PRINTF1(_L("Failed to get SBR."));
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set SBR."));	
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
CTestStepCIEAacPlusDecoderGetSetDsm::CTestStepCIEAacPlusDecoderGetSetDsm(TBool aIsGetTest) 
									: iIsGetTest(aIsGetTest)
	{
	if (iIsGetTest)
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0019-HP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0013-HP");
		}
	}

TVerdict CTestStepCIEAacPlusDecoderGetSetDsm::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test EAAC+ Decoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidEAacPlusDecoderTestDevice};
	#else
	TFourCC testUID('T','0','0','2');
	#endif
	
	MEAacPlusDecoderIntfc* interface = NULL;
	interface = static_cast<MEAacPlusDecoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidEAacPlusDecoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Setting the EAAC+ Decoder's Down Sampled Mode"));
		interface->SetDownSampledMode(ETrue);
		
		TBool testDSM = EFalse;
		interface->GetDownSampledMode(testDSM);
		if (testDSM)
			{
			iTestStepResult = EPass;
			if (iIsGetTest)
				{
				INFO_PRINTF1(_L("Successfully got the DSM."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully set the DSM."));
				}
			}
		else
			{
			iTestStepResult = EFail;
				ERR_PRINTF1(_L("Failed to set the DSM."));
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
CTestStepCIEAacPlusDecoderApplyConfig::CTestStepCIEAacPlusDecoderApplyConfig()
	{

	iTestStepName = _L("MM-MMF-DEVSOUNDCI-U-0014-HP");
	}

TVerdict CTestStepCIEAacPlusDecoderApplyConfig::DoTestStepL()
	{
	INFO_PRINTF1(_L("Initializing test EAAC+ Decoder CI device"));
	
	//Initialize - with the UID of our test HwDevice and try to get the interface
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidEAacPlusDecoderTestDevice};
	#else
	TFourCC testUID('T','0','0','2');
	#endif
	
	MEAacPlusDecoderIntfc* interface = NULL;
	interface = static_cast<MEAacPlusDecoderIntfc*>(InitializeAndInstantiateInterfaceL(testUID, KUidEAacPlusDecoderIntfc));
	if (interface)
		{
		INFO_PRINTF1(_L("Applying the config to the EAAC+ Decoder"));
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
