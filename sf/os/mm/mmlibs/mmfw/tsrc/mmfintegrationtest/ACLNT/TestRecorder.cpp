// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed the test of the MMF_ACLNT.
// 
//

/**
 @file TestRecorder.cpp
*/

#include "mmfwavformat.h"
#include <hal.h>
#include "TestRecorder.h"

//copy util
TInt	CopyFile(TPtrC aFileNameSrc, TPtrC aFileNameDst)
{
	TInt theErr = KErrNone;

	RFs		fs;
	fs.Connect();
	RFile	theFileNew;
	RFile	theFile;

	fs.MkDirAll(aFileNameDst);
	theFile.Open(fs, aFileNameSrc, EFileShareReadersOnly);
	TInt	theSize;
	theFile.Size(theSize);
	
	TBuf8<KSizeBuf>	theBuf;
	TInt	theStep = KSizeBuf;
	if(theStep > theSize)
		{
		theStep = theSize;
		}
	
	TInt theRes = theFileNew.Create(fs, aFileNameDst, EFileShareAny);
	if(theRes == KErrAlreadyExists)
		{
		theRes = theFileNew.Replace(fs, aFileNameDst, EFileShareAny);
		}
		

	if(theRes != KErrNone)
		{
		return theRes;
		}
		

	TInt	theCurLength = 0;
	while(theCurLength < theSize)
		{
		theFile.Read(theBuf, theStep);
		theFileNew.Write(theBuf, theStep);
		theCurLength += theStep;
		
		if(theCurLength + theStep > theSize )
			{
			theStep = theSize - theCurLength;
			}
		}

	theFileNew.Close();
	theFile.Close();
	fs.Close();

	return theErr;
}


CTestMmfAclntRecord::CTestMmfAclntRecord(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) :
	iNegative(aNegative)
	{
	// store the name of this test case
	// this is the name that is used by the script file
    // Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	}

void CTestMmfAclntRecord::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;
	INFO_PRINTF1( _L("CTestMmfAclntRecord : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF4( _L("iError %d iPreviousState %d iCurrentState %d"), iError, iPreviousState, iCurrentState);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecord::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorder"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	TPtrC			filename2; 
	
	if(!GetStringFromConfig(iSectName, _L("cropAudioFile"), filename1))
		{
		return EInconclusive;
		}
		

	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename2))
		{
		return EInconclusive;
		}
		
	
	
	GetDriveName(filename);
	filename.Append(filename1);

	CopyFile(filename2, filename);

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);
	recUtil->OpenFileL(filename);
	// wait
	INFO_PRINTF1(_L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();


	if(iError == KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &filename);
		ret = DoTestL(recUtil);
		}

	CleanupStack::PopAndDestroy(recUtil);
	User::After(KTwoSeconds);
	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	return ret;
	}

CTestMmfAclntRecGain::CTestMmfAclntRecGain(const TDesC& aTestName, const TDesC& aSectName, const TInt aGain, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	, iGain (aGain)
	{}

CTestMmfAclntRecGain* CTestMmfAclntRecGain::NewL(const TDesC& aTestName, const TDesC& aSectName,const TInt aGain, TBool aNegative)
	{
	CTestMmfAclntRecGain* self = new (ELeave) CTestMmfAclntRecGain(aTestName, aSectName, aGain, aNegative);
	return self;
	}

/**
 * RecordUtils test: Set gain to maximum and enquire gain.
 */
TVerdict CTestMmfAclntRecGain::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF2( _L("TestRecorder : Gain - (%d)"), iGain);

	TVerdict ret = EFail;

	// Set up a file to record to
	TPtrC filename;
	if (!GetStringFromConfig(iSectName, _L("outputAudioFile"), filename))
		{
		INFO_PRINTF1(_L("Error getting filename from INI file"));
		return EInconclusive;
		}

	RFs fs;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting file session"), err);
		fs.Close();
		return EInconclusive;
		}

	err = fs.Delete(filename);
	
	if (!((err == KErrNone) || (err == KErrNotFound)))
		{
		INFO_PRINTF2(_L("Error deleting output file"), err);
		return EInconclusive;
		}
	
	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRec"))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		}

	aRecUtil->OpenFileL(filename);
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &filename);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file for recording"), iError);
		return EInconclusive;
		}

	// Set gain
	if(iGain == 0)
		{
		iGain = aRecUtil->MaxGain();
		}
		
	aRecUtil->SetGain(iGain);

	// Set expected value
	TInt expectedGain = 0;
	if (iGain > aRecUtil->MaxGain())
		{
		expectedGain = aRecUtil->MaxGain();
		}
		
	else if (iGain < 0)
		{
		expectedGain = 0;
		}
		
	else
		{
		expectedGain = iGain;
		}
		

	aRecUtil->SetDestinationSampleRateL(8000);	

	// Start recording
	aRecUtil->RecordL();
	CActiveScheduler::Start();
	User::After(1000000);

	// Get gain
	TInt gain;
	aRecUtil->GetGain(gain);

	// The gain should be not be more than max gain
	if (gain > aRecUtil->MaxGain())
		{
		INFO_PRINTF3(_L("ERROR: GetGain value greater than MaxGain (%d > %d)"), gain, aRecUtil->MaxGain());
		ret = EFail;
		}
	// The gain should not be less than 0
	else if (gain < 0)
		{
		INFO_PRINTF3(_L("ERROR: GetGain value less than minimum gain (%d < %d)"), gain, 0);
		ret = EFail;
		}
	else if (gain != expectedGain)
		{
		ERR_PRINTF3(_L("ERROR: GetGain value not equal to expected value (%d != %d)"), gain, expectedGain);
		ret = EFail;
		}
	else
		{
		ret = EPass;	
		}

	// stop the recording - only necessary if anyone wanted to look at the output file
	aRecUtil->Stop();

	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntRecStateTran::CTestMmfAclntRecStateTran(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{}

CTestMmfAclntRecStateTran* CTestMmfAclntRecStateTran::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecStateTran* self = new (ELeave) CTestMmfAclntRecStateTran(aTestName, aSectName, aNegative);
	return self;
	}

TVerdict CTestMmfAclntRecStateTran::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Transition"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TPtrC filename; 


	if(!GetStringFromConfig(iSectName, _L("outputAudioFile"), filename))
		{
		return EInconclusive;
		}
		
	RFs	fs;
	fs.Connect();
	fs.Delete(filename);
	fs.Close();

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);
	
	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRecState"))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		}
	
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &filename);
	recUtil->OpenFileL(filename);
	// wait
	INFO_PRINTF1(_L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		ret = DoTestL(recUtil);
		}
		

	CleanupStack::PopAndDestroy(recUtil);
	User::After(KTwoSeconds);
	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	return ret;
	}

/**
 * Enquire state at each state transition.
 */
TVerdict CTestMmfAclntRecStateTran::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecorder : State Transition"));

	TVerdict ret= EFail;

	iReportedState = aRecUtil->State();
	if( iReportedState == CMdaAudioClipUtility::EOpen )
		{
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		aRecUtil->RecordL();
		CActiveScheduler::Start(); // wait for open -> record
		if (iError != KErrNone)
			{
			return EFail;
			}
			
		iReportedState = aRecUtil->State();
		if(iReportedState == CMdaAudioClipUtility::ERecording )
			{
			ret = EPass;
			}
			
		User::After(KTwoSeconds);
		aRecUtil->Stop();

		iReportedState = aRecUtil->State();
		if(iReportedState != CMdaAudioClipUtility::EOpen )
			{
			return EFail;
			}
			
		aRecUtil->PlayL();
		CActiveScheduler::Start(); // wait for open -> play
		if (iError != KErrNone)
			{
			return EFail;
			}
			
		iReportedState = aRecUtil->State();
		if( iReportedState == CMdaAudioClipUtility::EPlaying )
			{
			// Wait for play to complete
			CActiveScheduler::Start();
			if (iError != KErrNone)
				{
				return EFail;
				}
				
			//copy file
			}
		}
	return ret;
	}

//---------------------------------------------------------------------------------

CTestMmfFormatRead::CTestMmfFormatRead(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{
	}

CTestMmfFormatRead* CTestMmfFormatRead::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfFormatRead* self = new (ELeave) CTestMmfFormatRead(aTestName, aSectName, aNegative);
	return self;
	}

_LIT(KFileXyz, "\\AclntITestData\\Test.xyz");
_LIT(KFileAbc, "\\AclntITestData\\Test.abc");

TVerdict CTestMmfFormatRead::DoTestStepL()
	{
	INFO_PRINTF1( _L("CTestMmfFormatRead : Opening the test files"));
	TVerdict ret = EPass;

	INFO_PRINTF1( _L("Opening the test file test.xyz"));

	TPtrC fileXyz(KFileXyz);

	// File with extension .xyz does not have a plugin and hence opening this file should fail.
	CMdaAudioRecorderUtility* recUtilXyz = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtilXyz);
	TRAP_IGNORE(recUtilXyz->OpenFileL(fileXyz));
	CActiveScheduler::Start(); 
		 
	if (recUtilXyz->State() == CMdaAudioClipUtility::EOpen)
		{
		INFO_PRINTF1( _L("Opening the test file test.xyz failed"));
		ret = EFail;
		}
	
	CleanupStack::PopAndDestroy(recUtilXyz);

	INFO_PRINTF1( _L("Opening the test file test.abc"));
	TPtrC fileAbc(KFileAbc);
	
	// File with extension .abc does have a plugin and hence opening this file should pass.
	CMdaAudioRecorderUtility* recUtilAbc = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtilAbc);
	TRAP_IGNORE(recUtilAbc->OpenFileL(fileAbc));
	CActiveScheduler::Start(); 
	
	if (recUtilAbc->State() != CMdaAudioClipUtility::EOpen)
		{
		INFO_PRINTF1( _L("Opening the test file test.abc failed"));
		ret = EFail;
		}
	
	CleanupStack::PopAndDestroy(recUtilAbc);

	return ret;
	}

/**
 * Enquire state at each state transition.
 */

TVerdict CTestMmfFormatRead::DoTestL(CMdaAudioRecorderUtility* /*aRecUtil*/)
	{
	INFO_PRINTF1( _L("CTestMmfFormatRead::DoTestL"));
	return (TVerdict(EPass));
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntCrop::CTestMmfAclntCrop(const TDesC& aTestName, const TDesC& aSectName, const TBool aCropToEnd, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{
	iCropToEnd = aCropToEnd;
	}

CTestMmfAclntCrop* CTestMmfAclntCrop::NewL(const TDesC& aTestName, const TDesC& aSectName, const TBool aCropToEnd, TBool aNegative)
	{
	CTestMmfAclntCrop* self = new (ELeave) CTestMmfAclntCrop(aTestName, aSectName, aCropToEnd, aNegative);
	return self;
	}



/**
 * Audio croping.
 */
TVerdict CTestMmfAclntCrop::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
 	TTimeIntervalMicroSeconds expectedDuration(KPcm16FrameInterval * 5), duration;

	duration = aRecUtil->Duration();
	INFO_PRINTF3(_L("Duration of Original file, expected =  %f, actual: %f"),
					I64REAL(expectedDuration.Int64()), I64REAL(duration.Int64()));
					
	if(expectedDuration == duration)
		{
		return EInconclusive;
		}
	
	if(duration > expectedDuration)
		{
		aRecUtil->SetPosition(expectedDuration);
		
		if(iCropToEnd)
			{
			TRAP(iError, aRecUtil->CropL());
			}
		else
			{
			TRAP(iError, aRecUtil->CropFromBeginningL());
			}
		if(iError != KErrNone)
			{
			ERR_PRINTF2(_L("Crop left with error =  %d"),iError);
			return EFail;
			}
		if(iCropToEnd)
			{
			TTimeIntervalMicroSeconds actualDuration = aRecUtil->Duration();
			if (TimeComparison(I64INT(actualDuration.Int64()), I64INT(expectedDuration.Int64()), KExpectedDurationDeviation))
				{
				INFO_PRINTF1(_L("Duration within expected deviation"));
				return EPass;
				}
			else
				{
				ERR_PRINTF3(_L("Duration cropped file, expected =  %f, actual: %f"),
					I64REAL(expectedDuration.Int64()), I64REAL(actualDuration.Int64()));
				}
			}
		if(!iCropToEnd)
			{
			TInt64 theDelta = duration.Int64() - expectedDuration.Int64();

			TTimeIntervalMicroSeconds actualDuration = aRecUtil->Duration();
			if (TimeComparison(I64INT(actualDuration.Int64()), I64INT(theDelta), KExpectedDurationDeviation))
				{
				INFO_PRINTF1(_L("Duration within expected deviation"));
				return EPass;
				}
			else
				{
				ERR_PRINTF3(_L("Duration cropped file, expected =  %f, actual: %f"),
					I64REAL(theDelta), 
					I64REAL(aRecUtil->Duration().Int64()));
				}
			}
		}
	return	EFail ;
	}


//------------------------------------------------------------------

CTestMmfAclntRecCloseOpen::CTestMmfAclntRecCloseOpen(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{
	}

CTestMmfAclntRecCloseOpen* CTestMmfAclntRecCloseOpen::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecCloseOpen* self = new (ELeave) CTestMmfAclntRecCloseOpen(aTestName, aSectName, aNegative);
	return self;
	}

TVerdict CTestMmfAclntRecCloseOpen::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
/**
 * Open, Close and open a new audio clip.
 */
	{
	INFO_PRINTF1( _L("TestRecorder : Close/Open"));

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 

	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);

	iError = KErrTimedOut;
	

	aRecUtil->Close();
	aRecUtil->OpenFileL(filename);
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	//wait
	CActiveScheduler::Start(); 
	if(iError == KErrNone)
		{
		return EPass;	
		}
		
	return	EFail ;
	}


//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecFile::CTestMmfAclntRecFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)

	{
	iTestStepName = aTestName;
	iHeapSize = 200000;		// playback part of test loads whole sound file into buffer, so need large heap
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateFile = aCreateFile;
	}

CTestMmfAclntRecFile* CTestMmfAclntRecFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntRecFile* self = new (ELeave) CTestMmfAclntRecFile(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	return self;
	}

CTestMmfAclntRecFile* CTestMmfAclntRecFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntRecFile* self = CTestMmfAclntRecFile::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntRecFile::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecFile : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

    if(aCurrentState == CMdaAudioClipUtility::ERecording)
        {
        User::After(KFiveSeconds);
        }
	CActiveScheduler::Stop();
	}

// Audio utility callbacks
void CTestMmfAclntRecFile::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntRecFile::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

// Audio output stream callbacks
void CTestMmfAclntRecFile::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntRecFile::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntRecFile::MaoscPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntRecFile::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	SetupFormatL(iTestFormat);

	if(iCreateFile)
		{
		// Make sure file doesn't exist (maybe from a previous test)
		TParse fullFileName;
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 

		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
			
		GetDriveName(filename);
		filename.Append(filename1);
		

		// parse the filenames
		if(!fullFileName.Set(filename, &KDefault, NULL))
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
			TInt error = fs.Delete(fullFileName.FullName());

			if(error == KErrPathNotFound)
			{
				error = fs.MkDirAll(fullFileName.DriveAndPath());
				INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
			}
			else  if(error == KErrNotFound)
				INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
			else if(error==KErrNone)
				INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
			else
				{
				ret = EInconclusive;
				}
			fs.Close();
			}
		
		}

	if((ret == EInconclusive) || (ret == EFail))
		{
		return ret;	
		}
		
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Open a file based clip and record
 */
TVerdict CTestMmfAclntRecFile::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File"));
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	TPtrC filename;
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}
		
	
	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TMdaFileClipLocation location(filename);
	
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &filename);
	if(!iCreateFile)
		{
		if(iSectName == _L("SectionOggRec")) // - Added to use OpenFileL only as OpenL parameters are deprected.
			{
			TPtrC filename2; 

			if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename2))
				{
				return EInconclusive;
				}
				
			CopyFile(filename2, filename);
			}
		recUtil->OpenFileL(filename);
		}
	else
		{
		RFs fs;
		fs.Connect();
		fs.Delete(filename);
		fs.Close();
		if(iSectName == _L("SectionOggRec")) // - Added to use OpenFileL only as OpenL parameters are deprected.
			{
			recUtil->OpenFileL(filename); // - Should create the file internally
			}
		else
			{
			recUtil->OpenL(&location,iFormat,iCodec,&iAudioSettings);	
			}
		}

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		recUtil->RecordL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open -> record
		
		recUtil->Stop();
		}


	CleanupStack::PopAndDestroy(recUtil);
	recUtil = NULL;

	// Playback the file
	if (iError == KErrNone)
		{
		if (filename.Right(4).Compare(_L(".raw"))==0)
			{
			// Raw file playback
			CMdaAudioOutputStream* outputStream = CMdaAudioOutputStream::NewL(*this);
			CleanupStack::PushL(outputStream);

			outputStream->Open(NULL);
			CActiveScheduler::Start();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Error opening output stream for playback = %d"), iError);
				CleanupStack::PopAndDestroy(outputStream);
				return EFail;
				}

			// Connect file session
			RFs fs;
			TInt err = fs.Connect();
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error connecting to file session = %d"), err);
				return EFail;
				}
			CleanupClosePushL(fs);

			// Open file
			RFile file;
			err = file.Open(fs, filename, EFileRead);
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error opening file = %d"), err);
				CleanupStack::PopAndDestroy(2, outputStream);	// fs, outputStream
				return EFail;
				}
			CleanupClosePushL(file);

			// Get size of file
			TInt fileSize = 0;
			err = file.Size(fileSize);
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error getting size of file = %d"), err);
				CleanupStack::PopAndDestroy(3, outputStream);	// file, fs, outputStream
				return EFail;
				}

			// Read file
			HBufC8* buf = HBufC8::NewLC(fileSize);
			TPtr8 ptr(buf->Des());
			err = file.Read(ptr);
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error reading file %d"), err);
				}
			else
				{
				outputStream->WriteL(ptr);
				CActiveScheduler::Start();
				if (iError != KErrNone)
					INFO_PRINTF2(_L("Error writing file on audio output stream %d"), iError);
				}

			CleanupStack::PopAndDestroy(4, outputStream);	// buf, file, fs, outputStream
			}
		else
			{
			// Wav file playback
			CMdaAudioPlayerUtility* playUtil = CMdaAudioPlayerUtility::NewL(*this);
			CleanupStack::PushL(playUtil);
			TRAPD(err, playUtil->OpenFileL(filename));
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error opening file for playback err = %d"), err);
				CleanupStack::PopAndDestroy(playUtil);
				return EFail;
				}
			CActiveScheduler::Start();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Error opening file for playback iError = %d"), iError);
				CleanupStack::PopAndDestroy(playUtil);
				return EFail;
				}
			playUtil->Play();
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(playUtil);
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Error during playback of recorded file iError=%d"), iError);
				return EFail;
				}
			}
		}
	// DEF127335
   	// OggRecordController does not support recording into an already existing file.
   	// This is due to unavailability of APIs at the level of Ogg Vorbis C libraries.
	else if (iTestStepName == _L("MM-MMF-ACLNTOGG-I-0102-CP") && iError == KErrNotSupported)
		{
    	INFO_PRINTF1(_L("Expected Result: OggRecordController returned KErrNotSupported(-5)"));
		return EPass;
		}

	if( iError == KErrNone ) 
		{
		RFs fs;
		RFile file;
		TInt size = 0;
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(file.Open(fs,filename,EFileRead));
		CleanupClosePushL(file);
		User::LeaveIfError(file.Size(size));

		if(size > 0)
			{
			ret = EPass;
			}
			
		CleanupStack::PopAndDestroy(); //file
		}

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	User::After(KOneSecond);
	return	ret;
	}



//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecFileForceFormat::CTestMmfAclntRecFileForceFormat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iFormat = aFormat;
	}

CTestMmfAclntRecFileForceFormat* CTestMmfAclntRecFileForceFormat::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecFileForceFormat* self = new (ELeave) CTestMmfAclntRecFileForceFormat(aTestName,aSectName,aKeyName,aFormat);
	return self;
	}

CTestMmfAclntRecFileForceFormat* CTestMmfAclntRecFileForceFormat::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecFileForceFormat* self = CTestMmfAclntRecFileForceFormat::NewLC(aTestName,aSectName,aKeyName,aFormat);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntRecFileForceFormat::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecFile : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	//dont stop the playing track
	if (aCurrentState != CMdaAudioClipUtility::EPlaying || iError != KErrNone)
		{
		CActiveScheduler::Stop();
		}
	}


TVerdict CTestMmfAclntRecFileForceFormat::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	// Make sure file doesn't exist (maybe from a previous test)
	TParse fullFileName;
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 

	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
		

	GetDriveName(filename);
	filename.Append(filename1);
		

	// parse the filenames
	if(!fullFileName.Set(filename, &KDefault, NULL))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
		TInt error = fs.Delete(fullFileName.FullName());

		if(error == KErrPathNotFound)
			{
			error = fs.MkDirAll(fullFileName.DriveAndPath());
			INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
			}
		else  if(error == KErrNotFound)
			INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
		else if(error==KErrNone)
			INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
		else
			{
			ret = EInconclusive;
			}
		fs.Close();
		}

	if((ret == EInconclusive) || (ret == EFail))
		{
		return ret;	
		}
		
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Open a file based clip and record
 */


TVerdict CTestMmfAclntRecFileForceFormat::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File"));
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	TPtrC			filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TMdaFileClipLocation location(filename);

	RFs fs;
	fs.Connect();
	fs.Delete(filename);
	fs.Close();

	recUtil->OpenL(&location,&iFormat);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		recUtil->RecordL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open -> record

		User::After(KTwoSeconds);
		recUtil->Stop();

		//now try to play the file as a check that it recorded correctly
		recUtil->PlayL();
		CActiveScheduler::Start();

		recUtil->Close();

	
		if(iError == KErrNone) 
			{
			RFs fs;
			RFile file;
			TInt size = 0;

			User::LeaveIfError(fs.Connect());
			User::LeaveIfError(file.Open(fs,filename,EFileRead));
			CleanupClosePushL(file);
			User::LeaveIfError(file.Size(size));

			if(size > 0)
				{
				ret = EPass;
				}
				
			CleanupStack::PopAndDestroy(); //file
			}
		}
	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	CleanupStack::PopAndDestroy(recUtil);
	User::After(KOneSecond);
	return	ret;
	}


//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecDes::CTestMmfAclntRecDes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateDes):
	iAudioPtr(NULL,0,0)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateDes = aCreateDes;
	iSize = 0;
	iAudio = NULL;
	iHeapSize = 100000;
	}

