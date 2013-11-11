// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent
*/

#include <e32debug.h>
#include <w32std.h>
#include <iniparser.h>

#include "t_pseudoapppopupwindow.h"
#include "t_pseudoapputils.h"
#include "t_winutils.h"			//CTestBitmap

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("T_PseudoAppPopupWindow::E32Main - entry"));

//	__UHEAP_MARK; // mark heap state

	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, DisplayPopupWindowL());
	if (err)
		{
		User::Panic(KTPopupWindowAppPanic,err);
		}

    delete TheTrapCleanup;

//	__UHEAP_MARKEND; // check no memory leak

	User::After(20000000);
	RDebug::Print(_L("T_PseudoAppPopupWindow::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void DisplayPopupWindowL()
	{
	TPtrC multiBitmapPopupFile;
	TSize windowSize;
	TPoint windowPosition;
	TInt transparency;
	TInt screenNo;
	TDisplayMode aMode;

	//Create CIniData class for reading in values form ini files
	CIniData* utils = CIniData::NewL(KWServPseudoAppConfigFile);
	CleanupStack::PushL(utils);

	ReadPopupConfig(windowSize, windowPosition, multiBitmapPopupFile, transparency, screenNo, aMode, utils);

	RDebug::Print(_L("Create Window server session"));
	RWsSession session;
	User::LeaveIfError(session.Connect());

	RDebug::Print(_L("Create CWsScreenDevice\n"));
	CWsScreenDevice screenDevice(session);
	User::LeaveIfError(screenDevice.Construct(screenNo));

	RDebug::Print(_L("Create RWindowGroup\n"));
	RWindowGroup group(session);
	User::LeaveIfError(group.Construct(8970+screenNo,ETrue));

	RDebug::Print(_L("Create Window\n"));
	RWindow window(session);
	User::LeaveIfError(window.Construct((RWindowTreeNode)group,(TUint32)&window));

	DeterminePixelValues(windowSize, windowPosition, screenDevice);

	window.SetExtent(windowPosition, windowSize);

	TInt ret = window.SetRequiredDisplayMode(aMode);

	RDebug::Print(_L("Window display mode set to %d\n"), ret);

	RDebug::Print(_L("Create CWindowGc\n"));
	CWindowGc windowGc(&screenDevice);
	User::LeaveIfError(windowGc.Construct());

	//Make the window transparent
	CTestBitmap* bitmap = CTestBitmap::NewL(windowSize, EGray256);
	CleanupStack::PushL(bitmap);

	bitmap->Gc().SetBrushColor(TRgb::Gray256(128));
	bitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	bitmap->Gc().SetPenStyle(CGraphicsContext::ENullPen);
	bitmap->Gc().DrawRect(windowSize);

	window.SetTransparencyBitmap(bitmap->Bitmap());
	window.SetVisible(ETrue);
	window.Activate();

	windowGc.Activate(window);
	window.Invalidate();
	window.BeginRedraw();

	//Load popup bitmap
	CTestBitmap* bitmapSource = CTestBitmap::NewL(windowSize, aMode);
	CleanupStack::PushL(bitmapSource);

	User::LeaveIfError(bitmapSource->Bitmap().Load(multiBitmapPopupFile, 0));

	if(transparency)
		{
		CTestBitmap* bitmapSourceAlpha;
		CTestBitmap* bitmapDest;
		CTestBitmap* bitmapDestAlpha;

		bitmapSourceAlpha = CTestBitmap::NewL(windowSize, EGray256);
		CleanupStack::PushL(bitmapSourceAlpha);

		User::LeaveIfError(bitmapSourceAlpha->Bitmap().Load(multiBitmapPopupFile, 1));

		bitmapDest = CTestBitmap::NewL(windowSize, aMode);
		CleanupStack::PushL(bitmapDest);

		bitmapDest->Bitmap().SetSizeInTwips(bitmapSource->Bitmap().SizeInTwips());
		bitmapDest->Gc().DrawBitmap(TRect(TPoint(0,0), windowSize), &(bitmapSource->Bitmap()));

		bitmapDestAlpha = CTestBitmap::NewL(windowSize, EGray256);
		CleanupStack::PushL(bitmapDestAlpha);

		bitmapDestAlpha->Bitmap().SetSizeInTwips(bitmapSourceAlpha->Bitmap().SizeInTwips());
		bitmapDestAlpha->Gc().DrawBitmap(TRect(TPoint(0,0), windowSize), &(bitmapSourceAlpha->Bitmap()));

		TInt ret = windowGc.AlphaBlendBitmaps(TPoint(0,0), &(bitmapDest->Bitmap()), TRect(TPoint(0,0), windowSize), &(bitmapDestAlpha->Bitmap()), TPoint(0,0));

		if(ret != KErrNone)
			{
			RDebug::Print(_L("Transparent popup window drawing error\n"));
			}
		else
			{
			RDebug::Print(_L("Transparent popup window drawn correctly\n"));
			}

		CleanupStack::PopAndDestroy(3); //bitmapSourceAlpha, bitmapDest, bitmapDestAlpha
		}
	else
		{
		//Make window opaque
		window.SetTransparencyFactor(TRgb(0xFFFFFFFF));
		windowGc.DrawBitmap(TRect(TPoint(0,0), windowSize), &(bitmapSource->Bitmap()));
		}

	window.EndRedraw();
	session.Flush();
	windowGc.Deactivate();
	User::After(100000);

	CleanupStack::PopAndDestroy(3);	//utils, bitmap, bitmapSource
	}

LOCAL_C void ReadPopupConfig(TSize& aWindowSize, TPoint& aWindowPosition, TPtrC& aBitmapFile, TInt& aTransparency,
                             TInt& aScreenNo, TDisplayMode& aMode, CIniData* aUtils)
	{
	TInt maxNumberOfScreens;
	TPtrC windowSize;
	TPtrC windowPosition;
	TInt popupOccurances;

	TBuf<KMaxUiBitmapNameLength>	tempStore;
	TInt i;

	READ_INI1(max_number_of_screens, maxNumberOfScreens, aUtils);

	for(i=0; i<maxNumberOfScreens; i++)
		{
		TBuf<KMaxUiBitmapNameLength>	tempStore;

		tempStore.Format(KScreenWindowPopup, i);
		READ_INI1A(tempStore, occurances, popupOccurances, aUtils);

		tempStore.Format(KScreenWindowPopup, i);

		if(popupOccurances)
			{
			RDebug::Print(_L("Popup window enabled for screen %d"), i);
			aScreenNo = i;
			TInt screenMode;

			READ_INI1A(tempStore, multi_bitmap_popup_file, aBitmapFile, aUtils);

			READ_INI1A(tempStore, window_size, windowSize, aUtils);
			TImportScreenConfig::ExtractSize(windowSize, aWindowSize);

			READ_INI1A(tempStore, window_position, windowPosition, aUtils);
			TImportScreenConfig::ExtractPoint(windowPosition, aWindowPosition);

			READ_INI2A(tempStore, transparency,   True, False, aTransparency,  ETrue, EFalse, aUtils);
			READ_INI2(screen_mode, EColor16MA, EColor64K, screenMode, EColor16MA, EColor64K, aUtils);

			aMode = static_cast<TDisplayMode>(screenMode);

			//The popup is enabled for one screen per instance of the exe
			return;
			}
		else
			{
			RDebug::Print(_L("No popup window enabled for screen %d"), i);
			}
		}
	}


LOCAL_C void DeterminePixelValues(TSize& aWindowSize, TPoint& aWindowPosition, 	CWsScreenDevice& aScreenDevice)
	{
	TSize screenSize = aScreenDevice.SizeInPixels();

	//Convert surface size percentages to pixel sizes
	if(aWindowSize.iWidth <= 0)
		{
		RDebug::Print(_L("popup_width is zero or less, quitting\n"));
		User::Exit(0);
		}
	else
		{
		aWindowSize.iWidth = (aWindowSize.iWidth * screenSize.iWidth)/100;
		}

	if(aWindowSize.iHeight <= 0)
		{
		RDebug::Print(_L("popup_height is zero or less, quitting\n"));
		User::Exit(0);
		}
	else
		{
		aWindowSize.iHeight = (aWindowSize.iHeight * screenSize.iHeight)/100;
		}

	//Convert surface position percentages to pixel sizes
	aWindowPosition.iX = (aWindowPosition.iX * screenSize.iWidth)/100;
	aWindowPosition.iY = (aWindowPosition.iY * screenSize.iHeight)/100;
	}
