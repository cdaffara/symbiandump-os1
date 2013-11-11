// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed the test of the MMF_ACLNT.
// 
//

/**
 @file OpenFileByHandle_PlayTone.cpp
*/

#include "OpenFileByHandle_PlayTone.h"

//const TInt KHeapSizeToneTestEKA2 = 128000; // Heapsize for tone tests on EKA2

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntOpenToneFile::CTestMmfAclntOpenToneFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CTestMmfAclntTone(aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iSectName = aSectName;
	iKeyName= aKeyName;
	}

CTestMmfAclntOpenToneFile* CTestMmfAclntOpenToneFile::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntOpenToneFile* self = new (ELeave) CTestMmfAclntOpenToneFile(aTestName,aSectName,aKeyName);
	return self;
	}

TVerdict CTestMmfAclntOpenToneFile::DoTestStepPreambleL()
	{
	TPtrC filename;
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}

	// Create a sequence file
	TInt length;
	RFs fs;

	fs.Connect();
	CleanupClosePushL(fs);
#ifdef __IPC_V2_PRESENT__
	User::LeaveIfError(fs.ShareAuto());
#else
	User::LeaveIfError(fs.Share(RSessionBase::EExplicitAttach));
#endif
	RFile file;
	User::LeaveIfError(file.Replace(fs,filename,EFileWrite));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Write(KFixedSequenceData()));
	User::LeaveIfError(file.Size(length));
	CleanupStack::PopAndDestroy(2, &fs);

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Play a tone file
 */
TVerdict CTestMmfAclntOpenToneFile::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestTone : Play File"));

	TVerdict ret = EFail;

	RFs fs;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());

	TPtrC filename;
	if(!GetStringFromConfig(iSectName, iKeyName, filename))
		{
		return EInconclusive;
		}

	iError = KErrTimedOut;
	// perform test using this file
	CMdaAudioToneUtility* toneUtil = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(toneUtil);

	RFile file;
	User::LeaveIfError( file.Open( fs, filename, EFileRead | EFileShareAny ) );
	CleanupClosePushL(file);

	toneUtil->PrepareToPlayFileSequence(file);
	CleanupStack::PopAndDestroy(&file);

	// Wait for prepare
	INFO_PRINTF1( _L("Initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	User::LeaveIfError( file.Open( fs, filename, EFileRead | EFileShareAny ) );
	CleanupClosePushL(file);

	toneUtil->PrepareToPlayFileSequence(file);
	// Wait for prepare
	INFO_PRINTF1( _L("re-initialise CMdaAudioToneUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		ret = DoTestL(toneUtil);
		}

	if(ret == EFail)
		{
		ERR_PRINTF2( _L("CMdaAudioToneUtility failed with error %d"),iError );
		}

	CleanupStack::PopAndDestroy(3, &fs);

	return ret;
	}
