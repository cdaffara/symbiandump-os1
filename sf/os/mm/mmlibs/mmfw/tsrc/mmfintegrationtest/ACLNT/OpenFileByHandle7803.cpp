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
#include "OpenFileByHandle7803.h"

#include <caf/caf.h>

CTestMmfAclntOpenFile7803::CTestMmfAclntOpenFile7803(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName), iKeyName(aKeyName)
	{}

CTestMmfAclntOpenFile7803* CTestMmfAclntOpenFile7803::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntOpenFile7803* self = new (ELeave) CTestMmfAclntOpenFile7803(aTestName, aSectName, aKeyName);
	return self;
	}

/**
 * Open new file.
 */
TVerdict CTestMmfAclntOpenFile7803::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	return( PerformTestL( aPlayer ) );
	}

TVerdict CTestMmfAclntOpenFile7803::PerformTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	TVerdict ret = EPass;
	aPlayer->Close();

	RFs fs;

	// read optional test specific parameters
	iSkipShareProtected = iExpectToFail = EFalse;
	iExpectedError = KErrNone;
	// read the parameters, if any missing just ignore
	GetBoolFromConfig(iDefaultParamSet, _L("SkipShareProtected"), iSkipShareProtected);
	GetBoolFromConfig(iDefaultParamSet, _L("ExpectToFail"), iExpectToFail);
	GetIntFromConfig(iDefaultParamSet, _L("ExpectedError"), iExpectedError);

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	if (!iSkipShareProtected)
		{
		User::LeaveIfError(fs.ShareProtected());
		}

	// get second file name
	TBuf<KSizeBuf>	filename2;
	TPtrC			filename;

	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename2);
	filename2.Append(filename);

	RFile file;
	User::LeaveIfError( file.Open( fs, filename2, EFileRead ) );
	CleanupClosePushL(file);

	// Open second file
	aPlayer->OpenFileL(file);
	// wait for open to complete
	CActiveScheduler::Start();

	INFO_PRINTF2( _L("OpenFileL completed with error %d"), iError);

	if( iError != KErrNone )
		{
		if (iExpectToFail && iError == iExpectedError)
			{
			INFO_PRINTF2( _L("Error matches expected %d"), iExpectedError);
			ret = EPass;
			}
		else if (iExpectToFail)
			{
			INFO_PRINTF2( _L("Error does not match %d"), iExpectedError);
			ret = EFail;
			}
		else
			{
			INFO_PRINTF1( _L("No error was expected"));
			ret = EFail;
			}
		}
	else 
		{
		aPlayer->Play();
		CActiveScheduler::Start();

		if( iError != KErrNone )
			{
			ret = EFail;
			}

		User::After(1000000);
		aPlayer->Stop();
		aPlayer->Close();
		}

	CleanupStack::PopAndDestroy(2, &fs);
	return ret;
	}
