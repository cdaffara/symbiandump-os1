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
// An alpha channel test case. 
// Output different graphics primitives in each window using three graphics modes
// for pen and brush : semi-transparent,transparent, opaque
// Test also exercises anti-aliasing and fading for 16MA display mode
// Moves sprite on the window. Tests a sprite functionality.
// Creates RBackedUpWindow window and moves it over transparent window,
// hides and shows it.
// Moves windows on the screen, overlaps them
// Actions:
// Create a background window, and five foreground windows:
// -transparent and semi-transparent 
// -transparent with transparency factor
// -not transparent
// 
//

#include "TAlphaChannel.H"

const TInt KWindowIndention = 2;
const TInt KSizeKoeff = 15;


LOCAL_D TPtrC ColumnName[]={
	_L("Opaque"),
	_L("Semi-transparent"),
	_L("Transparent"),
	_L(""),
	_L(""),
	_L(""),
	};

LOCAL_D TPtrC RowName[]={
	_L("n"),
	_L("and"),
	_L("or"),
	_L("xor"),
	_L(""),
	_L(""),
	};


//
//
CTAlphaChannel::CTAlphaChannel(CTestStep* aStep):
	CTWsGraphicsBase(aStep), iArrWindow(8)
	{
	}

CTAlphaChannel::~CTAlphaChannel()
	{
	delete iBitmap64K_1;
	delete iBitmap16MA_1;
	delete iBitmap64K_2;
	delete iBitmap16MA_2;
	delete iBitmap64K_3;
	delete iBitmap16MA_3;

	delete iBitmapMask;
	delete iBitmapGray256Mask;
	
	delete iBitmapContext64K_1;
	delete iBitmapContext16MA_1;
	delete iBitmapContext64K_2;
	delete iBitmapContext16MA_2;
	delete iBitmapContext64K_3;
	delete iBitmapContext16MA_3;

	delete iBitmapDevice64K_1;
	delete iBitmapDevice16MA_1;
	delete iBitmapDevice64K_2;
	delete iBitmapDevice16MA_2;
	delete iBitmapDevice64K_3;
	delete iBitmapDevice16MA_3;

	delete iBackgroundWindow;
	
	
	iArrWindow.ResetAndDestroy();
	
	if(iFont)
		TheClient->iScreen->ReleaseFont(iFont);
	if(iFont1)
		TheClient->iScreen->ReleaseFont(iFont1);
	
	BaseWin->SetVisible(ETrue);
	TestWin->SetVisible(ETrue);
	}

void CTAlphaChannel::ConstructL()
	{
	const TInt KIndent = 10;
	BaseWin->SetVisible(EFalse);
	TestWin->SetVisible(EFalse);

	iText = _L("This is a text");
	iBrushStyle = CGraphicsContext::ESolidBrush;
	iPenTable = KRgbBlack;

	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth)-KIndent;
	TInt winHeight=screenSize.iHeight-KIndent;
	TDisplayMode dispMode = EColor64K;
	
	//background opaque window
	iBackgroundWindow = new (ELeave) CTWinAlpha();
	iBackgroundWindow->SetUpL(TPoint(5,5),TSize(winWidth,winHeight),TheClient->iGroup,*TheClient->iGc, &dispMode);
	RWindow *theWin = (RWindow*) (iBackgroundWindow->DrawableWin());
	theWin->SetBackgroundColor(TRgb(255, 0, 0));
	iBackgroundWindow->DrawNow();

	iSizeForegroundWindow.iWidth = (winWidth - KWindowIndention * 4) / 5 ;
	iSizeForegroundWindow.iHeight = winHeight;
	
	CalculateSizePrimitives(iSizeForegroundWindow);

	//-------------create bitmaps
	//create 64K bitmap
	iBitmap64K_1 = new (ELeave) CFbsBitmap ;
	iBitmap64K_1->Create(iBitmapSize,EColor64K);
	iBitmap64K_2 = new (ELeave) CFbsBitmap ;
	iBitmap64K_2->Create(iBitmapSize,EColor64K);
	iBitmap64K_3 = new (ELeave) CFbsBitmap ;
	iBitmap64K_3->Create(iBitmapSize,EColor64K);
	iBitmapDevice64K_1 = CFbsBitmapDevice::NewL(iBitmap64K_1);
	iBitmapDevice64K_1->CreateContext(iBitmapContext64K_1);
	iBitmapDevice64K_2 = CFbsBitmapDevice::NewL(iBitmap64K_2);
	iBitmapDevice64K_2->CreateContext(iBitmapContext64K_2);
	iBitmapDevice64K_3 = CFbsBitmapDevice::NewL(iBitmap64K_3);
	iBitmapDevice64K_3->CreateContext(iBitmapContext64K_3);
	
	//create 16MA bitmap
	iBitmap16MA_1 = new (ELeave) CFbsBitmap ;
	iBitmap16MA_1->Create(iBitmapSize, EColor16MA);
	iBitmap16MA_2 = new (ELeave) CFbsBitmap ;
	iBitmap16MA_2->Create(iBitmapSize, EColor16MA);
	iBitmap16MA_3 = new (ELeave) CFbsBitmap ;
	iBitmap16MA_3->Create(iBitmapSize, EColor16MA);

	iBitmapDevice16MA_1 = CFbsBitmapDevice::NewL(iBitmap16MA_1);
	iBitmapDevice16MA_1->CreateContext(iBitmapContext16MA_1);
	iBitmapDevice16MA_2 = CFbsBitmapDevice::NewL(iBitmap16MA_2);
	iBitmapDevice16MA_2->CreateContext(iBitmapContext16MA_2);
	iBitmapDevice16MA_3 = CFbsBitmapDevice::NewL(iBitmap16MA_3);
	iBitmapDevice16MA_3->CreateContext(iBitmapContext16MA_3);

	//create mask bitmap
	iBitmapMask = new (ELeave) CFbsBitmap ;
	iBitmapMask->Create(iBitmapSize, EGray2);
	
	CFbsBitmapDevice* theBitmapDevice = CFbsBitmapDevice::NewL(iBitmapMask);
	CGraphicsContext*  theBitmapContext = NULL;
	theBitmapDevice->CreateContext(theBitmapContext);
	TRect rect = TRect(iBitmapMask->SizeInPixels());
	theBitmapContext->SetBrushColor(KRgbWhite);
   	theBitmapContext->SetPenColor(KRgbWhite);
   	theBitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContext->DrawRect(rect);

	rect.Shrink(4, 4);
	theBitmapContext->SetBrushColor(KRgbBlack);
	theBitmapContext->SetPenColor(KRgbBlack);
	theBitmapContext->DrawEllipse(rect);
	delete theBitmapContext;
	delete theBitmapDevice;
	
	//create 256gray bitmap mask
	iBitmapGray256Mask = new (ELeave) CFbsBitmap;
	iBitmapGray256Mask->Create(iBitmapSize, EGray256);
	theBitmapDevice = CFbsBitmapDevice::NewL(iBitmapGray256Mask);
	theBitmapDevice->CreateContext(theBitmapContext);
	
	if(iBitmapSize != TSize(0, 0))
		{
	//fill bitmap with 256 gradation of gray
		TInt theStep = 256 / iBitmapGray256Mask->SizeInPixels().iWidth;
	
		for(TInt ii = 0; ii < iBitmapGray256Mask->SizeInPixels().iWidth; ii++)
			{
			TInt theSingleCol = theStep * ii; 
			TRgb theCol(theSingleCol, theSingleCol, theSingleCol);
			theBitmapContext->SetBrushColor(theCol);
			theBitmapContext->SetPenColor(theCol);
			TPoint ptFrom(ii, 0);
			TPoint ptTo(ii, iBitmapGray256Mask->SizeInPixels().iHeight);
			theBitmapContext->DrawLine(ptFrom, ptTo);
			}
		}
	delete theBitmapContext;
	delete theBitmapDevice;
	//--------------
	TFontSpec fontSpec(KTestFontTypefaceName,600);

	fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips(iFont, fontSpec));
	
	TFontSpec fontSpec1(KTestFontTypefaceName,100);
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips(iFont1, fontSpec1));
	}

