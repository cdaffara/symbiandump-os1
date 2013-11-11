// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests for various pointer cursors on different windows
// 
//

#include <e32std.h>
#include <hal.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class CPointerCursorWindow : public CTWin
	{
public:
	~CPointerCursorWindow();
	void SetChild(CPointerCursorWindow* aChild);
	inline CPointerCursorWindow* Child() {return iChild;}
	inline void SetBitmap(CFbsBitmap* aBitmap,TBool aTop=ETrue) {iBitmap=aBitmap;iTop=aTop;}
	void SetPointerCursor(CTPointerCursor* aCursor);
	//Virtual function from CTBaseWin
	void SetUpL(TPoint aPos,TSize aSize,CTWinBase *aParent,CWindowGc &aGc);
	void Draw();
	void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
private:
	CPointerCursorWindow* iChild;
	CFbsBitmap* iBitmap;
	TBool iTop;
	};

class CPointerCursorWindowGroup : public CTWindowGroup
	{
public:
	CPointerCursorWindowGroup(CTClient *aClient);
	void SetPointerCursor(CTPointerCursor* aCursor);
	void ConstructL();
	};

class CStatusWindow : public CTTitledWindow
	{
public:
	~CStatusWindow();
	void Construct(CTestBase* aTest);
	void DoDraw();
	//virtual functions from CTWinBase overridden by CTBaseWin
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	//virtual functions from CTBaseWin
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
	//virtual function from CTBaseWin overridden by CTTitledWindow
	void Draw();
private:
	void UpdateString1();
	void UpdateString2();
	void UpdateString3();
	void ChangeXyInputType();
	void ChangePointerCursorMode();
private:
	CTestBase* iTest;
	TBuf<16> iString1;
	TBuf<16> iString2;
	TBuf<16> iString3;
	TBuf<32> iString4;
	TRect iRect1;
	TRect iRect2;
	TRect iRect3;
	TRect iRect4;
	TXYInputType iXyInputType;
	TXYInputType iOriginalXyInputType;
	TPointerCursorMode iMode;
	TPointerCursorMode iOriginalMode;
	TBool iSimulatedPenDown;
	TBool iUpdateNeeded;
	TBool iExit;
	};

class CPointerCursorTest : public CTestBase
	{
	enum {eWindowGap=3,eChildWindowGap=2};
	enum {eNumPointerCursors=7};
public:
	CPointerCursorTest();
	~CPointerCursorTest();
	TestState DoTestL();
	void ConstructL();
private:
	CPointerCursorWindow *CreateWindowL(TInt aNum,TRect aLocation,CTWinBase *aGroup);
	void CreatePointerCursorsL();
	void CreateWindowsL(TSize aArea);
private:
	TSize iWinSize;
	TInt iState;
	CStatusWindow *iInfoWindow;
	CPointerCursorWindowGroup *iGroup1;
	CPointerCursorWindowGroup *iGroup2;
	CPointerCursorWindow *iWindows[6];
	CTPointerCursor *iCursors[eNumPointerCursors];
	CFbsBitmap *iBitmaps[eNumPointerCursors];
	};


/*CPointerCursorWindow*/

CPointerCursorWindow::~CPointerCursorWindow()
	{
	delete iChild;
	}

void CPointerCursorWindow::SetChild(CPointerCursorWindow* aChild)
	{
	iChild=aChild;
	iWin.SetShadowDisabled(ETrue);
	}

void CPointerCursorWindow::SetPointerCursor(CTPointerCursor* aCursor)
	{
	iWin.SetCustomPointerCursor(aCursor->PointerCursor());
	}

void CPointerCursorWindow::SetUpL(TPoint aPos,TSize aSize,CTWinBase *aParent,CWindowGc &aGc)
	{
	CTWin::SetUpL(aPos,aSize,aParent,aGc);
	iWin.SetShadowDisabled(ETrue);
	iWin.SetPointerGrab(EFalse);
	}

