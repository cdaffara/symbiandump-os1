// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <hal.h>
#include <u32hal.h>
#include <limits.h>
#include <e32const.h>
#include <graphics/surfaceupdateclient.h>
#include <graphics/suerror.h>
#include <graphics/wstestscreencapture.h>
#include <graphics/testscreencapture.h>
#include "surfaceupdatetest.h"
#include "tsurfaceupdateinteg.h"
#include "twindows.h"
#include "tredrawhandler.h"
#include "tsurfacehelper.h"

const TSize windowSize(160, 100);
const TSize windowHalfSize(windowSize.iWidth / 2, 100);
const TPoint windowTopLeft(10,10);
const TPoint windowTopMiddle(windowTopLeft.iX + windowSize.iWidth / 2, windowTopLeft.iY);


CTSurfaceUpdateInteg::CTSurfaceUpdateInteg(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

CTSurfaceUpdateInteg::~CTSurfaceUpdateInteg()
	{
	iWindows.ResetAndDestroy();
	iGroups.ResetAndDestroy();

	iWsDevs.ResetAndDestroy();
	iGcs.ResetAndDestroy();
	
	delete iRedrawHandler;
	iSurfaceUpdate.Close();
	delete iSurfaceHelper;
	iBitmaps.ResetAndDestroy();

	delete iGraphicsContext;
	delete iBitmapDevice;
	delete iRefBitmap;
	
	iWs.Close();
	}

/**
 * Create a screen device, drawing context and a window for each screen.
 * Create surfaces, open connection to the SUS.
 */
void CTSurfaceUpdateInteg::ConstructL()
	{
	INFO_PRINTF1(_L("Test case construction"));
	User::LeaveIfError(iWs.Connect());
	iNumOfScreens = iWs.NumberOfScreens();
	iSurfaceHelper = CSurfaceHelper::NewL();
	const TInt stride = windowSize.iWidth * 4;
	const TInt stride1 = windowHalfSize.iWidth * 4;
	iSurface = iSurfaceHelper->CreateSurfaceL(windowSize, EUidPixelFormatXRGB_8888, stride,1);
	iSurfaceDoubleBuffered = iSurfaceHelper->CreateSurfaceL(windowSize, EUidPixelFormatXRGB_8888, stride,2);
	iSurfaceAbove = iSurfaceHelper->CreateSurfaceL(windowHalfSize, EUidPixelFormatXRGB_8888, stride1, 1);
	iSurfaceAbove1 = iSurfaceHelper->CreateSurfaceL(windowSize, EUidPixelFormatXRGB_8888, stride, 1);
	INFO_PRINTF1(_L("The surfaces have been created"));

	iRefBitmap = new (ELeave)CFbsBitmap();
	User::LeaveIfError(iRefBitmap->Create(windowSize,EColor16MU));
	iBitmapDevice = CFbsBitmapDevice::NewL(iRefBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext(iGraphicsContext));

	//fill the whole surfaces with different color
	iSurfaceHelper->FillRectangleL(iSurfaceAbove, TPoint(), windowHalfSize, KRgbYellow);
	iSurfaceHelper->FillRectangleL(iSurfaceAbove1, TPoint(), windowSize, KRgbYellow);
	iSurfaceHelper->FillRectangleL(iSurface, TPoint(), windowSize, KRgbGreen);
	iSurfaceHelper->FillRectangleL(iSurfaceDoubleBuffered, TPoint(), windowSize, KRgbCyan);
	FillRefBitmap(KRgbGreen);
	INFO_PRINTF1(_L("The surfaces and a reference bitmap have been filled with color"));
	
	User::LeaveIfError(iSurfaceUpdate.Connect());
	
	iRedrawHandler = new (ELeave) CTRedrawHandler(iWs); //will deal with all redrawing command
	iRedrawHandler->Start();

	//create windows and assign surfaces to them
	for(TInt ii = 0; ii < 2; ii++)
		{
		CWsScreenDevice* wsDev = new (ELeave) CWsScreenDevice(iWs);
		iWsDevs.AppendL(wsDev);
		User::LeaveIfError(wsDev->Construct(ii));
		CWindowGc* gc = NULL;
		User::LeaveIfError(wsDev->CreateContext(gc));
		iGcs.AppendL(gc);

		CTWindowGroup* defaultGroup = CreateGroupL(wsDev);
		CTWindow* windowSingle = CTWindow::NewL(iWs, *defaultGroup, *gc);
		RegisterWindowL(windowSingle);
		windowSingle->Window()->SetExtent(windowTopLeft, windowSize);
		windowSingle->Window()->SetBackgroundSurface(iSurface);
		windowSingle->Window()->Activate();
		
		CTWindow* windowDouble = CTWindow::NewL(iWs, *defaultGroup, *gc);
		RegisterWindowL(windowDouble);
		windowDouble->Window()->SetExtent(windowTopLeft, windowSize);
		windowDouble->Window()->SetBackgroundSurface(iSurfaceDoubleBuffered);
		windowDouble->Window()->SetVisible(EFalse);
		windowDouble->Window()->Activate();

		CTWindow* windowAbove = CTWindow::NewL(iWs, *iGroups[ii], *iGcs[ii]);
		RegisterWindowL(windowAbove);
		windowAbove->Window()->SetExtent(windowTopLeft, windowHalfSize);
		windowAbove->Window()->SetBackgroundSurface(iSurfaceAbove);
		windowAbove->Window()->SetVisible(EFalse);
		windowAbove->Window()->Activate();
		if(ii == 0)
			{
			iWindowAbove11 = CTWindow::NewL(iWs, *iGroups[ii], *iGcs[ii]);
			RegisterWindowL(iWindowAbove11);
			iWindowAbove11->Window()->SetExtent(windowTopLeft, windowSize);
			iWindowAbove11->Window()->SetBackgroundSurface(iSurfaceAbove1);
			iWindowAbove11->Window()->SetVisible(EFalse);
			iWindowAbove11->Window()->Activate();

			iWindowAbove1 = windowAbove;
			iWindowSingleBuffured1 = windowSingle;
			iWindowDoubleBuffured1 = windowDouble;
			}
		else
			{
			iWindowAbove2 = windowAbove;
			iWindowSingleBuffured2 = windowSingle;
			iWindowDoubleBuffured2 = windowDouble;
			}

	    TPixelsTwipsAndRotation sizeAndRotation;
	    iWsDevs[ii]->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	    CFbsBitmap* bitmap = new (ELeave)CFbsBitmap();
	    iBitmaps.AppendL(bitmap);
	    User::LeaveIfError(bitmap->Create(sizeAndRotation.iPixelSize, EColor16MU));
        INFO_PRINTF4(_L("Bitmap with size: %d, %d was created to match the size of the screen %d"), sizeAndRotation.iPixelSize.iWidth, sizeAndRotation.iPixelSize.iHeight, ii);
		}
	INFO_PRINTF1(_L("The windows have been created"));
	
	iWs.Finish();
	}

/**
 * Create the window group for the given screen device and put it into the list.
 */
CTWindowGroup* CTSurfaceUpdateInteg::CreateGroupL(CWsScreenDevice* aScreenDevice)
	{
	CTWindowGroup* group = CTWindowGroup::NewL(iWs, aScreenDevice);
	CleanupStack::PushL(group);
	iGroups.AppendL(group);
	CleanupStack::Pop();
	return group;
	}

/**
 * Append a window into the list for retrawing it and destroying in the future.
 */
void CTSurfaceUpdateInteg::RegisterWindowL(CTWindowTreeNode* aWindow)
	{
	CleanupStack::PushL(aWindow);
	iWindows.AppendL(aWindow);
	CleanupStack::Pop(aWindow);
	}

/**
 * Go through all screens and compare the whole surface area with the reference bitmap.
 * 
 * @return ETrue, if the surface is exactly the same as the reference bitmap or EFalse otherwise. 
 */
TBool CTSurfaceUpdateInteg::CompareAllScreens()
	{
	TBool res = ETrue;
	TRect rc(windowTopLeft, windowSize);
	for(TInt ii = 0; (ii < iNumOfScreens) && res; ii++)
		{
		res = Compare(ii, rc, iRefBitmap);
		}
	return res;
	}

/**
 * Compare part of the screen with the reference bitmap.
 * 
 * @param aScreenNumber Screen number.
 * @param aRectSrc The area of the screen to be compared.
 * @param aRefBitmap Bitmap for comparison.
 * @return ETrue, if given part of the screen is exactly the same as the reference bitmap or EFalse otherwise. 
 */
TBool CTSurfaceUpdateInteg::Compare(TInt aScreenNumber, const TRect& aRectSrc, CFbsBitmap* aRefBitmap)
	{
	MTestScreenCapture* csc = static_cast<MTestScreenCapture*> (iWsDevs[aScreenNumber]->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
	if(!csc) //shouldn't happen, as we have checked this before
		{
		return EFalse;
		}
	
	TBool res = ETrue;
	TInt ret = csc->ComposeScreen(*(iBitmaps[aScreenNumber]));
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("Composition screen failure, err = %d"), ret);
		res = EFalse;
		}
	//Go through all pixels in screen bitmap for specified area and check that they are 
	//identical to corresponding pixels in reference bitmap
	for(TInt ii = 0; (ii < aRectSrc.Height()) && res; ii++)
		{
		for(TInt jj = 0; (jj < aRectSrc.Width()) && res; jj++)
			{
			TRgb refCol;
			aRefBitmap->GetPixel(refCol, TPoint(jj, ii));
			TRgb col;
			(iBitmaps[aScreenNumber])->GetPixel(col, TPoint(jj, ii) + aRectSrc.iTl);
			if(refCol != col)
				{
				res = EFalse;
				}
			}
		}
	//useful for debugging
#ifdef TEST_COMPARE_PARTLY_UPDATED_SCREEN 	
	if(!res)
		{
		iBitmap->Save(_L("c:\\bitmap.mbm"));
		aRefBitmap->Save(_L("c:\\ref.mbm"));
		}
#endif
	
	return res;
	}