CTestMmfAclntRecDes* CTestMmfAclntRecDes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat,const TBool aCreateDes)
	{
	CTestMmfAclntRecDes* self = new (ELeave) CTestMmfAclntRecDes(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	return self;
	}

CTestMmfAclntRecDes* CTestMmfAclntRecDes::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateDes)
	{
	CTestMmfAclntRecDes* self = CTestMmfAclntRecDes::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntRecDes::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF4( _L("MMdaObjectStateChangeObserver: previous state: %d current state: %d error: %d"),aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecDes::DoTestStepPreambleL()
	{
	SetupFormatL(iTestFormat);

	switch (iTestFormat) 
		{
		case EPcm16Wav:
		case EAlawWav:
		case EPcm8:
			iHeaderSize = KWavFileUncompressedDataHeaderSize; //(see mmfwavformat.cpp, CreateSinkBufferOfSizeL())
			iFrameSize = 4096;
			break;
		case EImaAdpcmWav:
		case EImasPcmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
			iFrameSize = 4096;
			break;
		case EMulawRaw:
		case EPcmU8:
		case EPcmU16:
			iHeaderSize = 0;
			iFrameSize = 4096;
			break;
		case EGsmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
			iFrameSize = 4095;
			break;
		default:
			/*skip*/ ;
		}

	if(!iCreateDes)
		{
		RFs fs;
		RFile file;

		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
			
		GetDriveName(filename);
		filename.Append(filename1);
		INFO_PRINTF2(_L("File for test - %S"), &filename);
		
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(file.Open(fs,filename,EFileRead));
		CleanupClosePushL(file);

		User::LeaveIfError(file.Size(iSize));
		INFO_PRINTF2(_L("size of file = %d\n"),iSize);//Statement Changed under DEF105143
		iAudio = HBufC8::NewMaxL(iSize);

		// Read a file into a descriptor
		iAudioPtr.Set(iAudio->Des());
		User::LeaveIfError(file.Read(iAudioPtr));

		CleanupStack::PopAndDestroy(); //file
		}
	else
		{
		INFO_PRINTF1(_L("Recording to New Descriptor for test"));
		iSize = KRecSize; // value is not been set yet.
		iAudio = HBufC8::NewMaxL(iSize);
		iAudioPtr.Set(iAudio->Des());
		iAudioPtr.SetLength(0);
		}

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

TVerdict CTestMmfAclntRecDes::DoTestStepPostambleL()
	{
	delete iAudio;
	return CTestMmfAclntCodecTest::DoTestStepPostambleL();
	}

/**
 * Open a descriptor and record
 */
TVerdict CTestMmfAclntRecDes::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorder : Record Des"));
	TVerdict ret = EFail;
	TMdaDesClipLocation* location = NULL;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	if(!iCreateDes)
		{
		recUtil->OpenDesL(iAudioPtr);
		}
	else
		{
		location = new (ELeave) TMdaDesClipLocation(iAudioPtr);
		CleanupStack::PushL(location);
		recUtil->OpenL(location,iFormat,iCodec,&iAudioSettings);
		CleanupStack::PopAndDestroy(location);
		}

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		// set the sample rate to 8K, otherwise the default value of 44.1Kh will be used
		// resulting in very large buffers (11K)
		recUtil->SetDestinationSampleRateL(8000);	

		iError = KErrTimedOut;
		recUtil->RecordL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open->record
		if (iError != KErrNone)
			{
			// DEF127335
		   	// OggRecordController does not support recording into an already existing descriptor.
		   	// This is due to unavailability of APIs at the level of Ogg Vorbis C libraries.
			if (iTestStepName == _L("MM-MMF-ACLNTOGG-I-0103-CP") && iError == KErrNotSupported)
				{
				INFO_PRINTF1(_L("Expected Result: OggRecordController returned KErrNotSupported(-5)"));
				ret = EPass;
				}
			ERR_PRINTF2( _L("CMdaAudioRecorderUtility failed with error %d"),iError );
			CleanupStack::PopAndDestroy(recUtil);	
			User::After(KOneSecond);
			return ret;
			}
		// wait to run out of space recording
		CActiveScheduler::Start(); // record -> open

		if (iError != KErrOverflow)
			{
            ERR_PRINTF2( _L("CMdaAudioRecorderUtility failed with error %d"),iError );
            CleanupStack::PopAndDestroy(recUtil);	
            User::After(KOneSecond);
            return ret;
			}

		TInt expectedLength=0;
		TInt actualLength = iAudio->Length();
		if(iCreateDes)
			{
			// Calculuate the frame size which is now dynamic.
			// This is more or less a duplicate of the calculation in 
			// CMMFAudioInput::NegotiateSourceL()
			expectedLength = iSize;	
			TUint sampleRate = recUtil->DestinationSampleRateL();
			TUint bitRate = recUtil->DestinationBitRateL();

			//xxx work around for GSM which returns zero as the bit rate because internally
			// BitsPerSample is zero
			if (bitRate==0)
				{
				bitRate = sampleRate * 8;
				}
				
			TUint numberOfChannels = recUtil->DestinationNumberOfChannelsL();
			iFrameSize = (bitRate * numberOfChannels / 8) / 4;
			iFrameSize = (iFrameSize + (KAudioInputDeltaFrameSize-1)) &~ (KAudioInputDeltaFrameSize-1);
			if(iFrameSize < KAudioInputMinFrameSize) 
				{
					iFrameSize = KAudioInputMinFrameSize;
				}
			
			else if(iFrameSize > KAudioInputMaxFrameSize)
				{
				iFrameSize = KAudioInputMaxFrameSize;
				}

			// GSM will onlt write out an integral number of GSM frames
			// which are 65 bytes long..
			if (iTestFormat == EGsmWav)
				{
				iFrameSize = (iFrameSize / KGsmEncodedFrameSize) * KGsmEncodedFrameSize;
				}
				
			
			expectedLength = ((iSize-iHeaderSize) / iFrameSize)* iFrameSize + iHeaderSize;

			INFO_PRINTF2(_L("sampleRate = %d"), sampleRate);
			INFO_PRINTF2(_L("bitRate = %d"), bitRate);
			INFO_PRINTF2(_L("numberOfChannels = %d"), numberOfChannels);
			INFO_PRINTF2(_L("iFrameSize = %d"), iFrameSize);
			INFO_PRINTF2(_L("Duration = %d"), I64LOW(recUtil->Duration().Int64()));
			INFO_PRINTF3(_L("Expected length %d, actual length %d"), expectedLength, actualLength);


#if defined __WRITE_CONVERSION_TO_FILE
			{
			_LIT(KFileOut, "\\TEST.WAV");
			RFs fs;

			fs.Connect();
			RFile file;
			file.Replace(fs, KFileOut(), EFileWrite);
			file.Write(iAudio->Des());
			TInt len = iAudio->Length();
			file.Size(len);
			file.Close();
			fs.Close();
			}
#endif

			// a more forgiving test would be :
			//if ((actualLength <= expectedLength) &&
			//	(actualLength >= (expectedLength - iFrameSize)))
			
			// if test is GSM then we expect it to be between 
			// expected Length and (iSize - iHeaderSize)
			if ((iTestFormat == EGsmWav) && 
				(actualLength <= (iSize - iHeaderSize))	&& 
				(actualLength >= expectedLength) )
				{
				ret = EPass;
				}
			else if (iAudio->Length() == expectedLength)
				{
				ret = EPass;
				}
			else
				{
				INFO_PRINTF3(_L("Expected length %d, actual length %d"), expectedLength, actualLength);
				
				TInt machineType;
	            TInt err = HAL::Get(HALData::EMachineUid, machineType);
	            if (err)
	                {
				    ERR_PRINTF1(_L("Error Getting Device information"));
				    iTestStepResult = EFail;
				    CActiveScheduler::Stop();
				    }
	            else
	                {
                    if ((machineType == 0x102864F7) && (sampleRate == 8000))
                        {
                            // If test is running on a NaviEngine and uses 8K sample rate
                            // then apply a tollerance when checking duration as we will
                            // have applied conversion function.
                            if ((iAudio->Length() >= expectedLength - 1000) &&
                                 (iAudio->Length() <= expectedLength + 1000))
                                {
                                ret = EPass;
                                }
                        }
	                }
				}
			}
		else
			{
			expectedLength = iSize;
			
			if(iAudio->Length() == expectedLength)
				{
				ret = EPass;
				}
			
			else
				{
				INFO_PRINTF3(_L("Expected length %d, actual length %d"), expectedLength, actualLength);
				}

			}
		}


	if (ret != EPass)
		{
		ERR_PRINTF2( _L("CMdaAudioRecorderUtility failed with error %d"),iError );
		}
		

	CleanupStack::PopAndDestroy(recUtil);	
	User::After(KOneSecond);
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecUrl::CTestMmfAclntRecUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfAclntRecUrl* CTestMmfAclntRecUrl::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecUrl* self = new (ELeave) CTestMmfAclntRecUrl(aTestName,aSectName,aKeyName);
	return self;
	}

CTestMmfAclntRecUrl* CTestMmfAclntRecUrl::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecUrl* self = CTestMmfAclntRecUrl::NewLC(aTestName,aSectName,aKeyName);
	CleanupStack::PushL(self);
	return self;
	};

void CTestMmfAclntRecUrl::MoscoStateChangeEvent(CBase* /*aObject*/, 
													TInt /*aPreviousState*/, 
													TInt /*aCurrentState*/, 
													TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecUrl : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	CActiveScheduler::Stop();
	}

/**
 * Open a URL
 */
TVerdict CTestMmfAclntRecUrl::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorder : Record URL"));
	TPtrC url;
	if(!GetStringFromConfig(iSectName, iKeyName, url))
		{
		return EInconclusive;
		}
		

	TVerdict ret = EFail;
	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TRAP_IGNORE(recUtil->OpenUrlL(url, KUseDefaultIap, _L8("Audio/Wav")));
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNotSupported)
	{
		ret = EPass;
	}

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	CleanupStack::PopAndDestroy(recUtil);
	return	ret;
	}

//------------------------------------------------------------------

CTestMmfAclntRecPosition::CTestMmfAclntRecPosition(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative), iPosition(aPosition)
	{
	}

CTestMmfAclntRecPosition* CTestMmfAclntRecPosition::NewL(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition, TBool aNegative)
	{
	CTestMmfAclntRecPosition* self = new (ELeave) CTestMmfAclntRecPosition(aTestName, aSectName, aPosition, aNegative);
	return self;
	}

TVerdict CTestMmfAclntRecPosition::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecorder : Position"));
	TVerdict ret = EFail;
	TBool validPosition = EFalse;

	//  Set position: middle of clip.
	if (I64INT(iPosition.Int64()) == 0)
		{
		iPosition = I64INT(aRecUtil->Duration().Int64())/2;
		}
		
	// Set position: end of clip.
	if (I64INT(iPosition.Int64()) == -1)
		{
		iPosition = aRecUtil->Duration();
		}
		
	aRecUtil->PlayL(); //Begin to Play otherwise position can not be set.
	// Position is beyond the end of the clips duration.
	// so check that the value is clipped.
	if(aRecUtil->Duration() < iPosition)
		{
		aRecUtil->SetPosition(iPosition);

		TInt64 temp = (aRecUtil->Duration().Int64() / KPcm16FrameInterval) - 1; // -1 for DevSound
		if(aRecUtil->Position().Int64() == (temp * KPcm16FrameInterval))
			{
			validPosition = ETrue;
			}
			
		}
	else
		{
		aRecUtil->SetPosition(iPosition);
		//CActiveScheduler::Start();

		TInt64 temp = (iPosition.Int64() / KPcm16FrameInterval) - 1; // -1 for DevSound
		TInt64	thePosActual = aRecUtil->Position().Int64();
		TInt64  thePosExpected = temp * KPcm16FrameInterval;
		aRecUtil->Stop();

		if (TimeComparison(I64INT(thePosActual), I64INT(thePosExpected), KExpectedDeviation))
			{
			validPosition = ETrue;
			}
		}

	if ((iNegative) && (!validPosition))
		{
		ret = EPass;
		}

	// Postion was set to a valid value.
	if(validPosition)
		{
		ret = EPass;
		}
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntRecDuration::CTestMmfAclntRecDuration(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{
	}

CTestMmfAclntRecDuration* CTestMmfAclntRecDuration::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecDuration* self = new (ELeave) CTestMmfAclntRecDuration(aTestName, aSectName, aNegative);
	return self;
	}

TVerdict CTestMmfAclntRecDuration::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecorder : Duration"));
	TVerdict ret = EFail;
	
	if(iSectName == _L("SectionOggRec"))
		{
			if (I64INT(aRecUtil->Duration().Int64()) == KSoundFileLength)
			ret = EPass;
		}
	else if (TimeComparison(I64INT(aRecUtil->Duration().Int64()), KSoundFileLength, KExpectedDeviation))
		{
		ret = EPass;	
		}
		
	else
		INFO_PRINTF3(_L("unexpected duration = %u, expected = %u"), I64INT(aRecUtil->Duration().Int64()), KSoundFileLength);
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntRecSetMeta::CTestMmfAclntRecSetMeta(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{
	}

CTestMmfAclntRecSetMeta* CTestMmfAclntRecSetMeta::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecSetMeta* self = new (ELeave) CTestMmfAclntRecSetMeta(aTestName, aSectName, aNegative);
	return self;
	}

_LIT(name,"<Copyright>");
_LIT(data,"<Symbian (c) 2002>");
_LIT(name2,"Artist");
_LIT(data2,"Test");
/**
 * Set meta-information
  */
TVerdict CTestMmfAclntRecSetMeta::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecorder : Set Metadata"));
	TVerdict ret = EFail;

	// build a meta daat object
	CMMFMetaDataEntry* metaData[2];
	metaData[0] = CMMFMetaDataEntry::NewL(name,data);
	CleanupStack::PushL(metaData[0]);
	metaData[1] = CMMFMetaDataEntry::NewL(name2,data2);
	CleanupStack::PushL(metaData[1]);

	TInt numEntries;
	TInt err = aRecUtil->GetNumberOfMetaDataEntries(numEntries);
	if(err != KErrNone)
	{
		INFO_PRINTF2(_L("GetNumberOfMetaDataEntries() returned error %d"), err);
		numEntries = 0; // so we don't try and remove anything 
	}

	// *** JW adapted test - remove any existing metadata
	// this means that a modified test file will not cause inconclusive
	if(numEntries != 0)
		{
		INFO_PRINTF2(_L("Removing %d existing entries..."), numEntries);
		for(TInt i = numEntries - 1; i >= 0; i--)
			{
			err = aRecUtil->RemoveMetaDataEntry(i);
			if(err != KErrNone)
				{
				INFO_PRINTF3(_L("RemoveMetaDataEntry(%d) returned error %d"), i, err);
				}
			}
		aRecUtil->GetNumberOfMetaDataEntries(numEntries);
		}

	if(numEntries == 0)
		{
		TRAPD(err, aRecUtil->AddMetaDataEntryL(*metaData[0]));
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("AddMetaDataEntryL(*metaData[0]) left with error %d"), err);	
			}
			

		TRAP(err, aRecUtil->AddMetaDataEntryL(*metaData[1]));
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("AddMetaDataEntryL(*metaData[1]) left with error %d"), err);
			}
			
		CleanupStack::PopAndDestroy(metaData[1]);
		CleanupStack::PopAndDestroy(metaData[0]);
		
		if(err)
		{
			if(err == KErrNotSupported) // Audio Controller doesn't support - this is ok
			{
				//
				// C-COVER Reports the following methods are not being called
				// Remove when metaData is supported
				TRAP(err,aRecUtil->GetNumberOfMetaDataEntries(numEntries));
				CMMFMetaDataEntry* testData=NULL;
				for(TInt i=0;i<numEntries;i++)
					{
					TRAP(err,testData = aRecUtil->GetMetaDataEntryL(0));
					//REPLACE WITH ITSELF??
					TRAP(err,aRecUtil->ReplaceMetaDataEntryL(i,*testData));
					TRAP(err,aRecUtil->RemoveMetaDataEntry(i));//@@@
					}

				//

				INFO_PRINTF1(_L("Note : Audio Controller does not support metadata"));
				return EPass;
			}
			else
			{
				ERR_PRINTF1(_L("Unexpected error (expected KErrNotSupported)"));
				return EFail;
			}
		}

		aRecUtil->GetNumberOfMetaDataEntries(numEntries);

		if(numEntries == 2)
			{
			CMMFMetaDataEntry* testData;
			
			testData = aRecUtil->GetMetaDataEntryL(0);
		
			if((testData->Name() == name) && (testData->Value() == data))
				{
				ret = EPass;
				}	
				
			testData = aRecUtil->GetMetaDataEntryL(1);

			if(!((testData->Name() == name2) &&	(testData->Value() == data2)))
				{
				ret = EFail;
				}
			}
		return ret;
		}

	// Audio file already contains meta data, and we couldn't remove it.
	ERR_PRINTF1(_L("Audio file still contains metadata"));
	CleanupStack::PopAndDestroy(metaData[1]);
	CleanupStack::PopAndDestroy(metaData[0]);
	return EInconclusive;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecPriority::CTestMmfAclntRecPriority(const TDesC& aTestName,const TDesC& aSectName)

	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	}

void CTestMmfAclntRecPriority::MchoComplete(TInt aID, TInt aError)
	{
	INFO_PRINTF1( _L("CTestMmfAclntRecPriority : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility called"));
	if (iError != KErrNone)
		{
		iError = aError;
		}
		
	INFO_PRINTF3( _L("iError %d ID %d"), iError, aID);

	if(iFirstCallback == -1)
		{
		iFirstCallback = aID;
		}
		

	if((--iCallbackCount) == 0)
		{
		CActiveScheduler::Stop();
		}
	}

//------------------------------------------------------------------

/**
 * Set Record priority
 */
TVerdict CTestMmfAclntRecPriority::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorder : Priority"));
	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);


    CMdaAudioRecorderUtility* recUtil[2];
	CStateCallbackHandler* callback[2];

	for(TInt i=0; i<2; i++)
		{
		callback[i] = new (ELeave) CStateCallbackHandler(i,this);
		CleanupStack::PushL(callback[i]);
		}
	
	recUtil[0] = CMdaAudioRecorderUtility::NewL(*callback[0],NULL,EMdaPriorityNormal,EMdaPriorityPreferenceTimeAndQuality);
	CleanupStack::PushL(recUtil[0]);
	recUtil[1] = CMdaAudioRecorderUtility::NewL(*callback[1],NULL,EMdaPriorityMin,EMdaPriorityPreferenceNone);
	CleanupStack::PushL(recUtil[1]);
	
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility[0]->OpenFileL(%S)"), &filename);
	recUtil[0]->OpenFileL(filename);
	// wait for initilisation callback #1
	iCallbackCount = 1;
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility #1"));
	CActiveScheduler::Start();
	
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility[1]->OpenFileL(%S)"), &filename);
	recUtil[1]->OpenFileL(filename);
	// wait for initilisation callback #2
	iCallbackCount = 1;
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility #2"));
	CActiveScheduler::Start();


	if(iError == KErrNone)
		{
		iError = KErrTimedOut;

		recUtil[0]->PlayL();
		recUtil[1]->PlayL();

		INFO_PRINTF1( _L("Play CMdaAudioRecorderUtility"));
		INFO_PRINTF1( _L("Play CMdaAudioRecorderUtility"));
		// wait for play to complete
		iCallbackCount = 2;
		iFirstCallback = -1;
		CActiveScheduler::Start();

		if((iError == KErrNone) && (iFirstCallback == 0))
			{
			CleanupStack::PopAndDestroy(4); // recUtil, callback
			return	EPass;
			}
		}

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );

	CleanupStack::PopAndDestroy(4);
	return	EFail;
	}

//------------------------------------------------------------------

CTestMmfAclntRecBalance::CTestMmfAclntRecBalance(const TDesC& aTestName, const TDesC& aSectName, const TInt aBalance, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative), iBalance(aBalance)	
	{
	}

CTestMmfAclntRecBalance* CTestMmfAclntRecBalance::NewL(const TDesC& aTestName, const TDesC& aSectName,const TInt aBalance, TBool aNegative)
	{
	CTestMmfAclntRecBalance* self = new (ELeave) CTestMmfAclntRecBalance(aTestName, aSectName, aBalance, aNegative);
	return self;
	}

/**
 * Set recorder balance
 */
TVerdict CTestMmfAclntRecBalance::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF2(_L("TestRecorder : Balance - (%d)"), iBalance);

	TInt savedBalance = iBalance;
	if (CheckPlaybackBalance(aRecUtil) == EPass)
		{
		iBalance = savedBalance;
		if (CheckRecordBalance(aRecUtil) == EPass)
			return EPass;
		}

	return EFail;
	}

TVerdict CTestMmfAclntRecBalance::CheckPlaybackBalance(CMdaAudioRecorderUtility* aRecUtil)
	{
	TInt err = 0;
	TUint theNumChanel = 0;
	TRAP(err, theNumChanel = aRecUtil->DestinationNumberOfChannelsL());
	if(err != KErrNone)
		{
		return EFail;
		}
		
	
	TInt balance;

	if (iBalance < KMinBalance)
		{
		err = aRecUtil->SetPlaybackBalance(iBalance);

		if ((err == KErrArgument) && (iNegative))
			{
			return EPass;
			}
		else if ((err == KErrArgument) && (!iNegative))
			{
			return EFail;
			}

		aRecUtil->GetPlaybackBalance(iBalance);	
		if(iBalance == KMinBalance)
			{
			return EPass;
			}	
		}
	else if (iBalance > KMaxBalance)
		{
		err = aRecUtil->SetPlaybackBalance(iBalance);

		if ((err == KErrArgument) && (iNegative))
			{
			return EPass;
			}
		else if ((err == KErrArgument) && (!iNegative))
			{
			return EFail;
			}

		aRecUtil->GetPlaybackBalance(iBalance);		
		if(iBalance == KMaxBalance)
			{
			return EPass;
			}
			
		}
	else
		{
		err = aRecUtil->SetPlaybackBalance(iBalance);
		if(theNumChanel <= 1)
			{
			if(err != KErrNone)	// this is now supported
				return EFail;
			}
		if ((err == KErrArgument) && (iNegative))
			{
			return EPass;
			}
		else if ((err == KErrArgument) && (!iNegative))
			{
			return EFail;
			}
		else if(err != KErrNone)
			{
			return EFail;
			}
			

		TInt	theRes = aRecUtil->GetPlaybackBalance(balance);	
		if((theRes == KErrNone) && (balance == iBalance))
			{
			return EPass;
			}
		}

	return EFail;
	}

TVerdict CTestMmfAclntRecBalance::CheckRecordBalance(CMdaAudioRecorderUtility* aRecUtil)
	{
	TInt err = 0;
	TUint theNumChanel = 0;
	TRAP(err, theNumChanel = aRecUtil->DestinationNumberOfChannelsL());
	if(err != KErrNone)
		{
		return EFail;
		}

	if (iBalance < KMinBalance)
		{
		err = aRecUtil->SetRecordBalance(iBalance);

		if ((err == KErrArgument) && (iNegative))
			{
			return EPass;
			}
		else if ((err == KErrArgument) && (!iNegative))
			{
			return EFail;
			}

		aRecUtil->GetRecordBalance(iBalance);			
		if(iBalance == KMinBalance)
			{
			return EPass;
			}
			
		}
	else if (iBalance > KMaxBalance)
		{
		err = aRecUtil->SetRecordBalance(iBalance);	
		
		if ((err == KErrArgument) && (iNegative))
			{
			return EPass;
			}
		else if ((err == KErrArgument) && (!iNegative))
			{
			return EFail;
			}

		aRecUtil->GetRecordBalance(iBalance);		
		if(iBalance == KMaxBalance)
			{
			return EPass;
			}
			
		}
	else
		{
		TInt balance;
		err = aRecUtil->SetRecordBalance(iBalance);
		if(theNumChanel <= 1)
			{
			if(err != KErrNone)		// this is now supported
				return EFail;
			}

		if ((err == KErrArgument) && (iNegative))
			{
			return EPass;
			}
		else if ((err == KErrArgument) && (!iNegative))
			{
			return EFail;
			}

		aRecUtil->GetRecordBalance(balance);		
		if(balance == iBalance)
			{
			return EPass;
			}
		}

	return EFail;
	}

//------------------------------------------------------------------

CTestMmfAclntRecLength::CTestMmfAclntRecLength(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{
	}

CTestMmfAclntRecLength* CTestMmfAclntRecLength::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecLength* self = new (ELeave) CTestMmfAclntRecLength(aTestName, aSectName, aNegative);
	return self;
	}

/** 
 *Set maximum length of file in bytes and record
 */
TVerdict CTestMmfAclntRecLength::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecorder : SetMaxWriteLength"));

	TVerdict ret = EFail;
	TTimeIntervalMicroSeconds recTime(aRecUtil->RecordTimeAvailable());
	
	aRecUtil->SetMaxWriteLength(800);

	if(aRecUtil->RecordTimeAvailable() != recTime)
		{
		ret = EPass;
		}
		
	return	ret;
	}

//------------------------------------------------------------------

CTestMmfAclntRecFormats::CTestMmfAclntRecFormats(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{
	}

CTestMmfAclntRecFormats* CTestMmfAclntRecFormats::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecFormats* self = new (ELeave) CTestMmfAclntRecFormats(aTestName, aSectName, aNegative);
	return self;
	}

/**
 * Get recordable formats.Test for supported mimetypes. 
 */
