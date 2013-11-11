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
#include "OpenFileByHandle8801.h"

#include <caf/caf.h>

CTestMmfAclntOpenFile8801::CTestMmfAclntOpenFile8801(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntOpenFile8801* CTestMmfAclntOpenFile8801::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntOpenFile8801* self = new (ELeave) CTestMmfAclntOpenFile8801(aTestName, aSectName);
	return self;
	}

/**
 * Open new file.
 */
TVerdict CTestMmfAclntOpenFile8801::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	return( PerformTestL( aPlayer ) );
	}

TVerdict CTestMmfAclntOpenFile8801::PerformTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : OpenFileL(RFile&)/Play"));

	TVerdict ret = EFail;
	iError = KErrTimedOut;

	RFs fs;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	RFile			file;

	if(!GetStringFromConfig(_L("SectionOne"), _L("inputTextFile"), filename1))
		{
		return EInconclusive;
		}
	GetDriveName(filename);
	filename.Append(filename1);

	User::LeaveIfError( file.Open( fs, filename, EFileRead ) );
	CleanupClosePushL(file);

	aPlayer->Close();	// close previous file opened by test framework

	TInt error = KErrNone;
	TRAP(error,aPlayer->OpenFileL(file));
	CActiveScheduler::Start();
	aPlayer->Play();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, &fs);

	// check OpenFile errors
	if(error == KErrNone)
		{
		ret = EPass;
		}

	if( iError != KErrNotReady )
		{
		// unexpected error
		ret = EFail;
		}

	return ret;
	}
