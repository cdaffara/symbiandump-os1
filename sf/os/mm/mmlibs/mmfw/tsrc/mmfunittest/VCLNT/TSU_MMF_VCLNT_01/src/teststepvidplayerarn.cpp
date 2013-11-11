// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidPlayer.h"

/**
 * NewL
 */
CTestGenericArnCallback* CTestGenericArnCallback::NewL(MTestArnCallback& aCallback)
	{
	return new(ELeave) CTestGenericArnCallback(aCallback);
	}

/**
 * MarncResourceAvailable
 */
void CTestGenericArnCallback::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	iCallback.MarncResourceAvailableTest(aNotificationEventId, aNotificationData, *this);
	}

/**
 * Constructor
 */
CTestGenericArnCallback::CTestGenericArnCallback(MTestArnCallback& aCallback):iCallback(aCallback)
	{

	}

/**
 * NewL
 */
CTestGenericAosCallback* CTestGenericAosCallback::NewL(MTestAosCallback& aCallback)
	{
	return new(ELeave) CTestGenericAosCallback(aCallback);
	}

/**
 * MaoscOpenComplete
 */
void CTestGenericAosCallback::MaoscOpenComplete(TInt aError)
	{
	iCallback.MaoscOpenCompleteTest(aError, *this);
	}

/**
 * MaoscBufferCopied
 */
void CTestGenericAosCallback::MaoscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{
	iCallback.MaoscBufferCopiedTest(aError, aBuffer, *this);	
	}

/**
 * MaoscPlayComplete
 */
void CTestGenericAosCallback::MaoscPlayComplete(TInt aError)
	{
	iCallback.MaoscPlayCompleteTest(aError, *this);	
	}

/**
 * Constructor
 */
CTestGenericAosCallback::CTestGenericAosCallback(MTestAosCallback& aCallback):iCallback(aCallback)
	{		
	}


/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * 
 */
CTestStepVidPlayerARN::CTestStepVidPlayerARN():iVideoOpen(EFalse)
	{
	}


/**
 *
 * Test step destructor.
 *
 * 
 */
CTestStepVidPlayerARN::~CTestStepVidPlayerARN()
	{
	}

TVerdict CTestStepVidPlayerARN::DoTestStepPreambleL()
	{
	// this installs the scheduler
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	
	//create the handlers
	iAudStreamACallback = CTestGenericAosCallback::NewL(*this);
	iAudStreamBCallback = CTestGenericAosCallback::NewL(*this);
	
	//create audio stream A - low priority
	iAudStreamA = CMdaAudioOutputStream::NewL(*iAudStreamACallback, EMdaPriorityMin, EMdaPriorityPreferenceNone);
	
	//create audio stream B - high priority
	iAudStreamB = CMdaAudioOutputStream::NewL(*iAudStreamBCallback, EMdaPriorityMax, EMdaPriorityPreferenceNone);
	const TInt KAudioBufferSize = 10240; //10KB
	iBuffer.CreateMaxL(KAudioBufferSize);
	
	iAudClientAArnCallback = CTestGenericArnCallback::NewL(*this);
	iAudClientBArnCallback = CTestGenericArnCallback::NewL(*this);
	iVidClientArnCallback = CTestGenericArnCallback::NewL(*this);
	
	InitWservL();
	
	iTestTimer = CTestTimer::NewL(*this);
	
	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceNone,
										iWs, *iScreen, *iWindow, rect, clipRect);
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	iTestStepResult = EFail;
	return EPass;		
	}
	
TVerdict CTestStepVidPlayerARN::DoTestStepPostambleL()
	{
	//cleanup audio stream
	delete iAudStreamA; 
	iAudStreamA = NULL;
	
	//cleanup ARN callback handlers for clients
	delete iAudClientAArnCallback;
	iAudClientAArnCallback = NULL;
	delete iAudClientBArnCallback;
	iAudClientBArnCallback = NULL;
	delete iVidClientArnCallback;
	iVidClientArnCallback = NULL;
	
	//cleanup Audio output stream handlers
	delete iAudStreamACallback;
	iAudStreamACallback = NULL;
	delete iAudStreamBCallback;
	iAudStreamBCallback = NULL;

	delete iPlayer;
	iPlayer = NULL;
		
	//[ Destroy the scheduler ]
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	DeInitWserv();
	
	delete iTestTimer;
	iTestTimer = NULL;
	delete iPlayer;
	iPlayer = NULL;
	
	return EPass;	
	}

void CTestStepVidPlayerARN::MvpuoOpenComplete(TInt aError)
	{
	INFO_PRINTF2(_L("Video open:%d"),aError);
#ifndef SYMBIAN_MMF_MODEL_CONTROLLERS
	if(aError == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Feature not supported"));	
		iTestStepResult = EPass;
		CActiveScheduler::Stop();
		return;	
		}
#endif
	Fsm(EVideo, EVideoOpenComplete, aError);
	}

