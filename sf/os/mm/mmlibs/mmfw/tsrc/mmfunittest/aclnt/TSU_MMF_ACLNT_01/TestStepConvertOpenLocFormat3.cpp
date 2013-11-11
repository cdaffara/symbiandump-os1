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

#include <mdaaudiosampleeditor.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepConvert.h"


#include "MmfAudioController.h"
#include "mmfclientaudioconverter.h"
#include <mmfcontrollerimplementationuids.hrh>
//#include <mmfFormatImplementationUIDs.hrh>
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepConvertOpenLocFormat3.
 *
 *
 * @return	"CTestStepConvertOpenLocFormat3*"
 *			The constructed CTestStepConvertOpenLocFormat3
 *
 * @xxxx
 * 
 */
CTestStepConvertOpenLocFormat3* CTestStepConvertOpenLocFormat3::NewL()
	{
	CTestStepConvertOpenLocFormat3* self = new(ELeave) CTestStepConvertOpenLocFormat3;
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
CTestStepConvertOpenLocFormat3::CTestStepConvertOpenLocFormat3() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0554-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertOpenLocFormat3::~CTestStepConvertOpenLocFormat3()
	{
	}

void CTestStepConvertOpenLocFormat3::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	//Tests for DEF032016
	if(aCurrentState == CMdaAudioClipUtility::ERecording)
		{
		iError = KErrGeneral;
		INFO_PRINTF1(_L("CTestStepConvertOpenLocFormat3::MoscoStateChangeEvent incorrect state == CMdaAudioClipUtility::ERecording"));
		}
	else
		iError = aErrorcCode;

	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertOpenLocFormat3::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	//INFO_PRINTF2(_L("%S"),iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::OpenL(location, location, controller, format, datatype) "));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName5"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert3"), iFileName2) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName8"), iFileNameWav1) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert4"), iFileNameWav2) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Convert utility
	if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;
	// create an empty target file
	iFs.Connect();
	if ( iFile.Replace(iFs, iFileName2, EFileWrite) )
		return EInconclusive;
	if ( iFile.SetSize(0) )
		return EInconclusive;

	if ( iFile.Replace(iFs, iFileNameWav2, EFileWrite) )
		return EInconclusive;
	if ( iFile.SetSize(0) )
		return EInconclusive;
	
	
	iFile.Close();
	iFs.Close();


	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertOpenLocFormat3::DoTestStepPostambleL(void)
	{
	//delete the converted file 
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFileMan->Delete(iFileName2);
	iFileMan->Delete(iFileNameWav2);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;

	delete iClipLocationSrc;
	delete iClipLocationTrgt;
	delete iConvert;
	iConvert = NULL;

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepConvertOpenLocFormat3::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
    
    TMdaAudioDataSettings iAudioSettings;
	iAudioSettings.iSampleRate = 8000;
    iAudioSettings.iChannels = 1;
  



	for(TInt conversion=0; conversion<2; conversion++)
		{
		if(conversion == 0)
			{
			iClipLocationSrc = new TMdaFileClipLocation(iFileName);
			iClipLocationTrgt = new TMdaFileClipLocation(iFileName2);

			
			TRAP(err,iConvert->OpenL(iClipLocationSrc,
						iClipLocationTrgt,
						&iRawClipFormat,
						&iRawClipFormat,
						&iRawAudioCodec,
						&iAudioSettings,
						&iRawAudio16Codec,
						&iAudioSettings ));
			}
		else
			{
			iClipLocationSrc = new TMdaFileClipLocation(iFileNameWav1);
			iClipLocationTrgt = new TMdaFileClipLocation(iFileNameWav2);

			TRAP(err,iConvert->OpenL(iClipLocationSrc,
						iClipLocationTrgt,
						&iWavClipFormat,
						&iWavClipFormat,
						&iWavAudioPcmCodec,
						&iAudioSettings,
						&iWavAudioulawCodec,
						&iAudioSettings ));
			}

		CActiveScheduler::Start();

		if (iConvert == NULL ||
			iError != KErrNone ||
			err != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen  )
			return EFail;

		iConvert->SetPosition(TTimeIntervalMicroSeconds(0));
		iConvert->ConvertL();
		CActiveScheduler::Start();
		if (iError!=KErrNone)
			return EFail;
		CActiveScheduler::Start();
		if (iError!=KErrNone)
			return EFail;


		delete iClipLocationSrc; iClipLocationSrc=NULL;
		delete iClipLocationTrgt;iClipLocationTrgt=NULL;
		}

	iConvert->Close();
	delete iConvert;
	iConvert = NULL;

	//check if target files OK
	TInt size = 0;
	iFs.Connect();
	if ( KErrNone != iFile.Open(iFs, iFileName2, EFileRead) )
		{
		INFO_PRINTF2(_L("****the target file %S doesn't exit!!!!!****"), &iFileName2);
		iTestStepResult = EFail;
		}
	else if (iFile.Size(size) != KErrNone)
		iTestStepResult = EInconclusive;
	if (size == 0)
		iTestStepResult = EFail;


	size =0;
	if ( KErrNone != iFile.Open(iFs, iFileNameWav2, EFileRead) )
		{
		INFO_PRINTF2(_L("****the target file %S doesn't exit!!!!!****"), &iFileNameWav2);
		iTestStepResult = EFail;
		}
	else if (iFile.Size(size) != KErrNone)
		iTestStepResult = EInconclusive;
	if (size == 0)
		iTestStepResult = EFail;




	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

