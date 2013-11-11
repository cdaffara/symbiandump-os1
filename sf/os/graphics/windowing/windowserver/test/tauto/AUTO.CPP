// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Automatically test the window server
// 
//


#include <e32std.h>
#include <w32std.h>
#include <e32svr.h>
#include <hal.h>
#include "../tlib/testbase.h"
#include <stdlib.h>
#include "AUTO.H"

LOCAL_D const TUint KPanicThreadHeapSize=0x8000;

GLREF_C TInt ProcPriTestOtherProcess(TAny *aScreenNumber);
GLREF_C TInt ProcToKill(TAny *aScreenNumber);
GLREF_C TInt ProcDirect(TAny *aScreenNumber);
GLREF_C TInt ProcMultiDisplay(TAny *aScreenNumber);

_LIT(Priority,"ProcPriTest");
_LIT(ToKill,"ProcToKill");
_LIT(Direct,"ProcDirect");
_LIT(MultiDisplay,"ProcMultiDisplay");

CProcess::TInitialFunction CProcess::iFunctions[]=
	{
	TInitialFunction(Priority,ProcPriTestOtherProcess),
	TInitialFunction(ToKill,ProcToKill),
	TInitialFunction(Direct,ProcDirect),
	TInitialFunction(MultiDisplay,ProcMultiDisplay)
	};

GLDEF_D TestWindow *BaseWin;
GLDEF_D TestWindow *TestWin;
GLDEF_D TestClient *TheClient=NULL;
GLDEF_D CWindowGc *TheGc;
TInt CTestBase::iMaxGrays=0;
TInt CTestBase::iScreenNo=0;
TInt CTestBase::iMaxColors=0;
TInt CTestBase::iNumberTestsPass=0;
TInt CTestBase::iNumberTests=0;
TRect CTestBase::iNormalPointerCursorArea;
TPartialRedrawType CTestBase::iRedrawType=EPartialRedraw_Unknown;
TInt CTestBase::iNumberOfGrpWndsOnPrimaryScreenWithZeroPriority = 0;
TInt CTestDriver::iTestNum=0;

GLDEF_D TInt TheTestResult=ETestPassed; // start with passed to anticipate empty test table

const TInt KBorderWinWidth = 5;

_LIT(Auto,"AUTO ");

TInt64 TTestRects::iSeed=0;
TRect TTestRects::iList[]={
// 0-5
	TRect(2,2,2,2),
	TRect(2,2,2,2),
	TRect(2,2,2,2),
	TRect(2,2,2,2),
	TRect(2,2,2,2),
// 5-13
	TRect(0,0,0,0),
	TRect(2,0,2,0),
	TRect(4,0,4,0),
	TRect(0,2,0,2),
	TRect(2,2,2,2),
	TRect(4,2,4,2),
	TRect(0,4,0,4),
	TRect(2,4,2,4),
	TRect(4,4,4,4),
// 13-22
	TRect(0,0,1,1),
	TRect(1,0,3,1),
	TRect(3,0,4,1),
	TRect(0,1,1,3),
	TRect(1,1,3,3),
	TRect(3,1,4,3),
	TRect(0,3,1,4),
	TRect(1,3,3,4),
	TRect(3,3,4,4),
// 23-31
	TRect(0,0,2,0),
	TRect(2,0,4,0),
	TRect(0,0,4,0),
	TRect(0,2,2,2),
	TRect(2,2,4,2),
	TRect(0,2,4,2),
	TRect(0,4,2,4),
	TRect(2,4,4,4),
	TRect(0,4,4,4),
// 32-40
	TRect(0,0,0,2),
	TRect(0,2,0,4),
	TRect(0,0,0,4),
	TRect(2,0,2,2),
	TRect(2,2,2,4),
	TRect(2,0,2,4),
	TRect(4,0,4,2),
	TRect(4,2,4,4),
	TRect(4,0,4,4),
// 41-44
	TRect(0,0,2,2),
	TRect(2,0,4,2),
	TRect(0,2,2,4),
	TRect(2,2,4,4),
// 45-48
	TRect(0,0,3,3),
	TRect(1,0,4,3),
	TRect(0,1,3,4),
	TRect(1,1,4,4),
// 49
	TRect(0,0,4,4),
// 40-53
	TRect(1,2,2,2),
	TRect(2,2,3,2),
	TRect(2,1,2,2),
	TRect(2,2,3,2),
// 54-59
	TRect(1,2,3,2),
	TRect(2,1,2,3),
	TRect(1,1,2,2),
	TRect(1,2,2,3),
	TRect(2,2,3,3),
	TRect(2,1,3,2),
// 60-63
	TRect(1,1,3,2),
	TRect(1,1,2,3),
	TRect(1,2,3,3),
	TRect(2,1,3,3)};

#if defined(__WINS__)
void FindNonMatchingPixelL(TPoint aPt1,TPoint aPt2,TSize aSize)
// This function is purely for use when debugging to find the first non-matching pixel
// when a check of two on screen rects has failed.
	{
	HBufC8* buf1=HBufC8::NewMaxLC(2*aSize.iWidth);
	HBufC8* buf2=HBufC8::NewMaxLC(2*aSize.iWidth);
	TPtr8 ptr1=buf1->Des();
	TPtr8 ptr2=buf2->Des();
	TInt row=0;
	TBool ret = true;
	for (;row<aSize.iHeight;++row)
		{
		TheClient->iScreen->GetScanLine(ptr1,aPt1,aSize.iWidth,EColor64K);
		TheClient->iScreen->GetScanLine(ptr2,aPt2,aSize.iWidth,EColor64K);
		if (ptr1!=ptr2)
			break;
		++aPt1.iY;
		++aPt2.iY;
		}
	TRgb color1,color2;
	if (row<aSize.iHeight)
		{
		for (TInt col=0;col<aSize.iWidth;++col)
			{
			TheClient->iScreen->GetPixel(color1,aPt1);
			TheClient->iScreen->GetPixel(color2,aPt2);
			if (color1!=color2)
				{	// Break here to find first pixel that didn't match.
				TBuf<256> buf;
				_LIT(KFindNonMatchingPixelFmt,"First non matching pixel (%d,%d)");
				buf.Format(KFindNonMatchingPixelFmt,col,row);
				TheClient->iWs.LogMessage(buf);
				break;
				
				}
			++aPt1.iX;
			++aPt2.iX;
			}
		}
	CleanupStack::PopAndDestroy(2);
	}

void FindNonMatchingPixel(TPoint aPt1,TPoint aPt2,TSize aSize)
	{
	TRAPD(ignore,FindNonMatchingPixelL(aPt1,aPt2,aSize));
	}
#endif

void AutoPanic(TInt aPanic)
	{
	User::Panic(_L("Auto"),aPanic);
	}

void CleanUpWindow(TAny *aWindow)
	{
	((RWindowTreeNode  *)aWindow)->Close();
	}

void PushWindowL(RWindowTreeNode *aWindow)
	{
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,aWindow));
	}


