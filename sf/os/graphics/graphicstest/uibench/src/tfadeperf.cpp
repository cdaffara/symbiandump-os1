// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#include "tfadeperf.h"

const TInt KIterationsToTest = 250; 		// Number of iterations to run tests

CTFadePerf::~CTFadePerf()
	{
	}

CTFadePerf::CTFadePerf()
	{
	SetTestStepName(KTFadePerfName);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTFadePerf::doTestStepPreambleL()
	{	
	User::LeaveIfError(iWs.Connect());
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
		
	return TestStepResult();
	}

TVerdict CTFadePerf::doTestStepPostambleL()
	{	
	iWs.Close();
		
	return TestStepResult();
	}

/**
	Override of base class pure virtual
	Our implementation only gets called if the base class doTestStepPreambleL() did
	not leave. That being the case, the current test result value will be EPass.
	
	@return - TVerdict code
*/
TVerdict CTFadePerf::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0108"));
	FadeOpaqueSingleWindowRepeatedlyL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0109"));
	FadeTransparentSingleWindowRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0110"));
	FadeTransparentSingleWindowRecordUnfadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0111"));
	FadeOpaqueSingleWindowRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0112"));
	FadeOpaqueSingleWindowRecordUnfadeL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0113"));
	FadeOpaqueWindowsBehindRepeatedlyL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0114"));
	FadeTransparentWindowsBehindRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0115"));
	FadeTransparentWindowsBehindRecordUnfadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0116"));
	FadeOpaqueWindowsBehindRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0117"));
	FadeOpaqueWindowsBehindRecordUnfadeL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0118"));
	SetSystemFadedRepeatedlyL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0119"));
	SetSystemFadedTransparentWindowsRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0120"));
	SetSystemFadedTransparentWindowsRecordUnfadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0121"));
	SetSystemFadedOpaqueWindowsRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0122"));
	SetSystemFadedOpaqueWindowsRecordUnfadeL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0123"));
	SetNonFadingRepeatedlyL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0124"));
	SetNonFadingTransparentWindowsRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0125"));
	SetNonFadingTransparentWindowsRecordUnfadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0126"));
	SetNonFadingOpaqueWindowsRecordFadeL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0127"));
	SetNonFadingOpaqueWindowsRecordUnfadeL();
	RecordTestResultL();
	return TestStepResult();
	}

/** Set up the window server environment in which the fading performance test cases are executed.
  	The RWsSession is connected to the window server, the CWsScreenDevice and the RWindowGroup
   	members are constructed.
 */
void CTFadePerf::SetUpWindowEnvironmentL()
	{
	SetScreenModeL(EColor16MA);
			
	iWsScreenDev = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iWsScreenDev->Construct());
	TSize screenSize = iWsScreenDev->SizeInPixels();
	
	iWinGroup = new(ELeave) RWindowGroup(iWs);
	User::LeaveIfError(iWinGroup->Construct(reinterpret_cast<TUint32>(iWinGroup),iWsScreenDev));
	}

/** Construct the single RWindow member and set its background colour according to the tranparency flag.
 	Make the window visible, activate it and draw it. 
  
    @param aUseTransparency the transparency flag of the window
 */
void CTFadePerf::ConstructSingleWindowL(TBool aUseTransparency)
	{
	iSingleWin = new(ELeave) RWindow(iWs);
	TUint32 winID = reinterpret_cast<TUint32>(iWinGroup+1);
	User::LeaveIfError(iSingleWin->Construct(*iWinGroup,winID));
	iSingleWin->SetExtent(TPoint(0,0),iWsScreenDev->SizeInPixels());
	
	if (aUseTransparency)
		{
		iSingleWin->SetTransparencyAlphaChannel();
		iSingleWin->SetBackgroundColor(TRgb(240,50,170,50)); //semi-transparent window (R,G,B,Alpha)
		}
	else
		{
		iSingleWin->SetBackgroundColor(TRgb(240,50,170,255)); //opaque window
		}
	
	iSingleWin->SetVisible(ETrue);
	iSingleWin->Activate();
	iSingleWin->Invalidate();
	iSingleWin->BeginRedraw();
	iSingleWin->EndRedraw();
	iWs.Flush();
	iWs.Finish();
	}