void CTestStepVidPlayerARN::MvpuoPrepareComplete(TInt aError)
	{
	INFO_PRINTF2(_L("Video prepare:%d"),aError);
	Fsm(EVideo, EVideoPrepareComplete, aError);		
	}
	
void CTestStepVidPlayerARN::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt /*aError*/)
	{	
	}

void CTestStepVidPlayerARN::MvpuoPlayComplete(TInt aError)
	{
	Fsm(EVideo, EVideoPlayComplete, aError);		
	}
	
void CTestStepVidPlayerARN::MvpuoEvent(const TMMFEvent& /* aEvent */)
	{	
	}

void CTestStepVidPlayerARN::MaoscOpenCompleteTest(TInt aError,CTestGenericAosCallback& aGenericAosCallback)
	{
	if(&aGenericAosCallback == iAudStreamACallback)
		{
		Fsm(EAudioStreamA, EAudioOpenComplete, aError);		
		}
	else if(&aGenericAosCallback == iAudStreamBCallback)
		{
		Fsm(EAudioStreamB, EAudioOpenComplete, aError);			
		}		
	}

void CTestStepVidPlayerARN::MaoscBufferCopiedTest(TInt aError, const TDesC8& /* aBuffer */, CTestGenericAosCallback& aGenericAosCallback)
	{
	if(&aGenericAosCallback == iAudStreamACallback)
		{
		Fsm(EAudioStreamA, EAudioBufferCopied, aError);		
		}
	else if(&aGenericAosCallback == iAudStreamBCallback)
		{
		Fsm(EAudioStreamB, EAudioBufferCopied, aError);			
		}			
	}

void CTestStepVidPlayerARN::MaoscPlayCompleteTest(TInt aError,CTestGenericAosCallback& aGenericAosCallback)
	{
	if(&aGenericAosCallback == iAudStreamACallback)
		{
		Fsm(EAudioStreamA, EAudioPlayComplete, aError);		
		}
	else if(&aGenericAosCallback == iAudStreamBCallback)
		{
		Fsm(EAudioStreamB, EAudioPlayComplete, aError);			
		}
	}

void CTestStepVidPlayerARN::FailTest()
	{
	iTestStepResult = EFail;
	CActiveScheduler::Stop();	
	}

/**
Accepts the input and checks if test can move to next/other state
@param	aClient
		a client which gives transition input
@param	aEvent
		event from the client
@param	aError
		error associated with the event
return  -1 if error should end the test; 0 test is in current state; 1 test move to new state		
 */
TInt CTestStepVidPlayerARN::AnalyseInput(TClient aClient, TTestEvent aEvent, TInt aError)
	{
	TInt res = JumpStateIfRequired(aClient, aEvent, aError,iCurrentState);
	if(res)
		{
		return res;		
		}
	
	if(aClient == ETestStep)
		{
		res = 1;			
		}
	else
		{
		CTransitionInput* pSet = ipTransitionSet[iCurrentState];
		for(TInt i = 0; i < iExpInputCnt[iCurrentState]; i++)
			{
			//check for client and event.
			if(pSet[i].iClient == aClient && pSet[i].iEvent == aEvent)
				{
				if(pSet[i].iError == aError)
					{
					if(!pSet[i].iMatched)
						{
						iRxInputCnt[iCurrentState]++;
						pSet[i].iMatched = ETrue;		
						}		
					}
				else if(!pSet[i].iIgnoreError)
					{
					return -1;	
					}
				break;
				}
			}
	
		if(iRxInputCnt[iCurrentState] == iExpInputCnt[iCurrentState])
			{
			res = 1;
			}
		else
			{
			res = 0;	
			}	
		}
		
	if(res == 1)
		{
		if(!NextState(iCurrentState))
			{
			res = -1;	
			}
		}
	return res;	
	}

/**
Takes input from the clients with the event and error. Calls  AnalyseInput(). 
If new state is entered, it calls OnStateEntry().
@param	aClient
		a client which gives transition input
@param	aEvent
		event from the client
@param	aError
		error associated with the event		
 */				
void CTestStepVidPlayerARN::Fsm(TClient aClient, TTestEvent aEvent, TInt aError)
	{
	TInt result = AnalyseInput(aClient,aEvent,aError);
	if(result == -1) //unexpected event/error - hence end test with fail
		{
		FailTest();
		return;		
		}	
	if(result == 1) //if moved to new state
		{
		OnStateEntry();
		}	
	}

void CTestStepVidPlayerARN::InitWservL()
	{
	TInt err = iWs.Connect();
	if (err != KErrNone)
		{
		// Access violation if ws is null
		INFO_PRINTF1(_L("Cannot test, no window server available"));
		User::Leave(err);
		}

	iScreen = new(ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction

	iRootWindow = RWindowGroup(iWs);
	User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));
	iWindow = new(ELeave) RWindow(iWs);
	User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
	iWindow->SetExtent(TPoint(0,0), TSize(400,200));
	iWindow->SetVisible(ETrue);
	iWindow->Activate();
	iWs.Flush();
	}