TBool OpacityAndAlphaSupportedL()
	{
	// If opacity is not implemented, EFalse will be returned
	if(TransparencySupportedL()!=KErrNone)
		return EFalse;

	const TRgb KTransparencyColor(0,0,0);
	RWindow winb(TheClient->iWs);
	CleanupClosePushL(winb);
	RWindow wint(TheClient->iWs);
	CleanupClosePushL(wint);
	User::LeaveIfError(winb.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	User::LeaveIfError(wint.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	winb.SetExtent(TPoint(0,0), TSize(50,50));
	wint.SetExtent(TPoint(0,0), TSize(50,50));
	winb.SetRequiredDisplayMode(EColor256);
	wint.SetRequiredDisplayMode(EColor256);
	wint.SetTransparencyFactor(KTransparencyColor);
	winb.SetBackgroundColor(TRgb(0,0,255));
	wint.SetBackgroundColor(TRgb(255,0,0));
	winb.Activate();
	wint.Activate();

	wint.BeginRedraw();
	TheClient->iGc->Activate(wint);
	TheClient->iGc->SetOpaque(ETrue);
	TheClient->iGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheClient->iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheClient->iGc->SetBrushColor(TRgb(0,255,0));
	TheClient->iGc->DrawRect(TRect(TPoint(0,0), TSize(50,50)));
	TheClient->iGc->SetOpaque(EFalse);
	TheClient->iGc->Deactivate();
	wint.EndRedraw();
	TheClient->Flush();

	// The window should be all green, if opacity is working, or all blue if it isn't.
	// If the window has any other colour, then something has broken.
	TRgb color;
	TheClient->iScreen->GetPixel(color,TPoint(25,25));
	if (color.Red()>0 || ((color.Green()==0) == (color.Blue()==0)))
		User::Leave(KErrGeneral);
	TBool ret=(color.Green()>0);

	CleanupStack::PopAndDestroy(2,&winb); // wint
	return ret;
	}

TInt TransparencySupportedL()
	{
	// Creates a window and enables alpha transparency, if this feature
	// is not enabled, KErrNotSupported will be returned
	RWindow win(TheClient->iWs);
	User::LeaveIfError(win.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	win.SetExtent(TPoint(0,0), TSize(50,50));
	TInt ret=win.SetTransparencyAlphaChannel();
	win.Close();
	return ret;
	}

TInt CheckScalingSupportedOrNot()
	{
	TBool scalingSupported=EFalse;
	TSizeMode originalModeData=TheClient->iScreen->GetCurrentScreenModeAttributes();
	TSizeMode tempData=originalModeData;
	tempData.iScreenScale=TSize(2,2);
	TheClient->iScreen->SetCurrentScreenModeAttributes(tempData);
	TSize scale=TheClient->iScreen->GetCurrentScreenModeScale();
	if (scale.iWidth==2 && scale.iHeight==2)
		{
		scalingSupported=ETrue;
		}
	TheClient->iScreen->SetCurrentScreenModeAttributes(originalModeData);
	TheClient->Flush();
	return scalingSupported;
	}

TBool CheckNonZeroOriginsSupportedOrNot()
	{
	TBool nonZeroOriginsSupported=EFalse;
	TSizeMode sizeMode1=TheClient->iScreen->GetCurrentScreenModeAttributes();
	TSizeMode sizeMode2=sizeMode1;
	sizeMode2.iOrigin=TPoint(30,30);
	TheClient->iScreen->SetCurrentScreenModeAttributes(sizeMode2);
	TPoint origin=TheClient->iScreen->GetCurrentScreenModeScaledOrigin();
	if (origin.iX==30 && origin.iY==30)
		{
		nonZeroOriginsSupported=ETrue;
		}
	TheClient->iScreen->SetCurrentScreenModeAttributes(sizeMode1);
	TheClient->Flush();
	return nonZeroOriginsSupported;
	}

TPoint PhysicalToLogical(TPoint aPhysicalPtMinusOrigin,TSize aScale)
	{
	TPoint logicalPt(aPhysicalPtMinusOrigin);
	if (aScale.iWidth!=1)
		{
		logicalPt.iX=(logicalPt.iX>= 0 ? logicalPt.iX/aScale.iWidth : (logicalPt.iX-(aScale.iWidth-1))/aScale.iWidth);
		}
	if (aScale.iHeight!=1)
		{
		logicalPt.iY=(logicalPt.iY>= 0 ? logicalPt.iY/aScale.iHeight : (logicalPt.iY-(aScale.iHeight-1))/aScale.iHeight);
		}
	return logicalPt;
	}

//
// Log window, logs testing //
//

LogWindow::LogWindow() : CTWin(), iTestTitle(KNullDesC), iSubTitle(KNullDesC), iMessage(KNullDesC)
	{}

void LogWindow::ConstructL(CTWinBase &parent)
	{
	CTWin::ConstructL(parent);
	iTitleHeight=iFont->HeightInPixels()+4;
	}

void LogWindow::DrawSubTitle()
	{
	iGc->DrawText(iSubTitle, TRect(2,iTitleHeight*2,iSize.iWidth-2,iTitleHeight*3),iFont->AscentInPixels(), CGraphicsContext::ECenter);
	}

void LogWindow::DrawMessage()
	{
	iGc->DrawText(iMessage, TRect(1,iTitleHeight*4,iSize.iWidth-2,iTitleHeight*5),iFont->AscentInPixels(), CGraphicsContext::ECenter);
	}

void LogWindow::Draw()
	{
	iGc->SetPenColor(TRgb::Gray16(8));
	iGc->SetPenColor(TRgb::Gray16(0));
	DrawBorder();
	iGc->DrawLine(TPoint(0,iTitleHeight),TPoint(iSize.iWidth,iTitleHeight));
	iGc->DrawText(iTestTitle, TPoint((iSize.iWidth-iFont->TextWidthInPixels(iTestTitle))/2,iFont->AscentInPixels()+2));
	DrawSubTitle();
	DrawMessage();
	}

/**
This function is not used at the moment but I leave it in in case I need it when I improve the logging in 
the log window.
*/
void LogWindow::LogTest(const TDesC &aTitle,TInt aNum)
	{
	_LIT(Test,"Test %d,%S");
	iTestTitle.Format(Test,aNum,&aTitle);
	TLogMessageText buf;
	_LIT(AutoNewTest,"AUTO New Test: ");
	buf.Append(AutoNewTest);
	buf.Append(iTestTitle);
	TheClient->LogMessage(buf);
	iMessage.Zero();
	iWin.Invalidate();
	}

/**
This function is not used at the moment but I leave it in in case I need it when I improve the logging in 
the log window.
*/
const TDesC& LogWindow::LogSubTest(const TDesC &aTitle,TInt aNum)
	{
	_LIT(SubTest,"Sub-Test[%d], %S");
	iSubTitle.Format(SubTest,aNum,&aTitle);
	TLogMessageText buf;
	buf.Append(Auto);
	buf.Append(iSubTitle);
	TheClient->LogMessage(buf);
	iMessage.Zero();
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	DrawSubTitle();
	DrawMessage();
	iGc->Deactivate();
	TheClient->iWs.Flush();
	return iSubTitle;
	}

const TDesC& LogWindow::LogMessage(TBool aLog,const TDesC& aText,TInt aNum)
	{
	if (aNum!=EDummyValue)
		{
		_LIT(StringInt,"%S %d");
		iMessage.Format(StringInt,&aText,aNum);
		}
	else
		{
		_LIT(String,"%S");
		iMessage.Format(String,&aText);
		}
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	DrawMessage();
	iGc->Deactivate();
	if (aLog)
		{
		TLogMessageText buf;
		buf.Append(Auto);
		buf.Append(iMessage);
		TheClient->LogMessage(buf);
		}
	TheClient->iWs.Flush();
	return iMessage;
	}


//
// Test window, simple window used to do test graphics in //
//
TestWindow::TestWindow() : CTWin()
	{
	}

TestWindow::~TestWindow()
	{
	delete iBorderWin;
	}

void TestWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	iBorderWin=new(ELeave) CBorderWindow();
	iBorderWin->SetUpL(pos,size,parent,aGc);
	TRAPD(err, CTWin::ConstructL(*iBorderWin));
	if (err==KErrNone)
		{
		SetExt(TPoint(2,2),TSize(size.iWidth-4,size.iHeight-4));
		if (err==KErrNone)
			{
			Activate();
			AssignGC(aGc);
			return;
			}
		}
	delete this;
	User::Leave(err);
	}

void TestWindow::Draw()
	{
	iGc->Clear();
	}

void TestWindow::ClearWin()
	{
	TheGc->Activate(*Win());
	TheGc->Clear();
	TheGc->Deactivate();
	}
	
void TestWindow::SetBorderExt(TPoint aPos, TSize aSize)
	{
	iBorderWin->SetExt(aPos, aSize);
	}

CBorderWindow* TestWindow::GetBorderWin()
	{
	return iBorderWin;
	}
//
CBorderWindow::CBorderWindow() : CTWin()
	{
	}

void CBorderWindow::ConstructL(CTWinBase &parent)
	{
	CTWin::ConstructL(parent);
	}

void CBorderWindow::Draw()
	{
	iGc->SetBrushColor(TRgb::Gray16(0));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->DrawRect(TRect(Size()));
	}

//

TestWindowGroup::TestWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{
	}

void TestWindowGroup::BecomeOwning()
	{
	iGroupWin.DefaultOwningWindow();
	}

void TestWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (aKey.iModifiers&EModifierFunc)
		{
		switch(aKey.iCode)
			{
			case 'f':
				TheClient->Driver()->iTest->TriggerFail();
				break;
			}
		}
	else if (iCurWin)
		iCurWin->WinKeyL(aKey,aTime);
	}

void TestWindowGroup::KeyDownL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (iCurWin)
		iCurWin->KeyDownL(aKey,aTime);
	}

void TestWindowGroup::KeyUpL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (iCurWin)
		iCurWin->KeyUpL(aKey,aTime);
	}

//

TestClient::TestClient()
	{}

void TestClient::ConstructL()
	{
	CTClient::ConstructL();

	iGroup=new(ELeave) TestWindowGroup(this);
	iGroup->ConstructL();

	iScreen->GetScreenSizeModeList(&iScreenModes);
	iScreen->SetScreenMode(iScreenModes[0]);
	TSize screenSize=iScreen->SizeInPixels();

	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	iStdLogWindow=new(ELeave) LogWindow();
	iStdLogWindow->SetUpL(TPoint(5,5),TSize(winWidth,winHeight),iGroup,*iGc);
	BaseWin=new(ELeave) TestWindow();
	BaseWin->SetUpL(TPoint(screenSize.iWidth/3+5,5),TSize(winWidth,winHeight),iGroup,*iGc);
	TestWin=new(ELeave) TestWindow();
	TestWin->SetUpL(TPoint(screenSize.iWidth/3*2+5,5),TSize(winWidth,winHeight),iGroup,*iGc);

	iDriver = CTestDriver::CreateL(iScreenNumber); // virtual constructor

	TheGc=new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(TheGc->Construct());

	_LIT(KTestLog,"WSERV Auto Test Log");
	LogMessage(KTestLog());
	LogAvailableScreenModeL();
	
	//This class was designed to be created once and reused by all the tests, now it is created for each test,
	//this needs to be changed back so tests like the following are only done once.
	TestWsSetBufferSizeL();
	TestWsSetMaxBufferSizeL();
	}
	
inline CTestDriver* TestClient::Driver()
	{
	return iDriver;
	}

TBool TestClient::WaitForEvent()
	{
	if (iEventHandler->iStatus!=KRequestPending)
		return ETrue;
	else if (iRedrawEventHandler->iStatus!=KRequestPending)
		return EFalse;
	User::WaitForRequest(iEventHandler->iStatus,iRedrawEventHandler->iStatus);
	TRequestStatus* status=&iEventHandler->iStatus;
	TBool ret=ETrue;
	if (iEventHandler->iStatus==KRequestPending)
		{
		status=&iRedrawEventHandler->iStatus;
		ret=EFalse;
		}
	TInt reason=status->Int();
	*status=KRequestPending;
	User::RequestComplete(status,reason);
	return ret;
	}

TBool TestClient::IsEventWaiting()
	{
	return (iEventHandler->iStatus!=KRequestPending || iRedrawEventHandler->iStatus!=KRequestPending);
	}

TestClient::~TestClient()
	{
	iScreenModes.Close();
	delete iDriver;
	delete TheGc;
	delete iStdLogWindow;
	delete BaseWin;
	delete TestWin;
	}

void TestClient::LogAvailableScreenModeL()
	{
	_LIT(KColorModes,"Color Modes: ");
	_LIT(KComma,", ");
	_LIT(KColor,"Color");
	_LIT(KGrey,"Grey");
	CArrayFixFlat<TInt>* modeList=new(ELeave) CArrayFixFlat<TInt>(15);
	iWs.GetColorModeList(modeList);
	TLogMessageText buf,buf2;
	buf.Append(KColorModes);
	TDisplayMode mode;
	TInt ii=0;
	FOREVER
		{
		mode=STATIC_CAST(TDisplayMode,(*modeList)[ii]);
		buf.AppendNum((*modeList)[ii]);
		buf2.Append(TDisplayModeUtils::IsDisplayModeColor(mode)?KColor():KGrey());
		buf2.AppendNum(TDisplayModeUtils::NumDisplayModeColors(mode));
		if (mode==EColor16MU)
			buf2.Append('U');
		if (++ii==modeList->Count())
			break;
		buf.Append(KComma);
		buf2.Append(KComma);
		}
	LogMessage(buf);
	LogMessage(buf2);
	delete modeList;
	}

void TestClient::TestWsSetBufferSizeL()
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	ws.SetBufferSizeL(256);		// default buffer size 640
	ws.SetBufferSizeL(0x8000);	// 16K is max buffer size
	ws.SetBufferSizeL(0x4000);
	ws.Close();
	}

void TestClient::TestWsSetMaxBufferSizeL() 
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// allow buffer to grow bigger than the default 640 bytes
	const TInt KBigMessageSize = 800;
	ws.SetMaxBufferSizeL(KBigMessageSize + 8);	// big message + command header length
	
	// send the big message to the wserv
	TBuf8<KBigMessageSize>	bigMessage;

	// LogMessage needs a pointer to a TInt with the message size at the start of the buffer
	const TInt szLength = sizeof(TInt);
	TInt length = KBigMessageSize - szLength;			// length in Unicode characters
	bigMessage.Append((TUint8*) &length, szLength);

	_LIT(KLetterA, "a");
	do 
		{
		bigMessage.Append((TUint8*) KLetterA().Ptr(), 2);
		}
		while (bigMessage.Length() < KBigMessageSize);

	// send to Wserv, note that the message is too long to be logged
	ws.TestWrite(ws.WsHandle(), EWsClOpLogMessage, bigMessage.Ptr(), KBigMessageSize);
	ws.Flush();

	ws.Close();
	}

void TestClient::SetTestClientScreenMode(TInt aMode)
	{
	iScreen->SetAppScreenMode(aMode);
	iScreen->SetScreenMode(aMode);
	UpdateTestClientScreenMode();
	}
	
