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
#include "OpenFileByHandle7902.h"

#include <caf/caf.h>

/**
 * Constructor
 */
CTestMmfAclntOpenFile7902::CTestMmfAclntOpenFile7902(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)

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

CTestMmfAclntOpenFile7902* CTestMmfAclntOpenFile7902::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile7902* self = new (ELeave) CTestMmfAclntOpenFile7902(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	return self;
	}

CTestMmfAclntOpenFile7902* CTestMmfAclntOpenFile7902::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile7902* self = CTestMmfAclntOpenFile7902::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntOpenFile7902::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF2( _L("iError %d "), iError);
	CActiveScheduler::Stop();
	}

// Audio utility callbacks
void CTestMmfAclntOpenFile7902::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile7902::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

// Audio output stream callbacks
void CTestMmfAclntOpenFile7902::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile7902::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestMmfAclntOpenFile7902::MaoscPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntOpenFile7902::DoTestStepPreambleL()
	{
	SetupFormatL(iTestFormat);

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Open a file based clip and record
 */
TVerdict CTestMmfAclntOpenFile7902::DoTestStepL()
	{
	return( PerformTestStepL() );
	}

/*
 * TVerdict CTestMmfAclntOpenFile7902::PerformTestStepL()
 *
 */
TVerdict CTestMmfAclntOpenFile7902::PerformTestStepL()
	{
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
		ret = EInconclusive;
		}

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recUtil);

	User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
	CleanupClosePushL(file);
	
	recUtil->OpenFileL(file);

	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		recUtil->RecordL();
		CActiveScheduler::Start(); // open -> record

		User::After(500000);	// 0.5 sec
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

		if( iError == KErrNone )
			{
			playUtil->Play();
			CActiveScheduler::Start();
			}
		CleanupStack::PopAndDestroy(playUtil);
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Error during playback of recorded file iError=%d"), iError);
			ret = EFail;
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

	CleanupStack::PopAndDestroy(&fs);	// fs

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	User::After(500000);

	return	ret;
	}
