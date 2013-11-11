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
// REQ1952
// 
//

#include <caf/caf.h>
#include <e32math.h>


#include "DRMTestVidPlayer.h"
#include "rightsdatabase.h"

using namespace ContentAccess;

const TInt KFrameWidth = 100;
const TInt KFrameHeight = 50;
const TInt KDefaultRightsID = 1;
const TInt KDefaultRightsCount = 2;
const TInt KZeroRightsCount = 0;

TAny	GetDriveName(TDes& aFileName)
{
#ifdef __WINS__
	aFileName = _L("c:");	
#elif defined(__MARM__) || defined(__X86GCC__)
	#ifdef __TEST_FILES_ON_C_DRIVE__
		aFileName = _L("c:");
	#else
		aFileName = _L("z:");	
	#endif // __TEST_FILES_ON_C_DRIVE__
#endif
}

/**
 *
 * Static constructor for CTestStepVidPlayerDRMGetFrame.
 *
 *
 * @return	"CTestStepVidPlayerDRMGetFrame*"
 *			The constructed CTestStepVidPlayerDRMGetFrame
 *
 * @xxxx
 * 
 */

CTestStepVidPlayerDRMGetFrame::CTestStepVidPlayerDRMGetFrame(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestStepVidPlayerDRMGetFrame* CTestStepVidPlayerDRMGetFrame::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestStepVidPlayerDRMGetFrame* self = new (ELeave) CTestStepVidPlayerDRMGetFrame(aTestName,aSectName,aKeyName);
	return self;
	}


void CTestStepVidPlayerDRMGetFrame::MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError)
	{
	iError = aError;
	if (aError==KErrNone)
		{
		if (aFrame.Handle() == NULL || aFrame.SizeInPixels() != TSize(KFrameWidth,KFrameHeight) )
			iError = KErrArgument;
		else
			{
				iError = KErrNone;
				INFO_PRINTF1(_L("MvpuoFrameReady: GetFrameL() COMPLETED OK"));
			}
		}
	CActiveScheduler::Stop();
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestStepVidPlayerDRMGetFrame::DoTestStepL()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	INFO_PRINTF1(_L("Test : Video Player DRM GetFrame"));
	
	
	TPtrC			filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;
	
	InitWservL();

	//setup DRM rights
	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	delete rights; // the database will now be reset and have no entries in it

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);
	
	TRAP(iError,player->OpenFileL(filename));
	
	if(iError == KErrNone)
		{
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening file"));
		CActiveScheduler::Start();
		}
	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestStepVidPlayerDRMGetFrame::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

		iError = KErrTimedOut;
		aPlayer->GetFrameL(EColor16M, EView);
		INFO_PRINTF1(_L("CVideoPlayerUtility: Getting Frame from file"));
		// Wait for init callback
		CActiveScheduler::Start();
		if(iError == KErrNone)
			ret = EPass;
		else
			ret = EFail;


	return ret;
	}

//
/**
 *
 * Static constructor for CTestStepVidPlayerDRMGetFrameProtected.
 *
 *
 * @return	"CTestStepVidPlayerDRMGetFrameProtected*"
 *			The constructed CTestStepVidPlayerDRMGetFrameProtected
 *
 * @xxxx
 * 
 */

CTestStepVidPlayerDRMGetFrameProtected::CTestStepVidPlayerDRMGetFrameProtected(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestStepVidPlayerDRMGetFrameProtected* CTestStepVidPlayerDRMGetFrameProtected::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestStepVidPlayerDRMGetFrameProtected* self = new (ELeave) CTestStepVidPlayerDRMGetFrameProtected(aTestName,aSectName,aKeyName);
	return self;
	}


void CTestStepVidPlayerDRMGetFrameProtected::MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError)
	{
	iError = aError;
	if (iError == KErrNone)
		{
		if (aFrame.Handle() == NULL || aFrame.SizeInPixels() != TSize(KFrameWidth,KFrameHeight) )	
			iError = KErrArgument;
		else
			{
				iError = KErrNone;
				INFO_PRINTF1(_L("MvpuoFrameReady: GetFrameL() COMPLETED OK"));
			}
		}
	CActiveScheduler::Stop();
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestStepVidPlayerDRMGetFrameProtected::DoTestStepL()
	{
TVerdict ret = EFail;
	iError = KErrTimedOut;

	INFO_PRINTF1(_L("Test : Video Player DRM GetFrame"));
	
	
	TPtrC			filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;
	
	InitWservL();

	//setup DRM rights
	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	CleanupStack::PushL(rights);
	rights->AddRightsL(KDefaultRightsID,KZeroRightsCount); // add rights count 
	CleanupStack::PopAndDestroy(rights); // cleanup rights object


	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);
	TMMFileSource fs(filename, KDefaultContentObject, EPeek);
	TRAP(iError,player->OpenFileL(fs));
	
	if(iError == KErrNone)
		{
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening file"));
		CActiveScheduler::Start();
		}
	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestStepVidPlayerDRMGetFrameProtected::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;

		iError = KErrTimedOut;
		//set rights count to zero
		aPlayer->GetFrameL(EColor16M, EView);
		INFO_PRINTF1(_L("CVideoPlayerUtility: Getting Frame from file"));
		// Wait for init callback
		CActiveScheduler::Start();
		if(iError == KErrCANoPermission)
			ret = EPass;
		else
			ret = EFail;

	return ret;
	}