void TestClient::UpdateTestClientScreenMode()
	{
	TSize screenSize=iScreen->SizeInPixels();

	// Sets new positions and dimensions for the three window and their controlling border windows
	
	if (screenSize.iHeight > screenSize.iWidth)		// Portrait mode
		{
		TInt winWidth=screenSize.iWidth - (KBorderWinWidth * 2);
		TInt winHeight=(screenSize.iHeight/3) - (KBorderWinWidth * 2);
		
		iStdLogWindow->SetExt(TPoint(KBorderWinWidth,KBorderWinWidth),TSize(winWidth,winHeight));
		BaseWin->SetBorderExt(TPoint(KBorderWinWidth,screenSize.iHeight/3+KBorderWinWidth),TSize(winWidth,winHeight));
		BaseWin->SetExt(TPoint(2,2),TSize(winWidth - (KBorderWinWidth - 1),winHeight - (KBorderWinWidth - 1)));
		TestWin->SetBorderExt(TPoint(KBorderWinWidth,screenSize.iHeight/3*2+KBorderWinWidth),TSize(winWidth,winHeight));
		TestWin->SetExt(TPoint(2,2),TSize(winWidth - (KBorderWinWidth - 1),winHeight - (KBorderWinWidth - 1)));
		}
	else											// Landscape modes
		{
		TInt winWidth=(screenSize.iWidth/3) - (KBorderWinWidth * 2);
		TInt winHeight=screenSize.iHeight - (KBorderWinWidth * 2);
		
		iStdLogWindow->SetExt(TPoint(KBorderWinWidth,KBorderWinWidth),TSize(winWidth,winHeight));
		BaseWin->SetBorderExt(TPoint(screenSize.iWidth/3 + KBorderWinWidth,KBorderWinWidth),TSize(winWidth,winHeight));
		BaseWin->SetExt(TPoint(2,2),TSize(winWidth - (KBorderWinWidth - 1),winHeight - (KBorderWinWidth - 1)));
		TestWin->SetBorderExt(TPoint(screenSize.iWidth/3*2+KBorderWinWidth,KBorderWinWidth),TSize(winWidth,winHeight));	
		TestWin->SetExt(TPoint(2,2),TSize(winWidth - (KBorderWinWidth - 1),winHeight - (KBorderWinWidth - 1)));		
		}	
		
	// Remove shading artefacts from window resizing operations		
	TestWin->Win()->Invalidate();
	TestWin->Win()->BeginRedraw();
	TestWin->ClearWin();
	TestWin->Win()->EndRedraw();
	
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	BaseWin->ClearWin();
	BaseWin->Win()->EndRedraw();
	
	TheClient->iWs.Flush();
	}


//
// TestDriver, drives the test code //
//
CTestDriver* CTestDriver::CreateL(TInt aScreenNumber)
	{
	CTestDriver* self = NULL;
	if (aScreenNumber==KDefaultScreen)
		self = new (ELeave) CTestDriverPrimary(aScreenNumber);
	else
		self = new (ELeave) CTestDriverSecondary(aScreenNumber);

	return self;
	}

CTestDriver::CTestDriver(TInt aScreenNumber)
	: iScreenNumber(aScreenNumber)
	{
	iStartTime.HomeTime();
	HAL::Get(HALData::EDisplayNumberOfScreens, iNumberOfScreens);
	}

CTestDriver::~CTestDriver()
	{
	}

void CTestDriver::DestroyTest()
	{
	delete iTest;
	iTest=NULL;
	}

void CTestDriver::TestComplete2()
	{
	++iTestNum;
	if (iTestNum==iTestSize)
		{
		TBuf<64> timeBuf;
		TTime endTime;
		endTime.HomeTime();
		TTimeIntervalMicroSeconds elapseTime=endTime.MicroSecondsFrom(iStartTime);
		TInt64 elapseTime2=elapseTime.Int64()/1000;
		TUint diffi = I64LOW(elapseTime2);
		_LIT(TestPass,"PASSED");
		_LIT(TestFail,"FAILED");
		TInt noOfTests=CTestBase::iNumberTests;
		TInt noOfTestsPass=CTestBase::iNumberTestsPass;
		_LIT(TimeBuf,"Elapse Time  %d:%02d.%03d   %S");
		timeBuf.Format(TimeBuf,diffi/60000,(diffi/1000)%60,diffi%1000000,&(noOfTests==noOfTestsPass?TestPass:TestFail));
		TBuf<60> testReport;
		_LIT(Checks,"Checks");
		_LIT(Fails,"Fails");
		_LIT(TestReport,"Tests:%d %S:%d");
		TInt testNumber=(noOfTests==noOfTestsPass? noOfTestsPass : noOfTests-noOfTestsPass);
		testReport.Format(TestReport,iTestNum,&(noOfTests==noOfTestsPass?Checks():Fails()),testNumber);
		if (noOfTests!=noOfTestsPass)
			{
			_LIT(NumTests,"/%d");
			testReport.AppendFormat(NumTests,noOfTests);
			}

		TBuf<60> logTestReport;
		_LIT(LogReport," %S:%d/%d");
		logTestReport.Format(LogReport,&Checks,noOfTestsPass,noOfTests);
		TLogMessageText buf;
		_LIT(Finished,"AUTO Testing Complete, ");
		buf.Append(Finished);
		buf.Append(timeBuf);
		buf.Append(logTestReport);
		TheClient->LogMessage(buf);

		TheTestResult = noOfTests==noOfTestsPass? ETestPassed : ETestFailed;
		DoDisplayDialog(timeBuf,testReport);
		}
	}

//
// Test driver for primary screen (has digitiser/pointer)
//
CTestDriverPrimary::CTestDriverPrimary(TInt aScreenNumber) : CTestDriver(aScreenNumber)
	{
	TInt i;
	TInt numOfEntries = 1;
	for (i=0; i<numOfEntries; ++i)
		{
		++iTestSize;
		}

	// omit multi display test (last entry in test table)
	// for single screen platform
	//
	if (iNumberOfScreens==1)
		--iTestSize;

	}

void CTestDriverPrimary::DoDisplayDialog(TDesC& timeBuf,TDesC& testReport)
	{
	DisplayDialog(_L("Auto tests complete"),timeBuf, testReport);
	}

//
// Test driver for secondary screens
//
CTestDriverSecondary::CTestDriverSecondary(TInt aScreenNumber) : CTestDriver(aScreenNumber)
	{
	TInt i;
	TInt numOfEntries = 1;
	for (i=0; i<numOfEntries; ++i)
		{
		++iTestSize;
		}
	}

void CTestDriverSecondary::DoDisplayDialog(TDesC& timeBuf,TDesC& testReport)
	{
	if (TheTestResult==ETestFailed)
		DisplayDialog(iScreenNumber,_L("Auto tests complete"),timeBuf, testReport);
	}

// TTestRects //
//
// This class provides a list of rectangles to test graphics to the test windows
// The list designed to test all cases total clipping, partial clipping, touching the edges,
// unclipped etc etc...
//
// NOTE:- Quick test version, does not provide a proper list yet
//
// For the rectangle list the graphics area is divided into 9 logical areas:-
// 0:The area above & left of the drawing area
// 1:The area above but within the left & right limits of the drawing area
// 2:The area above & right of the drawing area
// 3:The area left of but within the top & bottom limits of the drawing area
// 4:The area within the drawing area
// 5:The area right of but within the top & bottom limits of the drawing area
// 6:The area below & left of the drawing area
// 7:The area below but within the left & right limits of the drawing area
// 8:The area below & right of the drawing area
//
//			|		|
//		0	|	1	|	2
//			|		|
//	-------------------------
//			|		|
//		3	|	4	|	5
//			|		|
//	-------------------------
//			|		|
//		6	|	7	|	8
//			|		|
//
//
// The full list of rectangles needed to test an official release is as follows:
//
//	0-8:	A rectangle wholly within each area (0 to 8) without touching the edges (if possible)
//	9-17:	A rectangle wholly within each area and touching all bounding edges
//	18-26:	A rectangle traversing each horizontal pair & triple of areas without touching the boundarys
//	27-35:	A rectangle traversing each vertical pair & triple of areas without touching the boundarys
//	36-39:	A rectangle traversing each 2x2 block of areas without touching the boundarys
//	40-43:	A rectangle traversing each 2x2 block of areas touching each boundary
//  44:		A rectangle that includes all areas
//  45-48:	A rectangle fully enclosed in the central area that touches each edge
//	49-54:	A rectangle fully enclosed in the central area that touches each pair of edges
//	55-58:	A rectangle fully enclosed in the central area that touches each set of three edges
//  59-67:	A Zero size rectangle in each of the 9 areas
//	68-77:	Wide rectangles with heights from 0 to 9 in the central area
//	78-87:	Tall rectangles with widths from 0 to 9 in the central area
//

void TTestRects::Construct(const RWindow &aWindow)
	{
// 59-67
//		Special case, does not use rectangles from here
// 68-77
//		Special case, does not use rectangles from here
// 78-87
//		Special case, does not use rectangles from here
//

	iSize=aWindow.Size();
/*
	TInt wid=size.iWidth;
	TInt hgt=size.iHeight;
	TInt wid2=wid/2;
	TInt hgt2=hgt/2;
	TInt wid3=wid/3;
	TInt hgt3=hgt/3;
	TInt wid4=wid/4;
	TInt hgt4=hgt/4;
	TInt 2wid=wid*2;
	TInt 2hgt=hgt*2;
	TInt 3wid=wid*3;
	TInt 3hgt=hgt*3;
// 0-8
	iList[0]=TRect(-wid,-hgt,-wid2,-hgt2);
	iList[1]=TRect(wid3,-hgt,wid-wid3,-hgt2);
	iList[2]=TRect(wid+wid2,-2hgt,2wid,-hgt);
	iList[3]=TRect(-3wid,hgt3,-wid3,hgt-hgt3);
	iList[4]=TRect(wid4,hgt3,wid-wid3,hgt-hgt4);
	iList[5]=TRect(3wid,hgt4,wid+3wid,hgt-hgt4);
	iList[6]=TRect(-wid3,hgt+hgt4,-wid2,hgt+hgt);
	iList[7]=TRect(wid2,hgt+hgt4,wid-wid4,hgt+hgt3);
	iList[8]=TRect(2wid,hgt+hgt4,3wid,hgt+hgt2);
// 9-17
	iList[9]=TRect(-wid,-hgt,0,0);
	iList[10]=TRect(0,-2hgt,wid,0);
	iList[11]=TRect(wid,-hgt2,wid+wid3,0);
	iList[12]=TRect(-wid3,0,0,hgt);
	iList[13]=TRect(0,0,wid,hgt);
	iList[14]=TRect(wid,0,wid+wid4,hgt);
	iList[15]=TRect(-wid,hgt,0,hgt+hgt4);
	iList[16]=TRect(0,hgt,wid,hgt+hgt);
	iList[17]=TRect(wid,hgt,wid+3wid,hgt+3hgt);
// 18-26
	iList[18]=TRect(-wid,-hgt,wid2,hgt3);
	iList[19]=TRect(wid3,-2hgt,2wid,-hgt3);
	iList[20]=TRect(-wid,-hgt2,wid3,-hgt3);
	iList[21]=TRect(-wid3,hgt4,wid2,hgt2);
	iList[22]=TRect(wid3,hgt3,wid+wid3,hgt-hgt3);
	iList[23]=TRect(-wid,hgt2,wid+wid4,hgt-hgt3);
	iList[24]=TRect(-wid,2hgt,wid3,3hgt);
	iList[25]=TRect(wid-wid4,hgt+hgt3,wid+wid4,2hgt);
	iList[26]=TRect(-wid4,hgt+hgt4,wid+wid4,3);
*/
/*
	iList[0]=TRect(0,0,size.iWidth,size.iHeight);
	iList[1]=TRect(-10,-10,size.iWidth/2,size.iHeight/2);
	iList[2]=TRect(size.iWidth/2,size.iHeight/2,size.iWidth+10,size.iHeight+10);
	iList[3]=TRect(size.iWidth/4,size.iHeight/4,size.iWidth/2,size.iHeight/2);
	iList[4]=TRect(-10,size.iHeight/4,size.iWidth+10,size.iHeight/2);
*/
	}

