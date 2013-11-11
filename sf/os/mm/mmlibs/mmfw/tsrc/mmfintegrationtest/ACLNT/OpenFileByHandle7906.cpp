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
//

#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include "TestPlayerUtils.h"
#include "OpenFileByHandle7906.h"

#include <caf/caf.h>

/**
 * Constructor
 */
CTestMmfAclntOpenFile7906::CTestMmfAclntOpenFile7906(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)

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

CTestMmfAclntOpenFile7906* CTestMmfAclntOpenFile7906::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile7906* self = new (ELeave) CTestMmfAclntOpenFile7906(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	return self;
	}

CTestMmfAclntOpenFile7906* CTestMmfAclntOpenFile7906::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile7906* self = CTestMmfAclntOpenFile7906::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntOpenFile7906::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntOpenFile7906 : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	CActiveScheduler::Stop();
	}

// Audio utility callbacks
void CTestMmfAclntOpenFile7906::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile7906::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

// Audio output stream callbacks
void CTestMmfAclntOpenFile7906::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile7906::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile7906::MaoscPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntOpenFile7906::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	SetupFormatL(iTestFormat);

	if((ret == EInconclusive) || (ret == EFail))
		{
		return ret;
		}
	
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Open a file based clip and record
 */
TVerdict CTestMmfAclntOpenFile7906::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File"));
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	RFs fs;
	RFile file;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());

	TPtrC			filename; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	TUid audioController = TUid::Uid(KMmfUidControllerAudio);

	TFourCC invalidDataType(0xAA, 0xAA, 0xAA, 0xAA);

	User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
	CleanupClosePushL(file);

	recUtil->OpenFileL(file, audioController, KNullUid, KNullUid, invalidDataType);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if( iError == KErrNotSupported )
		{
		ret = EPass;
		}

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		recUtil->RecordL();
		INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
		CActiveScheduler::Start(); // open -> record

		User::After(500000);

		recUtil->Stop();
		}

	CleanupStack::PopAndDestroy(2, recUtil);	// file, recUtil
	recUtil = NULL;

	// Playback the file
	if (iError == KErrNone)
		{
		if (filename.Right(4).Compare(_L(".wav"))==0)
			{
			// Wav file playback
			CMdaAudioPlayerUtility* playUtil = CMdaAudioPlayerUtility::NewL(*this);
			CleanupStack::PushL(playUtil);
			TRAPD(err, playUtil->OpenFileL(filename));
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error opening file for playback err = %d"), err);
				ret = EFail;
				}
			CActiveScheduler::Start();
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Error opening file for playback iError = %d"), iError);
				ret = EFail;
				}
			playUtil->Play();
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(playUtil);
			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Error during playback of recorded file iError=%d"), iError);
				ret = EFail;
				}
			}
		}

	if( iError == KErrNone ) 
		{
		RFile file;
		TInt size = 0;
		User::LeaveIfError(file.Open(fs,filename,EFileRead));
		CleanupClosePushL(file);
		User::LeaveIfError(file.Size(size));

		if(size > 0)
			{
			ret = EPass;
			}
		CleanupStack::PopAndDestroy(); //file
		}

	CleanupStack::PopAndDestroy();	// fs

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	User::After(KOneSecond);

	return	ret;
	}
