// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <vclntapptest.rsg>

#ifdef SYMBIAN_BUILD_GCE
#include <gceavailable.h>
#endif

#include "VclntAppTest.h"

const TUid KVideoTestControllerUid = { KMmfVideoTestControllerUid };	//(0x101F88D9)

// CVclntTestView
CVideoPlayerUtility::CTestView* CVideoPlayerUtility::CTestView::NewL(const TRect& aRect, MVclntTestObserver* aObserver, CLog* aLog)
	{
	CTestView* self = new(ELeave) CTestView(aObserver, aLog);
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop(self);
	return self;
	}

CVideoPlayerUtility::CTestView::CTestView(MVclntTestObserver* aObserver, CLog* aLog)
	: iObserver(aObserver), iLog(aLog)
	{
	}

void CVideoPlayerUtility::CTestView::ConstructL(const TRect& aRect)
	{
	iIdle = CIdle::NewL(CActive::EPriorityIdle);
	CreateWindowL();
	SetRect(aRect);
	SetBlank();
	ActivateL();
	}

CVideoPlayerUtility::CTestView::~CTestView()
	{
	delete iPlayer;
	delete iIdle;
	delete iDialog;
	}

void CVideoPlayerUtility::CTestView::CreateVideoPlayerL()
	{
	TRect rect, clipRect;
	rect = clipRect = TRect(0, 0, 400, 200);
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal,
										EMdaPriorityPreferenceTimeAndQuality,
										iEikonEnv->WsSession(),
										*iEikonEnv->ScreenDevice(),
										Window(), rect, clipRect);

	iPlayer->OpenFileL(_L("C:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy"), KVideoTestControllerUid);
	}

void CVideoPlayerUtility::CTestView::CheckFunctionsCalledL()
	{
	TBuf8<64> functionText;
	TUid uid = { KMmfVideoTestControllerUid };
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg message(handleInfo);

	// Check DirectScreenAccessEventL is called
	TInt err = iPlayer->iBody->iController.CustomCommandSync(message, KLogFunction, KNullDesC8,
															 KNullDesC8, functionText);
	if (err == KErrNone) 
		{
		_LIT8(KEvent0nongce, "DirectScreenAccessEventL EResumeDSA");

#ifdef SYMBIAN_BUILD_GCE
		_LIT8(KEvent0gce, "MvpssUseSurfacesL Called");
		if ((GCEAvailable() && functionText.Compare(KEvent0gce) != 0)
			|| (!GCEAvailable() && functionText.Compare(KEvent0nongce) != 0))
#else
		if (functionText.Compare(KEvent0nongce) != 0)
#endif
			{
			iLog->Log(_L("DirectScreenAccessEventL not called"));
			iObserver->TestFinishedL(EFail);
			return;
			}
		}
	else
		{
		iLog->Log(_L("Error getting log function text"));
		iObserver->TestFinishedL(EInconclusive);
		return;
		}

	// Check UpdateDisplayRegionL is called
	err = iPlayer->iBody->iController.CustomCommandSync(message, KLogFunction2, KNullDesC8,
															 KNullDesC8, functionText);
	if (err == KErrNone)
		{
		_LIT8(KEvent1nongce, "UpdateDisplayRegionL called");

#ifdef SYMBIAN_BUILD_GCE
		_LIT8(KEvent1gce, "");
		if ((GCEAvailable() && functionText.Compare(KEvent1gce) == 0)
			|| (!GCEAvailable() && functionText.Compare(KEvent1nongce) == 0))
#else
		if (functionText.Compare(KEvent1nongce) == 0)
#endif
			{
			iLog->Log(_L("Test VCLNT U 0050 passed"));
			iObserver->TestFinishedL(EPass);
			}
		else
			{
			iLog->Log(_L("Test VCLNT U 0050 failed"));
			iObserver->TestFinishedL(EFail);
			}
		}
	else
		{
		iLog->Log(_L("Error getting log function text"));
		iObserver->TestFinishedL(EInconclusive);
		return;
		}
	}

void CVideoPlayerUtility::CTestView::MvpuoOpenComplete(TInt /*aError*/)
	{
	iDialog = new CEikDialog();
	if (!iDialog)
		{
		iLog->Log(_L("Could not create a CEikDialog"));
		iObserver->TestFinishedL(EInconclusive);
		return;
		}
	
	TRAPD(err, iDialog->ExecuteLD(R_VCLNT_TEST_DIALOG));
	if (err != KErrNone)
		{
		delete iDialog;
		iDialog = NULL;
		
		iLog->Log(_L("Error executing iDialog->ExecuteLD()"));
		iObserver->TestFinishedL(EInconclusive);
		return;
		}		

	// We have to wait a bit then check whether the correct function has been called
	TCallBack callBack(VclntTestCallBack, this);
	iIdle->Start(callBack);
	}

TInt CVideoPlayerUtility::CTestView::VclntTestCallBack(TAny* aPtr)
	{
	CTestView* self = STATIC_CAST(CTestView*, aPtr);
	TRAPD(err, self->DoVclntTestCallBackL());
	if (err != KErrNone)
		{
		self->iLog->Log(_L("Error during VclntTestCallBack %d"), err);
		}
	return EFalse;			// don't call again
	}

void CVideoPlayerUtility::CTestView::DoVclntTestCallBackL()
	{
	delete iDialog;
	iDialog = NULL;
	CheckFunctionsCalledL();
	}

void CVideoPlayerUtility::CTestView::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CVideoPlayerUtility::CTestView::MvpuoPlayComplete(TInt /*aError*/)
	{
	}
	

void CVideoPlayerUtility::CTestView::MvpuoPrepareComplete(TInt /*aError*/)
	{
	}


