// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Screen mode changing tests
// 
//

#include <e32std.h>
#include <e32def_private.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class CTimeWindow;

class CQBlankWindow : public CBlankWindow
	{
public:
	static TInt iInitialColor;
public:
	//Virtual function from CTBaseWin
	void InitWin();
public:
	static TSize iSize;
	TSglQueLink iLink;
	};

class CFixScreenModeGroup : public CTWindowGroup
	{
public:
	static TInt iNoClientWins;
public:
	~CFixScreenModeGroup();
	CFixScreenModeGroup(CTClient *aClient);
	void ConstructL();
public:
	TSglQueLink iLink;
private:
	TSglQue<CQBlankWindow> iWindows;
	};

class CFlipClient : public CTClient
	{
public:
	void ConstructL();
private:
	};

class CScreenModeTimes : public CTestBase
	{
public:
	CScreenModeTimes();
	~CScreenModeTimes();
	TestState DoTestL();
	void ConstructL();
	void DoTimings();
private:
	void CreateGroupL(CTClient* aClient,TSglQue<CFixScreenModeGroup>& aQue);
	void DeleteGroups(TSglQue<CFixScreenModeGroup>& aQue);
	void BringToFront(TSglQue<CFixScreenModeGroup>& aQue);
private:
	TScreenModeEnforcement iEnforcementMode;
	CInfoDialog* iDialogue;
	CTimeWindow* iInfoWindow;
	CFlipClient* iFlipClient;
	TSglQue<CFixScreenModeGroup> iNormalGroupQue;
	TSglQue<CFixScreenModeGroup> iFlipGroupQue;
	TInt iNoGroupWins;
	TInt iState;
	};

class CTimeWindow : public CTTitledWindow
	{
public:
	void Construct(CTestBase* aTest);
	void SetNumWindows(TInt aGroup,TInt aWin);
	void SetTime1(TTimeIntervalMicroSeconds& aTime);
	void SetTime2(TTimeIntervalMicroSeconds& aTime);
	void DoDraw();
	//virtual function from CTWinBase overridden by CTBaseWin
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	//virtual functions from CTBaseWin
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
	//virtual function from CTBaseWin overridden by CTTitledWindow
	void Draw();
private:
	inline CScreenModeTimes* Test() {return STATIC_CAST(CScreenModeTimes*,iTest);}
	void UpdateString1();
	void UpdateString2();
	void UpdateString3();
private:
	CTestBase* iTest;
	TInt iNumGroups;
	TInt iNumWins;
	TTimeIntervalMicroSeconds iTime1;
	TTimeIntervalMicroSeconds iTime2;
	TBuf<32> iString1;
	TBuf<32> iString2;
	TBuf<32> iString3;
	TBuf<32> iString4;
	TRect iRect1;
	TRect iRect2;
	TRect iRect3;
	TRect iRect4;
	TBool iPressed;
	TBool iExit;
	};


TSize CQBlankWindow::iSize;
TInt CQBlankWindow::iInitialColor;
TInt CFixScreenModeGroup::iNoClientWins=0;


GLDEF_C CTestBase *CreateScreenModeTimes()
	{
	return(new(ELeave) CScreenModeTimes());
	}


//
// CFlipClient
//

void CFlipClient::ConstructL()
	{
	CTClient::ConstructL();
	TPixelsAndRotation sizeAndRotation;
	iScreen->GetScreenModeSizeAndRotation(1,sizeAndRotation);
	iScreen->SetScreenSizeAndRotation(sizeAndRotation);
	}


//
// CTimeWindow
//

void CTimeWindow::Construct(CTestBase* aTest)
	{
	_LIT(Title,"StatusWindow");
	_LIT(String4,"Click here to finish");
	const TInt fontHeight=iFont->HeightInPixels();
	iTest=aTest;
	TWindowTitle title(Title);
	SetTitle(title);
	TInt yy=iTitleHeight+fontHeight+7;
	iRect1.SetRect(3,iTitleHeight+5,iSize.iWidth-1,yy);
	yy+=fontHeight/2;
	TInt yy1=yy+fontHeight+2;
	iRect2.SetRect(3,yy+3,iSize.iWidth-1,yy1);
	TInt yy2=yy1+iFont->HeightInPixels()+2;
	iRect3.SetRect(3,yy1+3,iSize.iWidth-1,yy2);
	yy2+=fontHeight/2;
	iRect4.SetRect(3,yy2+3,iSize.iWidth-1,yy2+iFont->HeightInPixels()+2);
	//
	iString4.Copy(String4);
	iExit=EFalse;
	}

void CTimeWindow::SetUpL(TPoint aPos,TSize aSize,CTWinBase *aParent,CWindowGc &aGc)
	{
	CTTitledWindow::SetUpL(aPos,aSize,aParent,aGc);
	}

void CTimeWindow::WinKeyL(const TKeyEvent &aKey,const TTime&)
	{
	switch(aKey.iCode)
		{
		case EKeyEscape:
			iTest->Request();
			break;
		default:;
		}
	}