void CTestStepVidPlayerARN::DeInitWserv()
	{
	if (iWindow)
		{
		iWindow->Close();
		delete iWindow;
		iWindow = NULL;
		}
	iRootWindow.Close();
	delete iScreen;
	iScreen = NULL;
	iWs.Flush();
	iWs.Close();
	}

/**
Explanation for tests CTestStepVidPlayerARN0700 - CTestStepVidPlayerARN0703:-
Every test consists of set of states. Transition from one set to next set occurs
on getting a set of inputs. Input/Transition-input consists of client,event and error.
A state may require one or more Transition-inputs, to move to other state.
Clients and test setup give their inputs to Fsm(). This calls AnalyseInput(), to determine
if next state can be reached.

AnalyseInput() checks if all Transition-inputs are received for the current state so that
next state can be reached.If conditions are met, AnalysInput() moves the test to next state.

Fsm() checks if new state is entered.If so it calls OnStateEntry(), which performs certain 
operations on entering new state.
*/

//----------------------------------------------------------------
//Check RegisterForAudioResourceNotification() is working for Video
//----------------------------------------------------------------
CTestStepVidPlayerARN0700* CTestStepVidPlayerARN0700::NewL()
	{
	CTestStepVidPlayerARN0700* self = new(ELeave) CTestStepVidPlayerARN0700();
	return self;	
	}

CTestStepVidPlayerARN0700::~CTestStepVidPlayerARN0700()
	{
	}
/**
Initialises inputs - client,event & error, that lead to next state
*/	
void CTestStepVidPlayerARN0700::InitTransitionSetL()
	{
	ipTransitionSet[0] = NULL;
	iExpInputCnt[0] = 0;
	iRxInputCnt[0] = 0;
	
	ipTransitionSet[1] = new(ELeave) CTransitionInput[1]; //state:EStart
	ipTransitionSet[1][0].SetData(EVideo, EVideoOpenComplete, 0);
	iExpInputCnt[1] = 1;
	iRxInputCnt[1] = 0;
	
	ipTransitionSet[2] = new(ELeave) CTransitionInput[1]; //state:EVideoOpen
	ipTransitionSet[2][0].SetData(EVideo, EVideoPrepareComplete,0);
	iExpInputCnt[2] = 1;
	iRxInputCnt[2] = 0;
	
	ipTransitionSet[3] = new(ELeave) CTransitionInput[1]; //state:EVideoPrepare
	ipTransitionSet[3][0].SetData(EAudioStreamB, EAudioOpenComplete,0);
	iExpInputCnt[3] = 1;
	iRxInputCnt[3] = 0;
	
	ipTransitionSet[4] = new(ELeave) CTransitionInput[2]; //state:EVideoPlayingAudBOpen
	ipTransitionSet[4][0].SetData(EVideo, EVideoPlayComplete, KErrInUse);
	ipTransitionSet[4][0].iIgnoreError = ETrue;
	ipTransitionSet[4][1].SetData(EAudioStreamB, EAudioBufferCopied, 0);
	iExpInputCnt[4] = 2;
	iRxInputCnt[4] = 0;
	
	ipTransitionSet[5] = new(ELeave) CTransitionInput[2]; //state:EVideoInterrupted
	ipTransitionSet[5][0].SetData(EVideo, EArnAvailable, 0);
	ipTransitionSet[5][1].SetData(EAudioStreamB, EAudioPlayComplete, KErrUnderflow);
	ipTransitionSet[5][1].iIgnoreError = ETrue;
	iExpInputCnt[5] = 2;
	iRxInputCnt[5] = 0;
	
	iCurrentState = EPreStart;
	}

TVerdict CTestStepVidPlayerARN0700::DoTestStepPreambleL()
	{
	TVerdict verdict =  CTestStepVidPlayerARN::DoTestStepPreambleL();
	InitTransitionSetL();	
	_LIT(KTestFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\AudVid.avi");
	iFileName.Set(KTestFileName);	
	return verdict;
	}

TVerdict CTestStepVidPlayerARN0700::DoTestStepPostambleL()
	{
	TVerdict verdict = CTestStepVidPlayerARN::DoTestStepPostambleL();
	
	delete[] ipTransitionSet[0];
	ipTransitionSet[0] = NULL;
	delete[] ipTransitionSet[1];
	ipTransitionSet[1] = NULL;
	delete[] ipTransitionSet[2];
	ipTransitionSet[2] = NULL;
	delete[] ipTransitionSet[3];
	ipTransitionSet[3] = NULL;
	delete[] ipTransitionSet[4];
	ipTransitionSet[4] = NULL;
	delete[] ipTransitionSet[5];
	ipTransitionSet[5] = NULL;
	
	return verdict;
	}

CTestStepVidPlayerARN0700::CTestStepVidPlayerARN0700()
	{
	//name of the test case
	iTestStepName = _L("MM-MMF-VCLNT-U-0700-HP");
	}

TVerdict CTestStepVidPlayerARN0700::DoTestStepL()
	{
	Fsm(ETestStep, ETestStart, 0);
	iTestTimer->After(10000000);
	CActiveScheduler::Start();	
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;		
	}

void CTestStepVidPlayerARN0700::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& /* aNotificationData */, CTestGenericArnCallback & aGenericArnCallback)
	{
	if(&aGenericArnCallback == iVidClientArnCallback && aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		Fsm(EVideo, EArnAvailable, 0);	
		}
	}
	