TVerdict CTestMmfAclntRecFormats::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecorder : Formats"));

	TVerdict ret = EPass;
	
	TInt recauMatches = 0;
	TInt playauMatches = 0;
	TInt recwavMatches = 0;
	TInt playwavMathces	= 0;
	
	const TInt KControllerAudioRecAuMatches = 4;
	const TInt KControllerAudioPlayAuMatches = 4;
	const TInt KControllerAudioRecWavMatches = 2;
	// there are 2 .wav play plugins - the standard .WAV plugin and
	// the test MmfMp3Format plugin with 2 mime types each :
	const TInt KControllerAudioPlayWavMatches = 2;

	RArray<TFourCC> codecs;
	aRecUtil->GetSupportedDestinationDataTypesL(codecs);
	TInt numCodecs = codecs.Count();

	INFO_PRINTF2(_L("GetSupportedDestinationDataTypesL found %d codecs"), numCodecs);

	// enhanced - to use CMMFControllerPluginSelectionParameters

	INFO_PRINTF1(_L("Querying record formats..."));

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* recFSelect = CMMFFormatSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* playFSelect = CMMFFormatSelectionParameters::NewLC();

	RArray<TUid> mediaIds;
	mediaIds.Append(KUidMediaTypeAudio);
	cSelect->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	cSelect->SetRequiredRecordFormatSupportL(*recFSelect); 	
	cSelect->SetRequiredPlayFormatSupportL(*playFSelect); 

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	TInt numControllers = controllers.Count();
	if(!numControllers)
		{
		ERR_PRINTF1(_L("Could not find any controllers"));
		}
	else
		{
		INFO_PRINTF2(_L("Found %d controllers"), numControllers);
		for(int i = 0; i < numControllers; i++)
			{
			INFO_PRINTF4(_L("- Controller: %d Uid: 0x%X  %S"),i, controllers[i]->Uid(), &controllers[i]->DisplayName());
			const RMMFFormatImplInfoArray& recFormats = controllers[i]->RecordFormats();
			const RMMFFormatImplInfoArray& playFormats = controllers[i]->PlayFormats();

			const CDesC8Array* fileExtensions = NULL;
			const CDesC8Array* mimeTypes = NULL;
			
			_LIT(KMimeTypeAudioBasic,	"audio/basic");
			_LIT(KMimeTypeAudioXAu, 	"audio/x-au");
			_LIT(KMimeTypeAudioAu, 		"audio/au");
			_LIT(KMimeTypeAudioXBasic,	"audio/x-basic");
			_LIT(KMimeTypeAudioWav, 	"audio/wav");
			_LIT(KMimeTypeAudioXWav, 	"audio/x-wav");
						
			_LIT(KWavFileExtension,		".wav");
			_LIT(KAuFileExtension,		".au");
			
				
			for(int recFormat = 0; recFormat < recFormats.Count(); recFormat++)
				{
				INFO_PRINTF2(_L("Record Format 0x%x"), recFormats[recFormat]->Uid());
				fileExtensions = &recFormats[recFormat]->SupportedFileExtensions();
				INFO_PRINTF2(_L("File Extensions %d"), (fileExtensions ? fileExtensions->MdcaCount() : 0));
				TBuf<24> fileType;
				for(int fileExtn = 0; fileExtn < (fileExtensions ? fileExtensions->MdcaCount() : 0); fileExtn++)
					{
					fileType.Copy(fileExtensions->MdcaPoint(fileExtn));
					INFO_PRINTF2(_L("-- %S"), &fileType);
					}
				
				mimeTypes = &recFormats[recFormat]->SupportedMimeTypes();
				INFO_PRINTF2(_L("Mime Types: %d"), (mimeTypes ? mimeTypes->MdcaCount() : 0));
				for(int mimeType = 0; mimeType < (mimeTypes ? mimeTypes->MdcaCount() : 0); mimeType++)
					{
					TBuf<24> ext16;
					ext16.Copy(mimeTypes->MdcaPoint(mimeType));								
					if(fileType.Compare(KAuFileExtension)== KErrNone)
						{
						switch (mimeType)
							{
							case 0:
								if (ext16 == KMimeTypeAudioBasic)
									{
									recauMatches++;
									}
									
						 		else
						 			{
						 			ret = EFail;
						 			}
									
								break;
							case 1:
								if (ext16 == KMimeTypeAudioXAu)
									{
									recauMatches++;
									}
									
								else
									{
									ret = EFail;
									}
									
								break;
							case 2:
								if(ext16 == KMimeTypeAudioAu)
									{
									recauMatches++;
									}
									
								else
									{
									ret = EFail;	
									}
								break;
								
							case 3:
								if(ext16 == KMimeTypeAudioXBasic)
									{
									recauMatches++;
									}
									
								else	
									{
									ret = EFail;
									}
									
								break;
							default:
								break;
							}
						INFO_PRINTF2(_L("-- %S"), &ext16);								
						}
					if(fileType.Compare(KWavFileExtension) == KErrNone)
						{
						switch (mimeType)
							{
							case 0:
								if(ext16 == KMimeTypeAudioWav)
									{
									recwavMatches++;
									}
									
								else
									{
									ret = EFail;
									}
									
								break;
							case 1:
								if(ext16 == KMimeTypeAudioXWav)
									{
									recwavMatches++;
									}
									
								else
									{
									ret = EFail;
									}
								
								break;
								
							default:
								break;
							}
						INFO_PRINTF2(_L("-- %S"), &ext16);				
						}
					} //mimetype
				} //recFormat

			for(int playFormat = 0; playFormat < playFormats.Count(); playFormat++)
				{
				INFO_PRINTF2(_L("Play Format 0x%x"), playFormats[playFormat]->Uid());
				fileExtensions = &playFormats[playFormat]->SupportedFileExtensions();
				INFO_PRINTF2(_L("File Extensions %d"), (fileExtensions ? fileExtensions->MdcaCount() : 0));
				TBuf<24> fileType;
				for(int fileExtn = 0; fileExtn < (fileExtensions ? fileExtensions->MdcaCount() : 0); fileExtn++)
					{
					fileType.Copy(fileExtensions->MdcaPoint(fileExtn));
					INFO_PRINTF2(_L("-- %S"), &fileType);
					}
				mimeTypes = &playFormats[playFormat]->SupportedMimeTypes();
				INFO_PRINTF2(_L("Mime Types: %d"), (mimeTypes ? mimeTypes->MdcaCount() : 0));
				for(int mimeType = 0; mimeType < (mimeTypes ? mimeTypes->MdcaCount() : 0); mimeType++)
					{
					TBuf<24> ext16;
					ext16.Copy(mimeTypes->MdcaPoint(mimeType));							
					if(fileType.Compare(KAuFileExtension) == KErrNone)
						{
						switch (mimeType)
							{
							case 0:
								if (ext16 == KMimeTypeAudioBasic)
									{
									playauMatches++;
									}
									
								else
									{
									ret = EFail;
									}
									
								break;
							case 1:
								if (ext16 == KMimeTypeAudioXAu)
									{
									playauMatches++;
									}
									
								else
									ret = EFail;
								break;
							case 2:
								if(ext16 == KMimeTypeAudioAu)
									playauMatches++;
								else
									ret = EFail;	
								break;
							case 3:
								if(ext16 == KMimeTypeAudioXBasic)
									{
									playauMatches++;
									}
								else	
									ret = EFail;
								break;
							default:
							break;
							}
						INFO_PRINTF2(_L("-- %S"), &ext16);								
						}
					if(fileType.Compare(KWavFileExtension)== KErrNone)
						{
						switch (mimeType)
							{
							case 0:
								if(ext16 == KMimeTypeAudioWav)
									{
									playwavMathces++;
									}
									
								else
									ret = EFail;
								break;
							case 1:
								if(ext16 == KMimeTypeAudioXWav)
									{
									playwavMathces++;
									}
									
								else
									ret = EFail;
								break;
							default:
								break;
							}
						INFO_PRINTF2(_L("-- %S"), &ext16);				
						}
					}//mimetype	
				} //playFormat
			}	//controllers
		}//else condition
		
	CleanupStack::PopAndDestroy(4);//controllers, recFSelect, playFSelect cSelect
	
	if((recauMatches != KControllerAudioRecAuMatches || playauMatches != KControllerAudioPlayAuMatches) ||
	(recwavMatches != KControllerAudioRecWavMatches || playwavMathces != KControllerAudioPlayWavMatches))
		{
		ret = EFail;
		}
		
	else
		ret =  EPass;

	return ret;
	} //end of function

		
//------------------------------------------------------------------

// Negative tests

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecDelete::CTestMmfAclntRecDelete(const TDesC& aTestName, const TDesC& aSectName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName; 
	iSectName = aSectName;
	}

void CTestMmfAclntRecDelete::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecDelete : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecDelete::DoTestStepPreambleL()
	{
	TVerdict verdict = EPass;

	// Delete the output file (will probably have been used by other tests)
	RFs fs;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		verdict = EFail;
		}
	else
		{
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if (!GetStringFromConfig(iSectName, _L("outputAudioFile"), filename1))
			{
			verdict = EInconclusive;
			}
		else
			{
			GetDriveName(filename);
			filename.Append(filename1);
			err = fs.Delete(filename);
			if (!((err == KErrNone) || (err == KErrNotFound)))
			{
				verdict = EInconclusive;
			}
				
			}
		fs.Close();
		}

	if (verdict == EPass)
		{
		verdict = CTestMmfAclntStep::DoTestStepPreambleL();
		}
		

	return verdict;
	}

/**
 * Record utility - Delete object before record operation has completed.
 */
TVerdict CTestMmfAclntRecDelete::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorder : Delete"));
	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName, _L("outputAudioFile"), filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);

	__MM_HEAP_MARK;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);
	
	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRec"))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		}
	
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &filename);
	recUtil->OpenFileL(filename);
	// wait
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if (iError == KErrNone)
		{
		TVerdict ret = EPass;
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		recUtil->RecordL();
		INFO_PRINTF1( _L("Destroy CMdaAudioRecorderUtility"));
		CleanupStack::PopAndDestroy(recUtil);

		// Check that the file no longer exists
		RFs fs;
		TInt err = fs.Connect();
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Error connecting to file server %d"), err);
			ret = EInconclusive;
			}
		else
			{
			TEntry entry;
			if ((fs.Entry(filename, entry) == KErrNone) && (entry.iSize > 0))
				{
				ERR_PRINTF1(_L("ERROR: File exists and size is greater than ZERO"));
				ret = EFail;	// file exists - test fails	
				}
			else
				{
				ret = EPass;
				}
			fs.Close();
			}

		__MM_HEAP_MARKEND;
		User::Heap().Check(); 

		return ret;
		}
	CleanupStack::PopAndDestroy(recUtil);
	return	EFail;
	}


//------------------------------------------------------------------

CTestMmfAclntAudioRecordVolume::CTestMmfAclntAudioRecordVolume(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative, const TInt aVolume)
	: CTestMmfAclntRecord(aTestName, aSectName,aNegative), iVolume(aVolume)
	{}

CTestMmfAclntAudioRecordVolume* CTestMmfAclntAudioRecordVolume::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative, const TInt aVolume)
	{
	CTestMmfAclntAudioRecordVolume* self = new (ELeave) CTestMmfAclntAudioRecordVolume(aTestName, aSectName,aNegative,aVolume);
	return self;
	}


TVerdict CTestMmfAclntAudioRecordVolume::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecordUtils : Audio Record Volume"));
	TVerdict ret = EFail;


	// Check maxvolume function
	if(iVolume == -1)
		{
		iVolume = aRecUtil->MaxVolume();
		}
	// Volume is truncated to maxvolume
	if(iVolume > aRecUtil->MaxVolume())
		{
		TInt volume;
		aRecUtil->SetVolume(iVolume);
		aRecUtil->GetVolume(volume);
		if(volume == aRecUtil->MaxVolume())
			{
			ret = EPass;
			}
		}
	// Volume is truncated to 0
	else if(iVolume < 0)
		{
		TInt volume;

		aRecUtil->SetVolume(iVolume);
		aRecUtil->GetVolume(volume);
		if(volume == 0)
			{
			ret = EPass;
			}
		}
	// Set volume and check
	else
		{
		TInt volume;
		aRecUtil->SetVolume(iVolume);
		aRecUtil->GetVolume(volume);
		if(volume == iVolume)
			{
			ret = EPass;
			}
			
		else
			{
			INFO_PRINTF3( _L("Expected volume: %d, received volume : %d"), iVolume, volume);
			}
		}

	return ret;
	}


CTestMmfAclntAudioRecordRamp::CTestMmfAclntAudioRecordRamp(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative)
	: CTestMmfAclntRecord(aTestName, aSectName,aNegative)
	{}

CTestMmfAclntAudioRecordRamp* CTestMmfAclntAudioRecordRamp::NewL(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative)
	{
	CTestMmfAclntAudioRecordRamp* self = new (ELeave) CTestMmfAclntAudioRecordRamp(aTestName, aSectName,aNegative);
	return self;
	}

TVerdict CTestMmfAclntAudioRecordRamp::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecordUtils : Audio Ramp"));
	TVerdict ret = EFail;
	TTimeIntervalMicroSeconds ramp(100);
	aRecUtil->SetVolumeRamp(ramp);

	aRecUtil->RecordL();
	CActiveScheduler::Start(); // wait for open -> record
	if (iError != KErrNone)
		{
		// DEF127335
	   	// OggRecordController does not support recording into an already existing file.
	   	// This is due to unavailability of APIs at the level of Ogg Vorbis C libraries.
		if (iTestStepName == _L("MM-MMF-ACLNTOGG-I-0123-HP") && iError == KErrNotSupported)
			{
			INFO_PRINTF1(_L("Expected Result: OggRecordController returned KErrNotSupported(-5)"));
			return EPass;
			}
		else 
			{
			return EFail;	
			}
		}
		
	iReportedState = aRecUtil->State();
	if(iReportedState == CMdaAudioClipUtility::ERecording )
		{
		ret = EPass;
		}
		
	User::After(KOneSecond);
	aRecUtil->Stop();

	return ret;
	}


//------------------------------------------------------------------


CTestMmfAclntRecordDestChannels::CTestMmfAclntRecordDestChannels(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aNumberOfChannels,const TTestFormat aFormat, const TBool aCreateDes)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateDes = aCreateDes;
	iNumberOfChannels=aNumberOfChannels;

	iSize = 0;
	iAudio = NULL;
	}

CTestMmfAclntRecordDestChannels* CTestMmfAclntRecordDestChannels::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aNumberOfChannels, const TTestFormat aFormat,const TBool aCreateDes)
	{
	CTestMmfAclntRecordDestChannels* self = new (ELeave) CTestMmfAclntRecordDestChannels(aTestName,aSectName,aKeyName,aNumberOfChannels,aFormat,aCreateDes);
	return self;
	}

CTestMmfAclntRecordDestChannels* CTestMmfAclntRecordDestChannels::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aNumberOfChannels, const TTestFormat aFormat, const TBool aCreateDes)
	{
	CTestMmfAclntRecordDestChannels* self = CTestMmfAclntRecordDestChannels::NewLC(aTestName,aSectName,aKeyName,aNumberOfChannels,aFormat,aCreateDes);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntRecordDestChannels::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF4( _L("MMdaObjectStateChangeObserver: previous state: %d current state: %d error: %d"),aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecordDestChannels::DoTestStepPreambleL()
	{
	SetupFormatL(iTestFormat);

	switch (iTestFormat) 
		{
		case EPcm16Wav:
		case EAlawWav:
		case EPcm8:
			iHeaderSize = KWavFileUncompressedDataHeaderSize; //(see mmfwavformat.cpp, CreateSinkBufferOfSizeL())
			iFrameSize = 4096;
			break;
		case EImaAdpcmWav:
		case EImasPcmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
			iFrameSize = 4096;
			break;
		case EMulawRaw:
		case EPcmU8:
		case EPcmU16:
			iHeaderSize = 0;
			iFrameSize = 4096;
			break;
		case EGsmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
			iFrameSize = 4096;
			break;
		default:
			/*skip*/ ;
		}

	if(!iCreateDes)
		{
		RFs fs;
		RFile file;

		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			return EInconclusive;
		GetDriveName(filename);
		filename.Append(filename1);
		INFO_PRINTF2(_L("File for test - %S"), &filename);
		
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(file.Open(fs,filename,EFileRead));
		CleanupClosePushL(file);
		
		User::LeaveIfError(file.Size(iSize));
		INFO_PRINTF2(_L("size of file = %d\n"),iSize);//Statement Changed under DEF105143
		iAudio = HBufC8::NewMaxL(iSize);

		// Read a file into a descriptor
		TPtr8 bufferDes(iAudio->Des());
		User::LeaveIfError(file.Read(bufferDes));

		CleanupStack::PopAndDestroy(); //file
		}
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

TVerdict CTestMmfAclntRecordDestChannels::DoTestStepPostambleL()
	{
	delete iAudio;
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Open a descriptor and record
 */
TVerdict CTestMmfAclntRecordDestChannels::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorder : Record Des"));
	TVerdict ret = EFail;
	HBufC8* audio = NULL;
	TMdaDesClipLocation* location = NULL;
	TInt err = KErrNone;
	RArray<TUint> channels;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);
	
	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRec"))
		{
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
			
		GetDriveName(filename);
		filename.Append(filename1);
		
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		recUtil->OpenFileL(filename);
		}
	else if(!iCreateDes)
		{
		audio=iAudio;
		TPtr8 bufferDes(iAudio->Des());
		recUtil->OpenDesL(bufferDes);
		}
	else
		{
		iSize = KRecSize; // value is not been set yet.
		audio = HBufC8::NewMaxLC(iSize);
		TPtr8 bufferDes(audio->Des());
		bufferDes.SetLength(0);
		location = new (ELeave) TMdaDesClipLocation(bufferDes);
		recUtil->OpenL(location,iFormat,iCodec,&iAudioSettings);
		
		delete location; 		
		}

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();
	TInt numChannels=-1;
	if(iError == KErrNone)
		{
		TRAP(err,recUtil->GetSupportedNumberOfChannelsL(channels));
		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::GetSupportedNumberOfChannelsL leave with error %d"),err);		
			if(iCreateDes)
				{
				CleanupStack::PopAndDestroy(audio);
				}
				
			CleanupStack::PopAndDestroy(recUtil);	
			User::After(KOneSecond);
			return ret;
			}
		TRAP(err,recUtil->SetDestinationNumberOfChannelsL(1));//@@@
		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationNumberOfChannelsL leave with error %d"),err);		
			if(iCreateDes)
				{
				CleanupStack::PopAndDestroy(audio);
				}
				
			CleanupStack::PopAndDestroy(recUtil);	
			User::After(KOneSecond);
			return ret;
			}
		TRAP(err,numChannels = recUtil->DestinationNumberOfChannelsL());
		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationNumberOfChannelsL leave with error %d"),err);		
			if(iCreateDes)
				{
				CleanupStack::PopAndDestroy(audio);
				}
				
			CleanupStack::PopAndDestroy(recUtil);	
			User::After(KOneSecond);
			return ret;
			}
		}

	if(numChannels==iNumberOfChannels)
		{
		ret = EPass;
		}
		

	if (ret != EPass)
		{
		ERR_PRINTF2( _L("CMdaAudioRecorderUtility failed with error %d"),iError );
		}
		
	if(iCreateDes)
		{
		CleanupStack::PopAndDestroy(audio);
		}
		
	CleanupStack::PopAndDestroy(recUtil);	
	User::After(KOneSecond);
	return ret;
	}



//------------------------------------------------------------------

CTestMmfAclntRecordSetPriority::CTestMmfAclntRecordSetPriority(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	: CTestMmfAclntRecord(aTestName, aSectName,aNegative)
	{}

CTestMmfAclntRecordSetPriority* CTestMmfAclntRecordSetPriority::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecordSetPriority* self = new (ELeave) CTestMmfAclntRecordSetPriority(aTestName, aSectName,aNegative);
	return self;
	}


TVerdict CTestMmfAclntRecordSetPriority::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecordUtils : SetPriority"));
	TVerdict ret = EPass;
	TInt err = KErrNone;
	TRAP(err,aRecUtil->SetPriority(EMdaPriorityMin,EMdaPriorityPreferenceNone));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetPriority (1st call) leave with error %d"),err);
		ret = EFail;
		}
	
	TRAP(err,aRecUtil->SetPriority(EMdaPriorityNormal,EMdaPriorityPreferenceTime));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetPriority (2nd call) leave with error %d"),err);
		ret = EFail;
		}
	
	TRAP(err,aRecUtil->SetPriority(EMdaPriorityMax,EMdaPriorityPreferenceTimeAndQuality));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetPriority (3rd call) leave with error %d"),err);
		ret = EFail;
		}
	return ret;
	}


//------------------------------------------------------------------

CTestMmfAclntRecordDestSampleRate::CTestMmfAclntRecordDestSampleRate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TTestFormat aFormat, const TBool aCreateDes)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateDes = aCreateDes;

	iSize = 0;
	iAudio = NULL;
	}

CTestMmfAclntRecordDestSampleRate* CTestMmfAclntRecordDestSampleRate::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat,const TBool aCreateDes)
	{
	CTestMmfAclntRecordDestSampleRate* self = new (ELeave) CTestMmfAclntRecordDestSampleRate(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	return self;
	}

CTestMmfAclntRecordDestSampleRate* CTestMmfAclntRecordDestSampleRate::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateDes)
	{
	CTestMmfAclntRecordDestSampleRate* self = CTestMmfAclntRecordDestSampleRate::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntRecordDestSampleRate::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF4( _L("MMdaObjectStateChangeObserver: previous state: %d current state: %d error: %d"),aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecordDestSampleRate::DoTestStepPreambleL()
	{
	SetupFormatL(iTestFormat);

	switch (iTestFormat) 
		{
		case EPcm16Wav:
		case EAlawWav:
		case EPcm8:
			iHeaderSize = KWavFileUncompressedDataHeaderSize; //(see mmfwavformat.cpp, CreateSinkBufferOfSizeL())
			iFrameSize = 4096;
			break;
		case EImaAdpcmWav:
		case EImasPcmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
			iFrameSize = 4096;
			break;
		case EMulawRaw:
		case EPcmU8:
		case EPcmU16:
			iHeaderSize = 0;
			iFrameSize = 4096;
			break;
		case EGsmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
			iFrameSize = 4096;
			break;
		default:
			/*skip*/ ;
		}

	if(!iCreateDes)
		{
		RFs fs;
		RFile file;

		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			return EInconclusive;
		GetDriveName(filename);
		filename.Append(filename1);
		INFO_PRINTF2(_L("File for test - %S"), &filename);
		
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(file.Open(fs,filename,EFileRead));
		CleanupClosePushL(file);

		User::LeaveIfError(file.Size(iSize));
		INFO_PRINTF2(_L("size of file = %d\n"),iSize);//Statement Changed under DEF105143
		iAudio = HBufC8::NewMaxL(iSize);

		// Read a file into a descriptor
		TPtr8 bufferDes(iAudio->Des());
		User::LeaveIfError(file.Read(bufferDes));

		CleanupStack::PopAndDestroy(); //file
		}
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

TVerdict CTestMmfAclntRecordDestSampleRate::DoTestStepPostambleL()
	{
	delete iAudio;
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Open a descriptor and record
 */
TVerdict CTestMmfAclntRecordDestSampleRate::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorder : Destination SampleRate"));
	TVerdict ret = EPass;
	HBufC8* audio = NULL;
	TMdaDesClipLocation* location = NULL;
	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);
	
	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRec"))
		{
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			return EInconclusive;
		GetDriveName(filename);
		filename.Append(filename1);
		
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		recUtil->OpenFileL(filename);
		}
	else if(!iCreateDes)
		{
		audio=iAudio;
		TPtr8 bufferDes(iAudio->Des());
		recUtil->OpenDesL(bufferDes);
		}
	else
		{
		iSize = KRecSize; // value is not been set yet.
		audio = HBufC8::NewMaxLC(iSize);
		TPtr8 bufferDes(audio->Des());
		bufferDes.SetLength(0);
		location = new (ELeave) TMdaDesClipLocation(bufferDes);
		recUtil->OpenL(location,iFormat,iCodec,&iAudioSettings);
		
		delete location;
		}
	CActiveScheduler::Start();

	iError = KErrTimedOut;
	TInt err = KErrNone;
	RArray<TUint> rates;
	TRAP(err,recUtil->GetSupportedSampleRatesL(rates));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::GetSupportedSampleRatesL leave with error %d"),err);
		ret = EFail;
		if(iCreateDes)
			CleanupStack::PopAndDestroy(audio);
		CleanupStack::PopAndDestroy(recUtil);	
		User::After(KOneSecond);
		return ret;
		}
	
	TInt numRates = rates.Count();
	for(TInt i=0;i<numRates;i++)
		{

		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationSampleRateL(%d)"),rates[i]);
		TUint sr = rates[i];
		TRAP(err,recUtil->SetDestinationSampleRateL(sr));
		if(err!=KErrNone)
			{
			INFO_PRINTF3(_L("CMdaAudioRecorderUtility::SetDestinationSampleRateL(%d) leave with error : %d"),sr,err);
			ret = EFail;
			break;
			}

			TUint rate=0;
			TRAP(err,rate = recUtil->DestinationSampleRateL());
			if(err!=KErrNone)
				{
				INFO_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationSampleRateL leave with error %d"),err);
				ret = EFail;
				}
			else
				{
				if(rate!=rates[i])
					{
					ret = EFail;
					INFO_PRINTF1(_L("Rate mismatch"));
					}
					else
					{	
					INFO_PRINTF1(_L("Rates match"));
					}
				}
		
		}
	if(iCreateDes)
		CleanupStack::PopAndDestroy(audio);
	CleanupStack::PopAndDestroy(recUtil);	
	User::After(KOneSecond);
	return ret;
	}	
//------------------------------------------------------------------

CTestMmfAclntRecordDestBitRate::CTestMmfAclntRecordDestBitRate(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	: CTestMmfAclntRecord(aTestName, aSectName, aNegative)
	{}

CTestMmfAclntRecordDestBitRate* CTestMmfAclntRecordDestBitRate::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecordDestBitRate* self = new (ELeave) CTestMmfAclntRecordDestBitRate(aTestName, aSectName, aNegative);
	return self;
	}


TVerdict CTestMmfAclntRecordDestBitRate::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecordUtils : Destination Bit Rate"));
	TVerdict ret = EPass;
	TInt err = KErrNone;
	
	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRec"))
		{
		TPtrC filename;
		if (!GetStringFromConfig(iSectName, _L("outputAudioFile"), filename))
			{
			INFO_PRINTF1(_L("Error getting filename from INI file"));
			return EInconclusive;
			}
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		// Open the file
		aRecUtil->OpenFileL(filename);
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &filename);
		CActiveScheduler::Start();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Error opening file for recording"), iError);
			return EInconclusive;
			}
		}
	
	RArray<TUint> rates;
	TRAP(err,aRecUtil->GetSupportedBitRatesL(rates));
	// in Typhoon KErrNotSupported is expected
	if(err==KErrNotSupported)
		{
		//test the other bitrate functions for code coverage (and behaviour)
		TRAP(err,aRecUtil->SetDestinationBitRateL(100)); // arbitrary value since KErrNotSupported is expected anyway
		if (err != KErrNotSupported)
			{
			return EFail;
			}
			
		TInt rate=0;
		TRAP(err, rate=aRecUtil->DestinationBitRateL());
		if (err != KErrNone)
			{
			return EFail;
			}
			
		INFO_PRINTF3(_L("CMdaAudioRecorderUtility::GetSupportedBitRatesL() leave with error %d bitrate of %d"),err, rate);
		ret = EPass;
		return ret;
		}
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::GetSupportedBitRatesL leave with error %d"),err);
		ret = EFail;
		return ret;
		}
	
	TInt numRates = rates.Count();
	for(TInt i=0;i<numRates;i++)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationBitRateL (%d)"),rates[i]);
		TRAP(err,aRecUtil->SetDestinationBitRateL(rates[i]));
		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationBitRateL leave with error %d"),err);
			ret = EFail;
			}
		else// no error, get rate
			{
			TUint rate=0;
			TRAP(err,rate = aRecUtil->DestinationBitRateL());
			if(err!=KErrNone)
				{
				INFO_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationBitRateL leave with error %d"),err);
				ret = EFail;
				}
			else
				{
				if(rate!=rates[i])
					{
					ret = EFail;
					INFO_PRINTF1(_L("Rate mismatch"));
					}
					else
					{	
					INFO_PRINTF1(_L("Rates match"));
					}
				}
			}
		}
	return ret;
	}