void CPointerCursorWindow::Draw()
	{
	iGc->Reset();
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->DrawRect(Size());
	if (iBitmap)
		{
		TPoint point(3,3);
		if (!iTop)
			point.iY=Size().iHeight-iBitmap->SizeInPixels().iHeight-3;
		iGc->BitBlt(point,iBitmap);
		}
	}

void CPointerCursorWindow::PointerL(const TPointerEvent&,const TTime&)
	{
	}


/*CPointerCursorWindowGroup*/

CPointerCursorWindowGroup::CPointerCursorWindowGroup(CTClient *aClient)
	:CTWindowGroup(aClient)
	{}

void CPointerCursorWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.EnableReceiptOfFocus(EFalse);
	}

void CPointerCursorWindowGroup::SetPointerCursor(CTPointerCursor* aCursor)
	{
	iGroupWin.SetCustomPointerCursor(aCursor->PointerCursor());
	}


/*CStatusWindow*/

CStatusWindow::~CStatusWindow()
	{
	Client()->iWs.SetPointerCursorMode(iOriginalMode);
#if defined(__WINS__)
	Client()->iWs.SimulateXyInputType(iOriginalXyInputType);
#endif
	}

void CStatusWindow::Construct(CTestBase* aTest)
	{
	_LIT(Title,"StatusWindow");
	_LIT(String4,"Click here to finish");
	iTest=aTest;
	TWindowTitle title(Title);
	SetTitle(title);
	TInt yy=iTitleHeight+iFont->HeightInPixels()+7;
	iRect1.SetRect(3,iTitleHeight+5,iSize.iWidth-1,yy);
	TInt yy1=yy+iFont->HeightInPixels()+2;
	iRect2.SetRect(3,yy+3,iSize.iWidth-1,yy1);
	TInt yy2=yy1+iFont->HeightInPixels()+2;
	iRect3.SetRect(3,yy1+3,iSize.iWidth-1,yy2);
	iRect4.SetRect(3,yy2+3,iSize.iWidth-1,yy2+iFont->HeightInPixels()+2);
	TMachineInfoV1Buf machineInfo;
	UserHal::MachineInfo(machineInfo);
	iXyInputType=machineInfo().iXYInputType;
	iOriginalXyInputType=iXyInputType;
	iSimulatedPenDown=EFalse;
	//
	iString3.Copy(KNullDesC);
	if (iXyInputType==EXYInputDeltaMouse)
		{
		if (HAL::Get(HALData::EMouseState,iSimulatedPenDown)==KErrNone)
			UpdateString3();
		}
	UpdateString1();
	iMode=Client()->iWs.PointerCursorMode();
	iOriginalMode=iMode;
	UpdateString2();
	iUpdateNeeded=EFalse;
	iString4.Copy(String4);
	iExit=EFalse;
	}

void CStatusWindow::SetUpL(TPoint aPos,TSize aSize,CTWinBase *aParent,CWindowGc &aGc)
	{
	CTTitledWindow::SetUpL(aPos,aSize,aParent,aGc);
	}

void CStatusWindow::WinKeyL(const TKeyEvent &aKey,const TTime&)
	{
	switch(aKey.iCode)
		{
		case EKeyEscape:
			iTest->Request();
			break;
		case 'M':
		case 'm':
			if (iXyInputType==EXYInputDeltaMouse)
				{
				if (HAL::Set(HALData::EMouseState,!iSimulatedPenDown)==KErrNone)
					{
					iSimulatedPenDown=!iSimulatedPenDown;
					UpdateString3();
					DoDraw();
					}
				}
			break;
	#if defined(__WINS__)
		case 'I':
		case 'i':
		case '1':
			ChangeXyInputType();
			Client()->iWs.SimulateXyInputType(iXyInputType);
			break;
	#endif
		case 'C':
		case 'c':
		case '2':
			ChangePointerCursorMode();
			Client()->iWs.SetPointerCursorMode(iMode);
			break;
		default:;
		}
	}

