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
// Test backed up windows
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TBACKUPW.H"

CTBackedUpWindow::CTBackedUpWindow(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}


void CTBackedUpWindow::CheckWindow(CFbsBitmap *aBitmap)
	{
	TPoint oldPos=iBackedUpWindow.Position();
	iBackedUpWindow.SetPosition(TPoint(0,0));

	iCheckWindow.SetVisible(ETrue);
	iCheckWindow.SetPosition(iCheckPos);

	iCheckWindow.BeginRedraw();
	TheGc->Activate(iCheckWindow);
	TheGc->BitBlt(TPoint(0,0), aBitmap);
	TheGc->Deactivate();
	iCheckWindow.EndRedraw();

	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	TBool retVal = TheClient->iScreen->RectCompare(TRect(iSize),TRect(iCheckPos,iSize));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare(TRect(iSize),TRect(iCheckPos,iSize)) return value - Expected: %d , Actual: %d"), ETrue, retVal);		

	iCheckWindow.SetVisible(EFalse);
	iBackedUpWindow.SetPosition(oldPos);
	}

void CTBackedUpWindow::CheckWindow()
	{
	CheckWindow(&iBitmap);
	}

void CTBackedUpWindow::Draw(TInt aDrawFunc, TAny *aParam)
	{
	TheGc->Activate(iBackedUpWindow);
	Draw(aDrawFunc, aParam, TheGc, iBitGc);
	TheGc->Deactivate();
	}

void CTBackedUpWindow::DrawWithTwoGcsL()
	{
	RBlankWindow blank(TheClient->iWs);
	User::LeaveIfError(blank.Construct(*(TheClient->iGroup->GroupWin()),ENullWsHandle));
	blank.Activate();
	CWindowGc *gc2;
	User::LeaveIfError(TheClient->iScreen->CreateContext(gc2));
	CleanupStack::PushL(gc2);

	CFbsFont *font1;
	TFontSpec fspec(KTestFontTypefaceName,200);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)font1,fspec));
	CFbsFont *font2;
	TFontSpec fspec2(KTestFontTypefaceName,400);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)font2,fspec2));

	gc2->Activate(iBackedUpWindow);
	gc2->UseFont(font2);
	TheGc->Activate(iBackedUpWindow);
	TheGc->UseFont(font1);
	_LIT(KText,"Testing123");
	TheGc->DrawText(KText,TPoint(20,20));
	gc2->DrawText(KText,TPoint(20,40));
	iBitGc->UseFont(font1);
	iBitGc->DrawText(KText,TPoint(20,20));
	iBitGc->UseFont(font2);
	iBitGc->DrawText(KText,TPoint(20,40));
	iBitGc->DiscardFont();
	TheGc->Deactivate();
	TheClient->iScreen->ReleaseFont(font2);
	TheClient->iScreen->ReleaseFont(font1);
	CleanupStack::PopAndDestroy();
	blank.Close();
	}

void CTBackedUpWindow::Draw(TInt aDrawFunc, TAny *aParam, CBitmapContext *gc1, CBitmapContext *gc2)			//DDD
	{
	CBitmapContext *gc;
	for(TInt mode=0;mode<2;mode++)
		{
		if (mode==0)
			gc=gc1;
		else
			gc=gc2;
		gc->Reset();
		switch(aDrawFunc)
			{
			case 0:
				gc->SetBrushColor(*((TRgb *)aParam));
				gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
				gc->SetPenStyle(CGraphicsContext::ENullPen);
				gc->DrawRect(TRect(iSize));
				break;
			case 1:
				{
				TSize half(iSize.iWidth/2,iSize.iHeight/2);
				gc->DrawEllipse(TRect(half));
				gc->DrawEllipse(TRect(TPoint(0,half.iHeight),half));
				TRect rect1(half);
				gc->SetOrigin(TPoint(half.iWidth,0));
				gc->SetClippingRect(rect1);
				gc->DrawEllipse(rect1);
				gc->SetOrigin(TPoint(half.iWidth,half.iHeight));
				gc->SetClippingRect(rect1);
				gc->DrawEllipse(rect1);
				gc->SetOrigin(TPoint(0,0));
				gc->CancelClippingRect();
				}
				break;
			case 2:
				{
				TInt param= *((TInt *)aParam);
				if (param&0x1)
					gc->DrawLine(TPoint(param+(param*27)%iSize.iWidth,0),
								 TPoint(iSize.iWidth-((param<<1)+(param*19)%iSize.iWidth),iSize.iHeight));
				else
					gc->DrawLine(TPoint(0, (param<<1)+(param*7)%iSize.iHeight),
								 TPoint(iSize.iWidth,param+(param*13)%iSize.iHeight));
				}
				break;
			case 3:
				{
				TPoint pos;
				for(;pos.iX<iSize.iWidth;pos.iX+=10)
					gc->DrawLine(pos,pos+TSize(0,iSize.iHeight));
				for(pos.iX=0;pos.iY<iSize.iHeight;pos.iY+=10)
					gc->DrawLine(pos,pos+TSize(iSize.iWidth,0));
				}
				break;
			}
		}
	}