//------------------------------------------------------------------



CTestMmfAclntRecordDestDataType::CTestMmfAclntRecordDestDataType(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateDes)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateDes = aCreateDes;
	iSize = 0;
	iAudio = NULL;
	}

CTestMmfAclntRecordDestDataType* CTestMmfAclntRecordDestDataType::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat,const TBool aCreateDes)
	{
	CTestMmfAclntRecordDestDataType* self = new (ELeave) CTestMmfAclntRecordDestDataType(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	return self;
	}

CTestMmfAclntRecordDestDataType* CTestMmfAclntRecordDestDataType::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateDes)
	{
	CTestMmfAclntRecordDestDataType* self = CTestMmfAclntRecordDestDataType::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntRecordDestDataType::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF4( _L("MMdaObjectStateChangeObserver: previous state: %d current state: %d error: %d"),aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecordDestDataType::DoTestStepPreambleL()
	{
	SetupFormatL(iTestFormat);

	switch (iTestFormat) 
		{
		case EPcm16Wav:
		case EAlawWav:
		case EPcm8:
			iHeaderSize = KWavFileUncompressedDataHeaderSize; //(see mmfwavformat.cpp, CreateSinkBufferOfSizeL())
			iFrameSize = 4096;
			break;
		case EImaAdpcmWav:
		case EImasPcmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
			iFrameSize = 4096;
			break;
		case EMulawRaw:
		case EPcmU8:
		case EPcmU16:
			iHeaderSize = 0;
			iFrameSize = 4096;
			break;
		case EGsmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
			iFrameSize = 4096;
			break;
		default:
			/*skip*/ ;
		}

	if(!iCreateDes)
		{
		RFs fs;
		RFile file;

		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
			
		GetDriveName(filename);
		filename.Append(filename1);
		INFO_PRINTF2(_L("File for test - %S"), &filename);
		
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(file.Open(fs,filename,EFileRead));
		CleanupClosePushL(file);

		User::LeaveIfError(file.Size(iSize));
		INFO_PRINTF2(_L("size of file = %d\n"),iSize);//Statement Changed under DEF105143
		iAudio = HBufC8::NewMaxL(iSize);

		// Read a file into a descriptor
		TPtr8 bufferDes(iAudio->Des());
		User::LeaveIfError(file.Read(bufferDes));

		CleanupStack::PopAndDestroy(); //file
		}
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

TVerdict CTestMmfAclntRecordDestDataType::DoTestStepPostambleL()
	{
	delete iAudio;
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Open a descriptor and record
 */
TVerdict CTestMmfAclntRecordDestDataType::DoTestStepL( void )
	{
	HBufC8* audio = NULL;
	TMdaDesClipLocation* location = NULL;
	TInt err = KErrNone;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRec"))
		{
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 
		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
			
		GetDriveName(filename);
		filename.Append(filename1);
		
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		recUtil->OpenFileL(filename);
		}
	else if(!iCreateDes)
		{
		audio=iAudio;
		TPtr8 bufferDes(iAudio->Des());
		recUtil->OpenDesL(bufferDes);
		}
	else
		{
		iSize = KRecSize; // value is not been set yet.
		audio = HBufC8::NewMaxLC(iSize);
		TPtr8 bufferDes(audio->Des());
		bufferDes.SetLength(0);
		location = new (ELeave) TMdaDesClipLocation(bufferDes);
		recUtil->OpenL(location,iFormat,iCodec,&iAudioSettings);
		
		delete location; 		
		}
	CActiveScheduler::Start();

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("TestRecordUtils : Destination data type"));
	
	RArray<TFourCC> codecs;
	recUtil->GetSupportedDestinationDataTypesL(codecs);
	TInt numCodecs = codecs.Count();
	TVerdict ret = EPass;
	for(TInt i=0;i<numCodecs;i++)
		{
		TRAP(err,recUtil->SetDestinationDataTypeL(codecs[i]));
		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationDataTypeL leave with error %d"),err);
			if(iCreateDes)
				CleanupStack::PopAndDestroy(audio);
			CleanupStack::PopAndDestroy(recUtil);	
			User::After(KOneSecond);
			return ret;
			}
		TFourCC dataType;
		TRAP(err,dataType = recUtil->DestinationDataTypeL());
		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationDataTypeL leave with error %d"),err);
			if(iCreateDes)
				CleanupStack::PopAndDestroy(audio);
			CleanupStack::PopAndDestroy(recUtil);	
			User::After(KOneSecond);
			return ret;
			}
		if(dataType!=codecs[i])
			{
			ret = EFail;
			}
		else
			{
			INFO_PRINTF1(_L("CMdaAudioRecorderUtility::DestinationDataType set-get OK"));
			}
		}
	if(iCreateDes)
		{
		CleanupStack::PopAndDestroy(audio);
		}
		
	CleanupStack::PopAndDestroy(recUtil);	
	User::After(KOneSecond);
	return ret;
	}




//------------------------------------------------------------------

CTestMmfAclntRecordDestFormat::CTestMmfAclntRecordDestFormat(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative, const TUid aFormat)
	: CTestMmfAclntRecord(aTestName, aSectName, aNegative), iFormat(aFormat)
	{}

CTestMmfAclntRecordDestFormat* CTestMmfAclntRecordDestFormat::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative, const TUid aFormat)
	{
	CTestMmfAclntRecordDestFormat* self = new (ELeave) CTestMmfAclntRecordDestFormat(aTestName, aSectName, aNegative, aFormat);
	return self;
	}


TVerdict CTestMmfAclntRecordDestFormat::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	// GET --> SET --> GET
	INFO_PRINTF1( _L("TestRecordUtils : Destination format"));
	TVerdict ret = EFail;
	TInt err = KErrNone;
	TRAP(err,iFormat = aRecUtil->DestinationFormatL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationFormatL leave with error %d"),err);
		return ret;
		}
	TRAP(err,aRecUtil->SetDestinationFormatL(iFormat));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::SetDestinationFormatL leave with error %d"),err);
		return ret;
		}
	TUid format;
	TRAP(err,format = aRecUtil->DestinationFormatL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility::DestinationFormatL leave with error %d"),err);
		return ret;
		}
	if(format==iFormat)
		{
		ret = EPass;
		}
		
	return ret;
	}


//------------------------------------------------------------------

CTestMmfAclntRecordAudioDeviceMode::CTestMmfAclntRecordAudioDeviceMode(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	: CTestMmfAclntRecord(aTestName, aSectName, aNegative)
	{}

CTestMmfAclntRecordAudioDeviceMode* CTestMmfAclntRecordAudioDeviceMode::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecordAudioDeviceMode* self = new (ELeave) CTestMmfAclntRecordAudioDeviceMode(aTestName, aSectName, aNegative);
	return self;
	}


TVerdict CTestMmfAclntRecordAudioDeviceMode::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecordUtils : SetAudioDeviceMode"));
	INFO_PRINTF1( _L("This function is provided for binary compatibility, but does not have any effect from 7.0s"));

	// Set the audio device mode. This function is provided for binary compatibility, but
	// does not have any effect from 7.0s
	aRecUtil->SetAudioDeviceMode(CMdaAudioRecorderUtility::EDefault);
	aRecUtil->SetAudioDeviceMode(CMdaAudioRecorderUtility::ETelephonyOrLocal);
	aRecUtil->SetAudioDeviceMode(CMdaAudioRecorderUtility::ETelephonyMixed);
	aRecUtil->SetAudioDeviceMode(CMdaAudioRecorderUtility::ETelephonyNonMixed);
	aRecUtil->SetAudioDeviceMode(CMdaAudioRecorderUtility::ELocal);
	TVerdict ret = EPass;
	return ret;
	}








//
//
//	CMdaAudioInputStream Integration Tests //
//
//

/**
 * Standard static NewL() taking a callback function
 */
CCallBackTimer* CCallBackTimer::NewL(TCallBack aCallBack, TPriority aPriority)
	{
	CCallBackTimer* self = new(ELeave) CCallBackTimer(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/**
 * Private c'tor
 */
CCallBackTimer::CCallBackTimer(TCallBack aCallBack, TPriority aPriority)
: CTimer(aPriority),
iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

/*
 * Callback on timer complete
 */
void CCallBackTimer::RunL()
	{
	iCallBack.CallBack();
	}


_LIT(KStreamRawFile,"c:\\AclntITestData\\test.raw");
_LIT(KRecordStreamRawFile,"c:\\AclntITestData\\streamrecorded.raw");
CTestStepAudInStream* CTestStepAudInStream::NewL()
	{
	CTestStepAudInStream* s = new(ELeave) CTestStepAudInStream();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CTestStepAudInStream::CTestStepAudInStream() : iError(KErrNone), iState(EStopped)
	{
	
	}

void CTestStepAudInStream::ConstructL()
	{
	// reset the buffer
	for(TInt i=0; i<KRecNumBuffer; i++)
		{
		iBufferList[i] = KNullDesC8;
		}
	iStartBuf=iEndBuf=0;
	}

CTestStepAudInStream::~CTestStepAudInStream()
	{
	}

void CTestStepAudInStream::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudInStream::MaiscRecordComplete(TInt aError)
	{
	iError = aError;
	
	iWriteBufferActive->Cancel();
	TInt filePosition=0;
	iFile.Seek(ESeekStart, filePosition);
	iFile.Close();
	CActiveScheduler::Stop();
	}

void CTestStepAudInStream::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	if(aError != KErrNone)
		{
		return;   
		}
	// Assume aBuffer == iBufferList[iEndBuf]

	// Increment iEndBuf
	if(iEndBuf==KRecNumBuffer-1)
		{
		iEndBuf = 0;
		}
		
	else
		iEndBuf++;

	// Starting writing out the buffers if we're not already
	if(!iWriteBufferActive->IsActive())
		{
		iWriteBufferActive->Write(iBufferList[iStartBuf]);
		}
	}

void CTestStepAudInStream::StartReadL()
	{
	// Issue reads on all available buffers
	for (TInt ii=0; ii<KRecNumBuffer; ii++)
		{
		iAudInStream->ReadL(iBufferList[ii]);
		}
	}

void CTestStepAudInStream::Read1BufL()
	{
	// Issue reads on 1 available buffer
	iAudInStream->ReadL(iBufferList[0]);
	}	


CTestStepAudInStream::CWriteBufferActive::CWriteBufferActive() : CActive(0)
	{
	CActiveScheduler::Add(this);
	}

CTestStepAudInStream::CWriteBufferActive::~CWriteBufferActive()
	{
	Cancel();
	}

void CTestStepAudInStream::CWriteBufferActive::RunL()
	{
	//ASSERT it wrote OK
	iParent->ProcessDataL();
	}

TInt CTestStepAudInStream::CWriteBufferActive::RunError(TInt aError) 
	{
	iParent->iError = aError;
	CActiveScheduler::Stop();
	
	return KErrNone;
	}


void CTestStepAudInStream::CWriteBufferActive::DoCancel()
	{
	// do nothing
	}

void CTestStepAudInStream::CWriteBufferActive::Write(const TDesC8& aBuffer)
	{
	iParent->iFile.Write(aBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}


void CTestStepAudInStream::ProcessDataL()
	{
	// Set the next buffer to handle
	iBufferList[iStartBuf] = KNullDesC8; // done with that one
	// Issue another read to input stream
	iAudInStream->ReadL(iBufferList[iStartBuf]);
	
	// Increment iStartBuf
	if(iStartBuf==KRecNumBuffer-1)
		{
		iStartBuf=0;
		}
		
	else
		iStartBuf++;
	
	if (iBufferList[iStartBuf] != KNullDesC8)
		{
		// Still more to write
		iWriteBufferActive->Write(iBufferList[iStartBuf]);
		}
	else
		CActiveScheduler::Stop();

	// else just wait for more recorded data to come through
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStream::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;

	// reset the buffer [in case new test reuses previous object]
	for(TInt i=0; i<KRecNumBuffer; i++)
		{
		iBufferList[i] = KNullDesC8;
		}
	iStartBuf=iEndBuf=0;

	 // this installs the scheduler
	 verdict = CTestMmfAclntStep::DoTestStepPreambleL();

	// create the active object
	iWriteBufferActive = new(ELeave) CWriteBufferActive();
	iWriteBufferActive->iParent = this;
	iFs.Connect();

	TRAPD(err, iAudInStream = CMdaAudioInputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudInStream == NULL )
		{
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
enum TVerdict CTestStepAudInStream::DoTestStepPostambleL(void)
	{
	iFs.Close();

	delete iAudInStream;
	iAudInStream = NULL;
	delete iWriteBufferActive;
	iWriteBufferActive = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
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
TVerdict CTestStepAudInStream::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;

	if (iError != KErrNone )
		 return EFail;

	err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		return EFail;

	StartReadL();
	CActiveScheduler::Start();

	iFile.Close();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



CTestStepAudInStreamNoUECap* CTestStepAudInStreamNoUECap::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamNoUECap* self = new(ELeave) CTestStepAudInStreamNoUECap(aTestName);
	return self;
	}


CTestStepAudInStreamNoUECap::CTestStepAudInStreamNoUECap(const TDesC& aTestName)
	{
	iTestStepResult = EFail;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}
	

	
CTestStepAudInStreamNoUECap::~CTestStepAudInStreamNoUECap()
	{
	}


void CTestStepAudInStreamNoUECap::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudInStreamNoUECap::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	INFO_PRINTF1(_L("MMdaAudioInputStreamCallback::MaiscBufferCopied was not supposed to be called"));
	iTestStepResult = EFail;
	CActiveScheduler::Stop();
	}
void CTestStepAudInStreamNoUECap::MaiscRecordComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MMdaAudioInputStreamCallback::MaiscRecordComplete was called with aError = %d"),aError);
	iMaiscRecordCompleteError = aError;
	CActiveScheduler::Stop();
	}

TVerdict CTestStepAudInStreamNoUECap::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EFail;
	
	// Printing to the console and log file
	INFO_PRINTF1(_L("this test calls MediaAudioClientInputStream::ReadL func with NO UserEnvironment capability"));

	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 1: error = %d"),iError);
		return EInconclusive;
		}

	iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 2: error = %d"),err);
		return EInconclusive;
		}
	
	iState=ERecording;

	// start feeding the input stream
	TRAP(err, Read1BufL());
	CActiveScheduler::Start();
	if ( (err == KErrNone && iMaiscRecordCompleteError == KErrPermissionDenied) || err == KErrPermissionDenied )
		{
		INFO_PRINTF2(_L("Feeding input stream without UserEnvironent cap left with expected error = %d"),err);
		iTestStepResult = EPass;
		}
	else
		{
		INFO_PRINTF1(_L("Failed this test step with error %d"));
		INFO_PRINTF2(_L("Expected error = %d"),KErrPermissionDenied);
		INFO_PRINTF1(_L("The requesting client should not have the UserEnvironment cap in this test"));
		iTestStepResult = EFail;
		}
	// test steps return a result
	
	return iTestStepResult;
	}

CTestStepAudInStreamWithUECap* CTestStepAudInStreamWithUECap::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamWithUECap* self = new(ELeave) CTestStepAudInStreamWithUECap(aTestName);
	return self;
	}


CTestStepAudInStreamWithUECap::CTestStepAudInStreamWithUECap(const TDesC& aTestName)
	{
	iTestStepResult = EFail;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}
	

	
CTestStepAudInStreamWithUECap::~CTestStepAudInStreamWithUECap()
	{
	}


void CTestStepAudInStreamWithUECap::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


void CTestStepAudInStreamWithUECap::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	//if weve stopped recording, each unused buffer will be returned with KErrAbort
	if(iState == EStopped && aError == KErrAbort)
		{
		return;
		}
		
	iError = aError;
	}

void CTestStepAudInStreamWithUECap::MaiscRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

TVerdict CTestStepAudInStreamWithUECap::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("This test calls MediaAudioClientInputStream::ReadL func with at list the UserEnvironment capability"));

	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 1: error = %d"),iError);
		return EInconclusive;
		}

	TInt err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 2: error = %d"),err);
		return EInconclusive;
		}

	
	iState=ERecording;

	// start feeding the input stream
	TRAP(err, Read1BufL());
	CActiveScheduler::Start();
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Test Failed with: error = %d"),iError);
		return EFail;
		}
		
	return EPass;
	}


CTestStepAudInStreamSetGain* CTestStepAudInStreamSetGain::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamSetGain* self = new(ELeave) CTestStepAudInStreamSetGain(aTestName);
	return self;
	}

CTestStepAudInStreamSetGain::CTestStepAudInStreamSetGain(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudInStreamSetGain::~CTestStepAudInStreamSetGain()
	{
	}

void CTestStepAudInStreamSetGain::MaiscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudInStreamSetGain::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudInStreamSetGain::MaiscRecordComplete(TInt /*aError*/)
	{
	}

enum TVerdict CTestStepAudInStreamSetGain::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	INFO_PRINTF2(_L("Test step %S DoTestStepPreambleL"),&iTestStepName);
	TInt err = KErrNone;
	TRAP(err, iAudInStream = CMdaAudioInputStream::NewL(*this));
	if (err != KErrNone || iAudInStream == NULL )
		{
		return EInconclusive;
		}
		
	return verdict;
	}

enum TVerdict CTestStepAudInStreamSetGain::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;
	//[ Destroy the scheduler ]
	INFO_PRINTF2(_L("Test step %S DoTestStepPostambleL"),&iTestStepName);
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

TVerdict CTestStepAudInStreamSetGain::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestAudioInputStream : Gain"));
	INFO_PRINTF2( _L("TestAudioInputStream : MaxGain()(%d)"),iAudInStream->MaxGain());
	TInt gain = iAudInStream->MaxGain()/2;
	if (gain < 0)
		{
		return EFail;
		}
		
	//set the volume
	INFO_PRINTF2( _L("TestAudioInputStream : SetGain(%d)"),gain);
	iAudInStream->SetGain(gain);

	//get the volume
	TInt rgain = iAudInStream->Gain();
	if (gain != rgain ) 
		{
		 return EFail;
		}
		
	INFO_PRINTF2( _L("TestAudioInputStream : Gain() returned %d"),rgain);
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
//
//
CTestStepAudInStreamSetPriority* CTestStepAudInStreamSetPriority::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamSetPriority* self = new(ELeave) CTestStepAudInStreamSetPriority(aTestName);
	return self;
	}

CTestStepAudInStreamSetPriority::CTestStepAudInStreamSetPriority(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudInStreamSetPriority::~CTestStepAudInStreamSetPriority()
	{
	}

void CTestStepAudInStreamSetPriority::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	}
void CTestStepAudInStreamSetPriority::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	}
void CTestStepAudInStreamSetPriority::MaiscRecordComplete(TInt aError)
	{
	iError = aError;
	}

enum TVerdict CTestStepAudInStreamSetPriority::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	
	TInt err = KErrNone;
	TRAP(err, iAudInStream = CMdaAudioInputStream::NewL(*this));
	if (err != KErrNone || iAudInStream == NULL )
		{
		return EInconclusive;
		}
		
	return verdict;
	}

enum TVerdict CTestStepAudInStreamSetPriority::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

TVerdict CTestStepAudInStreamSetPriority::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestAudioInputStream : Priority"));
	iAudInStream->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTime);
	iAudInStream->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceQuality);
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
//
//
CTestStepAudInStreamSetBalance* CTestStepAudInStreamSetBalance::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamSetBalance* self = new(ELeave) CTestStepAudInStreamSetBalance(aTestName);
	return self;
	}

CTestStepAudInStreamSetBalance::CTestStepAudInStreamSetBalance(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudInStreamSetBalance::~CTestStepAudInStreamSetBalance()
	{
	}

void CTestStepAudInStreamSetBalance::MaiscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudInStreamSetBalance::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudInStreamSetBalance::MaiscRecordComplete(TInt /*aError*/)
	{
	}

enum TVerdict CTestStepAudInStreamSetBalance::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	
	TInt err = KErrNone;
	TRAP(err, iAudInStream = CMdaAudioInputStream::NewL(*this));
	if (err != KErrNone || iAudInStream == NULL )
		{
		return EInconclusive;
		}

	return verdict;
	}

enum TVerdict CTestStepAudInStreamSetBalance::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

TVerdict CTestStepAudInStreamSetBalance::DoTestStepL()
	{
	TInt bal=0;

	TRAPD(err, iAudInStream->SetBalanceL()); // default KMMFBalanceCenter
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	//get the balance
	TRAP(err, bal = iAudInStream->GetBalanceL());
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}

	if (bal != KMMFBalanceCenter )
		{
		return EFail;
		}
		
	TRAP(err, iAudInStream->SetBalanceL(KMMFBalanceMaxRight));
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	//get the balance
	TRAP(err, bal = iAudInStream->GetBalanceL());
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		

	if (bal != KMMFBalanceMaxRight)
		{
		return EFail;
		}

	TRAP(err, iAudInStream->SetBalanceL(KMMFBalanceMaxLeft));
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	//get the balance
	TRAP(err, bal = iAudInStream->GetBalanceL());
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	if (bal != KMMFBalanceMaxLeft )
		{
		return EFail;
		}
		
	// In of bounds
	/*
	iAudInStream->SetBalance(KMMFBalanceMaxLeft+10);
	//check condition
	*/

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
//
//

LOCAL_C TInt stopActiveScheduler(TAny*)
	{
	CActiveScheduler::Stop();
	return FALSE;
	}

CTestStepAudInStreamGetBytes* CTestStepAudInStreamGetBytes::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamGetBytes* self = new(ELeave) CTestStepAudInStreamGetBytes(aTestName);
	return self;
	}

CTestStepAudInStreamGetBytes::CTestStepAudInStreamGetBytes(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudInStreamGetBytes::~CTestStepAudInStreamGetBytes()
	{
	}

void CTestStepAudInStreamGetBytes::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
void CTestStepAudInStreamGetBytes::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	//if weve stopped recording, each unused buffer will be returned with KErrAbort
	if(iState == EStopped && aError == KErrAbort)
		{
		return;
		}
		
	iError = aError;
	++iBuffersUsed;
	}

void CTestStepAudInStreamGetBytes::MaiscRecordComplete(TInt aError)
	{

	if(aError != KErrNone)
		{
		iError = aError;
		CActiveScheduler::Stop();
		return;   
		}
	// Assume aBuffer == iBufferList[iEndBuf]

	// Increment iEndBuf
	if(iEndBuf==KRecNumBuffer-1)
		{
		iEndBuf = 0;
		}
		
	else
		iEndBuf++;

	// Starting writing out the buffers if we're not already
	if(!iWriteBufferActive->IsActive())
		iWriteBufferActive->Write(iBufferList[iStartBuf]);
	}

enum TVerdict CTestStepAudInStreamGetBytes::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test gets the bytes rendered by the H/W until this moment: GetBytes()"));

	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 1: error = %d"),iError);
		return EInconclusive;
		}

	TInt err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 2: error = %d"),err);
		return EInconclusive;
		}

	
	iState=ERecording;

	// start feeding the input stream
	StartReadL();
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 3: error = %d"),iError);
		return EInconclusive;
		}

	return verdict;
	}

