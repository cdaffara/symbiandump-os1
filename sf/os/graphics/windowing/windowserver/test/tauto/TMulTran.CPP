// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test multiple level transparent window
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TMulTran.H"


LOCAL_D TDisplayMode DisplayMode=EColor64K;
LOCAL_D TSize WinSize;

static const TRgb KRed=TRgb(255, 0, 0, 128);
static const TRgb KGreen=TRgb(0, 255, 0, 128);
static const TRgb KYellow=TRgb(255, 255, 0, 128);
static const TRgb KBlue=TRgb(0, 0, 255, 128);
static const TRgb KCyan=TRgb(0, 255, 255, 128);

/*CTransWindow*/

CTransWindow* CTransWindow::NewL(RPointerArray<CTransWindow>& aWindows,TRgb aColor,TRect aPos)
	{
	CTransWindow* self=NewL(TheClient->iGroup,aColor,aPos,&DisplayMode);
	aWindows.Insert(self,0);
	return self;
	}

CTransWindow* CTransWindow::NewL(CTWinBase* aParent,TRgb aColor,TRect aPos, TDisplayMode* aDisplayMode)
	{
	CTransWindow* self=new(ELeave) CTransWindow(aColor);
	CleanupStack::PushL(self);
	self->SetUpL(aPos.iTl,aPos.Size(),aParent,*TheClient->iGc,aDisplayMode);
	User::LeaveIfError(self->iError);
	CleanupStack::Pop(self);
	return self;
	}

//the following function doesn't activate a window
CTransWindow* CTransWindow::NewL(CTWinBase* aParent,TRgb aColor,TDisplayMode* aDisplayMode)
	{
	CTransWindow* self=new(ELeave) CTransWindow(aColor);
	CleanupStack::PushL(self);
	self->ConstructL(*aParent);
	if (aDisplayMode)
		{
		self->BaseWin()->SetRequiredDisplayMode(*aDisplayMode);
		}
	User::LeaveIfError((self->iError) && (self->iError != KErrNotSupported));
	CleanupStack::Pop(self);
	return self;
	}

void CTransWindow::InitWin()
	{
	AdjustShadow(-1);
	iShadowHight=0;
	iWin.SetShadowDisabled(ETrue);
	iShadowing=EFalse;
	iError = iWin.SetTransparencyAlphaChannel();
	iWin.SetBackgroundColor(iCol);
	iDrawOpaque=EFalse;
	}

void CTransWindow::ToggleVisibility()
	{
	iVisible=!iVisible;
	SetVisible(iVisible);
	}

void CTransWindow::SetOrdinal(RPointerArray<CTransWindow>& aWindows,TInt aOldPos,TInt aNewPos)
	{
	CTransWindow* win=aWindows[aOldPos];
	aWindows.Remove(aOldPos);
	aWindows.Insert(win,aNewPos);
	win->iWin.SetOrdinalPosition(aNewPos);
	}

void CTransWindow::SetShadowDisabled(TBool aState)
	{
	iWin.SetShadowDisabled(aState);
	iShadowing=!aState;
	}

void CTransWindow::AdjustShadow(TInt aAdjust)
	{
	CBlankWindow::AdjustShadow(aAdjust);
	iShadowHight+=aAdjust;
	}

TPoint CTransWindow::Position() const
	{
	return CBlankWindow::Position()+iPosOffset;
	}

void CTransWindow::Draw()
	{
	//don't call CBlankWindow::Draw() since the background is already drawn

	if(iDrawOpaque)
		{
		iGc->SetPenStyle(CGraphicsContext::ESolidPen);
		iGc->SetPenSize(TSize(4,4));
		iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		iGc->SetPenColor(~iCol);
		iGc->SetOpaque(ETrue);
		iGc->DrawLine(TPoint(0,0),TPoint(1000,1000));
		iGc->SetOpaque(EFalse);
		}
	}

/*CTMultipleTrans*/

CTMultipleTrans::CTMultipleTrans(CTestStep* aStep) : CTWsGraphicsBase(aStep), iWindows(5), iShadows(EFalse)
	{
	}

CTMultipleTrans::~CTMultipleTrans()
	{
	((CTMultipleTransStep*)iStep)->CloseTMSGraphicsStep();
	delete iBackgroundWin;
	delete iCheckWin;
	delete iCheckBitmap;
	iWindows.ResetAndDestroy();
	iBlankWin.Close();
	}