void CTimeWindow::PointerL(const TPointerEvent &aPointer,const TTime &aTime)
	{
	if (iRect2.Contains(aPointer.iPosition) || iRect3.Contains(aPointer.iPosition))
		{
		if (aPointer.iType==TPointerEvent::EButton1Down)
			iPressed=ETrue;
		}
	else if (iRect4.Contains(aPointer.iPosition))
		iExit=ETrue;
	else
		CTTitledWindow::PointerL(aPointer,aTime);
	if ((iExit||iPressed) && aPointer.iType==TPointerEvent::EButton1Up)
		{
		if (iExit)
			iTest->Request();
		else
			{
			Test()->DoTimings();
			DoDraw();
			}
		}
	}

void CTimeWindow::Draw()
	{
	CTTitledWindow::Draw();
	TInt ascent=iFont->AscentInPixels()+1;
	iGc->DrawText(iString1,iRect1,ascent,CGraphicsContext::ELeft);
	iGc->DrawText(iString2,iRect2,ascent,CGraphicsContext::ELeft);
	iGc->DrawText(iString3,iRect3,ascent,CGraphicsContext::ELeft);
	iGc->DrawText(iString4,iRect4,ascent,CGraphicsContext::ELeft);
	}

void CTimeWindow::DoDraw()
	{
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	Draw();
	iGc->Deactivate();
	}

void CTimeWindow::SetNumWindows(TInt aGroups,TInt aWins)
	{
	iNumGroups=aGroups;
	iNumWins=aWins;
	UpdateString1();
	}

void CTimeWindow::UpdateString1()
	{
	_LIT(NumberWin,"Groups (2x)%d, Windows %d");
	iString1.Format(NumberWin,iNumGroups,iNumWins);
	}

void CTimeWindow::SetTime1(TTimeIntervalMicroSeconds& aTime)
	{
	iTime1=aTime;
	UpdateString2();
	}

void CTimeWindow::SetTime2(TTimeIntervalMicroSeconds& aTime)
	{
	iTime2=aTime;
	UpdateString3();
	}

_LIT(TimeFormat,"Time%d = %d.%04dsecs");

void CTimeWindow::UpdateString2()
	{
	TInt time = I64LOW(iTime1.Int64());
	iString2.Format(TimeFormat,1,time/1000000,(time%1000000)/100);
	}

void CTimeWindow::UpdateString3()
	{
	TInt time = I64LOW(iTime2.Int64());
	iString3.Format(TimeFormat,2,time/1000000,(time%1000000)/100);
	}


//
// CQBlankWindow
//

void CQBlankWindow::InitWin()
	{
	SetColor(TRgb::Gray4(iInitialColor));
	iWin.SetSize(iSize);
	}


//
// CFixScreenModeGroup
//

CFixScreenModeGroup::~CFixScreenModeGroup()
	{
	GroupWin()->EnableReceiptOfFocus(EFalse);
	ClearCurrentWindow();
	CQBlankWindow* win;
	while (!iWindows.IsEmpty())
		{
		win=iWindows.First();
		iWindows.Remove(*win);
		delete win;
		}
	}

CFixScreenModeGroup::CFixScreenModeGroup(CTClient *aClient) : CTWindowGroup(aClient), iWindows(_FOFF(CQBlankWindow,iLink))
	{}

void CFixScreenModeGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	CQBlankWindow* win;
	TInt ii;
	for(ii=0;ii<iNoClientWins;++ii)
		{
		win=new(ELeave) CQBlankWindow();
		CleanupStack::PushL(win);
		win->ConstructL(*this);
		win->Activate();
		win->AssignGC(*iClient->iGc);
		iWindows.AddLast(*win);
		CleanupStack::Pop(win);
		Client()->Flush();
		}
	SetCurrentWindow(iWindows.First());
	}

/*void CFixScreenModeGroup::ScreenDeviceChanged()
	{
	iWindow->ScreenDeviceChanged();
	}*/


//
// CScreenModeTimes
//

_LIT(TestName,"Screen Mode Times");
CScreenModeTimes::CScreenModeTimes() : CTestBase(TestName)
								,iNormalGroupQue(_FOFF(CFixScreenModeGroup,iLink)),iFlipGroupQue(_FOFF(CFixScreenModeGroup,iLink))
	{}

CScreenModeTimes::~CScreenModeTimes()
	{
	Client()->iGroup->GroupWin()->EnableScreenChangeEvents();
	Client()->iScreen->SetScreenMode(0);
	DeleteGroups(iNormalGroupQue);
	DeleteGroups(iFlipGroupQue);
	Client()->iGroup->SetCurrentWindow(NULL);
	delete iFlipClient;
	delete iInfoWindow;
	if (iEnforcementMode==ESizeEnforcementNone)
		 Client()->iScreen->SetScreenModeEnforcement(ESizeEnforcementNone);
	}