enum TVerdict CTestStepAudInStreamGetBytes::DoTestStepPostambleL(void)
	{
	iFile.Close();
	delete iAudInStream;
	iAudInStream = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

TVerdict CTestStepAudInStreamGetBytes::DoTestStepL()
	{
	TInt bytes1 = iAudInStream->GetBytes();
	INFO_PRINTF2(_L("bytes1 = %d"), bytes1);

	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(1000000);
	CActiveScheduler::Start();
	callBackTimer->Cancel(); // in case something else stopped the AS

	if (iError != KErrNone)
	{
		ERR_PRINTF2(_L("First call to GetBytes() returned error %d"), iError);
		delete callBackTimer;
		return EFail;
	}

	TInt bytes2 = iAudInStream->GetBytes();
	INFO_PRINTF2(_L("bytes2 = %d"), bytes2);

	callBackTimer->After(1000000);
	CActiveScheduler::Start();
	callBackTimer->Cancel();

	if (iError != KErrNone)
	{
		ERR_PRINTF2(_L("Second call to GetBytes() returned error %d"), iError);
		delete callBackTimer;
		return EFail;
	}

	delete callBackTimer;

	//Stop will kick off the write of the data to file through CWriteBufferActive, which will stop
	//ActiveSheduler when written
	iState=EStopped; //required so that acceptable KErrAbort in MaiscBufferCopied are handled properly
	iAudInStream->Stop();

	// Starting writing out the buffers if we're not already
	if(!iWriteBufferActive->IsActive())
		{
		iWriteBufferActive->Write(iBufferList[iStartBuf]);
		}
		
	CActiveScheduler::Start();

	TInt bytes3 = iAudInStream->GetBytes();
	INFO_PRINTF2(_L("bytes3 = %d"), bytes3);
	if (iError != KErrNone)
	{
		ERR_PRINTF2(_L("Third call to GetBytes() returned error %d"), iError);
		return EFail;
	}

	TInt fileSize;
	User::LeaveIfError(iFile.Size(fileSize));
	ERR_PRINTF2(_L("filesize %d"), fileSize);
	INFO_PRINTF4(_L("GetBytes returned sizes %d, %d, %d"), bytes1, bytes2, bytes3);

	INFO_PRINTF3(_L("Recording Complete used %d of the %d buffers available"),iBuffersUsed,KRecNumBuffer);

	if (bytes1 > bytes2 || bytes2 > bytes3)
		{
		return EFail;
		}
		

	if (bytes3 != fileSize)
	{
		ERR_PRINTF3(_L("Error : bytes3 %d != filesize %d"), bytes3, fileSize);
		return EFail;
	}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
//
//

CTestStepAudInStreamSglBuf* CTestStepAudInStreamSglBuf::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamSglBuf* self = new(ELeave) CTestStepAudInStreamSglBuf(aTestName);
	return self;
	}

CTestStepAudInStreamSglBuf::CTestStepAudInStreamSglBuf(const TDesC& aTestName) 
	: CTestStepAudInStreamGetBytes(aTestName),
	  iBufferPtr(NULL, 0)
	{
	iTestStepResult = EFail;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudInStreamSglBuf::~CTestStepAudInStreamSglBuf()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamSglBuf::DoTestStepPreambleL(void)
	{	
	iTestStepResult = EPass;

	// this installs the scheduler
	iTestStepResult = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test gets the bytes rendered by the H/W until this moment: GetBytes()"));

	iAudInStream->SetSingleBufferMode(ETrue);
	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 1: error = %d"),iError);
		return EInconclusive;
		}

	iError = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 2: error = %d"),iError);
		return EInconclusive;
		}
	
	iState=ERecording;

	// start feeding the input stream
	Read1BufL(); // single buffer
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("DEBUG LEAVE 3: error = %d"),iError);
		return EInconclusive;
		}

	return iTestStepResult;
	}


void CTestStepAudInStreamSglBuf::Read1BufL()
	{
	// Issue reads on 1 available buffer
	iBufferPtr.Set(const_cast<TUint8*>(iSingleBuffer.Ptr()), 0, iSingleBuffer.MaxLength());
	iAudInStream->ReadL(iBufferPtr);
	}	


void CTestStepAudInStreamSglBuf::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{
	CTestStepAudInStreamGetBytes::MaiscBufferCopied(aError, aBuffer);
	
   	TInt lengthRecorded = iSingleBuffer.Length()+aBuffer.Length();
   	iSingleBuffer.SetLength(lengthRecorded);
	}


void CTestStepAudInStreamSglBuf::MaiscRecordComplete(TInt aError)
	{	
	// Starting writing out the buffer if we're not already
	if(!iWriteBufferActive->IsActive())
		{
		iWriteBufferActive->Write(iSingleBuffer);		
		}
	
	CTestStepAudInStreamGetBytes::MaiscRecordComplete(aError);		
	}


TVerdict CTestStepAudInStreamSglBuf::DoTestStepL()
	{
	TInt bytes1 = iAudInStream->GetBytes();
	INFO_PRINTF2(_L("bytes1 = %d"), bytes1);

	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(1000000);
	CActiveScheduler::Start();
	callBackTimer->Cancel(); // in case something else stopped the AS

	if (iError != KErrNone)
		{
		ERR_PRINTF2(_L("First call to GetBytes() returned error %d"), iError);
		delete callBackTimer;
		return EFail;
		}

	TInt bytes2 = iAudInStream->GetBytes();
	INFO_PRINTF2(_L("bytes2 = %d"), bytes2);

	callBackTimer->After(1000000);
	CActiveScheduler::Start();
	callBackTimer->Cancel();

	if (iError != KErrNone)
		{
		ERR_PRINTF2(_L("Second call to GetBytes() returned error %d"), iError);
		delete callBackTimer;
		return EFail;
		}

	delete callBackTimer;

	//Stop will kick off the write of the data to file through CWriteBufferActive, which will stop
	//ActiveSheduler when written
	iState=EStopped; //required so that acceptable KErrAbort in MaiscBufferCopied are handled properly
	iAudInStream->Stop();

	// Starting writing out the buffers if we're not already
	if(!iWriteBufferActive->IsActive())
		{
		iWriteBufferActive->Write(iSingleBuffer);
		}
		
	CActiveScheduler::Start();

	TInt bytes3 = iAudInStream->GetBytes();
	INFO_PRINTF2(_L("bytes3 = %d"), bytes3);
	if (iError != KErrNone)
		{
		ERR_PRINTF2(_L("Third call to GetBytes() returned error %d"), iError);
		return EFail;
		}

	TInt fileSize;
	User::LeaveIfError(iFile.Size(fileSize));
	ERR_PRINTF2(_L("filesize %d"), fileSize);
	INFO_PRINTF4(_L("GetBytes returned sizes %d, %d, %d"), bytes1, bytes2, bytes3);

	INFO_PRINTF2(_L("Recording Complete using a single buffer of size %d bytes"), KRecBufferSglBufSize);

	if (bytes1 > bytes2 || bytes2 > bytes3)
		{
		return EFail;
		}
		
	if (bytes3 != fileSize)
		{
		ERR_PRINTF3(_L("Error : bytes3 %d != filesize %d"), bytes3, fileSize);
		return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


void CTestStepAudInStreamSglBuf::ProcessDataL()
	{
	// wait for more recorded data to come through
	CActiveScheduler::Stop();
	}

//
//
//

CTestStepAudInStreamSetProp* CTestStepAudInStreamSetProp::NewL(const TDesC& aTestName)
	{
	CTestStepAudInStreamSetProp* self = new(ELeave) CTestStepAudInStreamSetProp(aTestName);
	return self;
	}

CTestStepAudInStreamSetProp::CTestStepAudInStreamSetProp(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudInStreamSetProp::~CTestStepAudInStreamSetProp()
	{
	}

void CTestStepAudInStreamSetProp::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	}
void CTestStepAudInStreamSetProp::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	}
void CTestStepAudInStreamSetProp::MaiscRecordComplete(TInt aError)
	{
	iError = aError;
	}

enum TVerdict CTestStepAudInStreamSetProp::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	
	TInt err = KErrNone;
	TRAP(err, iAudInStream = CMdaAudioInputStream::NewL(*this));
	if (err != KErrNone || iAudInStream == NULL )
		{
		return EInconclusive;
		}
		
	return verdict;
	}

enum TVerdict CTestStepAudInStreamSetProp::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

TVerdict CTestStepAudInStreamSetProp::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestAudioInputStream : SetAudioProperties"));
	iTestStepResult = EPass;
	TInt err = KErrNone;

	// test 1
	TRAP(err, iAudInStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate8000Hz,TMdaAudioDataSettings::EChannelsMono) );

	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		{
		return EFail;
		}
		

	// test2
	TRAP(err, iAudInStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate44100Hz,TMdaAudioDataSettings::EChannelsStereo) );

	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		{
		return EFail;
		}
		

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



//
//
//	CMdaAudioOutputStream Integration Tests //
//
//

// base class for some audio Ouput stream tests
CTestStepAudOutStream* CTestStepAudOutStream::NewL()
	{
	CTestStepAudOutStream* s = new(ELeave) CTestStepAudOutStream();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CTestStepAudOutStream::CTestStepAudOutStream() : iError(KErrNone)
	{
	iIsAllDataRead = EFalse;
	iIsAllDataWritten = EFalse;
	}

void CTestStepAudOutStream::ConstructL()
	{
	// reset the buffer
	for(TInt i=0; i<KNumBuffer; i++)
		{
		iBufferList[i] = KNullDesC8;
		}
	iStartBuf=iEndBuf=0;
	}

CTestStepAudOutStream::~CTestStepAudOutStream()
	{
	}

void CTestStepAudOutStream::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudOutStream::MaoscPlayComplete(TInt aError)
	{
	if (iIsAllDataWritten && (aError == KErrNone || aError == KErrUnderflow))
		{
		iError = KErrNone;
		}
		
	else
		iError = aError;
	
	iReadBufferActive->Cancel();
	TInt filePosition=0;
	iFile.Seek(ESeekStart, filePosition);
	iFile.Close();
	CActiveScheduler::Stop();
	}

void CTestStepAudOutStream::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	if(aError)
		{
			//need add for deal with exception
			return;   
		}

	
	// adjust the buffer
	iBufferList[iStartBuf] = KNullDesC8;
	if(iStartBuf==KNumBuffer-1)
		iStartBuf = 0;
	else
		iStartBuf++;
 
	if (iIsAllDataRead && iStartBuf == iEndBuf)
		{
		iIsAllDataWritten = ETrue;
		}
	else if(!iReadBufferActive->IsActive())
		iReadBufferActive->Read();
	}


CTestStepAudOutStream::CReadBufferActive::CReadBufferActive() : CActive(0)
	{
	CActiveScheduler::Add(this);
	}

CTestStepAudOutStream::CReadBufferActive::~CReadBufferActive()
	{
	Cancel();
	}

void CTestStepAudOutStream::CReadBufferActive::RunL()
	{
	iParent->ProcessDataL(iBuffer);
	}
	
TInt CTestStepAudOutStream::CReadBufferActive::RunError(TInt aError) 
	{
	iParent->iError = aError;
	CActiveScheduler::Stop();
	
	return KErrNone;
	}

void CTestStepAudOutStream::CReadBufferActive::DoCancel()
	{
	// do nothing
	}

void CTestStepAudOutStream::CReadBufferActive::Read()
	{
	iParent->iFile.Read(iBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}


void CTestStepAudOutStream::ProcessDataL(TBuf8<KBufferSize>& aData)
	{
	if(aData.Length())
		{
		if(iBufferList[iEndBuf]==KNullDesC8)
			{
			iBufferList[iEndBuf] = aData;
			iAudOutStream->WriteL(iBufferList[iEndBuf]);

			// adjust the buffer
			if(iEndBuf==KNumBuffer-1)
				{
				iEndBuf=0;
				}
				
			else
				{
				iEndBuf++;
				}
			}

		if(iBufferList[iEndBuf]==KNullDesC8)
			{
			iReadBufferActive->Read();
			}
		// else all our buffers are full
		}
	else
		iIsAllDataRead = ETrue;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStream::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;

	// reset the buffer [incase new test reuses previous object]
	for(TInt i=0; i<KNumBuffer; i++)
		{
		iBufferList[i] = KNullDesC8;
		}
	iStartBuf=iEndBuf=0;

	 // this installs the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();

	// create the active object
	iReadBufferActive = new(ELeave) CReadBufferActive();
	iReadBufferActive->iParent = this;
	iFs.Connect();

	TRAPD(err, iAudOutStream = CMdaAudioOutputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudOutStream == NULL )
		{
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
enum TVerdict CTestStepAudOutStream::DoTestStepPostambleL(void)
	{
	iFs.Close();

	delete iAudOutStream;
	iAudOutStream = NULL;
	delete iReadBufferActive;
	iReadBufferActive = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStream::DoTestStepL()
	{
	iTestStepResult = EPass;
	//TInt err = KErrNone;

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	if (iError != KErrNone )
	{
	return EFail;	
	}
		

	TInt err = KErrNone;
	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		{
		return EFail;
		}

	iReadBufferActive->Read();
	CActiveScheduler::Start();

	// wait sometime to finish and then close the file
	User::After(1200000);
	iFile.Close();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}






CTestStepAudOutStreamSetProp* CTestStepAudOutStreamSetProp::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamSetProp* self = new(ELeave) CTestStepAudOutStreamSetProp(aTestName);
	return self;
	}

CTestStepAudOutStreamSetProp::CTestStepAudOutStreamSetProp(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudOutStreamSetProp::~CTestStepAudOutStreamSetProp()
	{
	}

enum TVerdict CTestStepAudOutStreamSetProp::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// if the parent preamble fails, we must fail too
	if (verdict != EPass)
		{
		return verdict;
		}
		
	 // Printing to the console and log file
	INFO_PRINTF1(_L("this test is setting the audio properties of an Audio Output Stream Utility Class"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();

	return verdict;
	}

TVerdict CTestStepAudOutStreamSetProp::DoTestStepL()
	{
	TInt err = KErrNone;

	// test 1
	TRAP(err, iAudOutStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate8000Hz,TMdaAudioDataSettings::EChannelsMono) );
	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		{
		return EFail;
		}
		

	// test2
	// fails with the enum values, correct?
	TRAP(err, iAudOutStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate44100Hz,TMdaAudioDataSettings::EChannelsStereo) );

	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		{
		return EFail;
		}
		 
	else 
		 return EPass;
	}







CTestStepAudOutStreamOpen* CTestStepAudOutStreamOpen::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamOpen* self = new(ELeave) CTestStepAudOutStreamOpen(aTestName);
	return self;
	}

CTestStepAudOutStreamOpen::CTestStepAudOutStreamOpen(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

enum TVerdict CTestStepAudOutStreamOpen::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler and initialises the iAudOutStream
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();
	INFO_PRINTF1(_L("this test is openning an Audio Output Stream Utility Class"));
	return verdict;
	}

TVerdict CTestStepAudOutStreamOpen::DoTestStepL()
	{
	TInt err = KErrNone;

	TMdaAudioDataSettings settings;
	settings.Query();
	settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz; // ESampleRateAnyInRange
	settings.iChannels = TMdaAudioDataSettings::EChannelsStereo;
	settings.iFlags = TMdaAudioDataSettings::ENoNetworkRouting;
	settings.iVolume = 0;

	iAudOutStream->Open(&settings);
	CActiveScheduler::Start();
	
	if (iAudOutStream == NULL)
		{
		err = KErrNotReady;
		}
		
	if (iError != KErrNone ||
		err != KErrNone)
		{
		return EFail;
		}
		
	else 
		{
		return EPass;
		}
	}



CTestStepAudOutStreamSetVol* CTestStepAudOutStreamSetVol::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamSetVol* self = new(ELeave) CTestStepAudOutStreamSetVol(aTestName);
	return self;
	}

CTestStepAudOutStreamSetVol::CTestStepAudOutStreamSetVol(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;	
	}

CTestStepAudOutStreamSetVol::~CTestStepAudOutStreamSetVol()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudOutStreamSetVol::MaoscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudOutStreamSetVol::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudOutStreamSetVol::MaoscPlayComplete(TInt /*aError*/)
	{
	}

enum TVerdict CTestStepAudOutStreamSetVol::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is setting the volume of an Audio Output Stream Utility Class"));

	TRAPD(err, iAudOutStream = CMdaAudioOutputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudOutStream == NULL)
		{
		return EInconclusive;
		}

	return verdict;
	}

enum TVerdict CTestStepAudOutStreamSetVol::DoTestStepPostambleL(void)
	{
	delete iAudOutStream;
	iAudOutStream = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}


TVerdict CTestStepAudOutStreamSetVol::DoTestStepL()
	{
	TInt vol = iAudOutStream->MaxVolume()/2;
	if (vol < 0)
		{
		return EFail;
		}
		
	//set the volume
	iAudOutStream->SetVolume(vol);

	//get the volume
	TInt rvol = iAudOutStream->Volume();
	if (vol != rvol )
		{
		return EFail;
		}
		
	else
		{
		iTestStepResult = EPass;
		}	 
		
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}




CTestStepAudOutStreamSetBalance* CTestStepAudOutStreamSetBalance::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamSetBalance* self = new(ELeave) CTestStepAudOutStreamSetBalance(aTestName);
	return self;
	}

CTestStepAudOutStreamSetBalance::CTestStepAudOutStreamSetBalance(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}


CTestStepAudOutStreamSetBalance::~CTestStepAudOutStreamSetBalance()
	{
	}

void CTestStepAudOutStreamSetBalance::MaoscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudOutStreamSetBalance::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudOutStreamSetBalance::MaoscPlayComplete(TInt /*aError*/)
	{
	}

enum TVerdict CTestStepAudOutStreamSetBalance::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is setting and getting the balance of an Audio Output Stream Utility Class"));

	TRAPD(err, iAudOutStream = CMdaAudioOutputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudOutStream == NULL )
		{
		return EInconclusive;
		}	
		

	return verdict;
	}

enum TVerdict CTestStepAudOutStreamSetBalance::DoTestStepPostambleL(void)
	{
	delete iAudOutStream;
	iAudOutStream = NULL;

	//[ Destroy the scheduler ]
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}


TVerdict CTestStepAudOutStreamSetBalance::DoTestStepL()
	{
	TInt bal = 0;

	TRAPD(err, iAudOutStream->SetBalanceL()); // default KMMFBalanceCenter
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	//get the balance
	TRAP(err, bal = iAudOutStream->GetBalanceL());
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	if (bal != KMMFBalanceCenter)
		{
		return EFail;
		}

	TRAP(err, iAudOutStream->SetBalanceL(KMMFBalanceMaxRight));
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	//get the balance
	TRAP(err, bal = iAudOutStream->GetBalanceL());
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	if (bal != KMMFBalanceMaxRight)
		{
		return EFail;
		}
		
	TRAP(err, iAudOutStream->SetBalanceL(KMMFBalanceMaxLeft));
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}

	//get the balance
	TRAP(err, bal = iAudOutStream->GetBalanceL());
	if (err != KErrNone)
		{
		return EFail; // we expect this _will_ be supported for Typhoon
		}
		
	if (bal != KMMFBalanceMaxLeft)
		{
		return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}






CTestStepAudOutStreamSetPrior* CTestStepAudOutStreamSetPrior::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamSetPrior* self = new(ELeave) CTestStepAudOutStreamSetPrior(aTestName);
	return self;
	}

CTestStepAudOutStreamSetPrior::CTestStepAudOutStreamSetPrior(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudOutStreamSetPrior::~CTestStepAudOutStreamSetPrior()
	{
	}

enum TVerdict CTestStepAudOutStreamSetPrior::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is setting the audio priorities of an Audio Output Stream Utility Class"));

	// if the parent preamble fails, we must fail too
	if (verdict != EPass)
		{
		return verdict;
		}
		
	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();

	TInt err = KErrNone;
	if(iAudOutStream==NULL)
		{
		err = KErrNotReady;
		}
		
	if (iError != KErrNone ||
		err != KErrNone)
		{
		 return EInconclusive;
		}
		

	return verdict;
	}

enum TVerdict CTestStepAudOutStreamSetPrior::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepAudOutStream::DoTestStepPostambleL();
	}

TVerdict CTestStepAudOutStreamSetPrior::DoTestStepL()
	{
	// test 1
	iAudOutStream->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTime);

	// test2
	iAudOutStream->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceQuality);

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}





CTestStepAudOutStreamStop* CTestStepAudOutStreamStop::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamStop* self = new(ELeave) CTestStepAudOutStreamStop(aTestName);
	return self;
	}

CTestStepAudOutStreamStop::CTestStepAudOutStreamStop(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudOutStreamStop::~CTestStepAudOutStreamStop()
	{
	}

enum TVerdict CTestStepAudOutStreamStop::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// if the parent preamble fails, we must fail too
	if (verdict != EPass)
		{
		return verdict;
		}
		
	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is stopping a playing Audio Output Stream Utility Class: Stop()"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	
	TInt err = KErrNone;
	if(iAudOutStream==NULL)
		{
		err=KErrNotReady;
		}
		
	if (iError != KErrNone ||
		err != KErrNone)
		{
		return EInconclusive;
		}
		
	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
		
	// start feeding the outputStream
	iReadBufferActive->Read(); // finally calls CMdaAudioOutputStream::WriteL(...)
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;

	if(iAudOutStream==NULL)
		{
		err = KErrNotReady;
		}
		
	if (err != KErrNone)
		{
		return EInconclusive;
		}
		
	return verdict;
}

enum TVerdict CTestStepAudOutStreamStop::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
	}

TInt StopAudOutStream(TAny* aAudOutStream)
	{
	REINTERPRET_CAST(CMdaAudioOutputStream*, aAudOutStream)->Stop();
	return FALSE;
	}


TVerdict CTestStepAudOutStreamStop::DoTestStepL()
	{
	TInt err = KErrNone;
	// stream is playing now from preamble
	// wait a fraction of the stream duration before stopping
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(StopAudOutStream, iAudOutStream));
	callBackTimer->After(1000000);
	CActiveScheduler::Start();

	if(iAudOutStream==NULL)
		{
		err=KErrNotReady;
		}
		
	if (iError != KErrCancel || // KErrAbort(?KErrCancel seems to be what is returned) is supposed to be returned ny the MaoscBufferCopied
		err != KErrNone)
		{
		INFO_PRINTF2(_L("Test failed with error %d"), iError);
		return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}





CTestStepAudOutStreamPosition* CTestStepAudOutStreamPosition::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamPosition* self = new(ELeave) CTestStepAudOutStreamPosition(aTestName);
	return self;
	}

CTestStepAudOutStreamPosition::CTestStepAudOutStreamPosition(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudOutStreamPosition::~CTestStepAudOutStreamPosition()
	{
	}

enum TVerdict CTestStepAudOutStreamPosition::DoTestStepPreambleL(void)
{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// if the parent preamble fails, we must fail too
	if (verdict != EPass)
		{
		return verdict;
		}
		

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is getting the position of a playing Audio Output Stream Utility Class: Position()"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume
	TInt err = KErrNone;
	if(iAudOutStream==NULL)
		{
		err = KErrNotReady;
		}
		
	if (iError != KErrNone ||
		err != KErrNone )
		{
		return EInconclusive;
		}
		
	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
		
	// start feeding the outputStream
	iReadBufferActive->Read(); // finally calls CMdaAudioOutputStream::WriteL(...)
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;

	if(iAudOutStream==NULL)
		{
		err = KErrNotReady;
		}
		
	if (err != KErrNone)
		{
		return EInconclusive;
		}

	return verdict;
}


enum TVerdict CTestStepAudOutStreamPosition::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
	}

TVerdict CTestStepAudOutStreamPosition::DoTestStepL()
	{
	if(iAudOutStream==NULL)
		{
		return EFail;
		}
		
	// Get the position before and after a 5s play;
	// We are asking for a two seconds play window, but due to scheduling issues etc, 
	// we may not get exactly what we want. However we can more accurately find out 
	// what we got by asking for the elapsed clock ticks during that time.
	// This test fails if waittime < 5 seconds, needs more wait time to calculate accuractely
	const TInt KWaitTime = 5000000; // 5s
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler), CActive::EPriorityStandard);
	callBackTimer->After(KWaitTime);
	const TTimeIntervalMicroSeconds pos1 = iAudOutStream->Position();
	const TInt tick1 = User::TickCount();
	CActiveScheduler::Start();
	delete callBackTimer;
	const TTimeIntervalMicroSeconds pos2 = iAudOutStream->Position();
	const TInt tick2 = User::TickCount();

	// How much did the postion move by, and in how much time?
	TInt tickPeriod = 0;
	if (HAL::Get(HALData::ESystemTickPeriod, tickPeriod) != KErrNone)
		{
		return EInconclusive;
		}
		
	const TInt64 KDiffPosition = pos2.Int64() - pos1.Int64();
	const TInt KElapsedTime = (tick2 - tick1) * tickPeriod;

	// Is the position diff inside an acceptable range?
	const TReal KTolerance = 0.2; // 20%
	const TInt KDelta = static_cast<TInt>(static_cast<TReal>(KElapsedTime) * KTolerance); // yuk
	const TInt KMin = KElapsedTime - KDelta;
	const TInt KMax = KElapsedTime + KDelta;
	INFO_PRINTF5 (_L("Step One:\tActual: %ld, Expected: %d, Min: %d, Max: %d"), KDiffPosition, KElapsedTime, KMin, KMax);
	if ((KDiffPosition < KMin) || (KDiffPosition > KMax))
		{
		return EFail;
		}
		

	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(StopAudOutStream, iAudOutStream));
	CActiveScheduler::Start();
	delete idleStopper;

	if (iError != KErrCancel)
		{
		// KErrCancel (KErrAbort is supposed to be returned ny the MaoscBufferCopied??)
		INFO_PRINTF2(_L("Test failed with error %d"), iError);
		return EFail;
		}

	
	// Get the position. For a stopped stream, this should always be zero.
	TTimeIntervalMicroSeconds pos = iAudOutStream->Position();
	INFO_PRINTF2(_L("Step Two:\tActual: %ld, Expected: 0"), pos.Int64());
	if (pos.Int64() != 0)
		{
		return EFail;
		}
		

	//xxx try cotinuing playing and get positions while playing if there is more time.

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return EPass;
	}



CTestStepAudOutStreamGetBytes* CTestStepAudOutStreamGetBytes::NewL(const TDesC& aTestName)
	{
	CTestStepAudOutStreamGetBytes* self = new(ELeave) CTestStepAudOutStreamGetBytes(aTestName);
	return self;
	}

CTestStepAudOutStreamGetBytes::CTestStepAudOutStreamGetBytes(const TDesC& aTestName)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

CTestStepAudOutStreamGetBytes::~CTestStepAudOutStreamGetBytes()
	{
	}