void CTMultipleTrans::ConstructL()
	{
	if(TransparencySupportedL() == KErrNotSupported)
		return;
	iMajorTest=0;
	const TSize scrSize(TheClient->iScreen->SizeInPixels());
	WinSize.SetSize(scrSize.iWidth/2,scrSize.iHeight);
	iBackgroundWin=new(ELeave) CTBlankWindow();
	iBackgroundWin->SetUpL(TPoint(),WinSize,TheClient->iGroup,*TheClient->iGc);
	TheClient->WaitForRedrawsToFinish();
	CTBackedUpWin* checkWin=new(ELeave) CTBackedUpWin(DisplayMode);
	checkWin->ConstructExtLD(*TheClient->iGroup,TPoint(WinSize.iWidth,0),WinSize);
	iCheckWin=checkWin;
	RBackedUpWindow& backWin=*iCheckWin->BackedUpWin();
	backWin.SetShadowHeight(0);
	iCheckWin->Activate();
	backWin.MaintainBackup();
	iCheckBitmap=CBitmap::NewL(backWin.BitmapHandle());
	iCheckBitmap->Gc().SetPenStyle(CGraphicsContext::ENullPen);
	iCheckBitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	RecreateWindowsL();
	iBlankWin=RBlankWindow(TheClient->iWs);
	User::LeaveIfError(iBlankWin.Construct(*TheClient->iGroup->WinTreeNode(),ENullWsHandle));
	iBlankWinVis=EFalse;
	iBlankWin.SetVisible(iBlankWinVis);
	User::LeaveIfError(iBlankWin.SetRequiredDisplayMode(EColor256));
	iBlankWinCol=TRgb(170,170,170);		//Grey
	iBlankWin.SetColor(iBlankWinCol);
	iBlankWin.SetOrdinalPosition(0,1);		//Set the priority to 1 so that the 4 main windows are the first 4 with priory 0
	iBlankWin.Activate();
	iOpacitySupported=EFalse;
	}

void CTMultipleTrans::ShadowRegion(TRegion& aRegion,TInt aWin)
	{
	TRect rect;
	TInt height;
	TInt window;
	aRegion.Clear();
	RRegion single;
	RRegion noShadow1;
	RRegion noShadow2;
	RRegion temp;
	RRegion temp2;
	for (window=0;window<aWin;++window)
		{
		CTransWindow& win=*iWindows[window];
		if (win.IsVisible())
			{
			rect.SetRect(win.Position(),win.Size());
			height=win.ShadowHeight();
			temp.Copy(single);
			temp.ClipRect(rect);
			noShadow1.Union(temp);
			temp.Clear();
			temp.AddRect(rect);
			temp.SubRegion(noShadow1);
			temp.SubRegion(noShadow2);
			temp.SubRegion(aRegion,&temp2);
			noShadow2.Union(temp2);
			temp2.Clear();
			single.Union(temp);
			if (win.IsShadowing())
				aRegion.AddRect(rect);
			aRegion.Offset(2*height,2*height);
			aRegion.Tidy();
			noShadow1.Tidy();
			noShadow2.Tidy();
			single.Tidy();
			}
		}
	if (window<iWindows.Count())
		{
		CTransWindow& win=*iWindows[window];
		rect.SetRect(win.Position(),win.Size());
		aRegion.ClipRect(rect);
		}
	aRegion.SubRegion(noShadow1);
	aRegion.SubRegion(noShadow2);
	aRegion.Tidy();
	noShadow1.Close();
	noShadow2.Close();
	single.Close();
	temp.Close();
	temp2.Close();
	}

