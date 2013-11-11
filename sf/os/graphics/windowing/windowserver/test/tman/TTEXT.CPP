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
// Print some text, to be checked by eye
// 
//

#include <e32std.h>
#include <e32svr.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class TTextTest;

enum {EDrawModeFonts,EDrawModeCharJust,EDrawModeWordJust};

_LIT(KTestFontTypefaceName,"DejaVu Sans Condensed");

class CTextWindow : public CTWin
	{
public:
	CTextWindow(TTextTest *aTest);
	void SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void ResetPrintLine();
	void Print(const CFont *aFont, const TDesC &aText);
	void PrintLine(const CFont *aFont, const TDesC &aText);
	void PrintDivider();
	void PrintStylesL(const TDesC &aText, TFontSpec &aFontSpec, const TFontStyle &aFontStyle);
	void DrawCharJustified(const TDesC &aText);
	void DrawWordJustified(const TDesC &aText);
	TBool NextPage();
	void SetDrawMode(TInt aMode);
private:
	TInt iDrawMode;
	TTextTest *iTest;
	CFbsFont *iTmpFont;
	TInt iNumTypeFaces;
	TInt iTypeFaceIndex;
	TInt iXStartPos;
	TInt iYpos;
	TInt iXpos;
	};

class TTextTest : public CTestBase
	{
public:
	TTextTest();
	~TTextTest();
	TestState DoTestL();
	void ConstructL();
private:
	CTextWindow *iWin;
	TSize iWinSize;
	TInt iState;
	};

GLDEF_C CTestBase *CreateTextTest()
	{
	return(new(ELeave) TTextTest());
	}

CTextWindow::CTextWindow(TTextTest *aTest) : CTWin(), iDrawMode(EDrawModeWordJust), iTest(aTest)
	{}

void CTextWindow::SetUpLD(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	Activate();
	AssignGC(aGc);
	iNumTypeFaces=Client()->iScreen->NumTypefaces();
	}

void CTextWindow::ResetPrintLine()
	{
	iXpos=iXStartPos=5;
	iYpos=2;
	}

void CTextWindow::PrintDivider()
	{
	iGc->DrawLine(TPoint(0,iYpos+5),TPoint(Size().iWidth,iYpos+5));
	iYpos+=10;
	}

void CTextWindow::Print(const CFont *aFont, const TDesC &aText)
	{
	iGc->DrawText(aText, TPoint(iXpos, iYpos+aFont->AscentInPixels()));
	iXpos+=aFont->TextWidthInPixels(aText);
	}

void CTextWindow::PrintLine(const CFont *aFont, const TDesC &aText)
	{
	iGc->DrawText(aText, TPoint(iXpos, iYpos+aFont->AscentInPixels()));
	iXpos=iXStartPos;
	iYpos+=aFont->HeightInPixels()+2;
	}

TBool CTextWindow::NextPage()
	{
	if (iTypeFaceIndex==(iNumTypeFaces-1))
		return(ETrue);
	++iTypeFaceIndex;
	return(EFalse);
	}

void CTextWindow::PrintStylesL(const TDesC &aText, TFontSpec &aFontSpec, const TFontStyle &aFontStyle)
	{
	aFontSpec.iFontStyle=aFontStyle;
	User::LeaveIfError(Client()->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iTmpFont, aFontSpec));
	iGc->UseFont(iTmpFont);
	Print(iTmpFont,aText);
	iGc->SetUnderlineStyle(EUnderlineOn);
	Print(iTmpFont,_L("Underline, "));
	iGc->SetStrikethroughStyle(EStrikethroughOn);
	Print(iTmpFont,_L("Strikethrough/underline, "));
	iGc->SetUnderlineStyle(EUnderlineOff);
	PrintLine(iTmpFont,_L("Strikethrough"));
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	Client()->iScreen->ReleaseFont(iTmpFont);
	iTmpFont=NULL;
	}

void CTextWindow::DrawCharJustified(const TDesC &aText)
	{
	iGc->SetCharJustification(Size().iWidth-10-iTmpFont->TextWidthInPixels(aText),aText.Length()-1);
	PrintLine(iTmpFont, aText);
	}

void CTextWindow::DrawWordJustified(const TDesC &aText)
	{
	TInt count=0;
	for(TInt index=0;index<aText.Length();index++)
		if (aText[index]==' ')
			count++;
	iGc->SetWordJustification(Size().iWidth-10-iTmpFont->TextWidthInPixels(aText),count);
	PrintLine(iTmpFont, aText);
	}