CTBackedUpWindow::~CTBackedUpWindow()
	{
	iBackedUpWindow.Close();
	iCheckWindow.Close();
	iOomFrontWindow.Close();
	delete iBitGc;
	delete iBitmapDevice;
	delete iTestBitmap;
	delete iMaskBitmap;
	}

void CopyToGray4L(CFbsBitmap*& aDestBitmap,const CFbsBitmap& aSrcBitmap)
	{
	aDestBitmap=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(aDestBitmap);
	User::LeaveIfError(aDestBitmap->Create(aSrcBitmap.SizeInPixels(),EGray4));
	CFbsBitmapDevice* device=CFbsBitmapDevice::NewL(aDestBitmap);
	CleanupStack::PushL(device);
	CFbsBitGc* gc;
	User::LeaveIfError(device->CreateContext(gc));
	gc->BitBlt(TPoint(),&aSrcBitmap);
	delete gc;
	CleanupStack::PopAndDestroy(device);
	CleanupStack::Pop(aDestBitmap);
	}

void CTBackedUpWindow::ConstructL()
	{
	iBackedUpWindow=RBackedUpWindow(TheClient->iWs);
	iBackedUpWindow.Construct(*(TheClient->iGroup->GroupWin()),EGray4,ENullWsHandle);
	TSize size=TheClient->iScreen->SizeInTwips();
	iSize=TheClient->iScreen->SizeInPixels();
	iSize.iWidth>>=1;
	iSize.iHeight>>=1;
	size.iWidth>>=1;
	size.iHeight>>=1;
	size.iWidth-=1;			//Modification to get the mapping factor the same as the screen
	size.iHeight-=1;		//Ditto
	iWinPos.SetXY(iSize.iWidth>>1,iSize.iHeight>>1);
	User::LeaveIfError(iBackedUpWindow.SetExtentErr(iWinPos,iSize));
	iBackedUpWindow.Activate();

	iCheckPos.SetXY(iSize.iWidth,0);
	iCheckWindow=RWindow(TheClient->iWs);
	iCheckWindow.Construct(*(TheClient->iGroup->GroupWin()),ENullWsHandle);
	iCheckWindow.SetSize(iSize);
	iCheckWindow.SetVisible(EFalse);
	iCheckWindow.Activate();

	TInt col,grey;
	TDisplayMode defMode=TheClient->iWs.GetDefModeMaxNumColors(col,grey);
	User::LeaveIfError(iBitmap.Create(iSize,defMode));
	iBitmap.SetSizeInTwips(size);
	iBitmapDevice=CFbsBitmapDevice::NewL(&iBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext(iBitGc));

	TRgb rgb(TRgb::Gray4(2));
	Draw(0,&rgb);

	iTestBitmap=new(ELeave) CFbsBitmap();
  	User::LeaveIfError(iTestBitmap->Load(TEST_BITMAP_NAME,0));
	iTestBitmap->SetSizeInTwips(TSize(1500,750));
	iMaskBitmap=new(ELeave) CFbsBitmap();
  	User::LeaveIfError(iMaskBitmap->Load(TEST_BITMAP_NAME,0));
  	TDisplayMode defMode2=iMaskBitmap->DisplayMode();
  	if (defMode>EGray4)
		{
		CFbsBitmap* bitmap=iTestBitmap;
		CopyToGray4L(iTestBitmap,*bitmap);
		delete bitmap;
		bitmap=iMaskBitmap;
		CopyToGray4L(iMaskBitmap,*bitmap);
		delete bitmap;
		}
	defMode2=iMaskBitmap->DisplayMode();
	defMode2=iMaskBitmap->DisplayMode();	
	}

