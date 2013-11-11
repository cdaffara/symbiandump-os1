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

#include "TestRecord.h"
#include "OpenFileByHandle0522.h"

#include <caf/caf.h>

/**
 * Constructor
 */
CTestMmfVclntOpenFile0522::CTestMmfVclntOpenFile0522(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	:iRec (aRec)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfVclntOpenFile0522* CTestMmfVclntOpenFile0522::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestMmfVclntOpenFile0522* self = new (ELeave) CTestMmfVclntOpenFile0522(aTestName,aSectName,aKeyName,aRec);
	return self;
	}

CTestMmfVclntOpenFile0522* CTestMmfVclntOpenFile0522::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aRec)
	{
	CTestMmfVclntOpenFile0522* self = CTestMmfVclntOpenFile0522::NewL(aTestName,aSectName,aKeyName,aRec);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfVclntOpenFile0522::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Open complete callback"));
	CActiveScheduler::Stop();
	}

void CTestMmfVclntOpenFile0522::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Record complete callback"));
	CActiveScheduler::Stop();
	}

void CTestMmfVclntOpenFile0522::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("Prepare complete callback"));
	CActiveScheduler::Stop();
	}
	
void CTestMmfVclntOpenFile0522::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestMmfVclntOpenFile0522::DoTestStepL()
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("Test : Video Recorder - OpenFileL(RFile&)"));

	RFs		fs;
	RFile	file;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());

	iError = KErrTimedOut;

	TPtrC filename;
	if(!GetStringFromConfig(iSectName,iKeyName,filename))
		{
		return EInconclusive;
		}

	User::LeaveIfError(file.Open(fs,filename,EFileWrite));
	CleanupClosePushL(file);
	CVideoRecorderUtility* rec = CVideoRecorderUtility::NewL(*this);
	CleanupStack::PushL(rec);

	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid};
	TRAP(iError,rec->OpenFileL(file, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat));

	if(iError == KErrNone)
		{
		INFO_PRINTF1(_L("CVideoRecorderUtility: Open file"));
		// Wait for initialisation callback
		CActiveScheduler::Start();
		}

	if(iError == KErrNotSupported)
		{
		ret = EPass;
		}

	// Check for errors.
	if ((iError == KErrNone) && (rec != NULL))
		{
		rec->Prepare();
		CActiveScheduler::Start();
		if(iError != KErrNone)
			{
			INFO_PRINTF2(_L("Prepare callback : error %d"), iError);
			return EInconclusive;
			}

		if(iRec)
			{
			iError = KErrTimedOut;
			rec->Record();
			INFO_PRINTF1(_L("CVideoRecorderUtility: Record"));
			// Wait for init callback
			CActiveScheduler::Start();
			if(iError == KErrNone)
				{
				ret = EPass;
				}
			User::After(1000000);
			rec->Stop();
			}
		else
			{
			ret = EPass;
			}
		}

	rec->Close();
	
	INFO_PRINTF1(_L("CVideoRecorderUtility: Destroy"));
	CleanupStack::PopAndDestroy(rec);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CVideoRecorderUtility failed with error %d"),iError );
		}

	CleanupStack::PopAndDestroy(2,&fs);
	
	return	ret;
	}