void CTestStepVidPlayerARN0700::EndTest()
	{
	if(iVideoOpen)
		{
		//no need of iVideoOpen = ETrue;there won't be further callbacks.
		iPlayer->Close();
		}
	FailTest();
	}
	
TBool CTestStepVidPlayerARN0700::JumpStateIfRequired(TClient /* aClient */, TTestEvent /* aEvent */, TInt /* aError */,TInt& /* aState */)
	{
	return EFalse;	
	}
	
TBool CTestStepVidPlayerARN0700::NextState(TInt& aState)
	{
	TBool res = ETrue;
	switch(iCurrentState)
		{
	case EPreStart:
		aState = EStart;
		break;
	case EStart:
		aState = EVideoOpen;
		break;
	case EVideoOpen:
		aState = EVideoPrepare;
		break;
	case EVideoPrepare:
		aState = EVideoPlayingAudBOpen;
		break;
	case EVideoPlayingAudBOpen:
		aState = EVideoInterrupted;
		break;
	case EVideoInterrupted:
		aState = ETestEnd;
		break;
	default:
		res = EFalse;
		break;
		}
	return res;
	}
/**
 On entering new state performs certain operations
*/
void CTestStepVidPlayerARN0700::OnStateEntry()
	{
	TInt err = KErrNone;
	switch(iCurrentState)
		{
	case EStart: //to begin, open the video
		TRAP(err,iPlayer->OpenFileL(iFileName));
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iPlayer->OpenFileL(iFileName):%d"),err);
			FailTest();
			}
		break;
		
	case EVideoOpen: //prepare video
		iVideoOpen = ETrue;
		iPlayer->Prepare();
		break;
		
		//register for audio notification on Video; open audio stream-B
	case EVideoPrepare:
		err = iPlayer->RegisterAudioResourceNotification(*iVidClientArnCallback,KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:iPlayer->RegisterAudioResourceNotification():%d"),err);
			FailTest();
			break;
			}
		iPlayer->Play(); //play video
		iAudStreamB->Open(NULL);
		break;
		
	case EVideoPlayingAudBOpen:	
		TRAP(err,iAudStreamB->WriteL(iBuffer));//start high priority audio	
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iAudStreamB->WriteL(iBuffer):%d"),err);
			FailTest();		
			}
		break;
	
	case EVideoInterrupted: //do nothing on state entry
		break;
		
	case ETestEnd:
		iTestStepResult = EPass;
		iPlayer->Close();
		INFO_PRINTF1(_L("Video:Got audio resource notification"));
		CActiveScheduler::Stop();
		break;
		}
	}

//----------------------------------------------------------------
//register for Audio Resource Notification from Video; then cancel
//it; Video should not get audio resource notification

CTestStepVidPlayerARN0701* CTestStepVidPlayerARN0701::NewL()
	{
	CTestStepVidPlayerARN0701* self = new(ELeave) CTestStepVidPlayerARN0701();
	return self;	
	}

/**
Initialises inputs - client,event & error, that lead to next state
*/	
void CTestStepVidPlayerARN0701::InitTransitionSetL()
	{
	ipTransitionSet[0] = NULL;
	iExpInputCnt[0] = 0;
	iRxInputCnt[0] = 0;
	
	ipTransitionSet[1] = new(ELeave) CTransitionInput[1]; //state:EStart
	ipTransitionSet[1][0].SetData(EVideo, EVideoOpenComplete, 0);
	iExpInputCnt[1] = 1;
	iRxInputCnt[1] = 0;
	
	ipTransitionSet[2] = new(ELeave) CTransitionInput[1]; //state:EVideoOpen
	ipTransitionSet[2][0].SetData(EVideo, EVideoPrepareComplete,0);
	iExpInputCnt[2] = 1;
	iRxInputCnt[2] = 0;
	
	ipTransitionSet[3] = new(ELeave) CTransitionInput[1]; //state:EVideoPrepare
	ipTransitionSet[3][0].SetData(EAudioStreamB, EAudioOpenComplete,0);
	iExpInputCnt[3] = 1;
	iRxInputCnt[3] = 0;
	
	ipTransitionSet[4] = new(ELeave) CTransitionInput[3]; //state:EVideoRegisterAudBOpen
	ipTransitionSet[4][0].SetData(EVideo, EVideoPlayComplete, KErrInUse);
	ipTransitionSet[4][0].iIgnoreError = ETrue;
	ipTransitionSet[4][1].SetData(EAudioStreamB, EAudioBufferCopied, 0);
	ipTransitionSet[4][2].SetData(EAudioStreamB, EAudioPlayComplete, KErrUnderflow);
	ipTransitionSet[4][2].iIgnoreError = ETrue;
	iExpInputCnt[4] = 3;
	iRxInputCnt[4] = 0;
	
	ipTransitionSet[5] = new(ELeave) CTransitionInput[1]; //state:EVideoInterrupted	
	ipTransitionSet[5][0].SetData(EVideo, EArnAvailable, 0);
	iExpInputCnt[5] = 1;
	iRxInputCnt[5] = 0;
	
	iCurrentState = EPreStart;	
	}