void CTBackedUpWindow::InvisVis()
	{
	iBackedUpWindow.SetVisible(EFalse);
	TheClient->iWs.Flush();
	iBackedUpWindow.SetVisible(ETrue);
	TheClient->iWs.Flush();
	}

void CTBackedUpWindow::WindowOnTop()
	{
	RBlankWindow blank(TheClient->iWs);
	blank.Construct(*(TheClient->iGroup->GroupWin()),ENullWsHandle);
	blank.Activate();
	blank.Close();
	TheClient->iWs.Flush();

	TPoint pos;
	TInt sizeMode=0;
	TInt draw=13;

	for(pos.iX=-iSize.iWidth;pos.iX<iSize.iWidth;pos.iX+=50)
		for(pos.iY=-iSize.iHeight;pos.iY<iSize.iHeight;pos.iY+=50)
			{
			blank=RBlankWindow(TheClient->iWs);
			blank.Construct(*(TheClient->iGroup->GroupWin()),ENullWsHandle);
			blank.SetColor(TRgb::Gray256(220));
			blank.SetShadowHeight(2);
			blank.SetExtent(pos+iWinPos,TSize((sizeMode&0x1)?iSize.iWidth>>1:iSize.iWidth<<1,(sizeMode&0x2)?iSize.iHeight>>1:iSize.iHeight<<1));
			sizeMode=(sizeMode+1)%4;
			blank.Activate();
			Draw(2,&draw);
			TheClient->iWs.Flush();
			draw++;
			blank.Close();
			}
	}

void CTBackedUpWindow::Resize()
	{
	RBlankWindow blank(TheClient->iWs);
	blank.Construct(*(TheClient->iGroup->GroupWin()),ENullWsHandle);
	blank.SetColor(TRgb::Gray256(128));
	blank.Activate();
	TInt xtop=(iSize.iWidth)-(iSize.iWidth>>2);
	TInt ytop=(iSize.iHeight)-(iSize.iHeight>>2);
	for(TInt winMode=0;winMode<3;winMode++)
		{
		switch(winMode)
			{
			case 0:
				blank.SetExtent(TPoint(0,ytop),TSize(iSize.iWidth,iSize.iHeight>>1));
				break;
			case 1:
				blank.SetExtent(TPoint(xtop,0),TSize(iSize.iWidth>>1,iSize.iHeight));
				break;
			case 2:
				blank.SetExtent(TPoint(xtop,ytop),TSize(iSize.iWidth>>1,iSize.iHeight>>1));
				break;
			}
		blank.SetShadowHeight(winMode);
		TPoint oldPos=iBackedUpWindow.Position();
		TPoint pos=oldPos;
		for(TUint i=0;i<sizeof(moveList)/sizeof(moveList[0]);i++)
			{
			pos+=moveList[i];
			iBackedUpWindow.SetPosition(pos);
			TheClient->iWs.Flush();
			}
		iBackedUpWindow.SetPosition(oldPos);
		}
	blank.Close();
	}

void CTBackedUpWindow::ChildWindows()
	{
	TPoint pos;
	TInt sizeMode=0;
	TInt draw=13;
	RBlankWindow blank(TheClient->iWs);
	for(pos.iX=-(iSize.iWidth>>1);pos.iX<iSize.iWidth;pos.iX+=33)
		for(pos.iY=-(iSize.iHeight>>1);pos.iY<iSize.iHeight;pos.iY+=33)
			{
			blank.Construct(iBackedUpWindow,ENullWsHandle);
			blank.SetColor(TRgb::Gray256(220));
			blank.SetShadowHeight(2);
			blank.SetExtent(pos,TSize((sizeMode&0x1)?iSize.iWidth>>2:iSize.iWidth,(sizeMode&0x2)?iSize.iHeight>>2:iSize.iHeight));
			sizeMode=(sizeMode+1)%4;
			blank.Activate();
			Draw(2,&draw);
			TheClient->iWs.Flush();
			draw++;
			blank.Close();
			}
	}

