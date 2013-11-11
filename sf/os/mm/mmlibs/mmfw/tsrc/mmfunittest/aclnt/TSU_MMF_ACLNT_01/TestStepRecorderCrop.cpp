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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
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

//#include <MdaAudioSamplePlayer.h>
#include <mdaaudiosampleeditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorder.h"


#include "MmfAudioController.h"
#include "mmfclientaudiorecorder.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
// --------------------------------------------

//const TInt KPcm16FrameInterval = 371512;	// EABI warning removal

// frame size is now variable, depeding on the sample rate
//const TInt KFrameSize8K16Bit = 4096;	// frame size for 8Khz	// EABI warning removal

//if frame size is 4096 bytes, then a sample file of 8KHz, 16bit, mono, 
// would be 16000 bytes per second, therefore a frame would represent 256 millisecs
// we make sure the deviation is not greater than a frame since we can only crop frames.
const TInt KExpectedDeviation	 = (256000); //200000




// Orig duration = 1000120 Us, 90% = 900108 Us; @8KHz, 16bits, 1 channel = 14400 bytes (aligned to sample)
const TInt KExpectedCropSize1	 = 14400 + 44;

// Second crop - original duration is still taken from source (known error) = 10000120 Us.
//Therefore crop point = 40% of 10000120 Us = 400048 Us.
//Crop from begining to 400048Us with @8KHz, 16bits, 1 channel = 6400 bytes (aligned to sample).
//New file = KExpectedCropSize1 - 6400 = 8046
const TInt KExpectedCropSize2	 = 8000 + 44;



/**
 *
 * Static constructor for CTestStepRecorderCrop.
 *
 *
 * @return	"CTestStepRecorderCrop*"
 *			The constructed CTestStepRecorderCrop
 *
 * @xxxx
 * 
 */
