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
// Integration tests.
// This program is designed the Audio Recorder tests using TMMSource.
// 
//

/**
 @file TestRecorderFileSource.cpp
*/

#include <hal.h>
#include "mmfwavformat.h"

#include "TestRecorder.h"
#include "TestRecorderFileSource.h"

/**
 * Constructor
 */
CTestMmfAclntRecFileSource::CTestMmfAclntRecFileSource(const TDesC& aTestName, 
                                                       const TDesC& aSectName,
                                                       const TDesC& aKeyName, 
                                                       TOpenFileType aOpenFileType, 
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
	iOpenFileType = aOpenFileType;
	}

CTestMmfAclntRecFileSource* CTestMmfAclntRecFileSource::NewL(const TDesC& aTestName, 
                                                             const TDesC& aSectName, 
                                                             const TDesC& aKeyName, 
                                                             TOpenFileType aOpenFileType, 
                                                             const TTestFormat aFormat, 
                                                             const TBool aCreateFile)
	{
	CTestMmfAclntRecFileSource* self = new (ELeave) CTestMmfAclntRecFileSource(aTestName,aSectName,aKeyName,aOpenFileType,aFormat,aCreateFile);
	return self;
	}

CTestMmfAclntRecFileSource* CTestMmfAclntRecFileSource::NewLC(const TDesC& aTestName, 
                                                              const TDesC& aSectName, 
                                                              const TDesC& aKeyName, 
                                                              TOpenFileType aOpenFileType, 
                                                              const TTestFormat aFormat, 
                                                              const TBool aCreateFile)
	{
	CTestMmfAclntRecFileSource* self = CTestMmfAclntRecFileSource::NewLC(aTestName,aSectName,aKeyName,aOpenFileType,aFormat,aCreateFile);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntRecFileSource::MoscoStateChangeEvent(CBase* /*aObject*/, 
                                                       TInt aPreviousState, 
                                                       TInt aCurrentState, 
                                                       TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF4(_L("MMdaObjectStateChangeObserver:: Previous State : %d, Current State : %d, Error : %d"),aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

// Audio utility callbacks
void CTestMmfAclntRecFileSource::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntRecFileSource::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

// Audio output stream callbacks
void CTestMmfAclntRecFileSource::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntRecFileSource::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntRecFileSource::MaoscPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntRecFileSource::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	SetupFormatL(iTestFormat);

	if(iCreateFile)
		{
		// Make sure file doesn't exist (maybe from a previous test)
		TParse fullFileName;
		
		// Get the file name from ini file.
		TBuf<KSizeBuf> filename;
		TPtrC filename1;
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

	if((ret == EInconclusive) || (ret == EFail))
		{
		return ret;
		}
	
	// Call Base - DoTestStepPreambleL()
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * DoTestStepPostambleL
 */
TVerdict CTestMmfAclntRecFileSource::DoTestStepPostambleL()
	{
	// 
	// Call Base - DoTestStepPostambleL()
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Open a file based clip and record
 */
TVerdict CTestMmfAclntRecFileSource::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File"));
	// Loval Variables
	TVerdict ret = EFail;
	iError = KErrTimedOut;
	// Get the file name from ini file.
	TBuf<KSizeBuf> filename;
	TPtrC filename1;
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
	GetDriveName(filename);
	filename.Append(filename1);
	
	// Create Recorder Utility
	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	if(!iCreateFile)
		{
		// OpenFileL()
		if (iOpenFileType == EFileName)
			{
			INFO_PRINTF1(_L("Opening with TMMFileSource"));
			TMMFileSource filesource(filename);
			recUtil->OpenFileL(filesource);	
			}
		else
			{
			// Create RFs and RFile Objects
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			User::LeaveIfError(fs.ShareProtected());
			
			RFile file;
			User::LeaveIfError(file.Replace(fs, filename, EFileWrite));
			CleanupClosePushL(file);
			
			INFO_PRINTF1(_L("Opening with TMMFileHandleSource"));
			TMMFileHandleSource filehandlesource(file);
			recUtil->OpenFileL(filehandlesource);	
			CleanupStack::PopAndDestroy(2, &fs);	// fs, file.
			}
		}
	else
		{
		RFs fs;
		fs.Connect();
		fs.Delete(filename);
		fs.Close();
		TMdaFileClipLocation location(filename);
		recUtil->OpenL(&location,iFormat,iCodec,&iAudioSettings);
		}

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		recUtil->RecordL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open -> record

		User::After(KFiveSeconds);
		recUtil->Stop();
		}

	// Destroy the Recorder
	CleanupStack::PopAndDestroy(recUtil);
	recUtil = NULL;

	// Playback the file
	if (iError == KErrNone)
		{
		if (filename.Right(4).Compare(_L(".raw"))==0)
			{
			// < Raw file playback >
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
			// < Wav file playback >
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

	if( iError == KErrNone )
		{
		RFs fs;
		RFile file;
		TInt size = 0;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		User::LeaveIfError(file.Open(fs,filename,EFileRead));
		CleanupClosePushL(file);
		User::LeaveIfError(file.Size(size));

		if(size > 0)
			{
			ret = EPass;
			}
		CleanupStack::PopAndDestroy(2); //file
		}

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	User::After(KOneSecond);
	return ret;
	}


/**
 * Constructor
 */
CTestMmfAclntRecPlayFileSource::CTestMmfAclntRecPlayFileSource(const TDesC& aTestName, 
                                                               const TDesC& aSectName, 
                                                               const TDesC& aKeyName, 
                                                               const TTestFormat aFormat, 
                                                               TOpenFileType aOpenFileType)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iOpenFileType = aOpenFileType;
	}

CTestMmfAclntRecPlayFileSource* CTestMmfAclntRecPlayFileSource::NewL(const TDesC& aTestName, 
                                                                     const TDesC& aSectName, 
                                                                     const TDesC& aKeyName, 
                                                                     const TTestFormat aFormat, 
                                                                     TOpenFileType aOpenFileType)
	{
	CTestMmfAclntRecPlayFileSource* self = new (ELeave) CTestMmfAclntRecPlayFileSource(aTestName, aSectName, aKeyName, aFormat, aOpenFileType);
	return self;
	}

void CTestMmfAclntRecPlayFileSource::MoscoStateChangeEvent(CBase* /*aObject*/, 
                                                           TInt aPreviousState, 
                                                           TInt aCurrentState, 
                                                           TInt aErrorCode)
	{
	iError = aErrorCode;
	iCurrentState = aCurrentState;
	INFO_PRINTF4(_L("MMdaObjectStateChangeObserver:: Previous State : %d, Current State : %d, Error : %d"),aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

/**
 * DoTestStepPreambleL
 */
TVerdict CTestMmfAclntRecPlayFileSource::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;
	
	SetupFormatL(iTestFormat);
	
	// Make sure file doesn't exist (maybe from a previous test)
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
	GetDriveName(filename);
	filename.Append(filename1);

	// parse the filenames
	TParse fullFileName;
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

	if((ret == EInconclusive) || (ret == EFail))
		{
		return ret;
		}
		
	// Call Base - DoTestStepPreambleL()
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * DoTestStepPostambleL
 */
TVerdict CTestMmfAclntRecPlayFileSource::DoTestStepPostambleL()
	{
	// 
	// Call Base - DoTestStepPostambleL()
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Open a file based clip and record, stop & then play
 * specifying a different sample rate / number of channels
 */
TVerdict CTestMmfAclntRecPlayFileSource::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File & play with diff sample rate/num of channels"));
	TVerdict ret = EPass;
	iError = KErrGeneral;	// set error to ensure callback takes place

	// Get the file name from ini file.
	TBuf<KSizeBuf> filename;
	TPtrC filename1;
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
	GetDriveName(filename);
	filename.Append(filename1);
	
	// Initialise CMdaAudioRecorderUtility
	CMdaAudioRecorderUtility* recUtil = NULL;
	
	// Create the Output file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());
	TInt er = fs.Delete(filename);
	RFile file;
	
	User::LeaveIfError( file.Replace( fs, filename, EFileWrite ) );
	file.Close();
	
	// Loop for record and play
	for (TInt index=0; index<2; index++)
		{

		// Create CMdaAudioRecorderUtility
		recUtil = CMdaAudioRecorderUtility::NewL(*this);
		CleanupStack::PushL(recUtil);
		
		if (index == 0)
			{
			INFO_PRINTF1(_L("Recording to file..."));
			}
		else
			{
			INFO_PRINTF1(_L("Playing recorded file"));
			}
		
		// OpenFileL()
		if (iOpenFileType == EFileName)
			{
			INFO_PRINTF1(_L("Opening with TMMFileSource"));
			TMMFileSource filesource(filename);
			recUtil->OpenFileL(filesource);	
			}
		else
			{
			// Create RFile Objects
			
			RFile file;
			User::LeaveIfError( file.Open( fs, filename, EFileWrite ) );
			CleanupClosePushL(file);
			
			INFO_PRINTF1(_L("Opening with TMMFileHandleSource"));
			TMMFileHandleSource filehandlesource(file);
			recUtil->OpenFileL(filehandlesource);	
			CleanupStack::PopAndDestroy(&file);
			}
		
		// Callback after OpenFileL()
		INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
		CActiveScheduler::Start();	// idle -> open
		if(iError != KErrNone)
			{
			ERR_PRINTF2(_L("CMdaAudioRecorderUtility->OpenFileL() completed with Error : %d"),iError);	
			ret = EFail;
			}
		else 
			{
			// Initialise the iError variable
			iError = KErrTimedOut;
			
			// Do recording or playback
			if (index == 0)		// record
				{
				recUtil->RecordL();
				INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
				CActiveScheduler::Start(); // open -> record
				// wait for 2 seconds
				User::After(KTwoSeconds);
				// check the current state
				if (iCurrentState != CMdaAudioClipUtility::ERecording)
					{
					INFO_PRINTF1(_L("CMdaAudioRecorderUtility is not recording"));
					ret = EFail;
					}
				else
					{
					recUtil->Stop();
					}
				}
			else				// play
				{
				recUtil->PlayL();
				INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
				CActiveScheduler::Start(); // open -> play
				// check the current state
				if (iCurrentState != CMdaAudioClipUtility::EPlaying)
					{
					INFO_PRINTF1(_L("CMdaAudioRecorderUtility is not playing"));
					ret = EFail;
					}
				else
					{
					CActiveScheduler::Start(); // play -> open
					}
				}

			}
		recUtil->Close();
		CleanupStack::PopAndDestroy(recUtil);
		}
		
	CleanupStack::PopAndDestroy(&fs);
	
	// Final Checkup
	if (iError != KErrNone)
		{
		ERR_PRINTF2(_L("CMdaAudioRecorderUtility completed with Error : %d"),iError);
		ret = EFail;
		}
	
	
	User::After(KOneSecond);
	return	ret;
	}