void CVideoPlayerUtility::CTestView::MvpuoEvent(const TMMFEvent& /*aError*/)
	{
	}

// CVclntTestAppUi
CVclntTestAppUi::~CVclntTestAppUi()
	{
	if (iView)
		{
		RemoveFromStack(iView);
		delete iView;
		}
	delete iIdleStarter;
	delete iLog;
	}

void CVclntTestAppUi::ConstructL()
	{
	// Trap here so that if anything goes wrong during the tests then (hopefully) we can still exit the app
	// (and signal the test framework to continue)
	TRAPD(err, DoConstructL());
	if (err != KErrNone && iLog)
		iLog->Log(_L("Error during vclnt test app %d"), err);
	}

TBool CVclntTestAppUi::ProcessCommandParametersL(TApaCommand aCommand,
												 TFileName& aDocumentName,
												 const TDesC8& aTail)
	{
	iTestStepName = aTail;
	return CEikAppUi::ProcessCommandParametersL(aCommand, aDocumentName, aTail);
	}

void CVclntTestAppUi::DoConstructL()
	{
	BaseConstructL();

	// Use existing logger from TestFramework
	iLog = CLog::NewL();
	iLog->OpenLogFileL();

	// Create view
	iView = CVideoPlayerUtility::CTestView::NewL(ClientRect(), this, iLog);
	AddToStackL(iView);

	// Schedule the tests to start when we're idle
	iIdleStarter = CIdle::NewL(CActive::EPriorityIdle);
	TCallBack callBack(StartTests, this);
	iIdleStarter->Start(callBack);
	}

TInt CVclntTestAppUi::StartTests(TAny* aPtr)
	{
	CVclntTestAppUi* self = STATIC_CAST(CVclntTestAppUi*, aPtr);
	TRAP_IGNORE(self->DoTestsL());
	return EFalse;		// don't call again
	}

void CVclntTestAppUi::DoTestsL()
	{
	// Run the tests
	TInt compare = iTestStepName.Compare(_L8("MM-MMF-VCLNT-U-0050-LP"));
	if (compare == 0)
		{
		iLog->Log(_L("Vclnt test 0050 started..."));
		RunTest_MM_MMF_VCLNT_U_0050L();
		}
	else
		{
		TBuf<KMaxLenTestStepName> testStepName;
		testStepName.Copy(iTestStepName);
		iLog->Log(_L("Test '%S' not found in vclnt test app"), &testStepName);
		WriteResultFileL(ETestSuiteError);
		}
	}

void CVclntTestAppUi::TestFinishedL(const TVerdict aVerdict)
	{
	// Write test result to the test result file
	WriteResultFileL(aVerdict);
	iLog->Log(_L("Vclnt test 0050 finished"));
	SendExitKeyEvent();	// Exit app (we only have one test to do)
	}

void CVclntTestAppUi::SendExitKeyEvent()
	{
	// Sends a key event to the window server to force the app to exit (without user intervention)
	TWsEvent event;
	event.SetType(EEventKey);
	event.Key()->iCode = 5;
	event.Key()->iScanCode = 'E';
	event.Key()->iModifiers = EModifierCtrl;
	event.Key()->iRepeats = 0;

	iEikonEnv->WsSession().SendEventToWindowGroup(iEikonEnv->RootWin().Identifier(), event);
	}

void CVclntTestAppUi::WriteResultFileL(const TVerdict aResult)
	{
	// Write the test result as a descriptor to the test results file
	RFs& fs = iEikonEnv->FsSession();
	RFile file;
	User::LeaveIfError(file.Create(fs, KVclntAppTestResultName, EFileRead | EFileWrite));
	TBuf8<32> resultBuf;
	switch (aResult)
		{
		case EPass:
			resultBuf = KLitPass;
			break;
		case EFail:
			resultBuf = KLitFail;
			break;
		case EInconclusive:
			resultBuf = KLitInconclusive;
			break;
		case ETestSuiteError:
			resultBuf = KLitTestSuiteError;
			break;
		default:
			resultBuf = _L8("Unrecognized result value");
			break;
		}
	User::LeaveIfError(file.Write(resultBuf));
	file.Close();
	}

void CVclntTestAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand) 
		{
		case EVclntCmdDoTests:
			{
			DoTestsL();
			}
			break;
		case EEikCmdExit: 
			{
			// Signal the test framework to proceed
			RSemaphore sem;
			TInt err = sem.OpenGlobal(KVclntAppSemName);
			if (err == KErrNone)
				{
				sem.Signal();
				sem.Close();
				}
			Exit();
			}
			break;
		}
	}

void CVclntTestAppUi::RunTest_MM_MMF_VCLNT_U_0050L()
	{
	iView->CreateVideoPlayerL();
	}

// CVclntTestDoc
CVclntTestDoc* CVclntTestDoc::NewL(CEikApplication& aApp)
	{
	CVclntTestDoc* self=new (ELeave) CVclntTestDoc(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CVclntTestDoc::CVclntTestDoc(CEikApplication& aApp)
	: CEikDocument(aApp)
	{
	}

void CVclntTestDoc::ConstructL()
	{
	}

CEikAppUi* CVclntTestDoc::CreateAppUiL()
	{
	return new(ELeave) CVclntTestAppUi();
	}

// CVclntTestApplication
CApaDocument* CVclntTestApplication::CreateDocumentL()
	{
	return CVclntTestDoc::NewL(*this);
	}

TUid CVclntTestApplication::AppDllUid() const
	{
	return KUidVclntTestApp;
	}


//
// Base factory function
//

#include <eikstart.h>
LOCAL_C CApaApplication* NewApplication()
	{
	return new CVclntTestApplication;
	}

//
// EXE Entry point
//

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}


	