TInt TTestRects::Count1() const
//
// Count1() provides the simple base set of rectangles
//
	{
	return(2);
	}

TInt TTestRects::Count2() const
//
// Count2() provides an increased set of rectangles for each graphics func to test itself more thoroughly
//
	{
	return(5);
	}

TInt TTestRects::Count3() const
//
// Count3() provides the full set of rects for each graphics func to a quick test on
//
	{
	return(88);
	}

/** Reset the seed value to 0. */
void TTestRects::ResetSeed()
	{
	iSeed = 0;
	}

TInt TTestRects::Rnd(TInt aSize)
//
// Return a random based around aSize, maybe bigger maybe smaller, who knows?
//
	{
	TInt rnd=Math::Rand(iSeed);
	TInt result;
	if (rnd&0x8)	// Increase from aSize
		result=aSize*((rnd&0x7)+1);
	else 			// Decrease from aSize
		result=aSize/((rnd&0x7)+1);
	return(result);
	}

TInt TTestRects::RndMax(TInt aSize)
//
// Return a random from 0 to aSize inclusive
//
	{
	TInt64 tmpl=Math::Rand(iSeed);
	TInt tmp = I64INT(tmpl) & 0xFFFF;
	tmp*=aSize;
	tmp/=0xFFFF;
	return(tmp);
	}

TInt TTestRects::RectCoord(TInt aSection,TInt aSize)
	{
	TInt result=0;
	switch(aSection)
		{
		case 0:
			result=-(1+Rnd(aSize));
			break;
		case 1:
			result=0;
			break;
		case 2:
			result=1+RndMax(aSize-2);
			break;
		case 3:
			result=aSize;
			break;
		case 4:
			result=aSize+1+Rnd(aSize);
			break;
		default:
			AutoPanic(EAutoPanicTestRectsSection);
		}
	return(result);
	}

void TTestRects::RectCoordPair(TInt &aTl, TInt &aBr, TInt aSection, TInt aSize)
	{
	do
		{
		aTl=RectCoord(aSection,aSize);
		aBr=RectCoord(aSection,aSize);
		} while(aTl==aBr && aSize>1);
	if (aTl>aBr)
		{
		TInt tmp=aBr;
		aBr=aTl;
		aTl=tmp;
		}
	}

TRect TTestRects::operator[](TInt aIndex)
	{
	TRect rect;
	if (aIndex<EMaxRectFromList)
		{
		const TRect *pRect=&iList[aIndex];
		if (pRect->iTl.iX==pRect->iBr.iX)
			RectCoordPair(rect.iTl.iX,rect.iBr.iX,pRect->iTl.iX,iSize.iWidth);
		else
			{
			rect.iTl.iX=RectCoord(pRect->iTl.iX,iSize.iWidth);
			rect.iBr.iX=RectCoord(pRect->iBr.iX,iSize.iWidth);
			}
		if (pRect->iTl.iY==pRect->iBr.iY)
			RectCoordPair(rect.iTl.iY,rect.iBr.iY,pRect->iTl.iY,iSize.iHeight);
		else
			{
			rect.iTl.iY=RectCoord(pRect->iTl.iX,iSize.iHeight);
			rect.iBr.iY=RectCoord(pRect->iBr.iX,iSize.iHeight);
			}
		}
	else if (aIndex<EMaxRectZeroSize)
		{
		rect.iTl.iX=RectCoord(((aIndex-EMaxRectFromList)%3)*2,iSize.iWidth);
		rect.iTl.iY=RectCoord(((aIndex-EMaxRectFromList)/3)*2,iSize.iHeight);
		rect.iBr=rect.iTl;
		}
	else if (aIndex<EMaxRectWide)
		{
		rect.iTl.iX=1;
		rect.iBr.iX=iSize.iWidth-1;
		rect.iTl.iY=1;
		rect.iBr.iY=rect.iTl.iY+(aIndex-EMaxRectWide);
		}
	else if (aIndex<EMaxRectHigh)
		{
		rect.iTl.iX=1;
		rect.iBr.iX=rect.iTl.iX+(aIndex-EMaxRectHigh);
		rect.iTl.iY=1;
		rect.iBr.iY=iSize.iHeight-1;
		}
	else
		AutoPanic(EAutoPanicTestRectsIndex);
	return(rect);
	}


/*CBitmap*/

CBitmap* CBitmap::NewLC(const TSize& aSizeInPixels,TDisplayMode aDispMode)
	{
	return NewLC(0,aSizeInPixels,aDispMode);
	}

CBitmap* CBitmap::NewL(const TSize& aSizeInPixels,TDisplayMode aDispMode)
	{
	CBitmap* self=NewLC(0,aSizeInPixels,aDispMode);
	CleanupStack::Pop(self);
	return self;
	}

CBitmap* CBitmap::NewL(TInt aHandle)
	{
	CBitmap* self=NewLC(aHandle,TSize(),ENone);
	CleanupStack::Pop(self);
	return self;
	}

CBitmap* CBitmap::NewL(TDesC& /*aFileName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CBitmap* CBitmap::NewLC(TInt aHandle,const TSize& aSizeInPixels,TDisplayMode aDispMode)
	{
	CBitmap* self=new(ELeave) CBitmap();
	CleanupStack::PushL(self);
	self->ConstructL(aHandle, aSizeInPixels, aDispMode);
	return self;
	}

void CBitmap::ConstructL(TInt aHandle,const TSize& aSizeInPixels,TDisplayMode aDispMode)
	{
	iBitmap=new(ELeave) CFbsBitmap();
	if (aHandle==0)
		{
		User::LeaveIfError(iBitmap->Create(aSizeInPixels,aDispMode));
		}
	else
		{
		User::LeaveIfError(iBitmap->Duplicate(aHandle));
		}
	iDevice=CFbsBitmapDevice::NewL(iBitmap);
	User::LeaveIfError(iDevice->CreateContext(iGc));
	}

CBitmap::~CBitmap()
	{
	delete iGc;
	delete iDevice;
	delete iBitmap;
	}


// CTestBase //

CTestBase::CTestBase(const TDesC& aTitle,CTWsGraphicsBase* aTestBase)
	{
	iTestBase=aTestBase;
	iTitle.Copy(aTitle);
	
	iScreenNo = iTestBase->GetScreenFromIni();
	
	TheClient=new(ELeave) TestClient();
	
	if (CTestBase::iScreenNo == 1)
		{
		TheClient->SetScreenNumber(1);
		iScreenNumber = 1;		
		}
	else
		{
		TheClient->SetScreenNumber(0);
		iScreenNumber = 0;
		}

	if (iScreenNumber == 1)
		{
		iMinWin = new(ELeave) CMinWin(iScreenNumber);
		iMinWin->ConstructL();
		}

	iTestNum=CTestDriver::iTestNum;
	TheClient->ConstructL();
	TheClient->StdLogWindow().LogTest(iTitle,iTestNum);
	iDriver=TheClient->Driver();
	
	if (CTestBase::iScreenNo == 1)
		{
		TheClient->iWs.SetFocusScreen(1);
		}
	else
		{
		TheClient->iWs.SetFocusScreen(0);
		}
	
	iTestRects.Construct(*BaseWin->Win());
	iStdTestWindowSize=BaseWin->Size();
	iRedrawType=EPartialRedraw_Unknown;	// Reset between tests
	if (iMaxGrays+iMaxColors==0)
		{
		TheClient->iWs.GetDefModeMaxNumColors(iMaxColors,iMaxGrays);
		iNormalPointerCursorArea=TheClient->iWs.PointerCursorArea();
		}
	}

CTestBase::~CTestBase()
	{
	delete iMinWin;
	delete TheClient;
	}

void CTestBase::CloseAllPanicWindows()
	{
	TInt idFocus = TheClient->iWs.GetFocusWindowGroup();
	TWsEvent event;
	event.SetType(EEventKey);
	TKeyEvent *keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;
	TInt theLimit = 50;
	while(idFocus != NULL && (theLimit-- > 0))
		{
		TheClient->iWs.SendEventToAllWindowGroups(event);
		idFocus = TheClient->iWs.GetFocusWindowGroup();
		}
	}
	
/**
Returns the size of the standard test windows.
@see iStdTestWindowSize
*/	
const TSize& CTestBase::StdTestWindowSize()
	{
	return iStdTestWindowSize;
	}

/** Returns the number of greys available in the richest grey mode */
TInt CTestBase::MaxGrays() const
	{
	return iMaxGrays;
	}

/** Returns the number of colours available in the richest supported colour mode. */
TInt CTestBase::MaxColors() const
	{
	return iMaxColors;
	}

void CTestBase::TriggerFail()
	{
	iFail=ETrue;
	}

void CTestBase::LogLeave(TInt aErr)
	{
	TLogMessageText buf;
	_LIT(Leave,"AUTO Left with error code %d in ");
	buf.AppendFormat(Leave,aErr);
	buf.Append(iSubTitle);
	TheClient->LogMessage(buf);
	}

void CTestBase::LogSubTest(const TDesC &aSubTitle)
	{
	Driver()->iSubTestNum++;
	iSubTitle=aSubTitle;
	iTestBase->Logger().Write(TheClient->StdLogWindow().LogSubTest(aSubTitle,iState));
	}

void CTestBase::LogMessage(TInt aValue)
	{
	_LIT(WinID,"Win Id:");
	TheClient->StdLogWindow().LogMessage(EFalse,WinID,aValue);
	}

void CTestBase::LogSubState(TInt aSubState)
	{
	_LIT(SubTest,"SubState");
	iTestBase->Logger().Write(TheClient->StdLogWindow().LogMessage(ETrue,SubTest,aSubState));
	}

TBool DoCheckRectRWin(RWindowBase &aWin1,RWindowBase &aWin2,const TRect &aRect)
	{
	TRect rect1(aRect);
	TRect rect2(aRect);
	rect1.Move(aWin1.InquireOffset(*TheClient->iGroup->WinTreeNode()));
	rect2.Move(aWin2.InquireOffset(*TheClient->iGroup->WinTreeNode()));
	TBool match=TheClient->iScreen->RectCompare(rect1,rect2);
#if defined(__WINS__)
	if (!match)
		FindNonMatchingPixel(rect1.iTl,rect2.iTl,aRect.Size());
#endif
	return match;
	}

TBool DoCheckRectRWin(RWindowBase &aWin1,RWindowBase &aWin2,const TRect &aRect, TUint aFlags)
	{
	TRect rect1(aRect);
	TRect rect2(aRect);
	rect1.Move(aWin1.InquireOffset(*TheClient->iGroup->WinTreeNode()));
	rect2.Move(aWin2.InquireOffset(*TheClient->iGroup->WinTreeNode()));
	TBool match=TheClient->iScreen->RectCompare(rect1,rect2, aFlags);
#if defined(__WINS__)
	if (!match)
		FindNonMatchingPixel(rect1.iTl,rect2.iTl,aRect.Size());
#endif
	return match;
	}

TBool DoCheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect)
	{
	return DoCheckRectRWin(*aWin1->BaseWin(), *aWin2->BaseWin(), aRect);
	}