enum TVerdict CTestStepAudOutStreamGetBytes::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// if the parent preamble fails, we must fail too
	if (verdict != EPass)
		{
		return verdict;
		}
		

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test gets the bytes rendered by the H/W until this moment: GetBytes()"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	TInt err = KErrNone;
	if(iAudOutStream==NULL)
		{
		err = KErrNotReady;
		}
		
	if (iError != KErrNone ||
		err != KErrNone)
		{
		return EInconclusive;
		}
		
	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
		

	// start feeding the outputStream
	iReadBufferActive->Read(); // finally calls CMdaAudioOutputStream::WriteL(...)
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;

	if(iAudOutStream==NULL)
		{
		err = KErrNotReady;
		}
		
	if (err != KErrNone)
		{
		return EInconclusive;
		}
		
	return verdict;
}

enum TVerdict CTestStepAudOutStreamGetBytes::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
	}

TVerdict CTestStepAudOutStreamGetBytes::DoTestStepL()
	{
	TInt bytes1 = iAudOutStream->GetBytes();
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(2000000);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Test failed with error %d"), iError);
		return EFail;
		}
	callBackTimer->Cancel(); // in case something else stopped the AS
	TInt bytes2 = iAudOutStream->GetBytes();
	callBackTimer->After(2000000);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Test failed with error %d"), iError);
		return EFail;
		}
	delete callBackTimer;
	TInt bytes3 = iAudOutStream->GetBytes();

	TInt fileSize;
	User::LeaveIfError(iFile.Size(fileSize));
	CActiveScheduler::Start(); // wait for sample to finish
	TInt bytes4 = iAudOutStream->GetBytes();
	if (bytes1 >= bytes2 || bytes2 >= bytes3 || bytes3 >= bytes4)
		{
		return EFail;
		}
		
	if (bytes4 != fileSize)
		{
		return EFail;
		}
		

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}





//------------------------------------------------------------------

/**
 * Constructor baseclass 
 */
CTestMmfAclntRecNoPlayback::CTestMmfAclntRecNoPlayback(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

void CTestMmfAclntRecNoPlayback::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecNoPlayback : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	//dont stop the playing track
	if (aCurrentState != CMdaAudioClipUtility::EPlaying || iError != KErrNone)
		{
		CActiveScheduler::Stop();
		}
		
	}


TVerdict CTestMmfAclntRecNoPlayback::DoTestStepPreambleL() 
	{
	TVerdict ret = EPass;

	// Make sure file doesn't exist (maybe from a previous test)
	TParse fullFileName;
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 

	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);
		

	// parse the filenames
	if(!fullFileName.Set(filename, &KDefault, NULL))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
		TInt error = fs.Delete(fullFileName.FullName());

		if(error == KErrPathNotFound)
			{
			error = fs.MkDirAll(fullFileName.DriveAndPath());
			INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
			}
		else  if(error == KErrNotFound)
			INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
		else if(error==KErrNone)
			INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
		else
			{
			ret = EInconclusive;
			}
		fs.Close();
		}

	if((ret == EInconclusive) || (ret == EFail))
		{
		return ret;	
		}
		
	
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}


TVerdict CTestMmfAclntRecNoPlayback::DoTestStepPostambleL() 
	{
	if(IsFileTest())
		{
		// Cleanup after the test
		TParse fullFileName;
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 

		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
			

		GetDriveName(filename);
		filename.Append(filename1);
			

		// parse the filenames
		if(!fullFileName.Set(filename, &KDefault, NULL))
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
			TInt error = fs.Delete(fullFileName.FullName());

			if(error != KErrNone)
				{
				INFO_PRINTF2(_L("Failed to delete %S"), &fullFileName.FullName());	
				}
			}
		}

	return CTestMmfAclntStep::DoTestStepPostambleL();
	}




TVerdict CTestMmfAclntRecNoPlayback::DoTestStepL()
	{
	TVerdict testStatus = EPass;
	TInt trapRet = KErrNone;
	TInt ret = KErrNone;

	const TInt durError = 2000000; //2 second
	const TInt posnError = 250000; //1/4 second
	const TInt cropError = 250000; //1/4 second

	iRecorder = CMdaAudioRecorderUtilityLC();

	if(!iRecorder)
		{
		testStatus = EFail;
		}
		
	//Should always work, must be done before play or record to ensure in EPrimed state
	if(testStatus == EPass)
		{
		TRAP(trapRet,iRecorder->SetPriority(EMdaPriorityMin,EMdaPriorityPreferenceNone));
		if(trapRet != KErrNone)
			{
			INFO_PRINTF2(_L("SetPriority leave with error %d"),trapRet);
			testStatus = EFail;
			}
		}



	if(testStatus == EPass)
		{
		iError = KErrTimedOut;
		iRecorder->RecordL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open -> record
	
		User::After(KTenSeconds);
		iRecorder->Stop();

		testStatus =  RecordSuccessfull();
		}

	
	//Can't detect difference in these interfaces with no play controller
	//void ClearPlayWindow();
	//void SetRepeats(....);
	//void Pause();
	//void SetMaxWriteLength(TInt aMaxWriteLength /*= KMdaClipLocationMaxWriteLengthNone*/);
	//void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	//void SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//These interfaces should always WORK if there is a record or playback controller
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds setPosn(KTwoSeconds);
	
		iRecorder->SetPosition(setPosn);
		TTimeIntervalMicroSeconds getPosn = iRecorder->Position();
		INFO_PRINTF3(_L("got position %d should be %d"),I64LOW(getPosn.Int64()), KTwoSeconds);

		if((getPosn.Int64()  < setPosn.Int64()-posnError) &&
		   (getPosn.Int64()  > setPosn.Int64()+posnError))
			{
			INFO_PRINTF1(_L("Position fail"));
			testStatus = EFail;
			}
		}	
	
	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds duration = iRecorder->Duration();
		INFO_PRINTF3(_L("got duration %d should be %d"),I64LOW(duration.Int64()), KTenSeconds);

		if((duration.Int64()  > KTenSeconds+durError) ||
		   (duration.Int64()  < KTenSeconds-durError))
			{
			INFO_PRINTF1(_L("Duration fail"));
			}
		}	


	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//These interfaces should WORK if there is a record controller but no playback controller
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(testStatus == EPass)
		{
		TInt gain;
		iRecorder->SetGain(2);

		ret = iRecorder->GetGain(gain);
		if((ret != KErrNone) || (gain != 2))
			{
			INFO_PRINTF1(_L("Gain fail"));
			testStatus = EFail;
			}
		}
	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds croppedDuration;
		TTimeIntervalMicroSeconds originalDuration;
		TTimeIntervalMicroSeconds setPosn(KTwoSeconds);
		iRecorder->SetPosition(setPosn);

		originalDuration = iRecorder->Duration();

		if((originalDuration.Int64()  > KTenSeconds+durError) ||
		   (originalDuration.Int64()  < KTenSeconds-durError))
			{
			INFO_PRINTF1(_L("getting Duration for CropL fail"));
			testStatus = EFail;
			}
		else
			{
			TRAP(trapRet, iRecorder->CropFromBeginningL());
			if(trapRet == KErrNone)
				{
				croppedDuration = iRecorder->Duration();
				INFO_PRINTF3(_L("got duration %d should be %d"),I64LOW(croppedDuration.Int64()), originalDuration.Int64()-KTwoSeconds);
				}
			if((trapRet != KErrNone) || 
				((croppedDuration.Int64()  > originalDuration.Int64()-KTwoSeconds+cropError) ||
				(croppedDuration.Int64()  < originalDuration.Int64()-KTwoSeconds-cropError)))
				{
				INFO_PRINTF1(_L("Crop fail"));
				testStatus = EFail;
				}
			}
		}

	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds zero(0);

		TTimeIntervalMicroSeconds recTimeAvail = iRecorder->RecordTimeAvailable();
		if(recTimeAvail == zero)
			{
			INFO_PRINTF1(_L("RecordTimeAvailable fail"));
			testStatus = EFail;
			}
		}

	
	if(testStatus == EPass)
		{
		TInt bal = KMMFBalanceMaxRight;
		ret = iRecorder->SetRecordBalance(KMMFBalanceMaxLeft);
		if(ret == KErrNone)
			{
			bal = KMMFBalanceMaxRight;
			ret = iRecorder->GetRecordBalance(bal);
			}
		if((bal != KMMFBalanceMaxLeft) || (ret != KErrNone))
			{
			INFO_PRINTF1(_L("Balance tests fail"));
			testStatus = EFail;
			}
		}

	//Haven't got a format that supports these - trace into code.
	/*
	if(testStatus == EPass)
		{
		TInt metaEntries;
		CMMFMetaDataEntry* metaEntry = NULL;
		CMMFMetaDataEntry* newMetaEntry = CMMFMetaDataEntry::NewL(_L("dummy meta"), _L("dummy val"));
		
		iRecorder->GetNumberOfMetaDataEntries(metaEntries);
		TRAP(trapRet, metaEntry = iRecorder->GetMetaDataEntryL(1));
		TRAP(trapRet, iRecorder->AddMetaDataEntryL(*newMetaEntry));
		TRAP(trapRet, iRecorder->ReplaceMetaDataEntryL(1, *newMetaEntry));
		iRecorder->RemoveMetaDataEntry(1);
		}
	*/


	if(testStatus == EPass)
		{
		RArray<TFourCC> codecs;
		TRAP(trapRet, iRecorder->GetSupportedDestinationDataTypesL(codecs));
		if((trapRet == KErrNone) && (codecs.Count() > 0) && IsFileTest())
			{
			TRAP(trapRet, iRecorder->SetDestinationDataTypeL(codecs[0]));
			}

		if((trapRet == KErrNone) && (codecs.Count() > 0))
			{
			TFourCC dataType;
			TRAP(trapRet, dataType = iRecorder->DestinationDataTypeL());
			if((trapRet != KErrNone) || (IsFileTest() && (dataType != codecs[0])))
				testStatus = EFail;
			}

		if((trapRet != KErrNone) || (testStatus == EFail))
			{
			INFO_PRINTF1(_L("DestinationDataTypes fail"));
			testStatus = EFail; 
			}
		}


	//These methods are not available for our formats and will therefore always fail - trace into code
	/*
	if(testStatus == EPass)
		{
		RArray<TUint> bitRates;
		TUint bitRate = 0;

		TRAP(trapRet, iRecorder->GetSupportedBitRatesL(bitRates));
		if((trapRet == KErrNone) && (bitRates.Count() >0))
			TRAP(trapRet, iRecorder->SetDestinationBitRateL(bitRates[0]));
		if((trapRet == KErrNone) && (bitRates.Count() >0))
			TRAP(trapRet, bitRate = iRecorder->DestinationBitRateL());

		if((trapRet != KErrNone) || (bitRate != bitRates[0]))
			{
			INFO_PRINTF1(_L("BitRates fail"));
			testStatus = EFail; 
			}
		}
		*/

	if(testStatus == EPass)
		{
		RArray<TUint> sampleRates;
		TUint sampleRate = 0;

		TRAP(trapRet, iRecorder->GetSupportedSampleRatesL(sampleRates));
		if((trapRet == KErrNone) && (sampleRates.Count() >0))
			{
			TRAP(trapRet, iRecorder->SetDestinationSampleRateL(sampleRates[0]));
			}
			
		if((trapRet == KErrNone) && (sampleRates.Count() >0))
			{
			TRAP(trapRet, sampleRate = iRecorder->DestinationSampleRateL());
			}

		if((trapRet != KErrNone) || (sampleRate != sampleRates[0]))
			{
			INFO_PRINTF1(_L("SampleRates fail"));
			testStatus = EFail; 
			}
		}



	if(testStatus == EPass)
		{
		RArray<TUint> channels;
		TUint channel = 0;

		TRAP(trapRet, iRecorder->GetSupportedNumberOfChannelsL(channels));
		if((trapRet == KErrNone) && (channels.Count() >0))
			{
			TRAP(trapRet, iRecorder->SetDestinationNumberOfChannelsL(channels[0]));
			}
			
		if((trapRet == KErrNone) && (channels.Count() >0))
			{
			TRAP(trapRet, channel = iRecorder->DestinationNumberOfChannelsL());
			}
			
		if((trapRet != KErrNone) || (channel != channels[0]))
			{
			INFO_PRINTF1(_L("channels fail"));
			testStatus = EFail; 
			}
		}


	if(testStatus == EPass)
		{
		TUid currentFormat = KNullUid;

		TRAP(trapRet, currentFormat = iRecorder->DestinationFormatL());
		
		/*can't do this, causes segmentation violation - trace into code
		TUid newFormat = {KMmfUidFormatRAWWrite};
		if(trapRet == KErrNone)
			TRAP(trapRet, iRecorder->SetDestinationFormatL(newFormat));
		if(trapRet == KErrNone)
			TRAP(trapRet, iRecorder->SetDestinationFormatL(currentFormat));
		*/

		if(trapRet != KErrNone)
			{
			INFO_PRINTF1(_L("format fail"));
			testStatus = EFail; 
			}
		}


//  These would pass when a record controller is present, also need test controller - trace into code
//	TInt RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
//	TInt RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
//	void RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
//	void RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//These interfaces should FAIL if there is a record controller but no playback controller
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(testStatus == EPass)
		{
		TRAP(trapRet, iRecorder->PlayL())
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("Play fail"));
			testStatus = EFail; 
			}
		}


	if(testStatus == EPass)
		{
		TInt maxVol = iRecorder->MaxVolume();
		if(maxVol != 0)
			{
			INFO_PRINTF1(_L("MaxVolume fail"));
			testStatus = EFail; 
			}
		}

	
	if(testStatus == EPass)
		{
		iRecorder->SetVolume(1);

		TInt vol;
		ret = iRecorder->GetVolume(vol);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("GetVolume fail"));
			testStatus = EFail; 
			}
		}

	if(testStatus == EPass)
		{
		TInt bal;
		TInt ret = iRecorder->SetPlaybackBalance(KMMFBalanceMaxRight);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("SetPlaybackBalance fail"));
			testStatus = EFail; 
			}

		ret = iRecorder->GetPlaybackBalance(bal);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("GetPlaybackBalance fail"));
			testStatus = EFail; 
			}
		}



	if(testStatus == EPass)
		{
		TMMFMessageDestinationPckg dummyPckg;
		TInt dummyFunc = 100;
		TBuf8<8> dummyBuff;

		ret = iRecorder->PlayControllerCustomCommandSync(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("PlayControllerCustomCommandSync - 1 fail"));
			testStatus = EFail; 
			}

		ret = iRecorder->PlayControllerCustomCommandSync(dummyPckg, dummyFunc, dummyBuff, dummyBuff);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("PlayControllerCustomCommandSync -2 fail"));
			testStatus = EFail; 
			}

		CTestMmfAclntAOCallback* aoCallback =  new CTestMmfAclntAOCallback(this);
		CleanupStack::PushL(aoCallback);
		aoCallback->PublicSetActive();


		iRecorder->PlayControllerCustomCommandAsync(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff, aoCallback->iStatus);
		CActiveScheduler::Start();

		if(iError != KErrNotSupported)
			{
			INFO_PRINTF1(_L("PlayControllerCustomCommandAsync -1 fail"));
			testStatus = EFail; 
			}
		
		aoCallback->PublicSetActive();
		iRecorder->PlayControllerCustomCommandAsync(dummyPckg, dummyFunc, dummyBuff, dummyBuff, aoCallback->iStatus);
		CActiveScheduler::Start();

		if(iError != KErrNotSupported)
			{
			INFO_PRINTF1(_L("PlayControllerCustomCommandAsync -2 fail"));
			testStatus = EFail; 
			}

		CleanupStack::Pop(aoCallback);
		}




	CleanupStack::PopAndDestroy(iRecorder);
	User::After(KOneSecond);
	return	testStatus;
	}















//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecNoPlaybackForceFormat::CTestMmfAclntRecNoPlaybackForceFormat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
: CTestMmfAclntRecNoPlayback(aTestName, aSectName, aKeyName)
	{
	iFormat = aFormat;
	}

CTestMmfAclntRecNoPlaybackForceFormat* CTestMmfAclntRecNoPlaybackForceFormat::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoPlaybackForceFormat* self = new (ELeave) CTestMmfAclntRecNoPlaybackForceFormat(aTestName,aSectName,aKeyName,aFormat);
	return self;
	}

CTestMmfAclntRecNoPlaybackForceFormat* CTestMmfAclntRecNoPlaybackForceFormat::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoPlaybackForceFormat* self = CTestMmfAclntRecNoPlaybackForceFormat::NewLC(aTestName,aSectName,aKeyName,aFormat);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoPlaybackForceFormat::CMdaAudioRecorderUtilityLC()
	{
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a file and forcing format"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		return NULL;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TMdaFileClipLocation location(iFilename);

	recUtil->OpenL(&location,&iFormat);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoPlaybackForceFormat::RecordSuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError == KErrNone)
		{
		RFs fs;
		RFile file;
		TInt size = 0;
		
		iError = fs.Connect();
		if (iError == KErrNone)
			{
			iError = file.Open(fs,iFilename,EFileRead);
			
			if (iError == KErrNone)
				{
				iError = file.Size(size);
					
				if (iError != KErrNone)
					{
					INFO_PRINTF1(_L("Unable to get size of file!"));
					}
				}
			else
				{
				INFO_PRINTF1(_L("Unable to open file."));
				return EInconclusive;
				}	
			
			}
		else
			{
			INFO_PRINTF1(_L("Unable to connect to file system."));
			return EInconclusive;
			}
		
		if(size == 0)
			{
			testStatus = EFail;
			}
			
		
		file.Close();	
		fs.Close();
		CleanupStack::PopAndDestroy(); //file
		}
	else
		testStatus = EFail;

	return testStatus;
	}
	


CTestMmfAclntRecNoPlaybackForceFormatDesc* CTestMmfAclntRecNoPlaybackForceFormatDesc::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoPlaybackForceFormatDesc* self = new (ELeave) CTestMmfAclntRecNoPlaybackForceFormatDesc(aTestName,aSectName,aKeyName,aFormat);
	return self;
	}

CTestMmfAclntRecNoPlaybackForceFormatDesc* CTestMmfAclntRecNoPlaybackForceFormatDesc::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoPlaybackForceFormatDesc* self = CTestMmfAclntRecNoPlaybackForceFormatDesc::NewLC(aTestName,aSectName,aKeyName,aFormat);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoPlaybackForceFormatDesc::CMdaAudioRecorderUtilityLC()
	{
	const TInt KMaxFileSize = 200000;
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a descriptor and forcing format"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		User::Leave(KErrNotFound);

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);


	HBufC8* iBuf = HBufC8::NewL( KMaxFileSize );
	CleanupStack::PushL(iBuf);

	iFileDesc = new (ELeave) TPtr8( NULL, 0 );
	CleanupStack::PushL(iFileDesc);

	iFileDesc->Set( iBuf->Des());


	TMdaDesClipLocation location(*iFileDesc);

	recUtil->OpenL(&location,&iFormat);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::Pop(iFileDesc);
	CleanupStack::Pop(iBuf);
	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoPlaybackForceFormatDesc::RecordSuccessfull()
	{
	TVerdict testStatus = EPass;

	TInt len = iFileDesc->Length();

	if(len == 0)
		{
		testStatus = EFail;
		}
		
	return testStatus;
	}




//------------------------------------------------------------------


CTestMmfAclntRecNoPlaybackFile* CTestMmfAclntRecNoPlaybackFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoPlaybackFile* self = new (ELeave) CTestMmfAclntRecNoPlaybackFile(aTestName,aSectName,aKeyName);
	return self;
	}

CTestMmfAclntRecNoPlaybackFile* CTestMmfAclntRecNoPlaybackFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoPlaybackFile* self = CTestMmfAclntRecNoPlaybackFile::NewLC(aTestName,aSectName,aKeyName);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoPlaybackFile::CMdaAudioRecorderUtilityLC()
	{
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a file"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		return NULL;
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	recUtil->OpenFileL(iFilename);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoPlaybackFile::RecordSuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError == KErrNone)
		{
		RFs fs;
		RFile file;
		TInt size = 0;
		
		iError = fs.Connect();
		if (iError == KErrNone)
			{
			iError = file.Open(fs,iFilename,EFileRead);
			
			if (iError == KErrNone)
				{
				iError = file.Size(size);
					
				if (iError != KErrNone)
					{
					INFO_PRINTF1(_L("Unable to get size of file!"));
					}
				}
			else
				{
				INFO_PRINTF1(_L("Unable to open file."));
				return EInconclusive;
				}	
			
			}
		else
			{
			INFO_PRINTF1(_L("Unable to connect to file system."));
			return EInconclusive;
			}
		
		if(size == 0)
			{
			testStatus = EFail;
			}
			
		
		file.Close();	
		fs.Close();
		CleanupStack::PopAndDestroy(); //file
		}
		
	else
		testStatus = EFail;

	return testStatus;
	}
	



CTestMmfAclntRecNoPlaybackDesc* CTestMmfAclntRecNoPlaybackDesc::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoPlaybackDesc* self = new (ELeave) CTestMmfAclntRecNoPlaybackDesc(aTestName,aSectName,aKeyName);
	return self;
	}

CTestMmfAclntRecNoPlaybackDesc* CTestMmfAclntRecNoPlaybackDesc::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoPlaybackDesc* self = CTestMmfAclntRecNoPlaybackDesc::NewLC(aTestName,aSectName,aKeyName);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoPlaybackDesc::CMdaAudioRecorderUtilityLC()
	{
	const TInt KMaxFileSize = 200000;
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a descriptor"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		User::Leave(KErrNotFound);
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);


	HBufC8* iBuf = HBufC8::NewL( KMaxFileSize );
	CleanupStack::PushL(iBuf);

	iFileDesc = new (ELeave) TPtr8( NULL, 0 );
	CleanupStack::PushL(iFileDesc);

	iFileDesc->Set( iBuf->Des());

	//Put some contents (one sample wav file) into the descriptor
	TPtrC oneSampleFileName;
	if(!GetStringFromConfig(iSectName, _L("OneSampleFile"), oneSampleFileName))
		{
		User::Leave(KErrNotFound);	
		}
		
	RFs fs;
	RFile file;
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,oneSampleFileName,EFileRead));
	CleanupClosePushL(file);
	if(file.Read(*iFileDesc) != KErrNone)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(&file);


	recUtil->OpenDesL(*iFileDesc);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::Pop(iFileDesc);
	CleanupStack::Pop(iBuf);
	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoPlaybackDesc::RecordSuccessfull()
	{
	TVerdict testStatus = EPass;

	TInt len = iFileDesc->Length();

	if(len == 0)
		{
		testStatus = EFail;	
		}
		
	return testStatus;
	}




CTestMmfAclntRecNoPlaybackFileForceController* CTestMmfAclntRecNoPlaybackFileForceController::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoPlaybackFileForceController* self = new (ELeave) CTestMmfAclntRecNoPlaybackFileForceController(aTestName,aSectName,aKeyName, aController);
	return self;
	}

CTestMmfAclntRecNoPlaybackFileForceController* CTestMmfAclntRecNoPlaybackFileForceController::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoPlaybackFileForceController* self = CTestMmfAclntRecNoPlaybackFileForceController::NewLC(aTestName,aSectName,aKeyName,aController);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoPlaybackFileForceController::CMdaAudioRecorderUtilityLC()
	{
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a file, forcing record controller"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		return NULL;
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	recUtil->OpenFileL(iFilename,iController);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoPlaybackFileForceController::RecordSuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError == KErrNone)
		{
		RFs fs;
		RFile file;
		TInt size = 0;
		
		iError = fs.Connect();
		if (iError == KErrNone)
			{
			iError = file.Open(fs,iFilename,EFileRead);
			
			if (iError == KErrNone)
				{
				iError = file.Size(size);
					
				if (iError != KErrNone)
					{
					INFO_PRINTF1(_L("Unable to get size of file!"));
					}
				}
			else
				{
				INFO_PRINTF1(_L("Unable to open file."));
				return EInconclusive;
				}	
			
			}
		else
			{
			INFO_PRINTF1(_L("Unable to connect to file system."));
			return EInconclusive;
			}
		
		if(size == 0)
			{
			testStatus = EFail;
			}
			
		file.Close();	
		fs.Close();
		CleanupStack::PopAndDestroy(); //file
		}
	else
		testStatus = EFail;

	return testStatus;
	}
	



CTestMmfAclntRecNoPlaybackDescForceController* CTestMmfAclntRecNoPlaybackDescForceController::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoPlaybackDescForceController* self = new (ELeave) CTestMmfAclntRecNoPlaybackDescForceController(aTestName,aSectName,aKeyName,aController);
	return self;
	}

CTestMmfAclntRecNoPlaybackDescForceController* CTestMmfAclntRecNoPlaybackDescForceController::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoPlaybackDescForceController* self = CTestMmfAclntRecNoPlaybackDescForceController::NewLC(aTestName,aSectName,aKeyName,aController);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoPlaybackDescForceController::CMdaAudioRecorderUtilityLC()
	{
	const TInt KMaxFileSize = 200000;

	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a descriptor, forcing record controller"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		User::Leave(KErrNotFound);
		}
		
	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);


	HBufC8* iBuf = HBufC8::NewL( KMaxFileSize );
	CleanupStack::PushL(iBuf);

	iFileDesc = new (ELeave) TPtr8( NULL, 0 );
	CleanupStack::PushL(iFileDesc);

	iFileDesc->Set( iBuf->Des());


	//Put some contents (one sample wav file) into the descriptor
	TPtrC oneSampleFileName;
	if(!GetStringFromConfig(iSectName, _L("OneSampleFile"), oneSampleFileName))
		{
		User::Leave(KErrNotFound);
		}
		
	RFs fs;
	RFile file;
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,oneSampleFileName,EFileRead));
	CleanupClosePushL(file);
	if(file.Read(*iFileDesc) != KErrNone)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(&file);


	recUtil->OpenDesL(*iFileDesc, iController);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::Pop(iFileDesc);
	CleanupStack::Pop(iBuf);
	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoPlaybackDescForceController::RecordSuccessfull()
	{
	TVerdict testStatus = EPass;

	TInt len = iFileDesc->Length();

	if(len == 0)
		{
		testStatus = EFail;
		}
		
	return testStatus;
	}