/**
 * Fill reference bitmap with color specified.
 * 
 */
void CTSurfaceUpdateInteg::FillRefBitmap(TRgb aColor)
	{
	TRect rect = TRect(iRefBitmap->SizeInPixels());
	iGraphicsContext->SetBrushColor(aColor);
	iGraphicsContext->SetPenColor(aColor);
	iGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGraphicsContext->DrawRect(rect);
	}

/**
  @SYMTestCaseID  GRAPHIC-SURFACEUPDATE-0026
  	
  @SYMCR CR1650

  @SYMREQ REQ11596

  @SYMTestCaseDesc 
  	SubmitUpdate() to cope with Global Updates with surface on various 
  	screens when the dirty region to update may not be visible on all screens

  @SYMTestPriority High

  @SYMTestStatus Implemented
	
  @SYMTestActions The test consists of a single surface, visible on 
  screens A (master), B.
	1.	Submit a global update for the surface, passing a null dirty region.
	2.	Submit a global update for the surface, passing a dirty region 
	that is only visible on screens A  (that particular region for the 
	surface is not visible on B).
	3.	Submit a global update for the surface, passing a dirty region 
	that is only visible on screens B (that particular region for the 
	surface is not visible on A).
	4.	Submit a global update for the surface, passing a dirty 
	region that is not visible on any of the screens.
  
  @SYMTestExpectedResults 
  	1.	TRequestStatus signals KErrNone in all cases. Timestamps must 
  	be valid. The surface gets updated on all screens.
	2.	TRequestStatus signals KErrNone in all cases. Timestamps must 
	be valid. The surface gets updated only on screens A. Screen B is unchanged.
	3.	TRequestStatus signals KErrNone in all cases. Timestamps must 
	be valid. The surface gets updated only on screens B. 
	Screen A is unchanged.
	4.	SubmitUpdate() returns KErrNone.
*/
void CTSurfaceUpdateInteg::TestCase1L()
	{
	INFO_PRINTF1(_L("Global Submit Update with/without update region supplied"));
	
	const TInt bufferNumber = 0; 
	TRequestStatus status = KRequestPending;
	TRequestStatus status1 = KRequestPending;
	TRequestStatus status2 = KRequestPending;
	TUint64 timestampComposition = 0;
	TTimeStamp timeStamp; 
	
	//Fill the surface to different colour, submit update for the whole surface 
	//and check that screen is updated correctly
	INFO_PRINTF1(_L("Fill the surface with the blue color"));
	iSurfaceHelper->FillRectangleL(iSurface, TPoint(), windowSize, KRgbBlue);
	FillRefBitmap(KRgbBlue);
	
	INFO_PRINTF1(_L("Updating the whole area of the surface"));
	TUint64 timestampBefore = User::FastCounter();
	iSurfaceUpdate.NotifyWhenAvailable(status);	
	iSurfaceUpdate.NotifyWhenDisplayed(status1, timeStamp);	
	iSurfaceUpdate.NotifyWhenDisplayedXTimes(10, status2);
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, bufferNumber);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking

	TUint64 timestampAfter = User::FastCounter();
	if(timestampAfter < timestampBefore)
		{
		timestampAfter += UINT_MAX;
		}
	timestampComposition = timeStamp();
	if(timestampComposition < timestampBefore)
		{
		timestampComposition += UINT_MAX;
		}
	
	TEST(status == KErrNone);
	TEST(status1 == KErrNone);
	TEST(status2 == KErrNone);
	TEST(timestampComposition != 0);
	TEST(timestampAfter >= timestampComposition);
	TEST(timestampBefore <= timestampComposition);
	TEST(CompareAllScreens());

	//Fill the surface to different colour; hide the part of the surface on 
	//the screen which is not a master, submit update for the area of the surface 
	//which is only visible on the master. Check that only master screen gets updated.
	INFO_PRINTF1(_L("Hide the part of the surface on the second screen"));
	iWindowAbove1->Window()->SetVisible(EFalse);
	iWindowAbove2->Window()->SetVisible(ETrue);
	iWs.Finish();
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, bufferNumber);
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking
	
	INFO_PRINTF1(_L("Fill the surface with the green color"));
	iSurfaceHelper->FillRectangleL(iSurface, TPoint(), windowSize, KRgbGreen);
	
	TRect rc(windowHalfSize);
	rc.Shrink(10, 10);
	RRegion region(1, &rc); //this area is hidden if the window above is visible
	timestampBefore = User::FastCounter();
	iSurfaceUpdate.NotifyWhenAvailable(status);	
	iSurfaceUpdate.NotifyWhenDisplayed(status1, timeStamp);	
	iSurfaceUpdate.NotifyWhenDisplayedXTimes(10, status2);
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, bufferNumber, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::After(TTimeIntervalMicroSeconds32(1000000)); //useful for visual checking

	timestampAfter = User::FastCounter();
	if(timestampAfter < timestampBefore)
		{
		timestampAfter += UINT_MAX;
		}
	timestampComposition = timeStamp();
	if(timestampComposition < timestampBefore)
		{
		timestampComposition += UINT_MAX;
		}
	
	TEST(status == KErrNone);
	TEST(status1 == KErrNone);
	TEST(status2 == KErrNone);
	TEST(timestampComposition != 0);
	TEST(timestampAfter >= timestampComposition);
	TEST(timestampBefore <= timestampComposition);
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking

	//first screen should be updated, as the surface is visible here
	TRect rectSrc = rc;
	rectSrc.Move(windowTopLeft);
	FillRefBitmap(KRgbGreen);
	TEST(Compare(0, rectSrc, iRefBitmap));
	//check that second screen doesn't update visible part of the surface 
	FillRefBitmap(KRgbYellow);
	TEST(Compare(1, rectSrc, iRefBitmap));

	//Fill the surface to different colour; hide the part of the surface 
	//on the master screen, submit update for the area of the surface 
	//which is only visible on non-master. Check that only non-master screen gets updated.
	INFO_PRINTF1(_L("Hide the part of the surface on the first screen"));
	iWindowAbove1->Window()->SetVisible(ETrue);
	iWindowAbove2->Window()->SetVisible(EFalse);
	iWs.Finish();
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking
	
	INFO_PRINTF1(_L("Fill the surface with the blue color"));
	iSurfaceHelper->FillRectangleL(iSurface, TPoint(), windowSize, KRgbBlue);
	
	timestampBefore = User::FastCounter();
	iSurfaceUpdate.NotifyWhenAvailable(status);	
	iSurfaceUpdate.NotifyWhenDisplayed(status1, timeStamp);	
	iSurfaceUpdate.NotifyWhenDisplayedXTimes(10, status2);
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, bufferNumber, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::After(TTimeIntervalMicroSeconds32(1000000)); //useful for visual checking

	timestampAfter = User::FastCounter();
	if(timestampAfter < timestampBefore)
		{
		timestampAfter += UINT_MAX;
		}
	timestampComposition = timeStamp();
	if(timestampComposition < timestampBefore)
		{
		timestampComposition += UINT_MAX;
		}
	
	TEST(status == KErrNone);
	TEST(status1 == KErrNone);
	TEST(status2 == KErrNone);
	TEST(timestampComposition != 0);
	TEST(timestampAfter >= timestampComposition);
	TEST(timestampBefore <= timestampComposition);

	//check that first screen doesn't update visible part of the surface 
	FillRefBitmap(KRgbYellow);
	TEST(Compare(0, rectSrc, iRefBitmap));
	//second screen should be updated, as the surface is visible here
	FillRefBitmap(KRgbBlue);
	TEST(Compare(1, rectSrc, iRefBitmap));
	
	//Fill the surface to different colour; hide the part of the surface on the master screen, 
	//submit update for the area of the surface which is not visible on any screen. 
	//Check that all screens have not been updated.
	INFO_PRINTF1(_L("Hide partly the surface on two screens"));
	iWindowAbove1->Window()->SetVisible(ETrue);
	iWindowAbove2->Window()->SetVisible(ETrue);
	iWs.Finish();
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, bufferNumber);
	User::After(TTimeIntervalMicroSeconds32(1000000)); //useful for visual checking
	
	INFO_PRINTF1(_L("Fill the surface with the green color"));
	iSurfaceHelper->FillRectangleL(iSurface, TPoint(), windowSize, KRgbGreen);
	FillRefBitmap(KRgbGreen);
	
	timestampBefore = User::FastCounter();
	iSurfaceUpdate.NotifyWhenAvailable(status);	
	iSurfaceUpdate.NotifyWhenDisplayed(status1, timeStamp);	
	iSurfaceUpdate.NotifyWhenDisplayedXTimes(10, status2);
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, bufferNumber, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::After(TTimeIntervalMicroSeconds32(1000000)); //useful for visual checking

	timestampAfter = User::FastCounter();
	if(timestampAfter < timestampBefore)
		{
		timestampAfter += UINT_MAX;
		}
	timestampComposition = timeStamp();
	if(timestampComposition < timestampBefore)
		{
		timestampComposition += UINT_MAX;
		}
	
	TEST(status == KErrNone);
	TEST(status1 == KErrNone);
	TEST(status2 == KErrNone);
	TEST(timestampComposition != 0);
	TEST(timestampAfter >= timestampComposition);
	TEST(timestampBefore <= timestampComposition);
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking
	
	//both first and second screen shouldn't be updated, as the surface is not visible here
	FillRefBitmap(KRgbYellow);
	TEST(Compare(0, rectSrc, iRefBitmap));
	TEST(Compare(1, rectSrc, iRefBitmap));

	INFO_PRINTF1(_L("Return to the initial state"));
	iWindowAbove1->Window()->SetVisible(EFalse);
	iWindowAbove2->Window()->SetVisible(EFalse);
	iWs.Finish();
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, bufferNumber);
	User::After(TTimeIntervalMicroSeconds32(1000000)); //useful for visual checking
	
	FillRefBitmap(KRgbGreen);
	TEST(CompareAllScreens());
	}