/** Populate the RWindow array member by constructing RWindow objcets. Set the background colour of the windows according to the
    tranparency flag. Set non-fading half of the windows according to the non-fade flag.
 	Make the windows visible, activate them and draw them.
  
    @param aUseTransparency the transparency flag of the windows 
    @param aNonFadeHalfWindows the non-fade flag of the windows 
 */
void CTFadePerf::ConstructArrayOfWindowsL(TBool aUseTransparency, TBool aNonFadeHalfWindows)
	{
	TRect rect(TPoint(0,0),iWsScreenDev->SizeInPixels());
	TInt i;
	TUint32 winID;
	for (i=0;i<ENumWins;++i)
		{
		iWins[i]=new(ELeave) RWindow(iWs);
		winID = reinterpret_cast<TUint32>(iWinGroup+i+1);
		if (i==0)
			{
			User::LeaveIfError(iWins[i]->Construct(*iWinGroup,winID)); //iWinGroup is the parent of iWins[0]
			}
		else
			{
			User::LeaveIfError(iWins[i]->Construct(*iWins[0],winID)); //iWins[0] is the the parent for every subsequent window
			}
		rect.Resize(-rect.Width()/10,-rect.Height()/10);
		iWins[i]->SetExtent(TPoint(0,0),rect.Size());
		if (aUseTransparency)
			{	
			iWins[i]->SetTransparencyAlphaChannel();
			iWins[i]->SetBackgroundColor(TRgb(240,(200*i)/ENumWins,170-(20*i)/ENumWins,(60*(i+1))/ENumWins)); //semi-transparent window (R,G,B,Alpha)
			}
		else
			{
			iWins[i]->SetBackgroundColor(TRgb(180,(30*i)/ENumWins,170-(40*i)/ENumWins,255)); //opaque window
			}
		
		if (aNonFadeHalfWindows)
			{
			if ((i%2)==0)  //every second window is set to non fading
				{
				iWins[i]->SetNonFading(ETrue);
				}
			}
			
		iWins[i]->SetVisible(ETrue);
		iWins[i]->Activate();	
		iWins[i]->Invalidate();
		iWins[i]->BeginRedraw();
		iWins[i]->EndRedraw();
		}
			
	iWs.Flush();
	iWs.Finish();
	}

/** Release the resources that the window server environment and the window construction has allocated.
 */
void CTFadePerf::ReleaseWindowsAndEnvironment(TBool aIsSingleWindow)
	{
	if (aIsSingleWindow)
		{
		if (iSingleWin)
			iSingleWin->Close();
		delete iSingleWin;
		iSingleWin=NULL;
		}
	else
		{
		for (TInt i=0;i<ENumWins;++i)
			{
			if (iWins[i])
				iWins[i]->Close();
			delete iWins[i];
			iWins[i]=NULL;
			}
		}
	
	if (iWinGroup)
		iWinGroup->Close();
	delete iWinGroup;
	iWinGroup=NULL;
	
	delete iWsScreenDev;
	iWsScreenDev=NULL;
	}

/**
   Fade single window test cases
 */ 

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0108

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade a single opaque window repeatedly without trying 
to unfade it first