void CTBackedUpWindow::DupBitmapTestL()
	{
	INFO_PRINTF1(_L("AUTO  Dup Bitmap Test "));
	CFbsBitmap *dup=new(ELeave) CFbsBitmap();
	dup->Duplicate(iBackedUpWindow.BitmapHandle());
	CheckWindow(dup);
	TRgb rgb(TRgb::Gray4(1));
	Draw(0,&rgb);
	CheckWindow(dup);
	INFO_PRINTF1(_L(" Done Window Drawing Test"));

	CFbsBitmapDevice *dupDevice=NULL;
	TRAPD(err,dupDevice=CFbsBitmapDevice::NewL(dup));
	CFbsBitGc *gc;
	if (err==KErrNone && dupDevice->CreateContext(gc)==KErrNone)
		{
		Draw(3,NULL,gc,iBitGc);		// Draw directly to backup bitmap (and test bitmap)
		iBackedUpWindow.UpdateScreen();
		INFO_PRINTF1(_L(" First Bitmap Drawing"));
		CheckWindow();
		TRgb rgb(TRgb::Gray256(128));
		TInt col,grey;
		if (TheClient->iWs.GetDefModeMaxNumColors(col,grey)>EGray4)
			rgb=TRgb::Gray4(2);
		Draw(0,&rgb,gc,iBitGc);
		Draw(1,NULL,gc,iBitGc);
		iBackedUpWindow.UpdateScreen(TRegionFix<1>(TRect(iSize)));
		INFO_PRINTF1(_L(" Second Bitmap Drawing"));
		CheckWindow();
		delete gc;
		}
	delete dupDevice;
	delete dup;
	}

void CTBackedUpWindow::UpdateBitmapTestL()
	{
	INFO_PRINTF1(_L("AUTO  UpdateBitmap "));
	CheckWindow();
	CFbsBitmap *dup=new(ELeave) CFbsBitmap();
	dup->Duplicate(iBackedUpWindow.BitmapHandle());
	Draw(3,NULL);
	iBackedUpWindow.UpdateBackupBitmap();
	INFO_PRINTF1(_L(" First Drawing"));
	CheckWindow(dup);
	TRgb rgb;
	if (iSupState==0)
		rgb=TRgb::Gray256(128);
	else
		rgb=TRgb::Gray4(2);
	Draw(0,&rgb);
	Draw(1,NULL);
	iBackedUpWindow.UpdateBackupBitmap();
	INFO_PRINTF1(_L(" Second Drawing"));
	CheckWindow(dup);
	delete dup;
	}

void CTBackedUpWindow::OOML()
	{
	iOomFrontWindow=RBlankWindow(TheClient->iWs);
	TSize size(iBackedUpWindow.Size());
	size.iWidth>>=1;
	size.iHeight>>=1;
	TPoint pos(size.iWidth*3/2,size.iHeight*3/2);
	iOomFrontWindow.Construct(*(TheClient->iGroup->GroupWin()),ENullWsHandle);
	iOomFrontWindow.SetColor(TRgb(TRgb::Gray4(1)));
	iOomFrontWindow.SetExtent(pos,size);
	iOomFrontWindow.Activate();
	TheClient->iWs.Flush();

	TPoint buwPos=iBackedUpWindow.Position();
	TSize buwSize=iBackedUpWindow.Size();
	for(TInt count=0;count<100;count++)
		{
		iOomFrontWindow.SetVisible(ETrue);
		TheClient->iWs.HeapSetFail(RHeap::EDeterministic,count);
		iBackedUpWindow.SetPosition(buwPos+TPoint(10,5));
		iBackedUpWindow.SetPosition(buwPos);
		iOomFrontWindow.SetSize(size+TSize(10,5));
		iOomFrontWindow.SetSize(size);
		iBackedUpWindow.SetSizeErr(buwSize+TSize(13,7));
		iBackedUpWindow.SetSizeErr(buwSize);
		iOomFrontWindow.SetVisible(EFalse);
		TheClient->iWs.HeapSetFail(RHeap::ENone,0);
		User::LeaveIfError(iBackedUpWindow.SetSizeErr(buwSize));
		TheClient->WaitForRedrawsToFinish();
		CheckWindow();
		}

	iOomFrontWindow.Close();
	}