void CTAlphaChannel::CalculateSizePrimitives(const TSize& aSize)
	{
	TInt theLen = aSize.iWidth / KSizeKoeff;
	
	iBitmapSize = TSize(2 * theLen, 2 * theLen);
	iRectangleSize = TSize(2 * theLen, 2 * theLen);
	iEllipseSize = TSize(4 * theLen, 2 * theLen);
	iTriangleSize = TSize(3 * theLen, 2 * theLen);
	iFirstCellWidth = static_cast <TInt> (1.2 * theLen);
	iFirstCellHeight = static_cast <TInt> (1.5 * theLen);
	}

void CTAlphaChannel::DrawOffscreenBitmapsL(const TRgb& /*aPen*/, const TRgb& aBrush,
										CGraphicsContext* aBitmapContext64K, CGraphicsContext* aBitmapContext16MA, 
										CFbsBitmap* aBitmap64K, CFbsBitmap* aBitmap16MA)
	{

	if(aBitmapContext64K && aBitmap64K)
		{
		SEpocBitmapHeader header = aBitmap64K->Header();
		TInt source_buffer_size = header.iBitmapSize / header.iSizeInPixels.iHeight * header.iBitsPerPixel ;
		TUint8* buffer = new(ELeave) TUint8[source_buffer_size];
		TPtr8 source_ptr(buffer, source_buffer_size, source_buffer_size);
		
		TUint16* bufferCur = reinterpret_cast<TUint16*> (buffer);
		TInt ii;
		for(ii = 0; ii < header.iSizeInPixels.iWidth; ++ii)
			{
			*bufferCur = aBrush._Color64K();
			bufferCur ++;
			}
	
		for(ii = 0; ii < header.iSizeInPixels.iHeight; ++ii)
			{
			aBitmap64K -> SetScanLine(source_ptr, ii);
			}
		
		delete [] buffer;
		}
		
	if(aBitmapContext16MA && aBitmap16MA)
		{
		SEpocBitmapHeader header = aBitmap16MA -> Header();
		TInt source_buffer_size = header.iBitmapSize / header.iSizeInPixels.iHeight * header.iBitsPerPixel ;
		TUint8* buffer = new(ELeave) TUint8[source_buffer_size];
		TPtr8 source_ptr(buffer, source_buffer_size, source_buffer_size);
		
		TUint32* bufferCur = reinterpret_cast<TUint32*> (buffer);
		TInt ii;
		for(ii = 0; ii < header.iSizeInPixels.iWidth; ++ii)
			{
			*bufferCur = aBrush._Color16MA();
			bufferCur ++;
			}
	
		for(ii = 0; ii < header.iSizeInPixels.iHeight; ++ii)
			{
			aBitmap16MA -> SetScanLine(source_ptr, ii);
			}
		
		delete [] buffer;
		}
	
	}

enum {EOpaque, ESemiTrans, ETrans};

TRgb CTAlphaChannel::GetBrush(TInt aIndex) const
	{
	switch(aIndex)
		{
			case EOpaque:
			return TRgb(0, 0, 255, 255);
			case ESemiTrans:
			return TRgb(0, 0, 255, 128);
			case ETrans:
			return TRgb(0, 0, 255, 0);
			default : break;
		}
		
	return 	TRgb(0, 0, 255, 255);
	}

TRgb CTAlphaChannel::GetPen(TInt aIndex) const
	{
	switch(aIndex)
		{
			case EOpaque:
			return TRgb(0, 0, 255, 255);
			case ESemiTrans:
			return TRgb(0, 0, 255, 128);
			case ETrans:
			return TRgb(0, 0, 255, 0);
			default : break;
		}
		
	return 	TRgb(0, 0, 255, 255);
	}
   
CGraphicsContext::TDrawMode CTAlphaChannel::GetDrawMode(TInt /*aIndex*/) const
	{
	return CGraphicsContext::EDrawModePEN;
	}

enum {EAll64K, EAll16MA, EAllDifferent};// later may add EAll16M
void CTAlphaChannel::SetDisplayModeConfiguration(TInt aConfig)
	{
	switch (aConfig)
		{
	case EAll64K:
		if(iForegroundWindowOpaque)
			iForegroundWindowOpaque->SetDisplayMode(EColor64K);
		if(iForegroundWindowSemiTrans)
			iForegroundWindowSemiTrans->SetDisplayMode(EColor64K);
		if(iForegroundWindowTrans)
			iForegroundWindowTrans->SetDisplayMode(EColor64K);
		break;
	case EAll16MA:
		if(iForegroundWindowOpaque)
			iForegroundWindowOpaque->SetDisplayMode(EColor16MA);
		if(iForegroundWindowSemiTrans)
			iForegroundWindowSemiTrans->SetDisplayMode(EColor16MA);
		if(iForegroundWindowTrans)
			iForegroundWindowTrans->SetDisplayMode(EColor16MA);
		break;
	case EAllDifferent:
		if(iForegroundWindowOpaque)
			iForegroundWindowOpaque->SetDisplayMode(EColor256);
		if(iForegroundWindowSemiTrans)
			iForegroundWindowSemiTrans->SetDisplayMode(EColor64K);
		if(iForegroundWindowTrans)
			iForegroundWindowTrans->SetDisplayMode(EColor16MA);
		break;
		}
	}

enum {ETiled, EOverlapping};

void CTAlphaChannel::SetPositionConfiguration(TInt aConfig)
	{
	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth)-10;
	TInt winHeight=screenSize.iHeight-10;
	if (aConfig==ETiled)
		{
		if(iForegroundWindowOpaque)
			iForegroundWindowOpaque->SetPos(TPoint(winWidth/5, 0));
		if(iForegroundWindowSemiTrans)
			iForegroundWindowSemiTrans->SetPos(TPoint(2*winWidth/5, 0));
		if(iForegroundWindowTrans)
			iForegroundWindowTrans->SetPos(TPoint(3*winWidth/5, 0));
		}
	else
		{
		if(iForegroundWindowOpaque)
			iForegroundWindowOpaque->SetPos(TPoint(winWidth/3, -winHeight/3));
		if(iForegroundWindowSemiTrans)
			iForegroundWindowSemiTrans->SetPos(TPoint(2*winWidth/5, winHeight/3));
		if(iForegroundWindowTrans)
			iForegroundWindowTrans->SetPos(TPoint(winWidth/2, -winHeight/3));
		}
	if (iForegroundWindowBottom)
		iForegroundWindowBottom->SetPos(TPoint(0, 0));
	if (iForegroundWindowTop)
		iForegroundWindowTop->SetPos(TPoint(4*winWidth/5, 0));
	}

enum {EVaryingTransparency, EVaryingColour};

void CTAlphaChannel::SetColourConfiguration(TInt aConfig)
	{
	RWindow* win;
	if (aConfig==EVaryingTransparency)
		{
		// red background, green foregrounds of varying transparency
		win = (RWindow*)(iBackgroundWindow->DrawableWin());
		win->SetBackgroundColor(TRgb(255,0,0,255));

		if(iForegroundWindowOpaque)
			iForegroundWindowOpaque->SetBackgroundColor(TRgb(0,255,0,255));
		if(iForegroundWindowSemiTrans)
			iForegroundWindowSemiTrans->SetBackgroundColor(TRgb(0,255,0,128));
		if(iForegroundWindowTrans)
			iForegroundWindowTrans->SetBackgroundColor(TRgb(0,255,0,0));
		}
	else
		{
		// white background, semi-transparent foregrounds in primary colours
		win = (RWindow*)(iBackgroundWindow->DrawableWin());
		win->SetBackgroundColor(TRgb(255,255,255,255));

		if(iForegroundWindowOpaque)
			iForegroundWindowOpaque->SetBackgroundColor(TRgb(255,0,0,128));
		if(iForegroundWindowSemiTrans)
			iForegroundWindowSemiTrans->SetBackgroundColor(TRgb(0,255,0,128));
		if(iForegroundWindowTrans)
			iForegroundWindowTrans->SetBackgroundColor(TRgb(0,0,255,128));
		}
	}