TBool DoCheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2)
	{
	TSize winSize=aWin1->Size();
	TRect rect1(aWin1->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),winSize);
	TRect rect2(aWin2->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),winSize);
	return TheClient->iScreen->RectCompare(rect1,rect2);
	}

TBool DoCheckRect(CTBaseWin *aWin1, CTBaseWin *aWin2, const TRect &aRect, TUint aFlags)
	{
	return DoCheckRectRWin(*aWin1->BaseWin(), *aWin2->BaseWin(), aRect, aFlags);
	}


/**
Compares the contents of 2 rectangular areas of the screen.

@param aPos1 The top left corner of the first rectangle.
@param aPos2 The top left corner of the second rectangle.
@param aSize The size of the rectangles
@return ETrue if the 2 areas have the same content, EFalse otherwise.
*/
TBool DoCheckRect(TPoint aPos1,TPoint aPos2,TSize aSize)
	{
	return TheClient->iScreen->RectCompare(TRect(aPos1,aSize),TRect(aPos2,aSize));
	}

void CTestBase::DrawTestBackground(TBool aInvertColors, const TSize &aSize, TInt aGrays/*=16*/)
//
// Draws a standard test background with a mix of colors (shades).
// This is mainly used to test for graphic functions writing outside the intended area.
//
// This code assumes an TheGc is already active on the window to use.
//
	{
	TheGc->SetBrushColor(TRgb::Gray256(255));
	TInt step=5;
	TInt col=0;
	TInt colorInc=(aGrays>9 ? 17 : 85);
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	for(TInt row=0;row<aSize.iHeight;row+=step)
		{
		TheGc->SetBrushColor(TRgb::Gray256(aInvertColors ? 255-col:col));
		TheGc->DrawRect(TRect(0,row,aSize.iWidth,row+step));
		col=col+colorInc;
		if (col>255)
			col=0;
		}
	}

void CTestBase::AbortL()
	{
	CTestDriver* driver=iDriver;
	iDriver->DestroyTest();
	driver->TestComplete2();
	User::Leave(ETestFailed);
	}

/*CTWsGraphicsBase*/

CTWsGraphicsBase::CTWsGraphicsBase(CTestStep* aStep) : CTGraphicsBase(aStep)
	{
	}

CTWsGraphicsBase::~CTWsGraphicsBase()
	{
	delete iTest;
	}

void CTWsGraphicsBase::CreateTestBaseL(CTTMSGraphicsStep* aTmsStep)
	{
	__ASSERT_ALWAYS(iTest==NULL,AutoPanic(EAutoPanicRecalledCreateTestBaseL));
	iTest=new(ELeave) CTestBase(iStep->TestStepName(),this);
	iTmsStep = aTmsStep;
	}
/**
Gets the Screen Number from an .ini file supplied to the RUN_TEST_STEP. Screen number should
be put under the section [useScreen] as screen=0 or screen=1.

@return Screen number Defined in .ini file, otherwise 0.
*/
TInt CTWsGraphicsBase::GetScreenFromIni() const
	{
	_LIT(KUseScreenSection, "useScreen");
	_LIT(KScreen, "screen");

	TInt screen = 0;
	TBool configAvailable = iStep->GetIntFromConfig(KUseScreenSection, KScreen, screen);
	if(configAvailable)
		{
		return screen;
		}
	else
		{
		return 0;
		}
	}

void CTWsGraphicsBase::TestComplete()
	{
	_LIT(KTestComplete,"Test complete");
	INFO_PRINTF1(KTestComplete);
	CTGraphicsBase::TestComplete();
	}

void CTWsGraphicsBase::LogMessage(const TText8* aFile,TInt aLine,TRefByValue<const TDesC> aFmt,...)
	{
	TLogMessageText buf;
	VA_LIST list;
	VA_START(list,aFmt);
	buf.AppendFormatList(aFmt,list);
	TheClient->LogMessage(buf);
	Logger().LogExtra(aFile,aLine,ESevrInfo,buf);
	VA_END(list);
	}

TBool CTWsGraphicsBase::CheckRetValue(TBool aPass, const TDesC *aErrorMsg, const TDesC &aErrorFunction)
	{
	if (!aPass && aErrorMsg)
		{
		LOG_MESSAGE3(_L("%S %S failed"),aErrorMsg,&aErrorFunction);
		iTmsStep->MQCTest(EFalse,((TText8*)__FILE__),__LINE__);
		}
	iStep->TEST(aPass);
	return aPass;
	}

void CTWsGraphicsBase::CompareWindowsSoftFailWinscw(const TText8* aFile, TInt aLine)
	{
	if (!DoCheckRect(BaseWin, TestWin, BaseWin->Size()))
		{
#ifdef __WINS__
		_LIT(KMessage,"ERROR: Test Failed but is ignored on WINSCW");
		Logger().LogExtra(aFile, aLine, ESevrErr, KMessage);
#else // __WINS__
		iStep->TEST(EFalse);
		_LIT(KMessage,"ERROR: Test Failed");
		Logger().LogExtra(aFile, aLine, ESevrErr, KMessage);
#endif // __WINS__
		}
	}

void CTWsGraphicsBase::CheckRect(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC *aErrorMsg)
	{
	CheckRetValue(DoCheckRect(aPos1,aPos2,aSize),aErrorMsg,_L("CheckRect()"));
	}

void CTWsGraphicsBase::CheckRect(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC &aErrorMsg)
	{
	CheckRect(aPos1,aPos2,aSize,&aErrorMsg);
	}

void CTWsGraphicsBase::CheckRectNoMatch(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC *aErrorMsg)
	{
	CheckRetValue(!DoCheckRect(aPos1,aPos2,aSize),aErrorMsg,_L("CheckRectNoMatch()"));
	}

void CTWsGraphicsBase::CheckRectNoMatch(TPoint aPos1,TPoint aPos2,TSize aSize, const TDesC &aErrorMsg)
	{
	CheckRectNoMatch(aPos1,aPos2,aSize,&aErrorMsg);
	}

void CTWsGraphicsBase::CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC *aErrorMsg)
	{
	CheckRetValue(DoCheckRect(aWin1,aWin2),aErrorMsg,_L("CheckRect()"));
	}

void CTWsGraphicsBase::CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC &aErrorMsg)
	{
	CheckRect(aWin1,aWin2,&aErrorMsg);
	}

void CTWsGraphicsBase::CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC *aErrorMsg)
	{
	CheckRetValue(!DoCheckRect(aWin1,aWin2),aErrorMsg,_L("CheckRectNoMatch()"));
	}

void CTWsGraphicsBase::CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2, const TDesC &aErrorMsg)
	{
	CheckRectNoMatch(aWin1,aWin2,&aErrorMsg);
	}

TBool CTWsGraphicsBase::CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC *aErrorMsg)
	{
	return CheckRetValue(DoCheckRect(aWin1,aWin2,aRect),aErrorMsg,_L("CheckRect()"));
	}

TBool CTWsGraphicsBase::CheckRect(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC &aErrorMsg)
	{
	return CheckRect(aWin1,aWin2,aRect,&aErrorMsg);
	}

void CTWsGraphicsBase::CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC *aErrorMsg)
	{
	CheckRetValue(!DoCheckRect(aWin1,aWin2,aRect),aErrorMsg,_L("CheckRectNoMatch()"));
	}

void CTWsGraphicsBase::CheckRectNoMatch(CTBaseWin *aWin1,CTBaseWin *aWin2,const TRect &aRect, const TDesC &aErrorMsg)
	{
	CheckRectNoMatch(aWin1,aWin2,aRect,&aErrorMsg);
	}

void CTWsGraphicsBase::CompareWindows(const TRect &aRect, const TDesC *aErrorMsg)
	{
	CheckRetValue(DoCheckRect(BaseWin,TestWin,aRect),aErrorMsg,_L("CompareWindows()"));
	}

TBool CTWsGraphicsBase::CompareWindows(const TDesC *aErrorMsg)
	{
	return CheckRetValue(DoCheckRect(BaseWin,TestWin,TRect(BaseWin->Size())),aErrorMsg,_L("CompareWindows()"));
	}

void CTWsGraphicsBase::CompareWindows(const TRect &aRect, const TDesC &aErrorMsg)
	{
	CompareWindows(aRect,&aErrorMsg);
	}

TBool CTWsGraphicsBase::CompareWindows(const TDesC &aErrorMsg)
	{
	return CompareWindows(&aErrorMsg);
	}

/**
Returns the size of the standard test windows.

Several tests use 3 windows : one is a log window, one is a reference window 
and one is the actual output of the test. All these windows have the same width which is roughly
1/3 of the screen. They also have the same height which is roughly equal to the screen height.
*/
const TSize& CTWsGraphicsBase::StdTestWindowSize()
	{
	return iTest->StdTestWindowSize();
	}

/** Returns the number of greys available in the richest grey mode */
TInt CTWsGraphicsBase::MaxGrays() const
	{
	return iTest->MaxGrays();
	}

/** Returns the number of colours available in the richest supported colour mode. */
TInt CTWsGraphicsBase::MaxColors() const
	{
	return iTest->MaxColors();
	}

//
// Panic testing //
//

LOCAL_C TInt PanicThreadFunc(TAny *aPtr)
	{
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	SPanicParams *ptr=(SPanicParams *)aPtr;
	TInt ret;
	TRAP(ret,ret=(*ptr->func)(ptr->num,ptr->ptr));
	delete CleanUpStack;
	if (ret==EWsExitReasonBad)
		AutoPanic(EAutoPanicPanicFailed);
	return(ret);
	}

TInt CTestBase::LaunchPanicThread(RThread &aThread, SPanicParams *aPtr)
	{
	TBuf<32> threadName;
	threadName.Format(TRefByValue<const TDesC>(_L("AutoPanicThread%d")),iThreadNumber++);
	return(aThread.Create(threadName,PanicThreadFunc,KDefaultStackSize,KPanicThreadHeapSize,KPanicThreadHeapSize,aPtr,EOwnerThread));
	}

TInt CTestBase::TestPanicL(SPanicParams *aPtr, TInt aExitReason, const TDesC &aCategory, TBool* aTestFinished)
	{
	RThread thread;
	TRequestStatus stat;
	TInt err=LaunchPanicThread(thread, aPtr);
	if (err==KErrAlreadyExists)
		{
		// wait for kernel to clear up old threads
		// and have several attempts at starting the thread
		// if unsuccessful the first time
		for (TInt i=0;i<3;i++)
			{
			User::After(TTimeIntervalMicroSeconds32(100000));		//0.1 secs
			err=LaunchPanicThread(thread, aPtr);
			if (err!=KErrAlreadyExists)
				break;
			}
		}
	User::LeaveIfError(err);
	thread.Logon(stat);
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(stat);
	User::SetJustInTime(ETrue);

	TBool testFinished=EFalse;
	TBool testPassed=ETrue;
	if (thread.ExitType()==EExitKill)
		{
		User::LeaveIfError(thread.ExitReason());
		if(thread.ExitReason()!=EWsExitReasonFinished)
			{
			testPassed=EFalse;
			}
		testFinished=ETrue;	// Finish tests
		}
	else
		{
		if ((thread.ExitCategory().Compare(aCategory)!=0)
		|| (aExitReason!=EWservNoPanic && thread.ExitReason()!=aExitReason)
		|| (thread.ExitType()!=EExitPanic))
			{
			testPassed=EFalse;
			}			
		}

	if(aTestFinished)
		*aTestFinished=testFinished;
	thread.Close();
	return(testPassed);
	}

