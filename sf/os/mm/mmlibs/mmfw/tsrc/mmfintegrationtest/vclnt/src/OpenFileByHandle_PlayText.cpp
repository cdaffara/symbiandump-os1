// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// OpenFileByHandlePlayTextFile.CPP
// 
//

#include "TestPlayer.h"
#include "OpenFileByHandle_PlayText.h"

#include <caf/caf.h>

/**
 * Constructor
 */
CTestMmfVclntOpenFilePlayTextFile::CTestMmfVclntOpenFilePlayTextFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:iPlay (aPlay)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;

	// expand heap, so we can load 80k video
	iHeapSize = 150000;
	}

CTestMmfVclntOpenFilePlayTextFile* CTestMmfVclntOpenFilePlayTextFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfVclntOpenFilePlayTextFile* self = new (ELeave) CTestMmfVclntOpenFilePlayTextFile(aTestName,aSectName,aKeyName,aPlay);
	return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
	}

CTestMmfVclntOpenFilePlayTextFile* CTestMmfVclntOpenFilePlayTextFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfVclntOpenFilePlayTextFile* self = CTestMmfVclntOpenFilePlayTextFile::NewLC(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfVclntOpenFilePlayTextFile::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open Complete callback"));
	CActiveScheduler::Stop();
	}

void CTestMmfVclntOpenFilePlayTextFile::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open Complete callback"));
	CActiveScheduler::Stop();
	}

void CTestMmfVclntOpenFilePlayTextFile::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Frame Ready callback"));
	}

void CTestMmfVclntOpenFilePlayTextFile::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Play Complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestMmfVclntOpenFilePlayTextFile::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Load and initialise an audio file.
 */
TVerdict CTestMmfVclntOpenFilePlayTextFile::DoTestStepL()
	{
	return( PerformTestStepL() );
	}

TVerdict CTestMmfVclntOpenFilePlayTextFile::PerformTestStepL()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	INFO_PRINTF1(_L("Test : Video Player - OpenFileL(RFile&)"));

	RFs		fs;
	RFile	file;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		{
		return EInconclusive;
		}

	User::LeaveIfError(file.Open(fs,filename,EFileRead));
	CleanupClosePushL(file);

	InitWservL();

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);
	TRAP(iError,player->OpenFileL(file));

	if(iError == KErrNone)
		{
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening file"));
		CActiveScheduler::Start();
		}

	// text file is not supported
	if(iError == KErrNotSupported)
		{
		ret = EPass;
		}

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		if(iPlay)
			{
			iError = KErrTimedOut;
			player->Play();
			INFO_PRINTF1(_L("CVideoPlayerUtility: Playing file"));
			// Wait for init callback
			CActiveScheduler::Start();
			if(iError == KErrNone)
				{
				ret = EPass;
				}
			}
		else
			{
			ret = EPass;
			}
		}

	User::After(500000);
	player->Stop();
	player->Close();
		
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
		}

	CleanupStack::PopAndDestroy(3, &fs);

	return	ret;
	}