void CStatusWindow::PointerL(const TPointerEvent &aPointer,const TTime &aTime)
	{
#if defined(__WINS__)
	if (iRect1.Contains(aPointer.iPosition))
		{
		if (aPointer.iType==TPointerEvent::EButton1Down)
			ChangeXyInputType();
		}
	else
#endif
        if (iRect2.Contains(aPointer.iPosition))
		{
		if (aPointer.iType==TPointerEvent::EButton1Down)
			ChangePointerCursorMode();
		}
	else if (iRect4.Contains(aPointer.iPosition))
		iExit=ETrue;
	else
		CTTitledWindow::PointerL(aPointer,aTime);
	if ((iUpdateNeeded || iExit) && aPointer.iType==TPointerEvent::EButton1Up)
		{
		if (iExit)
			iTest->Request();
		else
			{
			Client()->iWs.SetPointerCursorMode(iMode);
		#if defined(__WINS__)
			Client()->iWs.SimulateXyInputType(iXyInputType);
		#endif
			Client()->iWs.Flush();
			iUpdateNeeded=EFalse;
			}
		}
	}

void CStatusWindow::Draw()
	{
	CTTitledWindow::Draw();
	TInt ascent=iFont->AscentInPixels()+1;
	iGc->DrawText(iString1,iRect1,ascent,CGraphicsContext::ELeft);
	iGc->DrawText(iString2,iRect2,ascent,CGraphicsContext::ELeft);
	iGc->DrawText(iString3,iRect3,ascent,CGraphicsContext::ELeft);
	iGc->DrawText(iString4,iRect4,ascent,CGraphicsContext::ELeft);
	}

void CStatusWindow::DoDraw()
	{
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	Draw();
	iGc->Deactivate();
	}

void CStatusWindow::UpdateString1()
	{
	_LIT(Text0,"No Pointer");
	_LIT(Text1,"Pen");
	_LIT(Text2,"Mouse");
	_LIT(Text3,"Relative Mouse");
	switch (iXyInputType)
		{
	case EXYInputNone:
		iString1.Copy(Text0);
		break;
	case EXYInputPointer:
		iString1.Copy(Text1);
		break;
	case EXYInputMouse:
		iString1.Copy(Text2);
		break;
	case EXYInputDeltaMouse:
		iString1.Copy(Text3);
		break;
		}
	}

void CStatusWindow::UpdateString2()
	{
	_LIT(Text0,"None");
	_LIT(Text1,"Fixed");
	_LIT(Text2,"Normal");
	_LIT(Text3,"Window");
	switch (iMode)
		{
	case EPointerCursorNone:
		iString2.Copy(Text0);
		break;
	case EPointerCursorFixed:
		iString2.Copy(Text1);
		break;
	case EPointerCursorNormal:
		iString2.Copy(Text2);
		break;
	case EPointerCursorWindow:
		iString2.Copy(Text3);
		break;
		}
	}

void CStatusWindow::UpdateString3()
	{
	if (iSimulatedPenDown)
		{
		_LIT(TextD,"Sim Pen Down");
		iString3.Copy(TextD);
		}
	else
		{
		_LIT(TextU,"Sim Pen Up");
		iString3.Copy(TextU);
		}
	}

void CStatusWindow::ChangeXyInputType()
	{
	if (iXyInputType==EXYInputMouse)
		iXyInputType=EXYInputPointer;
	else if (iXyInputType==EXYInputPointer)
		iXyInputType=EXYInputMouse;
	UpdateString1();
	DoDraw();
	iUpdateNeeded=ETrue;
	}

void CStatusWindow::ChangePointerCursorMode()
	{
	TInt mode=(iMode+1)%(EPointerCursorLastMode+1);
	iMode=STATIC_CAST(TPointerCursorMode,mode);
	UpdateString2();
	DoDraw();
	iUpdateNeeded=ETrue;
	}


/*Sprite Drawing Functions*/

#if defined(__WINS__)
	#define DRAW_COLOR 1
#else
	#define DRAW_COLOR 0
#endif