TInt CTestBase::TestWsPanicL(TPanicFunction aFunction, TClientPanic aExitReason, TInt aInt, TAny *aPtr, TBool* aTestFinished)
	{
	return TestPanicL(aFunction,aExitReason,aInt,aPtr,KWSERV, aTestFinished);
	}

TInt CTestBase::TestW32PanicL(TPanicFunction aFunction, TW32Panic aExitReason, TInt aInt, TAny *aPtr, TBool* aTestFinished)
	{
	return TestPanicL(aFunction,aExitReason,aInt,aPtr,KW32,aTestFinished);
	}
	
 TInt CTestBase::TestWservPanicL(TPanicFunction aFunction, TWservPanic aExitReason, TInt aInt, TAny *aPtr)
 	{
	_LIT(KWSERV1,"Wserv Internal Panic");
	return TestPanicL(aFunction,aExitReason,aInt,aPtr,KWSERV1);
	}
	
TInt CTestBase::TestPanicL(TPanicFunction aFunction, TInt aExitReason, TInt aInt, TAny *aPtr, const TDesC &aCategory, TBool* aTestFinished)
	{
	SPanicParams params;
	params.num=aInt;
	params.func=aFunction;
	params.ptr=aPtr;
	return TestPanicL(&params, aExitReason, aCategory, aTestFinished);
	}

TBool CTestBase::IsFullRomL()
	{
	TBool isFullRom = EFalse;
	_LIT(KWinName,"EikonServer");
	TInt numWinGroups=TheClient->iWs.NumWindowGroups();
	CArrayFixFlat<TInt>* list=new(ELeave) CArrayFixFlat<TInt>(numWinGroups);
	TheClient->iWs.WindowGroupList(list);
	numWinGroups=list->Count();	// Just in case it changed between originally getting it and getting the actual list
	TBuf<64> name;
	TInt ii;
	for(ii=0;ii<numWinGroups;++ii)
		{
		TheClient->iWs.GetWindowGroupNameFromIdentifier((*list)[ii],name);
	#ifndef DISABLE_FAIL_DIALOG
		TInt ordinalPos=0;
		ordinalPos+=ordinalPos;		//To stop a warning
		ordinalPos=
	#endif
			TheClient->iWs.GetWindowGroupOrdinalPriority((*list)[ii]);
		if (name==KWinName)
			{
			isFullRom = ETrue;
			break;
			}
		}
	delete list;
	return isFullRom;
	}

void CTestBase::DelayIfFullRomL()
	{
	if (IsFullRomL())
		User::After(400000);
	}

TPartialRedrawType CTestBase::RedrawStoreTypeL()
	{
/*	if (iRedrawType==EPartialRedraw_Unknown)
		{
		const TRgb KRed=TRgb(255,0,0);
		const TRgb KGreen=TRgb(0,255,0);
		const TRgb KBlue=TRgb(0,0,255);
		CWsScreenDevice* scrDev=TheClient->iScreen;
		TSize winSize=scrDev->SizeInPixels();
		CBlankWindow* win=new(ELeave) CBlankWindow(KRed);	//Window will be red if WSERV just draws in background color
		CleanupStack::PushL(win);
		TDisplayMode mode=EColor256;
		win->SetUpL(TPoint(),winSize,TheClient->iGroup,*TheClient->iGc,&mode);	//Window is activated
		win->RealDraw(ETrue);
		win->SetColor(KGreen);
		CBlankWindow* win2=new(ELeave) CBlankWindow(KRed);
		CleanupStack::PushL(win2);
		win2->SetUpL(TPoint(),winSize,TheClient->iGroup,*TheClient->iGc,&mode);	//New Window completely obscures other window
		win2->RealDraw(ETrue);
		win->CTWin::DrawNow();	//Window will be green if drawn from stored commands
		win2->CTWin::DrawNow();
		win2->SetVisible(EFalse);
		TRgb col;
		scrDev->GetPixel(col,TPoint(5,5));	//Pixel will be red if storing off by default and green otherwise
		if (col==KRed)
			{
			win->Win()->EnableRedrawStore(ETrue);
			win->CTWin::DrawNow();	//Create stored commands
			}
		else
			__ASSERT_ALWAYS(col==KGreen,AutoPanic(EAutoPanicRedrawStoring));
		win->SetColor(KBlue);
		TRect redrawRect(TSize(10,10));
		win->Invalidate(redrawRect);
		win->Win()->BeginRedraw(redrawRect);
		win->DrawNow(redrawRect);	//Top left of Window will be blue if it draws itself
		win->Win()->EndRedraw();
		win2->SetVisible(ETrue);
		win2->SetVisible(EFalse);
		scrDev->GetPixel(col,TPoint(5,5));	//Pixel will be red if stored commands were lost
		iRedrawType=EPartialRedraw_None;
		if (col!=KRed)
			{
			__ASSERT_ALWAYS(col==KBlue,AutoPanic(EAutoPanicRedrawStoring));
			TheClient->WaitForRedrawsToFinish();
			win2->SetVisible(ETrue);
			win2->SetVisible(EFalse);
			scrDev->GetPixel(col,TPoint(15,15));	//Pixel will be blue if partial redraw triggers full redraw
			iRedrawType=EPartialRedraw_PreserveStoredCmds;
			if (col!=KBlue)
				{
				__ASSERT_ALWAYS(col==KGreen,AutoPanic(EAutoPanicRedrawStoring));
				iRedrawType=EPartialRedraw_FullRedrawSupport;
				}
			}
		CleanupStack::PopAndDestroy(2,win);
		}
	return iRedrawType;*/
	return EPartialRedraw_FullRedrawSupport;
	}

void CTestBase::SetUpMember(TSpriteMember &aMember)
	{
	aMember.iMaskBitmap=NULL;
	aMember.iInvertMask=EFalse;
	aMember.iDrawMode=CGraphicsContext::EDrawModePEN;
	aMember.iOffset=TPoint();
	aMember.iInterval=TTimeIntervalMicroSeconds32(0);
	}

void CTestBase::SimulateKeyDownUpWithModifiers(TInt aScanCode,TUint aModifiers)
	{
	if (aModifiers&EModifierAlt)
		SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftFunc);
	if (aModifiers&EModifierCtrl)
		SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftCtrl);
	if (aModifiers&EModifierShift)
		SimulateKey(TRawEvent::EKeyDown,EStdKeyLeftShift);
	SimulateKeyDownUp(aScanCode);
	if (aModifiers&EModifierShift)
		SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftShift);
	if (aModifiers&EModifierCtrl)
		SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftCtrl);
	if (aModifiers&EModifierAlt)
		SimulateKey(TRawEvent::EKeyUp,EStdKeyLeftFunc);
	}

void CTestBase::SimulateKeyDownUp(TInt aScanCode)
	{
	__ASSERT_DEBUG(aScanCode<'a' || aScanCode>'z',AutoPanic(EAutoPanicScanCapital));
	SimulateKey(TRawEvent::EKeyDown,aScanCode);
	SimulateKey(TRawEvent::EKeyUp,aScanCode);
	}

void CTestBase::SimulatePointerDownUp(TInt aX, TInt aY)
	{
	SimulatePointer(TRawEvent::EButton1Down,aX,aY);
	SimulatePointer(TRawEvent::EButton1Up,aX,aY);
	}

void CTestBase::SimulateKey(TRawEvent::TType aType, TInt aScanCode)
	{
	TRawEvent rawEvent;
	rawEvent.Set(aType,aScanCode);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	}

/**
 * Determine if the configuration supports pointer event testing.
 * 
 * There are certain circumstances where we want to skip pointer event
 * testing because we are simulating pointer events, and don't want to
 * simulate a pointer event from an impossible co-ordinate.  We'd rather
 * just identify that there is no point in doing the test and skip over
 * to the next test case.
 * 
 * In particular, when a ROM configured with a digitiser is deployed on a
 * Naviengine, with hardware configuration DIP switches which say that there
 * is an external screen connected, then no touch pad is active.
 * The base port under these conditions returns a digitiser area (0,0,0,0)
 * 
 * @return ETrue if the configuration supports pointer event testing, otherwise
 *         return EFalse.
 */
TBool CTestBase::ConfigurationSupportsPointerEventTesting() const
    {
    if (iNormalPointerCursorArea.IsEmpty())
        {
        return EFalse;
        }
    return ETrue;
    }


void CTestBase::SimulatePointer(TRawEvent::TType aType, TInt aX, TInt aY)
	{
	__ASSERT_DEBUG(ConfigurationSupportsPointerEventTesting(), AutoPanic(EAutoPanicNoDigitiser));


#ifdef WSERV_TAUTO_LOG_POINTER_EVENTS
	TLogMessageText buf;
	_LIT(KLog,"SimulatePointer  Type=%d  Pos=(%d,%d)");
	buf.Format(KLog,aType,aX,aY);
	TheClient->LogMessage(buf);
#endif
	
	TRawEvent rawEvent;
	rawEvent.Set(aType,aX,aY);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	}

void CTestBase::SimulateEvent(TRawEvent::TType aType)
	{
	TRawEvent rawEvent;
	rawEvent.Set(aType);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	}

void CTestBase::LogColors(const CBitmapDevice& aDevice,TPoint aBasePoint, TPoint aStartPoint, TPoint aEndPoint)
	{
	_LIT(KPixel,"Pixel(%d,%d) R=%d G=%d B=%d");
	TLogMessageText buf;
	TBuf8<2560> screen;
	const TRgb* pixel;
	if (aStartPoint.iX==aEndPoint.iX)
		++aEndPoint.iX;
	if (aStartPoint.iY==aEndPoint.iY)
		++aEndPoint.iY;
	TInt width=aEndPoint.iX-aStartPoint.iX;
	TInt xx,yy;
	for (yy=aStartPoint.iY;yy<aEndPoint.iY;++yy)
		{
		xx=aStartPoint.iX;
		aDevice.GetScanLine(screen,aBasePoint+TPoint(xx,yy),width,ERgb);
		pixel=REINTERPRET_CAST(const TRgb*,screen.Ptr());
		for (;xx<aEndPoint.iX;++xx,++pixel)
			{
			buf.Format(KPixel,xx,yy,pixel->Red(),pixel->Green(),pixel->Blue());
			//RDebug::Print(buf);
			TheClient->iWs.LogMessage(buf);
			}
		}
	TheClient->iWs.Flush();
	}

void CTestBase::LogColors4(const CBitmapDevice& aDevice,TPoint aStartPoint,TInt aLen)
	{
	_LIT(KValue,"Pixel(%d,%d)  Byte %d,  Value %d");
	TLogMessageText buf;
	TBuf8<2560> screen;
	aDevice.GetScanLine(screen,aStartPoint,aLen,EGray4);
	TInt len=(aLen+3)/4;
	TInt ii;
	for (ii=0;ii<len;++ii,aStartPoint.iX+=4)
		{
		buf.Format(KValue,aStartPoint.iX,aStartPoint.iY,ii,screen[ii]);
		TheClient->iWs.LogMessage(buf);
		}
	}

void CTestBase::UpdateTestResults(TInt aNoOfTest, TInt aNoOfTestPass)
	{
	iNumberTests+=aNoOfTest;
	iNumberTestsPass+=aNoOfTestPass;
	}

