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

#include "teststepciaudiodspcontrol.h"


TVerdict CTestStepCIAudioDspControl::DoTestStepL()
	{
	iTestStepResult = ETestSuiteError;

	INFO_PRINTF1(_L("Initializing test CI device"));

	if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0076-HP")) == 0)
		{
		iTestStepResult = DoTestStep0076L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0077-HP")) == 0)
		{
		iTestStepResult = DoTestStep0077L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0078-HP")) == 0)
		{
		iTestStepResult = DoTestStep0078L();
		}
	else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUNDCI-U-0079-HP")) == 0)
		{
		iTestStepResult = DoTestStep0079L();
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIAudioDspControl::DoTestStep0076L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("AudioDSPControl - Instantiating"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAudioDspControlTestDevice};
	#else
	TFourCC testUID('T','0','1','3');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMFDSPControl
	MMMFDSPControl* ptr = static_cast <MMMFDSPControl*> (iMMFDevSound->CustomInterface(KUidDSPControl));

	if (ptr)
		{
		INFO_PRINTF1(_L("MMMFDSPControl instantiated successfully"));

		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF1(_L("MMMFDSPControl failed to retrieve the interface"));
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIAudioDspControl::DoTestStep0077L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("AudioDSPControl - MmdspcAcceptRecordBuffersInvalidFollowingStop"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAudioDspControlTestDevice};
	#else
	TFourCC testUID('T','0','1','3');
	#endif
	 
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMFDSPControl
	MMMFDSPControl* ptr = static_cast <MMMFDSPControl*> (iMMFDevSound->CustomInterface(KUidDSPControl));

	if (ptr)
		{
		TInt result = KErrGeneral;

		result = ptr->MmdspcAcceptRecordBuffersInvalidFollowingStop();

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFDSPControl::MmdspcAcceptRecordBuffersInvalidFollowingStop finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFDSPControl::MmdspcAcceptRecordBuffersInvalidFollowingStop failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFDSPControl failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIAudioDspControl::DoTestStep0078L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("AudioDSPControl - MmdspcAcceptPlaybackBuffersInvalidFollowingStop"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAudioDspControlTestDevice};
	#else
	TFourCC testUID('T','0','1','3');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMFDSPControl
	MMMFDSPControl* ptr = static_cast <MMMFDSPControl*> (iMMFDevSound->CustomInterface(KUidDSPControl));

	if (ptr)
		{
		TInt result = KErrGeneral;

		result = ptr->MmdspcAcceptPlaybackBuffersInvalidFollowingStop();

		if (result == KErrNone)
			{
			INFO_PRINTF1(_L("MMMFDSPControl::MmdspcAcceptPlaybackBuffersInvalidFollowingStop finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFDSPControl::MmdspcAcceptPlaybackBuffersInvalidFollowingStop failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFDSPControl failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}


TVerdict CTestStepCIAudioDspControl::DoTestStep0079L()
	{
	iTestStepResult = EFail;

	INFO_PRINTF1(_L("AudioDSPControl - MmdspcGetAudioPlaybackInfo"));

	//Initialize - with the UID of our test HwDevice
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	TUid testUID = {KUidAudioDspControlTestDevice};
	#else
	TFourCC testUID('T','0','1','3');
	#endif
	
	iTestStepResult = TestInitialize(testUID, EMMFStatePlaying);

	if (iTestStepResult != EPass)
		{
		INFO_PRINTF1(_L("DevSound failed to instantiate the test device"));
		return EInconclusive;
		}

	// reset the value as previous test is pass
	iTestStepResult = EFail;

	// KUidMMFDSPControl
	MMMFDSPControl* ptr = static_cast <MMMFDSPControl*> (iMMFDevSound->CustomInterface(KUidDSPControl));

	if (ptr)
		{
		TInt result = KErrGeneral;
		TInt64 samplesPlayed = 0;
		TInt64 systemTime = 0;
		TUint  bytesDecoderConsumed = 0;
		TUint  bytesDecoderDecoded = 0;

		result = ptr->MmdspcGetAudioPlaybackInfo (samplesPlayed, systemTime, bytesDecoderConsumed, bytesDecoderDecoded); // call method

		if ( (result == KErrNone) &&
			 (samplesPlayed == 999)	&& (systemTime == 999) &&
			 (bytesDecoderConsumed == 999) && (bytesDecoderDecoded == 999) )
			{
			INFO_PRINTF1(_L("MMMFDSPControl::MmdspcGetAudioPlaybackInfo finished successfully"));

			iTestStepResult = EPass;
			}
		else
			{
			ERR_PRINTF2(_L("MMMFDSPControl::MmdspcGetAudioPlaybackInfo failed with result %d"), result);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFDSPControl failed to retrieve the interface"));
		iTestStepResult = EInconclusive;
		}

	return iTestStepResult;
	}