//------------------------------------------------------------------
/**
 * Constructor baseclass 
 */
CTestMmfAclntRecNoRecord::CTestMmfAclntRecNoRecord(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

void CTestMmfAclntRecNoRecord::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecNoRecord : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	//dont stop the playing track
	if (aCurrentState != CMdaAudioClipUtility::EPlaying || iError != KErrNone)
		{
		CActiveScheduler::Stop();
		}	
	}

TVerdict CTestMmfAclntRecNoRecord::DoTestStepPreambleL()
	{
	// Make sure file exists
	TParse fullFileName;
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 

	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);
		

	// parse the filenames
	if(!fullFileName.Set(filename, &KDefault, NULL))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Open(fs,filename1,EFileRead));
		file.Close();
		fs.Close();
		}

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}




TVerdict CTestMmfAclntRecNoRecord::DoTestStepL()
	{
	TVerdict testStatus = EPass;
	TInt trapRet = KErrNone;
	TInt ret = KErrNone;

	const TInt durError = 2500; //As best as could be read from Goldwave
	const TInt posError = 250;
	const TInt MainTst_KDur = 8359100; //MainTst.wav = 8.3591 sec from GoldWave

	iRecorder = CMdaAudioRecorderUtilityLC();

	if(!iRecorder)
		{
		testStatus = EFail;
		}
		

	//Should always work, must be done before play or record to ensure in EPrimed state
	if(testStatus == EPass)
		{
		TRAP(trapRet,iRecorder->SetPriority(EMdaPriorityMin,EMdaPriorityPreferenceNone));
		if(trapRet != KErrNone)
			{
			INFO_PRINTF2(_L("SetPriority leave with error %d"),trapRet);
			testStatus = EFail;
			}
		}



	if(testStatus == EPass)
		{
		iError = KErrTimedOut;
		iRecorder->PlayL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open -> record
	
		testStatus =  PlaySuccessfull();
		}

	
	//Can't detect difference in these interfaces
	//void ClearPlayWindow();
	//void SetRepeats(....);
	//void Pause();
	//void SetMaxWriteLength(TInt aMaxWriteLength /*= KMdaClipLocationMaxWriteLengthNone*/);
	//void SetVolume(TInt aVolume);
	//void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//These interfaces should always WORK if there is a record or playback controller
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//if no rec controller, report playback duration.
	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds duration = iRecorder->Duration();

		if((duration.Int64() > MainTst_KDur + durError) ||
		   (duration.Int64() < MainTst_KDur - durError))
			{
			INFO_PRINTF1(_L("Duration fail"));
			testStatus = EFail;
			}
		}	


	//returns rec values if rec controller, else playback values
	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds setPosn(KTwoSeconds);
	
		iRecorder->SetPosition(setPosn);
		TTimeIntervalMicroSeconds getPosn = iRecorder->Position();
		INFO_PRINTF3(_L("got position %d should be %d"),I64LOW(getPosn.Int64()), KTwoSeconds);

		if((getPosn.Int64() > KTwoSeconds+posError) ||
		   (getPosn.Int64() < KTwoSeconds-posError)) 
			{
			INFO_PRINTF1(_L("Position fail"));
			testStatus = EFail;
			}
		}	




	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//These interfaces should FAIL if there is a playback controller, but no record controller
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	if(testStatus == EPass)
		{
		TInt gain;
		iRecorder->SetGain(1);

		ret = iRecorder->GetGain(gain);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("Gain fail"));
			testStatus = EFail;
			}
		}

	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds setPosn(KTwoSeconds);
		iRecorder->SetPosition(setPosn);

		TRAP(trapRet, iRecorder->CropFromBeginningL());
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("Crop fail"));
			testStatus = EFail;
			}
		}

	if(testStatus == EPass)
		{
		TTimeIntervalMicroSeconds zero(0);

		TTimeIntervalMicroSeconds recTimeAvail = iRecorder->RecordTimeAvailable();
		if(recTimeAvail != zero)
			{
			INFO_PRINTF1(_L("RecordTimeAvailable fail"));
			testStatus = EFail;
			}
		}


	if(testStatus == EPass)
		{
		TInt bal = KMMFBalanceMaxRight;
		ret = iRecorder->SetRecordBalance(KMMFBalanceMaxLeft);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("SetRecordBalance test failed"));
			testStatus = EFail;
			}

		ret = iRecorder->GetRecordBalance(bal);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("GetRecordBalance test failed"));
			testStatus = EFail;
			}
		}


	//Haven't got a format that supports these - trace into code.
	/*
	if(testStatus == EPass)
		{
		TInt metaEntries;
		CMMFMetaDataEntry* metaEntry = NULL;
		CMMFMetaDataEntry* newMetaEntry = CMMFMetaDataEntry::NewL(_L("dummy meta"), _L("dummy val"));
		
		iRecorder->GetNumberOfMetaDataEntries(metaEntries);
		TRAP(trapRet, metaEntry = iRecorder->GetMetaDataEntryL(1));
		TRAP(trapRet, iRecorder->AddMetaDataEntryL(*newMetaEntry));
		TRAP(trapRet, iRecorder->ReplaceMetaDataEntryL(1, *newMetaEntry));
		iRecorder->RemoveMetaDataEntry(1);
		}
	*/


	if(testStatus == EPass)
		{
		RArray<TFourCC> codecs;
		TRAP(trapRet, iRecorder->GetSupportedDestinationDataTypesL(codecs));
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("GetSupportedDestinationDataTypesL test failed"));
			testStatus = EFail;
			}

		TFourCC dataType;
		TRAP(trapRet, iRecorder->SetDestinationDataTypeL(dataType));
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("SetDestinationDataTypeL test failed"));
			testStatus = EFail;
			}

		TRAP(trapRet, dataType = iRecorder->DestinationDataTypeL());
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("DestinationDataTypeL test failed"));
			testStatus = EFail;
			}
		}


	//These methods are not available for our formats and will therefore always fail - trace into code.
	/*
	if(testStatus == EPass)
		{
		RArray<TUint> bitRates;
		TUint bitRate = 0;

		TRAP(trapRet, iRecorder->GetSupportedBitRatesL(bitRates));
		if((trapRet == KErrNone) && (bitRates.Count() >0))
			TRAP(trapRet, iRecorder->SetDestinationBitRateL(bitRates[0]));
		if((trapRet == KErrNone) && (bitRates.Count() >0))
			TRAP(trapRet, bitRate = iRecorder->DestinationBitRateL());

		if((trapRet != KErrNone) || (bitRate != bitRates[0]))
			{
			INFO_PRINTF1(_L("BitRates fail"));
			testStatus = EFail; 
			}
		}
	*/

	if(testStatus == EPass)
		{
		RArray<TUint> sampleRates;
		TUint sampleRate = 0;

		TRAP(trapRet, iRecorder->GetSupportedSampleRatesL(sampleRates));
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("GetSupportedSampleRatesL test failed"));
			testStatus = EFail;
			}

		sampleRate = 8000;
		TRAP(trapRet, iRecorder->SetDestinationSampleRateL(sampleRate));
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("SetDestinationSampleRateL test failed"));
			testStatus = EFail;
			}
		
		TRAP(trapRet, sampleRate = iRecorder->DestinationSampleRateL());
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("DestinationSampleRateL test failed"));
			testStatus = EFail;
			}
		}



	if(testStatus == EPass)
		{
		RArray<TUint> channels;
		TUint channel = 0;

		TRAP(trapRet, iRecorder->GetSupportedNumberOfChannelsL(channels));
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("GetSupportedNumberOfChannelsL test failed"));
			testStatus = EFail;
			}


		TRAP(trapRet, iRecorder->SetDestinationNumberOfChannelsL(1));
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("SetDestinationNumberOfChannelsL test failed"));
			testStatus = EFail;
			}

		TRAP(trapRet, channel=iRecorder->DestinationNumberOfChannelsL());
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF2(_L("DestinationNumberOfChannelsL test failed with channel %d"), channel);
			testStatus = EFail;
			}
		}


	if(testStatus == EPass)
		{
		TUid currentFormat = KNullUid;

		TRAP(trapRet, currentFormat = iRecorder->DestinationFormatL());
		if(trapRet != KErrNotSupported)
			{
			INFO_PRINTF1(_L("DestinationFormatL test failed"));
			testStatus = EFail;
			}

		//NB: Can't do this test here, as it causes seg violation - trace into code
		/*
		TUid newFormat = {KMmfUidFormatRAWWrite};
		if(trapRet == KErrNone)
			TRAP(trapRet, iRecorder->SetDestinationFormatL(newFormat));
		if(trapRet == KErrNone)
			TRAP(trapRet, iRecorder->SetDestinationFormatL(currentFormat));
		*/
		}



	if(testStatus == EPass)
		{
		TMMFMessageDestinationPckg dummyPckg;
		TInt dummyFunc = 100;
		TBuf8<8> dummyBuff;

		ret = iRecorder->RecordControllerCustomCommandSync(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("RecordControllerCustomCommandSync - 1 fail"));
			testStatus = EFail; 
			}

		ret = iRecorder->RecordControllerCustomCommandSync(dummyPckg, dummyFunc, dummyBuff, dummyBuff);
		if(ret != KErrNotSupported)
			{
			INFO_PRINTF1(_L("RecordControllerCustomCommandSync -2 fail"));
			testStatus = EFail; 
			}

		CTestMmfAclntAOCallback* aoCallback =  new CTestMmfAclntAOCallback(this);
		CleanupStack::PushL(aoCallback);

		aoCallback->PublicSetActive();
		iRecorder->RecordControllerCustomCommandAsync(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff, aoCallback->iStatus);
		CActiveScheduler::Start();

		if(iError != KErrNotSupported)
			{
			INFO_PRINTF1(_L("RecordControllerCustomCommandAsync -1 fail"));
			testStatus = EFail; 
			}

		aoCallback->PublicSetActive();
		iRecorder->RecordControllerCustomCommandAsync(dummyPckg, dummyFunc, dummyBuff, dummyBuff, aoCallback->iStatus);
		CActiveScheduler::Start();

		if(iError != KErrNotSupported)
			{
			INFO_PRINTF1(_L("RecordControllerCustomCommandAsync -2 fail"));
			testStatus = EFail; 
			}

		CleanupStack::Pop(aoCallback);
		}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//These interfaces should PASS if there is a playback controller, but no record controller
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	if(testStatus == EPass)
		{
		const TUint KPlayError = 500000; //1/2 second in play duration

		TTimeIntervalMicroSeconds duration = iRecorder->Duration();

		iRecorder->SetPlayWindow(duration.Int64()/2, duration);

		iError = KErrTimedOut;
		iRecorder->PlayL();

		TTime startTime; startTime.UniversalTime();

		CActiveScheduler::Start();

		TTime endTime; endTime.UniversalTime();		

		TInt64 playTime = endTime.Int64() - startTime.Int64();

		INFO_PRINTF3(_L("Expected to play for %d Usec, played for %d Usec"), I64LOW(duration.Int64()/2), I64LOW(playTime));

		if((I64LOW(playTime) > I64LOW(duration.Int64()/2 + KPlayError)) ||
		   (I64LOW(playTime) < I64LOW(duration.Int64()/2 - KPlayError)))
			{
			INFO_PRINTF1(_L("SetPlayWindow fail"));
			testStatus = EFail; 
			}
		else
			{
			testStatus =  PlaySuccessfull();
			}
		}

	if(testStatus == EPass)
		{
		TInt maxVol = iRecorder->MaxVolume();
		if(maxVol > 0)
			{
			iRecorder->SetVolume(maxVol/2);
			TInt vol;
			ret = iRecorder->GetVolume(vol);
			if((ret != KErrNone) || (vol != maxVol/2))
				{
				INFO_PRINTF1(_L("GetVolume fail"));
				testStatus = EFail; 
				}
			}
		else
			{
			INFO_PRINTF1(_L("MaxVolume fail"));
			testStatus = EFail;
			}

		}

	if(testStatus == EPass)
		{
		TInt bal;
		TInt ret = iRecorder->SetPlaybackBalance(KMMFBalanceMaxRight);
		if(ret == KErrNone)
			{
			ret = iRecorder->GetPlaybackBalance(bal);
			if((ret != KErrNone) || (bal != KMMFBalanceMaxRight))
				{
				INFO_PRINTF1(_L("GetPlaybackBalance fail"));
				testStatus = EFail; 
				}
			}
		else
			{
			INFO_PRINTF1(_L("SetPlaybackBalance fail"));
			testStatus = EFail; 
			}
		}



//  These would pass when a playback controller is present, also need test controller
//	TInt PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
//	TInt PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
//	void PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
//	void PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

	CleanupStack::PopAndDestroy(iRecorder);
	User::After(KOneSecond);
	return	testStatus;
	}














//------------------------------------------------------------------

/**
 * Constructor
 */

CTestMmfAclntRecNoRecordForceFormat::CTestMmfAclntRecNoRecordForceFormat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
: CTestMmfAclntRecNoRecord(aTestName, aSectName, aKeyName)
	{
	iFormat = aFormat;
	}

CTestMmfAclntRecNoRecordForceFormat* CTestMmfAclntRecNoRecordForceFormat::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoRecordForceFormat* self = new (ELeave) CTestMmfAclntRecNoRecordForceFormat(aTestName,aSectName,aKeyName,aFormat);
	return self;
	}

CTestMmfAclntRecNoRecordForceFormat* CTestMmfAclntRecNoRecordForceFormat::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoRecordForceFormat* self = CTestMmfAclntRecNoRecordForceFormat::NewLC(aTestName,aSectName,aKeyName,aFormat);
	CleanupStack::PushL(self);
	return self;
	}




CMdaAudioRecorderUtility* CTestMmfAclntRecNoRecordForceFormat::CMdaAudioRecorderUtilityLC()
	{
	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		return NULL;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TMdaFileClipLocation location(iFilename);

	recUtil->OpenL(&location,&iFormat);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoRecordForceFormat::PlaySuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError != KErrNone)
		{
		testStatus = EFail;
		}
		
	return testStatus;
	}
	



CTestMmfAclntRecNoRecordForceFormatDesc* CTestMmfAclntRecNoRecordForceFormatDesc::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoRecordForceFormatDesc* self = new (ELeave) CTestMmfAclntRecNoRecordForceFormatDesc(aTestName,aSectName,aKeyName,aFormat);
	return self;
	}

CTestMmfAclntRecNoRecordForceFormatDesc* CTestMmfAclntRecNoRecordForceFormatDesc::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat)
	{
	CTestMmfAclntRecNoRecordForceFormatDesc* self = CTestMmfAclntRecNoRecordForceFormatDesc::NewLC(aTestName,aSectName,aKeyName,aFormat);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoRecordForceFormatDesc::CMdaAudioRecorderUtilityLC()
	{
	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		User::Leave(KErrNotFound);
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);


	RFs fs;
	RFile file;
	TInt size = 0;
	
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,iFilename,EFileRead));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Size(size));
		
	if(size == 0)
		{
		User::Leave(KErrNotFound);
		}

	HBufC8* iBuf = HBufC8::NewL( size );
	CleanupStack::PushL(iBuf);

	iFileDesc = new (ELeave) TPtr8( NULL, 0 );
	CleanupStack::PushL(iFileDesc);

	iFileDesc->Set( iBuf->Des());

	if(file.Read(*iFileDesc) != KErrNone)
		{
		User::Leave(KErrNotFound);
		}

	TMdaDesClipLocation location(*iFileDesc);

	recUtil->OpenL(&location,&iFormat);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::Pop(iFileDesc);
	CleanupStack::Pop(iBuf);
	CleanupStack::PopAndDestroy(&file);
	return	recUtil;
	}


TVerdict CTestMmfAclntRecNoRecordForceFormatDesc::PlaySuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError != KErrNone)
		{
		testStatus = EFail;
		}
		
	return testStatus;
	}


//------------------------------------------------------------------


CTestMmfAclntRecNoRecordFile* CTestMmfAclntRecNoRecordFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoRecordFile* self = new (ELeave) CTestMmfAclntRecNoRecordFile(aTestName,aSectName,aKeyName);
	return self;
	}

CTestMmfAclntRecNoRecordFile* CTestMmfAclntRecNoRecordFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoRecordFile* self = CTestMmfAclntRecNoRecordFile::NewLC(aTestName,aSectName,aKeyName);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoRecordFile::CMdaAudioRecorderUtilityLC()
	{
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a file"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		return NULL;
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	recUtil->OpenFileL(iFilename);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoRecordFile::PlaySuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError != KErrNone)
		{
		testStatus = EFail;
		}
		
	return testStatus;
	}
	



CTestMmfAclntRecNoRecordDesc* CTestMmfAclntRecNoRecordDesc::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoRecordDesc* self = new (ELeave) CTestMmfAclntRecNoRecordDesc(aTestName,aSectName,aKeyName);
	return self;
	}

CTestMmfAclntRecNoRecordDesc* CTestMmfAclntRecNoRecordDesc::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRecNoRecordDesc* self = CTestMmfAclntRecNoRecordDesc::NewLC(aTestName,aSectName,aKeyName);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoRecordDesc::CMdaAudioRecorderUtilityLC()
	{
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a descriptor"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		User::Leave(KErrNotFound);
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);


	RFs fs;
	RFile file;
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,iFilename,EFileRead));
	CleanupClosePushL(file);

	TInt size;
	User::LeaveIfError(file.Size(size));
		
	if(size == 0)
		{
		User::Leave(KErrNotFound);
		}


	HBufC8* iBuf = HBufC8::NewL( size );
	CleanupStack::PushL(iBuf);

	iFileDesc = new (ELeave) TPtr8( NULL, 0 );
	CleanupStack::PushL(iFileDesc);

	iFileDesc->Set( iBuf->Des());

	if(file.Read(*iFileDesc) != KErrNone)
		{
		User::Leave(KErrNotFound);
		}


	recUtil->OpenDesL(*iFileDesc);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::Pop(iFileDesc);
	CleanupStack::Pop(iBuf);
	CleanupStack::PopAndDestroy(&file);
	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoRecordDesc::PlaySuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError != KErrNone)
		{
		testStatus = EFail;
		}
		
	return testStatus;
	}




CTestMmfAclntRecNoRecordFileForceController* CTestMmfAclntRecNoRecordFileForceController::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoRecordFileForceController* self = new (ELeave) CTestMmfAclntRecNoRecordFileForceController(aTestName,aSectName,aKeyName, aController);
	return self;
	}

CTestMmfAclntRecNoRecordFileForceController* CTestMmfAclntRecNoRecordFileForceController::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoRecordFileForceController* self = CTestMmfAclntRecNoRecordFileForceController::NewLC(aTestName,aSectName,aKeyName,aController);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoRecordFileForceController::CMdaAudioRecorderUtilityLC()
	{
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a file, forcing record controller"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		return NULL;
		}
		

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	recUtil->OpenFileL(iFilename, KNullUid, iController);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoRecordFileForceController::PlaySuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError != KErrNone)
		{
		testStatus = EFail;
		}
		

	return testStatus;
	}


CTestMmfAclntRecNoRecordDescForceController* CTestMmfAclntRecNoRecordDescForceController::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoRecordDescForceController* self = new (ELeave) CTestMmfAclntRecNoRecordDescForceController(aTestName,aSectName,aKeyName,aController);
	return self;
	}

CTestMmfAclntRecNoRecordDescForceController* CTestMmfAclntRecNoRecordDescForceController::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TUid aController)
	{
	CTestMmfAclntRecNoRecordDescForceController* self = CTestMmfAclntRecNoRecordDescForceController::NewLC(aTestName,aSectName,aKeyName,aController);
	CleanupStack::PushL(self);
	return self;
	}


CMdaAudioRecorderUtility* CTestMmfAclntRecNoRecordDescForceController::CMdaAudioRecorderUtilityLC()
	{
	INFO_PRINTF1( _L("Create CMdaAudioRecorderUtility with a descriptor, forcing record controller"));

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		User::Leave(KErrNotFound);
		}
		
	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);


	RFs fs;
	RFile file;
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,iFilename,EFileRead));
	CleanupClosePushL(file);

	TInt size;
	User::LeaveIfError(file.Size(size));
		
	if(size == 0)
		{
		User::Leave(KErrNotFound);
		}



	HBufC8* iBuf = HBufC8::NewL( size );
	CleanupStack::PushL(iBuf);

	iFileDesc = new (ELeave) TPtr8( NULL, 0 );
	CleanupStack::PushL(iFileDesc);

	iFileDesc->Set( iBuf->Des());

	if(file.Read(*iFileDesc) != KErrNone)
		{
		User::Leave(KErrNotFound);
		}


	recUtil->OpenDesL(*iFileDesc, KNullUid, iController);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone)
		{
		User::Leave(KErrNotFound);
		}

	CleanupStack::Pop(iFileDesc);
	CleanupStack::Pop(iBuf);
	CleanupStack::PopAndDestroy(&file);
	return	recUtil;
	}

TVerdict CTestMmfAclntRecNoRecordDescForceController::PlaySuccessfull()
	{
	TVerdict testStatus = EPass;

	if(iError != KErrNone)
		{
		testStatus = EFail;
		}
		
	return testStatus;
	}



//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntRecFileAppend::CTestMmfAclntRecFileAppend(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType, TBool aDiffSampleRate)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iFormatType = aFormatType;
	iCodecType = aCodecType;
	iDiffSampleRate = aDiffSampleRate;
	}

CTestMmfAclntRecFileAppend* CTestMmfAclntRecFileAppend::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType, TBool aDiffSampleRate)
	{
	CTestMmfAclntRecFileAppend* self = new (ELeave) CTestMmfAclntRecFileAppend(aTestName, aSectName, aKeyName, aFormatType, aCodecType, aDiffSampleRate);
	return self;
	}

void CTestMmfAclntRecFileAppend::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	INFO_PRINTF3( _L("MoscoStateChangeEvent: aCurrentState = %d, iError %d "), iCurrentState, iError);

	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntRecFileAppend::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	// Make sure file doesn't exist (maybe from a previous test)
	TParse fullFileName;
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 

	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
		

	GetDriveName(filename);
	filename.Append(filename1);
		

	// parse the filenames
	if(!fullFileName.Set(filename, &KDefault, NULL))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
		TInt error = fs.Delete(fullFileName.FullName());

		if(error == KErrPathNotFound)
			{
			error = fs.MkDirAll(fullFileName.DriveAndPath());
			INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
			}
		else  if(error == KErrNotFound)
			INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
		else if(error==KErrNone)
			INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
		else
			{
			ret = EInconclusive;
			}
		fs.Close();
		}

	if((ret == EInconclusive) || (ret == EFail))
		return ret;	
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}


const TInt KSeventyFive = 75;	
const TInt KTwentyFive = 25;	
const TInt KOneHundred = 100;	

/**
 * Open a file based clip and record, stop & then record again
 */


