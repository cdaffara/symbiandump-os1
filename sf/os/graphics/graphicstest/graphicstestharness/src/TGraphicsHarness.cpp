// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <hal.h>
#include <w32debug.h>
#include <e32std.h> 
#include <graphics/fbsdefs.h>

#include "apgrfxfacade.h"
#include "TGraphicsHarness.h"
#include "GraphicsTestUtilsServer.h"

_LIT(KGraphicsTestFrameworkPanic, "Graphics Test Framework");
const TInt KTestCleanupStack = 0x40;

const TDisplayMode testDisplayModes[] =	
	{
	EGray2,
	EGray4,
	EGray16,
	EGray256,
	EColor16,
	EColor256,
	EColor64K,
	EColor16M,
	EColor4K,
	EColor16MU,
	EColor16MA,
	EColor16MAP,
	};

/**
  The test code manager, provides functions to set active object 
  with lowest priority for running tests in auto mode.
 */
class CTestManager : public CActive
	{
friend class CTGraphicsBase;
protected:
	static CTestManager* NewL(MTestCases* aAutoTestApp);
	~CTestManager();
	void FinishAllTestCases();
	void StartTest(); //initialize an active object and set it status as active
	void SetSelfDrive(TBool aSelfDrive);
	void CaseComplete();
private:
	CTestManager(MTestCases* aAutoTestApp);
	void NextTest();
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	MTestCases* iTestCase;
	TInt iCurTestCaseNumber; //a current test case, every time when function RunTestCaseL is called this member inc by one
	TBool iTestCompleted;
	TBool iSelfDrive;
	};

/** Construct an active object with the lowest priority */
CTestManager::CTestManager(MTestCases* aTestCases) :
		CActive(EPriorityIdle), iTestCase(aTestCases), iSelfDrive(EFalse)
	{}

CTestManager::~CTestManager()
	{
	Cancel();
	}

CTestManager* CTestManager::NewL(MTestCases* aTestCases)
	{
     CTestManager *theTestManager=new (ELeave) CTestManager(aTestCases);
     CActiveScheduler::Add(theTestManager);
	 return theTestManager;
	}

void CTestManager::RunL()
	{
	__ASSERT_ALWAYS(iTestCase, User::Panic(KGraphicsTestFrameworkPanic, KErrBadHandle));
	if(	iTestCompleted)
		{
		return;
		}
	
	// debug statement to indicate progress of test suite by
	// printing the sub-test that is currently active
	TTime timeStamp;
	timeStamp.HomeTime();
	TBuf<50> dateStr;
	_LIT(KDateString3,"%-B%:0%J%:1%T%:2%S%:3.%Cms");
	timeStamp.FormatL(dateStr, KDateString3);
	++iCurTestCaseNumber;
	RDebug::Print(_L("%S - Test Case Number: %d"), &dateStr, iCurTestCaseNumber);

	iTestCase->RunTestCaseL(iCurTestCaseNumber);
	if (iTestCompleted)
		{
		_LIT(KFinished,"Finished test case %d and test completed");
		RDebug::Print(KFinished,iCurTestCaseNumber);
		}
	else
		{
		if (iSelfDrive)
			{
			_LIT(KStarted,"Started test case %d");
			RDebug::Print(KStarted,iCurTestCaseNumber);
			}
		else
			{
			NextTest();
			}
		}
	}

void CTestManager::DoCancel()
	{
	if (!iSelfDrive)
		iTestCompleted = ETrue;
	}

TInt CTestManager::RunError(TInt aError)
	{
	TTime timeStamp;
	timeStamp.HomeTime();
	TBuf<50> dateStr;
	_LIT(KDateString3,"%-B%:0%J%:1%T%:2%S%:3 %Cms");
	timeStamp.FormatL(dateStr, KDateString3);
	_LIT(KTestLeft,"RunTestCaseL left of %S - Test Case Number: %d");
	RDebug::Print(KTestLeft, &dateStr, iCurTestCaseNumber);
	
	aError = iTestCase->RunTestCaseLeft(aError);
	FinishAllTestCases();
	return aError;
	}