TInt CTestBase::SaveScreen(const TDesC& aFileName)
	{
	return SaveScreen(aFileName,TheClient->iScreen->SizeInPixels(),TheClient->iScreen->DisplayMode());
	}

TInt CTestBase::SaveScreen(const TDesC& aFileName,const TSize& aScreenSize,TDisplayMode aColorDepth)
	{
	TRAPD(err,SaveScreenL(aFileName,aScreenSize,aColorDepth));
	return err;
	}

void CTestBase::SaveScreenL(const TDesC& aFileName,const TSize& aScreenSize,TDisplayMode aColorDepth)
	{
	CBitmap* copyOfScreen=CBitmap::NewLC(aScreenSize,aColorDepth);
	CFbsScreenDevice* scrDevice=CFbsScreenDevice::NewL(iScreenNumber,aColorDepth);
	CleanupStack::PushL(scrDevice);
	CFbsBitGc* gc;
	User::LeaveIfError(scrDevice->CreateContext(gc));
	CleanupStack::PushL(gc);
	copyOfScreen->Gc().BitBlt(TPoint(),*gc);
	User::LeaveIfError(copyOfScreen->Bitmap().Save(aFileName));
	CleanupStack::PopAndDestroy(3,copyOfScreen);
	}


/*CProcess*/
_LIT(KScreenTag,"Screen");

void CProcess::GetProcArg(const TWinCommand& aParam,TBufArg& aProcArg)
	{
	TInt pos = aParam.Find(KScreenTag);
	if (pos!=KErrNotFound)
		aProcArg = aParam.Left(pos-1);
	else
		aProcArg = aParam;
	}

void CProcess::GetScreenArg(const TWinCommand& aParam, TInt& aScreenArg)
	{
	TInt pos = aParam.Find(KScreenTag);
	if (pos!=KErrNotFound)
		{
		TBufArg secondArg(aParam.Right(aParam.Length()-pos));
		if (secondArg.Length()>6)
			{
			TBuf<1> digit(secondArg.Mid(6,1));
			TLex lex(digit);
			lex.Val(aScreenArg);
			}
		}
	}

TInt CProcess::Start(const TWinCommand& aParam)
	{
	// parse command line aParam to retrieve value of
	// screen number if it is specified
	//
	// command line format: <process-id> [screen<id>]
	//
	TBufArg procArg(_L(""));
	TInt screenArg = KDefaultScreen;

	GetProcArg(aParam, procArg);
	GetScreenArg(aParam, screenArg);

	TInt ii;
	for(ii=0;ii<eNumProcessCalls;ii++)
		{
		if (procArg==iFunctions[ii].iParam)
			{
			TRAPD(ret,iFunctions[ii].iFunction((TAny*)screenArg));

			// need to pass test result to owning process
			// for multiple display test
			if (ii==eProcessMultiDisplayTest)
				ret = TheTestResult;

			return ret;
			}
		}

	return KErrNone;		//Shouldn't get here
	}

CProcess* CProcess::NewL(TInt aFunctionNo,TInt aScreenNumber)
	{
	CProcess* self=new(ELeave) CProcess();
	CleanupStack::PushL(self);
	self->ConstructL(aFunctionNo,aScreenNumber);
	CleanupStack::Pop(self);
	return self;
	}

CProcess* CProcess::NewTL(TInt aFunctionNo,TInt aScreenNumber,TRequestStatus* aStatus /*=NULL*/)
	{
	CProcess* self=new(ELeave) CProcess();
	CleanupStack::PushL(self);
	self->ConstructTL(aFunctionNo,aScreenNumber,aStatus);
	CleanupStack::Pop(self);
	return self;
	}

CProcess* CProcess::NewThreadL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr
																						,TRequestStatus* aStatus)
	{
	CProcess* self=new(ELeave) CProcess();
	CleanupStack::PushL(self);
	self->ConstructL(aName,aFunction,aPtr,aStatus);
	CleanupStack::Pop(self);
	return self;
	}

CProcess* CProcess::NewThreadRendezvousL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr ,TRequestStatus* aLogonStatus,TRequestStatus& aRendesvouzStatus)
    {
    CProcess* self=new(ELeave) CProcess();
    CleanupStack::PushL(self);
    self->ConstructRendezvousL(aName,aFunction,aPtr,aLogonStatus,aRendesvouzStatus);
    CleanupStack::Pop(self);
    return self;
    }

CProcess* CProcess::NewThreadRendezvousL(const TDesC& aName,TThreadStartUp* aPtr,TRequestStatus& aRendesvouzStatus)
    {
    return NewThreadRendezvousL(aName,ThreadInit,aPtr,NULL,aRendesvouzStatus);
    }

CProcess* CProcess::NewThreadL(const TDesC& aName,TThreadStartUp* aPtr)
	{
	return NewThreadL(aName,ThreadInit,aPtr,NULL);
	}

CProcess* CProcess::NewSimpleThreadL(const TDesC& aName,TThreadStartUp* aPtr,TRequestStatus* aStatus/*=NULL*/)
	{
	return NewThreadL(aName,SimpleThreadInit,aPtr,aStatus);
	}

_LIT(KSpace," ");
_LIT(KScreenFormat,"%d");

void CProcess::ConstructL(TInt aFunctionNo,TInt aScreenNumber/*=KDefaultScreen*/)
	{
	Close();
	// add screen number into command line param
	// format: <proc-id> [screen<id>]
	//
	TBuf<100> commandLine;

	commandLine = iFunctions[aFunctionNo].iParam;
	commandLine.Append(KSpace);
	commandLine.Append(KScreenTag);
	commandLine.AppendFormat(KScreenFormat,aScreenNumber);
	User::LeaveIfError(iOther.Create(RProcess().FileName(),commandLine));
	iCreated|=eOtherCreated;
	iOther.Resume();
	}

void CProcess::ConstructTL(TInt aFunctionNo,TInt aScreenNumber/*=KDefaultScreen*/,TRequestStatus* aStatus)
	{
	Close();
	TUint flag=eThreadCreated;
	TInt err=iThread.Create(iFunctions[aFunctionNo].iParam,iFunctions[aFunctionNo].iFunction
													,KDefaultStackSize,KOtherProcHeapSize,KOtherProcHeapSize,(TAny*)aScreenNumber,EOwnerThread);
	User::LeaveIfError(err);
	iCreated|=flag;
	if (aStatus)
		Logon(*aStatus);
	iThread.Resume();
	}


void CProcess::ConstructL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr,TRequestStatus* aStatus)
	{
	Close();
	User::LeaveIfError(iThread.Create(aName,aFunction,KDefaultStackSize,KOtherProcHeapSize,KOtherProcHeapSize,aPtr,EOwnerThread));
	iCreated|=eThreadCreated;
	if (aStatus)
		Logon(*aStatus);
	iThread.Resume();
	}

void CProcess::ConstructRendezvousL(const TDesC& aName,TThreadFunction aFunction,TThreadStartUp* aPtr,TRequestStatus* aLogonStatus,TRequestStatus& aRendezvousStatus)
    {
    Close();
    User::LeaveIfError(iThread.Create(aName,aFunction,KDefaultStackSize,KOtherProcHeapSize,KOtherProcHeapSize,aPtr,EOwnerThread));
    iCreated|=eThreadCreated;
    if (aLogonStatus)
        Logon(*aLogonStatus);

    iThread.Rendezvous(aRendezvousStatus);     
    iThread.Resume();
    }

void CProcess::Logon(TRequestStatus& aStatus) const
	{
	if (iCreated&eThreadCreated)
		iThread.Logon(aStatus);
	else
		{
		iOther.Logon(aStatus);
		}
	}

void CProcess::Terminate(TInt aReason)
	{
	if (iCreated&eThreadCreated)
		iThread.Terminate(aReason);
	else
		{
		iOther.Terminate(aReason);
		}
	Close();
	}

void CProcess::Close()
	{
	if (iCreated&eOtherCreated)
		iOther.Close();
	if (iCreated&eThreadCreated)
		iThread.Close();
	iCreated=0;
	}

CProcess::~CProcess()
	{
	Close();
	}

TBool CProcess::StillAlive()
	{
	if (iCreated&eOtherCreated)
		return iOther.ExitType()==EExitPending;
	return iThread.ExitType()==EExitPending;
	}

void CProcess::LeaveIfDied()		//Can Leave
	{
	User::After(200000);		//0.2 secs
	if (StillAlive())
		return;
	if (iCreated&eOtherCreated)
		User::Leave(iOther.ExitReason());
	User::Leave(iThread.ExitReason());
	}

const TInt KFirstInstanceId = 1;
const TInt KOtherInstanceId = 2;

TBool CProcess::ProcessDied(TInt aScreenNo/*=KDefaultScreen*/)
	{
	_LIT(pName,"TAutoServer*");
	TFindProcess find(pName);
	TFullName name;

	TBool found = EFalse;
	TInt instanceId = aScreenNo==KDefaultScreen? KFirstInstanceId : KOtherInstanceId;
	// find the correct instance of the process
	// required in multi display test
	while (!found && find.Next(name)==KErrNone)
		{
		TPtrC scrId = name.Right(1);
		TInt id;
		TLex lex(scrId);
		lex.Val(id);
		if (id==instanceId)
			found = ETrue;
		}
	if (!found)
		return EFalse;

	RProcess p;
	p.Open(name);
	if (p.Id()!=RProcess().Id())
		return EFalse;
	p.Close();
	return (find.Next(name)!=KErrNone);
	}

TInt CProcess::ThreadInit(TAny *aPtr)
	{
	__UHEAP_MARK;
	TInt err=KErrNone;
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	if (CleanUpStack==NULL)
		err=KErrNoMemory;
	else
		{
		TRAP(err,InitialiseL(STATIC_CAST(TThreadStartUp*,aPtr)))
		delete CleanUpStack;
		}
	__UHEAP_MARKEND;
	return(err);
	}

void CProcess::InitialiseL(TThreadStartUp* aPtr)
	{
	CActiveScheduler* activeScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);
	CleanupStack::PushL(activeScheduler);
	aPtr->iInitFunction(aPtr->iParam);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(activeScheduler);
	}

TInt CProcess::SimpleThreadInit(TAny *aPtr)
	{
	__UHEAP_MARK;
	TInt err=KErrNone;
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	if (CleanUpStack==NULL)
		err=KErrNoMemory;
	else
		{
		TThreadStartUp* ptr=STATIC_CAST(TThreadStartUp*,aPtr);
		ptr->iInitFunction(ptr->iParam);
		delete CleanUpStack;
		}
	__UHEAP_MARKEND;
	return(err);
	}

/*CMinWin*/

CMinWin::CMinWin(TInt aScreenNo): iScreenNo(aScreenNo)
	{}

void CMinWin::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iScr=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScr->Construct(iScreenNo));
	iGroup=RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(8970+iScreenNo,ETrue));
	iWin=RWindow(iWs);
	User::LeaveIfError(iWin.Construct((RWindowTreeNode)iGroup,(TUint32)this));
	iRect=TSize(10,10);
	iWin.SetExtent(TPoint(0,0),iRect.Size());
	iWin.SetRequiredDisplayMode(EColor256);
	iWin.SetBackgroundColor(KRgbGreen);
	iWin.Activate();
	iGc=new(ELeave) CWindowGc(iScr);
	User::LeaveIfError(iGc->Construct());

	Draw(iRect);
	iWs.Flush();
	}

