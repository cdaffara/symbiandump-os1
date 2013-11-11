// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//Tests for INC044215
	
// --------------------------------------------

/**
 *
 * Static constructor for CTestStepConvertOpenLocFormat4.
 *
 *
 * @return	"CTestStepConvertOpenLocFormat4*"
 *			The constructed CTestStepConvertOpenLocFormat4
 *
 * @xxxx
 * 
 */
CTestStepConvertOpenLocFormat4* CTestStepConvertOpenLocFormat4::NewL()
	{
	CTestStepConvertOpenLocFormat4* self = new(ELeave) CTestStepConvertOpenLocFormat4;
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
CTestStepConvertOpenLocFormat4::CTestStepConvertOpenLocFormat4() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0575-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertOpenLocFormat4::~CTestStepConvertOpenLocFormat4()
	{
	}

void CTestStepConvertOpenLocFormat4::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	if(aCurrentState == CMdaAudioClipUtility::ERecording)
		{
		iError = KErrGeneral;
		INFO_PRINTF1(_L("CTestStepConvertOpenLocFormat4::MoscoStateChangeEvent incorrect state == CMdaAudioClipUtility::ERecording"));
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
enum TVerdict CTestStepConvertOpenLocFormat4::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	//INFO_PRINTF2(_L("%S"),iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::OpenL(location, location, controller, format, datatype) "));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName9"), iFileName) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Convert utility
	if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
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
enum TVerdict CTestStepConvertOpenLocFormat4::DoTestStepPostambleL(void)
	{
	//delete the converted file 
	iFs.Connect();
	iFileMan = CFileMan::NewL(iFs);
	iFs.Close();
	delete iFileMan;
	iFileMan = NULL;

	delete iClipLocationSrc;
	delete iClipLocationTrgtDes;
	delete iConvert;
	iConvert = NULL;
	iBuffer.ResetAndDestroy();

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
TVerdict CTestStepConvertOpenLocFormat4::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
    
    TMdaAudioDataSettings iAudioSettings;
	iAudioSettings.iSampleRate = 8000;
    iAudioSettings.iChannels = 1;

	//from MuVee
	iAudioSettings.iCaps = TMdaAudioDataSettings::ESampleRate8000Hz | TMdaAudioDataSettings::EChannelsMono;
    iAudioSettings.iMaxVolume = 100;
	iAudioSettings.iVolume = 100;
	iAudioSettings.iFlags = 0;
 
	TDes8* buffer = new(ELeave) TBuf8<4096 * 5>;
	buffer->Zero();
	TDes8* buffer2 = new(ELeave) TBuf8<4096 * 4>;
	buffer2->Zero();

	for(TInt conversion=0; conversion<2; conversion++)
		{
		if(conversion == 0)
			{
			
			//descriptor large enough to hold the output of duration of the play window

			User::LeaveIfError(iBuffer.Append(buffer));

			iClipLocationSrc = new TMdaFileClipLocation(iFileName);
			
			iClipLocationTrgtDes = new TMdaDesClipLocation(*iBuffer[0]);

			_LIT(KSourceWavFile,"c:\\mm\\mmf\\testfiles\\aclnt\\4sec_wav.wav");

			TRAP(err,iConvert->OpenL(KSourceWavFile,
						iClipLocationTrgtDes,
						&iRawClipFormat,
						&iWavAudioPcmCodec,
						&iAudioSettings ));

			}
		if(conversion == 1)
			{
			
			//small descriptor, will KErrOverflow (-9)

			User::LeaveIfError(iBuffer.Append(buffer2));

			iClipLocationSrc = new TMdaFileClipLocation(iFileName);
			
			iClipLocationTrgtDes = new TMdaDesClipLocation(*iBuffer[1]);

			_LIT(KSourceWavFile,"c:\\mm\\mmf\\testfiles\\aclnt\\4sec_wav.wav");


			TRAP(err,iConvert->OpenL(KSourceWavFile,
						iClipLocationTrgtDes,
						&iRawClipFormat,
						&iWavAudioPcmCodec,
						&iAudioSettings ));
			}
/*
//unsafe test as don't know OOM limits

		if(conversion == 2)
			{
			//large descriptor, will KErrNoMemory (-4)
			TDes8* buffer3 = new(ELeave) TBuf8<44214>;
			buffer3->Zero();

			User::LeaveIfError(iBuffer.Append(buffer3));

			iClipLocationSrc = new TMdaFileClipLocation(iFileName);
			iClipLocationTrgt = new TMdaFileClipLocation(iFileName2);
			
			iClipLocationTrgt2 = new TMdaDesClipLocation(*iBuffer[2]);

			_LIT(KSourceWavFile,"c:\\mm\\mmf\\testfiles\\aclnt\\4sec_wav.wav");

			TRAP(err,iConvert->OpenL(KSourceWavFile,
						iClipLocationTrgt2,
						&iRawClipFormat,
						&iWavAudioPcmCodec,
						&iAudioSettings ));

			}
*/

		CActiveScheduler::Start();

		if (iConvert == NULL ||
			iError != KErrNone ||
			err != KErrNone ||
			iConvert->State() != CMdaAudioConvertUtility::EOpen  )
				return EFail;

		iConvert->SetPlayWindow( TTimeIntervalMicroSeconds(0),TTimeIntervalMicroSeconds(KOneSecondInMicroSeconds) );

		TRAP(err,iConvert->ConvertL());
		err = err;
		CActiveScheduler::Start();
		if (iError!=KErrNone) 
			return EFail;
		CActiveScheduler::Start();
		if ((conversion == 0) && (iError != KErrNone))
			return EFail;
		if ((conversion == 1) && (iError != KErrOverflow))
			return EFail;



		delete iClipLocationSrc; iClipLocationSrc=NULL;
		delete iClipLocationTrgtDes; iClipLocationTrgtDes=NULL;

		}

	iConvert->Close();
	delete iConvert;
	iConvert = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}