/**
 	Initialize an active object and set it as active
 */
void CTestManager::StartTest()
	{
	TRequestStatus *pS= (&iStatus);
	User::RequestComplete(pS, 0);
	SetActive();
	}

/**
 Time to move on to the next test step
 */
void CTestManager::NextTest()
	{
	_LIT(KFinished,"Finished test case %d");
	RDebug::Print(KFinished,iCurTestCaseNumber);
	StartTest();
	}

/**
 Stop active scheduler, and quit a test step
 */
void CTestManager::FinishAllTestCases()
	{
	iTestCompleted = ETrue;
	CActiveScheduler::Stop();
	}

/**
 Controls wether the manager calls RunTestCaseL whenever the system is otherwise idol
 */
void CTestManager::SetSelfDrive(TBool aSelfDrive)
	{
	if (iSelfDrive!=aSelfDrive)
		{
		iSelfDrive = aSelfDrive;
		if (aSelfDrive)
			{
			if (IsActive())
				Cancel();
			}
		else
			{
			if (!IsActive() && !iTestCompleted)
				NextTest();
			}
		}
	}

/**
 Tells the manager this case is finished so you can call RunTestCaseL again
 */
void CTestManager::CaseComplete()
	{
	__ASSERT_DEBUG(!IsActive(), User::Panic(KGraphicsTestFrameworkPanic, ETestPanicAlreadyActive));
	if (iSelfDrive && !iTestCompleted)
		NextTest();
	}


//-------------

EXPORT_C const TDesC& CTGraphicsBase::ColorModeName(TDisplayMode aMode)
	{
	_LIT(KNone,"None");
	_LIT(KGray2,"Grey2");
	_LIT(KGray4,"Grey4");
	_LIT(KGray16,"Grey16");
	_LIT(KGray256,"Grey256");
	_LIT(KColor16,"Color16");
	_LIT(KColor256,"Color256");
	_LIT(KColor4K,"Color4K");
	_LIT(KColor64K,"Color64K");
	_LIT(KColor16M,"Color16M");
	_LIT(KColor16MU,"Color16MU");
	_LIT(KColor16MA,"Color16MA");
	_LIT(KColor16MAP,"Color16MAP");
	_LIT(KRgb,"RGB");
	_LIT(KUnknown,"Unknown");
	switch(aMode)
		{
	case ENone:
		return KNone;
	case EGray2:
		return KGray2;
	case EGray4:
		return KGray4;
	case EGray16:
		return KGray16;
	case EGray256:
		return KGray256;
	case EColor16:
		return KColor16;
	case EColor256:
		return KColor256;
	case EColor64K:
		return KColor64K;
	case EColor16M:
		return KColor16M;
	case ERgb:
		return KRgb;
	case EColor4K:
		return KColor4K;
	case EColor16MU:
		return KColor16MU;
	case EColor16MA:
		return KColor16MA;
	case EColor16MAP:
		return KColor16MAP;
	default:
		return KUnknown;
		}
	}

EXPORT_C const TDesC& CTGraphicsBase::RotationName(CFbsBitGc::TGraphicsOrientation aOrientation)
	{
	_LIT(K0,"Normal");
	_LIT(K90,"Rotated90");
	_LIT(K180,"Rotated180");
	_LIT(K270,"Rotated270");
	_LIT(KUnknown,"Unknown");
	switch(aOrientation)
		{
	case CFbsBitGc::EGraphicsOrientationNormal:
		return K0;
	case CFbsBitGc::EGraphicsOrientationRotated90:
		return K90;
	case CFbsBitGc::EGraphicsOrientationRotated180:
		return K180;
	case CFbsBitGc::EGraphicsOrientationRotated270:
		return K270;
	default:
		return KUnknown;
		}
	}

