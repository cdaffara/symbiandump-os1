// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains test steps exercising the Audio recorder
// functions: xxxxxxxxDestination[DataType|BitRate|SampleRate|Format]x
// and xxxxxxNumberOfChannelsx 
// 
//

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

#include <mdaaudiosampleeditor.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorder.h"


#include "MmfAudioController.h"
#include "mmfclientaudiorecorder.h"
#include <mmf/common/mmffourcc.h>
//#include <mmf/common/mmfpaniccodes.h>
#include <mmf/plugin/mmfformatimplementationuids.hrh>


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderDestinDataType.
 *
 *
 * @return	"CTestStepRecorderDestinDataType*"
 *			The constructed CTestStepRecorderDestinDataType
 *
 * @xxxx
 * 
 */
CTestStepRecorderDestinDataType* CTestStepRecorderDestinDataType::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderDestinDataType* self = new(ELeave) CTestStepRecorderDestinDataType(aTestName, aIsConverterTest);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderDestinDataType::CTestStepRecorderDestinDataType(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderDestinDataType::~CTestStepRecorderDestinDataType()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderDestinDataType::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0264-HP"));
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::xxxDestinationDataTypexx() functions"));
		}
	else
		{
		INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0557-HP"));
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::xxxDestinationDataTypexx() functions"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord4"), iFileName2) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;
		TRAPD( err, OpenAndStartSchedulerL() );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		}
	else
		{
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;
		TRAPD( err, OpenAndStartSchedulerL() );

		if (iConvert == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;
		}

		
	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderDestinDataType::DoTestStepPostambleL(void)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName2);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderDestinDataType::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderDestinDataType::OpenAndStartSchedulerL()
	{
	if (!iIsConverterTest)
		iRecorder->OpenFileL( iFileName );
	else
		iConvert->OpenL( iFileName, iFileName3 );

	CActiveScheduler::Start(); 

	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderDestinDataType::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TFourCC fourCC;
	RArray<TFourCC> supFourCC;

	if (!iIsConverterTest)
		{
		// check the number of supported audio types
		// just checking that there are some types available
		// since it's not known how many types are supported each time
		TRAP(err, iRecorder->GetSupportedDestinationDataTypesL(supFourCC));
		if (err != KErrNone ||
			supFourCC.Count() <= 0 ||
			supFourCC.Find(TFourCC(KMMFFourCCCodeALAW)) < 0 ||
			supFourCC.Find(TFourCC(KMMFFourCCCodePCM16)) < 0 )
			return EFail;
		// get the type of the openned file of known type
		// and check if type correct
		TRAP(err, fourCC = iRecorder->DestinationDataTypeL());
		if (err != KErrNone ||
			fourCC != KMMFFourCCCodePCM16)
			return EFail;

		// close this file and
		iRecorder->Close();
		//open a new non-existing file for recording
		TRAP(err, iRecorder->OpenFileL(iFileName2));
		if (err != KErrNone ||
			iError != KErrNone)
			return EInconclusive;

		CActiveScheduler::Start();
		// set the destination type to a specific one of our choice
		TRAP(err, iRecorder->SetDestinationDataTypeL(KMMFFourCCCodeIMAD));
		if (err != KErrNone)
			return EFail;

		//get the newly set type
		//and check if it has been set correctly
		TRAP(err, fourCC = iRecorder->DestinationDataTypeL());
		if (err != KErrNone ||
			fourCC != KMMFFourCCCodeIMAD)
			return EFail;
		}
	else // iIsConverterTest
		{
		TRAP(err, iConvert->GetSupportedDestinationDataTypesL(supFourCC));
		if (err != KErrNone ||
			supFourCC.Count() <= 0 ||
			supFourCC.Find(TFourCC(KMMFFourCCCodeALAW)) < 0 ||
			supFourCC.Find(TFourCC(KMMFFourCCCodePCM8)) < 0 )
			return EFail;

		//TRAP(err, fourCC = iConvert->DestinationDataTypeL());
		//if (err != KErrNone ||
		//	fourCC != KMMFFourCCCodePCM16)
		//	return EFail;

		// set the destination type to a specific one of our choice
		TRAP(err, iConvert->SetDestinationDataTypeL(KMMFFourCCCodePCM8));
		if (err != KErrNone)
			return EFail;

		//get the newly set type
		//and check if it has been set correctly
		TRAP(err, fourCC = iConvert->DestinationDataTypeL());
		if (err != KErrNone ||
			fourCC != KMMFFourCCCodePCM8)
			return EFail;
		}


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderBitRate.
 *
 *
 * @return	"CTestStepRecorderBitRate*"
 *			The constructed CTestStepRecorderBitRate
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRate* CTestStepRecorderBitRate::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderBitRate* self = new(ELeave) CTestStepRecorderBitRate(aTestName, aIsConverterTest);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRate::CTestStepRecorderBitRate(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRate::~CTestStepRecorderBitRate()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderBitRate::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::xxxBitRatexx() functions"));
		}
	else
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::xxxBitRatexx() functions"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord4"), iFileName2) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	//delete files that should not be there
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName2);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		}
	else
		{
		// create the Converter utility
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iConvert == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;
		}
		
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderBitRate::DoTestStepPostambleL(void)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName2);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderBitRate::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderBitRate::OpenAndStartSchedulerL()
	{
	if (!iIsConverterTest)
		iRecorder->OpenFileL( iFileName );
	else
		iConvert->OpenL( iFileName, iFileName3 );

	CActiveScheduler::Start(); 
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderBitRate::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TUint bitrate = 0;
	RArray<TUint> supBitRates;

	if (!iIsConverterTest)
		{
		// get the type of the openned file of known bitrate
		// (wav file 8KHz x 16 bit = 128 Kbps) 
		// and check if type correct
		TRAP(err, bitrate = iRecorder->DestinationBitRateL());
		if (err != KErrNone ||
			bitrate != 128000)
			return EFail;

		// close this file and
		iRecorder->Close();

		//open a new non-existing file for recording
		TRAP(err, iRecorder->OpenFileL(iFileName2));
		if (err != KErrNone ||
			iError != KErrNone)
			return EInconclusive;

		CActiveScheduler::Start();
		if (iError != KErrNone)
			return EFail;

		// check the number of supported bitrates
		// just checking that there are some bitrates available
		TRAP(err, iRecorder->GetSupportedBitRatesL(supBitRates));
		if ( err != KErrNotSupported ) // KErrNotSupported expected
			{
			if (err != KErrNone ||
				supBitRates.Count() <= 0 )
				return EFail;
			}
		
		// set the destination bitrate to a specific one of our choice
		TRAP(err, iRecorder->SetDestinationBitRateL(64000));

		//NB KErrNotSupported is a valid response, the format used may not support this ability
		//If it is supported, read the value to confirm it was correctly set.
		if ( err != KErrNotSupported ) 
			{
			if (err != KErrNone)
				return EFail;

			//get the newly set type
			//and check if it has been set correctly
			TRAP(err, bitrate = iRecorder->DestinationBitRateL());
			if ( err != KErrNotSupported ) 
				{
				if (err != KErrNone ||
					bitrate != 64000)
					return EFail;
				}
			}
		TInt errorSetDestinationBitRate = err;

		//get the newly set type
		//and check if it has been set correctly
		TRAP(err, bitrate = iRecorder->DestinationBitRateL());
		if ( err != KErrNotSupported ) 
			{
			if (err != KErrNone ||
				(errorSetDestinationBitRate == KErrNone && bitrate != 64000))
				return EFail;
			}
		}
	else // IsConverterTest == ETrue
		{
		// get source file known bitrate
		// (wav file 8KHz x 16 bit = 128 Kbps) 
		// and check if type correct
		TRAP(err, bitrate = iConvert->SourceBitRateL());
		if (err != KErrNone ||
			bitrate != 128000)
			return EFail;

		// check the number of supported bitrates
		// just checking that there are some bitrates available
		TRAP(err, iConvert->GetSupportedConversionBitRatesL(supBitRates));
		if ( err != KErrNotSupported ) // KErrNotSupported expected
			{
			if (err != KErrNone ||
				supBitRates.Count() <= 0 )
				return EFail;
			}
		
		// set the destination bitrate to a specific one of our choice
		TRAP(err, iConvert->SetDestinationBitRateL(64000));

		//NB KErrNotSupported is a valid response, the format used may not support this ability
		//If it is supported, read the value to confirm it was correctly set.
		if ( err != KErrNotSupported ) 
			{
			if (err != KErrNone)
				return EFail;
			}
		TInt errSetDestinationBitRate = err;

		//get the newly set type
		//and check if it has been set correctly
		TRAP(err, bitrate = iConvert->DestinationBitRateL());
		if ( err != KErrNotSupported ) 
			{
			if ((err == KErrNone && errSetDestinationBitRate == KErrNone && bitrate != 64000) 
			||	(err != KErrNone))
				return EFail;
			}
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderSampleRate.
 *
 *
 * @return	"CTestStepRecorderSampleRate*"
 *			The constructed CTestStepRecorderSampleRate
 *
 * @xxxx
 * 
 */
CTestStepRecorderSampleRate* CTestStepRecorderSampleRate::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderSampleRate* self = new(ELeave) CTestStepRecorderSampleRate(aTestName, aIsConverterTest);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSampleRate::CTestStepRecorderSampleRate(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSampleRate::~CTestStepRecorderSampleRate()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSampleRate::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::xxxSampleRatexx() functions"));
		}
	else
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::xxxSampleRatexx() functions"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord4"), iFileName2) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		}
	else
		{
		// create the Converter utility
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iConvert == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;
		}
		
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSampleRate::DoTestStepPostambleL(void)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName2);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderSampleRate::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderSampleRate::OpenAndStartSchedulerL()
	{
	if (!iIsConverterTest)
		iRecorder->OpenFileL( iFileName );
	else
		iConvert->OpenL( iFileName, iFileName3 );

	CActiveScheduler::Start(); 
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderSampleRate::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TUint samplerate = 0;
	RArray<TUint> supSampleRates;

	if (!iIsConverterTest)
		{
		// get the type of the openned file of known bitrate
		// (wav file 8KHz) 
		// and check if type correct
		TRAP(err, samplerate = iRecorder->DestinationSampleRateL());
		if (err != KErrNone ||
			samplerate != 8000)
			{
			INFO_PRINTF3(_L("iRecorder->DestinationSampleRateL() test failed (err=%d, samplerate=%d)"),
						 err, samplerate);
			return EFail;
			}

		// close this file and
		iRecorder->Close();

		//open a new non-existing file for recording
		TRAP(err, iRecorder->OpenFileL(iFileName2));
		if (err != KErrNone ||
			iError != KErrNone)
			{
			INFO_PRINTF3(_L("iRecorder->OpenFileL() test failed (err=%d, iError=%d)"), err, iError);
			return EInconclusive;
			}
		CActiveScheduler::Start();

		// check the number of supported bitrates
		// just checking that there are some sample rates available
		TRAP(err, iRecorder->GetSupportedSampleRatesL(supSampleRates));
		if (err != KErrNone ||
			supSampleRates.Count() <= 0 )
			{
			INFO_PRINTF3(_L("iRecorder->GetSupportedSampleRatesL() test failed (err=%d, count=%d)"),
						 err, supSampleRates.Count());
			return EFail;
			}
		
		// set the destination samplerate to a specific one of our choice
		TRAP(err, iRecorder->SetDestinationSampleRateL(supSampleRates[0]) ); //4000));
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("iRecorder->SetDestinationSampleRateL() test failed (err=%d)"), err);
			return EFail;
			}

		//get the newly set sample rate
		//and check if it has been set correctly
		TRAP(err, samplerate = iRecorder->DestinationSampleRateL());
		if (err != KErrNone ||
			samplerate != supSampleRates[0])
			{
			INFO_PRINTF3(_L("iRecorder->DestinationSampleRateL() test failed (err=%d, samplerate=%d)"),
						 err, supSampleRates[0]);
			return EFail;
			}
		}
	else // isConverterTest == ETrue
		{
		// get the known sample rate of the source file 
		// (wav file 8KHz) 
		// and check if type correct
		TRAP(err, samplerate = iConvert->SourceSampleRateL());
		if (err != KErrNone ||
			samplerate != 8000)
			{
			INFO_PRINTF3(_L("iConvert->SourceSampleRateL() test failed (err=%d, samplerate=%d)"), err, samplerate);
			return EFail;
			}

		// check the number of supported bitrates
		// just checking that there are some sample rates available
		TRAP(err, iConvert->GetSupportedConversionSampleRatesL(supSampleRates));
		if (err != KErrNone ||
			supSampleRates.Count() <= 0 )
			{
			INFO_PRINTF3(_L("iConvert->GetSupportedConversionSampleRatesL() test failed (err=%d, count=%d)"),
						 err, supSampleRates.Count());
			return EFail;
			}
		
		// set the destination samplerate to a specific one of our choice
		TRAP(err, iConvert->SetDestinationSampleRateL(supSampleRates[0]) ); //4000));
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("iConvert->SetDestinationSampleRateL() test failed (err=%d)"), err);
			return EFail;
			}

		//get the newly set sample rate
		//and check if it has been set correctly
		TRAP(err, samplerate = iConvert->DestinationSampleRateL());
		if (err != KErrNone ||
			samplerate != supSampleRates[0])
			{
			INFO_PRINTF3(_L("iConvert->DestinationSampleRateL() test failed (err=%d, samplerate=%d)"),
						 err, supSampleRates[0]);
			return EFail;
			}
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderFormat.
 *
 *
 * @return	"CTestStepRecorderFormat*"
 *			The constructed CTestStepRecorderFormat
 *
 * @xxxx
 * 
 */
