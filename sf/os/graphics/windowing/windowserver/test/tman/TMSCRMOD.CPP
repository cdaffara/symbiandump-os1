// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class CScreenModeTest;

class CScreenModeWindow : public CTWin
	{
public:
	CScreenModeWindow();
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Draw();
	void ScreenDeviceChanged();
private:
	TInt iSubState;
	};

class CScreenModeGroup : public CTWindowGroup
	{
public:
	~CScreenModeGroup();
	CScreenModeGroup(CTClient *aClient);
	void ConstructL();
	void ScreenDeviceChanged();
private:
	CScreenModeWindow *iWindow;
	};

class CScreenModeTest : public CTestBase
	{
public:
	CScreenModeTest();
	~CScreenModeTest();
	TestState DoTestL();
	void ConstructL();
private:
	CScreenModeGroup *iScreenModeGroup;
	TSize iWinSize;
	TInt iState;
	};

GLDEF_C CTestBase *CreateScreenModeTest()
	{
	return(new(ELeave) CScreenModeTest());
	}

//
// CScreenModeWindow
//

CScreenModeWindow::CScreenModeWindow() : CTWin()
	{}

void CScreenModeWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape || aKey.iCode==EKeyEnter)
		{
		if (iSubState==0)
			{
			iSubState=1;
			ScreenDeviceChanged();
			Invalidate();
			}
		else
			CActiveScheduler::Stop();
		}
	}

void CScreenModeWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	Activate();
	AssignGC(aGc);
	}

void CScreenModeWindow::Draw()
	{
	iGc->Clear();
	TSize winSize(Win()->Size());
	iGc->DrawRect(winSize);
	iGc->DrawLine(TPoint(0,0),TPoint(winSize.iWidth,winSize.iHeight));
	iGc->DrawLine(TPoint(0,winSize.iHeight),TPoint(winSize.iWidth,0));
	TInt xpos=winSize.iWidth/2-100;
	iGc->DrawText(iSubState==0?_L("Twips not adjusted,"):
								_L("Twips adjusted,"),TPoint(xpos,20));
	iGc->DrawText(_L("Cycle through all screen modes,"),TPoint(xpos,40));
	iGc->DrawText(_L("Check the window is resized and rotated correctly,"),TPoint(xpos,60));
	iGc->DrawText(_L("Then press <Enter> when finished"),TPoint(xpos,80));
//
	TSize twips=Client()->iScreen->SizeInTwips();
	TSize pixels=Client()->iScreen->SizeInPixels();
// Horizontal line
	TInt inches=twips.iWidth/KTwipsPerInch-1;
	TInt lineLen=Client()->iScreen->HorizontalTwipsToPixels(inches*KTwipsPerInch);
	TPoint linePos=TPoint((pixels.iWidth-lineLen)/2,pixels.iHeight/2);
	iGc->DrawLine(linePos,linePos+TPoint(lineLen,0));
	TBuf<0x20> buf;
	buf.Format(TRefByValue<const TDesC>(_L("Width %d\"")),inches);
	iGc->DrawText(buf,TPoint((pixels.iWidth-iFont->TextWidthInPixels(buf))/2,linePos.iY-iFont->HeightInPixels()+iFont->AscentInPixels()-2));
	TInt index;
	for(index=0;index<=inches;index++)
		{
		TInt dx=Client()->iScreen->HorizontalTwipsToPixels(index*KTwipsPerInch);
		TInt dy=Client()->iScreen->VerticalTwipsToPixels(KTwipsPerInch/(index==0 || index==inches ? 8 : 16));
		iGc->DrawLine(linePos+TPoint(dx,1), linePos+TPoint(dx,dy));
		}
// Vertical line
	inches=twips.iHeight/KTwipsPerInch;
	lineLen=Client()->iScreen->VerticalTwipsToPixels(inches*KTwipsPerInch);
	linePos.iY=(pixels.iHeight-lineLen)/2;
	iGc->DrawLine(linePos,linePos+TPoint(0,lineLen));
	buf.Format(TRefByValue<const TDesC>(_L("Height %d\"")),inches);
	iGc->DrawText(buf,TPoint(linePos.iX+10, pixels.iHeight/4));
	for(index=0;index<=inches;index++)
		{
		TInt dx=Client()->iScreen->HorizontalTwipsToPixels(KTwipsPerInch/(index==0 || index==inches ? 8 : 16));
		TInt dy=Client()->iScreen->VerticalTwipsToPixels(index*KTwipsPerInch);
		iGc->DrawLine(linePos+TPoint(1,dy), linePos+TPoint(dx,dy));
		}

	}

void CScreenModeWindow::ScreenDeviceChanged()
	{
	if (iSubState==0)
		{
		TPixelsAndRotation sizeAndRotation;
		Client()->iScreen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
		Client()->iScreen->SetScreenSizeAndRotation(sizeAndRotation);
		}
	else
		{
		TPixelsTwipsAndRotation sizeAndRotation;
		Client()->iScreen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
		Client()->iScreen->SetScreenSizeAndRotation(sizeAndRotation);
		}
	SetSize(Client()->iScreen->SizeInPixels());
	}

//
// CScreenModeGroup
//

CScreenModeGroup::~CScreenModeGroup()
	{
	GroupWin()->EnableReceiptOfFocus(EFalse);
	ClearCurrentWindow();
	Client()->iGroup->SetCurrentWindow(NULL);
	CTWin::Delete(iWindow);
	}

CScreenModeGroup::CScreenModeGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{}

void CScreenModeGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iWindow=new(ELeave) CScreenModeWindow;
	iWindow->SetUpLD(TPoint(0,0),Client()->iScreen->SizeInPixels(),this,*Client()->iGc);
	SetCurrentWindow(iWindow);
	GroupWin()->EnableScreenChangeEvents();
	GroupWin()->SetOrdinalPosition(0);
	}

void CScreenModeGroup::ScreenDeviceChanged()
	{
	iWindow->ScreenDeviceChanged();
	}

//
// CScreenModeTest
//

CScreenModeTest::CScreenModeTest() : CTestBase(_L("Screen mode"))
	{}

CScreenModeTest::~CScreenModeTest()
	{
	Client()->iGroup->GroupWin()->EnableScreenChangeEvents();
	Client()->iScreen->SetScreenMode(0);
	delete iScreenModeGroup;
	}

void CScreenModeTest::ConstructL()
	{
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0);
	Client()->iGroup->GroupWin()->DisableScreenChangeEvents();
//
	iScreenModeGroup=new(ELeave) CScreenModeGroup(Client());
	iScreenModeGroup->ConstructL();
	}

TestState CScreenModeTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Screen mode 1"),1);
 			CActiveScheduler::Start();
			iState++;
			break;
		default:
			return(EFinished);
		}
	return(ENext);
	}