void CTMultipleTrans::CheckDisplay()
	{
	RRegion shadowRegion;
	CFbsBitGc& gc=iCheckBitmap->Gc();
	gc.SetBrushColor(KRgbWhite);
	gc.DrawRect(TRect(WinSize));
	TInt windows=iWindows.Count();
	if (iShadows)
		{
		ShadowRegion(shadowRegion,windows);
		gc.ShadowArea(&shadowRegion);
		}
	TInt window;
	for (window=windows;window>0;)
		{
		CTransWindow& win=*iWindows[--window];
		if (win.IsVisible())
			{
			gc.SetBrushColor(win.Color());
			const TPoint& winTopLeft=win.Position();
			gc.DrawRect(TRect(winTopLeft, win.Size()));

			if(win.DrawOpaque())
				{
				gc.SetPenColor(~(win.Color()));
				gc.SetPenSize(TSize(4,4));
				gc.SetPenStyle(CGraphicsContext::ESolidPen);
				gc.SetClippingRect(TRect(TPoint(winTopLeft.iX,winTopLeft.iY),TPoint(winTopLeft.iX + win.Size().iWidth, winTopLeft.iY + win.Size().iHeight)));
				gc.DrawLine(TPoint(winTopLeft.iX,winTopLeft.iY),TPoint(winTopLeft.iX+1000, winTopLeft.iY+1000));
				gc.SetPenStyle(CGraphicsContext::ENullPen);
				gc.SetClippingRect(TRect(WinSize));
				}
			
			if (iShadows)
				{
				ShadowRegion(shadowRegion,window);
				gc.ShadowArea(&shadowRegion);
				}
			}
		}
	iCheckWin->BackedUpWin()->UpdateScreen();
	TheClient->Flush();
	if (windows>=4)
		{
		AdvancedCheckRect();
		}
	_LIT(KTest,"Multiple Transparent Test, SubTest %d, OpacityOn=%d");
	TBuf<128> buf;
	buf.Format(KTest,iTest->iState,iMajorTest);
	CheckRect(iBackgroundWin,iCheckWin,TRect(WinSize),buf);
	shadowRegion.Close();
	}

