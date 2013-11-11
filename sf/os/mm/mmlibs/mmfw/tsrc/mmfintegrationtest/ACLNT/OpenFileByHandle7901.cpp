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
#include "OpenFileByHandle7901.h"

#include <caf/caf.h>

CTestMmfAclntOpenFile7901::CTestMmfAclntOpenFile7901(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	:CTestMmfAclntRecord(aTestName, aSectName, aNegative)
	{
	}

CTestMmfAclntOpenFile7901* CTestMmfAclntOpenFile7901::NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative)
	{
	CTestMmfAclntOpenFile7901* self = new (ELeave) CTestMmfAclntOpenFile7901(aTestName, aSectName, aNegative);
	return self;
	}


/**
 * Open new file.
 */
TVerdict CTestMmfAclntOpenFile7901::DoTestL(CMdaAudioRecorderUtility* aPlayer)
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

	if(!GetStringFromConfig(_L("SectionOne"), _L("PCM16"), filename1))
		{
		return EInconclusive;
		}

	GetDriveName(filename);
	filename.Append(filename1);

	// open file for play
	User::LeaveIfError( file.Open( fs, filename, EFileRead ) );
	CleanupClosePushL(file);

	TInt error = KErrNone;
	TRAP(error,aPlayer->OpenFileL(file));
	CActiveScheduler::Start();
	aPlayer->Close();
	file.Close();

	// open file again for play
	User::LeaveIfError( file.Open( fs, filename, EFileRead ) );

	error = KErrNone;
	TRAP(error,aPlayer->OpenFileL(file));
	CActiveScheduler::Start();
	aPlayer->Close();

	CleanupStack::PopAndDestroy(2, &fs);

	if(error == KErrNone)
		{
		ret = EPass;
		}

	return ret;
	}