CTestStepRecorderFormat* CTestStepRecorderFormat::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderFormat* self = new(ELeave) CTestStepRecorderFormat(aTestName, aIsConverterTest);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderFormat::CTestStepRecorderFormat(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderFormat::~CTestStepRecorderFormat()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderFormat::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::xxxFormatxx() functions"));
		}
	else
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::xxxFormatxx() functions"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord5"), iFileName2) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		}
	else
		{
		// create the Converter utility
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iConvert == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;
		}
		
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderFormat::DoTestStepPostambleL(void)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName2);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderFormat::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderFormat::OpenAndStartSchedulerL()
	{
	if (!iIsConverterTest)
		iRecorder->OpenFileL( iFileName );
	else
		iConvert->OpenL( iFileName, iFileName3 );

	CActiveScheduler::Start(); 
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderFormat::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TUid format = TUid::Null();

	if (!iIsConverterTest)
		{
		// get the format of the openned file of known format
		// (wav file 8KHz) 
		// and check if type correct
		TRAP(err, format = iRecorder->DestinationFormatL());
		if (err != KErrNone ||
			format != TUid::Uid(KMmfUidFormatWAVWrite) )
			return EFail;

		// close this file and
		iRecorder->Close();

		//open a new non-existing file for recording
		TRAP(err, iRecorder->OpenFileL(iFileName2));
		if (err != KErrNone ||
			iError != KErrNone)
			return EInconclusive;
		CActiveScheduler::Start();
		// set the destination format to a specific one of our choice
		TRAP(err, iRecorder->SetDestinationFormatL(TUid::Uid(KMmfUidFormatAUWrite) ) );
		if (err != KErrNone)
			return EFail;

		//get the newly set format
		//and check if it has been set correctly
		TRAP(err, format = iRecorder->DestinationFormatL());
		if (err != KErrNone ||
			format != TUid::Uid(KMmfUidFormatAUWrite) )
			return EFail;
		}
	else // isConverterTest == ETrue
		{
		// get the format of the source file of known format
		// (wav file 8KHz) 
		// and check if type correct
		TRAP(err, format = iConvert->SourceFormatL());
		if (err != KErrNone ||
			format != TUid::Uid(KMmfUidFormatWAVRead) )
			return EFail;

		// set the destination format to a specific one of our choice
		TRAP(err, iConvert->SetDestinationFormatL(TUid::Uid(KMmfUidFormatAUWrite) ) );
		if (err != KErrNone)
			return EFail;

		//get the newly set format
		//and check if it has been set correctly
		TRAP(err, format = iConvert->DestinationFormatL());
		if (err != KErrNone ||
			format != TUid::Uid(KMmfUidFormatAUWrite) )
			return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// ----------CTestStepRecorderChannels----------
//
/**
 *
 * Static constructor for CTestStepRecorderChannels.
 *
 *
 * @return	"CTestStepRecorderChannels*"
 *			The constructed CTestStepRecorderChannels
 *
 * @xxxx
 * 
 */
CTestStepRecorderChannels* CTestStepRecorderChannels::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderChannels* self = new(ELeave) CTestStepRecorderChannels(aTestName, aIsConverterTest);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderChannels::CTestStepRecorderChannels(const TDesC& aTestName, TBool aIsConverterTest)
: iIsConverterTest(aIsConverterTest)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderChannels::~CTestStepRecorderChannels()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderChannels::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::xxxChannelsx() functions"));
		}
	else
		{
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::xxxChannelsx() functions"));
		}

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToRecord4"), iFileName2) && // wav
		 GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName3) // au
		)) 
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	if (!iIsConverterTest)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;
		}
	else
		{
		// create the Converter utility
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TInt err;
		TRAP( err, OpenAndStartSchedulerL() );

		if (iConvert == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;
		}
		
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderChannels::DoTestStepPostambleL(void)
	{
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName2);
	iFileMan->Delete(iFileName3);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	if (!iIsConverterTest)
		{
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{
		delete iConvert;
		iConvert = NULL;
		}

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderChannels::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderChannels::OpenAndStartSchedulerL()
	{
	if (!iIsConverterTest)
		iRecorder->OpenFileL( iFileName );
	else
		iConvert->OpenL( iFileName, iFileName3 );

	CActiveScheduler::Start(); 
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderChannels::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err       = KErrNone;
	TUint channels = 0;
	RArray<TUint> supChannels;

	if (!iIsConverterTest)
		{
		// get the number of channels of the openned file of known channels number
		// (wav mono file 8KHz) 
		// and check if channels correct
		TRAP(err, channels = iRecorder->DestinationNumberOfChannelsL());
		if (err != KErrNone ||
			channels != 1)
			return EFail;

		// close this file
		iRecorder->Close();

		//open a new non-existing file for recording
		TRAP(err, iRecorder->OpenFileL(iFileName2));
		if (err != KErrNone ||
			iError != KErrNone)
			return EInconclusive;

		CActiveScheduler::Start();
		// get the supported num of channels
		TRAP(err, iRecorder->GetSupportedNumberOfChannelsL(supChannels));
		if (err != KErrNone ||
			supChannels.Count() <= 0 )
			return EFail;
		
		// set the destination number of channels to a specific one of our choice
		TRAP(err, iRecorder->SetDestinationNumberOfChannelsL(2));
		if (err != KErrNone)
			return EFail;

		//get the newly set number of channels
		//and check if it has been set correctly
		TRAP(err, channels = iRecorder->DestinationNumberOfChannelsL());
		if (err != KErrNone ||
			channels != 2)
			return EFail;
		}
	else // isConverterTest == ETrue
		{
		// get the number of channels of the source file of known channels number
		// (wav mono file 8KHz) 
		// and check if channels correct
		TRAP(err, channels = iConvert->SourceNumberOfChannelsL());
		if (err != KErrNone ||
			channels != 1)
			return EFail;

		// get the supported num of channels
		TRAP(err, iConvert->GetSupportedConversionNumberOfChannelsL(supChannels));
		if (err != KErrNone ||
			supChannels.Count() <= 0 )
			return EFail;
		
		// set the destination number of channels to a specific one of our choice
		TRAP(err, iConvert->SetDestinationNumberOfChannelsL(2));
		if (err != KErrNone)
			return EFail;

		//get the newly set number of channels
		//and check if it has been set correctly
		TRAP(err, channels = iConvert->DestinationNumberOfChannelsL());
		if (err != KErrNone ||
			channels != 2)
			return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