void DrawCross(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *aLineWidth)
	{
	TInt halfLineWidth=(*STATIC_CAST(TInt*,aLineWidth)+1)/2;
	TInt lineWidth=2*halfLineWidth+1;		//Must be odd
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 3));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : 0));
	aGc->SetPenSize(TSize(lineWidth,lineWidth));
	aGc->DrawLine(TPoint(halfLineWidth,halfLineWidth),TPoint(aSize.iWidth-lineWidth,aSize.iHeight-lineWidth));
	aGc->DrawLine(TPoint(halfLineWidth,aSize.iHeight-lineWidth),TPoint(aSize.iWidth-lineWidth,halfLineWidth));
	}

void DrawArrow(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *aLineWidth)
	{
	TInt lineWidth=*STATIC_CAST(TInt*,aLineWidth);
	TInt halfLineWidth=(lineWidth-1)/2;
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 3));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : 0));
	aGc->SetPenSize(TSize(1,1));
	aGc->DrawLine(TPoint(0,0),TPoint(lineWidth,0));
	aGc->DrawLine(TPoint(0,0),TPoint(0,lineWidth));
	aGc->SetPenSize(TSize(lineWidth,lineWidth));
	aGc->DrawLine(TPoint(halfLineWidth,halfLineWidth),TPoint(aSize.iWidth-halfLineWidth-1,aSize.iHeight-halfLineWidth-1));
	aGc->DrawLine(TPoint(halfLineWidth,halfLineWidth),TPoint(aSize.iWidth/2,halfLineWidth));
	aGc->DrawLine(TPoint(halfLineWidth,halfLineWidth),TPoint(halfLineWidth,aSize.iHeight/2));
	}

void DrawSquare(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 3));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 3 : DRAW_COLOR));
	aGc->DrawRect(TRect(1,1,aSize.iWidth-1,aSize.iHeight-1));
	}

void DrawCircle(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 3));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 3 : 1));
	aGc->DrawEllipse(TRect(1,1,aSize.iWidth-1,aSize.iHeight-1));
	}

void DrawTriangle(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 3));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : DRAW_COLOR));
	aGc->SetPenSize(TSize(1,1));
	for(TInt y=0;y<aSize.iHeight;y++)
		{
		TInt xfact=aSize.iWidth*y/aSize.iHeight;
		aGc->DrawLine(TPoint(aSize.iWidth-xfact-1,y),TPoint(aSize.iWidth-1,y));
		}
	}

void DrawOpenSquare(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *aLineWidth)
	{
	TInt halfLineWidth=*STATIC_CAST(TInt*,aLineWidth)/2;
	TInt lineWidth=2*halfLineWidth+1;		//Must be odd
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 3));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : DRAW_COLOR));
	aGc->SetPenSize(TSize(lineWidth,lineWidth));
	TPoint botRig(aSize.iWidth-aSize.iWidth/4-1,aSize.iHeight-aSize.iHeight/4-1);
	aGc->DrawRect(TRect(aSize.iWidth/4,aSize.iHeight/4,botRig.iX+1,botRig.iY+1));
	aGc->DrawLine(TPoint(halfLineWidth,halfLineWidth),TPoint(aSize.iWidth/4-halfLineWidth,aSize.iHeight/4-halfLineWidth));
	aGc->DrawLine(TPoint(aSize.iWidth-1-halfLineWidth,halfLineWidth),TPoint(botRig.iX+halfLineWidth,aSize.iHeight/4-halfLineWidth));
	aGc->DrawLine(TPoint(aSize.iWidth-1-halfLineWidth,aSize.iHeight-1-halfLineWidth),TPoint(botRig.iX+halfLineWidth,botRig.iY+halfLineWidth));
	aGc->DrawLine(TPoint(halfLineWidth,aSize.iHeight-1-halfLineWidth),TPoint(aSize.iWidth/4-halfLineWidth,botRig.iY+halfLineWidth));
	}

