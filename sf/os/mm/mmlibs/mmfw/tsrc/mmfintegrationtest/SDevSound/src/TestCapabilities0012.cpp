
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
// TestCapabilities.CPP
// This program is designed the test of the MMF_ACLNT.
// 
//

/**
 @file TestPlayerUtils.cpp
*/

#include <simulprocclient.h>
#include "TestPlayerCaps.h"
#include "TestPlayerCaps0012.h"



/**
 * Constructor
 */
CTestMmfAclntCaps0012::CTestMmfAclntCaps0012(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfAclntCaps0012* CTestMmfAclntCaps0012::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntCaps0012* self = new (ELeave) CTestMmfAclntCaps0012(aTestName, aSectName, aKeyName);
	return self;
	}

CTestMmfAclntCaps0012* CTestMmfAclntCaps0012::NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntCaps0012* self = NewL(aTestName, aSectName, aKeyName);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntCaps0012::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	DoProcess(aError);
	}

void CTestMmfAclntCaps0012::MapcPlayComplete(TInt aError)
	{
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	DoProcess(aError);
	}



/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntCaps0012::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Caps"));
	iVerdict = EFail;

	iServerMonitor = CServerMonitor::NewL(this);

	// Start Server to play back from another process
	RTestServ server;
	User::LeaveIfError(server.Connect(_L("CapTestServer")));
	CleanupClosePushL(server);
	
	RTestSession session1;
	User::LeaveIfError(session1.Open(server,_L("CapTestStep0012")));
	TRequestStatus* status = &iServerMonitor->ActiveStatus();
	// Start Server playback. The RunL of the CServerMonitor class will be called by the server when
	// playback has started
	session1.StartProcessing(*status);
	// Begin activescheduler loop. This will only exit when the whole test is complete 
	CActiveScheduler::Start();
	
	// The test is complete. Now shut down the server and get any errors /messages from the server
	TBuf8<256> message;
	TVerdict verdict = session1.EndProcessingAndReturnResult(message);
	if (verdict != EPass)
		iVerdict = verdict;
	TBuf16<256> copymess;
	copymess.Copy(message);
	INFO_PRINTF2(_L("end processing and return result: %S"),&copymess);

	CleanupStack::PopAndDestroy(&server);
	return iVerdict;
	}
	
void CTestMmfAclntCaps0012::CreatePlayer() 
	{
	// Get the test filename from the configuration file
	TBuf<KSizeBuf> filename;
	TPtrC filename1; 
	if(!GetStringFromConfig(iSectName,iKeyName,filename1))
		{
		iVerdict = EFail;
		CActiveScheduler::Stop();
		}
	GetDriveName(filename);
	filename.Append(filename1);

	iPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this, EMdaPriorityNormal);	
	}
	
void CTestMmfAclntCaps0012::BeginPlayback()
	{
	iPlayer->Play();
	}
	
	
void CTestMmfAclntCaps0012::DoProcess(TInt aError)
	{
	if (aError == KErrNone)
		{
		InternalState nextState = iState;
		switch (iState)
			{
		case EWaitingForServer:
			CreatePlayer();
			nextState = EInitPlayer;
			break;
		case EInitPlayer : 
			BeginPlayback();
			nextState = EPlaying;
			break;
		case EPlaying :
			iVerdict = EPass;
			CActiveScheduler::Stop();
			break;
			}
		iState = nextState;
		}
	else 
		{
		iVerdict = EFail;
		INFO_PRINTF2(_L("Unexpected failure in test, error code %d"), aError);
		CActiveScheduler::Stop();
		}
	}

CTestMmfAclntCaps0012::CServerMonitor::CServerMonitor(CTestMmfAclntCaps0012* aParent)
	:CActive(EPriorityNormal), iParent(aParent)
	{
	}

	
CTestMmfAclntCaps0012::CServerMonitor* CTestMmfAclntCaps0012::CServerMonitor::NewL(CTestMmfAclntCaps0012* aParent) 
	{
	CServerMonitor* self = new (ELeave) CServerMonitor(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestMmfAclntCaps0012::CServerMonitor::ConstructL() 
	{
	CActiveScheduler::Add(this);
	}

void CTestMmfAclntCaps0012::CServerMonitor::DoCancel() 
	{
	Cancel();
	}

void CTestMmfAclntCaps0012::CServerMonitor::RunL() 
	{
	// start the client state machine
	iParent->DoProcess(iStatus.Int());
	}
	
TRequestStatus& CTestMmfAclntCaps0012::CServerMonitor::ActiveStatus() 
	{
	SetActive();
	return iStatus;
	}


CTestMmfAclntCaps0012::~CTestMmfAclntCaps0012()
	{
	delete iPlayer;
	delete iServerMonitor;
	}