void CTBackedUpWindow::doGraphicFunctionsL(CBitmapContext *gc,TBool aExtraDrawBitMap)
	{
	TSize size=iBackedUpWindow.Size();
	CFbsFont *aFont;
	_LIT(KFontName,"Swiss");
	TFontSpec fspec(KFontName,190);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)aFont,fspec));
	CFbsBitmap* aBitmap=iTestBitmap;
	CFbsBitmap* aMaskBitmap=iMaskBitmap;

	#include "DLLDRAW.H"	// Draws to a Gc called 'gc'
	
	TheClient->iScreen->ReleaseFont(aFont);
	}

void CTBackedUpWindow::AllGraphicFunctionsL(RBlankWindow &aBlank,TBool aExtraDrawBitMap/*=ETrue*/)
	{
	aExtraDrawBitMap=EFalse;		//Check out when bitblit scaling has changed again
	aBlank.SetVisible(ETrue);
	TheGc->Activate(iBackedUpWindow);
	TRAPD(err,doGraphicFunctionsL(TheGc,aExtraDrawBitMap));
	TheGc->Deactivate();
	User::LeaveIfError(err);
	iBitGc->Reset();
	iBitGc->SetUserDisplayMode(EGray4);
	doGraphicFunctionsL(iBitGc,aExtraDrawBitMap);
	aBlank.SetVisible(EFalse);
	CheckWindow();
	}

void CTBackedUpWindow::AllGraphicFunctionsTestsL()
	{
	RBlankWindow blank(TheClient->iWs);
	TInt xtop=(iSize.iWidth)-(iSize.iWidth>>1);
	TInt ytop=(iSize.iHeight)-(iSize.iHeight>>1);
	blank.Construct(*(TheClient->iGroup->GroupWin()),ENullWsHandle);
	blank.SetColor(TRgb::Gray256(128));
	blank.Activate();
	blank.SetExtent(TPoint(0,ytop),TSize(iSize.iWidth,iSize.iHeight>>2));
	AllGraphicFunctionsL(blank);
	blank.SetExtent(TPoint(0,ytop+10),TSize(iSize.iWidth,iSize.iHeight>>2));
	AllGraphicFunctionsL(blank);
	blank.SetExtent(TPoint(xtop,ytop),TSize(iSize.iWidth>>1,iSize.iHeight));
	AllGraphicFunctionsL(blank,EFalse);
	blank.SetExtent(TPoint(xtop+(iSize.iWidth>>1),ytop),TSize(iSize.iWidth>>1,iSize.iHeight));
	AllGraphicFunctionsL(blank,EFalse);
	blank.SetExtent(TPoint(xtop+10,iSize.iHeight),TSize(iSize.iWidth,iSize.iHeight));
	AllGraphicFunctionsL(blank,EFalse);
	blank.SetExtent(TPoint(xtop,ytop),iSize);
	AllGraphicFunctionsL(blank,EFalse);
	blank.SetExtent(TPoint(0,0),TSize(0,0));
	AllGraphicFunctionsL(blank);
	blank.Close();
	}

void CTBackedUpWindow::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	switch(iSupState)
		{
		case 0:
			for (TInt iSubTest=0;iSubTest<KLastSubtest;iSubTest++)
				{
				DoSubTestL(iSubTest);
				}
			iBackedUpWindow.MaintainBackup();		//Putting this line here is a work around
			break;
		case 1:
			for (TInt iSubTest=0;iSubTest<KLastSubtest;iSubTest++)
				{
				DoSubTestL(iSubTest);
				}
			break;
		default:
			TestComplete();
			break;
		}
	iSupState++;
	}