TVerdict CTestMmfAclntRecFileAppend::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File"));
	TVerdict ret = EPass;
	iError = KErrGeneral;	// set error to ensure callback takes place

	TInt size1 = 0;		// size of file after first record
	TInt size2 = 0;		// size of file after append

	TPtrC filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		return EInconclusive;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TMdaFileClipLocation location(filename);

	RFs fs;
	fs.Connect();
	fs.Delete(filename);
	fs.Close();

	for (TInt index=0; index<2; index++)
		{
		if (index == 0)
			INFO_PRINTF1(_L("Recording to file..."));
		else
			INFO_PRINTF1(_L("Appending to file..."));
		
		TMdaWavClipFormat mdaWavFormat;
		TMdaAuClipFormat mdaAuFormat;
		TMdaClipFormat* format = NULL;

		TMdaImaAdpcmWavCodec wavImadCodec;
		TMdaPcmWavCodec wav8PcmCodec;
		TMdaPcm8BitAuCodec au8PcmCodec;

		TMdaPackage* codec = NULL;

		switch (iFormatType)
			{
			case EWavFormat:
				format = &mdaWavFormat;
				switch (iCodecType)
					{
					case EAdpcmCodec:
						codec = &wavImadCodec;
						break;
					case EPcmCodec:
						codec = &wav8PcmCodec;
						break;
					}
				break;
			case EAuFormat:
				format = &mdaAuFormat;
				switch (iCodecType)
					{
					case EPcmCodec:
						codec = &au8PcmCodec;
						break;
					default:
						break;
					}
				break;
			}

		if (format == NULL || codec == NULL)
			{
			ret = EInconclusive;
			break;
			}

		TMdaAudioDataSettings audioSettings;
		audioSettings.iCaps = 
			TMdaAudioDataSettings::ESampleRateFixed | 
			TMdaAudioDataSettings::EChannelsMono |
			TMdaAudioDataSettings::ESampleRate8000Hz |
			TMdaAudioDataSettings::ESampleRate11025Hz;

		audioSettings.iSampleRate = KSampleRate8K;
		audioSettings.iChannels = 1;
		audioSettings.iFlags = TMdaAudioDataSettings::ENoNetworkRouting;
		if (index == 1 && iDiffSampleRate)
			{
			audioSettings.iSampleRate = KSampleRate11K;
			}
			
			
		recUtil->OpenL(&location, format, codec, &audioSettings);

		INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
		CActiveScheduler::Start();	// idle -> open
		
		if(iError != KErrNone)
			{
			break;
			}
	
		iError = KErrTimedOut;
		recUtil->RecordL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open -> record

		User::After(KTwoSeconds);
		
		if (iCurrentState != CMdaAudioClipUtility::ERecording)
			{
			INFO_PRINTF1( _L("CMdaAudioRecorderUtility is not recording"));
			ret = EFail;
			break;
			}

		recUtil->Stop();

				
		if (index == 1)
			{
			TTimeIntervalMicroSeconds actualDuration;
			TTimeIntervalMicroSeconds expectedDuration(KFourSeconds);
			actualDuration = recUtil->Duration();
			INFO_PRINTF3(_L("Duration recorded file, expected =  %f, actual: %f"),
				I64REAL(expectedDuration.Int64()), I64REAL(actualDuration.Int64()));
			// allow a deviation of 25%
			if (actualDuration.Int64() < expectedDuration.Int64() * KSeventyFive / KOneHundred)
				{
				ERR_PRINTF1(_L("Appended duration is too short"));
				ret = EFail;
				}
			else if (actualDuration.Int64() > expectedDuration.Int64() * (KOneHundred+KTwentyFive) / KOneHundred)
				{
				ERR_PRINTF1(_L("Appended duration is too long"));
				ret = EFail;
				}
			}

		
		
		recUtil->Close();
	
		if(iError == KErrNone) 
			{
			RFs fs;
			RFile file;
			TInt size = 0;

			User::LeaveIfError(fs.Connect());
			User::LeaveIfError(file.Open(fs,filename,EFileRead));
			CleanupClosePushL(file);
			User::LeaveIfError(file.Size(size));
			INFO_PRINTF2(_L("File size = %d"), size);

			if(size <= 0)
				{
				ret = EFail;
				}
				
			CleanupStack::PopAndDestroy(&file);

			if (index == 0)
				{	
				size1 = size;
				}
				
			else
				{
				size2 = size;
				}
				
			}
		}

	if (iError != KErrNone)
		{
		ret = EFail;
		}
		

	// the appended size should be double the original size
	// but to allow a little deviation, ensure it's at least 1.5 times bigger
	if (ret == EPass)
		{
		if (size2 < (size1*(KOneHundred+KSeventyFive)/KOneHundred))
			{
			ERR_PRINTF1(_L("Appended file size is too short"));
			ret = EFail;
			}
		else if (size2 > (size1*(KOneHundred+KOneHundred+KTwentyFive)/KOneHundred))
			{
			ERR_PRINTF1(_L("Appended file size is too long"));
			ret = EFail;
			}
		}

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	CleanupStack::PopAndDestroy(recUtil);
	User::After(KOneSecond);
	return	ret;
	}


/**
 * Constructor
 */
CTestMmfAclntRecFilePlay::CTestMmfAclntRecFilePlay(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iFormatType = aFormatType;
	iCodecType = aCodecType;
	}

CTestMmfAclntRecFilePlay* CTestMmfAclntRecFilePlay::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType)
	{
	CTestMmfAclntRecFilePlay* self = new (ELeave) CTestMmfAclntRecFilePlay(aTestName, aSectName, aKeyName, aFormatType, aCodecType);
	return self;
	}

void CTestMmfAclntRecFilePlay::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	INFO_PRINTF3( _L("MoscoStateChangeEvent: aCurrentState = %d, iError %d "), iCurrentState, iError);

	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntRecFilePlay::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	// Make sure file doesn't exist (maybe from a previous test)
	TParse fullFileName;
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 

	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		return EInconclusive;

	GetDriveName(filename);
	filename.Append(filename1);
		

	// parse the filenames
	if(!fullFileName.Set(filename, &KDefault, NULL))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
		TInt error = fs.Delete(fullFileName.FullName());

		if(error == KErrPathNotFound)
			{
			error = fs.MkDirAll(fullFileName.DriveAndPath());
			INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
			}
		else  if(error == KErrNotFound)
			INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
		else if(error==KErrNone)
			INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
		else
			{
			ret = EInconclusive;
			}
		fs.Close();
		}

	if((ret == EInconclusive) || (ret == EFail))
		return ret;	
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Open a file based clip and record, stop & then play
 * specifying a different sample rate / number of channels
 */


TVerdict CTestMmfAclntRecFilePlay::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File & play with diff sample rate/num of channels"));
	TVerdict ret = EPass;
	iError = KErrGeneral;	// set error to ensure callback takes place

	TPtrC filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		return EInconclusive;

	CMdaAudioRecorderUtility* recUtil = NULL;

	TMdaFileClipLocation location(filename);

	RFs fs;
	fs.Connect();
	fs.Delete(filename);
	fs.Close();

	for (TInt index=0; index<2; index++)
		{
		if (recUtil)
			{
			CleanupStack::PopAndDestroy(recUtil);
			recUtil = NULL;
			}

		recUtil = CMdaAudioRecorderUtility::NewL(*this);
		CleanupStack::PushL(recUtil);

		if (index == 0)
			INFO_PRINTF1(_L("Recording to file..."));
		else
			INFO_PRINTF1(_L("Playing recorded file"));
		
		TMdaWavClipFormat mdaWavFormat;
		TMdaAuClipFormat mdaAuFormat;
		TMdaClipFormat* format = NULL;

		TMdaImaAdpcmWavCodec wavImadCodec;
		TMdaPcmWavCodec wav8PcmCodec;
		TMdaPcm8BitAuCodec au8PcmCodec;

		TMdaPackage* codec = NULL;

		switch (iFormatType)
			{
			case EWavFormat:
				format = &mdaWavFormat;
				switch (iCodecType)
					{
					case EAdpcmCodec:
						codec = &wavImadCodec;
						break;
					case EPcmCodec:
						codec = &wav8PcmCodec;
						break;
					}
				break;
			case EAuFormat:
				format = &mdaAuFormat;
				switch (iCodecType)
					{
					case EPcmCodec:
						codec = &au8PcmCodec;
						break;
					default:
						break;
					}
				break;
			}

		if (format == NULL || codec == NULL)
			{
			ret = EInconclusive;
			break;
			}

		TMdaAudioDataSettings audioSettings;
		audioSettings.iCaps = 
			TMdaAudioDataSettings::ESampleRateFixed | 
			TMdaAudioDataSettings::EChannelsMono |
			TMdaAudioDataSettings::EChannelsStereo |
			TMdaAudioDataSettings::ESampleRate8000Hz |
			TMdaAudioDataSettings::ESampleRate11025Hz;

		audioSettings.iFlags = TMdaAudioDataSettings::ENoNetworkRouting;

		// record in stereo at 8KHz, playback in mono at 11KHz
		if (index == 0)	// record
			{
			audioSettings.iSampleRate = KSampleRate8K;
			audioSettings.iChannels = 2;
			}
		else			// play
			{
			audioSettings.iSampleRate = KSampleRate11K;
			audioSettings.iChannels = 1;
			}

			
		recUtil->OpenL(&location, format, codec, &audioSettings);

		INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
		CActiveScheduler::Start();	// idle -> open
		
		if(iError != KErrNone)
			break;

		iError = KErrTimedOut;
		
		if (index == 0)		// record
			{
			recUtil->RecordL();
			INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
			CActiveScheduler::Start(); // open -> record

			User::After(KTwoSeconds);
			
			if (iCurrentState != CMdaAudioClipUtility::ERecording)
				{
				INFO_PRINTF1( _L("CMdaAudioRecorderUtility is not recording"));
				ret = EFail;
				break;
				}

			recUtil->Stop();

			}
		else				// play
			{
			recUtil->PlayL();
			INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
			CActiveScheduler::Start(); // open -> play
			CActiveScheduler::Start(); // play -> open
			}

		recUtil->Close();
	

		}

	if (iError != KErrNone)
		ret = EFail;

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	if (recUtil)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	User::After(KOneSecond);
	return	ret;
	}


/**
 * DEF039893
 * Gets balance prior to setting balance
 * @class CTestMmfAclntRecord
 */
CTestMmfAclntRecordGetBalance::CTestMmfAclntRecordGetBalance(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	}

CTestMmfAclntRecordGetBalance* CTestMmfAclntRecordGetBalance::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntRecordGetBalance* self = new (ELeave) CTestMmfAclntRecordGetBalance(aTestName);
	return self;
	}

void CTestMmfAclntRecordGetBalance::MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode)
	{
	iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;
	iError = aErrorCode;
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecordGetBalance::DoTestStepL()
	{
	// Set up a file to record to
	TVerdict verdict = EPass;

	TPtrC filename;
	if (!GetStringFromConfig(_L("SectionThree"), _L("outputAudioFile"), filename))
		{
		INFO_PRINTF1(_L("Error getting filename from INI file"));
		return EInconclusive;
		}

	RFs fs;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting file session"), err);
		return EInconclusive;
		}

	err = fs.Delete(filename);
	if (!((err == KErrNone) || (err == KErrNotFound)))
		{
		INFO_PRINTF2(_L("Error deleting output file"), err);
		return EInconclusive;
		}

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);

	TInt balance;
	err = recUtil->GetPlaybackBalance(balance);

	if (err != KErrNotSupported)
		{//would have thought KErrNotReady would be a more logical error
		verdict = EFail;
		}

	TRAP(err,recUtil->OpenFileL(filename));
	CActiveScheduler::Start();

	if (err)
		{
		INFO_PRINTF2(_L("Error opening output file"), err);
		verdict =  EInconclusive;
		}
	else
		{
		err = recUtil->GetPlaybackBalance(balance);

		if ((err)||(balance))
			{//there should be no errors and balance should be 0
			verdict = EFail;
			}

		recUtil->Close();

		err = recUtil->GetPlaybackBalance(balance);
		if (err != KErrNotSupported)
			{
			verdict = EFail;
			}
		}

	delete recUtil;

	return verdict;
	}
/*
DEF039308 - No integration test to test playback of recorded raw file
The tmfad record test option b + 4 to record an alaw raw file and then play it back: this uncovered a defect that was not picked up by the integration tests suggesting there are no integration tests to record and playback a recorded raw file.
Not clear whether this applies to just recording raw alaw or recorded raw in general- this needs checking as part of the 'under investigation' phase. 
*/

/**
 * Constructor
 */
CTestMmfAclntRecRawFilePlay::CTestMmfAclntRecRawFilePlay(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iFormatType = aFormatType;
	iCodecType = aCodecType;
	}

CTestMmfAclntRecRawFilePlay* CTestMmfAclntRecRawFilePlay::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType)
	{
	CTestMmfAclntRecRawFilePlay* self = new (ELeave) CTestMmfAclntRecRawFilePlay(aTestName, aSectName, aKeyName, aFormatType, aCodecType);
	return self;
	}

void CTestMmfAclntRecRawFilePlay::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	INFO_PRINTF3( _L("MoscoStateChangeEvent: aCurrentState = %d, iError %d "), iCurrentState, iError);

	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntRecRawFilePlay::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	// Make sure file doesn't exist (maybe from a previous test)
	TParse fullFileName;
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 

	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		return EInconclusive;

	GetDriveName(filename);
	filename.Append(filename1);
		

	// parse the filenames
	if(!fullFileName.Set(filename, &KDefault, NULL))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
		TInt error = fs.Delete(fullFileName.FullName());

		if(error == KErrPathNotFound)
			{
			error = fs.MkDirAll(fullFileName.DriveAndPath());
			INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
			}
		else  if(error == KErrNotFound)
			INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
		else if(error==KErrNone)
			INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
		else
			{
			ret = EInconclusive;
			}
		fs.Close();
		}

	if((ret == EInconclusive) || (ret == EFail))
		return ret;	
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Open a file based clip and record, stop & then play
 * specifying a different sample rate / number of channels
 */


TVerdict CTestMmfAclntRecRawFilePlay::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record RAW File & play with diff sample rate/num of channels"));
	TVerdict ret = EPass;
	iError = KErrGeneral;	// set error to ensure callback takes place

	TPtrC filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		return EInconclusive;

	CMdaAudioRecorderUtility* recUtil = NULL;

	TMdaFileClipLocation location(filename);

	RFs fs;
	fs.Connect();
	fs.Delete(filename);
	fs.Close();

	for (TInt index=0; index<2; index++)
		{
		if (recUtil)
			{
			CleanupStack::PopAndDestroy(recUtil);
			recUtil = NULL;
			}

		recUtil = CMdaAudioRecorderUtility::NewL(*this);
		CleanupStack::PushL(recUtil);

		if (index == 0)
			INFO_PRINTF1(_L("Recording to file..."));
		else
			INFO_PRINTF1(_L("Playing recorded file"));
		
		TMdaRawAudioClipFormat mdaRawFormat;
		TMdaClipFormat* format = NULL;

		TMdaAlawRawAudioCodec rawAlawCodec;

		TMdaPackage* codec = NULL;

		switch (iFormatType)
			{
			case ERawFormat:
				format = &mdaRawFormat;
				switch (iCodecType)
					{
					case EAlawCodec:
						codec = &rawAlawCodec;
						break;
					default:
						break;
					}
				break;
			}

		if (format == NULL || codec == NULL)
			{
			ret = EInconclusive;
			break;
			}

		TMdaAudioDataSettings audioSettings;
		audioSettings.iCaps = 
			TMdaAudioDataSettings::ESampleRateFixed | 
			TMdaAudioDataSettings::EChannelsMono |
			TMdaAudioDataSettings::EChannelsStereo |
			TMdaAudioDataSettings::ESampleRate8000Hz |
			TMdaAudioDataSettings::ESampleRate11025Hz;

		audioSettings.iFlags = TMdaAudioDataSettings::ENoNetworkRouting;

		// record in stereo at 8KHz, playback in mono at 11KHz
		if (index == 0)	// record
			{
			audioSettings.iSampleRate = KSampleRate8K;
			audioSettings.iChannels = 2;
			}
		else			// play
			{
			audioSettings.iSampleRate = KSampleRate11K;
			audioSettings.iChannels = 1;
			}

			
		recUtil->OpenL(&location, format, codec, &audioSettings);

		INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
		CActiveScheduler::Start();	// idle -> open
		
		if(iError != KErrNone)
			break;

		iError = KErrTimedOut;
		
		if (index == 0)		// record
			{
			recUtil->RecordL();
			INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
			CActiveScheduler::Start(); // open -> record

			User::After(KTwoSeconds);
			
			if (iCurrentState != CMdaAudioClipUtility::ERecording)
				{
				INFO_PRINTF1( _L("CMdaAudioRecorderUtility is not recording"));
				ret = EFail;
				break;
				}

			recUtil->Stop();

			}
		else				// play
			{
			recUtil->PlayL();
			INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
			CActiveScheduler::Start(); // open -> play
			CActiveScheduler::Start(); // play -> open
			}

		recUtil->Close();
	

		}

	if (iError != KErrNone)
		ret = EFail;

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	if (recUtil)
		{
		CleanupStack::PopAndDestroy(recUtil);
		recUtil = NULL;
		}

	User::After(KOneSecond);
	return	ret;
	}


/**
 * REQ2870
 * Sierra: AudioRecorderUtility to get AudioBitrare 
 * @class CTestMmfAclntRecSrcAuBitrate
 */
CTestMmfAclntRecSrcAuBitrate::CTestMmfAclntRecSrcAuBitrate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName1, const TDesC& aKeyName2)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyFileName = aKeyName1;
	iKeyBitRateName = aKeyName2;
	}

CTestMmfAclntRecSrcAuBitrate* CTestMmfAclntRecSrcAuBitrate::NewL(const TDesC& aTestName, 
																const TDesC& aSectName,
																const TDesC& aKeyName1, 
																const TDesC& aKeyName2)
	{
	CTestMmfAclntRecSrcAuBitrate* self = new (ELeave) CTestMmfAclntRecSrcAuBitrate(aTestName, 
																					aSectName, 
																					aKeyName1, 
																					aKeyName2);
	return self;
	}

void CTestMmfAclntRecSrcAuBitrate::MoscoStateChangeEvent(CBase* aObject,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorCode)
	{
	iError = aErrorCode;
	iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;
	INFO_PRINTF1( _L("CTestMmfAclntRecSrcAuBitrate : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF4( _L("iError %d iPreviousState %d iCurrentState %d"), iError, iPreviousState, iCurrentState);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntRecSrcAuBitrate::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Audio Bit Rate"));
	TVerdict verdict = EPass;

	TPtrC filename;
	TInt bitRate = 0;
	TInt err = 0;

	if(!GetStringFromConfig(iSectName,iKeyFileName,filename))
		{
		INFO_PRINTF1(_L("Error getting filename from INI file"));
		return EInconclusive;
		}
	if(!GetIntFromConfig(iSectName, iKeyBitRateName, iExpectedBitRate))
		{
		INFO_PRINTF1(_L("Error getting bitrate from INI file"));
		return EInconclusive;
		}	


	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	if (recUtil == NULL || 
		iError != KErrNone)
		{
		INFO_PRINTF1(_L("Error to create a recUtil object"));
		return EFail;
		}
	CleanupStack::PushL(recUtil);


	INFO_PRINTF2(_L("Opening file = %S"), &filename);
	TRAP(err, recUtil->OpenFileL(filename));
	CActiveScheduler::Start();

	if (err != KErrNone || 
		iError != KErrNone)
		{
		INFO_PRINTF2(_L("Opening file error: %d"), err);
		CleanupStack::PopAndDestroy(recUtil);
		verdict = EFail;
		}
	else
		{
		recUtil->PlayL();
		CActiveScheduler::Start(); // wait for open -> play
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Error to play the file: %d"), iError);
			verdict = EFail;
			}

		TRAP(err, bitRate = recUtil->SourceBitRateL());
		if (err != KErrNone || 
			iError != KErrNone)
			{
			INFO_PRINTF2(_L("Error to query the bitrate: %d"), err);
			verdict = EFail;
			}

		if	(bitRate != iExpectedBitRate)
			{
			verdict = EFail;
			}
		INFO_PRINTF2(_L("Expected bit rate: %d"), iExpectedBitRate);
		INFO_PRINTF2(_L("Queried Bitrate: %d"), bitRate);
		recUtil->Close();
		CleanupStack::PopAndDestroy(recUtil);
		}

	return verdict;
}


/**
 * REQ2870
 * Sierra: AudioRecorderUtility to get AudioBitrare from a recorded file
 * @class CTestMmfAclntRecBitrateRecordedFile
 */
CTestMmfAclntRecBitrateRecordedFile::CTestMmfAclntRecBitrateRecordedFile(const TDesC& aTestName, 
																		 const TDesC& aSectName,
																		 const TDesC& aKeyName, 
																		 const TTestFormat aFormat, 
																		 const TBool aCreateFile)

	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iHeapSize = 200000;		// playback part of test loads whole sound file into buffer, so need large heap
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateFile = aCreateFile;
	}

CTestMmfAclntRecBitrateRecordedFile* CTestMmfAclntRecBitrateRecordedFile::NewL(const TDesC& aTestName, 
																			   const TDesC& aSectName,
																			   const TDesC& aKeyName, 
																			   const TTestFormat aFormat, 
																			   const TBool aCreateFile)
	{
	CTestMmfAclntRecBitrateRecordedFile* self = new (ELeave) CTestMmfAclntRecBitrateRecordedFile(aTestName,
																								 aSectName,
																								 aKeyName,
																								 aFormat,
																								 aCreateFile);
	return self;
	}

void CTestMmfAclntRecBitrateRecordedFile::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntRecBitrateRecordedFile : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntRecBitrateRecordedFile::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	//Setup codec and format to test (CTestMmfAclntCodecTest)
	SetupFormatL(iTestFormat);

	if(iCreateFile)
		{
		// Make sure file doesn't exist (maybe from a previous test)
		TParse fullFileName;
		TBuf<KSizeBuf>	filename;
		TPtrC			filename1; 

		if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			{
			return EInconclusive;
			}
		GetDriveName(filename);
		filename.Append(filename1);
		
		// parse the filenames
		if(!fullFileName.Set(filename, &KDefault, NULL))
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 
			TInt error = fs.Delete(fullFileName.FullName());

			if(error == KErrPathNotFound)
				{
				error = fs.MkDirAll(fullFileName.DriveAndPath());
				INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
				}
			else  if(error == KErrNotFound)
				{
				INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
				}
			else if(error==KErrNone)
				{
				INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
				}
			else
				{
				ret = EInconclusive;
				}
			fs.Close();
			}
		}

	if(ret == EInconclusive)
		{
		return ret;	
		}
	
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}


TVerdict CTestMmfAclntRecBitrateRecordedFile::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Query bitrate from a recorded file"));
	TVerdict verdict = EPass;
	iError = KErrTimedOut;

	TPtrC	filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TMdaFileClipLocation location(filename);

	recUtil->OpenL(&location,iFormat,iCodec,&iAudioSettings);
	CActiveScheduler::Start();
	INFO_PRINTF1( _L("Open a new file for recording"));

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file for recording iError = %d"), iError);
		CleanupStack::PopAndDestroy(recUtil);
		return EFail;
		}

	iError = KErrTimedOut;
	recUtil->RecordL();
	CActiveScheduler::Start(); // open -> record
	INFO_PRINTF1( _L("Recording"));
	User::After(KFiveSeconds);

	if(iError != KErrNone)
		{
		INFO_PRINTF2( _L("Fail to do recording with error: %d"), iError);
		verdict = EFail;
		}

	recUtil->Stop();
	User::After(KOneSecond);
	

	TRAPD(err, recUtil->OpenFileL(filename));
	CActiveScheduler::Start();
	INFO_PRINTF1( _L("Open the recorded file"));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file for playback err = %d"), err);
		CleanupStack::PopAndDestroy(recUtil);
		return EFail;
		}
	
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file for playback iError = %d"), iError);
		CleanupStack::PopAndDestroy(recUtil);
		return EFail;
		}

	recUtil->PlayL();
	CActiveScheduler::Start();
	INFO_PRINTF1( _L("Play the recorded file"));

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error during playback of recorded file iError=%d"), iError);
		verdict = EFail;
		}
	User::After(KFiveSeconds);
		
	TInt bitRate = 0;

	TRAP(err, bitRate = recUtil->SourceBitRateL());
	INFO_PRINTF1( _L("Query bitrate from the recorded file"));

	if (err != KErrNone || 
		iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error to query the bitrate: %d"), err);
		verdict = EFail;
		}

	INFO_PRINTF2(_L("Queried Bitrate: %d"), bitRate);
	
	recUtil->Close();

	CleanupStack::PopAndDestroy(recUtil);
	recUtil = NULL;

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	User::After(KOneSecond);
	return	verdict;
	}

//Record-Stop-Record test. Added for CR1566

CTestMmfAclntRecStopRec::CTestMmfAclntRecStopRec(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)	
	{}

CTestMmfAclntRecStopRec* CTestMmfAclntRecStopRec::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntRecStopRec* self = new (ELeave) CTestMmfAclntRecStopRec(aTestName, aSectName, aNegative);
	return self;
	}

TVerdict CTestMmfAclntRecStopRec::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record-Stop-Record"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TPtrC filename; 


	if(!GetStringFromConfig(iSectName, _L("outputAudioFile"), filename))
		{
		return EInconclusive;
		}
		
	RFs	fs;
	fs.Connect();
	fs.Delete(filename);
	fs.Close();

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);
	
	// Add create file for Ogg-tests
	if(iSectName == _L("SectionOggRecState"))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		RFile file;
		User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
		file.Close();
		fs.Close();
		}
	
	INFO_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL(%S)"), &filename);
	recUtil->OpenFileL(filename);
	// wait
	INFO_PRINTF1(_L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		ret = DoTestL(recUtil);
		}
		

	CleanupStack::PopAndDestroy(recUtil);
	User::After(KTwoSeconds);
	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	return ret;
	}

/**
 * Enquire state at each state transition.
 */
TVerdict CTestMmfAclntRecStopRec::DoTestL(CMdaAudioRecorderUtility* aRecUtil)
	{
	INFO_PRINTF1( _L("TestRecorder : State Transition"));

	TVerdict ret= EFail;

	iReportedState = aRecUtil->State();
	if( iReportedState == CMdaAudioClipUtility::EOpen )
		{
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		aRecUtil->RecordL();
		CActiveScheduler::Start(); // wait for open -> record
		if (iError != KErrNone)
			{
			return EFail;
			}
			
		iReportedState = aRecUtil->State();
		if(iReportedState == CMdaAudioClipUtility::ERecording )
			{
			ret = EPass;
			}
			
		User::After(KTwoSeconds);
		aRecUtil->Stop();

		iReportedState = aRecUtil->State();
		if(iReportedState != CMdaAudioClipUtility::EOpen ) //Check that record returns to open
			{
			return EFail;
			}
			
		aRecUtil->RecordL();
		CActiveScheduler::Start(); // wait for open -> record again
		if (iError != KErrNone)
			{
			return EFail;
			}
			
		iReportedState = aRecUtil->State();
		if( iReportedState == CMdaAudioClipUtility::ERecording )
			{
			aRecUtil->Close();
			ret = EPass;
			}
		}
	return ret;
	}

