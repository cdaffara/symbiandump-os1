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
// GETPIXEL.CPP
// Automatically test GetPixel
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TGETPIXEL.H"

CTGetPixel::CTGetPixel(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTGetPixel::~CTGetPixel()
	{
	iWindow.Close();
	User::Free(iRgbBuf);
	}

LOCAL_C TInt DoPanicTest(TInt aFunc, TAny *aScreenNumber)
	{
	RWsSession ws;
	if (ws.Connect()==KErrNone)
		switch(aFunc)
			{
			case 1:	// Get pixels into a TPtr 1 pixel too small
				{
				CWsScreenDevice *screen = new(ELeave) CWsScreenDevice(ws);
				User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
				CFbsBitmap *bitmap=new(ELeave) CFbsBitmap();
				TInt bitWid=80;
				TRgb *buf=NULL;
				if (bitmap->Create(TSize(1,bitWid), EGray16)==KErrNone && (buf=(TRgb *)User::Alloc((bitWid-1)*sizeof(TRgb)))!=NULL)
					{
					TPtr8 desc((TUint8 *)buf,(bitWid-1)*sizeof(TRgb));
    				screen->GetScanLine(desc,TPoint(0,0),bitWid, ERgb);
					}
				}
				break;
			}
	return(EWsExitReasonBad);
	}

void CTGetPixel::ClearOutRedraws()
// This has been added because partial redraw store can be triggered to request a
// low priority redraw from the window even though in normal circumstances the window
// would not expect to receive any sort of redraw event.
	{
	iWindow.BeginRedraw();
	iWindow.EndRedraw();
	}

void CTGetPixel::TestPanicsL()
	{
	if (!iTest->IsFullRomL())
		{
		TEST(iTest->TestWsPanicL(DoPanicTest,EWservPanicInvalidParameter,1,(TAny*)iTest->iScreenNumber));
		}
	}

void CTGetPixel::DrawAndCheckLine(const TPoint &aPos,TInt aLen,TRgb aColor)
	{
	TheGc->Activate(iWindow);
	TheGc->SetPenColor(aColor);
	iWindow.BeginRedraw(TRect(aPos,TSize(aLen, 1)));
	TheGc->DrawLine(aPos,aPos+TSize(aLen,0));
	iWindow.EndRedraw();
	TheGc->Deactivate();
	iRgbBuf=(TRgb *)User::ReAlloc(iRgbBuf,aLen*sizeof(TRgb));
	TPtr8 ptr((TUint8 *)iRgbBuf,aLen*sizeof(TRgb));
	TheClient->iScreen->GetScanLine(ptr, aPos+iWindow.InquireOffset(*TheClient->iGroup->WinTreeNode()), aLen, EColor16MA);
	TRgb result(TRgb::Gray16(aColor.Gray16()));
	if (TheClient->iScreen->DisplayMode()==EColor64K)
		result=TRgb::Color64K(result.Color64K());
	for(TInt index=0;index<aLen;index++)
		{
		TEST(iRgbBuf[index]==result);
		}
	}

void CTGetPixel::ConstructL()
	{
	iWindow = TheClient->iWs;
	TheGc->Activate(*BaseWin->Win());
	TheGc->Clear();
	TheGc->SetBrushColor(TRgb::Gray16(0));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(iWinSize));
	TheGc->Deactivate();
	
	iWindow.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle);
	iWinSize=TSize(TheClient->iScreen->SizeInPixels());
	iWinSize.iWidth/=3;
	iWinSize.iHeight/=3;
	iWindow.SetRequiredDisplayMode(EGray16);
	TheClient->iWs.Flush();
	iTest->DelayIfFullRomL();	// Need to wait for view server to mess around when display mode changed
	TheClient->WaitForRedrawsToFinish();// otherwise it will stomp on top of our window invalidating it.
	iWindow.SetExtent(TPoint(iWinSize.iWidth,iWinSize.iHeight),iWinSize);
	iWindow.EnableRedrawStore(EFalse);	// Otherwise drawing might trigger a redraw when purging redraw store
	
	iWindow.Activate();
	iWindow.BeginRedraw();
	iWindow.EndRedraw();
		
	}