CTestStepVidPlayerARN0701::~CTestStepVidPlayerARN0701()
	{
	}
	
TVerdict CTestStepVidPlayerARN0701::DoTestStepPreambleL()
	{
	TVerdict verdict = CTestStepVidPlayerARN::DoTestStepPreambleL();
	InitTransitionSetL();
	_LIT(KTestFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\AudVid.avi");
	iFileName.Set(KTestFileName);
	return verdict;
	}

TVerdict CTestStepVidPlayerARN0701::DoTestStepPostambleL()
	{
	TVerdict verdict = CTestStepVidPlayerARN::DoTestStepPostambleL();
	delete iTestTimer;
	iTestTimer = NULL;
	delete iPlayer;
	iPlayer = NULL;
	
	delete[] ipTransitionSet[0];
	ipTransitionSet[0] = NULL;
	delete[] ipTransitionSet[1];
	ipTransitionSet[1] = NULL;
	delete[] ipTransitionSet[2];
	ipTransitionSet[2] = NULL;
	delete[] ipTransitionSet[3];
	ipTransitionSet[3] = NULL;
	delete[] ipTransitionSet[4];
	ipTransitionSet[4] = NULL;
	delete[] ipTransitionSet[5];
	ipTransitionSet[5] = NULL;
		
	return verdict;
	}

CTestStepVidPlayerARN0701::CTestStepVidPlayerARN0701():iTestSimulationComplete(EFalse)
	{
	//name of the test case
	iTestStepName = _L("MM-MMF-VCLNT-U-0701-HP");
	}

TVerdict CTestStepVidPlayerARN0701::DoTestStepL()
	{
	Fsm(ETestStep, ETestStart, 0);
	iTestTimer->After(10000000);
	CActiveScheduler::Start();	

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;		
	}

void CTestStepVidPlayerARN0701::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& /* aNotificationData */, CTestGenericArnCallback & aGenericArnCallback)
	{
	if(&aGenericArnCallback == iVidClientArnCallback && aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		INFO_PRINTF1(_L("Video: Got Audio Resource Notification"));
		Fsm(EVideo, EArnAvailable, 0);	
		}
	}
	
void CTestStepVidPlayerARN0701::EndTest()
	{
	if(iVideoOpen)
		{
		//no need of iVideoOpen = ETrue;there won't be further callbacks.
		iPlayer->Close();			
		}
	//if test simulation conditions are complete - including registration and 
	//cancellation for video player
	if(iTestSimulationComplete)
		{
		INFO_PRINTF1(_L("Video: register & cancel for Arn. No MarncResouceAvailable callback.Test passes"));
		iTestStepResult = EPass;	
		}
	CActiveScheduler::Stop();
	}

TBool CTestStepVidPlayerARN0701::JumpStateIfRequired(TClient aClient, TTestEvent aEvent, TInt /* aError */,TInt& aState)
	{
	TBool res = EFalse;
	if(aClient == EVideo && aEvent == EArnAvailable) //move to EFailureOnNotification
		{
		if(iCurrentState != EFailureOnNotification)
			{
			aState = EFailureOnNotification;
			res = ETrue;
			}	
		}
	return res;
	}
	
TBool CTestStepVidPlayerARN0701::NextState(TInt& aState)
	{
	TBool res = ETrue;
	switch(iCurrentState)
		{
	case EPreStart:
		aState = EStart;
		break;
	case EStart:
		aState = EVideoOpen;
		break;
	case EVideoOpen:
		aState = EVideoPrepare;
		break;
	case EVideoPrepare:
		aState = EVideoRegisterAudBOpen;
		break;
	case EVideoRegisterAudBOpen:
		aState = EVideoInterrupted;
		break;
	case EVideoInterrupted:
		aState = EFailureOnNotification ;
		break;
	default:
		res = EFalse;
		break;
		}
	return res;
	}