void CTextWindow::Draw()
//This function is virtual and so cannot have an 'L' at the end of it's name
	{
	iGc->Clear();
	ResetPrintLine();
	switch(iDrawMode)
		{
	case EDrawModeWordJust:
		User::LeaveIfError(Client()->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iTmpFont, TFontSpec(KTestFontTypefaceName,200)));
		iGc->UseFont(iTmpFont);
		DrawWordJustified(_L("Hello World"));
		DrawWordJustified(_L("One Two Three Four Five Six Seven"));
		DrawWordJustified(_L("AA    B        CC D"));
		DrawWordJustified(_L("ONEWORD"));
		iGc->DiscardFont();
		Client()->iScreen->ReleaseFont(iTmpFont);
		iTmpFont=NULL;
		break;
	case EDrawModeCharJust:
		User::LeaveIfError(Client()->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iTmpFont, TFontSpec(KTestFontTypefaceName,200)));
		iGc->UseFont(iTmpFont);
		DrawCharJustified(_L("Hello World"));
		DrawCharJustified(_L("One Two Three Four Five Six Seven"));
		DrawCharJustified(_L("AA    B        CC D"));
		DrawCharJustified(_L("ONEWORD"));
		iGc->DiscardFont();
		Client()->iScreen->ReleaseFont(iTmpFont);
		iTmpFont=NULL;
		break;
	case EDrawModeFonts:
		{
		TTypefaceSupport typefaceSupport;
		Client()->iScreen->TypefaceSupport(typefaceSupport,iTypeFaceIndex);
		TBuf<0x40> title;
		TBuf16<KMaxTypefaceNameLength> tmpBuf;
		tmpBuf.Copy(typefaceSupport.iTypeface.iName);
		title.Append(tmpBuf);
		title.AppendFormat(TRefByValue<const TDesC>(_L(", Heights (Min=%d, Max=%d, Num=%d)")),typefaceSupport.iMinHeightInTwips,typefaceSupport.iMaxHeightInTwips,typefaceSupport.iNumHeights);
		PrintLine(iFont,title);
		PrintDivider();
		for (TInt tfHeight=0;tfHeight<typefaceSupport.iNumHeights;tfHeight++)
			{
			TFontSpec fspec(typefaceSupport.iTypeface.iName,Client()->iScreen->FontHeightInTwips(iTypeFaceIndex,tfHeight));
			PrintStylesL(_L("Normal, "), fspec, TFontStyle());
			PrintStylesL(_L("Bold, "), fspec, TFontStyle(EPostureUpright,EStrokeWeightBold,EPrintPosNormal));
			PrintStylesL(_L("Italic, "), fspec, TFontStyle(EPostureItalic,EStrokeWeightNormal,EPrintPosNormal));
			PrintStylesL(_L("Bold/italic, "), fspec, TFontStyle(EPostureItalic,EStrokeWeightBold,EPrintPosNormal));
			if (iYpos>Size().iHeight)
				break;
			}
		}
		break;
		}
	}

void CTextWindow::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (iDrawMode!=EDrawModeFonts || (aKey.iCode==EKeyEscape || NextPage()))
		CActiveScheduler::Stop();
	else
		iWin.Invalidate();
	}

void CTextWindow::SetDrawMode(TInt aDrawMode)
	{
	iDrawMode=aDrawMode;
	iWin.Invalidate();
	}

TTextTest::TTextTest() : CTestBase(_L("Text"))
	{}

TTextTest::~TTextTest()
	{
	CTWin::Delete(iWin);
	}

void TTextTest::ConstructL()
	{
	CTextWindow *win=new(ELeave) CTextWindow(this);
	win->SetUpLD(TPoint(0,0),Client()->iScreen->SizeInPixels(),Client()->iGroup,*Client()->iGc);
	iWin=win;
	Client()->iGroup->SetCurrentWindow(iWin);
	Client()->iGroup->GroupWin()->SetOrdinalPosition(0);
	}

TestState TTextTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("SetWordJustification"),1);
			iWin->SetDrawMode(EDrawModeWordJust);
 			CActiveScheduler::Start();
			iState++;
			break;
		case 1:
			LogSubTest(_L("SetCharJustification"),2);
			iWin->SetDrawMode(EDrawModeCharJust);
			CActiveScheduler::Start();
			iState++;
			break;
		case 2:
			LogSubTest(_L("Text 1"),3);
			iWin->SetDrawMode(EDrawModeFonts);
			CActiveScheduler::Start();
			iState++;
			break;
		default:
			return(EFinished);
		}
	return(ENext);
	}