void CTGetPixel::TestCheckRect()
	{
	TSize size(TheClient->iScreen->SizeInPixels());
	TEST(TheClient->iScreen->RectCompare(TRect(size),TRect(TPoint(iTest->StdTestWindowSize().iWidth>>1,0),iTest->StdTestWindowSize()))==EFalse);
	}

void CTGetPixel::DrawColorsL()
	{
	TPoint point(0,0);
	TInt color=-1;
	iWindow.BeginRedraw();
	FOREVER
		{
		TheGc->SetPenColor(TRgb::Color256(++color));
		TheGc->Plot(point);
		if (color==255)
			break;
		if (++point.iX==iWinSize.iWidth)
			{
			point.iX=0;
			if (++point.iY==iWinSize.iHeight)
				break;
			}
		}
	iWindow.EndRedraw();
	}

void CTGetPixel::TestColors(CPalette* aPalette)
	{
	TInt numColors=iWinSize.iWidth;
	TPtr8 ptr(REINTERPRET_CAST(TUint8*,iRgbBuf),numColors*sizeof(TRgb));
	TPtr8 paletteData(NULL,0);
	TInt color=0;
	TPoint point(iWinSize.AsPoint());
	do	{
		if (color+numColors>256)
			numColors=256-color;
		TheClient->iScreen->GetScanLine(ptr,point,numColors,EColor16MA);
		aPalette->GetDataPtr(color,numColors,paletteData);
		TEST(ptr==paletteData);
		color+=numColors;
		} while (color<256 && ++point.iY<2*iWinSize.iHeight);
	}

void CTGetPixel::CheckPaletteL(CPalette*& aPalette)
	{
	User::LeaveIfError(TheClient->iScreen->SetCustomPalette(aPalette));
	TestColors(aPalette);
	delete aPalette;
	User::LeaveIfError(TheClient->iScreen->GetPalette(aPalette));
	TestColors(aPalette);
	}

inline TInt Inc(TInt& aValue)
	{
	if (aValue>255-83)
		aValue-=256-83;
	else
		aValue+=83;
	return aValue;
	}

inline TInt Inc2(TInt& aValue)
	{
	if (aValue>255-41)
		aValue-=256-41;
	else
		aValue+=41;
	return aValue;
	}

void CTGetPixel::PaletteTestL()
	{
	//INFO_PRINTF1(_L("AUTO  PaletteTest "));
	TInt numEntries;
	TBool modifiable;
	TheClient->iScreen->PaletteAttributes(modifiable,numEntries);
	INFO_PRINTF2(_L("Number of entries in the palette %d"), numEntries);
	if(numEntries > 65536)
		{
		INFO_PRINTF1(_L("These test cases has been skipped, as the screen is set up in true colour display mode, where palette is not applicable"));
		return; 
		}
	CPalette* palette=NULL;
	if (!modifiable)
		{
		if (numEntries==4)
			palette=CPalette::NewDefaultL(EGray4);
		else if (numEntries==16)
			palette=CPalette::NewDefaultL(EGray16);
		else
			palette=CPalette::NewL(numEntries);
		//INFO_PRINTF1(_L(" Creating Empty Palette, setting it as palette"));
		TInt err=TheClient->iScreen->SetCustomPalette(palette);
		TEST(err==KErrNoMemory || err==KErrNotSupported || (err==KErrNone && numEntries<=16));		//Series5MX palettes are changeable even though they say they aren't
		//INFO_PRINTF1(_L(" Palette setting test passed OK"));
		delete palette;
		}
	TInt err=iWindow.SetRequiredDisplayMode(EColor256);
	TheGc->Activate(iWindow);
	DrawColorsL();
	//INFO_PRINTF1(_L(" Drawn Colors"));
	TheClient->iScreen->PaletteAttributes(modifiable,numEntries);
	iRgbBuf=STATIC_CAST(TRgb*,User::ReAllocL(iRgbBuf,Max(256,iWinSize.iWidth)*sizeof(TRgb)));
	if (err<KErrNone || modifiable==EFalse)
		{
		//INFO_PRINTF1(_L(" Palette Fixed"));
		TInt err=TheClient->iScreen->GetPalette(palette);
		if (err==KErrNotSupported)
			return;
		User::LeaveIfError(err);
		TestColors(palette);
		delete palette;
		//INFO_PRINTF1(_L(" Tested Palette OK"));
		return;
		}
	//INFO_PRINTF1(_L(" Palette Changeable"));
	TEST(numEntries==256);
	CPalette* defPalette=CPalette::NewDefaultL(EColor256);
	CleanupStack::PushL(defPalette);
	TestColors(defPalette);
	User::LeaveIfError(TheClient->iScreen->GetPalette(palette));
	TestColors(palette);
	TInt color;
	TInt index=0;
	for (color=0;color<256;++color)
		palette->SetEntry(color,TRgb::Gray256(color));
	CheckPaletteL(palette);
	for (color=0;color<256;++color)
		palette->SetEntry(color,TRgb(Inc(index),Inc(index),Inc(index)));
	CheckPaletteL(palette);
	for (color=0;color<256;++color)
		palette->SetEntry(color,TRgb(Inc2(index),Inc2(index),Inc2(index)));
	CheckPaletteL(palette);
	delete palette;
	User::LeaveIfError(TheClient->iScreen->SetCustomPalette(defPalette));
	TestColors(defPalette);
	CleanupStack::PopAndDestroy(defPalette);
	ClearOutRedraws();
	}