CTestStepRecorderCrop* CTestStepRecorderCrop::NewL(const TDesC& aTestName, TBool aIsConverterTest)
	{
	CTestStepRecorderCrop* self = new(ELeave) CTestStepRecorderCrop(aTestName, aIsConverterTest);
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
CTestStepRecorderCrop::CTestStepRecorderCrop(const TDesC& aTestName, TBool aIsConverterTest)
: iError(0), iHasCropped(EFalse), iIsConverterTest(aIsConverterTest)
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
CTestStepRecorderCrop::~CTestStepRecorderCrop()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderCrop::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	if (!iIsConverterTest)
		INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::CropL(beginning/end)"));
	else
		INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::CropL(beginning/end)"));

	// open the file to be cropped
	
	
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL(iFs);
	if (iIsConverterTest)
		{
		if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvertAndCrop"), iFileName))  //au
			return EInconclusive;

		iFileNameTmp = iFileName;
		_LIT(KExtTmp,".tmp.au");
		iFileNameTmp.Append(KExtTmp);

		//create a copy of the input file to work on
		User::LeaveIfError(iFileMan->Copy(iFileName, iFileNameTmp));

		//construct an autput filename
		iOutputFileName = iFileName;
		_LIT(KExtOutput,".output.wav");
		iOutputFileName.Append(KExtOutput);

		//delete any existing output file
		TInt deleteErr = iFileMan->Delete(iOutputFileName); //delete oput file before start test
		if(deleteErr != KErrNone && deleteErr != KErrNotFound)
			User::Leave(deleteErr);	
		}
	else
		{
		if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToCrop"), iFileName))
			return EInconclusive;

		iFileNameTmp = iFileName;
		_LIT(KExtTmp,".tmp.wav");
		iFileNameTmp.Append(KExtTmp);

		User::LeaveIfError(iFileMan->Copy(iFileName, iFileNameTmp));
		}


	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;
	User::After(200000); // just in case

	if (!iIsConverterTest)
		{//0240
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			verdict = EInconclusive;

		iRecorder->OpenFileL(iFileNameTmp);
		CActiveScheduler::Start(); 
		if (iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;

		iRecorder->SetGain(iRecorder->MaxGain()/2);
		iOrigDuration = iRecorder->Duration().Int64();
		
		if ( iError != KErrNone || 
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			verdict = EInconclusive;
		}
	else
		{//566
		if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
			verdict = EInconclusive;

		iConvert->OpenL(iFileNameTmp, iOutputFileName);
		// Another initialisation of the convert utility
		// should behave the same as the one above
		//TMdaFileClipLocation* target = new TMdaFileClipLocation(iFileName3);
		//TMdaAuClipFormat auClipFormat;
		//TMdaPcm16BitAuCodec au16bitCodec; //KMMFFourCCCodePCM16B
		//iConvert->OpenL(iFileNameTmp, target, 
		//				&auClipFormat,
		//				&au16bitCodec);

		CActiveScheduler::Start(); 
		if (iError != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			return EInconclusive;

		iOrigDuration = iConvert->Duration().Int64();
		
		if ( iError != KErrNone || 
			iConvert->State() != CMdaAudioConvertUtility::EOpen)
			verdict = EInconclusive;

		// convert because cropping only applies to destination
		// before conversion destination is empty -> hence no cropping available
		TRAPD(err, iConvert->ConvertL());
		// Note: Converting by just openning a wav and an au file will work incorrectly:
		// the resutling file will contain noise, since the default conversion will be e.g.
		// wav:PCM16 -> au:PCM16, instead of wav:PCM16 -> au:PCM16BE (Big Endian)
		// Cropping, duration, position and the flow of the tested code is fine though, 
		// so it serves the purpose of this test, i.e. cropping
		// This error might be fixed in the future so Big Endian codec is chosen
		CActiveScheduler::Start(); // open -> record
		if (err != KErrNone || iError != KErrNone)
			return EInconclusive;
		CActiveScheduler::Start(); // record -> open
		if (err != KErrNone || iError != KErrNone)
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
enum TVerdict CTestStepRecorderCrop::DoTestStepPostambleL(void)
	{
	if (!iIsConverterTest)
		iRecorder->Close();
	else
		iConvert->Close();

	//delete the temp, cropped file 
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL(iFs);
	User::LeaveIfError(iFileMan->Delete(iFileNameTmp));

	if (iIsConverterTest)
		User::LeaveIfError(iFileMan->Delete(iOutputFileName));

	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;

	if (!iIsConverterTest)
		{//240
		delete iRecorder;
		iRecorder = NULL;
		}
	else
		{//566
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
void CTestStepRecorderCrop::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderCrop::CropAndStartSchedulerL(const TTimeIntervalMicroSeconds& aBegin,
												   const TTimeIntervalMicroSeconds& aEnd) 
	{
	if (!iIsConverterTest)
		{
		iRecorder->CropL(aBegin, aEnd);
		//CActiveScheduler::Start(); // it works synchronously even if we don't start the ActiveScheduler


		//Check if the cropped duration is within the deviation from the expected duration
		TInt64 cropDur = iRecorder->Duration().Int64();
		TInt64 difDur = aEnd.Int64() - aBegin.Int64();
		TInt delta = I64INT(cropDur - difDur);
		if(delta < 0)
			delta *= (-1);
		
		if ( delta <= KExpectedDeviation )
			iHasCropped = ETrue;
		//iRecorder->Stop();
		}
	else // not really used in this test
		{
		}
	}
	
void CTestStepRecorderCrop::CropAndStartSchedulerL(TTimeIntervalMicroSeconds aCropPoint, TBool aCropToEnd) 
	{
	_LIT(_toEnd,"to end from");
	_LIT(_fromBegining,"from beginning to");

	if (!iIsConverterTest)
		{//240
		iRecorder->SetPosition(aCropPoint);
		const TTimeIntervalMicroSeconds pos = iRecorder->Position();

		iRecorder->CropL(aCropToEnd);
		//CActiveScheduler::Start(); // it works synchronously even if we don't start the ActiveScheduler


		//Check if the cropped duration is within the deviation from the expected duration
		TInt64 cropDur = iRecorder->Duration().Int64();
		TInt64 difDur = 0;
		if (aCropToEnd)
			difDur = pos.Int64();
		else
			difDur = iOrigDuration - pos.Int64();
		TInt delta = I64INT(cropDur - difDur);
		if(delta < 0)
			delta *= (-1);
		
		if ( delta <= KExpectedDeviation )
			iHasCropped = ETrue;
		
		
		INFO_PRINTF2(_L("Cropping from original duration = %d"), iOrigDuration);
		INFO_PRINTF3(_L("Cropping %S %d "), (aCropToEnd ? &_toEnd : &_fromBegining), aCropPoint.Int64());
		INFO_PRINTF2(_L("Crop point was set at = %d"), pos.Int64());
		INFO_PRINTF2(_L("Cropped to = %d"), cropDur);
		//iRecorder->Stop();
		}
	else
		{
		iHasCropped = ETrue;

		iConvert->SetPosition(aCropPoint);

		if (aCropToEnd)
			iConvert->CropL(); // default: crop to end
		else
			iConvert->CropFromBeginningL();
		//CActiveScheduler::Start(); // it works synchronously even if we don't start the ActiveScheduler


		//Check if the cropped duration is within the deviation from the expected duration
		// Note: there is a problem with duration, since Duration() always returns the 
		// duration of the original clip (source clip) before conversion
		// One can set the position beyond the end of the cropped file, but still below the
		// original duration before cropping, and start a second cropping. This might lead
		// to unexpected behaviour.
		//TInt64 cropDur = iConvert->Duration().Int64();
		// there is no converter API to check the destination(sink) duration after cropping

		// we are just checking if the cropping took place by checking the size of the
		// cropped file
		TInt croppedSize = 0;

		User::LeaveIfError(iFs.Connect());
		RFile croppedFile;
		User::LeaveIfError(croppedFile.Open(iFs, iOutputFileName, EFileRead ));
		CleanupClosePushL(croppedFile);
		User::LeaveIfError(croppedFile.Size(croppedSize));

		TInt expectedCrop=0;

		if (aCropToEnd)
			{
			expectedCrop=KExpectedCropSize1;
			if (croppedSize != expectedCrop)
			iHasCropped = EFalse;
			}
		else // second crop to beginning
			{
			expectedCrop=KExpectedCropSize2;
			if (croppedSize != expectedCrop) 
				iHasCropped = EFalse;
			}

		INFO_PRINTF2(_L("Cropping from original duration = %d"), iOrigDuration);
		INFO_PRINTF3(_L("Cropping %S %d "), (aCropToEnd ? &_toEnd : &_fromBegining), aCropPoint.Int64());
		INFO_PRINTF3(_L("Cropped to = %d bytes (expected %d bytes)"), croppedSize,expectedCrop);

		CleanupStack::PopAndDestroy(&croppedFile);
		iFs.Close();
		}
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
TVerdict CTestStepRecorderCrop::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	//NB: crop points are carfully choosen to ensure that at least one block is removed 
	//during each crop operation.

	if (!iIsConverterTest)
		{//240
		//crop 40% from beginning of clip
		TRAP( err, CropAndStartSchedulerL(iOrigDuration / 5 *2, EFalse) );

		if (err != KErrNone ||
			!iHasCropped ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
			return EFail;

		iHasCropped = EFalse;
		iOrigDuration = iRecorder->Duration().Int64();
		//crop frop 80% to end of file
		TRAP( err, CropAndStartSchedulerL(iOrigDuration - (iOrigDuration/5), ETrue) );

		if (err != KErrNone ||
			!iHasCropped ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
			{
			INFO_PRINTF4(_L("Failed err=%d  iHasCropped=%d  State=%d"),err, iHasCropped,  iRecorder->State());
			return EFail;
			}
		}
	else // iIsConverterTest == ETRue
		{//566
		//crop from 90% to end of file
		TRAP( err, CropAndStartSchedulerL(iOrigDuration - (iOrigDuration/10), ETrue) );

		if (err != KErrNone ||
			!iHasCropped ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen )
			{
			INFO_PRINTF4(_L("Failed err=%d  iHasCropped=%d  State=%d"),err, iHasCropped,  iConvert->State());
			return EFail;
			}

		// crop again
		iHasCropped = EFalse;
		iOrigDuration = iConvert->Duration().Int64();
		
		//crop 40% from beginning of clip
		TRAP( err, CropAndStartSchedulerL(iOrigDuration / 5 * 2, EFalse) ); // crop from beginning

		if (err != KErrNone ||
			!iHasCropped ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen )
			{
			INFO_PRINTF4(_L("Failed err=%d  iHasCropped=%d  State=%d"),err, iHasCropped,  iConvert->State());
			return EFail;
			}
		}
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// add new test here for descriptor crop
//

/**
 *
 * Static constructor for CTestStepRecCropDescriptor
 *
 *
 * @return	"CTestStepRecCropDescriptor*"
 *			The constructed CTestStepRecCropDescriptor
 *
 * @xxxx
 * 
 */
CTestStepRecCropDescriptor* CTestStepRecCropDescriptor::NewL(const TDesC& aTestName, 	TBool aUseOldApi )
	{
	CTestStepRecCropDescriptor* self = new(ELeave) CTestStepRecCropDescriptor(aTestName, aUseOldApi);
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
CTestStepRecCropDescriptor::CTestStepRecCropDescriptor(const TDesC& aTestName, 	TBool aUseOldApi ) 
: iError(0), iHasCropped(EFalse), iIsOldAPiTest( aUseOldApi )
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
CTestStepRecCropDescriptor::~CTestStepRecCropDescriptor()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecCropDescriptor::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict = EPass;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);

	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::CropL(beginning/end)"));
	
	// open the file to be cropped

	if(!(GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToCrop"), iFileName)))
		{
		return EInconclusive;
		}
	
	// copy file to descriptor
	iFileNameTmp = iFileName;

	//[ append to file name and copy file to tmp file ]
	_LIT(KExtTmp,".tmp.wav");
	iFileNameTmp.Append(KExtTmp);

    iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Copy(iFileName, iFileNameTmp);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;


	//[ read the file into a descriptor ]
	ReadFileToDescriptorL( iFileNameTmp);
	
	delete iFileMan;
	iFileMan = NULL;

	User::After(200000); // just in case
	
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;
	
	//[ Open the descriptor]
    //[ set up the tdes8 here and every thing should be kosher]

	if( iIsOldAPiTest ) 
		{
		iRecorder->OpenDesL( *iDes8 );
		}
	else 
		{
		//[ since TDes8 is derived from TDesC8 we can do this cast
		// to force the correct api to be used ]
		iRecorder->OpenDesL( (const TDesC8&)(*iDes8)  );
		}


	CActiveScheduler::Start(); 
	if (iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;
	
	iRecorder->SetGain(iRecorder->MaxGain()/2);
	iOrigDuration = iRecorder->Duration().Int64();
	
	if ( iError != KErrNone || 
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		verdict = EInconclusive;

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecCropDescriptor::DoTestStepPostambleL(void)
	{
	iRecorder->Close();

	//delete the temp, cropped file 
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileNameTmp);

	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;

	delete iRecorder;
	iRecorder = NULL;

	delete iDescHBuf;
	delete iDes8;
		
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
void CTestStepRecCropDescriptor::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecCropDescriptor::CropAndStartSchedulerL(const TTimeIntervalMicroSeconds& aBegin,
												   const TTimeIntervalMicroSeconds& aEnd) 
	{
		iRecorder->CropL(aBegin, aEnd);
	
		//Check if the cropped duration is within the deviation from the expected duration
		TInt64 cropDur = iRecorder->Duration().Int64();
		TInt64 difDur = aEnd.Int64() - aBegin.Int64();
		TInt delta = I64INT(cropDur - difDur);
		if(delta < 0)
			delta *= (-1);
		
		if ( delta <= KExpectedDeviation )
			iHasCropped = ETrue;

	}
	
void CTestStepRecCropDescriptor::CropAndStartSchedulerL(TBool aCropToEnd) 
	{
	const TTimeIntervalMicroSeconds pos = iRecorder->Position();
	
	iRecorder->CropL(aCropToEnd);
	
	//Check if the cropped duration is within the deviation from the expected duration
	TInt64 cropDur = iRecorder->Duration().Int64();
	TInt64 difDur = 0;
	if (aCropToEnd)
		difDur = pos.Int64();
	else
		difDur = iOrigDuration - pos.Int64();
	TInt delta = I64INT(cropDur - difDur);
	if(delta < 0)
		delta *= (-1);
	
	if ( delta <= KExpectedDeviation )
		iHasCropped = ETrue;

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
TVerdict CTestStepRecCropDescriptor::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	const TTimeIntervalMicroSeconds middle( iOrigDuration / 2 );
	const TTimeIntervalMicroSeconds threeQuarters( iOrigDuration - (iOrigDuration/4) );

    iRecorder->SetPosition(threeQuarters);
	TRAP( err, CropAndStartSchedulerL(ETrue) );
	
	if (err != KErrNone ||
		!iHasCropped ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		return EFail;
	
	iHasCropped = EFalse;
	iOrigDuration = iRecorder->Duration().Int64();
	iRecorder->SetPosition(middle);
	
	CropAndStartSchedulerL(EFalse);
	
	if (err != KErrNone ||
		!iHasCropped ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen )
		return EFail;
		
	INFO_PRINTF1(_L("finished with this test step"));

	return iTestStepResult;
	}

/**
 * This function reads from a source file into the member variable heap buffer, 'iDescHBuf'.
 * @param aFilename - The file to read from.
 */
void CTestStepRecCropDescriptor::ReadFileToDescriptorL(const TDesC& aFilename )
	{	
	RFs rfs;
	rfs.Connect();
	RFile file;

	// Try to open the file.
	User::LeaveIfError( file.Open(rfs,aFilename,EFileRead|EFileShareAny) );

	// Create the databuffer in which to store the data.
	TInt fileSize = 0;
	file.Size(fileSize);
	iDescHBuf = HBufC8::NewL(fileSize);	
	
	TPtr8 dataBuf = iDescHBuf->Des();	

	// Read the data from the file to the data buffer
	User::LeaveIfError( file.Read(dataBuf) );	

	//[ set up the descriptor ]
	iDes8 = new (ELeave) TPtr8( NULL, 0 );
	iDes8->Set( iDescHBuf->Des());
    
	file.Close();
	rfs.Close();	
	}


