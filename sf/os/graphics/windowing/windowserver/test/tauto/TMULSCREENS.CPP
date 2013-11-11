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
// Enable Multiple Displays
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TMULSCREENS.h"

const TInt KFstScreenNo = 0;
const TInt KSndScreenNo = 1;

CTMulScreens::CTMulScreens(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

void CTMulScreens::ConstructL()
	{
	User::LeaveIfError(iRws.Connect());
	iFstScreenDevice =new(ELeave) CWsScreenDevice(iRws);
	TInt err = iFstScreenDevice->Construct(KFstScreenNo);
	iTest->LogLeave(err);
	User::LeaveIfError(err);
	iNumScreens = iRws.NumberOfScreens();
	if(KSndScreenNo<iNumScreens)
		{
		iSndScreenDevice =new(ELeave) CWsScreenDevice(iRws);
		err = iSndScreenDevice->Construct(KSndScreenNo);
		iTest->LogLeave(err);
		User::LeaveIfError(err);
		}
	INFO_PRINTF2(_L("The number of screens supported on this device is %d"),iNumScreens);
	}

CTMulScreens::~CTMulScreens()
	{
	iFstWinGp.Close();
	if(KSndScreenNo<iNumScreens)
		{
		iSndWinGp.Close();
		delete iSndScreenDevice;
		}
	delete iFstScreenDevice;
	iRws.Close();
	}

/**
   @SYMTestCaseID          GRAPHICS-WSERV-0383

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDesc			Create Window Group(s) on each screen.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Call RWsSession::NumWindowGroups() to get the number of window groups of a given window group priority running on a specified screen.
   							Test the number of Window Groups on each screen.
   							Test the size of the list of window group on each screen.
   							Test the Window Group which has the keyboard focus on the second screen.
							API Calls:\n
							TInt RWsSession::NumWindowGroups(TInt aScreenNumber,TInt aPriority) const
							TInt RWsSession::WindowGroupList(CArrayFixFlat<TInt>* aWindowList,TInt aScreenNumber,TInt aPriority)
							TInt RWindowGroup::Construct(TUint32 aClientHandle,CWsScreenDevice* aScreenDevice)
							TInt RWsSession::GetFocusWindowGroup(TInt aScreenNumber)

   @SYMTestExpectedResults	Provided that a second screen is configured in the epoc.ini and the wsini.ini, this test case will create one window group on the first screen
   							and two window groups on the second screen.
							If only one screen is configured, then it will create one group on the primary screen.
							Otherwise this function leaves with a system-wide error.
*/

void CTMulScreens::TestCreateGroupWindowsL()
	{
	const TInt fstNumWinGps = iRws.NumWindowGroups(KFstScreenNo,EAllPriorities);
	CArrayFixFlat<TInt>* fstList = new(ELeave) CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(fstList);
	TInt err = iRws.WindowGroupList(fstList,KFstScreenNo,EAllPriorities);
	TEST(err==KErrNone);
	TEST(fstList->Count()==fstNumWinGps);
	err = iRws.WindowGroupList(fstList); //test existing api
	TEST(err==KErrNone);
	TEST(fstList->Count()==(iRws.NumWindowGroups())); 
	iFstWinGp = RWindowGroup(iRws);
	TRAP(err,iFstWinGp.Construct(ENullWsHandle,iFstScreenDevice));
	TEST(err==KErrNone);
	if(err==KErrNone)
		{
		TEST(iRws.NumWindowGroups(KFstScreenNo,EAllPriorities) == (fstNumWinGps+1));
		}
	CleanupStack::PopAndDestroy(fstList);

	//Second screen has been configured.
	if(KSndScreenNo<iNumScreens)
		{
		TInt winId = iRws.GetFocusWindowGroup(KSndScreenNo);
		TEST(winId==KErrGeneral);
		iSndWinGp = RWindowGroup(iRws);
		TRAP(err,iSndWinGp.Construct(ENullWsHandle,ETrue,iSndScreenDevice));
		TEST(err==KErrNone);
		if(err==KErrNone)
			{
			winId = iRws.GetFocusWindowGroup(KSndScreenNo);
			TEST(winId==iSndWinGp.Identifier());
			TEST(iRws.NumWindowGroups(KFstScreenNo,EAllPriorities) == (fstNumWinGps+1)); //test that the no. of screens unchanged on first screen
			CArrayFixFlat<TInt>* sndList =new(ELeave) CArrayFixFlat<TInt>(1);
			CleanupStack::PushL(sndList);
			TInt sndNumWinGps = iRws.NumWindowGroups(KSndScreenNo,iSndWinGp.OrdinalPriority());
			err = iRws.WindowGroupList(sndList,KSndScreenNo,iSndWinGp.OrdinalPriority());
			TEST(err==KErrNone);
			TEST(sndList->Count()==sndNumWinGps);

			iSndWinGp.SetOrdinalPosition(iSndWinGp.OrdinalPosition(),EAllPriorities);
			TInt allPriNumWinGps= iRws.NumWindowGroups(EAllPriorities);
			CArrayFixFlat<TInt>* allPriList =new(ELeave) CArrayFixFlat<TInt>(1);
			CleanupStack::PushL(allPriList);
			err = iRws.WindowGroupList(EAllPriorities,allPriList);
			TEST(err==KErrNone);
			TEST(allPriList->Count()==allPriNumWinGps);

			CleanupStack::PopAndDestroy(2,sndList);
			}
		RWindowGroup trdWinGp = RWindowGroup(iRws);
		TRAP(err,trdWinGp.Construct(ENullWsHandle,ETrue,iSndScreenDevice));
		TEST(err==KErrNone);
		if(err==KErrNone)
			{
			winId = iRws.GetFocusWindowGroup(KSndScreenNo);
			TEST(winId==trdWinGp.Identifier());
			trdWinGp.Close();
			}
		}
	}

/**
   @SYMTestCaseID          GRAPHICS-WSERV-0384

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDependencies	SYMTestCaseID GRAPHICS-WSERV-0383

   @SYMTestCaseDesc			Create a blank window on each screen.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Test that each blank window is created on a different screen.
							API Calls:\n
							TInt RBlankWindow::Construct(const RWindowTreeNode &parent, TUint32 aClientHandle) where parent is
							a Window Group created on each screen.

   @SYMTestExpectedResults	The background colour of the first screen changes to blue and that of the second one changes to green.
   							Otherwise this function leaves with a system-wide error.
*/
void CTMulScreens::TestCreateBlankWindowsL()
	{
	RBlankWindow fstBlankWin(iRws);
	CleanupClosePushL(fstBlankWin);
	TRAPD(err,fstBlankWin.Construct(iFstWinGp,ENullWsHandle));
	TEST(err==KErrNone);
	if(err==KErrNone)
		{
		fstBlankWin.SetRequiredDisplayMode(EColor256);
		fstBlankWin.SetColor(TRgb(0,0,255)); // paint the screen blue
		fstBlankWin.Activate();
		iRws.Flush();
		TheClient->WaitForRedrawsToFinish();
		User::After(6000000);
		}
	CleanupStack::PopAndDestroy();//fstBlankWin	
	
	// Second screen
	if(KSndScreenNo<iNumScreens)
		{
		RBlankWindow sndBlankWin(iRws);
		CleanupClosePushL(sndBlankWin);
		TRAP(err,sndBlankWin.Construct(iSndWinGp,ENullWsHandle));
		TEST(err==KErrNone);
		if(err==KErrNone)
			{
			sndBlankWin.SetRequiredDisplayMode(EColor256);
			sndBlankWin.SetColor(TRgb(0,255,0));	//paint the screen green
			sndBlankWin.Activate(); //there is a defect in Activate() because there is a delay before the second screen turns green
			iRws.Flush();
			TheClient->WaitForRedrawsToFinish();
			User::After(6000000);
			}
		CleanupStack::PopAndDestroy(); //sndBlankWin
		}
	}


/**
   @SYMTestCaseID          GRAPHICS-WSERV-0385

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDependencies	SYMTestCaseID GRAPHICS-WSERV-0383

   @SYMTestCaseDesc			Test the Window Group that has the keyboard focus for each screen.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Call RWsSession::GetFocusWindowGroup on each screen.
							API Calls:\n
							TInt RWsSession::GetFocusWindowGroup(TInt aScreenNumber)

   @SYMTestExpectedResults	The identifier returned by the API for each screen is tested to see if it is the expected Window Group ID; ie the
   							ID of the window group created in the first test case.
*/
void CTMulScreens::TestGetFocusWindow()
	{
	TInt winId = iRws.GetFocusWindowGroup(KFstScreenNo);
	TEST(winId==iFstWinGp.Identifier());
	if(KSndScreenNo<iNumScreens)
		{
		winId = iRws.GetFocusWindowGroup(KSndScreenNo);
		TEST(winId==iSndWinGp.Identifier());
		TEST(iRws.GetFocusWindowGroup()==iFstWinGp.Identifier());
		}
	}

/**
   @SYMTestCaseID          GRAPHICS-WSERV-0386

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDependencies	SYMTestCaseID GRAPHICS-WSERV-0383

   @SYMTestCaseDesc			Test the default owning window on each screen.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Call RWsSession::GetDefaultOwningWindow() to return the ID of the default owning window.
   							Call RWsSession::DefaultOwningWindow() on the second Window Group to set it as the default owning window on the second screen.
   							Call new API RWsSession::GetDefaultOwningWindow(TInt aScreenNumber) to get the default owning window on the second screen.
   							API Calls:\n
							TInt RWsSession::GetFocusWindowGroup()
							TInt RWsSession::GetFocusWindowGroup(TInt aScreenNumber)

   @SYMTestExpectedResults	The identifier returned by the API for each screen is tested to see that they are not the same.
*/
void CTMulScreens::TestGetDefaultOwningWindow()
	{
	if(KSndScreenNo<iNumScreens)
		{
		TInt prevWinId = iRws.GetDefaultOwningWindow();
		iSndWinGp.DefaultOwningWindow();
		TInt winId = iRws.GetDefaultOwningWindow(KSndScreenNo);
		TEST(winId==iSndWinGp.Identifier());
		TEST(prevWinId!=winId);
		}
	}

/**
   @SYMTestCaseID          GRAPHICS-WSERV-0387

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDesc			Test the background colour.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Call RWsSession::GetBackgroundColor() to store the default value;Change the background colour by calling RWsSession::SetBackgroundColor();
   							Test that the background has changed; Restore the background colour.
   							API Calls:\n
   							TRgb RWsSession::GetBackgroundColor() const
							void RWsSession::SetBackgroundColor(TRgb colour)

   @SYMTestExpectedResults	 Background color should change when SetBackgroundColor is called.
*/
void CTMulScreens::TestSetBackgroundColour()
	{
	TRgb rgb_b4 = iRws.GetBackgroundColor();
	iRws.SetBackgroundColor(KRgbBlack);
	TEST(iRws.GetBackgroundColor()==KRgbBlack);
	iRws.SetBackgroundColor(rgb_b4);
	TEST(iRws.GetBackgroundColor()==rgb_b4);
	}

/**
   @SYMTestCaseID          GRAPHICS-WSERV-0388

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDesc			Test the shadow vector.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Call RWsSession::ShadowVector() to store the default value;Call RWsSession::SetShadowVector() to change the shadow vector;
   							Test that the shadow vector has changed; Restore the shadow vector.
   							API Calls:\n
   							TPoint RWsSession::ShadowVector() const
							void RWsSession::SetShadowVector(const TPoint &aVector);

   @SYMTestExpectedResults	 The shadow vector should change when SetShadowVector is called.
*/
void CTMulScreens::TestSetShadowVector()
	{
	TPoint point_b4 = iRws.ShadowVector();
	
	iRws.SetShadowVector(TPoint(3,3));
	
	#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	TEST(iRws.ShadowVector()==TPoint(0,0)); // in NGA SetShadowVector & ShadowVector are deprecated
	#else
	TEST(iRws.ShadowVector()==TPoint(3,3));
	#endif
	
	iRws.SetShadowVector(point_b4);
	
	#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	TEST(iRws.ShadowVector()==TPoint(0,0)); // in NGA SetShadowVector & ShadowVector are deprecated
	#else
	TEST(iRws.ShadowVector()==point_b4);
	#endif
	}

/**
   @SYMTestCaseID          GRAPHICS-WSERV-0389

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDesc			Negative tests for the APIs listed below.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Launch a thread to call TestInvalidScreenNumber. Pass an invalid screen number to the APIs listed below.
   							API Calls:\n
   							TInt RWsSession::NumWindowGroups(TInt aScreenNumber,TInt aPriority) const
							TInt RWsSession::WindowGroupList(CArrayFixFlat<TInt>* aWindowList,TInt aScreenNumber,TInt aPriority)
							TInt RWsSession::GetFocusWindowGroup(TInt aScreenNumber)
							TInt GetDefaultOwningWindow(TInt aScreenNumber)
							TDisplayMode GetDefModeMaxNumColors(TInt aScreenNumber,TInt& aColor,TInt& aGray) const
   							TInt GetColorModeList(TInt aScreenNumber,CArrayFixFlat<TInt>* aModeList) const

   @SYMTestExpectedResults	 The thread panics and exits with reason EWservPanicScreenNumber.
*/
void CTMulScreens::TestPanicsL()
	{
	TInt firstTest = 1;
	TInt lastTest = 6;
	const TInt KInvalidScreenNumber = 2;
	for (TInt option=firstTest;option<=lastTest;option++)
		{
		TEST(iTest->TestWsPanicL(&TestInvalidScreenNumberL,EWservPanicScreenNumber,option,(TAny*)KInvalidScreenNumber));
		}
	}

TInt CTMulScreens::TestInvalidScreenNumberL(TInt aOption, TAny *aScreenNumber)
	{
	RWsSession wsSession;
	wsSession.Connect();
	switch((TInt)aOption)
		{
		case 1:
			wsSession.NumWindowGroups((TInt)aScreenNumber,EAllPriorities);
			break;
		case 2:
			{
			CArrayFixFlat<TInt>* list = new(ELeave) CArrayFixFlat<TInt>(1);
			wsSession.WindowGroupList(list,(TInt)aScreenNumber,EAllPriorities);
			}
			break;
		case 3:
			wsSession.GetFocusWindowGroup((TInt)aScreenNumber);
			break;
		case 4:
			wsSession.GetDefaultOwningWindow((TInt)aScreenNumber);
			break;
		case 5:
			{
			CArrayFixFlat<TInt>* list = new(ELeave) CArrayFixFlat<TInt>(1);
			wsSession.GetColorModeList((TInt)aScreenNumber,list);
			}
			break;
		case 6:
			{
			TInt color,gray;
			wsSession.GetDefModeMaxNumColors((TInt)aScreenNumber,color,gray);
			}
			break;
		default:
			User::Panic(_L("Default panic"),KErrGeneral);
			break;
		}
	return KErrNone;
	}


/**
   @SYMTestCaseID          GRAPHICS-WSERV-0390

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDependencies	SYMTestCaseID GRAPHICS-WSERV-0383

   @SYMTestCaseDesc			Test that CWindowGc updates its screendevice each time it is activated on a window.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Activate the gc on both RWindows.CWindowGc::Device() is called after each activation has taken place.
   							API Calls:\n
   							void CWindowGc::Activate(RDrawableWindow &aDevice);
   							void CWindowGc::Deactivate();
   							CGraphicsDevice* CWindowGc::Device() const
   							TInt RWindow::Construct(const RWindowTreeNode &parent,TUint32 aHandle);

   @SYMTestExpectedResults	CWindowGc::Device() returns the screendevice on which the gc was last activated.
*/
void CTMulScreens::TestDeviceL()
	{
	RWindow fstWin(iRws);
	User::LeaveIfError(fstWin.Construct(iFstWinGp,ENullWsHandle));
	CleanupClosePushL(fstWin);
	fstWin.Activate();

	CWindowGc* gc=new (ELeave) CWindowGc(iFstScreenDevice);
	User::LeaveIfError(gc->Construct());
	CleanupStack::PushL(gc);
	gc->Activate(fstWin);

	TEST((CWsScreenDevice*)gc->Device()==iFstScreenDevice);
	gc->Deactivate();

	if(KSndScreenNo<iNumScreens)
		{
		RWindow sndWin(iRws);
		User::LeaveIfError(sndWin.Construct(iSndWinGp,ENullWsHandle));
		CleanupClosePushL(sndWin);
		sndWin.Activate();

		gc->Activate(sndWin);
		TEST((CWsScreenDevice*)gc->Device()==iSndScreenDevice);
		gc->Deactivate();
		CleanupStack::PopAndDestroy();
		}

	CleanupStack::PopAndDestroy(2,&fstWin);
	}

/**
   @SYMTestCaseID          GRAPHICS-WSERV-0034

   @SYMPREQ					PREQ1227

   @SYMREQ					REQ5541

   @SYMTestCaseDesc			Test that the order of creating a screen device and window group does not matter.

   @SYMTestPriority			High

   @SYMTestStatus			Implemented

   @SYMTestActions			Create a window group before creating the screen device. Create a graphics context and call activate on it.
   							API Calls:\n
   							TInt RWindowGroup::Construct(TUint32 aClientHandle)
   							TInt CWsScreenDevice::Construct()
    						void CWindowGc::Activate(RDrawableWindow &aDevice);
   							void CWindowGc::Deactivate();
   							TInt RWindow::Construct(const RWindowTreeNode &parent,TUint32 aHandle);

   @SYMTestExpectedResults	 The test code does not panic with EWservPanicGroupWinScreenDeviceDeleted
*/
void CTMulScreens::TestInitaliseScreenDeviceL()
	{
	RWsSession rws1;
	User::LeaveIfError(rws1.Connect());
	CleanupClosePushL(rws1);

	RWindowGroup gw1(rws1);
	User::LeaveIfError(gw1.Construct(ENullWsHandle));
	CleanupClosePushL(gw1);

	CWsScreenDevice* screen1 = new (ELeave) CWsScreenDevice(rws1);
	User::LeaveIfError(screen1->Construct());
	CleanupStack::PushL(screen1);

	RWindow win1(rws1);
	User::LeaveIfError(win1.Construct(gw1,ETrue));
	CleanupClosePushL(win1);
	win1.Activate();

	CWindowGc* gc=new (ELeave) CWindowGc(screen1);
	User::LeaveIfError(gc->Construct());
	CleanupStack::PushL(gc);
	gc->Activate(win1);

	CleanupStack::PopAndDestroy(5,&rws1);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0492

@SYMDEF             PDEF126432

@SYMTestCaseDesc    Test the screen number that a window is located on

@SYMTestPriority    Medium

@SYMTestStatus      Implemented

@SYMTestActions     Create two windows on two screens respectively 
					and check the screen number that each window is located on

@SYMTestExpectedResults The screen numbers should match the expected ones
*/
void CTMulScreens::TestScreenNumbersOfWindowsL()
	{
	// First screen
	RWindow fstWin(iRws);
	CleanupClosePushL(fstWin);
	TRAPD(err,fstWin.Construct(iFstWinGp,ENullWsHandle));
	TEST(err==KErrNone);
	if(err==KErrNone)
		{
		fstWin.Activate();
		iRws.Flush();
		}
	TEST(fstWin.ScreenNumber()==KFstScreenNo);
	CleanupStack::PopAndDestroy();//fstWin	
	
	// Second screen
	if(KSndScreenNo<iNumScreens)
		{
		RWindow sndWin(iRws);
		CleanupClosePushL(sndWin);
		TRAP(err,sndWin.Construct(iSndWinGp,ENullWsHandle));
		TEST(err==KErrNone);
		if(err==KErrNone)
			{
			sndWin.Activate(); 
			iRws.Flush();
			}
		TEST(sndWin.ScreenNumber()==KSndScreenNo);
		CleanupStack::PopAndDestroy(); //sndWin
		}
	}	
	
void CTMulScreens::RunTestCaseL(TInt aCurTestCase)
	{
	_LIT(KTest0,"Create a window group on each screen");
	_LIT(KTest1,"Create a blank window on each screen");
	_LIT(KTest2,"Get focus window");
	_LIT(KTest3,"Get default owning window");
	_LIT(KTest4,"Change background colour");
	_LIT(KTest5,"Change shadow vector");
	_LIT(KTest6,"Panic Tests");
	_LIT(KTest7,"Test device pointer returned by GC");
	_LIT(KTest8,"Initialise ScreenDevice");
	_LIT(KTest9,"Test screen numbers that windows are located on");
	((CTMulScreensStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(aCurTestCase)
		{
		case 1:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0383"));
			INFO_PRINTF1(KTest0);
			TestCreateGroupWindowsL();
			break;
		case 2:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0384"));
			INFO_PRINTF1(KTest1);
			TestCreateBlankWindowsL();
			break;
		case 3:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0385"));
			INFO_PRINTF1(KTest2);
			TestGetFocusWindow();
		case 4:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0386"));
			INFO_PRINTF1(KTest3);
			TestGetDefaultOwningWindow();
			break;
		case 5:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0387"));
			INFO_PRINTF1(KTest4);
			TestSetBackgroundColour();
			break;
		case 6:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0388"));
			INFO_PRINTF1(KTest5);
			TestSetShadowVector();
			break;
		case 7:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0389"));
			INFO_PRINTF1(KTest6);
			TestPanicsL();
			break;
		case 8:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0390"));
			INFO_PRINTF1(KTest7);
			TestDeviceL();
			break;
		case 9:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0034"));
			INFO_PRINTF1(KTest8);
			TestInitaliseScreenDeviceL();
			break;
		case 10:
			((CTMulScreensStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0492"));
			INFO_PRINTF1(KTest9);
			TestScreenNumbersOfWindowsL();
		case 11:
			((CTMulScreensStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTMulScreensStep*)iStep)->CloseTMSGraphicsStep();
			INFO_PRINTF1(_L("All tests completed.\n"));
			TestComplete();
			break;
		default:
			((CTMulScreensStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTMulScreensStep*)iStep)->CloseTMSGraphicsStep();
			INFO_PRINTF1(_L("CTMulScreens::RunTestCaseL default case\n"));
			break;
		}
	((CTMulScreensStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(MulScreens)