void CTGetPixel::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTGetPixelStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0212

@SYMDEF             DEF081259

@SYMTestCaseDesc    Draw lines and check them by scanning the lines

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw lines and then scan them to check that they have
					been drawn correctly

@SYMTestExpectedResults Scanning the lines reveals they have been drawn correctly
*/
		case 1:
			((CTGetPixelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0212"));
			TheClient->iWs.SetAutoFlush(ETrue);
			iTest->LogSubTest(_L("Basic"));
			DrawAndCheckLine(TPoint(0,2),iWinSize.iWidth-2,TRgb::Gray4(0));
			DrawAndCheckLine(TPoint(0,iWinSize.iHeight-2),iWinSize.iWidth,TRgb::Gray4(1));
			DrawAndCheckLine(TPoint(iWinSize.iWidth-1,iWinSize.iHeight-1),1,TRgb::Gray4(2));
			ClearOutRedraws();
			TheClient->iWs.SetAutoFlush(EFalse);
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0213

@SYMDEF             DEF081259

@SYMTestCaseDesc    Draw line on every line of a window and
					check them by scanning the lines

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw lines on every line of a window and
					check them by scanning the lines

@SYMTestExpectedResults Scanning the lines reveals they have been drawn correctly
*/

		case 2:
			{
			((CTGetPixelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0213"));
			iTest->LogSubTest(_L("Full window"));
			for(TInt ypos=0;ypos<iWinSize.iHeight;ypos++)
				DrawAndCheckLine(TPoint(0,ypos),iWinSize.iWidth,TRgb::Gray4(0));
			ClearOutRedraws();
			}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0214

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test the check rect method 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Use the check rect method to check the image in a window

@SYMTestExpectedResults The check rect method functions correctly
*/
		case 3:
			((CTGetPixelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0214"));
			iTest->LogSubTest(_L("Check Rect"));
			TestCheckRect();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0215

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test that the GetScanLine method panics correctly 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Cause the GetScanLine method to panic and chekc the response

@SYMTestExpectedResults The panic for the GetScanLine method is correct
*/
		case 4:
			((CTGetPixelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0215"));
			iTest->LogSubTest(_L("Panic"));
			TestPanicsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0216

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test that CPalette class functions correctly 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create a CPalette object and exercise all its methods

@SYMTestExpectedResults The palette functions as exepcted
*/
		case 5:
			((CTGetPixelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0216"));
			iTest->LogSubTest(_L("Palette Test"));
			PaletteTestL();
			break;
		case 6:
            		((CTGetPixelStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTGetPixelStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTGetPixelStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(GetPixel)
