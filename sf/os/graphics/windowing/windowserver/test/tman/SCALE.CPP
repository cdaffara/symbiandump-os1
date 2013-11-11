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
// Test GDI scaling (pixels<->twips) functions
// You can probably delete this test as it is now done by TMSCRMOD for each screen mode.
// 
//

#include <e32std.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class TScaleTest;

class CScaleWindow : public CTWin
	{
public:
	CScaleWindow(TScaleTest *aTest);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	TScaleTest *iTest;
	};

class TScaleTest : public CTestBase
	{
public:
	TScaleTest();
	~TScaleTest();
	TestState DoTestL();
	void ConstructL();
private:
	CScaleWindow *iWin;
	TSize iWinSize;
	TInt iState;
	};

GLDEF_C CTestBase *CreateScaleTest()
	{
	return(new(ELeave) TScaleTest());
	}

CScaleWindow::CScaleWindow(TScaleTest *aTest) : CTWin(), iTest(aTest)
	{}

void CScaleWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	Activate();
	AssignGC(aGc);
	}

void CScaleWindow::Draw()
	{
	iGc->Clear();
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

void CScaleWindow::WinKeyL(const TKeyEvent &,const TTime &)
	{
	CActiveScheduler::Stop();
	}

TScaleTest::TScaleTest() : CTestBase(_L("Scale"))
	{}

TScaleTest::~TScaleTest()
	{
	CTWin::Delete(iWin);
	}

void TScaleTest::ConstructL()
	{
	CScaleWindow *win=new(ELeave) CScaleWindow(this);
	win->SetUpLD(TPoint(0,0),Client()->iScreen->SizeInPixels(),Client()->iGroup,*Client()->iGc);
	iWin=win;
	Client()->iGroup->SetCurrentWindow(iWin);
	}

TestState TScaleTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Scale 1"),1);
			CActiveScheduler::Start();
			iState++;
			break;
		default:
			return(EFinished);
		}
	return(ENext);
	}