void CTAlphaChannel::DoMoving()
	{
	TPoint pos;
	for (TInt i = 0; i<20; i++)
		{
		if (iForegroundWindowBottom)
			{
			pos = iForegroundWindowBottom->Position();
			pos += TPoint(1,5);
			iForegroundWindowBottom->SetPos(pos);
			}

		if (iForegroundWindowOpaque)
			{
			pos = iForegroundWindowOpaque->Position();
			pos += TPoint(1,5);
			iForegroundWindowOpaque->SetPos(pos);
			}

		if (iForegroundWindowSemiTrans)
			{
			pos = iForegroundWindowSemiTrans->Position();
			pos += TPoint(1,5);
			iForegroundWindowSemiTrans->SetPos(pos);
			}

		if (iForegroundWindowTrans)
			{
			pos = iForegroundWindowTrans->Position();
			pos += TPoint(1,5);
			iForegroundWindowTrans->SetPos(pos);
			}

		if (iForegroundWindowTop)
			{
			pos = iForegroundWindowTop->Position();
			pos += TPoint(1,5);
			iForegroundWindowTop->SetPos(pos);
			}

		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(50000));
		}

	}

void CTAlphaChannel::DoMoveBehind()
	{
	TPoint pos = iBackgroundWindow->Position();
	for (TInt i = 0; i<20; i++)
		{
		pos += TPoint(0,5);
		iBackgroundWindow->SetPos(pos);

		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(50000));
		}
	iBackgroundWindow->SetPos(TPoint(5,5));
	}

void CTAlphaChannel::DoInvisibility()
	{
	RWindow* win;
	if (iForegroundWindowBottom)
		{
		win = (RWindow*)(iForegroundWindowBottom->DrawableWin());
		win->SetVisible(EFalse);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));

		win->SetVisible(ETrue);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));
		}
	if (iForegroundWindowOpaque)
		{
		win = (RWindow*)(iForegroundWindowOpaque->DrawableWin());
		win->SetVisible(EFalse);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));

		win->SetVisible(ETrue);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));
		}
	if (iForegroundWindowSemiTrans)
		{
		win = (RWindow*)(iForegroundWindowSemiTrans->DrawableWin());
		win->SetVisible(EFalse);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));

		win->SetVisible(ETrue);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));
		}
	if (iForegroundWindowTrans)
		{
		win = (RWindow*)(iForegroundWindowTrans->DrawableWin());
		win->SetVisible(EFalse);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));

		win->SetVisible(ETrue);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));
		}
	if (iForegroundWindowTop)
		{
		win = (RWindow*)(iForegroundWindowTop->DrawableWin());
		win->SetVisible(EFalse);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));

		win->SetVisible(ETrue);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(1000000));
		}
	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0328
  	
  	@SYMTestCaseDesc			Testing a Fading.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Set fading parameters. Draw all graphics primitives and bitmaps with 
		 various transparency				.
 
  	@SYMTestExpectedResults
 				All windows should be drawn according their fading values. 
 */

void CTAlphaChannel::TestFading()
	{
	iIsFading = ETrue;
	iBlackFading = 0;
	iWhiteFading = 128;
	iText.Format(_L("Fading. %dX%d"), iBlackFading, iWhiteFading);
	INFO_PRINTF1(iText);
	DrawTestWindowsNow();

	User::After(TTimeIntervalMicroSeconds32(1000000 * 1));
	iIsFading = EFalse;
	DrawTestWindowsNow();
	iIsFading = ETrue;
	iBlackFading = 128;
	iWhiteFading = 255;
	iText.Format(_L("Fading. %dX%d"), iBlackFading, iWhiteFading);
	INFO_PRINTF1(iText);
	DrawTestWindowsNow();

	iIsFading = EFalse;
	}
	
/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0329
  	
  	@SYMTestCaseDesc			Tests moving of foreground windows.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Set fading parameters. Set position of foreground windows as tile				.
 
  	@SYMTestExpectedResults
 			Foreground window has to be redrawn properly.
 */