/**
 On entering new state performs certain operations
*/
void CTestStepVidPlayerARN0701::OnStateEntry()
	{
	TInt err = KErrNone;
	switch(iCurrentState)
		{
	case EStart: //to begin, open the video
		TRAP(err,iPlayer->OpenFileL(iFileName));
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iPlayer->OpenFileL(iFileName):%d"),err);
			FailTest();
			}
		break;
		
	case EVideoOpen: //prepare video
		iVideoOpen = ETrue;
		iPlayer->Prepare();
		break;
		
		//register for audio notification on Video; open audio stream-B
	case EVideoPrepare:
		err = iPlayer->RegisterAudioResourceNotification(*iVidClientArnCallback,KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:iPlayer->RegisterAudioResourceNotification():%d"),err);
			FailTest();
			break;
			}
		iAudStreamB->Open(NULL);
		break;
		
	case EVideoRegisterAudBOpen:
		iPlayer->Play(); //play video
		err = iPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:iPlayer->CancelRegisterAudioResourceNotification():%d"),err);
			FailTest();
			break;	
			}
		TRAP(err,iAudStreamB->WriteL(iBuffer));//start high priority audio	
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iAudStreamB->WriteL(iBuffer):%d"),err);
			FailTest();
			}
		iTestSimulationComplete = ETrue;
		break;
		
	case EVideoInterrupted:
		break;
		
	case EFailureOnNotification:
		iPlayer->Close();
		FailTest();
		break;
		}	
	}

//----------------------------------------------------------------

CTestStepVidPlayerARN0702* CTestStepVidPlayerARN0702::NewL()
	{
	CTestStepVidPlayerARN0702* self = new(ELeave) CTestStepVidPlayerARN0702();
	return self;	
	}

/**
Initialises inputs - client,event & error, that lead to next state
*/	
void CTestStepVidPlayerARN0702::InitTransitionSetL()
	{
	ipTransitionSet[0] = NULL;
	iExpInputCnt[0] = 0;
	iRxInputCnt[0] = 0;
	
	ipTransitionSet[1] = new(ELeave) CTransitionInput[1]; //state:EStart
	ipTransitionSet[1][0].SetData(EVideo, EVideoOpenComplete, 0);
	iExpInputCnt[1] = 1;
	iRxInputCnt[1] = 0;
	
	ipTransitionSet[2] = new(ELeave) CTransitionInput[1]; //state:EVideoOpen
	ipTransitionSet[2][0].SetData(EVideo, EVideoPrepareComplete,0);
	iExpInputCnt[2] = 1;
	iRxInputCnt[2] = 0;
	
	ipTransitionSet[3] = new(ELeave) CTransitionInput[2]; //state:EVideoPrepare
	ipTransitionSet[3][0].SetData(EAudioStreamA, EAudioOpenComplete,0);
	ipTransitionSet[3][1].SetData(EAudioStreamB, EAudioOpenComplete,0);
	iExpInputCnt[3] = 2;
	iRxInputCnt[3] = 0;
	
	ipTransitionSet[4] = new(ELeave) CTransitionInput[3]; //state:EAudABOpenRegister
	ipTransitionSet[4][0].SetData(EVideo, EVideoPlayComplete, KErrInUse);
	ipTransitionSet[4][0].iIgnoreError = ETrue;
	ipTransitionSet[4][1].SetData(EAudioStreamA, EAudioPlayComplete, KErrInUse);
	ipTransitionSet[4][2].SetData(EAudioStreamB, EAudioBufferCopied, 0);
	iExpInputCnt[4] = 3;
	iRxInputCnt[4] = 0;
	
	ipTransitionSet[5] = new(ELeave) CTransitionInput[2]; //state:EVideoAudAThrownoff
	ipTransitionSet[5][0].SetData(EVideo, EArnAvailable, 0);
	ipTransitionSet[5][1].SetData(EAudioStreamB, EAudioPlayComplete, KErrUnderflow);
	ipTransitionSet[5][1].iIgnoreError = ETrue;
	iExpInputCnt[5] = 2;
	iRxInputCnt[5] = 0;
	
	ipTransitionSet[6] = new(ELeave) CTransitionInput[1]; //state:EVideoNotified
	ipTransitionSet[6][0].SetData(EAudioStreamA, EArnAvailable, 0);
	iExpInputCnt[6] = 1;
	iRxInputCnt[6] = 0;
	
	iCurrentState = EPreStart;		
	}
	
CTestStepVidPlayerARN0702::~CTestStepVidPlayerARN0702()
	{
	}
	
