
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
#include "TestPlayerCaps0014.h"



/**
 * Constructor
 */
CTestMmfAclntCaps0014::CTestMmfAclntCaps0014(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfAclntCaps0014* CTestMmfAclntCaps0014::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntCaps0014* self = new (ELeave) CTestMmfAclntCaps0014(aTestName, aSectName, aKeyName);
	return self;
	}

CTestMmfAclntCaps0014* CTestMmfAclntCaps0014::NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntCaps0014* self = NewL(aTestName, aSectName, aKeyName);
	CleanupStack::PushL(self);
	return self;
	}

//void CTestMmfAclntCaps0014::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
//	{
//	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
//	DoProcess(aError);
//	}

//void CTestMmfAclntCaps0014::MapcPlayComplete(TInt aError)
//	{
//	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
//	DoProcess(aError);
//	}

void CTestMmfAclntCaps0014::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	if (aErrorCode == KErrNone)
		{
		if (aPreviousState == CMdaAudioClipUtility::ENotReady && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			iPlayer->RecordL();
			}
		else if (aPreviousState == CMdaAudioClipUtility::EOpen && aCurrentState==CMdaAudioClipUtility::ERecording)
			{
			User::After( 1000000 );		// record for this long
			iPlayer->Stop();
			iVerdict = EPass;
			CActiveScheduler::Stop();
			}
		else if (aPreviousState == CMdaAudioClipUtility::ERecording && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			// this shouldn't happen as record will continue until interrupted
			iVerdict = EPass;
			CActiveScheduler::Stop();
			}
		}
	else if (aErrorCode == KErrInUse)
		{
		// double check state to ensure behaviour is correct
		if (aPreviousState == CMdaAudioClipUtility::ERecording && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			iVerdict = EPass;
			CActiveScheduler::Stop();
			}
		}

	else 
		{
		iVerdict = EFail;
		CActiveScheduler::Stop();
		}

	}

/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntCaps0014::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Caps"));
	iVerdict = EFail;

	iServerMonitor = CServerMonitor::NewL(this);

	// Start Server to play back from another process
	RTestServ server;
	User::LeaveIfError(server.Connect(_L("CapTestServer")));
	CleanupClosePushL(server);
	
	RTestSession session1;
	User::LeaveIfError(session1.Open(server,_L("CapTestStep0014")));
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
	
void CTestMmfAclntCaps0014::CreatePlayer() 
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

	iPlayer = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityNormal);
	iPlayer->OpenFileL(_L("c:\\rectest2.wav"));
	}
	
void CTestMmfAclntCaps0014::BeginPlayback()
	{
	iPlayer->RecordL();
	}
	
	
void CTestMmfAclntCaps0014::DoProcess(TInt aError)
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

CTestMmfAclntCaps0014::CServerMonitor::CServerMonitor(CTestMmfAclntCaps0014* aParent)
	:CActive(EPriorityNormal), iParent(aParent)
	{
	}

	
CTestMmfAclntCaps0014::CServerMonitor* CTestMmfAclntCaps0014::CServerMonitor::NewL(CTestMmfAclntCaps0014* aParent) 
	{
	CServerMonitor* self = new (ELeave) CServerMonitor(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestMmfAclntCaps0014::CServerMonitor::ConstructL() 
	{
	CActiveScheduler::Add(this);
	}

void CTestMmfAclntCaps0014::CServerMonitor::DoCancel() 
	{
	Cancel();
	}

void CTestMmfAclntCaps0014::CServerMonitor::RunL() 
	{
	// start the client state machine
	iParent->DoProcess(iStatus.Int());
	}
	
TRequestStatus& CTestMmfAclntCaps0014::CServerMonitor::ActiveStatus() 
	{
	SetActive();
	return iStatus;
	}


CTestMmfAclntCaps0014::~CTestMmfAclntCaps0014()
	{
	delete iPlayer;
	delete iServerMonitor;
	}
