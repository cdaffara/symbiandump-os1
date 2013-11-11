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
#include "OpenFileByHandle7804.h"

#include <caf/caf.h>

CTestMmfAclntOpenFile7804::CTestMmfAclntOpenFile7804(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntOpenFile7804* CTestMmfAclntOpenFile7804::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntOpenFile7804* self = new (ELeave) CTestMmfAclntOpenFile7804(aTestName, aSectName);
	return self;
	}

/**
 * Open new file.
 */
TVerdict CTestMmfAclntOpenFile7804::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	return( PerformTestL( aPlayer ) );
	}

TVerdict CTestMmfAclntOpenFile7804::PerformTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : OpenFileL(RFile&)/Play"));

	TVerdict ret = EFail;
	iError = KErrNone;

	RFs fs;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	RFile			file;

	if(!GetStringFromConfig(_L("SectionOne"), _L("PCMU16BE"), filename1))
		{
		return EInconclusive;
		}

	GetDriveName(filename);
	filename.Append(filename1);

	User::LeaveIfError( file.Open( fs, filename, EFileRead ) );
	CleanupClosePushL(file);

	aPlayer->OpenFileL(file);
	CActiveScheduler::Start();
	
	if (iError == KErrNone)
		{
		aPlayer->Play();
		CActiveScheduler::Start();
		}

	if (iError == KErrNone)
		{
		ret = EPass;
		}
	
	aPlayer->Close();	
	CleanupStack::PopAndDestroy(2, &fs);

	return ret;
	}