//
/**
 *
 * Static constructor for CTestStepVidPlayerDRMPlayNoRights.
 *
 *
 * @return	"CTestStepVidPlayerDRMPlayNoRights*"
 *			The constructed CTestStepVidPlayerDRMPlayNoRights
 *
 * @xxxx
 * 
 */

CTestStepVidPlayerDRMPlayNoRights::CTestStepVidPlayerDRMPlayNoRights(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestStepVidPlayerDRMPlayNoRights* CTestStepVidPlayerDRMPlayNoRights::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestStepVidPlayerDRMPlayNoRights* self = new (ELeave) CTestStepVidPlayerDRMPlayNoRights(aTestName,aSectName,aKeyName);
	return self;
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestStepVidPlayerDRMPlayNoRights::DoTestStepL()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	INFO_PRINTF1(_L("Test : Video Player - Open File with no rights"));

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	
	InitWservL();

	//setup DRM rights
	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	delete rights; // the database will now be reset and have no entries in it

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);
	TRAP(iError,player->OpenFileL(filename));
	if (iError != KErrNone)
		{
		ERR_PRINTF2( _L("OpenFileL() left with %d, expecting"),iError);
		CleanupStack::PopAndDestroy(player);
		return	ret;
		}
	CActiveScheduler::Start();
	
	if(iError == KErrCANoRights)
		{
		ret = EPass;
		INFO_PRINTF1(_L("Expected failure with KErrCANoRights"));
		}
	else
		ERR_PRINTF3( _L("CVideoPlayerUtility received error %d, expecting"),iError, KErrCANoRights );
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));

	CleanupStack::PopAndDestroy(player);
	return	ret;
	}

TVerdict CTestStepVidPlayerDRMPlayNoRights::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;
	
	iError = KErrTimedOut;
	aPlayer->Play();
	INFO_PRINTF1(_L("CVideoPlayerUtility: Playing file"));
	// Wait for init callback
	CActiveScheduler::Start();
	if(iError == KErrNone)
		ret = EPass;
	else
		ret = EFail;
	
	return ret;
	}

//
/**
 *
 * Static constructor for CTestStepVidPlayerDRMPlayRights.
 *
 *
 * @return	"CTestStepVidPlayerDRMPlayRights*"
 *			The constructed CTestStepVidPlayerDRMPlayRights
 *
 * @xxxx
 * 
 */

CTestStepVidPlayerDRMPlayRights::CTestStepVidPlayerDRMPlayRights(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestVclntVideoFile(aTestName,aSectName,aKeyName,EFalse)
	{}

CTestStepVidPlayerDRMPlayRights* CTestStepVidPlayerDRMPlayRights::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestStepVidPlayerDRMPlayRights* self = new (ELeave) CTestStepVidPlayerDRMPlayRights(aTestName,aSectName,aKeyName);
	return self;
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestStepVidPlayerDRMPlayRights::DoTestStepL()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;

	INFO_PRINTF1(_L("Test : Video Player - File"));

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		return EInconclusive;

	
	InitWservL();

	//setup DRM rights
	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	CleanupStack::PushL(rights);
	rights->AddRightsL(KDefaultRightsID,KDefaultRightsCount); // add rights count 
	CleanupStack::PopAndDestroy(rights); // cleanup rights object

	TRect rect, clipRect;
	CVideoPlayerUtility* player = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
															EMdaPriorityPreferenceTimeAndQuality,
															iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(player);
	TRAP(iError,player->OpenFileL(filename));
	
	if(iError == KErrNone)
		{
		// Wait for initialisation callback
		INFO_PRINTF1(_L("CVideoPlayerUtility: Opening file"));
		CActiveScheduler::Start();
		}
	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		player->Prepare();
		CActiveScheduler::Start();
		}

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CVideoPlayerUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CVideoPlayerUtility failed with error %d"),iError );
	return	ret;
	}

TVerdict CTestStepVidPlayerDRMPlayRights::DoTestL(CVideoPlayerUtility* aPlayer)
	{
	TVerdict ret = EFail;
	
	iError = KErrTimedOut;
	aPlayer->Play();
	INFO_PRINTF1(_L("CVideoPlayerUtility: Playing file"));
	// Wait for init callback
	CActiveScheduler::Start();
	if(iError == KErrNone)
		ret = EPass;
	else
		ret = EFail;
	
	return ret;
	}