void CTBackedUpWindow::DoSubTestL(TInt iState)
	{
	_LIT(KTest0,"Simple draw");
	_LIT(KTest1,"Resizing");
	_LIT(KTest2,"Invisible/Visible");
	_LIT(KTest3,"Windows on top");
	_LIT(KTest4,"Child Windows");
	_LIT(KTest5,"OOM");
	_LIT(KTest6,"Update Bitmap");
	_LIT(KTest7,"Bitmap duplicate");
	_LIT(KTest8,"Two Graphic Contexts");
	_LIT(KTest9,"All Graphic Functions");
	_LIT(KTest10,"Reactivate");
	_LIT(KTest11,"DoDrawCommand");

	TRgb rgb1(255,255,255);
	TRgb rgb2(255,255,255);
	TRgb rgb3(255,255,255);
	TRgb rgb4(255,255,255);
	TRgb rgb5(255,255,255);
	TRgb rgb6(255,255,255);
	TRgb rgb7(255,255,255);
	TRgb color(TRgb::Gray4(2));

	iTest->iState=iState;
((CTBackedUpWindowStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0202-0001

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing in a backed up window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw in a backed up window and a normal window and check
					the two drawings are the same

@SYMTestExpectedResults The two drawings are exactly the same
*/
		case 0:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0202-0001"));
			iTest->LogSubTest(KTest0);
			Draw(1,NULL);
			CheckWindow();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0202-0002

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing in a backed up window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw in a backed up window and a normal window and check
					the two drawings are the same

@SYMTestExpectedResults The two drawings are exactly the same
*/
		case 1:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0202-0002"));
			iTest->LogSubTest(KTest1);
			Draw(0,&rgb1);
			Draw(3,NULL);
			Resize();
			CheckWindow();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0203

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test making a backed up window invisible

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw in a backed up window and a normal window, make
					the backed up window invisible then visible and
					then check the two drawings are the same

@SYMTestExpectedResults The two drawings are exactly the same
*/
		case 2:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0203"));
			iTest->LogSubTest(KTest2);
			Draw(0,&rgb2);
			Draw(1,NULL);
			InvisVis();
			CheckWindow();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0204

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing in a backed up window and then placing a
					window on top of it

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw in a backed up window and a normal window, then
					place a window on top of the backed up window and then
					check the two drawings are the same

@SYMTestExpectedResults The two drawings are exactly the same
*/
		case 3:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0204"));
			iTest->LogSubTest(KTest3);
			//TRgb rgb(220,220,220);
			Draw(0,&rgb3);
			Draw(1,NULL);
			WindowOnTop();
			CheckWindow();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0205

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing in a backed up window and then drawing in
					a child window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw in a backed up window and a normal window, then
					create and draw in a child window and then
					check the two original drawings are the same

@SYMTestExpectedResults The two drawings are exactly the same
*/
		case 4:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0205"));
			iTest->LogSubTest(KTest4);
			Draw(0,&rgb4);
			Draw(3,NULL);
			ChildWindows();
			CheckWindow();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0206

@SYMDEF             DEF081259

@SYMTestCaseDesc    Out of memeory test for backed up windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Out of memeory test for backed up windows

@SYMTestExpectedResults Backed up window responds correctly when out
						of memory
*/
		case 5:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0206"));
			iTest->LogSubTest(KTest5);
			if (!iTest->IsFullRomL())
				{
				Draw(0,&rgb5);
				Draw(3,NULL);
				OOML();
				CheckWindow();
				}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0207

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test updating a bitmap in a backed up window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Update a bitmap in a backed up window and a normal window
					and check the two bitmaps are the same

@SYMTestExpectedResults The two bitmaps are exactly the same
*/
		case 6:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0207"));
			iTest->LogSubTest(KTest6);
			Draw(0,&rgb6);
			Draw(1,NULL);
			UpdateBitmapTestL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0208

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test updating a duplicate bitmap in a backed up window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Update a duplicated bitmap in a backed up window and a normal window
					and check the two bitmaps are the same

@SYMTestExpectedResults The two bitmaps are exactly the same
*/
		case 7:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0208"));
			if (iSupState==0)		//Will fail unless the window is fully backup.
				break;
			iTest->LogSubTest(KTest7);
			Draw(0,&rgb7);
			Draw(1,NULL);
			DupBitmapTestL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0209

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing with two graphic contexts in a backed up window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw using two graphic contexts in a backed up window and a normal
					window and check the two drawings are the same

@SYMTestExpectedResults The two drawings are exactly the same
*/
		case 8:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0209"));
			iTest->LogSubTest(KTest8);
			Draw(0,&color);
			Draw(1,NULL);
			DrawWithTwoGcsL();
			CheckWindow();
			break;
			
/**
@SYMTestCaseID		GRAPHICS-WSERV-0210

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing using all the graphic functions in a backed up window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw using all the graphic functions in a backed up window and a normal
					window and check the two drawings are the same

@SYMTestExpectedResults The two drawings are exactly the same
*/
		case 9:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0210"));
			iTest->LogSubTest(KTest9);
			if(TheClient->iScreen->SizeInPixels() == TSize(640,240))
				AllGraphicFunctionsTestsL();
			break;

		//A Coverage test, nothing spectacular just making the code
		//go into CWsGc::Reactivate
		case 10:
			{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0502
*/
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0502"));
			iTest->LogSubTest(KTest10);
			TheGc->Deactivate();
			RBackedUpWindow extentWindow;
			extentWindow=RBackedUpWindow(TheClient->iWs);
			extentWindow.Construct(*(TheClient->iGroup->GroupWin()),EGray4,ENullWsHandle);
			TheGc->Activate(extentWindow);
			TheClient->Flush();
			User::LeaveIfError(extentWindow.SetExtentErr(TPoint(2,2), TSize(4,4)));
			extentWindow.Activate();
			extentWindow.Close();
			TheGc->Deactivate();
			break;
			}

		//Coverage for various messages for CWsGc::DoDrawCommand
		case 11:
			{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0507
*/
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0507"));
			iTest->LogSubTest(KTest11);
			TheGc->Activate(iBackedUpWindow);
			TheGc->Reset();
			iBitGc->Reset();
			iBitGc->SetUserDisplayMode(EGray4);
			
			// EWsGcOpDrawWsGraphic
			TheGc->DrawWsGraphic(TWsGraphicId(0), TRect(0,0,10,10)); 
			
			//create a large junk buffer so that messages with Ptr suffix will be sent
			RBuf8 junkBuf8;
			RBuf  junkBuf;
			junkBuf8.CreateMax(650); // a large enough buffer to sent as Ptr (this value used to crash the code before)
			junkBuf.CreateMax(650); // a large enough buffer to sent as Ptr
			for (int i=0; i<junkBuf.MaxLength()-1;i++)
				{
				junkBuf8[i] = 'A';
				junkBuf[i] = 'A';
				}
			junkBuf8[junkBuf8.MaxLength()-1] = '\0';
			junkBuf[junkBuf.MaxLength()-1] = '\0';
			
			// EWsGcOpDrawWsGraphicPtr
			TheGc->DrawWsGraphic(TWsGraphicId(0), TRect(0,0,10,10), junkBuf8);
			
			// Set font for drawing on screen
			CFbsFont *font1;
			TFontSpec fspec(KTestFontTypefaceName,200);
			User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)font1,fspec));
			TheGc->UseFont(font1);
			iBitGc->UseFont(font1);
						
			// EWsGcOpDrawTextPtr
			TheGc->DrawText(junkBuf, TPoint(0,0));
			iBitGc->DrawText(junkBuf, TPoint(0,0));
			
			// EWsGcOpDrawBoxText - unreachable 299-too lo, 300-too hi

			// EWsGcOpDrawBoxTextPtr
			TheGc->DrawText(junkBuf, TRect(0,0,10,10), 0, CGraphicsContext::ELeft, 0); 
			iBitGc->DrawText(junkBuf, TRect(0,0,10,10), 0, CGraphicsContext::ELeft, 0); 
			
			// EWsGcOpDrawTextVerticalPtr
			TheGc->DrawTextVertical(junkBuf, TPoint(0,0), ETrue);
			iBitGc->DrawTextVertical(junkBuf, TPoint(0,0), ETrue);
			
			// EWsGcOpDrawBoxTextVerticalPtr
			TheGc->DrawTextVertical(junkBuf, TRect(0,0,10,10), 0, ETrue, CGraphicsContext::ELeft, 0);
			iBitGc->DrawTextVertical(junkBuf, TRect(0,0,10,10), 0, ETrue, CGraphicsContext::ELeft, 0);
			
			// EWsGcOpMoveBy
			TheGc->MoveBy(TPoint(2,2));
			iBitGc->MoveBy(TPoint(2,2));
			
			// a bitmap for bitblt ops
			CWsBitmap bmp(TheClient->iWs);
			bmp.Create(TSize(16,16), iBackedUpWindow.DisplayMode());
			
			// EWsGcOpGdiWsBlt2
			TheGc->BitBlt(TPoint(0,0), &bmp);
			iBitGc->BitBlt(TPoint(0,0), &bmp);
			
			// EWsGcOpGdiWsBlt3
			TheGc->BitBlt(TPoint(0,0), &bmp, TRect(0,0,10,10));
			iBitGc->BitBlt(TPoint(0,0), &bmp, TRect(0,0,10,10));
			
			// EWsGcOpGdiWsBltMasked
			TheGc->BitBltMasked(TPoint(0,0), &bmp, TRect(0,0,10,10), &bmp, EFalse);
			iBitGc->BitBltMasked(TPoint(0,0), &bmp, TRect(0,0,10,10), &bmp, EFalse);
			
			// EWsGcOpGdiWsAlphaBlendBitmaps
			TheGc->AlphaBlendBitmaps(TPoint(0,0), &bmp, TRect(0,0,10,10), &bmp, TPoint(1,1));
			iBitGc->AlphaBlendBitmaps(TPoint(0,0), &bmp, TRect(0,0,10,10), &bmp, TPoint(1,1));
			
			// EWsGcOpWsDrawBitmapMasked
			TheGc->DrawBitmapMasked(TRect(0,0,10,10), &bmp, TRect(0,0,8,8), &bmp, ETrue);
			iBitGc->DrawBitmapMasked(TRect(0,0,10,10), &bmp, TRect(0,0,8,8), &bmp, ETrue);
			
			// EWsGcOpDrawBitmap 
			TheGc->DrawBitmap(TPoint(0,0), &bmp);
			iBitGc->DrawBitmap(TPoint(0,0), &bmp);
			
			// EWsGcOpDrawBitmapMasked 
			TheGc->DrawBitmapMasked(TRect(0,0,10,10), static_cast<const CFbsBitmap*>(&bmp), 
					                TRect(0,0,8,8), static_cast<const CFbsBitmap*>(&bmp), 
					                EFalse);
			iBitGc->DrawBitmapMasked(TRect(0,0,10,10), static_cast<const CFbsBitmap*>(&bmp), 
	                				TRect(0,0,8,8), static_cast<const CFbsBitmap*>(&bmp), 
	                				EFalse);
			
			// EWsGcOpDrawPolyLineContinued
			TheGc->DrawPolyLine(reinterpret_cast<const TPoint*>(junkBuf8.Ptr()),
    				            TInt(junkBuf8.Size()/sizeof(TPoint)));
			iBitGc->DrawPolyLine(reinterpret_cast<const TPoint*>(junkBuf8.Ptr()),
		            			TInt(junkBuf8.Size()/sizeof(TPoint)));
			
			// EWsGcOpCopyRect
			TheGc->CopyRect(TPoint(0,0), TRect(0,0,10,10));
			iBitGc->CopyRect(TPoint(0,0), TRect(0,0,10,10));
			
			//cleanup
			TheClient->Flush();
			TheGc->DiscardFont();
			iBitGc->DiscardFont();
			TheClient->iScreen->ReleaseFont(font1);
			junkBuf.Close();
			junkBuf8.Close();
			TheGc->Deactivate();
		
			CheckWindow();
			
			//the following have no client equivalent methods
			// EWsGcOpMapColorsLocal
			// EWsGcOpDrawPolyLineLocalBufLen
			// EWsGcOpDrawPolyLineLocal
			// EWsGcOpDrawPolygonLocalBufLen
			// EWsGcOpDrawPolygonLocal
			// EWsGcOpDrawBitmapLocal
			// EWsGcOpDrawBitmap2Local
			// EWsGcOpDrawBitmap3Local
			// EWsGcOpDrawBitmapMaskedLocal
			// EWsGcOpDrawTextPtr1
			// EWsGcOpDrawBoxTextPtr1
			// EWsGcOpDrawTextVerticalPtr1
			// EWsGcOpDrawBoxTextVerticalPtr1
			// EWsGcOpDrawTextLocal
			// EWsGcOpDrawBoxTextLocal
			// EWsGcOpGdiBlt2Local
			// EWsGcOpGdiBlt3Local
			// EWsGcOpGdiBltMaskedLocal
			((CTBackedUpWindowStep*)iStep)->CloseTMSGraphicsStep();
			break;
			}

		default:
			((CTBackedUpWindowStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTBackedUpWindowStep*)iStep)->CloseTMSGraphicsStep();
			break;
		}
       ((CTBackedUpWindowStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(BackedUpWindow)