void DrawOpenCircle(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *aLineWidth)
	{
	TInt halfLineWidth=*STATIC_CAST(TInt*,aLineWidth)/2;
	TInt lineWidth=2*halfLineWidth+1;		//Must be odd
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 3));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : DRAW_COLOR));
	aGc->SetPenSize(TSize(lineWidth,lineWidth));
	TPoint botRig(aSize.iWidth-aSize.iWidth/4-1,aSize.iHeight-aSize.iHeight/4-1);
	aGc->DrawEllipse(TRect(aSize.iWidth/4+1,aSize.iHeight/4+1,botRig.iX+1,botRig.iY+1));
	aGc->DrawLine(TPoint(aSize.iWidth/2,1+halfLineWidth),TPoint(aSize.iWidth/2,aSize.iHeight/4+1-halfLineWidth));
	aGc->DrawLine(TPoint(aSize.iWidth/2,aSize.iHeight-1-halfLineWidth),TPoint(aSize.iWidth/2,botRig.iY+halfLineWidth));
	aGc->DrawLine(TPoint(1+halfLineWidth,aSize.iWidth/2),TPoint(aSize.iWidth/4+1-halfLineWidth,aSize.iHeight/2));
	aGc->DrawLine(TPoint(aSize.iWidth-1-halfLineWidth,aSize.iHeight/2),TPoint(botRig.iX+halfLineWidth,aSize.iWidth/2));
	}


/*CPointerCursorTest*/

GLDEF_C CTestBase *CreatePointerCursorTest()
	{
	return(new(ELeave) CPointerCursorTest());
	}

CPointerCursorTest::CPointerCursorTest() : CTestBase(_L("PointerCursor"))
	{}

CPointerCursorTest::~CPointerCursorTest()
	{
	TInt ii;
	Client()->iGroup->SetCurrentWindow(NULL);
	delete iInfoWindow;
	for(ii=0;ii<6;++ii)
		{
		delete iWindows[ii];
		}
	for(ii=0;ii<eNumPointerCursors;++ii)
		{
		delete iCursors[ii];
		delete iBitmaps[ii];
		}
	delete iGroup1;
	delete iGroup2;
	}

void CPointerCursorTest::ConstructL()
	{
	TSize size=Client()->iScreen->SizeInPixels();
	TInt infoWidth=Min(210,5*size.iWidth/12);
	size.iWidth=Max(Min(415,size.iWidth-infoWidth),85);
	iInfoWindow=new(ELeave) CStatusWindow();
	iInfoWindow->SetUpL(TPoint(size.iWidth,50),TSize(infoWidth,180),Client()->iGroup,*Client()->iGc);
	Client()->iGroup->SetCurrentWindow(iInfoWindow);
	iInfoWindow->Construct(this);
	iGroup1=new(ELeave) CPointerCursorWindowGroup(Client());
	iGroup1->ConstructL();
	iGroup2=new(ELeave) CPointerCursorWindowGroup(Client());
	iGroup2->ConstructL();
	size.iWidth-=5;
	CreateWindowsL(size);
	CreatePointerCursorsL();
	iGroup1->SetPointerCursor(iCursors[0]);
	iWindows[1]->SetBitmap(iBitmaps[0],EFalse);
	iWindows[0]->SetPointerCursor(iCursors[1]);
	iWindows[0]->SetBitmap(iBitmaps[1]);
	iWindows[0]->Child()->SetPointerCursor(iCursors[2]);
	iWindows[0]->Child()->SetBitmap(iBitmaps[2]);
	iWindows[2]->SetPointerCursor(iCursors[3]);
	iWindows[2]->SetBitmap(iBitmaps[3]);
	iWindows[2]->Child()->Child()->SetPointerCursor(iCursors[4]);
	iWindows[2]->Child()->Child()->SetBitmap(iBitmaps[4]);
	iWindows[3]->Child()->SetPointerCursor(iCursors[5]);
	iWindows[3]->Child()->SetBitmap(iBitmaps[5]);
	iWindows[4]->SetPointerCursor(iCursors[6]);
	iWindows[4]->SetBitmap(iBitmaps[6]);
	}