/**
  @SYMTestCaseID GRAPHIC-SURFACEUPDATE-0031

  @SYMCR CR1650

  @SYMREQ REQ11598

  @SYMTestCaseDesc NotifyWhenAvailable() to deal with global updates and be signalled 
 	correctly when screen is unplugged.
 	
  @SYMTestPriority 	High

  @SYMTestStatus Implemented
	
  @SYMTestActions 
 	The test consists of a double-buffered surface, visible on screens A 
 	(master) and B,
	1	Make global submit update with notify when available for the buffer 1.
	2	Unplug screen A (notification is still in a progress )
	3	Make global submit update with notify when available for the 
	buffer 2 to trigger notification.
	4	Make global submit update with notify when available for the 
	buffer 1.
	5	Plug in screen A. (notification is still in a progress )
	6	Make global submit update with notify when available for 
	the buffer 2.
  
  @SYMTestExpectedResults 
  	After step 3, notification is received with KErrNone (this is 
  	triggered by the Receiver for screen B).
	After step 6, notification is received with KErrNone (this is 
	triggered by the Receiver for screen A).
*/
void CTSurfaceUpdateInteg::TestCase2L()
	{
	INFO_PRINTF1(_L("Exercising NotifyWhenAvailable while some screens are disconnected."));

	//hide single-buffered surface and show a double buffered-surface
	iWindowSingleBuffured1->Window()->SetVisible(EFalse);
	iWindowSingleBuffured2->Window()->SetVisible(EFalse);
	iWindowDoubleBuffured1->Window()->SetVisible(ETrue);
	iWindowDoubleBuffured2->Window()->SetVisible(ETrue);
	iWs.Finish();
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, 0);
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurfaceDoubleBuffered, 0);
	FillRefBitmap(KRgbCyan);
	TEST(CompareAllScreens());
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking
	
	INFO_PRINTF1(_L("Fill the surface with the blue color"));
	iSurfaceHelper->FillRectangleL(iSurfaceDoubleBuffered, TPoint(), windowSize, KRgbBlue);
	
	INFO_PRINTF1(_L("Disconnect the screen while notify when available request is still in a progress")); 
	TRequestStatus status;
	TSwitchDisplayAndReleaseBuffer param;
	param.iBuffer = 0;
	param.iScreen = 0;
	param.iSurfaceId=iSurfaceDoubleBuffered;
	CCommandDispatcher commandDispatcher(CCommandDispatcher::EDisconnectDisplayAndReleaseBuffer, &param);
	
	//Command dispatcher will be running in the separate thread. 
	//The requirement for this caused by the fact that for dubble-buffered surfaces 
	//the content update receiver will postpone completion of the request for availability until 
	//client sends SubmitUpdate for another buffer. 
	iSurfaceUpdate.NotifyWhenAvailable(status);	
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurfaceDoubleBuffered, 1);
	TInt res = commandDispatcher.Start();  //kickoff another thread, which SubmitUpdate 
											//for buffer 1 and disconnects the first screen
	TEST(res == KErrNone);
	User::WaitForRequest(status);//At that point we are blocked until command dispatcher sends SubmitUpdate for the buffer number 1
	TEST(status == KErrNone);
	
	//connect the screen
	INFO_PRINTF1(_L("Connect the screen while notify when available request is still in a progress")); 
	CCommandDispatcher commandDispatcher1(CCommandDispatcher::EConnectDisplayAndReleaseBuffer, &param);
	iSurfaceUpdate.NotifyWhenAvailable(status);	
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurfaceDoubleBuffered, 0);
	res = commandDispatcher.Start();//kickoff another thread, which SubmitUpdate 
										//for buffer 1 and connects the first screen

	TEST(res == KErrNone);
	User::WaitForRequest(status);
	TEST(status == KErrNone);

	//restore the original state
	iWindowSingleBuffured1->Window()->SetVisible(ETrue);
	iWindowSingleBuffured2->Window()->SetVisible(ETrue);
	iWindowDoubleBuffured1->Window()->SetVisible(EFalse);
	iWindowDoubleBuffured2->Window()->SetVisible(EFalse);
	iWs.Finish();
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, 0);
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurfaceDoubleBuffered, 0);
	FillRefBitmap(KRgbGreen);
	TEST(CompareAllScreens());
	}