EXPORT_C void CTGraphicsBase::SaveScreenShotL(CFbsScreenDevice* aScdv)
	{
#ifdef __WINS__
	_LIT(KBitmapDrive, "c:");
#else
	_LIT(KBitmapDrive, "e:");
#endif
	_LIT(KBitmapPath, "\\screenshot.mbm");
	TDisplayMode dispMode = aScdv->DisplayMode();
	TSize scrSize = aScdv->SizeInPixels();
	CFbsBitmap* dstBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(dstBmp);
	User::LeaveIfError(dstBmp->Create(scrSize, dispMode));
	HBufC8* row = HBufC8::NewLC(scrSize.iWidth*4);
	TPtr8 prow = row->Des();
	for (TInt ii = 0; ii < scrSize.iHeight; ++ii)
		{
		aScdv->GetScanLine(prow, TPoint(0, ii), scrSize.iWidth, dispMode);
		dstBmp->SetScanLine(prow,ii);
		}
	CleanupStack::PopAndDestroy(row);
	TFileName mbmFile;
	mbmFile.Append(KBitmapDrive);
	mbmFile.Append(KBitmapPath);	
	User::LeaveIfError(dstBmp->Save(mbmFile));
	CleanupStack::PopAndDestroy(dstBmp);
	}

EXPORT_C CTGraphicsBase::CTGraphicsBase(CTestStep* aStep) :
			iStep(aStep)
	{}

EXPORT_C CTGraphicsBase::~CTGraphicsBase()
	{
	if(iTestManager)
		{
		iTestManager -> Cancel();
		}
	delete iTestManager;
	}

void CTGraphicsBase::InitializeL()
	{
	__ASSERT_DEBUG(iStep, User::Panic(KGraphicsTestFrameworkPanic, KErrBadHandle));
	iTestManager = CTestManager::NewL(this);
	}
	
/**
 	The function must be called after all test cases completed 
 */	
EXPORT_C void CTGraphicsBase::TestComplete()
	{
	if(iTestManager)
		{
		iTestManager -> FinishAllTestCases();
		}
	}

EXPORT_C void CTGraphicsBase::SetSelfDrive(TBool aSelfDrive)
	{
	if (iTestManager)
		iTestManager->SetSelfDrive(aSelfDrive);
	}

EXPORT_C void CTGraphicsBase::CaseComplete()
	{
	if (iTestManager)
		iTestManager->CaseComplete();
	}

EXPORT_C TInt CTGraphicsBase::RunTestCaseLeft(TInt aError)
	{
	_LIT(KRunTestCaseLLeft,"The RunTestCaseL left with %d");
	ERR_PRINTF2(KRunTestCaseLLeft,aError);
	iStep->SetTestStepResult(EFail);
	return KErrNone;
	}

/** Start a test cases loop */
void CTGraphicsBase::Execute()
	{
	__ASSERT_DEBUG(iTestManager, User::Panic(KGraphicsTestFrameworkPanic, KErrBadHandle));
	__ASSERT_DEBUG(CActiveScheduler::Current(), User::Panic(KGraphicsTestFrameworkPanic, KErrBadHandle));

	iTestManager -> StartTest();
	CActiveScheduler::Start();
	}
/**
	Reset test cases counter to a new value. Can be used for running the same consequence of test 
	cases with different initial parameters.
	aNewCurrentTestCase cannot be negative.
*/
EXPORT_C void CTGraphicsBase::ResetCounter(TInt aNewCurrentTestCase )
	{
	__ASSERT_DEBUG(aNewCurrentTestCase >= 0, User::Panic(KGraphicsTestFrameworkPanic, KErrArgument));
	iTestManager->iCurTestCaseNumber = aNewCurrentTestCase;
	}