void CPointerCursorTest::CreateWindowsL(TSize aArea)
	{
	//TSize screenSize=Client()->iScreen->SizeInPixels();
	TInt height=eWindowGap+(aArea.iHeight-5*eWindowGap)/4;
	TInt halfWidth=aArea.iWidth/2;
	TRect rect(5,eWindowGap,halfWidth,height);
	iWindows[0]=CreateWindowL(2,rect,iGroup1);
	rect.Move(halfWidth,0);
	iWindows[1]=CreateWindowL(2,rect,iGroup1);
	rect.Move(-halfWidth,height);
	rect.iBr.iX=aArea.iWidth;
	iWindows[2]=CreateWindowL(4,rect,iGroup2);
	rect.Move(0,height);
	rect.iBr.iX=aArea.iWidth-11;
	iWindows[3]=CreateWindowL(3,rect,iGroup2);
	rect.Move(0,height);
	rect.iBr.iX=halfWidth;
	iWindows[4]=CreateWindowL(2,rect,iGroup2);
	rect.Move(halfWidth,0);
	iWindows[5]=CreateWindowL(2,rect,iGroup2);
	}

CPointerCursorWindow* CPointerCursorTest::CreateWindowL(TInt aNum,TRect aLocation,CTWinBase *aGroup)
	{
	CPointerCursorWindow* firstWin=NULL;
	CPointerCursorWindow* parent=NULL;
	CPointerCursorWindow* win;
	TInt xInc=aLocation.Width()/aNum-eChildWindowGap;
	TInt ii;
	for (ii=aNum;ii>0;ii--)
		{
		win=new(ELeave) CPointerCursorWindow();
		CleanupStack::PushL(win);
		win->SetUpL(aLocation.iTl,aLocation.Size(),aGroup,*Client()->iGc);
		if (!parent)
			firstWin=win;
		else
			{
			parent->SetChild(win);
			CleanupStack::Pop();
			}
		aLocation.iBr=TPoint(-eChildWindowGap,-eChildWindowGap)+aLocation.Size();
		aLocation.iTl.iX=xInc;
		aLocation.iTl.iY=eChildWindowGap;
		aGroup=win;
		parent=win;
		}
	CleanupStack::Pop();
	return firstWin;
	}

void CPointerCursorTest::CreatePointerCursorsL()
	{
	const TSize size(32,32);
	TSpriteCreateParams params(size,TPoint(-16,-16),DrawSquare);
	TSpriteCreateParams paramarray[eNumPointerCursors];
	TInt lineWidth1=3;
	TInt lineWidth2=5;
	paramarray[2]=params;
	params.iDrawFunc=DrawCircle;
	paramarray[3]=params;
	params.iDrawFunc=DrawOpenSquare;
	params.iDrawFuncParam=&lineWidth1;
	paramarray[5]=params;
	params.iDrawFunc=DrawOpenCircle;
	paramarray[6]=params;
	params.iOffset=TPoint(0,0);
	params.iDrawFunc=DrawArrow;
	params.iDrawFuncParam=&lineWidth2;
	paramarray[1]=params;
	params.iOffset.iX=-31;
	params.iDrawFunc=DrawTriangle;
	paramarray[4]=params;
	params.iOffset=TPoint(-15,-15);
	params.iDrawFunc=DrawCross;
	paramarray[0]=params;
	CFbsBitmap *bitmap;
	TInt ii,jj;
	TDisplayMode mode=Client()->iWs.GetDefModeMaxNumColors(ii,jj);
	for (ii=0;ii<eNumPointerCursors;++ii)
		{
		bitmap=NULL;
		iBitmaps[ii]=new(ELeave) CFbsBitmap();
		User::LeaveIfError(iBitmaps[ii]->Create(size,mode));
		iCursors[ii]=new(ELeave) CTPointerCursor(Client()->iWs);
		iCursors[ii]->ConstructL(1,&paramarray[ii],0,iBitmaps[ii],bitmap);
		delete bitmap;
		}
	}

TestState CPointerCursorTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Pointer Cursor"),1);
			TestL(ETrue);
			iState++;
			return(EContinue);
		default:
			return(EFinished);
		}
	}