#define OFFSET 2
void CTMultipleTrans::AdvancedCheckRect()
	{
	const TSize compareSize(5,5);
	const TInt OFFSET2=OFFSET+compareSize.iWidth;
	TPoint comparePos=iCheckWin->Position();
	TPoint checkPos;
	//Check Single Level for Top Left Win
	checkPos=iTopLeft->Position()+TPoint(OFFSET,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Check Single Level for Top Right Win
	checkPos=iTopRight->Position()+TPoint(iTopRight->Size().iWidth-OFFSET2,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Check Single Level for Bottom Left Win
	checkPos=iBotLeft->Position()+TPoint(OFFSET,iBotLeft->Size().iHeight-OFFSET2);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Check Single Level for Bottom Right Win
	checkPos=iBotRight->Position()+TPoint(iBotRight->Size().iWidth-OFFSET2,iBotRight->Size().iHeight-OFFSET2);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Double level top 2 windows
	checkPos=iTopRight->Position()+TPoint(OFFSET,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Double level left 2 windows
	checkPos=iBotLeft->Position()+TPoint(OFFSET,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Double level bot 2 windows
	checkPos=iBotRight->Position()+TPoint(OFFSET,iBotRight->Size().iHeight-OFFSET2);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Double level right 2 windows
	checkPos=iBotRight->Position()+TPoint(iBotRight->Size().iWidth-OFFSET2,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Triple level all but bot right
	checkPos=iTopRight->Position()+TPoint(OFFSET,iTopRight->Size().iHeight-OFFSET2);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Triple level all but bot left
	checkPos=iBotRight->Position()+TPoint(OFFSET,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Triple level all but top right
	checkPos=iTopLeft->Position()+TPoint(iTopLeft->Size().iWidth-OFFSET2,iTopLeft->Size().iHeight-OFFSET2);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Triple level all but top left
	checkPos=iBotLeft->Position()+TPoint(iBotLeft->Size().iWidth-OFFSET2,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	//Quad level
	checkPos=TPoint(iBotRight->Position().iX,iBotLeft->Position().iY)+TPoint(OFFSET,OFFSET);
	CheckRect(checkPos,checkPos+comparePos,compareSize);
	}

void CTMultipleTrans::SimpleTest()
	{
	CheckDisplay();
	}

void CTMultipleTrans::ChangeVisibility()
	{
	TUint toggle;
	TUint toggle2;
	TInt win;
	TInt ii=0;
	for (ii=2;ii>0;--ii)
		{
		for (toggle=1;toggle<16;++toggle)
			{
			win=0;
			toggle2=toggle;
			while ((toggle2&0x1)==0)
				{
				++win;
				toggle2/=2;
				}
			iWindows[win]->ToggleVisibility();
			CheckDisplay();
			}
		}
	}

void CTMultipleTrans::ChangeVisibility2()
	{
	iBlankWin.SetOrdinalPosition(0,0);
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);
	CheckDisplay();
	const TInt xSteps=4;
	const TInt ySteps=3;
	iBlankWinSize.SetSize(WinSize.iWidth/xSteps,WinSize.iHeight/ySteps);
	iBlankWin.SetSize(iBlankWinSize);
	TInt ordPos,xx,yy;
	for (ordPos=0;ordPos<=4;++ordPos)
		{
		iBlankWin.SetOrdinalPosition(ordPos);
		for (xx=0;xx<xSteps;++xx)
			{
			TInt xPos=xx*WinSize.iWidth/xSteps;
			for (yy=0;yy<ySteps;++yy)
				{
				iBlankWin.SetPosition(TPoint(xPos,yy*WinSize.iHeight/ySteps));
				iBlankWin.SetVisible(ETrue);
				iBlankWin.SetVisible(EFalse);
				CheckDisplay();
				}
			}
		}
	iBlankWin.SetOrdinalPosition(0,1);
	}

void CTMultipleTrans::OrdinalPos()
	{
	TInt ii,jj,kk;
	for (kk=2;kk>=0;--kk)
		{
		for (jj=3;jj>kk;--jj)
			{
			for (ii=jj;ii>=kk;--ii)
				{
				CTransWindow::SetOrdinal(iWindows,jj,kk);
				CheckDisplay();
				}
			}
		}
	for (kk=2;kk>=0;--kk)
		{
		for (jj=3;jj>kk;--jj)
			{
			for (ii=jj;ii>=kk;--ii)
				{
				CTransWindow::SetOrdinal(iWindows,kk,jj);
				CheckDisplay();
				}
			}
		}
	}

TSize CTMultipleTrans::GetSize(TInt aPos)
	{		//Comments show values for 640x240 screen
	const TInt halfSize=40;
	const TPoint topLeft=iTopLeft->Position();	//10,10
	const TInt xLeft=WinSize.iWidth/2-topLeft.iX-halfSize;		//110
	const TInt xRight=xLeft+2*halfSize;		//190
	const TInt yTop=WinSize.iHeight/2-topLeft.iY-halfSize;		//70
	const TInt yBot=yTop+2*halfSize;		//150
	if (aPos<6)
		return TSize(xLeft+16*aPos-8,yTop);		//1->118, 5->182
	if (aPos<11)
		return TSize(xRight,yTop+16*aPos-88);		//6->78, 10->142
	if (aPos<16)
		return TSize(xRight-16*aPos+168,yBot);		//11->182, 15->118
	return TSize(xLeft,yBot-16*aPos+248);			//16->142, 20->78
	}

void CTMultipleTrans::SizeChange(TInt aJump)
	{
	TInt ii=aJump;
	TSize size;
	while (ii<20)
		{
		size=GetSize(ii);
		iTopLeft->SetSize(size);
		if(iTopLeft->DrawOpaque())
			{
			iTopLeft->CTWin::DrawNow();
			TheClient->Flush();
			}
		CheckDisplay();
		ii+=aJump;
		if (ii>20)
			ii-=20;	
		}
	}

void CTMultipleTrans::SizeChange()
	{
	CTransWindow::SetOrdinal(iWindows,2,1);
	TSize size=iBotLeft->Size();
	TInt change;
	for (change=70;change>=0;change-=14)
		{
		iBotLeft->SetSize(TSize(size.iWidth-change,size.iHeight));
		if(iBotLeft->DrawOpaque())
			{
			iBotLeft->CTWin::DrawNow();
			TheClient->Flush();
			}
		CheckDisplay();
		}
	size=iTopRight->Size();
	for (change=25;change>-50;change-=14)
		{
		iTopRight->SetSize(TSize(size.iWidth,size.iHeight+change));
		if(iTopRight->DrawOpaque())
			{
			iTopRight->CTWin::DrawNow();
			TheClient->Flush();
			}
		CheckDisplay();
		}
	iTopRight->SetSize(size);
	if(iTopRight->DrawOpaque())
		{
		iTopRight->CTWin::DrawNow();
		TheClient->Flush();
		}
	CTransWindow::SetOrdinal(iWindows,1,0);
	CheckDisplay();
	size=iTopLeft->Size();
	SizeChange(3);
	iTopLeft->SetSize(size);
	if(iTopLeft->DrawOpaque())
		{
		iTopLeft->CTWin::DrawNow();
		TheClient->Flush();
		}
	CheckDisplay();
	CTransWindow::SetOrdinal(iWindows,0,2);
	CheckDisplay();
	SizeChange(7);
	iTopLeft->SetSize(size);
	if(iTopLeft->DrawOpaque())
		{
		iTopLeft->CTWin::DrawNow();
		TheClient->Flush();
		}
	CheckDisplay();
	}

void CTMultipleTrans::MoveWin()
	{
	const TPoint topLeftPos=iTopLeft->Position();
	const TPoint topRightPos=iTopRight->Position();
	const TPoint botLeftPos=iBotLeft->Position();
	const TPoint botRightPos=iBotRight->Position();
	const TInt maxMove=Min(topRightPos.iX,botRightPos.iY)-10;
	TInt steps,move;
	for (steps=2;steps>=0;--steps)
		{
		move=maxMove*(3-steps)/3;
		iTopLeft->SetPos(TPoint(topLeftPos.iX+move,topLeftPos.iY+move));
		CheckDisplay();
		iTopRight->SetPos(TPoint(topRightPos.iX-move,topRightPos.iY+move));
		CheckDisplay();
		iBotRight->SetPos(TPoint(botRightPos.iX-move,botRightPos.iY-move));
		CheckDisplay();
		iBotLeft->SetPos(TPoint(botLeftPos.iX+move,botLeftPos.iY-move));
		CheckDisplay();
		}
	for (steps=4;steps>=0;--steps)
		{
		move=maxMove*steps/5;
		iBotLeft->SetPos(TPoint(botLeftPos.iX+move,botLeftPos.iY-move));
		CheckDisplay();
		iTopLeft->SetPos(TPoint(topLeftPos.iX+move,topLeftPos.iY+move));
		CheckDisplay();
		iBotRight->SetPos(TPoint(botRightPos.iX-move,botRightPos.iY-move));
		CheckDisplay();
		iTopRight->SetPos(TPoint(topRightPos.iX-move,topRightPos.iY+move));
		CheckDisplay();
		}
	}

void CTMultipleTrans::GroupWinL()
	{
	CTWindowGroup* group=new(ELeave) CTWindowGroup(TheClient);
	CleanupStack::PushL(group);
	group->ConstructL();
	CTransWindow* win=CTransWindow::NewL(group,KCyan,TRect(40,40,WinSize.iWidth-40,WinSize.iHeight-40),&DisplayMode);
	win->CTWin::DrawNow();
	if(iMajorTest==1)
		{
		win->SetDrawOpaque(ETrue);
		win->Invalidate();
		TheClient->WaitForRedrawsToFinish();
		}
	iWindows.Insert(win,0);
	CheckDisplay();
	group->GroupWin()->SetOrdinalPosition(1);
	iWindows.Remove(0);
	CheckDisplay();
	group->GroupWin()->SetOrdinalPosition(0);
	iWindows.Insert(win,0);
	CheckDisplay();
	iWindows.Remove(0);
	delete win;		//Would be better to delete this after the group window but the test window don't support this behaviour currently.
	CleanupStack::PopAndDestroy(group);
	CheckDisplay();
	}

void CTMultipleTrans::FadeTranWin()
	{
	// system fade on
	TheClient->iWs.SetSystemFaded(ETrue);
	// system fade off
	TheClient->iWs.SetSystemFaded(EFalse);
	CheckDisplay();
	}

void CTMultipleTrans::StartShadows()
	{
	iShadows=ETrue;
	iTopLeft->SetShadowDisabled(EFalse);
	iTopLeft->AdjustShadow(1);
	iTopRight->SetShadowDisabled(EFalse);
	iTopRight->AdjustShadow(1);
	iBotLeft->SetShadowDisabled(EFalse);
	iBotLeft->AdjustShadow(1);
	iBotRight->SetShadowDisabled(EFalse);
	iBotRight->AdjustShadow(1);
	if(!iOpacitySupported)
		{
		iTopRight->ToggleVisibility();		// In 8.1 this isn't needed.
		iTopRight->ToggleVisibility();
		}
	CheckDisplay();
	}

void CTMultipleTrans::ShadowsOnOff()
	{
	if(!iOpacitySupported)
		return;								// In 8.1 this isn't needed.
	iBotRight->SetShadowDisabled(EFalse);
	iBotRight->AdjustShadow(1);
	CheckDisplay();
	iBotRight->SetShadowDisabled(ETrue);
	CheckDisplay();
	iTopLeft->SetShadowDisabled(EFalse);
	iTopLeft->AdjustShadow(1);
	CheckDisplay();
	iTopLeft->SetShadowDisabled(ETrue);
	CheckDisplay();
	iTopRight->SetShadowDisabled(ETrue);
	CheckDisplay();
	iBotLeft->SetShadowDisabled(EFalse);
	iBotLeft->AdjustShadow(1);
	CheckDisplay();
	iBotLeft->SetShadowDisabled(ETrue);
	CheckDisplay();
	iTopLeft->SetShadowDisabled(EFalse);
	iTopRight->SetShadowDisabled(EFalse);
	iBotLeft->SetShadowDisabled(EFalse);
	iBotRight->SetShadowDisabled(EFalse);
	iTopLeft->ToggleVisibility();
	CheckDisplay();
	iTopLeft->ToggleVisibility();
	CheckDisplay();
	iTopRight->ToggleVisibility();
	CheckDisplay();
	iTopRight->ToggleVisibility();
	CheckDisplay();
	iBotLeft->ToggleVisibility();
	CheckDisplay();
	iBotLeft->ToggleVisibility();
	CheckDisplay();
	iBotRight->ToggleVisibility();
	CheckDisplay();
	iBotRight->ToggleVisibility();
	CheckDisplay();
	}

void CTMultipleTrans::StartChildL()
	{
	CTransWindow* win;
	TInt windows=iWindows.Count();
	TInt window;
	//Delete the first 4 windows from the array, could use ResetAndDestroy but would be broken by other uses of the array
	for (window=windows;window>0;--window)
		{
		win=iWindows[0];
		iWindows.Remove(0);
		delete win;
		}
	RecreateWindowsL(iBackgroundWin);
	CheckDisplay();	
	}

void CTMultipleTrans::StartChild2L()
	{
	CTransWindow* win;
	TInt windows=iWindows.Count();
	TInt window;
	//Delete the first 4 windows from the array, could use ResetAndDestroy but would be broken by other uses of the array
	for (window=windows;window>0;--window)
		{
		win=iWindows[0];
		iWindows.Remove(0);
		delete win;
		}
	iShadows=EFalse;
	TRect rect=WinSize;
	rect.Shrink(10,10);
	iBotRight=CTransWindow::NewL(iBackgroundWin,KBlue,rect,&DisplayMode);
	iBotRight->CTWin::DrawNow();
	iWindows.Insert(iBotRight,0);
	CheckDisplay();	
	rect.Resize(-20,-20);
	iTopLeft=CTransWindow::NewL(iBotRight,KRed,rect,&DisplayMode);
	iTopLeft->CTWin::DrawNow();
	iTopLeft->SetPosOffset(TPoint(10,10));
	iWindows.Insert(iTopLeft,0);
	CheckDisplay();	
	rect.Resize(-20,-20);
	iTopRight=CTransWindow::NewL(iTopLeft,KGreen,rect,&DisplayMode);
	iTopRight->CTWin::DrawNow();
	iTopRight->SetPosOffset(TPoint(20,20));
	iWindows.Insert(iTopRight,0);
	CheckDisplay();	
	rect.Resize(-20,-20);
	iBotLeft=CTransWindow::NewL(iTopRight,KYellow,rect,&DisplayMode);
	iBotLeft->CTWin::DrawNow();
	iBotLeft->SetPosOffset(TPoint(30,30));
	iWindows.Insert(iBotLeft,0);
	if(iMajorTest==1)
		{
		for (TInt window = 0; window < iWindows.Count(); ++window)
			{
			iWindows[window]->SetDrawOpaque(ETrue);
			iWindows[window]->Invalidate();
			}
		TheClient->WaitForRedrawsToFinish();
		}
	CheckDisplay();	
	}
	
void CTMultipleTrans::RecreateWindowsL(CTWinBase *aParent)
	{
	CTransWindow* win;
	TInt windows=iWindows.Count();
	TInt window;
	//Delete existing windows:
	for (window=windows;window>0;--window)
		{
		win=iWindows[0];
		iWindows.Remove(0);
		delete win;
		}

	if(!aParent)
		aParent = TheClient->iGroup;
		
	iBotRight=CTransWindow::NewL(aParent,KBlue,TRect(WinSize.iWidth/2-10,WinSize.iHeight/2-30,WinSize.iWidth-20,WinSize.iHeight-20),&DisplayMode);
	iBotRight->CTWin::DrawNow();
	iWindows.Insert(iBotRight,0);
	iTopLeft=CTransWindow::NewL(aParent,KRed,TRect(10,10,WinSize.iWidth/2+10,WinSize.iHeight/2+30),&DisplayMode);
	iTopLeft->CTWin::DrawNow();
	iWindows.Insert(iTopLeft,0);
	iTopRight=CTransWindow::NewL(aParent,KGreen,TRect(WinSize.iWidth/2-30,20,WinSize.iWidth-10,WinSize.iHeight/2+10),&DisplayMode);
	iTopRight->CTWin::DrawNow();
	iWindows.Insert(iTopRight,0);
	iBotLeft=CTransWindow::NewL(aParent,KYellow,TRect(20,WinSize.iHeight/2-10,WinSize.iWidth/2+30,WinSize.iHeight-10),&DisplayMode);
	iBotLeft->CTWin::DrawNow();
	iWindows.Insert(iBotLeft,0);
	iShadows=EFalse;

	if(iMajorTest==1)
		{
		for (TInt window=0;window<iWindows.Count();++window)
			{
			iWindows[window]->SetDrawOpaque(ETrue);
			iWindows[window]->Invalidate();
			}
		TheClient->WaitForRedrawsToFinish();
		}
	}

void CTMultipleTrans::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KCheck,"Check");
	_LIT(KSimple,"Simple Test");
	_LIT(KVisibility,"Change Visibility");
	_LIT(KVisibility2,"Change Visibility 2");
	_LIT(KOrdinal,"Ordinal Position");
	_LIT(KSizeChange,"Size Change");
	_LIT(KMoveWin,"Move Window");
	_LIT(KGroupWin,"Group Window Ordinal");
	_LIT(KBackground,"Background Color Changes");
	_LIT(KShadowsOnOff,"Shadows On Off");
	_LIT(KChild1,"Child 1");
	_LIT(KChild2,"Child 2");
	_LIT(KOpacity,"Opaque Drawing");
	_LIT(KTranWinFade,"Fading Transparent Windows");
	((CTMultipleTransStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(iTest->iState)
		{
		case 0:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0500
*/
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0500"));
			// Check to see if Transparency is enabled before running tests
			iTest->LogSubTest(KCheck);
			if (TransparencySupportedL()==KErrNotSupported)
				{
				LOG_MESSAGE(_L("Transparency is not supported\n"));
				TestComplete();
				return;
				}
			++iTest->iState;		//Fall Through
/**
@SYMTestCaseID		GRAPHICS-WSERV-0111

@SYMDEF             DEF081259

@SYMTestCaseDesc    Simple transparent windows test.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) are created before the test on the screen's left side.
					The test draws the windows on the screen's right side using FBS bitmaps direcly.
					The test does nothing if transparency is not enabled on the screen.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 1:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0111"));
			iTest->LogSubTest(KSimple);
			SimpleTest();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0112

@SYMDEF             DEF081259

@SYMTestCaseDesc    On/off switching of visibility of transparent windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) are created before the test on the screen's left side.
					The test toggles visibility of those four windows going through all
					combinations. For each combination it draws visible windows on the screen's right side using FBS bitmaps direcly.
					The test does nothing if transparency is not enabled on the screen.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 2:
		case 23:
		case 42:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0112"));
			iTest->LogSubTest(KVisibility);
			ChangeVisibility();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0113

@SYMDEF             DEF081259

@SYMTestCaseDesc    On/off switching of visibility of a small transparent window on top of existing transparent windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) are created before the test on the screen's left side.
					Toggles visibility of 5th blank little window making this appear in different places 
					with different ordinal position. Draws visible windows on the screen's right side using FBS bitmaps direcly
					each time when the 5th window becomes invisible.
					The test does nothing if transparency is not enabled on the screen.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 3:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0113"));
			iTest->LogSubTest(KVisibility2);
			ChangeVisibility2();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0114

@SYMDEF             DEF081259

@SYMTestCaseDesc    Ordinal positions' switches of intersecting transparent windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) are created before the test on the screen's left side.
					Switches ordinal positions of the four windows trying all combinations. Draws visible windows on the screen's right side
					using FBS bitmaps direcly each time when a new combination is applied.
					The test does nothing if transparency is not enabled on the screen.


@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 4:
		case 43:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0114"));
			iTest->LogSubTest(KOrdinal);
			OrdinalPos();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0115

@SYMDEF             DEF081259

@SYMTestCaseDesc    Changing sizes of intersecting transparent windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) are created before the test on the screen's left side.
					Plays with sizes of each of the four windows to affect overlapping. 
					Draws visible windows on the screen's right side using FBS bitmaps direcly
					each time when a new size for a particular window is applied.
					The test does nothing if transparency is not enabled on the screen.


@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 5:
		case 24:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0115"));
			iTest->LogSubTest(KSizeChange);
			SizeChange();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0116

@SYMDEF             DEF081259

@SYMTestCaseDesc    Moving intersecting transparent windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) are created before the test on the screen's left side.
					Performs moves of those windows one by one making little steps in direction of the center of intersection and backwards.
					Draws visible windows on the screen's right side using FBS bitmaps direcly
					each time when a position of a particular window changes.
					The test does nothing if transparency is not enabled on the screen.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 6:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0116"));
			iTest->LogSubTest(KMoveWin);
			MoveWin();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0117

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test with a second window group

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow, within the same group) are created before the test on the screen's left side.
					Creates a 5th transparent window in a separate window group
					on top of existing windows which covers the intersection area of initial 4 windows. 
					Changes the ordinal position of the group to 1, changes ordinal position 
					back to 0, removes the new group.
					Redraws visible windows on the screen's right side using FBS bitmaps direcly
					each time when any change on the screen's left side is performed.
					The test does nothing if transparency is not enabled on the screen.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 7:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0117"));
			iTest->LogSubTest(KGroupWin);
			GroupWinL();
			break;
		case 8:
			((CTMultipleTransStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iTest->LogSubTest(KBackground);
			//Change the background color of the 4 test windows and the window behind them
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0118

@SYMDEF             DEF081259

@SYMTestCaseDesc    On/off switching of fading

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) are created before the test on the screen's left side.
					Switches system fading on and right after that back to off.
					Draws visible windows on the screen's right side using FBS bitmaps direcly
					after that.
					The test does nothing if transparency is not enabled on the screen.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 9:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0118"));
			iTest->LogSubTest(KTranWinFade);
			FadeTranWin();
			break;
		case 10:
			((CTMultipleTransStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iTest->iState=21-1;
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0120

@SYMDEF             DEF081259

@SYMTestCaseDesc    Shadows' on/off test.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) with shadowing enabled are created before the test on the screen's left side.
					The test plays with on/off-switching of shadowing and visibility for the windows.
					The test redraws visible windows on the screen's right side using FBS bitmaps direcly
					each time when state of ony window changes.
					The test does nothing if transparency is not enabled on the screen.
					The test also repeats GRAPHICS-WSERV-0111 - GRAPHICS-WSERV-015 foor the shadowed windows.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 22:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0120"));
			iTest->LogSubTest(KShadowsOnOff);
			ShadowsOnOff();
			break;
		case 25:
			((CTMultipleTransStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iTest->iState=41-1;
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0121

@SYMDEF             DEF081259

@SYMTestCaseDesc    Transparent child-windows of a bacground window 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) with shadowing enabled are created before the test on the screen's left side.
					The test removes those windows and creates new four overlapping transparent windows 
					but as children of existing background window. After that the test redraws visible windows on the screen's right side using FBS bitmaps direcly.
					The test does nothing if transparency is not enabled on the screen.
					The test also repeats GRAPHICS-WSERV-0111 - GRAPHICS-WSERV-014 foor the shadowed windows.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 41:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0121"));
			iTest->LogSubTest(KChild1);
			StartChildL();
			break;
		case 44:
			((CTMultipleTransStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iTest->iState=61-1;
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0122

@SYMDEF             DEF081259

@SYMTestCaseDesc    

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Four overlapping transparent windows (RWindow) with shadowing enabled are created before the test on the screen's left side.
					The test removes those windows and creates new four transparent windows 
					so that each one (except first) is a child of prevoiusly created window. 
					The test redraws visible windows on the screen's right side using FBS bitmaps direcly
					after each window's creation.
					The test does nothing if transparency is not enabled on the screen.

@SYMTestExpectedResults Expects that left and rightside bitmaps are identical
*/		
		case 61:
			((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0122"));
			iTest->LogSubTest(KChild2);
			StartChild2L();
			break;
		default:
				((CTMultipleTransStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0123"));
/**
@SYMTestCaseID		GRAPHICS-WSERV-0123

@SYMDEF             DEF081259

@SYMTestCaseDesc    

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test draws four overlapping transparent windows (RWindow) on the screen's left side.
					Each window has an opaque diagonal line (\) which goes from the top-left corner of each window.
					The test repeats all tests from GRAPHICS-WSERV-0111 to GRAPHICS-WSERV-0122 
					with those four windows and diagonaly lines on them.

@SYMTestExpectedResults Expects that all tests will obtain results they expected. 
*/		
			if (iMajorTest==1 || !iOpacitySupported)
				{
				if (!iOpacitySupported)
					{
					_LIT(KNoOpacity,"Opacity is not supported");
					LOG_MESSAGE(KNoOpacity);
					}
                		((CTMultipleTransStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
				TestComplete();
				return;
				}
			LOG_MESSAGE(KOpacity);
			iTest->iState=0;
			iMajorTest=1;
			RecreateWindowsL();
			break;
		}
	((CTMultipleTransStep*)iStep)->RecordTestResultL();
	++iTest->iState;
	}


__WS_CONSTRUCT_STEP__(MultipleTrans)