void CScreenModeTimes::ConstructL()
	{
	iEnforcementMode=Client()->iScreen->ScreenModeEnforcement();
	if (iEnforcementMode==ESizeEnforcementNone)
		 Client()->iScreen->SetScreenModeEnforcement(ESizeEnforcementPixelsAndRotation);
	iDialogue=new(ELeave) CInfoDialog(Client()->iGroup,Client()->iGc);
	iDialogue->ConstructLD();
	iFlipClient=new(ELeave) CFlipClient();
	iFlipClient->ConstructL();
	Client()->iGroup->GroupWin()->DisableScreenChangeEvents();
	iNoGroupWins=12;		//20;
	CFixScreenModeGroup::iNoClientWins=20;		//30;
	TSize screenSize=Client()->iScreen->SizeInPixels();
	CQBlankWindow::iSize.iWidth=Max(screenSize.iHeight,screenSize.iWidth);
	CQBlankWindow::iSize.iHeight=CQBlankWindow::iSize.iWidth;
	TInt ii;
	for(ii=0;ii<iNoGroupWins;++ii)
		{
		CQBlankWindow::iInitialColor=1;
		CreateGroupL(Client(),iNormalGroupQue);
		CQBlankWindow::iInitialColor=2;
		CreateGroupL(iFlipClient,iFlipGroupQue);
		}
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0);
	iInfoWindow=new(ELeave) CTimeWindow();
	iInfoWindow->SetUpL(TPoint(Max(Min(415,screenSize.iWidth-210),0),Min(50,screenSize.iHeight-180)),TSize(210,180),Client()->iGroup,*Client()->iGc);
	Client()->iGroup->SetCurrentWindow(iInfoWindow);
	iInfoWindow->Construct(this);
	iInfoWindow->WinTreeNode()->SetOrdinalPosition(1);
	iInfoWindow->SetNumWindows(iNoGroupWins,CFixScreenModeGroup::iNoClientWins);

	Client()->iGroup->GroupWin()->DisableScreenChangeEvents();
	Client()->iWs.SetAutoFlush(ETrue);
	iFlipClient->iWs.SetAutoFlush(ETrue);
	Client()->iWs.SetPointerCursorMode(EPointerCursorNone);
	}

void CScreenModeTimes::CreateGroupL(CTClient *aClient,TSglQue<CFixScreenModeGroup>& aQue)
	{
	CFixScreenModeGroup* group;
	group=new(ELeave) CFixScreenModeGroup(aClient);
	CleanupStack::PushL(group);
	group->ConstructL();
	aQue.AddLast(*group);
	CleanupStack::Pop(group);
	}

void CScreenModeTimes::DeleteGroups(TSglQue<CFixScreenModeGroup>& aQue)
	{
	CFixScreenModeGroup* group;
	while (!aQue.IsEmpty())
		{
		group=aQue.First();
		aQue.Remove(*group);
		delete group;
		}
	}

void CScreenModeTimes::BringToFront(TSglQue<CFixScreenModeGroup>& aQue)
	{
	TSglQueIter<CFixScreenModeGroup> iter(aQue);
	CFixScreenModeGroup* group=iter++;
	group->GroupWin()->SetOrdinalPosition(0);
	while ((group=iter++)!=NULL)
		group->GroupWin()->SetOrdinalPosition(1);
	}

void CScreenModeTimes::DoTimings()
	{
	//Timings 1 SCRDEV, DWsScreenDevice::CommandL, around call to SetScreenMode
	//Timings 2 GROUPWIN, CWsWindowGroup::SetScreenDeviceValidStates, around main body of function including calls to old way
	//Timings 3 CLIWIN, CWsTopClientWindow::SetScreenDeviceValidState, around call to ResetHiddenFlagAndAdjustRegions
	//Timings 3 GROUPWIN, CWsWindowGroup::SetScreenDeviceValidStates, around main body of function excluding calls to old way
	//Timings 4&5 CLIWIN, CWsClientWindow::ResetHiddenFlagAndAdjustRegions, going invisible and becomming visible sections
	Client()->iGroup->GroupWin()->SetOrdinalPosition(-2);
	Client()->Flush();
	TTime startTime;
	TTime middleTime;
	TTime endTime;
__PROFILE_START(0);
	startTime.HomeTime();
	BringToFront(iNormalGroupQue);
	Client()->iScreen->SetScreenMode(1);
	middleTime.HomeTime();
	BringToFront(iFlipGroupQue);
	Client()->iScreen->SetScreenMode(0);
	endTime.HomeTime();
__PROFILE_END(0);
#if defined(__PROFILING__)
	TProfile profile;
	RDebug::ProfileResult(&profile,3,1);
	if (profile.iCount>0)
		iDialogue->TimerResults(0);
#endif
	TTimeIntervalMicroSeconds time=middleTime.MicroSecondsFrom(startTime);
	iInfoWindow->SetTime1(time);
	time=endTime.MicroSecondsFrom(middleTime);
	iInfoWindow->SetTime2(time);
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0);
#if defined(__PROFILING__)
	if (profile.iCount>0)
		iDialogue->Display();
#endif
	}

TestState CScreenModeTimes::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Time ScreenMode Changes"),1);
			DoTimings();
			iState++;
			return EContinue;
		default:
			return EFinished;
		}
	}