//---------------------
/**
 Initialise the cleanup stack.
*/
void CTGraphicsStep::SetupCleanup(CTrapCleanup*& tc)
    {
	tc = CTrapCleanup::New();
	if (!tc)
		{
		User::Panic(_L("Out of memory"), KErrNoMemory);
		}

	TRAPD(r, 
		{
		for ( TInt ii = KTestCleanupStack; ii > 0; ii-- )
			CleanupStack::PushL( (TAny*)1 );
		TEST( r == KErrNone );
		CleanupStack::Pop( KTestCleanupStack );
		} );
	}

void CTGraphicsStep::LogHeapInfo(RWsSession& aWs, TBool aStart)
	{
	_LIT(KInfoStart,"Start");
	_LIT(KInfoEnd,"End");
	_LIT(KInfoCheckPassed,"WsHeap leak check passed");
	_LIT(KInfoCheckFailed,"Memory leak detected. Number of orphaned cells=%d.");
	_LIT(KInfoCheckFailed2,"See epocwindout for the address of the first orphaned cell.");
	_LIT(KInfoHeapSummary," WsHeap - Count=%d,Total=%d,Free=%d,Max free=%d");
	_LIT(KInfoDisabled,"Memory leak testing has not been enabled.");

	TPckgBuf<TWsDebugHeapInfo> heapInfo;
	aWs.DebugInfo(EWsDebugInfoHeap,heapInfo);
	TBuf<256> infoBuf;
	if (aStart)
		{
		infoBuf.Append(KInfoStart);
		}
	else
		{
		infoBuf.Append(KInfoEnd);
		TInt heapFailCount=aWs.DebugInfo(EWsDebugFetchCheckHeapResult);
		if (heapFailCount==KErrNone)
			{
			INFO_PRINTF1(KInfoCheckPassed);
			}
		else if (heapFailCount>0) // Negative error should be ignored as it means the check has not been enabled
			{
			TEST(0);
			ERR_PRINTF2(KInfoCheckFailed,heapFailCount);
			ERR_PRINTF1(KInfoCheckFailed2);
			}
		else
		    {
		    WARN_PRINTF1(KInfoDisabled);
		    }
		}
	infoBuf.AppendFormat(KInfoHeapSummary,heapInfo().iCount,heapInfo().iTotal,heapInfo().iAvailable,heapInfo().iLargestAvailable);
	INFO_PRINTF1(infoBuf);
	}

