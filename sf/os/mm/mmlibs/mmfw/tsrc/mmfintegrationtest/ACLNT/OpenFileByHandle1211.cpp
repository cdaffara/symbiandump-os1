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

#include "TestPlayerUtils.h"
#include "OpenFileByHandle1211.h"

#include <caf/caf.h>

/**
 * Constructor
 */
CTestMmfAclntOpenFile1211::CTestMmfAclntOpenFile1211(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)

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

CTestMmfAclntOpenFile1211* CTestMmfAclntOpenFile1211::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile1211* self = new (ELeave) CTestMmfAclntOpenFile1211(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	return self;
	}

CTestMmfAclntOpenFile1211* CTestMmfAclntOpenFile1211::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile1211* self = CTestMmfAclntOpenFile1211::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntOpenFile1211::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntOpenFile1211 : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);

	CActiveScheduler::Stop();
	}

// Audio utility callbacks
void CTestMmfAclntOpenFile1211::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile1211::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

// Audio output stream callbacks
void CTestMmfAclntOpenFile1211::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile1211::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile1211::MaoscPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntOpenFile1211::DoTestStepPreambleL()
	{
	SetupFormatL(iTestFormat);

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Open a file based clip and record
 */
TVerdict CTestMmfAclntOpenFile1211::DoTestStepL()
	{
	return( PerformTestStepL() );
	}

/*
 * TVerdict CTestMmfAclntOpenFile1211::PerformTestStepL()
 *
 */
TVerdict CTestMmfAclntOpenFile1211::PerformTestStepL()
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

	User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
	CleanupClosePushL(file);

	recUtil->OpenFileL(file);

	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

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
	else
		{
		if( iError == KErrNotSupported )
			{
			ret = EPass;
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

	CleanupStack::PopAndDestroy(); // fs

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	User::After(KOneSecond);

	return	ret;
	}