void CTAlphaChannel::TestMoving()
	{
	SetPositionConfiguration(ETiled);
	DoMoving();
	SetPositionConfiguration(ETiled);
	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0330
  	
  	@SYMTestCaseDesc			Tests moving of foreground windows.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Set fading parameters. Moves foreground windows over the screen			.
 
  	@SYMTestExpectedResults
 			Foreground window has to be redrawn properly.
 */


void CTAlphaChannel::TestMovingOverlapping()
	{
	TPoint pos;

	for (TInt i = 0; i < 20; i++)
		{
		if (iForegroundWindowOpaque)
			{
			pos = iForegroundWindowOpaque->Position();
			pos += TPoint(3,0);
			iForegroundWindowOpaque->SetPos(pos);
			}
		if (iForegroundWindowTrans)
			{
			pos = iForegroundWindowTrans->Position();
			pos -= TPoint(3,0);
			iForegroundWindowTrans->SetPos(pos);
			}
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(50000));
		}
	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0331
  	
  	@SYMTestCaseDesc			Testing transparency factor for windows with alpha channel.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Sets background colour with various level of transparency			.
 
  	@SYMTestExpectedResults
 			Foreground window must be redrawn properly.
 */
void CTAlphaChannel::TestChangingTransparencyFactor()
	{
	TInt i = 0;
	while (i <= 255)
		{
		iForegroundWindowOpaque->SetBackgroundColor(TRgb(0,255,0,255-i));
		iForegroundWindowTrans->SetBackgroundColor(TRgb(0,255,0,i));
		iForegroundWindowOpaque->DrawNow();
		iForegroundWindowTrans->DrawNow();
		i+=15;
		}
	i=0;
	while (i <= 255)
		{
		iForegroundWindowOpaque->SetBackgroundColor(TRgb(0,255,0,i));
		iForegroundWindowTrans->SetBackgroundColor(TRgb(0,255,0,255-i));
		iForegroundWindowOpaque->DrawNow();
		iForegroundWindowTrans->DrawNow();
		User::After(TTimeIntervalMicroSeconds32(50000));// 20 frames per second
		i+=15;
		}
	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0332
  	
  	@SYMTestCaseDesc			Testing redrawing of foreground windows while their positions and 
  				invisibility have been changed.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Sets position of the foreground windows as overlapping, tiled.
		Change visability of semi transparent foreground window.		.
 
  	@SYMTestExpectedResults
 		Foreground window must be redrawn properly.
 */

void CTAlphaChannel::TestInvisibility()
	{
	RWindow* win = (RWindow*)(iForegroundWindowSemiTrans->DrawableWin());
	SetPositionConfiguration(EOverlapping);
	TheClient->iWs.Flush();	
	User::After(TTimeIntervalMicroSeconds32(1000000));

	win->SetVisible(EFalse);
	TheClient->iWs.Flush();	
	User::After(TTimeIntervalMicroSeconds32(1000000));

	win->SetVisible(ETrue);
	TheClient->iWs.Flush();	
	User::After(TTimeIntervalMicroSeconds32(1000000));

	SetPositionConfiguration(ETiled);

	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0333
  	
  	@SYMTestCaseDesc			Testing redrawing of foreground windows after a 
  				background window has been moved.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Sets position of the foreground windows as tiled.
		Moves background window.
 
  	@SYMTestExpectedResults
 		Foreground windows must be redrawn properly.
 */
void CTAlphaChannel::TestMoveUnderneath()
	{
	SetPositionConfiguration(ETiled);
	DoMoveBehind();
	}
/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0334
  	
  	@SYMTestCaseDesc			Testing redrawing of foreground windows after a 
  				background window has been moved.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Sets different position of the foreground windows: tiled, overlapping.
		Moves background window.
 
  	@SYMTestExpectedResults
 		Foreground windows has to be redrawn properly.
 */

void CTAlphaChannel::TestMoveBehindInvisible()
	{
	SetPositionConfiguration(ETiled);
	RWindow* win;
	win = (RWindow*)(iForegroundWindowOpaque->DrawableWin());
	win->SetVisible(EFalse);
	win = (RWindow*)(iForegroundWindowSemiTrans->DrawableWin());
	win->SetVisible(EFalse);
	win = (RWindow*)(iForegroundWindowTrans->DrawableWin());
	win->SetVisible(EFalse);
	TheClient->iWs.Flush();

	DoMoveBehind();

	SetPositionConfiguration(EOverlapping);
	DoMoveBehind();

	win = (RWindow*)(iForegroundWindowOpaque->DrawableWin());
	win->SetVisible(ETrue);
	win = (RWindow*)(iForegroundWindowSemiTrans->DrawableWin());
	win->SetVisible(ETrue);
	win = (RWindow*)(iForegroundWindowTrans->DrawableWin());
	win->SetVisible(ETrue);
	TheClient->iWs.Flush();
	}
/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0335
  	
  	@SYMTestCaseDesc			Testing redrawing of foreground windows.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Redraws background window
 
  	@SYMTestExpectedResults
 		Foreground windows should be redrawn properly, if background window 
  		 has been redrawn.
 */
void CTAlphaChannel::TestRedrawBehind()
	{
	TheClient->iWs.Flush();	
	iBackgroundWindow->iState += 1;
	iBackgroundWindow->DrawNow();
	TheClient->iWs.Flush();	
	User::After(TTimeIntervalMicroSeconds32(1000000));
	}
/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0336
  	
  	@SYMTestCaseDesc			Testing transparency with RWsSprite class.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Creates RBackedUpWindow window and moves it over transparent window
		appears and dissapears set up window as transparent
 
  	@SYMTestExpectedResults
 			Must work with transparent windows. 
 */

void CTAlphaChannel::TestAnimationL()
	{
	for(TInt ii = 0; ii < iArrWindow.Count(); ii++)
		{
		iArrWindow[ii]->StartAnimationL();
		}
	}
/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0337
  	
  	@SYMTestCaseDesc			Testing transparency with RBackedUpWindow window class.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Creates RBackedUpWindow window and moves it over transparent window
		appears and dissapears set up transparent window as transparent
 
  	@SYMTestExpectedResults
 				Do not expect correct work of alpha channel with 
 				RBackedUpWindow window class. 
*/
void CTAlphaChannel::TestBackedWindowL()
	{
	for(TInt ii = 0; ii < iArrWindow.Count(); ii++)
		{
		iArrWindow[ii]->CreateBackedWindowL();
		}
	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0338
  	
  	@SYMTestCaseDesc			Implication of setting SetDrawOpaque on drawing with alpha channel.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 Change graphic context to opaque and none opaque and rewdraw all test windows
 
  	@SYMTestExpectedResults
 				must not impact on output with alpha channel 
 */

void CTAlphaChannel::TestEffectSetOpaque()
	{
	for(TInt ii = 0; ii < 3; ii++)
		{
		
		for(TInt ii = 0; ii < iArrWindow.Count(); ii++)
			{
			iArrWindow[ii]->SetDrawOpaque(EFalse);
			}
	
		DrawTestWindowsNow();
		User::After(TTimeIntervalMicroSeconds32(1000000));

		for(TInt jj = 0; jj < iArrWindow.Count(); jj++)
			{
			iArrWindow[jj]->SetDrawOpaque(ETrue);
			}

		DrawTestWindowsNow();
		User::After(TTimeIntervalMicroSeconds32(1000000));
		}
	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0339
  	
  	@SYMTestCaseDesc			Redrawing of child windows with transparency.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 	Creates a few child windows with various levels of transparency.
		Moves parent window over the screen.
  	@SYMTestExpectedResults
 				Child and parent windows must be redrawn properly 
 */

void CTAlphaChannel::TestChildWindowL()
	{
	SetPositionConfiguration(ETiled);
	SetColourConfiguration(EVaryingColour);
	DrawTestWindowsNow(ETrue);
	TDisplayMode mode = EColor64K;
	CTWinAlphaForeground* childWin[] = {NULL, NULL, NULL, NULL, NULL};
	if (iForegroundWindowBottom)
		{
		childWin[0] = CTWinAlphaForeground::NewL(*this, TPoint(0,0),TSize(50,50),iForegroundWindowBottom,*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ENonTransparentAlpha);
		childWin[0]->DrawNow();
		}
	if (iForegroundWindowOpaque)
		{
		childWin[1] = CTWinAlphaForeground::NewL(*this, TPoint(0,0),TSize(50,50),iForegroundWindowOpaque,*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ETransparencyFactor);
		childWin[1]->DrawNow();
		}
	if (iForegroundWindowSemiTrans)
		{
		childWin[2] = CTWinAlphaForeground::NewL(*this, TPoint(0,0),TSize(50,50),iForegroundWindowSemiTrans,*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ETransparencyAlpha);
		childWin[2]->DrawNow();
		}
	if (iForegroundWindowTrans)
		{
		childWin[3] = CTWinAlphaForeground::NewL(*this, TPoint(0,0),TSize(50,50),iForegroundWindowTrans,*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ENonTransparentAlpha);
		childWin[3]->DrawNow();
		}
	if (iForegroundWindowTop)
		{
		childWin[4] = CTWinAlphaForeground::NewL(*this, TPoint(0,0),TSize(50,50),iForegroundWindowTop,*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ENonTransparentAlpha);
		childWin[4]->DrawNow();
		}
	TheClient->iWs.Flush();	

	User::After(TTimeIntervalMicroSeconds32(1000000));
	DoMoving();

	delete childWin[0];
	delete childWin[1];
	delete childWin[2];
	delete childWin[3];
	delete childWin[4];
	}
/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0340
  	
  	@SYMTestCaseDesc			Redrawing of multiple child windows with transparency.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 	Creates multiple child windows with various levels of transparency.
		Moves parent windows over the screen.
  	@SYMTestExpectedResults
 				Child and parent windows must be redrawn properly 
 */

void CTAlphaChannel::TestMultipleChildrenL()
	{
	SetPositionConfiguration(ETiled);
	SetColourConfiguration(EVaryingColour);
	TDisplayMode mode = EColor64K;
	CTWinAlphaForeground* childWin[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	if (iForegroundWindowBottom)
		{
		childWin[0] = CTWinAlphaForeground::NewL(*this, TPoint(0,0),TSize(50,50),iForegroundWindowBottom,*TheClient->iGc, &mode, TRgb(255, 255, 255,128), ETransparencyFactor);
		childWin[1] = CTWinAlphaForeground::NewL(*this, TPoint(20,20),TSize(50,50),childWin[0],*TheClient->iGc, &mode, TRgb(255, 255, 255,128), ENonTransparentAlpha);
		childWin[0]->DrawNow();
		childWin[1]->DrawNow();
		}
	if (iForegroundWindowOpaque)
		{
		childWin[4] = CTWinAlphaForeground::NewL(*this, TPoint(0,0),TSize(100,100),iForegroundWindowOpaque,*TheClient->iGc, &mode, TRgb(255, 255, 255,128), ETransparencyAlpha);
		childWin[5] = CTWinAlphaForeground::NewL(*this, TPoint(20,80),TSize(50,50),iForegroundWindowOpaque,*TheClient->iGc, &mode, TRgb(255, 255, 255,128), ENonTransparentAlpha);
		childWin[6] = CTWinAlphaForeground::NewL(*this, TPoint(20,20),TSize(60,60),childWin[4],*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ENonTransparentAlpha);
		childWin[7] = CTWinAlphaForeground::NewL(*this, TPoint(20,20),TSize(50,50),childWin[6],*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ENonTransparentAlpha);
		childWin[4]->DrawNow();
		childWin[5]->DrawNow();
		childWin[6]->DrawNow();
		childWin[7]->DrawNow();
		}
	DrawTestWindowsNow(ETrue);

	TestMoving();

	delete childWin[0];
	delete childWin[1];
	delete childWin[2];
	delete childWin[3];
	delete childWin[4];
	delete childWin[5];
	delete childWin[6];
	delete childWin[7];
	}

/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0341
  	
  	@SYMTestCaseDesc			Test transparent window which positioned under opaque.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 	Creates an opaque window on the topt of transparent windows.
		Moves transparent windows over the screen.
  	@SYMTestExpectedResults
 				Transparent windows must be redrawn properly 
 */

void CTAlphaChannel::TestTransparentMovingUnderOpaqueL()
	{
	SetColourConfiguration(EVaryingColour);
	TDisplayMode mode = EColor64K;
	CTWinAlphaForeground* win = CTWinAlphaForeground::NewL(*this, TPoint(0,100),TSize(600,40),TheClient->iGroup,*TheClient->iGc, &mode, TRgb(0, 255, 0,128), ENonTransparentAlpha);
	win->DrawNow();

	TestMoving();

	delete win;
	}
/** 
	@SYMTestCaseID	GRAPHICS-WSERV-0342
  	
  	@SYMTestCaseDesc			Changing orinary position of the foreground windows.
  	
  	@SYMTestPriority			High
  	
  	@SYMTestStatus      		Implemented
  	
  	@SYMTestActions		
		 	Set ordinal position of foreground windows.
  	@SYMTestExpectedResults
 				Foreground windows must be redrawn properly 
 */

void CTAlphaChannel::TestSetOrdinalPosition()
	{
	SetColourConfiguration(EVaryingColour);
	SetPositionConfiguration(EOverlapping);
	DrawTestWindowsNow(ETrue);
	TheClient->iWs.Flush();	
	User::After(TTimeIntervalMicroSeconds32(1000000));
	RWindow* win;
	for (TInt i=0; i<3; i++)
		{
		if (iForegroundWindowOpaque)
			{
			win = (RWindow*)(iForegroundWindowOpaque->DrawableWin());
			win->SetOrdinalPosition(0);
			TheClient->iWs.Flush();	
			User::After(TTimeIntervalMicroSeconds32(1000000));
			}
		if (iForegroundWindowSemiTrans)
			{
			win = (RWindow*)(iForegroundWindowSemiTrans->DrawableWin());
			win->SetOrdinalPosition(0);
			TheClient->iWs.Flush();	
			User::After(TTimeIntervalMicroSeconds32(1000000));
			}
		if (iForegroundWindowTrans)
			{
			win = (RWindow*)(iForegroundWindowTrans->DrawableWin());
			win->SetOrdinalPosition(0);
			TheClient->iWs.Flush();	
			User::After(TTimeIntervalMicroSeconds32(1000000));
			}
		}
	}

void CTAlphaChannel::RunTestCaseL(TInt aCurTestCase)
	{
	User::After(TTimeIntervalMicroSeconds32(1000000 * 2));
	
	((CTAlphaChannelStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (aCurTestCase)
		{
		case 1:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			CreateForegroundWindowsL(iSizeForegroundWindow, EColor16MU);
			break;
		case 2:
			{
			CreateForegroundWindowsL(iSizeForegroundWindow, EColor64K);
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0339"));
			TestChildWindowL();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0340"));
			TestMultipleChildrenL();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0342"));
			TestSetOrdinalPosition();
			}
			break;
		case 3: 
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0336"));
			TestAnimationL();
		case 4: 
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0337"));
			TestBackedWindowL();
			break;
		case 5: 
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0338"));
			TestEffectSetOpaque();
			break;
		case 6:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iIsFading = EFalse;
			DrawTestWindowsNow();
			INFO_PRINTF1(_L("Anti-aliasing"));
			iIsFading = EFalse;
			iDrawText = ETrue;
			DrawTestWindowsNow();
			break;
		case 7:	
			SetColourConfiguration(EVaryingColour);
			SetPositionConfiguration(EOverlapping);
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0335"));
			TestRedrawBehind();		
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0329"));
			TestMoving();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0330"));
			TestMovingOverlapping();		
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0341"));
			TestMoveBehindInvisible();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0334"));
			TestTransparentMovingUnderOpaqueL();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0331"));
			TestInvisibility();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0332"));
			TestChangingTransparencyFactor();
			break;
		case 8:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0328"));
			TestFading();
			break;
		case 9:
			{
			((CTAlphaChannelStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iDrawText = EFalse;
			SetDisplayModeConfiguration(EAll16MA);
			}
			break;
		case 10:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iIsFading = EFalse;
			DrawTestWindowsNow();
			INFO_PRINTF1(_L("Anti-aliasing"));
			iIsFading = EFalse;
			iDrawText = ETrue;
			DrawTestWindowsNow();
			break;
		case 11:
			SetColourConfiguration(EVaryingColour);
			SetPositionConfiguration(EOverlapping);
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0333"));
			TestMoveUnderneath();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0335"));
			TestRedrawBehind();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0329"));
			TestMoving();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0332"));
			TestInvisibility();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0331"));
			TestChangingTransparencyFactor();
			break;
		case 12:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0328"));
			TestFading();
			break;
		case 13:
			SetDisplayModeConfiguration(EAllDifferent);
			SetColourConfiguration(EVaryingColour);
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0333"));
			TestMoveUnderneath();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0335"));
			TestRedrawBehind();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0332"));
			TestInvisibility();
			((CTAlphaChannelStep*)iStep)->RecordTestResultL();
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0329"));
			TestMoving();
			break;
		case 14:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0336"));
			TestAnimationL();
			break;
		case 15:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0337"));
			TestBackedWindowL();
			break;
		case 16:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0338"));
			TestEffectSetOpaque();
			break;
		case 17:
			((CTAlphaChannelStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTAlphaChannelStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTAlphaChannelStep*)iStep)->RecordTestResultL();
	}
	
/** Calculate foreground windows margin and create them*/
void CTAlphaChannel::CreateForegroundWindowsL(const TSize& aSize, TDisplayMode aMode)
	{
	TPoint pt = TPoint(5, 5);
	
	//creating foreground windows
	//bottom window
	TDisplayMode dispMode = aMode;
	iForegroundWindowBottom = CTWinAlphaForeground::NewL(*this, pt,aSize,TheClient->iGroup,*TheClient->iGc, &dispMode, TRgb(0, 255, 0,128), ETransparencyFactor);
	iArrWindow.AppendL( iForegroundWindowBottom);

	//dispMode = EColor16MA;
	pt.iX += aSize.iWidth + KWindowIndention;
	//opaque window
	iForegroundWindowOpaque = CTWinAlphaForeground::NewL(*this, pt,aSize,TheClient->iGroup,*TheClient->iGc, &dispMode, TRgb(0, 255, 0, 255), ETransparencyAlpha);
	iArrWindow.AppendL( iForegroundWindowOpaque);

	pt.iX += aSize.iWidth + KWindowIndention;
	//semi-transparent window
	iForegroundWindowSemiTrans = CTWinAlphaForeground::NewL(*this, pt,aSize,TheClient->iGroup,*TheClient->iGc, &dispMode, TRgb(0, 255, 0, 128), ETransparencyAlpha);
	iArrWindow.AppendL( iForegroundWindowSemiTrans);

	//transparent window
	pt.iX += aSize.iWidth + KWindowIndention;
	iForegroundWindowTrans = CTWinAlphaForeground::NewL(*this, pt,aSize,TheClient->iGroup,*TheClient->iGc, &dispMode, TRgb(0, 255, 0, 0), ETransparencyAlpha);
	iArrWindow.AppendL( iForegroundWindowTrans);

	// top window
	pt.iX += aSize.iWidth + KWindowIndention;
	iForegroundWindowTop = CTWinAlphaForeground::NewL(*this, pt,aSize,TheClient->iGroup,*TheClient->iGc, &dispMode, TRgb(0, 255, 0,128), ENonTransparentAlpha);
	iArrWindow.AppendL( iForegroundWindowTop);
	}

void CTAlphaChannel::DestroyForegroundWindows()
	{
	iArrWindow.ResetAndDestroy();
	iForegroundWindowBottom = NULL;
	iForegroundWindowOpaque = NULL;
	iForegroundWindowSemiTrans = NULL;
	iForegroundWindowTrans = NULL;
	iForegroundWindowTop = NULL;
	}

/** Draw all foreground windows */
void CTAlphaChannel::DrawTestWindowsNow(TBool aDrawBackgroundWin)
	{
	if(iBackgroundWindow && aDrawBackgroundWin)
		{
		iBackgroundWindow->DrawNow();
		}

	for(TInt ii = 0; ii < iArrWindow.Count(); ii++)
		{
		iArrWindow[ii]->DrawNow();
		}
	}
	
//-------------

CTWinAlphaForeground* CTWinAlphaForeground::NewL(CTAlphaChannel& aTest, TPoint aPos, TSize aSize, CTWinBase *aParent, CWindowGc &aGc, TDisplayMode *aMode, TRgb aCol, TInt aTransparencyType)
	{
	CTWinAlphaForeground* theWin = new(ELeave) CTWinAlphaForeground(aTest);
	
	theWin->ConstructL(*aParent);
	if (aMode)
		theWin->SetDisplayMode(*aMode);
	theWin->SetExtL(aPos, aSize);
	theWin->AssignGC(aGc);
	theWin->PrepareForDrawingL();	

	// for hardware testing	only we create an additional bitmap
#ifndef __WINS__
	theWin->CreateBackgroundBitmapL(*aMode);
#endif
		
	RWindow* win = (RWindow*) (theWin->DrawableWin());
	win->SetShadowHeight(0);
	win->SetShadowDisabled(ETrue);
	switch (aTransparencyType)
		{
	case ETransparencyFactor:
		{
		win->SetTransparencyFactor(aCol);
		theWin->SetBackgroundColor(aCol);
		}
		break;
	case ETransparencyAlpha:
		win->SetTransparencyAlphaChannel();
		// fall through into next case
	case ENonTransparentAlpha:
	default:
		theWin->SetBackgroundColor(aCol);
		break;
		}
	
	theWin->Activate();

	return theWin;
	}

CTWinAlphaForeground::~CTWinAlphaForeground()
	{
	if(iPolygon)
		{
		iPolygon->Reset();
		delete iPolygon;
		}
	delete iBitmapBackground;
	delete iBitmapDeviceBackground;
	delete iBitmapContextBackground;
	}
	
CTWinAlphaForeground::CTWinAlphaForeground(CTAlphaChannel& aTest)
	: iTest(aTest)
	{	
	}


void CTWinAlphaForeground::SetDisplayMode(TDisplayMode aDisplayMode)
	{
	BaseWin()->SetRequiredDisplayMode(aDisplayMode);
	switch (aDisplayMode)
		{
	case EColor256:
		iTitle1 = _L("256");
		break;
	case EColor64K:
		iTitle1 = _L("64K");
		break;
	case EColor16MU:
		iTitle1 = _L("16MU");
		break;
	case EColor16MA:
		iTitle1 = _L("16MA");
		break;
	case EColor16MAP:
		iTitle1 = _L("16MAP");
		break;
	default:
		iTitle1 = _L("");
		break;
		}
	}

void CTWinAlphaForeground::SetBackgroundColor(TRgb aRgb)
	{
	switch (aRgb.Alpha())
		{
		case 0:
		iTitle2 = _L(" trans ");		
		break;
		case 255:
		iTitle2 = _L(" opaque ");		
		break;
		case 128:
		default:
		iTitle2 = _L(" s-trans ");		
		break;
		}
	if (aRgb.Red())
		iTitle2 += _L("R");
	if (aRgb.Green())
		iTitle2 += _L("G");
	if (aRgb.Blue())
		iTitle2 += _L("B");

	iBackgroundColor = aRgb;
	((RWindow*) DrawableWin())->SetBackgroundColor(aRgb);
	}

void CTWinAlphaForeground::Draw()
	{
	CBitmapContext* theGc = TheClient->iGc;
	((CWindowGc*)theGc)->SetOpaque(iDrawOpaque);
	
	if(iBitmapContextBackground)
		{
		//CGraphicsContext
		theGc = iBitmapContextBackground;
		CleanBackgroundBitmap();
		}
	if(iTest.iFont1)
		theGc->UseFont(iTest.iFont1);
	
	
	theGc->SetFaded(EFalse);
	DrawTable(*theGc);

	TInt numRows = sizeof(iRows) / sizeof(iRows[0]) ;

	theGc->SetBrushStyle(iTest.iBrushStyle);
	if(iTest.iFont && iTest.iDrawText)
		{
		theGc->DiscardFont();
		theGc->UseFont(iTest.iFont);
		}
	
	theGc->SetFaded(iTest.iIsFading);
	
	// the factor and offset are calculated as follows: 
	// iFadeMapFactor = iWhiteFading - iBlackFading;
	// iFadeMapOffset = iBlackFading;

	theGc->SetFadingParameters(iTest.iBlackFading, iTest.iWhiteFading); //black and white
		
	for(TInt ii = 0; ii < numRows - 1; ii++)
		{
		theGc -> SetBrushStyle(iTest.iBrushStyle);

		TRect theRect = TRect(iCol, iRows[ii], Size().iWidth, iRows[ii + 1]);
				
   		TRgb theBrush = iTest.GetBrush(ii);
   		TRgb thePen = iTest.GetPen(ii);

   		CGraphicsContext::TDrawMode theDrawMode = iTest.GetDrawMode(ii);
    		
   		theGc->SetBrushColor(theBrush);
		theGc->SetPenColor(thePen);
   		theGc->SetDrawMode(theDrawMode);
		
		CGraphicsContext* theBitmapContext64K;
		CGraphicsContext* theBitmapContext16MA;
		CFbsBitmap* theBitmap64K;
		CFbsBitmap* theBitmap16MA;

		switch(ii)
			{
			case 0:
				{
				theBitmapContext64K = iTest.iBitmapContext64K_1;
				theBitmapContext16MA = iTest.iBitmapContext16MA_1;
				theBitmap64K = iTest.iBitmap64K_1;
				theBitmap16MA = iTest.iBitmap16MA_1;
				break;
				}
			case 1:
				{
				theBitmapContext64K = iTest.iBitmapContext64K_2;
				theBitmapContext16MA = iTest.iBitmapContext16MA_2;
				theBitmap64K = iTest.iBitmap64K_2;
				theBitmap16MA = iTest.iBitmap16MA_2;
				break;
				}
			default:
				{
				theBitmapContext64K = iTest.iBitmapContext64K_3;
				theBitmapContext16MA = iTest.iBitmapContext16MA_3;
				theBitmap64K = iTest.iBitmap64K_3;
				theBitmap16MA = iTest.iBitmap16MA_3;
				break;
				}
			}
			if(iTest.iDrawText)
				{
				DrawTextInCell(*theGc, theRect);
				}
			else
				{
				TRAP_IGNORE(iTest.DrawOffscreenBitmapsL(thePen, theBrush, theBitmapContext64K, 
					theBitmapContext16MA, theBitmap64K, theBitmap16MA));
				DrawPrimitivesInCell(*theGc, theRect, theBitmap64K, theBitmap16MA);
				}
		TheClient->iWs.Flush();	
		}

	theGc->DiscardFont();

	if(iBitmapContextBackground)
		{
		theGc = TheClient->iGc;
		theGc->BitBlt(TPoint(0, 0), iBitmapBackground);
		}
	}

void CTWinAlphaForeground::SetPoligonLocation(const TPoint &ptOffset)
	{
	TRect rect = TRect(iTest.iTriangleSize);
	(*iPolygon)[0] = rect.iTl + ptOffset;
	(*iPolygon)[1] = TPoint(rect.iTl.iX, rect.iBr.iY) + ptOffset;
	(*iPolygon)[2] = rect.iBr + ptOffset;
	}
	
/** Define boundary of the table*/
void CTWinAlphaForeground::CalculateTableMargin()
	{
	TInt numRows = sizeof(iRows) / sizeof(iRows[0]) ;
	iRows[0] = iTest.iFirstCellHeight;
	TInt theRowHeight = (Size().iHeight - iTest.iFirstCellHeight) / 3;
	for(TInt ii = 1; ii < numRows; ii++)
		{
		iRows[ii] = iRows[ii-1] + theRowHeight;
		}
	}
	
/** Draw a table which comprises 3 rows: for transparent, semi-transparent 
	and opaque output

*/	
void CTWinAlphaForeground::DrawTable(CBitmapContext& aGc) const
	{
	TInt numRows = sizeof(iRows) / sizeof(iRows[0]) ;
	
	aGc.SetPenColor(iTest.iPenTable);
	
	for (TInt ii = 0; ii < numRows - 1; ii++)
		{
		TBuf<4> iBuf;
		TPoint pt1 = TPoint(0, iRows[ii]);
		TPoint pt2 = TPoint(Size().iWidth, iRows[ii]);

		aGc.DrawLine(pt1, pt2);
		
		if(iCol)
			{
			TPoint pt3 = TPoint(0, iRows[ii]) + TPoint(2,(iRows[1] - iRows[0]) / 2); 
			switch(ii)
				{
					case 0: iBuf = _L("o"); break;
					case 1: iBuf = _L("s"); break;
					case 2: iBuf = _L("t"); break;
					default : iBuf = _L(""); break;
				}
			aGc.DrawText(iBuf, pt3); 
			}
		}
	if(iCol)
		{
		TPoint pt3 = TPoint(iCol, iRows[0]) + TPoint(1,-2); 
		TBuf<32> text = iTitle1;
		text += iTitle2;
		aGc.DrawText(text, pt3); 
	
		TPoint pt1 = TPoint(iCol, 0);
		TPoint pt2 = TPoint(iCol, Size().iHeight);
		aGc.DrawLine(pt1, pt2);
		}
		
	}

/** Draw truetype font to check anti-aliasing*/
void CTWinAlphaForeground::DrawTextInCell(CBitmapContext& aGc, const TRect& aRect)
	{
    TPoint pt(aRect.iTl.iX, aRect.iBr.iY);
    pt += TPoint(2, -10);
    
    aGc.DrawText(_L("ABCD"), pt);
	}

/** Draw graphics primitive in a cell:
	rectangle, ellipse, triangle, a few lines, bitmaps

 */	
void CTWinAlphaForeground::DrawPrimitivesInCell(CBitmapContext& aGc, const TRect& aRect, 
								CFbsBitmap* aBitmap64K, CFbsBitmap* aBitmap16MA)
	{
	TRect theCellRect = aRect;
	theCellRect.Shrink(1, 1);
	
	//rectangle
	TRect rect = TRect(iTest.iRectangleSize);
	rect.Move(theCellRect.iTl);
    aGc.DrawRect(rect);
    
    //ellipse
    rect = TRect(iTest.iEllipseSize);
	rect.Move(theCellRect.iTl + TPoint(iTest.iRectangleSize.iWidth + 5, 0));
    aGc.DrawEllipse(rect);

	//triangle
	TPoint pt = TPoint(rect.iBr.iX, rect.iTl.iY) + TPoint(5, 0);
	SetPoligonLocation(pt);
   	aGc.DrawPolygon(iPolygon);
    
    //DrawLine
    pt = pt + TPoint (((*iPolygon)[2]).iX - ((*iPolygon)[0]).iX, 0) + TPoint(5, 2);
    rect = TRect(iTest.iTriangleSize);
    rect.Move(pt);
    aGc.DrawLine(rect.iTl, TPoint(rect.iTl.iX, rect.iBr.iY));
    aGc.DrawLine(TPoint(rect.iTl.iX, rect.iBr.iY), rect.iBr);
    aGc.DrawLine(rect.iBr, TPoint(rect.iBr.iX, rect.iTl.iY));
    aGc.DrawLine(TPoint(rect.iBr.iX, rect.iTl.iY), rect.iTl);
    aGc.DrawLine(rect.iTl, rect.iBr);
    aGc.DrawLine(TPoint(rect.iBr.iX, rect.iTl.iY), TPoint(rect.iTl.iX, rect.iBr.iY));

    //64K bitmap
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    pt = TPoint(theCellRect.iTl.iX, rect.iBr.iY) + TPoint(0, 5);
    if(aBitmap64K)
    	{
    	aGc.BitBlt(pt, aBitmap64K);
    	pt = pt + TPoint( aBitmap64K->SizeInPixels().iWidth, 0) + TPoint(2, 0);

    	aGc.BitBltMasked(pt, 
    		aBitmap64K, TRect(aBitmap64K->SizeInPixels()),
	    	iTest.iBitmapGray256Mask, EFalse);
    	pt = pt + TPoint( aBitmap64K->SizeInPixels().iWidth, 0) + TPoint(2, 0);

    	aGc.BitBltMasked(pt, 
	    	aBitmap64K, TRect(aBitmap64K->SizeInPixels()),
    		iTest.iBitmapMask, EFalse);

    	pt = pt + TPoint( aBitmap64K->SizeInPixels().iWidth, 0) + TPoint(2, 0);
	   	}
    
    if(aBitmap16MA)
    	{
    	aGc.BitBlt(pt, aBitmap16MA);

    	pt = pt + TPoint( aBitmap16MA->SizeInPixels().iWidth, 0) + TPoint(2, 0);

    	aGc.BitBltMasked(pt, 
	    	aBitmap16MA, TRect(aBitmap16MA->SizeInPixels()),
    		iTest.iBitmapGray256Mask, EFalse);

    	pt = pt + TPoint( aBitmap16MA->SizeInPixels().iWidth, 0) + TPoint(2,0);

    	pt = pt + TPoint( 0, aBitmap16MA->SizeInPixels().iHeight);
    	}
    
	pt.iX = aRect.iTl.iX + 2;
	pt.iY = pt.iY + 18;
	
    if(aBitmap64K)
    	{
		TSize size = aBitmap64K->SizeInPixels();
		TRect srcRect(TPoint(0,0),size);
		size += TSize(5,5);
		TRect destRect(pt - TPoint(0, 8), size);
    	aGc.DrawBitmap(destRect, aBitmap64K, srcRect);
    	pt = pt + TPoint( size.iWidth, 0) + TPoint(2, 0);

#ifdef __WINS__
    	destRect.Move(TPoint(aBitmap64K->SizeInPixels().iWidth + 8, 0));
    	((CWindowGc&) aGc).DrawBitmapMasked(destRect, 
    		aBitmap64K, srcRect,
	    	iTest.iBitmapGray256Mask, EFalse);
    	pt = pt + TPoint( aBitmap64K->SizeInPixels().iWidth, 0) + TPoint(2, 0);
#endif
	   	}
    
    aGc.DrawText(iTest.iText, pt);
    
#ifdef __WINS__
    ((CWindowGc&) aGc).DrawTextVertical(iTest.iText, pt, ETrue);
#endif
	}
	
/** calculate table's size and size of graphics primitieves */
void CTWinAlphaForeground::PrepareForDrawingL()
	{
	iCol = iTest.iFirstCellWidth;

	CalculateTableMargin();

	//create triangle
	iPolygon = new (ELeave) CArrayFixFlat<TPoint> (3);
	TRect rect = TRect(iTest.iTriangleSize);
	iPolygon->AppendL(rect.iTl);
	iPolygon->AppendL(TPoint(rect.iTl.iX, rect.iBr.iY));
	iPolygon->AppendL(rect.iBr);
	}

/** Bitmap is intended to use on target only*/	
void CTWinAlphaForeground::CreateBackgroundBitmapL(const TDisplayMode& aDispMode)
	{
	ASSERT(!iBitmapBackground);
	
	TSize size = Size(); 
		
	iBitmapBackground = new (ELeave) CFbsBitmap ;
	iBitmapBackground->Create(size,aDispMode);
	
	iBitmapDeviceBackground = CFbsBitmapDevice::NewL(iBitmapBackground);
	CGraphicsContext *&theGc = (CGraphicsContext*&)iBitmapContextBackground;
	iBitmapDeviceBackground->CreateContext(theGc);
	}
	
void CTWinAlphaForeground::CleanBackgroundBitmap()
	{
	ASSERT(iBitmapBackground);
	
	TRect rect = TRect(iBitmapBackground->SizeInPixels());
	iBitmapContextBackground->SetBrushColor(iBackgroundColor);
   	iBitmapContextBackground->SetPenColor(iBackgroundColor);
   	iBitmapContextBackground->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmapContextBackground->DrawRect(rect);
	}

void CTWinAlphaForeground::StartAnimationL()
	{
	__UHEAP_MARK;
	RWsSprite theSprite;
	
	RDrawableWindow *theWin = DrawableWin();

	theSprite=RWsSprite(TheClient->iWs);
	TPoint theSpritePos = TPoint(10, 10); 
	TInt theFlags = 0;
	theSprite.Construct(*theWin, theSpritePos, theFlags);
	TDisplayMode dispMode = EColor256;
	//create a ball bitmap for animation
	CFbsBitmap* theBitmapBall= new (ELeave) CFbsBitmap;
	theBitmapBall->Create(TSize(32, 32), dispMode);
	CFbsBitmapDevice* theBitmapDeviceBall = CFbsBitmapDevice::NewL(theBitmapBall);
	CGraphicsContext* theBitmapContextBall = NULL;
	theBitmapDeviceBall->CreateContext(theBitmapContextBall);
	//draw a background
	theBitmapContextBall->SetBrushColor(TRgb(128, 0, 255));
   	theBitmapContextBall->SetPenColor(TRgb(128, 0, 255));
   	theBitmapContextBall->SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContextBall->DrawRect(theBitmapBall->SizeInPixels());
	
	//create a ball's mask
	CFbsBitmap* theBitmapMask= new (ELeave) CFbsBitmap;
	theBitmapMask->Create(TSize(32, 32), dispMode);
	CFbsBitmapDevice* theBitmapDeviceMask = CFbsBitmapDevice::NewL(theBitmapMask);
	CGraphicsContext* theBitmapContextMask = NULL;
	theBitmapDeviceMask->CreateContext(theBitmapContextMask);
	//draw a mask
	theBitmapContextMask->SetBrushColor(TRgb(0, 0, 0));
   	theBitmapContextMask->SetPenColor(TRgb(0, 0, 0));
   	theBitmapContextMask->SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContextMask->DrawRect(theBitmapMask->SizeInPixels());

	theBitmapContextMask->SetBrushColor(TRgb(255, 255, 255));
   	theBitmapContextMask->SetPenColor(TRgb(255, 255, 255));
   	TRect rect = TRect(theBitmapMask->SizeInPixels());
	theBitmapContextMask->SetPenSize(TSize(6,6));
	theBitmapContextMask->DrawLine(rect.iTl, rect.iBr);
	theBitmapContextMask->DrawLine(TPoint(0, rect.iBr.iY), TPoint(rect.iBr.iX, 0));

	//create a second ball's mask
	CFbsBitmap* theBitmapMask1= new (ELeave) CFbsBitmap;
	theBitmapMask1->Create(TSize(32, 32), dispMode);
	CFbsBitmapDevice* theBitmapDeviceMask1 = CFbsBitmapDevice::NewL(theBitmapMask1);
	CGraphicsContext* theBitmapContextMask1 = NULL;
	theBitmapDeviceMask1->CreateContext(theBitmapContextMask1);
	//draw a mask
	theBitmapContextMask1->SetBrushColor(TRgb(0, 0, 0));
   	theBitmapContextMask1->SetPenColor(TRgb(0, 0, 0));
   	theBitmapContextMask1->SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContextMask1->DrawRect(theBitmapMask1->SizeInPixels());

	theBitmapContextMask1->SetBrushColor(TRgb(255, 255, 255));
   	theBitmapContextMask1->SetPenColor(TRgb(255, 255, 255));
   	rect = TRect(theBitmapMask1->SizeInPixels());
	theBitmapContextMask1->SetPenSize(TSize(6,6));
	theBitmapContextMask1->DrawLine(TPoint(rect.iBr.iX/2, 0), TPoint(rect.iBr.iX/2, rect.iBr.iY));
	theBitmapContextMask1->DrawLine(TPoint(0, rect.iBr.iY/2), TPoint(rect.iBr.iX, rect.iBr.iY/2));


	TSpriteMember theSpriteList;
	theSpriteList.iBitmap = theBitmapBall;
	theSpriteList.iMaskBitmap = theBitmapMask;
	theSpriteList.iInvertMask = EFalse;
	theSpriteList.iDrawMode = CGraphicsContext::EDrawModePEN;
	theSpriteList.iOffset = TPoint(0, 0);
	theSpriteList.iInterval = TTimeIntervalMicroSeconds32(100000);
	theSprite.AppendMember(theSpriteList);

	TSpriteMember theSpriteList1;
	theSpriteList1.iBitmap = theBitmapBall;
	theSpriteList1.iMaskBitmap = theBitmapMask1;
	theSpriteList1.iInvertMask = EFalse;
	theSpriteList1.iDrawMode = CGraphicsContext::EDrawModePEN;
	theSpriteList1.iOffset = TPoint(0, 0);
	theSpriteList1.iInterval = TTimeIntervalMicroSeconds32(100000);
	
	theSprite.AppendMember(theSpriteList1);
	
	theSprite.Activate();
	
	for(TInt ii = 0; ii < 20; ii++)
		{
		theSpritePos += TPoint(3, 8);
		theSprite.SetPosition(theSpritePos);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(50000));		
		}

	for(TInt jj = 0; jj < 20; jj++)
		{
		theSpritePos -= TPoint(0, 8);
		theSprite.SetPosition(theSpritePos);
		TheClient->iWs.Flush();	
		User::After(TTimeIntervalMicroSeconds32(50000));
		}
	
	theSprite.Close();
	
	delete theBitmapContextBall;
	delete theBitmapDeviceBall;
	
	delete theBitmapBall;
	
	delete theBitmapMask;
	delete theBitmapContextMask;
	delete theBitmapDeviceMask;
	
	delete theBitmapMask1;
	delete theBitmapContextMask1;
	delete theBitmapDeviceMask1;

	__UHEAP_MARKEND;
	}

void CTWinAlphaForeground::CreateBackedWindowL()
	{
	RBackedUpWindow theBackedWindow(TheClient->iWs); 
	CleanupClosePushL(theBackedWindow);
	
	TDisplayMode theDisplayMode = EColor16MA;
	
	RWindow* theWin = (RWindow*) DrawableWin();

	theBackedWindow.Construct(*theWin,theDisplayMode, ENullWsHandle);
	
	TPoint pos =TPoint(10, 10);
	TSize size = theWin->Size();
	size.SetSize(size.iWidth / 5, size.iHeight/10);
	
	theBackedWindow.SetExtentErr(pos, size);
	theBackedWindow.SetOrdinalPosition(0);

	
	//draw to backed window	
	TRgb color = TRgb(255, 0, 128);
	TInt bitmapHandle = theBackedWindow.BitmapHandle();
	CFbsBitmap bitmapWin;
	bitmapWin.Duplicate(bitmapHandle);
	//Leave poss here - theBackedWindow could leak
	CFbsBitmapDevice* theBitmapDevice = CFbsBitmapDevice::NewL(&bitmapWin);
	CGraphicsContext* theBitmapContext = NULL;
	theBitmapDevice->CreateContext(theBitmapContext);
	theBitmapContext->SetBrushColor(color);
   	theBitmapContext->SetPenColor(color);
   	theBitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContext->DrawRect(TRect(bitmapWin.SizeInPixels()));

	color = TRgb(0, 0, 0);
   	theBitmapContext->SetPenColor(color);
	for(TInt kk = 0; kk < bitmapWin.SizeInPixels().iWidth; kk += 8)
		{
		theBitmapContext->DrawLine(TPoint(kk, 0), TPoint(kk, bitmapWin.SizeInPixels().iHeight));
		}
	
	
	theBackedWindow.Activate();
	TheClient->iWs.Flush();
	User::After(TTimeIntervalMicroSeconds32(500000));
	
	//hide the window
	theBackedWindow.SetVisible(EFalse);
	TheClient->iWs.Flush();
	User::After(TTimeIntervalMicroSeconds32(500000));
	theBackedWindow.SetVisible(ETrue);
	TheClient->iWs.Flush();
	
	for(TInt ii = 0; ii < 7; ii++)
		{
		User::After(TTimeIntervalMicroSeconds32(100000));
		pos.iX += 5;
		pos.iY += 15;
		 
		theBackedWindow.SetExtentErr(pos, size);
		TheClient->iWs.Flush();
		}

	User::After(TTimeIntervalMicroSeconds32(500000));
	
	//transparent color. don't expect it to work
	pos.iX -= 5;
	pos.iY -= 15;

	theBackedWindow.SetExtentErr(pos, size);
	color = TRgb(255, 255, 128, 128);
	theBitmapContext->SetBrushColor(color);
   	theBitmapContext->SetPenColor(color);
	theBitmapContext->DrawRect(TRect(bitmapWin.SizeInPixels()));
	TheClient->iWs.Flush();
	User::After(TTimeIntervalMicroSeconds32(500000));

	//semi-transparent color
	pos.iX -= 5;
	pos.iY -= 15;
	theBackedWindow.SetExtentErr(pos, size);
	color = TRgb(255, 255, 128, 255);
	theBitmapContext->SetBrushColor(color);
   	theBitmapContext->SetPenColor(color);
	TheClient->iWs.Flush();
	User::After(TTimeIntervalMicroSeconds32(500000));

	
	CleanupStack::PopAndDestroy();	 //bitmapWin
	delete theBitmapContext;
	delete theBitmapDevice;
	
	}

	
//-------------------
void CTWinAlpha::Draw()
	{
	CBitmapContext* theGc = TheClient->iGc;
	TSize size = Size();
	
	for(TInt ii = 0; ii < size.iHeight; ii += (20+iState))
		{
		theGc->DrawLine(TPoint(0, ii), TPoint(size.iWidth, ii));
		}
	}
	
__WS_CONSTRUCT_STEP__(AlphaChannel)