/**
 Main entry point from CTestStep class. 
 Creates cleanup stack, background window and launches a test
*/
EXPORT_C TVerdict CTGraphicsStep::doTestStepL()
	{
	TInt memFree;
	HAL::Get(HALData::EMemoryRAMFree, memFree);
	INFO_PRINTF2(_L("Test started - RAM free: %d"), memFree);	
	
	__UHEAP_MARK;
	CTrapCleanup* tc = NULL;
	SetupCleanup(tc);
	
	TInt err = RFbsSession::Connect();
	if(err != KErrNone)
		{
		FbsStartup();
		err = RFbsSession::Connect();

		if(err != KErrNone) 
			{
			User::Panic(_L("Font bitmap session connect"), err);
			}
		}
	RWsSession ws;
	err = ws.Connect();
	if (iLogHeapInfo && err==KErrNone)
		{
		ws.DebugInfo(EWsDebugSetCheckHeapOnDisconnectClient);
		LogHeapInfo(ws,ETrue);
		}
	TInt theId1 = NULL;
#ifdef __WINS__
	RWindowGroup winGroup = RWindowGroup(ws);
	RWindow theWindow(ws);
	if (err==KErrNone)
		{
		const TUint32 ENullWsHandle = 0xFFFFFFFF;	// Events delivered to this handle are thrown away
	//creat background window with the size of the screen
	//it is particulary important for tests which draw on the screen, read screen buffer
	//and compare result. By default top TEF window is concole and blinking cursor can interfere
	//with the work of the tests
		TSize size(640, 240); //some default value
	//retrieve screen size
		err = GetScreenSize(size);
		if(err)
			{
			WARN_PRINTF2(_L("Can't retrieve size from screen driver err = %d"), err);
			}

		winGroup.Construct(ENullWsHandle);
		winGroup.AutoForeground(ETrue);
		theId1 = winGroup.Identifier();
	
		theWindow.Construct(winGroup, ENullWsHandle);
		theWindow.SetExtent(TPoint(0, 0), size);
		theWindow.SetVisible(ETrue);
		theWindow.Activate();
	
	//the following trick we need to put the window on the top
		TApaTaskListFacade taskList(ws);
		TApaTaskFacade task = taskList.FindByPos(1);
		task.BringToForeground();
		TApaTaskFacade task1(ws);
		task1.SetWgId(theId1);
		task1.BringToForeground();
		}
#endif
	if (err==KErrNone)
		{
		CloseAllPanicWindows(theId1, ws);
		}

	// initialize the test and kick off the test cases loop
	TRAP(err, TestSetupL();  // initialize a test step
				ExecuteL()); // run active scheduler, in order to complete test execution 
							 // CTGraphicsStep::TestComplete function needs to be called

	if (err)
		{
		SetTestStepResult(EFail);
		}
		
	TestClose(); // free all allocated resources here
	
	if (ws.Handle())
		{
		CloseAllPanicWindows(theId1, ws);
#ifdef __WINS__	
		theWindow.Close();
		winGroup.Close();
#endif
		ws.Close();
		User::After(50000);	
		// Prev close will trigger Wserv dummy shutdown and heap check
		// We then re-connect to get the result of the shutdown.
		RWsSession ws2;
		TInt errConnection;
		TInt MaxNumberOfAttempt=10;
		TInt currentAttempt=0;		
		do
			{
			errConnection=ws2.Connect();		
			if (errConnection==KErrNone)
				{
				LogHeapInfo(ws2,EFalse);
				ws2.Close();
				}
			else
				{
				RDebug::Printf("Connection Error with Wserv... %i",errConnection);
				User::After(50000);		
				}
			currentAttempt++;
			}while( (errConnection!=KErrNone) && (currentAttempt<MaxNumberOfAttempt) );
		}
	RFbsSession::Disconnect();	

	delete tc;

	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

void CTGraphicsStep::CloseAllPanicWindows(TInt aId, RWsSession& ws) const
	{
	TInt idFocus = ws.GetFocusWindowGroup();
	TWsEvent event;
	event.SetType(EEventKey); //EEventKeyDown
	TKeyEvent *keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;
	TInt theLimit = 50;
	while(idFocus != aId && (theLimit-- > 0))
		{
		ws.SendEventToAllWindowGroups(event);
		idFocus = ws.GetFocusWindowGroup();
		}
	}
	
TInt CTGraphicsStep::GetScreenSize(TSize& aSize) const
	{
	CFbsScreenDevice* dev = NULL;
	
	TInt err = KErrNone;
	TInt sizeOfDisplayMode = sizeof (testDisplayModes) / sizeof(testDisplayModes[0]);
	 
	for(TInt theScreenModeIndex = sizeOfDisplayMode - 1; theScreenModeIndex ; theScreenModeIndex--)
		{
		TDisplayMode disp = testDisplayModes[theScreenModeIndex];
		TRAP(err, dev = CFbsScreenDevice::NewL(_L(""), disp)); //scdv
		if(err == KErrNone)
			{
			aSize = dev->SizeInPixels();
			delete dev;
			return KErrNone;
			}
		delete dev;
		dev = NULL;		
		}
	
	return err;		
	}

/** Installs an active scheduler and launches a test*/
void CTGraphicsStep::ExecuteL()
	{
	CActiveScheduler* theAs = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(theAs);
	CActiveScheduler::Install(theAs);

	CTGraphicsBase*	autoTest = CreateTestL();
	User::LeaveIfNull(autoTest);
	CleanupStack::PushL(autoTest);

	autoTest -> InitializeL();
	autoTest -> ConstructL();
	autoTest -> Execute();

	CleanupStack::PopAndDestroy(2, theAs);
	}

TInt  E32Dll( )
	{
	return 0;
	}