TVerdict CTestStepVidPlayerARN0702::DoTestStepPreambleL()
	{
	TVerdict verdict = CTestStepVidPlayerARN::DoTestStepPreambleL();
	InitTransitionSetL();
	_LIT(KTestFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\AudVid.avi");
	iFileName.Set(KTestFileName);
	return verdict;
	}

TVerdict CTestStepVidPlayerARN0702::DoTestStepPostambleL()
	{
	TVerdict verdict = CTestStepVidPlayerARN::DoTestStepPostambleL();
	delete iTestTimer;
	iTestTimer = NULL;
	delete iPlayer;
	iPlayer = NULL;
	
	delete[] ipTransitionSet[0];
	ipTransitionSet[0] = NULL;
	delete[] ipTransitionSet[1];
	ipTransitionSet[1] = NULL;
	delete[] ipTransitionSet[2];
	ipTransitionSet[2] = NULL;
	delete[] ipTransitionSet[3];
	ipTransitionSet[3] = NULL;
	delete[] ipTransitionSet[4];
	ipTransitionSet[4] = NULL;
	delete[] ipTransitionSet[5];
	ipTransitionSet[5] = NULL;
	delete[] ipTransitionSet[6];
	ipTransitionSet[6] = NULL;
	
	return verdict;
	}

CTestStepVidPlayerARN0702::CTestStepVidPlayerARN0702():iAudStreamANotified(EFalse)
	{
	//name of the test case
	iTestStepName = _L("MM-MMF-VCLNT-U-0702-HP");
	}

TVerdict CTestStepVidPlayerARN0702::DoTestStepL()
	{
	Fsm(ETestStep, ETestStart, 0);
	iTestTimer->After(10000000);
	CActiveScheduler::Start();	

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;		
	}

void CTestStepVidPlayerARN0702::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& /* aNotificationData */, CTestGenericArnCallback & aGenericArnCallback)
	{
	if(&aGenericArnCallback == iAudClientAArnCallback && aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		iAudStreamANotified = ETrue;
		Fsm(EAudioStreamA, EArnAvailable, 0);				
		}
	else if(&aGenericArnCallback == iVidClientArnCallback && aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		Fsm(EVideo, EArnAvailable, 0);
		}				
	}
	
void CTestStepVidPlayerARN0702::EndTest()
	{
	if(iVideoOpen)
		{
		//no need of iVideoOpen = ETrue;there won't be further callbacks.
		iPlayer->Close();			
		}
	if(iAudStreamANotified)
		{
		iTestStepResult = EFail;	
		}
	else
		{
		iTestStepResult = EPass;			
		}
	CActiveScheduler::Stop();	
	}


TBool CTestStepVidPlayerARN0702::JumpStateIfRequired(TClient /* aClient */, TTestEvent /* aEvent */, TInt /* aError */, TInt& /* aState */)
	{
	return EFalse;	
	}
	
TBool CTestStepVidPlayerARN0702::NextState(TInt& aState)
	{
	TBool res = ETrue;
	switch(iCurrentState)
		{
	case EPreStart:
		aState = EStart;
		break;
	case EStart:
		aState = EVideoOpen;
		break;
	case EVideoOpen:
		aState = EVideoPrepare;
		break;
	case EVideoPrepare:
		aState = EAudABOpenRegister;
		break;
	case EAudABOpenRegister:
		aState = EVideoAudAThrownoff;
		break;
	case EVideoAudAThrownoff:
		aState = EVideoNotified;
		break;
	case EVideoNotified:
		aState = EFailureOnAudioANotification;
		break;
	default: 
		res = EFalse;
		break;
		}
	return res;
	}

/**
 On entering new state performs certain operations
*/
void CTestStepVidPlayerARN0702::OnStateEntry()
	{
	TInt err = KErrNone;
	switch(iCurrentState)
		{
	case EStart: //to begin, open the video
		TRAP(err,iPlayer->OpenFileL(iFileName));
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iPlayer->OpenFileL(iFileName):%d"),err);
			FailTest();
			}
		break;
		
	case EVideoOpen: //prepare video
		iVideoOpen = ETrue;
		iPlayer->Prepare();
		break;
		
		//register for audio notification on Video; open audio stream-B
	case EVideoPrepare:
		//open the audio streams and register for audio resource notification
		iAudStreamA->Open(NULL);
		err = iAudStreamA->RegisterAudioResourceNotification(*iAudClientAArnCallback,KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:iAudStreamA->RegisterAudioResourceNotification():%d"),err);
			FailTest();
			break;
			}
		iAudStreamB->Open(NULL);
		err = iAudStreamB->RegisterAudioResourceNotification(*iAudClientBArnCallback,KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error: iAudStreamB->RegisterAudioResourceNotification():%d"),err);
			FailTest();
			break;
			}
		break;
		
	case EAudABOpenRegister:
		//start playing video; then start Audio streams A and B
		iPlayer->Play(); //start video play	
		TRAP(err,iAudStreamA->WriteL(iBuffer)) //start low priority audio
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iAudStreamA->WriteL(iBuffer):%d"),err);
			FailTest();
			break;	
			}
		TRAP(err,iAudStreamB->WriteL(iBuffer));//start high priority audio
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iAudStreamB->WriteL(iBuffer):%d"),err);		
			FailTest();
			break;
			}
		break;
		
		
	case EVideoAudAThrownoff:
		err = iPlayer->RegisterAudioResourceNotification(*iVidClientArnCallback,KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:iPlayer->RegisterAudioResourceNotification():%d"),err);		
			FailTest();
			}
		break;
		
	case EVideoNotified:
		err = iPlayer->WillResumePlay();
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error:WillResumePlay():%d"),err);
			FailTest();	
			}
		break;
		
	case EFailureOnAudioANotification:
		iPlayer->Close();
		INFO_PRINTF1(_L("Audio stream-A got notification"));
		FailTest();
		break;
		}	
	}