@SYMTestActions
Set up the window server environment and construct an opaque window. Over a specific number
of iterations fade the window by calling RWindow::SetFaded() and record the time this action requires. 

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeOpaqueSingleWindowRepeatedlyL()
	{
	_LIT(KTestName, "FadeOpaqueSingleWindowRepeatedlyL");
	
	SetUpWindowEnvironmentL();
	ConstructSingleWindowL(EFalse);
	
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iSingleWin->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
	
	ReleaseWindowsAndEnvironment(ETrue);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0109

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade a single transparent window, when at each iteration the window is later unfaded

@SYMTestActions
Set up the window server environment and construct a transparent window. Over a specific number
of iterations fade the window by calling RWindow::SetFaded() and record the time this action requires.
The window is unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeTransparentSingleWindowRecordFadeL()
	{
	_LIT(KTestName, "FadeTransparentSingleWindowRecordFadeL");
		
	SetUpWindowEnvironmentL();
	ConstructSingleWindowL(ETrue);
		
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();		
		iSingleWin->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iSingleWin->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
	
	ReleaseWindowsAndEnvironment(ETrue);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0110

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to unfade a single transparent window, when at each iteration the window is faded first

@SYMTestActions
Set up the window server environment and construct a transparent window. Over a specific number
of iterations fade the window and successively unfade it by calling RWindow::SetFaded() and record the time the latter action requires.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeTransparentSingleWindowRecordUnfadeL()
	{
	_LIT(KTestName, "FadeTransparentSingleWindowRecordUnfadeL");
			
	SetUpWindowEnvironmentL();
	ConstructSingleWindowL(ETrue);
			
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{		
		iSingleWin->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		iProfiler->StartTimer();
		iSingleWin->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
		
	ReleaseWindowsAndEnvironment(ETrue);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0111

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade a single opaque window, when at each iteration the window is later unfaded

@SYMTestActions
Set up the window server environment and construct an opaque window. Over a specific number
of iterations fade the window by calling RWindow::SetFaded(). Record the time this action requires.
The window is unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeOpaqueSingleWindowRecordFadeL()
	{
	_LIT(KTestName, "FadeOpaqueSingleWindowRecordFadeL");
			
	SetUpWindowEnvironmentL();
	ConstructSingleWindowL(EFalse);
			
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();		
		iSingleWin->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iSingleWin->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
		
	ReleaseWindowsAndEnvironment(ETrue);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0112

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to unfade a single opaque window, when at each iteration the window is faded first

@SYMTestActions
Set up the window server environment and construct an opaque window. Over a specific number
of iterations fade the window and successively unfade it by calling RWindow::SetFaded(). Record the time the latter action requires.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeOpaqueSingleWindowRecordUnfadeL()
	{
	_LIT(KTestName, "FadeOpaqueSingleWindowRecordUnfadeL");
				
	SetUpWindowEnvironmentL();
	ConstructSingleWindowL(EFalse);
				
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{		
		iSingleWin->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		iProfiler->StartTimer();
		iSingleWin->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
		
	ReleaseWindowsAndEnvironment(ETrue);
	}

/**
   Fade behind test cases
 */

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0113

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade opaque windows behind another opaque window repeatedly without trying 
to unfade them first

@SYMTestActions
Set up the window server environment and construct an array of opaque windows. Over a specific number
of iterations fade the windows behind the one on top by calling RWindow::FadeBehind() and record the time this action requires. 

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeOpaqueWindowsBehindRepeatedlyL()
	{
	_LIT(KTestName, "FadeOpaqueWindowsBehindRepeatedlyL");
	
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, EFalse);
		
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWins[ENumWins-1]->FadeBehind(ETrue);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
		
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0114

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade transparent windows behind the window on top when at each iteration the windows are later unfaded.

@SYMTestActions
Set up the window server environment and construct an array of transparent windows. Over a specific number
of iterations fade the windows behind the one on top by calling RWindow::FadeBehind() and record the time this action requires. 
The windows are unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeTransparentWindowsBehindRecordFadeL()
	{
	_LIT(KTestName, "FadeTransparentWindowsBehindRecordFadeL");
		
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(ETrue, EFalse);
			
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWins[ENumWins-1]->FadeBehind(ETrue);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iWins[ENumWins-1]->FadeBehind(EFalse);
		iWs.Flush();
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
			
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0115

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to unfade transparent windows behind the window on top when at each iteration the windows are faded first

@SYMTestActions
Set up the window server environment and construct an array of transparent windows. Over a specific number
of iterations fade and unfade the windows behind the one on top by calling RWindow::FadeBehind(). Record the time the latter action requires. 
The windows are unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeTransparentWindowsBehindRecordUnfadeL()
	{
	_LIT(KTestName, "FadeTransparentWindowsBehindRecordUnfadeL");
			
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(ETrue, EFalse);
				
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iWins[ENumWins-1]->FadeBehind(ETrue);
		iWs.Flush();
		iWs.Finish();
		iProfiler->StartTimer();
		iWins[ENumWins-1]->FadeBehind(EFalse);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
				
	ReleaseWindowsAndEnvironment(EFalse);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0116

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade opaque windows behind the window on top, when at each iteration the windows are later unfaded.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows. Over a specific number
of iterations fade the windows behind the one on top by calling RWindow::FadeBehind() and record the time this action requires. 
The windows are unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeOpaqueWindowsBehindRecordFadeL()
	{
	_LIT(KTestName, "FadeOpaqueWindowsBehindRecordFadeL");
			
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, EFalse);
				
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWins[ENumWins-1]->FadeBehind(ETrue);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iWins[ENumWins-1]->FadeBehind(EFalse);
		iWs.Flush();
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
				
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0117

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to unfade opaque windows behind the window on top, when at each iteration the windows are faded first

@SYMTestActions
Set up the window server environment and construct an array of opaque windows. Over a specific number
of iterations fade and unfade the windows behind the one on top by calling RWindow::FadeBehind(). Record the time the latter action requires. 
The windows are unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::FadeOpaqueWindowsBehindRecordUnfadeL()
	{
	_LIT(KTestName, "FadeOpaqueWindowsBehindRecordUnfadeL");
				
		SetUpWindowEnvironmentL();
		ConstructArrayOfWindowsL(EFalse, EFalse);
					
		iProfiler->InitResults();
		for (TInt count=KIterationsToTest; count>=0; --count)
			{
			iWins[ENumWins-1]->FadeBehind(ETrue);
			iWs.Flush();
			iWs.Finish();
			iProfiler->StartTimer();
			iWins[ENumWins-1]->FadeBehind(EFalse);
			iWs.Flush();
			iWs.Finish();
			iProfiler->MarkResultSetL();
			}
		iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
					
		ReleaseWindowsAndEnvironment(EFalse);	
	}

/**
   SetSystemFaded test cases
 */

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0118

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to set the windows in the system faded repeatedly, without trying to unfade them first

@SYMTestActions
Set up the window server environment and construct an array of opaque windows. Half of them are set to non fading. 
Over a specific number of iterations set the system faded by calling RWsSession::SetSystemFaded() and record the time
this action requires. 

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetSystemFadedRepeatedlyL()
	{
	_LIT(KTestName, "SetSystemFadedRepeatedlyL");
						
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, ETrue);
							
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWs.SetSystemFaded(ETrue);
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
						
	ReleaseWindowsAndEnvironment(EFalse);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0119

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to set the windows in the system faded, when at each iteration the windows are later unfaded.

@SYMTestActions
Set up the window server environment and construct an array of tranparent windows. Half of them are set to non fading.
Over a specific number of iterations set the system faded by calling RWsSession::SetSystemFaded() and record the time this action requires. 
The system is set unfaded at the end of each iteration.


@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetSystemFadedTransparentWindowsRecordFadeL()
	{
	_LIT(KTestName, "SetSystemFadedTransparentWindowsRecordFadeL");
					
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(ETrue, ETrue);
						
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWs.SetSystemFaded(ETrue);
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iWs.SetSystemFaded(EFalse);
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
						
	ReleaseWindowsAndEnvironment(EFalse);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0120

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to set the windows in the system unfaded, when at each iteration the windows are faded first.

@SYMTestActions
Set up the window server environment and construct an array of transparent windows. Half of them are set to non fading.
Over a specific number of iterations set the system faded and successively unfaded by calling RWsSession::SetSystemFaded(). Record the time the latter action requires. 

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetSystemFadedTransparentWindowsRecordUnfadeL()
	{
	_LIT(KTestName, "SetSystemFadedTransparentWindowsRecordUnfadeL");
						
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(ETrue, ETrue);
							
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iWs.SetSystemFaded(ETrue);
		iWs.Finish();
		iProfiler->StartTimer();
		iWs.SetSystemFaded(EFalse);
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
							
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0121

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to set all the windows in the system faded, when at each iteration the windows are later unfaded.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows. Half of them are set to non fading.
Over a specific number of iterations set the system faded by calling RWsSession::SetSystemFaded(). Record the time the latter action requires. 
The system is set unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetSystemFadedOpaqueWindowsRecordFadeL()
	{
	_LIT(KTestName, "SetSystemFadedOpaqueWindowsRecordFadeL");
							
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, ETrue);
								
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWs.SetSystemFaded(ETrue);
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iWs.SetSystemFaded(EFalse);
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
								
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0122

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to set the windows in the system unfaded, when at each iteration the windows are faded first.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows. Half of them are set to non fading.
Over a specific number of iterations set the system faded and successively unfaded by calling RWsSession::SetSystemFaded(). Record the time the latter action requires. 

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetSystemFadedOpaqueWindowsRecordUnfadeL()
	{
	_LIT(KTestName, "SetSystemFadedOpaqueWindowsRecordUnfadeL");
							
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, ETrue);
								
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iWs.SetSystemFaded(ETrue);
		iWs.Finish();
		iProfiler->StartTimer();
		iWs.SetSystemFaded(EFalse);
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
								
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
   SetNonFading test cases
 */

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0123

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to set a parent window and its children faded repeatedly, without trying to unfade them first.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows, so that the first window is the parent for all the other windows. 
Half of them are set to non fading. Over a specific number of iterations set the parent window faded including its children by calling RWindow::SetFaded().
Record the time this action requires. 

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetNonFadingRepeatedlyL()
	{
	_LIT(KTestName, "SetNonFadingRepeatedlyL");
							
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, ETrue);
								
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWins[0]->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
							
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0124

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade a parent window and its children, when at each iteration they are later unfaded.

@SYMTestActions
Set up the window server environment and construct an array of transparent windows, so that the first window is the parent for all the other windows. 
Half of them are set to non fading. Over a specific number of iterations set the parent window faded including its children by calling RWindow::SetFaded().
Record the time this action requires. The parent and children windows are unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetNonFadingTransparentWindowsRecordFadeL()
	{
	_LIT(KTestName, "SetNonFadingTransparentWindowsRecordFadeL");
											
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(ETrue, ETrue);
												
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWins[0]->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iWins[0]->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
												
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0125

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to unfade a parent window and its children, when at each iteration they are first faded.

@SYMTestActions
Set up the window server environment and construct an array of transparent windows, so that the first window is the parent for all the other windows. 
Half of them are set to non fading. Over a specific number of iterations set the parent window faded and successively unfaded including its children
by calling RWindow::SetFaded(). Record the time the latter action requires.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetNonFadingTransparentWindowsRecordUnfadeL()
	{
	_LIT(KTestName, "SetNonFadingTransparentWindowsRecordUnfadeL");
									
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(ETrue, ETrue);
										
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iWins[0]->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		iProfiler->StartTimer();
		iWins[0]->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
										
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0126

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to fade an parent window and its children, when at each iteration they are later unfaded.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows, so that the first window is the parent for all the other windows. 
Half of them are set to non fading. Over a specific number of iterations set the parent window faded including its children by calling RWindow::SetFaded().
Record the time this action requires. The parent and children windows are unfaded at the end of each iteration.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetNonFadingOpaqueWindowsRecordFadeL()
	{
	_LIT(KTestName, "SetNonFadingOpaqueWindowsRecordFadeL");
										
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, ETrue);
											
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		iWins[0]->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iWins[0]->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
											
	ReleaseWindowsAndEnvironment(EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0127

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to unfade a parent window and its children, when at each iteration they are first faded.

@SYMTestActions
Set up the window server environment and construct an array of transparent windows, so that the first window is the parent for all the other windows. 
Half of them are set to non fading. Over a specific number of iterations set the parent window faded and successively unfaded including its children
by calling RWindow::SetFaded(). Record the time the latter action requires.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTFadePerf::SetNonFadingOpaqueWindowsRecordUnfadeL()
	{
	_LIT(KTestName, "SetNonFadingOpaqueWindowsRecordUnfadeL");
								
	SetUpWindowEnvironmentL();
	ConstructArrayOfWindowsL(EFalse, ETrue);
									
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iWins[0]->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		iProfiler->StartTimer();
		iWins[0]->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(KTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
									
	ReleaseWindowsAndEnvironment(EFalse);
	}
