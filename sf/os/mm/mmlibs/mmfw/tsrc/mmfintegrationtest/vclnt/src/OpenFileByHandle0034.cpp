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

#include "TestPlayer.h"
#include "OpenFileByHandle0034.h"

#include <caf/caf.h>

/**
 * Constructor
 */
CTestMmfVclntOpenFile0034::CTestMmfVclntOpenFile0034(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
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

CTestMmfVclntOpenFile0034* CTestMmfVclntOpenFile0034::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfVclntOpenFile0034* self = new (ELeave) CTestMmfVclntOpenFile0034(aTestName,aSectName,aKeyName,aPlay);
	return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
	}

CTestMmfVclntOpenFile0034* CTestMmfVclntOpenFile0034::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfVclntOpenFile0034* self = CTestMmfVclntOpenFile0034::NewLC(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfVclntOpenFile0034::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open Complete callback"));
	CActiveScheduler::Stop();
	}

void CTestMmfVclntOpenFile0034::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Prepare Complete callback"));
	CActiveScheduler::Stop();
	}

void CTestMmfVclntOpenFile0034::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Frame Ready callback"));
	}

void CTestMmfVclntOpenFile0034::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Play Complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestMmfVclntOpenFile0034::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 * Load and initialise an audio file.
 */
TVerdict CTestMmfVclntOpenFile0034::DoTestStepL()
	{
	InitWservL();

	return( PerformTestStepL() );
	}

TVerdict CTestMmfVclntOpenFile0034::PerformTestStepL()
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

	User::After(1000000);
	player->Stop();
	player->Close();
		
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
		}

	CleanupStack::PopAndDestroy(2, &fs);
	return	ret;
	}