//--------------------------------------------------------------------
//open video without audio component.Register for Audio Resource Notification
//it should get KErrNotSupported
CTestStepVidPlayerARN0703* CTestStepVidPlayerARN0703::NewL()
	{
	CTestStepVidPlayerARN0703* self = new(ELeave) CTestStepVidPlayerARN0703();
	return self;	
	}

/**
Initialises inputs - client,event & error, that lead to next state
*/
void CTestStepVidPlayerARN0703::InitTransitionSetL()
	{
	ipTransitionSet[0] = NULL;
	iExpInputCnt[0] = 0;
	iRxInputCnt[0] = 0;
	
	ipTransitionSet[1] = new(ELeave) CTransitionInput[1]; //state:EStart
	ipTransitionSet[1][0].SetData(EVideo, EVideoOpenComplete, 0);
	iExpInputCnt[1] = 1;
	iRxInputCnt[1] = 0;
	
	ipTransitionSet[2] = new(ELeave) CTransitionInput[1]; //state:EVideoOpen
	ipTransitionSet[2][0].SetData(EVideo, EVideoPrepareComplete,0);
	iExpInputCnt[2] = 1;
	iRxInputCnt[2] = 0;
	}

CTestStepVidPlayerARN0703::~CTestStepVidPlayerARN0703()
	{
	}
	
TVerdict CTestStepVidPlayerARN0703::DoTestStepPreambleL()
	{
	TVerdict verdict =  CTestStepVidPlayerARN::DoTestStepPreambleL();
	InitTransitionSetL();
	_LIT(KTestFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\Vid.avi");
	iFileName.Set(KTestFileName);
	return verdict;
	}

TVerdict CTestStepVidPlayerARN0703::DoTestStepPostambleL()
	{
	TVerdict verdict = CTestStepVidPlayerARN::DoTestStepPostambleL();
	delete iPlayer;
	iPlayer = NULL;
	
	delete[] ipTransitionSet[0];
	ipTransitionSet[0] = NULL;
	delete[] ipTransitionSet[1];
	ipTransitionSet[1] = NULL;
	delete[] ipTransitionSet[2];
	ipTransitionSet[2] = NULL;
	
	return verdict;
	}

CTestStepVidPlayerARN0703::CTestStepVidPlayerARN0703()
	{
	//name of the test case
	iTestStepName = _L("MM-MMF-VCLNT-U-0703-HP");
	}

TVerdict CTestStepVidPlayerARN0703::DoTestStepL()
	{
	Fsm(ETestStep, ETestStart, 0);
	CActiveScheduler::Start();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;		
	}
	
void CTestStepVidPlayerARN0703::MarncResourceAvailableTest(TUid /* aNotificationEventId */, const TDesC8& /* aNotificationData */, CTestGenericArnCallback& /* aGenericArnCallback */)
	{
	}
		
void CTestStepVidPlayerARN0703::EndTest()
	{
	}

TBool CTestStepVidPlayerARN0703::JumpStateIfRequired(TClient /* aClient */, TTestEvent /* aEvent */, TInt /* aError */,TInt& /* aState */)
	{
	return EFalse;	
	}
	
TBool CTestStepVidPlayerARN0703::NextState(TInt& aState)
	{
	TBool res = ETrue;
	switch(iCurrentState)
		{
	case EPreStart:
		aState = EStart;
		break;
	case EStart:
		aState = EVideoOpen;
		break;
	case EVideoOpen:
		aState = EVideoPrepare;
		break;
	default:
		res = EFalse;
		break;
		}
	return res;
	}

/**
 On entering new state performs certain operations
*/
void CTestStepVidPlayerARN0703::OnStateEntry()
	{
	TInt err = KErrNone;
	switch(iCurrentState)
		{
	case EStart: //to begin, open the video
		TRAP(err,iPlayer->OpenFileL(iFileName));
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Leave error:iPlayer->OpenFileL(iFileName):%d"),err);
			FailTest();
			}
		break;
		
	case EVideoOpen: //prepare video
		iPlayer->Prepare();
		break;
		
		//register for audio notification on Video; open audio stream-B
	case EVideoPrepare:
		TInt err = iPlayer->RegisterAudioResourceNotification(*iVidClientArnCallback,KMMFEventCategoryAudioResourceAvailable);
		if(err == KErrArgument)
			{
			iTestStepResult = EPass;
			INFO_PRINTF1(_L("iPlayer->RegisterAudioResourceNotification() - returned - KErrArgument"));	
			}
		iPlayer->Close();
		CActiveScheduler::Stop();
		break;
		}
	}
//-------------------------------------------------------------------------------------