CMinWin::~CMinWin()
	{
	delete iGc;
	iWin.Close();
	iGroup.Close();
	delete iScr;
	iWs.Close();
	}

void CMinWin::Draw(const TRect& aRect)
	{
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbGreen);
	iGc->DrawRect(aRect);
	iGc->Deactivate();
	iWin.EndRedraw();
	}


/*CTimeOut*/

void CTimeOut::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CTimeOut::Start(TTimeIntervalMicroSeconds32 aInterval,TCallBack aCallBack)
	{
	iCallBack=aCallBack;
	After(aInterval);
	}

void CTimeOut::RunL()
	{
	iCallBack.CallBack();
	}

/* 
 * Simply returns the DisplayMode as a string. Used to display mode details on test results. 
 */
GLDEF_C TPtrC DisplayModeAsString(TDisplayMode aMode)
	{
	
	TPtrC modeAsString;
	
	switch(aMode)
		{
		case ENone:
			_LIT(KENoneMode,"ENone");
			modeAsString.Set(KENoneMode);
			break;
		case EGray2: 
			_LIT(KEGray2Mode,"EGray2");
			modeAsString.Set(KEGray2Mode);
			break;
		case EGray4:
			_LIT(KEGray4Mode,"EGray4");
			modeAsString.Set(KEGray4Mode);
			break;
		case EGray16:
			_LIT(KEGray16Mode,"EGray16");
			modeAsString.Set(KEGray16Mode);
			break;
		case EGray256:
			_LIT(KEGray256Mode,"EGray256");
			modeAsString.Set(KEGray256Mode);
			break;
		case EColor16:
			_LIT(KEColor16Mode,"EColor16");
			modeAsString.Set(KEColor16Mode);
			break;
		case EColor256:
			_LIT(KEColor256Mode,"EColor256");
			modeAsString.Set(KEColor256Mode);
			break;
		case EColor64K:
			_LIT(KEColor64KMode,"EColor64K");
			modeAsString.Set(KEColor64KMode);
			break;
		case EColor16M:
			_LIT(KEColor16MMode,"EColor16M");
			modeAsString.Set(KEColor16MMode);
			break;
		case EColor4K:
			_LIT(KEColor4KMode,"EColor4K");
			modeAsString.Set(KEColor4KMode);
			break;
		case EColor16MU:
			_LIT(KEColor16MUMode,"EColor16MU");
			modeAsString.Set(KEColor16MUMode);
			break;
		case EColor16MA:
			_LIT(KEColor16MAMode,"EColor16MA");
			modeAsString.Set(KEColor16MAMode);
			break;
		case EColor16MAP:
			_LIT(KEColor16MAPMode,"EColor16MAP");
			modeAsString.Set(KEColor16MAPMode);
			break;
		default:
			_LIT(KUnknownMode,"Unknown");
			modeAsString.Set(KUnknownMode);
			break;
		}
	
	return modeAsString;
	}

// Check if an area of a bitmap is of a certain color
GLDEF_C TBool LossyCheckBlankBitmap(const CFbsBitmap& aBitmap, const TRect aArea, const TRgb aCheckColor, TBool aLossyCompare)
	{
	const TReal errorLimit = (aLossyCompare ? 0.05 : 0.00); //Lossy(default) or exact compare?
	
	TBool result = ETrue;
	/* TInt mismatchedPixels = 0; */ // -- Useful for debugging
	TRgb bitmapPix = TRgb(0,0,0,0); //testWin Pixel
	for (TInt x = 0; x < aArea.Size().iWidth; x++)
		{
		for (TInt y = 0; y < aArea.Size().iHeight; y++)
			{
			aBitmap.GetPixel(bitmapPix, TPoint(x,y));
			
			//Check if there are differeces in color between the bitmap and the test color
			if(((TReal)abs(bitmapPix.Red() - aCheckColor.Red())/255) > errorLimit || 
					((TReal)abs(bitmapPix.Blue() - aCheckColor.Blue())/255) > errorLimit || 
					((TReal)abs(bitmapPix.Green() - aCheckColor.Green())/255) > errorLimit || 
					((TReal)abs(bitmapPix.Alpha() - aCheckColor.Alpha())/255) > errorLimit)
				{
				/* mismatchedPixels++; */ // -- Useful for debugging
				result = EFalse;
				break;
				}
			}
		}
	
	/* INFO_PRINTF2(_L("Number of different pixels: %i"), mismatchedPixels); */ // -- Useful for debugging
	return result;
	}

// Compare a section of two bitmaps
GLDEF_C TBool LossyCompareBitmap(const CFbsBitmap& aBitmap1, const CFbsBitmap& aBitmap2, const TRect aCompareRect, TBool aLossyCompare)
	{
	const TReal errorLimit = (aLossyCompare ? 0.05 : 0.00); //Lossy or Exact compare?
	
	TBool result = ETrue;

	if (aBitmap1.DisplayMode() != aBitmap2.DisplayMode())
		{
		RDebug::Printf(" DisplayMode difference %d, %d", aBitmap1.DisplayMode(), aBitmap2.DisplayMode());
		}
	
	TRgb bitmap1Pix = TRgb(0,0,0,0);
	TRgb bitmap2Pix = TRgb(0,0,0,0);
	for (TInt x = 0; x < aCompareRect.Size().iWidth; x++)
		{
		for (TInt y = 0; y < aCompareRect.Size().iHeight; y++)
			{
			aBitmap1.GetPixel(bitmap1Pix, TPoint(x,y));
			aBitmap2.GetPixel(bitmap2Pix, TPoint(x,y));
						
			//Check if there are differences between the colors of the two bitmaps
			if(((TReal)abs(bitmap1Pix.Red() - bitmap2Pix.Red())/255) > errorLimit || 
					((TReal)abs(bitmap1Pix.Blue() - bitmap2Pix.Blue())/255) > errorLimit || 
					((TReal)abs(bitmap1Pix.Green() - bitmap2Pix.Green())/255) > errorLimit || 
					((TReal)abs(bitmap1Pix.Alpha() - bitmap2Pix.Alpha())/255) > errorLimit)
				{
				/*
				 * There was a difference so return without checking the rest of the
				 * bitmap.  If you are seeing Lossy compare errors, and want to diagnose
				 * further, consider switching over to using a recording version of this
				 * function, LossyCompareBitmapRecord()
				 */
				result = EFalse;
				break;
				}
			}
		}
	
	return result;
	}

/**
 * Compare two bitmaps, optionally a lossy comparison, recording any differences and saving bitmaps
 * 
 * @param	aBitmap1			Bitmap being checked
 * @param	aBitmap2			Reference Bitmap
 * @param	aCompareRect		Area of bitmap to compare
 * @param	aLossyCompare		ETrue means use a lossy compare strategy, else do an exact compare
 * @param	aPixelsDifferent	Returned value representing the number of pixels which are different
 * @param	aLogger				Facility for logging to be reported by this function
 * @return						ETrue if the bitmaps are the same (or similar).
 * 								Otherwise EFalse, with logging reporting the differences, and bitmaps saved in c:\logs\
 * @pre 						c:\logs directory must exist
 */
GLDEF_C TBool LossyCompareBitmapRecord(CFbsBitmap& aBitmap1, CFbsBitmap& aBitmap2, const TRect aCompareRect, TBool aLossyCompare, TInt& aPixelsDifferent, CTestExecuteLogger& aLogger)
	{
	const TReal errorLimit = (aLossyCompare ? 0.05 : 0.00); //Lossy or Exact compare?
	
	TBool result = ETrue;
	TInt mismatchedPixels = 0;
	TRgb bitmap1Pix = TRgb(0,0,0,0);
	TRgb bitmap2Pix = TRgb(0,0,0,0);
	if (aBitmap1.DisplayMode() != aBitmap2.DisplayMode())
		{
		aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,
				_L(" DisplayMode difference %d, %d"), aBitmap1.DisplayMode(), aBitmap2.DisplayMode());
		}
	for (TInt x = 0; x < aCompareRect.Size().iWidth; x++)
		{
		for (TInt y = 0; y < aCompareRect.Size().iHeight; y++)
			{
			aBitmap1.GetPixel(bitmap1Pix, TPoint(x,y));
			aBitmap2.GetPixel(bitmap2Pix, TPoint(x,y));
						
			//Check if there are differences between the colors of the two bitmaps
			if(((TReal)abs(bitmap1Pix.Red() - bitmap2Pix.Red())/255) > errorLimit || 
					((TReal)abs(bitmap1Pix.Blue() - bitmap2Pix.Blue())/255) > errorLimit || 
					((TReal)abs(bitmap1Pix.Green() - bitmap2Pix.Green())/255) > errorLimit || 
					((TReal)abs(bitmap1Pix.Alpha() - bitmap2Pix.Alpha())/255) > errorLimit)
				{
				mismatchedPixels++;
				
				aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,
						_L(" Pixel difference %d,%d: %d"),x,y, bitmap1Pix.Difference(bitmap2Pix));
				result = EFalse;
				// we loop around again to pick up all the differences
				}
			}
		}
	aPixelsDifferent = mismatchedPixels;
	
	/*
	 * When the bitmaps are different, we store them locally in c:\\logs in
	 * timestamped files.  Save() is a non-const method; this is why aBitmap1
	 * and aBitmap2 are non-const.  Saving can fail, perhaps because we have
	 * exceeded storage limits.
	 */
	if (!result) 
		{
		TTime now;
		now.UniversalTime();
		TInt timestamp = I64INT(now.Int64() & 0x7fffffffu);
		timestamp/=1000; // a millisecond resolution is easier to track
		TFileName mbmFileSrc;
		mbmFileSrc.Format (_L("c:\\logs\\%d_LossyCompareBitmap1.mbm"), timestamp);
		TFileName mbmFileDst;
		mbmFileDst.Format (_L("c:\\logs\\%d_LossyCompareBitmap2.mbm"), timestamp); 
		TInt saveResult1;
		TInt saveResult2;
		saveResult1 = aBitmap1.Save(mbmFileSrc);
		saveResult2 = aBitmap2.Save(mbmFileDst);		
		if (saveResult1 == KErrNone && saveResult2 == KErrNone)
			{
			aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,
						_L(" Bitmaps are different: see %S, %S"), &mbmFileSrc, &mbmFileDst);				
			}
		else
			{
			aLogger.LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo,
						_L(" Bitmaps are different, but could not save files into c:\\logs : %d %d"), saveResult1, saveResult2);
			}
		}

	return result;
	}

// Check if an area of a screen is of a certain color
GLDEF_C TBool LossyCheckBlankWindow(const CWsScreenDevice& aScreen, CFbsBitmap& aBitmap, const TRect aArea, const TRgb aCheckColor)
	{
	aScreen.CopyScreenToBitmap(&aBitmap, aArea);
	return LossyCheckBlankBitmap(aBitmap, aArea, aCheckColor);
	}

// Compare a section of two windows on the screen
GLDEF_C TBool LossyCompareWindow(const CWsScreenDevice& aScreen, CFbsBitmap& aBitmap1, CFbsBitmap& aBitmap2, const TRect aCompareRect)
	{
	aScreen.CopyScreenToBitmap(&aBitmap1, aCompareRect);
	aScreen.CopyScreenToBitmap(&aBitmap2, aCompareRect);
	return LossyCompareBitmap(aBitmap1, aBitmap2, aCompareRect);
	}