/**
  @SYMTestCaseID GRAPHIC-SURFACEUPDATE-0032
  	
  @SYMCR CR1650

  @SYMREQ REQ11599, REQ11600, REQ11601

  @SYMTestCaseDesc NotifyWhenDisplayedXTimes() to deal with global 
  updates and be signalled correctly when screen is unplugged.
	
  @SYMTestPriority 	High

  @SYMTestStatus Implemented
	
  @SYMTestActions 
  	The test consists of a single-buffered surface, S1, visible on 
  	screens A (master) and B.

	Screens are ordered in the following priority: A > B
		1	Make global submit update for surface S1 with notify 
		when notify when displayed X times. X should be approximately 10 
		seconds.
		2	Unplug screen A while display XTimes is still in progress.
		3	After receiving notification, plug in screen A
		4	Make global submit update with notify when displayed X times. 
		X should be approximately 10 seconds.
		5	Make global submit update for surface S1, with notify when 
		displayed X times. X should be approximately 10 seconds.
		6	Unplug screen B while display XTimes is still in progress.
        7   Make global submit update for surface S1, with notify when 
        displayed X times. X should be approximately 10 seconds.		
		8	Unplug screen A as well whilst display XTimes is still in progress.
		9   Reconnect both screens
  	
  @SYMTestExpectedResults
  	After the 10 seconds have elapsed from step 1, notification is 
  	received (step 3) with KErrNone (this is triggered by the Receiver 
  	for screen B).
	After 10 seconds have elapsed from step 4, notification is received 
	with KErrNone (this is triggered by the Receiver for screen A).
	After step 8 (which must occur before 10 seconds) notification will 
	be received with KErrNotVisible (ie. The surface is not visible on 
	any screen).
*/
void CTSurfaceUpdateInteg::TestCase3()
	{
	INFO_PRINTF1(_L("Exercising NotifyWhenDisplayedXTimes while some screen is disconnected."));

	TRequestStatus status;
	TInt count = 100 * 2; 
	iSurfaceUpdate.NotifyWhenDisplayedXTimes(count, status);	
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, 0);
	
	INFO_PRINTF1(_L("Disconnect the screen when notification when displayed X times request is still in a progress")); 
	TInt displayState = EDisconnect;
	TInt disconnectedScreenNo = 0;
	TInt res = UserSvr::HalFunction(EHalGroupDisplay | (disconnectedScreenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	TEST(res == KErrNone);
	User::WaitForRequest(status); //The content update receiver will complete notification when “count” number of display refreshes has occurred.  
									//We are not blocked here indefinitely, so there is no point to use a command dispatcher
	TEST(status == KErrNone);

	//restore the first screen 
	INFO_PRINTF1(_L("Connect the screen when notification when displayed X times request is still in a progress")); 
	displayState = ENormalResolution;
	res = UserSvr::HalFunction(EHalGroupDisplay | (disconnectedScreenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	TEST(res == KErrNone);

	iSurfaceUpdate.NotifyWhenDisplayedXTimes(count, status);	
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, 0);
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	
	// Make sure other surface is not visible
	iWindowAbove11->Window()->SetVisible(EFalse);
	iWs.Finish();
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking
	
	// above1 surface should not be visible because window is not visible
	TTimeStamp timeStamp; 
	iSurfaceUpdate.NotifyWhenDisplayed(status, timeStamp);	
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurfaceAbove1, 0);
	User::WaitForRequest(status);
	TEST(status == KErrNotVisible);
	
	//disconnect the secondary screen - surface still visible on primary
	INFO_PRINTF1(_L("Disconnect the secondary screen - surface still visible on primary")); 
	displayState = EDisconnect;
	disconnectedScreenNo = 1;
	iSurfaceUpdate.NotifyWhenDisplayedXTimes(count, status);	
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, 0);
	
	res = UserSvr::HalFunction(EHalGroupDisplay | (disconnectedScreenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
	TEST(res == KErrNone);
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking

   //disconnect the primary screen - surface now not visible on any screen
    INFO_PRINTF1(_L("Disconnect the primary screen - surface not visible on any screen")); 
    displayState = EDisconnect;
    disconnectedScreenNo = 0;
    iSurfaceUpdate.NotifyWhenDisplayedXTimes(count, status);    
    iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, 0);
    
    res = UserSvr::HalFunction(EHalGroupDisplay | (disconnectedScreenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
    TEST(res == KErrNone);
    User::WaitForRequest(status);
    TEST(status == KErrNotVisible); // not visible raised if primary is disconnected
    User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking
        

    //restore initial state 
    disconnectedScreenNo = 0;
    displayState = ENormalResolution;
    res = UserSvr::HalFunction(EHalGroupDisplay | (disconnectedScreenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
    TEST(res == KErrNone);
    disconnectedScreenNo = 1;
    res = UserSvr::HalFunction(EHalGroupDisplay | (disconnectedScreenNo<<16), EDisplayHalSetDisplayState, &displayState, NULL);
    TEST(res == KErrNone);

    
	iWs.Finish();	
	iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurface, 0);
	User::After(TTimeIntervalMicroSeconds32(100000)); //useful for visual checking
	TEST(CompareAllScreens());
	}

void CTSurfaceUpdateInteg::RunTestCaseL(TInt aCurTestCase)
	{
	((CTSurfaceUpdateIntegStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		{
		
		if((static_cast <CTSurfaceUpdateIntegStep*> (iStep)) -> IsScreenCaptureSupported()  && (iNumOfScreens > 1))
			{
			((CTSurfaceUpdateIntegStep*)iStep)->SetTestStepID(_L("GRAPHIC-SURFACEUPDATE-0026"));
			INFO_PRINTF1(_L("TestCase1"));
			TestCase1L();
			}
		else
			{
			((CTSurfaceUpdateIntegStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			INFO_PRINTF3(_L("The test will be skipped as the environment is not ready: Number of screen : %d, Is screen capture supported: %d"), 
					iNumOfScreens, (static_cast <CTSurfaceUpdateIntegStep*> (iStep)) -> IsScreenCaptureSupported());
			TestComplete();		
			}	
		break;
		}
#ifdef __WINS__
	case 2:
		{
		((CTSurfaceUpdateIntegStep*)iStep)->SetTestStepID(_L("GRAPHIC-SURFACEUPDATE-0031"));
		INFO_PRINTF1(_L("TestCase2"));
		TestCase2L();
		break;
		}
	case 3:
		{
		((CTSurfaceUpdateIntegStep*)iStep)->SetTestStepID(_L("GRAPHIC-SURFACEUPDATE-0032"));
		INFO_PRINTF1(_L("TestCase3"));
		TestCase3();
		break;
		}
#endif		
	default:
		((CTSurfaceUpdateIntegStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTSurfaceUpdateIntegStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTSurfaceUpdateIntegStep*)iStep)->RecordTestResultL();
	}
//--------------
_LIT(KCommandDispatcher, "CommandDispatcher");
const TUint KDefaultHeapSize=0x10000;

/**
 * Launch the command thread.
 */
TInt CCommandDispatcher::Start()
	{
	RThread commandThread;
	TInt res = KErrGeneral;
	TBuf<64> commandThreadName;
	TBuf<64> commandThreaMask;
	
	// Guarantee uniqueness of thread name by using timestamp
	TTime tm;
	TBuf<32> timeStamp;
	tm.UniversalTime();
	TRAP(res, tm.FormatL(timeStamp, _L("_%H%T%S%C")));
	if(res != KErrNone)
		{
		return res;
		}

	commandThreadName.Append(KCommandDispatcher);
	commandThreadName.Append(timeStamp);
	commandThreaMask = commandThreadName;
	commandThreaMask.Insert(0, _L("*"));
	TFindThread findThread(commandThreaMask);
	TFullName name;
	  // Need to check that the thread exists.
	if (findThread.Next(name)!=KErrNone)
		{
		  // Create the thread for the server.
		res = commandThread.Create(commandThreadName,
			CCommandDispatcher::ThreadFunction,
			KDefaultStackSize,
			KDefaultHeapSize,
			KDefaultHeapSize,
			this
			);
			
          // The thread has been created OK so get it started - however
          // we need to make sure that it has started before we continue.
		if (res==KErrNone)
			{
			TRequestStatus rendezvousStatus;
			commandThread.SetPriority(EPriorityNormal);
			commandThread.Rendezvous(rendezvousStatus);
			commandThread.Resume();
			}
		}
	commandThread.Close();
	return res;
	}

TInt CCommandDispatcher::ThreadFunction(TAny* aAny)
	{
	  // get clean-up stack
	CTrapCleanup* cleanup=CTrapCleanup::New();
	CCommandDispatcher* commandDispatcher = (CCommandDispatcher*) aAny;
	TRAPD(res, commandDispatcher->ExecuteCommandL());
	delete cleanup; 
	return res;
	}

/**
 * Run commands in a thread different to the thread where the instance 
 * of the class was initialized.
 */
void CCommandDispatcher::ExecuteCommandL()
	{
	switch(iCommandName)
		{
	case EDisconnectDisplayAndReleaseBuffer:
		{
		RSurfaceUpdateSession surfaceUpdate;
		User::LeaveIfError(surfaceUpdate.Connect());
		CleanupClosePushL(surfaceUpdate);
		TSwitchDisplayAndReleaseBuffer* param = static_cast<TSwitchDisplayAndReleaseBuffer*>(iParam); 
		if(!param)
			{
			User::Leave(KErrArgument);
			}
		TInt displayState = EDisconnect;
		TInt screenNumber = param->iScreen;
		TInt bufferNumber = param->iBuffer;
		User::LeaveIfError(UserSvr::HalFunction(EHalGroupDisplay | (screenNumber<<16), EDisplayHalSetDisplayState, &displayState, NULL));
		//Submit Update to unblock the main client thread 
		User::LeaveIfError(surfaceUpdate.SubmitUpdate(KAllScreens, param->iSurfaceId, bufferNumber));
		CleanupStack::PopAndDestroy(&surfaceUpdate);
		break;
		}
	case EConnectDisplayAndReleaseBuffer:
		{
		RSurfaceUpdateSession surfaceUpdate;
		User::LeaveIfError(surfaceUpdate.Connect());
		CleanupClosePushL(surfaceUpdate);
		TSwitchDisplayAndReleaseBuffer* param = static_cast<TSwitchDisplayAndReleaseBuffer*>(iParam); 
		if(!param)
			{
			User::Leave(KErrArgument);
			}
		TInt displayState = ENormalResolution;
		TInt screenNumber = param->iScreen;
		TInt bufferNumber = param->iBuffer;
		User::LeaveIfError(UserSvr::HalFunction(EHalGroupDisplay | (screenNumber<<16), EDisplayHalSetDisplayState, &displayState, NULL));
		//Submit Update to unblock the main client thread 
		User::LeaveIfError(surfaceUpdate.SubmitUpdate(KAllScreens, param->iSurfaceId, bufferNumber));
		CleanupStack::PopAndDestroy(&surfaceUpdate);
		break;
		}
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

//--------------
__CONSTRUCT_STEP__(SurfaceUpdateInteg)

/**
 * Retrieve screen capturing plug-in, if succeed set a flag.
 */
void CTSurfaceUpdateIntegStep::TestSetupL()
	{
//	check if the screen capture plug-in presents in the system
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);

	TInt numOfScreens = ws.NumberOfScreens();
	if(numOfScreens < 2)
		{
		ERR_PRINTF2(_L("Environment is not ready, the number of the screens in the system for this test should be 2 or more, the current number is: %d"), numOfScreens);
		User::Leave(KErrNotReady);
		}
	
	CWsScreenDevice* wsDev = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(wsDev);
	User::LeaveIfError(wsDev->Construct(0));
	TDisplayMode dispMode = wsDev->DisplayMode();
	INFO_PRINTF2(_L("Screen device display mode is: %d."), dispMode);
	
	MTestScreenCapture* csc = static_cast<MTestScreenCapture*> (wsDev->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
	if(csc)
		{
		INFO_PRINTF1(_L("Screen capture plug-in presents in the system."));
		iScreenCapture = ETrue;
		}
	else
		{
		INFO_PRINTF1(_L("Screen capture plug-in is not installed in the system, to make automated checking screen capture rendering stage is required, \
see graphics\\wserv_std_plugins\\screencapture"));
		}
	
	CleanupStack::PopAndDestroy(2, &ws); //ws, wsDev
	}
	
void CTSurfaceUpdateIntegStep::TestClose()
	{
	}

