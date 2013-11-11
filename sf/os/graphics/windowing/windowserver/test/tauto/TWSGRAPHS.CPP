// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test step contains a series of tests cases to validate the correct behaviour of PREQ1246 implementation.
// In order to create these test cases, basic implementations of the objects involved in this PREQ will be created,
// .i.e. CWsGraphic-derived objects (generically named CWsGraphicTest) and CWsGraphicDrawer-derived objects
// (generically named CWsGraphicDrawerTest).
// Actual construction is performed by a UI-specific entity such as a theme manager. The test code shall replace
// that theme manager functionality, in terms of being the test code who owns a collection of CWsGraphicTest
// objects.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TWSGRAPHS.H"
#include "../inc/WSGRAPHICDRAWERARRAY.H"
#include "../../nga/graphicdrawer/panics.h"
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
#include "wsbufferdrawer.h"
#endif

_LIT(KTestExe, "TWSGRAPHICTEST.exe");
_LIT(KSpace, " ");

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
const TInt KCustomTextCursorId = TTextCursor::ETypeLastBasic + 57; // 57 is arbitrary
#endif

CCrWin* CCrWin::NewL(TInt aScreenId, TBool aDraw)
	{
	CCrWin* win = new(ELeave) CCrWin;
	CleanupStack::PushL(win);
	win->ConstructL(aScreenId, aDraw);
	CleanupStack::Pop(win);
	return win;
	}

CCrWin::~CCrWin()
	{
	iWin.Close();
	iGroup.Close();
	delete iGc;
	delete iScr;
	iWs.Close();
	}

void CCrWin::ConstructL(TInt aScreenId, TBool aDraw)
	{
	User::LeaveIfError(iWs.Connect());
	iScr = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScr->Construct(aScreenId));
	User::LeaveIfError(iScr->CreateContext(iGc));
	iGroup = RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(0xbadbabe,ETrue));
	iGroup.SetOrdinalPosition(0,100);
	iWin = RWindow(iWs);
	User::LeaveIfError(iWin.Construct(iGroup,0xbadcafe));
	iWin.SetRequiredDisplayMode(EColor64K);
	iWin.Activate();
	iWs.Flush();
	if (aDraw)
	Draw();
	}

void CCrWin::Draw()
	{
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	iGc->SetBrushColor(KRgbRed);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect rect(iScr->SizeInPixels());
	iGc->DrawRect(rect);
	iGc->SetBrushColor(KRgbBlue);
	iGc->DrawEllipse(TRect(rect.iTl.iX,rect.iTl.iY,rect.iBr.iX/2,rect.iBr.iY));
	iGc->DrawEllipse(TRect(rect.iBr.iX/2,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY));
	iGc->Deactivate();
	iWin.EndRedraw();
	iWs.Flush();
	}


void CCrWin::DrawFirstHalf()
	{
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	iGc->SetBrushColor(KRgbRed);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect rect(TPoint(0,0),TSize(iScr->SizeInPixels().iWidth/2,iScr->SizeInPixels().iHeight));
	iGc->DrawRect(rect);
	iWs.Flush();	
	}

void CCrWin::DrawSecondHalf()
	{
	TRect rect(TPoint(iScr->SizeInPixels().iWidth/2,0),TSize(iScr->SizeInPixels().iWidth/2,iScr->SizeInPixels().iHeight));
	iGc->DrawRect(rect);
	iGc->Deactivate();
	iWin.EndRedraw();	
	iWs.Flush();		
	}

/** 
The objective of this function is, two animations should run independently
with respective frame rate in the given time interval.
The time delay allows to draw animations freely and the plug-in
calculates number of times DoDraw() function called during this interval.

@param TInt Wsgrphic test plug-in id.
*/
void CCrWin::DrawGraphic(TInt aWsId)
	{
	// draw the animation in two positions
	const TSize screenSize = iScr->SizeInPixels();	
	const TRect position(0,0,screenSize.iWidth/2,screenSize.iHeight);
	const TRect position2((screenSize.iWidth/2)+1,0,screenSize.iWidth,screenSize.iHeight);
	//PeterI if CWsGraphic animation areas overlap then when one redraws the other will as well.
	//2 separate positions are needed otherwise the framerates will be identical.
	
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	const TUint8 animid1=0;
	const TUint8 fps1=20;
	TBuf8<2> animData1;
	animData1.Append(animid1);   //animId1
	animData1.Append(fps1); //20fps
	iGc->DrawWsGraphic(aWsId,position,animData1);
	iWs.Flush();
	User::After(200000);
	const TUint8 animid2=1;
	const TUint8 fps2=60;
	TBuf8<2> animData2;
	animData2.Append(animid2);   //animId2
	animData2.Append(fps2); //60fps
	iGc->DrawWsGraphic(aWsId,position2,animData2);
	iWs.Flush();
	User::After(200000);
	iGc->Deactivate();
	iWin.EndRedraw();
	iWs.Flush();
	}

/**
 * Set a standard text cursor on this window.
 * @see RWindowGroup::SetTextCursor()
 */
void CCrWin::SetTextCursor(const TPoint &aPos, const TTextCursor &aCursor)
	{
	iGroup.SetTextCursor(iWin, aPos, aCursor);
	}

/**
 * Cancel a text cursor from this window.
 * @see RWindowGroup::CancelTextCursor()
 */
void CCrWin::CancelTextCursor()
	{
	iGroup.CancelTextCursor();
	}

CCrAlphaWin* CCrAlphaWin::NewL(TInt aScreenId)
	{
	CCrAlphaWin* win = new(ELeave) CCrAlphaWin;
	CleanupStack::PushL(win);
	win->ConstructL(aScreenId);
	CleanupStack::Pop(win);
	return win;
	}

CCrAlphaWin::~CCrAlphaWin()
	{
	iWin.Close();
	iGroup.Close();
	delete iScr;
	iWs.Close();
	}

void CCrAlphaWin::ConstructL(TInt aScreenId)
	{
	User::LeaveIfError(iWs.Connect());
	iScr = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScr->Construct(aScreenId));
	iGroup = RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(0xbadc0de,ETrue));
	iGroup.SetOrdinalPosition(0,100);
	iWin = RWindow(iWs);
	User::LeaveIfError(iWin.Construct(iGroup,0xbadbeef));
	iWin.SetRequiredDisplayMode(EColor64K);
	iWin.SetTransparencyAlphaChannel();
	iWin.SetBackgroundColor(TRgb(0xff,0xff,0,0x80));
	iWin.Activate();
	iWs.Flush();
	}


//
//  CTWsGraphs
//

CTWsGraphs::CTWsGraphs(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
LOCAL_D void DeleteSpriteMember(TAny* aSpriteMember)
	{
	TSpriteMember* member=reinterpret_cast<TSpriteMember*>(aSpriteMember);
	delete member->iBitmap;
	member->iBitmap=NULL;
	delete member->iMaskBitmap;
	member->iMaskBitmap=NULL;
	}
#endif

CTWsGraphs::~CTWsGraphs()
	{
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	DeleteSpriteMember(&iSpriteMemberArray[0]);
	iSpriteMemberArray.Close();
#endif
	delete iGdCoverage;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	delete iAfter;
	delete iBefore;
	delete iBackCopy;
	delete iFrontCopy;
#endif
	delete iListen;
	delete iNotify2;
	delete iNotify1;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	delete iRedir;
#endif
	}

void CTWsGraphs::ConstructL()
	{
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	iRedir = CWsRedir::NewL(iTest->iScreenNumber,ETrue);
	iRedir->SetCallBack(TCallBack(CTWsGraphs::PluginCallBack,this));
#endif
	iNotify1 = CWsNotify::NewL(EFalse);
	iNotify2 = CWsNotify::NewL(ETrue);
	iListen = CWsListen::NewL(ETrue);
	iListen->SetCallBack(TCallBack(CTWsGraphs::PluginCallBack,this));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	iFrontCopy = new(ELeave) CFbsBitmap;
	iBackCopy = new(ELeave) CFbsBitmap;
	iBefore = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBefore->Create(TheClient->iScreen->SizeInPixels(), EColor64K));
	iAfter = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iAfter->Create(TheClient->iScreen->SizeInPixels(), EColor64K));
#endif
	iGdCoverage = CWsGdCoverage::NewL();
	iGdCoverage->SetCallBack(TCallBack(CTWsGraphs::PluginCallBack,this));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	TSpriteMember spriteMember;
	spriteMember.iBitmap = NULL;
	spriteMember.iMaskBitmap = NULL;
	spriteMember.iInvertMask =EFalse;
	spriteMember.iDrawMode = CGraphicsContext::EDrawModePEN;
	spriteMember.iOffset = TPoint();
	spriteMember.iInterval = TTimeIntervalMicroSeconds32(0);
	CleanupStack::PushL(TCleanupItem(DeleteSpriteMember, &spriteMember));
	spriteMember.iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(spriteMember.iBitmap->Load(TEST_BITMAP_NAME, EMbmWsautotestBmp1));
	spriteMember.iMaskBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(spriteMember.iMaskBitmap->Load(TEST_BITMAP_NAME, EMbmWsautotestBmp1mask));
	User::LeaveIfError(iSpriteMemberArray.Append(spriteMember));
	CleanupStack::Pop(&spriteMember);
#endif
	}

void CTWsGraphs::LaunchNewProcess(const TDesC& aExecutable)
	{
	TBuf<128> args;
 	args.Append(KSpace);
	args.AppendNum(iTest->iScreenNumber);
	RProcess pr;
	TInt err = pr.Create(aExecutable,args);
	if (err == KErrNone)
		{
		TRequestStatus status;
		pr.Logon(status);
		pr.Resume();
		User::WaitForRequest(status);
		err = pr.ExitReason();
		pr.Close();
		if (err != KErrNone)
			{
			_LIT(KLog,"%S returned error: %d. Check RDebug output.");
			LOG_MESSAGE3(KLog, &aExecutable, err);
			}
		}
	else
		{
		_LIT(KLog,"Can't create the process (%S), err=%d");
		LOG_MESSAGE3(KLog, &aExecutable, err);
		}
	TEST(err == KErrNone);
	// Restore main test group to foreground.
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0);
	}

TInt CTWsGraphs::PluginCallBack(TAny* /*aArg*/)
	{
	return (TInt)EWait;
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

/**
 @SYMTestCaseID				GRAPHICS-WSERV-0371
 @SYMREQ					GT247-CR0714
 @SYMTestCaseDesc			Test interface extension
 @SYMTestPriority			High
 @SYMTestStatus				Implemented
 @SYMTestActions			Retrieves object interfaces from Content Rendering Plugin (plugin).
							Actions step:
							-Create plugin
							-Query interfaces obtained from plugin side
 @SYMTestExpectedResults	Supported interfaces should return non null
*/
TestState CTWsGraphs::TestInterfaceExtensionL()
	{
	if (iSubState==0)
		{
		_LIT(KTestInterfaceExtension, "TestInterfaceExtension");
		INFO_PRINTF1(KTestInterfaceExtension);

		++iSubState;
		Mem::FillZ(&iRedirInfo, sizeof(TRedirectorInfo));
		iRedir->QueryPlugin(iRedirInfo);
		return EWait;
		}
	TEST(iRedirInfo.iScreenConfigInterface!=NULL);
	TEST(iRedirInfo.iFrontBufferInterface!=NULL);
	TEST(iRedirInfo.iScreenBitmapHandle!=0);
	iFrontCopy->Duplicate(iRedirInfo.iScreenBitmapHandle);

	if (TransparencySupportedL()!=KErrNotSupported)
		{
		TEST(iRedirInfo.iBackBufferInterface!=NULL);
		TEST(iRedirInfo.iFlickerBitmapHandle!=0);
		iBackCopy->Duplicate(iRedirInfo.iFlickerBitmapHandle);
		}

	++(iTest->iState);
	iSubState = 0;
	return ENext;
	}

#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

void CTWsGraphs::CreateWindowL(TBool aDraw)
	{
	iWin = CCrWin::NewL(iTest->iScreenNumber, aDraw);
	}

void CTWsGraphs::DestroyWindowL()
	{
	delete iWin;
	iWin = NULL;
	}

void CTWsGraphs::CreateAlphaWindowL()
	{
	iAlpha = CCrAlphaWin::NewL(iTest->iScreenNumber);
	}

void CTWsGraphs::DestroyAlphaWindowL()
	{
	delete iAlpha;
	iAlpha = NULL;
	}

TBool CTWsGraphs::CompareBitmapArea16Bpp(CFbsBitmap* aBmp1, const TPoint& aPos1, CFbsBitmap* aBmp2, const TPoint& aPos2, const TSize& aSize)
	{
	const TDisplayMode dispmode = aBmp1->DisplayMode();
	if (dispmode!=aBmp2->DisplayMode())
		return EFalse;
	const TInt stride1 = aBmp1->DataStride();
	const TInt stride2 = aBmp2->DataStride();
	const TInt linebytes = aSize.iWidth * 2;
	const TInt pixelbytes = 2;
	aBmp1->LockHeap();
	const TUint8* p1 = ((const TUint8*)aBmp1->DataAddress())+aPos1.iY*stride1+aPos1.iX*pixelbytes;
	const TUint8* p2 = ((const TUint8*)aBmp2->DataAddress())+aPos2.iY*stride2+aPos2.iX*pixelbytes;
	for (TInt y=0; y<aSize.iHeight; ++y)
		{
		if (Mem::Compare(p1+y*stride1,linebytes,p2+y*stride2,linebytes)!=0)
			{
			aBmp1->UnlockHeap();
			return EFalse;
			}
		}
	aBmp1->UnlockHeap();
	return ETrue;
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
TestState CTWsGraphs::TestScreenRedirectionL()
	{
	if (iSubState==0)
		{
		_LIT(KTestScreenRedirection, "TestScreenRedirection");
		INFO_PRINTF1(KTestScreenRedirection);

		++iSubState;
		/**
		 @SYMTestCaseID				GRAPHICS-WSERV-0372
		 @SYMREQ					GT247-CR0714
		 @SYMTestCaseDesc			Redirect wserv screen drawing to custom graphics context
		 @SYMTestPriority			High
		 @SYMTestStatus				Implemented
		 @SYMTestActions			Redirects wserv screen drawing to bitmap context owned by plugin.
									Actions step:
									-Draw opaque window.
									-Save screen content to a bitmap
									-Instruct plugin to redirect wserv screen drawing to a bitmap device
									-Draw the same window again
									-Retrieve plugin bitmap and compare against  the saved bitmap
		 @SYMTestExpectedResults	Bitmap content match
		*/
		CreateWindowL();		
		TheClient->iScreen->CopyScreenToBitmap(iBefore);
		DestroyWindowL();
		iRedir->Redirect(CWsRedir::EFrontBuffer, ETrue);
		return EWait;
		}

	if (iSubState==1)
		{
		++iSubState;
		CreateWindowL();
		TSize sz = iBefore->SizeInPixels();
		TInt bytes = sz.iWidth*sz.iHeight*2; // EColor64K
		iBefore->LockHeap();
		TEST(Mem::Compare((const TUint8*)iFrontCopy->DataAddress(),bytes,(const TUint8*)iBefore->DataAddress(),bytes)==0);
		iBefore->UnlockHeap();

		Mem::FillZ(&iRedirInfo, sizeof(TRedirectorInfo));			
		iRedir->QueryPlugin(iRedirInfo);
		return EWait;
		}

	if (iSubState==2)
		{
		++iSubState;
		/**
		 @SYMTestCaseID				GRAPHICS-WSERV-0047
		 @SYMTestCaseDesc			Screen update event
		 @SYMTestPriority			Medium
		 @SYMTestStatus				Implemented
		 @SYMTestActions			Check plugin receive screen update event during redirection
		 @SYMTestExpectedResults	Counter is non-zero
		*/
		TEST(iRedirInfo.iUpdateCounter>0);

		DestroyWindowL();
		/**
		 @SYMTestCaseID				GRAPHICS-WSERV-0373
		 @SYMREQ					GT247-CR0714
		 @SYMTestCaseDesc			Stop wserv screen drawing redirection
		 @SYMTestPriority			High
		 @SYMTestStatus				Implemented
		 @SYMTestActions			Stop wserv screen drawing redirection.
									Actions step:
									-Instruct plugin to stop wserv screen drawing redirection
									-Draw the same window again
									-Save screen content to another bitmap
									-Compare the saved bitmap against newly saved bitmap
		 @SYMTestExpectedResults	Bitmap content match
		*/
		iRedir->Redirect(CWsRedir::EFrontBuffer, EFalse);
		return EWait;
		}

	if (iSubState==3)
		{
		++iSubState;
		CreateWindowL();
		TheClient->iScreen->CopyScreenToBitmap(iAfter);
		TSize sz = iBefore->SizeInPixels();	
		TInt bytes = sz.iWidth*sz.iHeight*2; // EColor64K	
		iAfter->LockHeap();
		TEST(Mem::Compare((const TUint8*)iAfter->DataAddress(),bytes,(const TUint8*)iBefore->DataAddress(),bytes)==0);
		iAfter->UnlockHeap();
		DestroyWindowL();

		Mem::FillZ(&iRedirInfo, sizeof(TRedirectorInfo));			
		iRedir->QueryPlugin(iRedirInfo);
		return EWait;
		}

	/**
	 @SYMTestCaseID				GRAPHICS-WSERV-0374
	 @SYMTestCaseDesc			Screen update event
	 @SYMTestPriority			Medium
	 @SYMTestStatus				Implemented
	 @SYMTestActions			Check plugin receive no screen update event when redirection is terminated
	 @SYMTestExpectedResults	Counter is zero
	*/
	TEST(iRedirInfo.iUpdateCounter==0);

	++(iTest->iState);
	iSubState = 0;

	return ENext;
	}

TestState CTWsGraphs::TestTextCursorUnderRedirectionL(TTestCursorType aCursorType)
	{
	/**
	 @SYMTestCaseID				GRAPHICS-WSERV-0363
	 @SYMTestCaseDesc			Text Cursor when drawing redirected
	 @SYMTestPriority			Medium
	 @SYMTestStatus				Implemented
	 @SYMTestActions			Action steps:
	 							- Draw the text cursor in the left side of the screen
	 							- Re-direct the Front Buffer
	 							- Move the text cursor to the right side of the screen
	 							- Pause 0.5 seconds because this amount of time is needed
								  to change from the flash ON phase to the flash OFF phase
	 							- Stop re-directing
	 							- See if when we exit re-direction in a different place in
	 							  the phase of the text cursor flashing, whether we get
	 							  non-text cursor drawing artefacts in the old location
	 							  where the text cursor used to be
	 @SYMTestExpectedResults	Left side of the screen does not show a Text Cursor
	 */
	
	ASSERT(aCursorType == ETestStandardTextCursor || aCursorType == ETestCustomTextCursor);
	
	// Cursor Flash Period is 1 second (comprising two phases; ON and OFF)
	const TInt KCursorFlashPeriod = 1000000;
	const TInt KWaitForNextFlashPhase = KCursorFlashPeriod / 2;
	
	// Size of the cursor; it may be either a standard or custom text cursor
	TSize cursorSize = (aCursorType == ETestStandardTextCursor) ? TSize(15, 20) : TSize(80, 80);
		
	// Original Text Cursor position in the left part of the screen
	const TPoint originalCursorPos(45, 40);
	
	// New Text Cursor position in the right half of the screen
	const TPoint newCursorPos((TheClient->iScreen->SizeInPixels().iWidth/2) + 45, 40);
	
	// Clean area of the screen which never had a text cursor
	const TPoint cleanReferencePos(45, 40 + 80);
	
	/* Initial setup to get a window with a standard flashing text cursor */
	if (iSubState == 0)
		{
		++iSubState;			
		CreateWindowL(ETrue);
		
		if (aCursorType == ETestStandardTextCursor)
			{
			_LIT(KTestTextCursorUnderRedirection, "TestTextCursorUnderRedirection(Standard Cursor)");
			INFO_PRINTF1(KTestTextCursorUnderRedirection);	
			iTextCursor.iType=TTextCursor::ETypeRectangle;
			iTextCursor.iHeight=cursorSize.iHeight;
			iTextCursor.iAscent=0;
			iTextCursor.iWidth=cursorSize.iWidth;
			iTextCursor.iFlags=0; 			// means flash the cursor
			iTextCursor.iColor=KRgbGreen;
			iWin->SetTextCursor(originalCursorPos, iTextCursor);
			}
		else if (aCursorType == ETestCustomTextCursor)
			{
			_LIT(KTestTextCursorUnderRedirection, "TestTextCursorUnderRedirection(Custom Cursor)");
			INFO_PRINTF1(KTestTextCursorUnderRedirection);	

			TInt err = TheClient->iWs.SetCustomTextCursor(
					KCustomTextCursorId,
					iSpriteMemberArray.Array(),
					ESpriteFlash,
					RWsSession::ECustomTextCursorAlignTop
					);
			iTextCursor.iType=KCustomTextCursorId;
			iTextCursor.iHeight=cursorSize.iHeight;
			iTextCursor.iAscent=0;
			iTextCursor.iWidth=cursorSize.iWidth;
			iTextCursor.iFlags=TTextCursor::EFlagClipHorizontal; // means flash the cursor and clip the sprite
			iTextCursor.iColor=KRgbCyan;
			iWin->SetTextCursor(originalCursorPos, iTextCursor);
			}
		else
			{
			// unknown type of test being requested
			ASSERT(0);
			}
		iWin->DrawFirstHalf();	
		return EWait;
		}
	/*
	 * Re-direct drawing to another Front Buffer.  Whilst re-directed, change the
	 * position of the text cursor.  Then pause 0.5 seconds because this is how
	 * long it will take to enter the next phase in the flashing cycle.  Finally
	 * stop re-directing.  We exit the re-direction in a different point in the
	 * phase of the text cursor from when we entered it.
	 * This is key to testing for faulty behaviour.
	 */
	if (iSubState==1)
		{
		++iSubState;
		User::After(KCursorFlashPeriod * 2);  // so its easy to visually review progress
		iRedir->Redirect(CWsRedir::EFrontBuffer, ETrue);
		iWin->SetTextCursor(newCursorPos, iTextCursor);
		User::After(KWaitForNextFlashPhase);
		iRedir->Redirect(CWsRedir::EFrontBuffer, EFalse);
		return EWait;
		}
	/*
	 * Paint the right hand side of the screen which should now have a text cursor.
	 */
	if (iSubState==2)
		{
		++iSubState;
		iWin->DrawSecondHalf();
		return EWait;
		}
	/*
	 * Let the cursor flash a few times, as it assists manual viewing of the progress
	 * of the test.
	 */
	if (iSubState==3)
		{
		++iSubState;
		User::After(KCursorFlashPeriod * 3);
		return EWait;
		}
	/*
	 * Check to see if the text cursor did move to the right of the screen.
	 */
	if (iSubState==4)
		{
		++iSubState;
		
		/* When we do a screen comparison, we supply flag 0, which means
		 * don't include the text cursor.  We do this because we are interested
		 * in screen artefacts.
		 */
		TEST(TheClient->iScreen->RectCompare(
				TRect(originalCursorPos, cursorSize),
				TRect(cleanReferencePos, cursorSize),
				0)); // must not supply CWsScreenDevice::EIncludeTextCursor
		
		return EWait;
		}
	/* Clean up */
	if (iSubState==5)
		{
		++iSubState;
		iWin->CancelTextCursor();
		DestroyWindowL();
		}
	
	iSubState = 0;

	return ENext;
	}

/**
 @SYMTestCaseID				GRAPHICS-WSERV-0375
 @SYMREQ					GT247-CR0714
 @SYMTestCaseDesc			Redirect wserv flickerfree drawing to custom graphics context
 @SYMTestPriority			Medium
 @SYMTestStatus				Implemented
 @SYMTestActions			Redirect wserv flickerfree buffer drawing.
							Action step:
							-Draw opaque window (as background) and transparent window
							-Save screen content to a bitmap
							-Instruct plugin to redirect wserv flickerfree buffer drawing
							-Draw the same opaque window and transparent window again
							-Retrieve plugin bitmap and compare against the saved bitmap
 @SYMTestExpectedResults	Bitmap content match
*/
/**
 @SYMTestCaseID				GRAPHICS-WSERV-0376
 @SYMREQ					GT247-CR0714
 @SYMTestCaseDesc			Stop wserv flickerfree drawing redirection
 @SYMTestPriority			Medium
 @SYMTestStatus				Implemented
 @SYMTestActions			Stop wserv flickerfree buffer drawing redirection.
							Action step:
							-Instruct plugin to stop wserv filckerfree drawing redirection
							-Draw the same opaque and transparent window
							-Save screen content to another bitmap
							-Compare saved bitmap against newly saved bitmap
 @SYMTestExpectedResults	Bitmap content match
*/

TestState CTWsGraphs::TestFlickerRedirectionL()
	{
	if (TransparencySupportedL()==KErrNotSupported)
		{
		++(iTest->iState);
		return ENext;
		}

	// flush transparent window destruction created in TransparencySupportedL before
	// proceeding with the test

	TheClient->iWs.Flush();

	if (iSubState==0)
		{
		_LIT(KTestFlickerRedirection, "TestFlickerRedirection");
		INFO_PRINTF1(KTestFlickerRedirection);

		++iSubState;

		CreateWindowL();
		CreateAlphaWindowL();
		TheClient->iScreen->CopyScreenToBitmap(iBefore);
		DestroyAlphaWindowL();
		DestroyWindowL();
		iRedir->Redirect(CWsRedir::EBackBuffer, ETrue);
		return EWait;
		}

	if (iSubState==1)
		{
		++iSubState;
		CreateWindowL();
		CreateAlphaWindowL();
		TSize sz = iBefore->SizeInPixels();
		TInt bytes = sz.iWidth*sz.iHeight*2; // EColor64K
		iBefore->LockHeap();
		TInt ret=Mem::Compare((const TUint8*)iBackCopy->DataAddress(),bytes,(const TUint8*)iBefore->DataAddress(),bytes);
		TEST(ret==0);
		if (ret!=0)
			{
			_LIT(KLog,"The memory of two bitmaps doesn't match");
			LOG_MESSAGE(KLog);
			}
		iBefore->UnlockHeap();
		DestroyAlphaWindowL();
		DestroyWindowL();
		iRedir->Redirect(CWsRedir::EBackBuffer, EFalse);
		return EWait;
		}

	CreateWindowL();
	CreateAlphaWindowL();
	TheClient->iScreen->CopyScreenToBitmap(iAfter);
	TSize sz = iBefore->SizeInPixels();
	TInt bytes = sz.iWidth*sz.iHeight*2; // EColor64K
	iAfter->LockHeap();
	TEST(Mem::Compare((const TUint8*)iAfter->DataAddress(),bytes,(const TUint8*)iBefore->DataAddress(),bytes)==0);
	iAfter->UnlockHeap();
	DestroyAlphaWindowL();
	DestroyWindowL();

	++(iTest->iState);
	iSubState = 0;
	return ENext;
	}

#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

/**
 @SYMTestCaseID				GRAPHICS-WSERV-0377
 @SYMREQ					GT247-CR0714
 @SYMTestCaseDesc			Enable event notification
 @SYMTestPriority			Medium
 @SYMTestStatus				Implemented
 @SYMTestActions			Enable plugin to register to event notification.
							Action step:
							-Instruct plugin to register event handler
							-Draw fullscreen window (plugin will receive window visibility changed event)
							-Query visibility region from plugin side
							-Compare window visible region against value obtained by plugin
 @SYMTestExpectedResults	Visible region match
*/
/**
 @SYMTestCaseID				GRAPHICS-WSERV-0378
 @SYMREQ					GT247-CR0714
 @SYMTestCaseDesc			Disable event notification
 @SYMTestPriority			Medium
 @SYMTestStatus				Implemented
 @SYMTestActions			Disable plugin to register to event notification.
							Action step:
							-Instruct plugin to unregister event handler
							-Destroy fullscreen window (plugin will not receive window visibility changed event)
							-Query visibility region from plugin side

 @SYMTestExpectedResults	Plugin does not receive events notification
*/
TestState CTWsGraphs::TestEventNotificationL()
	{
	if (iSubState==0)
		{
		_LIT(KTestEventNotification, "TestEventNotification");
		INFO_PRINTF1(KTestEventNotification);

		++iSubState;
		iListen->Enable(ETrue);
		CreateWindowL();
		Mem::FillZ(&iListenInfo, sizeof(TListenerInfo));
		TheClient->iWs.Finish();
		TheClient->WaitForRedrawsToFinish();
		iListen->QueryPlugin(iListenInfo);
		return EWait;
		}

	if (iSubState==1)
		{
		++iSubState;
		iListen->Enable(EFalse);
		TEST(iListenInfo.iNumRect==1);
		TEST(iListenInfo.iRect==TRect(TPoint(0,0),TheClient->iScreen->SizeInPixels()));
		DestroyWindowL();
		Mem::FillZ(&iListenInfo, sizeof(TListenerInfo));
		iListen->QueryPlugin(iListenInfo);
		iListen->Enable(EFalse);
		DestroyWindowL();
		return EWait;
		}
		
	TEST(iListenInfo.iNumRect==0);
	
	++(iTest->iState);
	iSubState = 0;
	return ENext;
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

TestState CTWsGraphs::TestRedirectionUsingWsBackBufferL()
	{
	if (TransparencySupportedL()==KErrNotSupported)
		{
		++(iTest->iState);
		return ENext;
		}

	// flush transparent window destruction created in TransparencySupportedL before
	// proceeding with the test

	TheClient->iWs.Flush();

	if (iSubState==0)
		{
		_LIT(KTestRedirectionWsBack, "TestRedirectionUsingWsBackBuffer");
		INFO_PRINTF1(KTestRedirectionWsBack);
		
		++iSubState;

		/**
		 @SYMTestCaseID				GRAPHICS-WSERV-0379
		 @SYMTestCaseDesc			Redirect wserv flickerfree to MWsBackBuffer object
		 @SYMTestPriority			Medium
		 @SYMTestStatus				Implemented
		 @SYMTestActions			-Draw opaque window (as background) and transparent window
									-Save screen content to a bitmap
									-Instruct plugin to redirect flickerfree buffer to MWsBackBuffer object
									-Draw the same opaque window and transparent window again
									-Retrieve plugin bitmap and compare against the saved bitmap
		 @SYMTestExpectedResults	Bitmap content match
		*/
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0379"));
		CreateWindowL();
		CreateAlphaWindowL();
		TheClient->iScreen->CopyScreenToBitmap(iBefore);
		DestroyAlphaWindowL();		
		DestroyWindowL();		
		iRedir->RedirectUsingWsBackBuffer(ETrue);
		return EWait;
		}

	if (iSubState==1)
		{
		++iSubState;
		CreateWindowL();
		CreateAlphaWindowL();
		TSize sz = iBefore->SizeInPixels();
		TInt bytes = sz.iWidth*sz.iHeight*2; // EColor64K
		iBefore->LockHeap();
		TInt ret=Mem::Compare((const TUint8*)iBackCopy->DataAddress(),bytes,(const TUint8*)iBefore->DataAddress(),bytes);
		TEST(ret==0);
		if (ret!=0)
			{
			_LIT(KLog,"The memory of two bitmaps doesn't match");
			LOG_MESSAGE(KLog);
			}
		iBefore->UnlockHeap();
		DestroyAlphaWindowL();
		DestroyWindowL();

		/**
		 @SYMTestCaseID				GRAPHICS-WSERV-0380
		 @SYMTestCaseDesc			Restore wserv flickerfree redirection from MWsBackBuffer object
		 @SYMTestPriority			Medium
		 @SYMTestStatus				Implemented
		 @SYMTestActions			-Instruct plugin to stop wserv filckerfree drawing redirection
									-Draw the same opaque and transparent window
									-Save screen content to another bitmap
									-Compare saved bitmap against newly saved bitmap
		 @SYMTestExpectedResults	Bitmap content match
		*/
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0380"));
		iRedir->RedirectUsingWsBackBuffer(EFalse);
		return EWait;
		}

	CreateWindowL();
	CreateAlphaWindowL();
	TheClient->iScreen->CopyScreenToBitmap(iAfter);
	TSize sz = iBefore->SizeInPixels();
	TInt bytes = sz.iWidth*sz.iHeight*2; // EColor64K
	iAfter->LockHeap();
	TEST(Mem::Compare((const TUint8*)iAfter->DataAddress(),bytes,(const TUint8*)iBefore->DataAddress(),bytes)==0);
	iAfter->UnlockHeap();
	DestroyAlphaWindowL();
	DestroyWindowL();
/**
	@SYMTestCaseID	GRAPHICS-WSERV-0527
*/
	((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0527"));
	++(iTest->iState);
	iSubState = 0;
	return ENext;
	}

#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

//A call to do coverage through a plugin. 
//This can serve as a basis for future coverage to objects exposed by the
//plugin. For now a single simple test is implmeneted.
TestState CTWsGraphs::TestGraphicDrawerCoverage()
	{
	__ASSERT_ALWAYS(iGdCoverage->RunTest(1)==KErrNone||KErrNotReady, User::Invariant());
	return ENext;
	}

//Simplified non-functional class to create a few dummy CWsGraphicDrawer instances.
//This code is intended to test the Array class, not the Drawer.
//Note that this object is not at all functional! The only guaranteed method is Id().
//WsGraphicDrawer is declared as a friend and is actually intended to be the factory class for CWsGraphicDrawer
class WsGraphicDrawer:public CWsGraphicDrawer
	{
public:
	WsGraphicDrawer()
		{
		}
	//stub for virtual construction. Most members are ignored
	virtual void ConstructL(MWsGraphicDrawerEnvironment& ,const TGraphicDrawerId& id,MWsClient& ,const TDesC8& )
		{
		ConstructL(id);
		}
	//simplified custom construction
	void ConstructL(const TGraphicDrawerId& id)
		{
		MWsGraphicDrawerEnvironment* nullEnv=NULL;
		MWsGraphicDrawerEnvironment& aEnv=*nullEnv;
		MWsClient* nullClient=NULL;
		MWsClient& aOwner=*nullClient;
		this->BaseConstructL(aEnv,id,aOwner);
		this->iDtor_ID_Key=TUid::Null();
		
		}
	//stubs for pure virtual methods
	virtual void HandleMessage(const TDesC8& )
		{}
	virtual void DoDraw(MWsGc& ,const TRect& ,const TDesC8& ) const
		{}
	
	};
//Class to allow me to pre-allocate the CWsGraphicDrawerArray so it doesn't pop up a false-positive memory allocation!
class DummyCleanup:public TCleanupItem
	{
public:
	static void CleanUp(TAny*)		{}
	DummyCleanup(): TCleanupItem(CleanUp,this)	{}
	operator DummyCleanup*()	{	return this;	}
	
	};

//Helper function to explain test fails. Most other tests are against KErrNone
void CTWsGraphs::ReportNegativeResultfail(TInt aLine,TInt aResult,TInt aExpectedResult)
	{
	testBooleanTrue((aResult==aExpectedResult), (TText8*)__FILE__, aLine);
	if (aResult!=aExpectedResult)
		{
		INFO_PRINTF3(_L("Expected return code %i, got %i"),aExpectedResult,aResult);	
		}
	
	}
	
//This is an attempt to use wserv test's pre-existing Panic handler to perform some negative tests.
//At present this handler appears to be broken:
// 1) It doesn't write to the correct html log file
// 2) It no longer writes to the WSERV.LOG file it was writing to a few versions ago
// 3) It doesn't close the panic window so subsequent tests that check the display output fail.
//That was a waste of effort.
struct CTWsGraphs::WrapTestCall
	{
	CTWsGraphs* thisThis;
	TUint		testCount;
	TBool		continueTests;
	TUint 		testFailedLine;
	//This field was intended to allow threaded panicing tests to report other errors
	//As I can't get threaded panicing tests to operate correctly, I have not implemented support for the field.
	//TBuf<1024>	errorMessages;	
		
	WrapTestCall (	CTWsGraphs* thisThis,	TUint		testCount):
		thisThis(thisThis),	testCount(testCount)
		{	continueTests=false;testFailedLine=0;	}
	};
	
TInt	CTWsGraphs::DoNegTestCall(TInt /*aInt*/, TAny *aPtr)
	{
	CTWsGraphs::WrapTestCall* aWrap=static_cast<CTWsGraphs::WrapTestCall*>(aPtr);
	aWrap->continueTests=aWrap->thisThis->NegTestAddSwapGDArrayL(aWrap->testCount,aWrap);	
	return 0;
	}
	
TBool	CTWsGraphs::LaunchNegTestCall(TUint		aTestCount,TUint PanicCode,const TDesC &aPanicCategory)
	{
	WrapTestCall wt(this,aTestCount);
	(void)PanicCode;
	(void)aPanicCategory;
//I have disabled the panicing tests because they don't output diagnostics 
//and the open panic window causes subsequent screen bitmap comparrisson tests to fail.
//	iTest->TestPanicL(DoNegTestCall,aPanicCode,3,&wt,aPanicCategory);
	return wt.continueTests;	
	}

/**
	Loops through all the positive and negative tests associated with the  GraphicDrawerArray.
	The aim is to perform isolated testing of these classes as some are not currently being used.
	
**/
void CTWsGraphs::TestAddSwapGDArrayL()
	{


	INFO_PRINTF1(_L("Positive tests for GraphicDrawerArray"));
	for (TInt i=0;PosTestAddSwapGDArrayL(i);i++)
		{}



	INFO_PRINTF1(_L("Verifying that negative tests for GraphicDrawerArray don't actually panic"));
	for (TInt i=1;NegTestAddSwapGDArrayL(i);i++)
		{}
	}
/**
	Resets and deallocates the GDA withoud deleting the objects.
	@param 	 testArray	the array to reset
	@return true if the array was already empty.
**/
static bool	ResetArray(CWsGraphicDrawerArray& testArray)
{
	bool rv=(testArray.IsEmpty());
	MWsClient* nullClient=NULL;
	testArray.RemoveAll(*nullClient);
	testArray.ResetAndDestroy();		
	return rv;
}
/**
   @SYMTestCaseID GRAPHICS-WSERV-AddSwapGDArray-0001
   @SYMDEF			DEF093926  
  
   @SYMTestCaseDesc DEF093926: Check for stability of Add and Swap unwind methods in isolation.
   		Note that this code is testing the functionality of a class internal to CWindowServer.
   		At present CWindowServer presents just a simple shim on this class, 
   		but if that implementation of  CWindowServer changes than this test will be redudant.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions
	The sequence for each of these positive test sections is pretty much the same:
	Add one record with id 1234(test for errors)
	Add one record with id Badf00d
	Swap with another record with id Badf00d
	Remove 2 records
	result=no leaks
	
		Add/Swap: no Cleanup item - no leaks after Remove the added items
   	    AddTLC/SwapTLC: Cleanup item requires Commit - check for no leaks after Remove.
   	    AddTLC/SwapTLC: Cleanup item gets executed by forced Leave  - check for no leaks after.
		AddTLC, SwapTLC in allocation failure scenarios. Add/Swap don't allocate anything!
		obsoleted AddLC/SwapLC to ensure correct function when forced Leave - check for no leaks after
		obsoleted AddLC/SwapLC to ensure correct function. These will always leak in good case.
   @SYMTestExpectedResults 
   		no exceptions or panics within this fn. 
   		only the old AddLC and SwapLC should leak as indicated.
 */
TBool CTWsGraphs::PosTestAddSwapGDArrayL(TInt testcase)
	{
	((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-AddSwapGDArray-0001"));
    CWsGraphicDrawerArray testArray;
    //Represents the memory cached in the array once it has been used even when it is then resized to zero
#if defined(_DEBUG)
    const TInt KArrayMemUseBaseline=1;
#endif
    //Use testArray.IsEmpty() to prove the array is clear when it should be!
    TGraphicDrawerId id1234=  	{  	1234,		EFalse    	};
    TGraphicDrawerId idBADF00D=	{  	0xBADF00D,	EFalse    	};
    WsGraphicDrawer dg1234;		
    dg1234.ConstructL(id1234);
    WsGraphicDrawer dgBADF00D;	
    dgBADF00D.ConstructL(idBADF00D);
    WsGraphicDrawer dgBADF00D_2;
    dgBADF00D_2.ConstructL(idBADF00D);
    CWsGraphicDrawerArray::XRollBackBase* rollBack1;
 	TInt errCode=KErrAbort;
	TInt leaveCode=KErrNone;
	DummyCleanup markerCleanup;
	CleanupStack::PushL(markerCleanup);	 //This allows me to check the stack is clear!
	//expected result of this fn: no exceptions or panics. 
	//Put as much as you like in here, so long as it shouldn't fail.
	
	__UHEAP_MARK;
			__UHEAP_CHECK(0);
	TUint expectedLeakCount=0;
	TBool returnCode=ETrue;
	switch (testcase)
		{
		case 0:

			INFO_PRINTF1(_L("Sub test P0: AddL/Swap: no Cleanup item"));
			TEST(testArray.IsEmpty());
				TRAP(leaveCode,errCode=testArray.Add(&dg1234));
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TEST(!testArray.IsEmpty());
				TRAP(leaveCode,errCode=testArray.Add(&dgBADF00D));
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
				TRAP(leaveCode,errCode=testArray.Swap(&dgBADF00D_2));
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
				TRAP(leaveCode,errCode=testArray.Remove(dg1234.Id()));			
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
				//Note that it is the ID that matters here... dgBADF00D & dgBADF00D_2 have same id.
				TRAP(leaveCode,errCode=testArray.Remove(dgBADF00D_2.Id()));			
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TEST(testArray.IsEmpty());
		break;
		//
		case 1:
			INFO_PRINTF1(_L("Sub test P1: AddTLC/SwapTLC: Cleanup item requires Commit."));
			
			rollBack1=NULL;
			TEST(testArray.IsEmpty());
			TRAP(leaveCode,
				rollBack1=testArray.AddTLC(&dg1234);
				CleanupStack::Check(rollBack1);
				testArray.CommitP(rollBack1);	
				)
			TEST(rollBack1!=NULL);
			TEST(leaveCode==KErrNone);
			TEST(!testArray.IsEmpty());
			rollBack1=NULL;
			TRAP(leaveCode,
				rollBack1=testArray.AddTLC(&dgBADF00D);
				CleanupStack::Check(rollBack1);
				testArray.CommitP(rollBack1);	
				)
			TEST(rollBack1!=NULL);
			TEST(leaveCode==KErrNone);
			rollBack1=NULL;
			TRAP(leaveCode,
				rollBack1=testArray.SwapTLC(&dgBADF00D_2);
				CleanupStack::Check(rollBack1);
				testArray.CommitP(rollBack1);	
				)
			TEST(rollBack1!=NULL);
			TEST(leaveCode==KErrNone);
			rollBack1=NULL;
			TRAP(leaveCode,
				rollBack1=testArray.RemoveTLC(dg1234.Id());
				CleanupStack::Check(rollBack1);
				testArray.CommitP(rollBack1);	
				)
			TEST(rollBack1!=NULL);
			TEST(leaveCode==KErrNone);
			rollBack1=NULL;
				TRAP(leaveCode,
				rollBack1=testArray.RemoveTLC(dgBADF00D_2.Id());			
				CleanupStack::Check(rollBack1);
				testArray.CommitP(rollBack1);	
				)
			TEST(rollBack1!=NULL);
			TEST(leaveCode==KErrNone);
			rollBack1=NULL;
			TEST(testArray.IsEmpty());
		break;
		//
		case 2:
			INFO_PRINTF1(_L("Sub test P2: AddTLC/SwapTLC: Cleanup item gets executed."));
			
			rollBack1=NULL;
			TEST(testArray.IsEmpty());
			TRAP(leaveCode,
				rollBack1=testArray.AddTLC(&dg1234);
				User::Leave(1234);
				);	
			TEST(rollBack1!=NULL);
			TEST(leaveCode==1234);
			TEST(testArray.IsEmpty());
			rollBack1=NULL;
			CleanupStack::Check(markerCleanup);
			__UHEAP_CHECK(KArrayMemUseBaseline);

			TRAP(leaveCode,	errCode=testArray.Add(&dgBADF00D));
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			rollBack1=NULL;
			TRAP(leaveCode,
				rollBack1=testArray.SwapTLC(&dgBADF00D_2);
				User::Leave(1234);
				);	
			TEST(rollBack1!=NULL);
			TEST(leaveCode==1234);
			TEST(!testArray.IsEmpty());
			rollBack1=NULL;
			TRAP(leaveCode,
				rollBack1=testArray.RemoveTLC(dgBADF00D_2.Id());
				TEST(testArray.IsEmpty());
				User::Leave(1234);
				);	
			TEST(rollBack1!=NULL);
			TEST(leaveCode==1234);
			TEST(!testArray.IsEmpty());
			
			
				TRAP(leaveCode,errCode=testArray.Remove(dgBADF00D_2.Id()));			
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TEST(testArray.IsEmpty());
			rollBack1=NULL;
		break;
		//
		case 3:
			INFO_PRINTF1(_L("Sub test P3: AddLC/SwapLC: Cleanup item gets executed - doesn't leak"));
			TEST(testArray.IsEmpty());
			TRAP(leaveCode,
				testArray.AddLC(&dg1234);
				User::Leave(1234);
				);	
			TEST(leaveCode==1234);
			TEST(testArray.IsEmpty());
			rollBack1=NULL;
		__UHEAP_CHECK(KArrayMemUseBaseline);	  //because it threw it didn't leak
			
			//use my new method to add the object to be swapped out so no leak
			TRAP(leaveCode,errCode=testArray.Add(&dgBADF00D));
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
		__UHEAP_CHECK(KArrayMemUseBaseline);	  //new method doesn't leak.
			rollBack1=NULL;
			TRAP(leaveCode,
				errCode=testArray.SwapLC(&dgBADF00D_2);
				User::Leave(1234);
				);	
			TEST(errCode==KErrNone);
			TEST(leaveCode==1234);
			TEST(!testArray.IsEmpty());
			
				TRAP(leaveCode,errCode=testArray.Remove(dgBADF00D_2.Id()));			
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TEST(testArray.IsEmpty());
			rollBack1=NULL;
		break;
		//
		case 4:
	
		//I don't really care whether the individual calls succeed or fail, 
		//just whether it leaks overall, and that the error codes correspond to no-action
		for (TInt faultRate=1;faultRate<5;faultRate++)
			{
			INFO_PRINTF2(_L("Sub test P4: Add/Swap: memory faulting %i"),faultRate);
			__UHEAP_SETFAIL(RAllocator::EDeterministic,faultRate);
			TInt err1=KErrNone;
			rollBack1=NULL;
			TRAP(leaveCode,
				errCode=testArray.Add(&dg1234);
				)
			err1=errCode;
			__UHEAP_SETFAIL(RAllocator::ENone,0);
			TRAP(leaveCode,errCode=testArray.Add(&dgBADF00D));
			__UHEAP_SETFAIL(RAllocator::EDeterministic,faultRate);
			rollBack1=NULL;
			TRAP(leaveCode,
				errCode=testArray.Swap(&dgBADF00D_2);
				)
			__UHEAP_SETFAIL(RAllocator::ENone,0);
			//If the first Add fails then the object should not be removed
			if (!err1)
				{
				TRAP(leaveCode,errCode=testArray.Remove(dg1234.Id()));			
				TEST(errCode==KErrNone);
				TEST(leaveCode==KErrNone);
				}
			//If the swap fails, then the add still needs to be removed
			//Note that it is the ID that matters here... dgBADF00D & dgBADF00D_2 have same id.
			TRAP(leaveCode,errCode=testArray.Remove(dgBADF00D_2.Id()));			
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TEST(testArray.IsEmpty());
			ResetArray(testArray);
			CleanupStack::Check(markerCleanup);
			__UHEAP_CHECK(0);
			}
		break;
		//
		case 5:
				
		//I don't really care whether the individual calls succeed or fail, 
		//just whether it leaks overall, and that the error codes correspond to no-action
		for (TInt faultRate=1;faultRate<5;faultRate++)
			{
			INFO_PRINTF2(_L("Sub test P5: AddTLC/SwapTLC: memory faulting %i"),faultRate);
			__UHEAP_SETFAIL(RAllocator::EDeterministic,faultRate);
			TInt err1=KErrNone,err2=KErrNone,err3=KErrNone;
			rollBack1=NULL;
			TRAP(leaveCode,
				rollBack1=testArray.AddTLC(&dg1234);
				CleanupStack::Check(rollBack1);
				testArray.CommitP(rollBack1);	
				)
			err1=leaveCode;
			__UHEAP_SETFAIL(RAllocator::ENone,0);
			TRAP(leaveCode,errCode=testArray.Add(&dgBADF00D));
			__UHEAP_SETFAIL(RAllocator::EDeterministic,faultRate);
			rollBack1=NULL;
			TRAP(leaveCode,
				rollBack1=testArray.SwapTLC(&dgBADF00D_2);
				CleanupStack::Check(rollBack1);
				testArray.CommitP(rollBack1);	
				)
			//If the first Add fails then the object should not be removed
			if (!err1)
				{
				TRAP(leaveCode,
					rollBack1=testArray.RemoveTLC(dg1234.Id());			
					CleanupStack::Check(rollBack1);
					testArray.CommitP(rollBack1);	
					)
				err2=leaveCode;
				}
			//If the swap fails, then the add still needs to be removed
			//Note that it is the ID that matters here... dgBADF00D & dgBADF00D_2 have same id.
			TRAP(leaveCode,
					rollBack1=testArray.RemoveTLC(dgBADF00D_2.Id());
					CleanupStack::Check(rollBack1);
					testArray.CommitP(rollBack1);	
					)
			err3=leaveCode;
			
			
			__UHEAP_SETFAIL(RAllocator::ENone,0);
			//If the Removes failed then the object should be removed again
			if (err2)
				{
				TRAP(leaveCode,errCode=testArray.Remove(dg1234.Id()));			
				TEST(errCode==KErrNone);
				TEST(leaveCode==KErrNone);
				}
			if (err3)
				{
				TRAP(leaveCode,errCode=testArray.Remove(dgBADF00D_2.Id()));			
				TEST(errCode==KErrNone);
				TEST(leaveCode==KErrNone);
				}
			TEST(testArray.IsEmpty());
			ResetArray(testArray);
			CleanupStack::Check(markerCleanup);
			__UHEAP_CHECK(0);
			}
				
		break;
		//
		case 6:
	//this set does leak:
	
		INFO_PRINTF1(_L("Sub test P6: AddLC/SwapLC: Cleanup item gets popped - unfixable leaks"));
			TEST(testArray.IsEmpty());
			TRAP(leaveCode,
				testArray.AddLC(&dg1234);
				CleanupStack::Pop();
				);	
			TEST(leaveCode==KErrNone);
			TEST(!testArray.IsEmpty());
		
		CleanupStack::Check(markerCleanup);
		__UHEAP_CHECK(KArrayMemUseBaseline+1);	
			
			TRAP(leaveCode,
				testArray.AddLC(&dgBADF00D);
				CleanupStack::Pop();
				)
			TEST(leaveCode==KErrNone);
		CleanupStack::Check(markerCleanup);
		__UHEAP_CHECK(KArrayMemUseBaseline+2);	
			rollBack1=NULL;
			TRAP(leaveCode,
				errCode=testArray.SwapLC(&dgBADF00D_2);
				CleanupStack::Pop();
				);	
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TEST(!testArray.IsEmpty());
				TRAP(leaveCode,errCode=testArray.Remove(dg1234.Id()));			
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
				TRAP(leaveCode,errCode=testArray.Remove(dgBADF00D_2.Id()));			
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TEST(testArray.IsEmpty());
			rollBack1=NULL;
			expectedLeakCount=3;
		break;
		//
		case 7:
			returnCode=EFalse;
		break;
		}
	ResetArray(testArray);
	CleanupStack::Check(markerCleanup);
	__UHEAP_CHECK(expectedLeakCount);
	__UHEAP_MARKENDC(expectedLeakCount);
	if (expectedLeakCount!=0)
		{	//Ensure that the leaked items are no longer associated with this debug level.
			//Note that __DbgSetAllocFail(FALSE,RAllocator::EReset,1) resets the debug level to 0, 
			//so levels can't be nested when using this call.
		__UHEAP_MARK;			
	    __UHEAP_TOTAL_RESET;	
		INFO_PRINTF2(_L("Anticipated %i leaks declassified"),expectedLeakCount);	//can't get here if wrong
		}
			
	CleanupStack::PopAndDestroy(markerCleanup);
	((CTWsGraphsStep*)iStep)->RecordTestResultL();
	return returnCode;
	}

/**
	@param 	failcase		index to test to perform
	@param	aWrappedParams  represents inter-thread information when test is run on a private thread
				if aWrappedParams is NULL then the test is running on the main thread.
	@return true if there are higher-numbered fail cases.

   @SYMTestCaseID GRAPHICS-WSERV-NegAddSwapGDArray-0001
   @SYMDEF			DEF093926  
  
   @SYMTestCaseDesc DEF093926: Check for stability of Add and Swap unwind methods in isolation,
   					specifically checking that bad inputs are rejected gracefully.
   		Note that this code is testing the functionality of a class internal to CWindowServer.
   		At present CWindowServer presents just a simple shim on this class, 
   		but if that implementation of  CWindowServer changes than this test will be redudant.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions
   
		Add/Swap: no Cleanup item - no leaks after Remove the added items
   	    AddTLC/SwapTLC: Cleanup item requires Commit - check for no leaks after Remove.
   	    AddTLC/SwapTLC: Cleanup item gets executed by forced Leave  - check for no leaks after.
		AddTLC, SwapTLC in allocation failure scenarios. Add/Swap don't allocate anything!
		obsoleted AddLC/SwapLC to ensure correct function when forced Leave - check for no leaks after
		obsoleted AddLC/SwapLC to ensure correct function. These will always leak in good case.
	Calls NegTestAddSwapGDArrayL.	
	case 1/2/3: Tests AddL, AddLC, AddTLC that a NULL input is rejected
	case 4/5/6: Tests SwapL, SwapLC, SwapTLC that a NULL input is rejected
	case 7/8/9: Tests AddL, AddLC, AddTLC that a repeat input is rejected
	case 10/11/12: Tests SwapL, SwapLC, SwapTLC that a non-repeat input is rejected
   @SYMTestExpectedResults 
 */
TBool CTWsGraphs::NegTestAddSwapGDArrayL(TInt failcase,WrapTestCall*aWrappedParams)
	{
	((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-NegAddSwapGDArray-0001"));
	_LIT(KCategory,"WsGraphicDrawer");
	if (!aWrappedParams)
		{
		INFO_PRINTF2(_L("NegTestAddSwapGDArrayL Negative sub test %i"),failcase);
		};
    CWsGraphicDrawerArray testArray;
    TGraphicDrawerId id1234=  	{  	1234,		EFalse    	};
    TGraphicDrawerId idBADF00D=	{  	0xBADF00D,	EFalse    	};
    WsGraphicDrawer dg1234;		
    dg1234.ConstructL(id1234);
    WsGraphicDrawer dgBADF00D;	
    dgBADF00D.ConstructL(idBADF00D);
    WsGraphicDrawer dgBADF00D_2;
    dgBADF00D_2.ConstructL(idBADF00D);
    CWsGraphicDrawerArray::XRollBackBase* rollBack1=NULL;
 	TInt errCode=KErrAbort;
	TInt leaveCode=KErrNone;
	TBool	returnMoreTests=ETrue;
	DummyCleanup markerCleanup;
	TBool mayPanic=EFalse;
#ifdef __WINS__
	mayPanic=ETrue;
#endif

	CleanupStack::PushL(markerCleanup);	 //This allows me to check the stack is clear!
	__UHEAP_MARK;
	
	switch (failcase)
		{
		case 1:	//NULL arg: expected result:  returns KErrArgument
	    	TRAP(leaveCode,
	    		errCode=testArray.Add(NULL);
	    		)
			TEST(leaveCode==KErrNone);
	    	ReportNegativeResultfail(__LINE__,errCode,KErrArgument);
	    break;
		case 2:	//NULL arg: expected result: throws KErrArgument
			TRAP(leaveCode,	
				rollBack1=testArray.AddTLC(NULL);
				TEST(EFalse);	//Should never get here!
				)
			TEST(rollBack1==NULL);
	    	ReportNegativeResultfail(__LINE__,leaveCode,KErrArgument);
	    break;
		case 3:	//NULL arg: expected result: debug: panic. In release doesn't return any information!
			if (!aWrappedParams && mayPanic)
				{
			    LaunchNegTestCall(failcase,EWsGraphicDrawerPanicBadArgument,KCategory);
				}
			else
				{
				TRAP(leaveCode,
					testArray.AddLC(NULL);
					User::Leave(1234);  //Panics before here in debug. No leak if cleanup is taken.
					);	
				TEST(leaveCode==1234);	//Panics before here in debug
				}
	    break;
		//
		case 4:	//NULL arg: expected result: returns KErrArgument
	    	TRAP(leaveCode,
	    		errCode=testArray.Swap(NULL)
	    		)
			TEST(leaveCode==KErrNone);
	    	ReportNegativeResultfail(__LINE__,errCode,KErrArgument);
	    break;
		case 5:	//expected result:  throws KErrArgument
			TRAP(leaveCode,
				rollBack1=testArray.SwapTLC(NULL);
				testArray.CommitP(rollBack1);
				)
			TEST(rollBack1==NULL);
	    	ReportNegativeResultfail(__LINE__,leaveCode,KErrArgument);
	    break;
		case 6:	//NULL arg: expected result: debug: panic. In release doesn't return any information!
			if (!aWrappedParams && mayPanic)
				{
			    LaunchNegTestCall(failcase,EWsGraphicDrawerPanicBadArgument,KCategory);
				}
			else
				{
				TRAP(leaveCode,
					errCode=testArray.SwapLC(NULL);
					User::Leave(1234);  //Panics before here in debug. No leak if cleanup is taken.
					);
				TEST(leaveCode==1234);	//Panics before here in debug
				TEST(errCode==KErrNotFound);
				}
		break;
		//
		case 7:	//Add overwrites: expected result: returns KErrAlreadyExists
	    	TRAP(leaveCode,
	    		errCode=testArray.Add(&dg1234);
	    		)
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
	    	TRAP(leaveCode,
	    		errCode=testArray.Add(&dg1234);		//oops! Already added!
	    		)
			TEST(leaveCode==KErrNone);
	    	ReportNegativeResultfail(__LINE__,errCode,KErrAlreadyExists);
	    break;
		case 8:	//Add overwrites: expected result:  throws KErrAlreadyExists
	    	TRAP(leaveCode,
	    		errCode=testArray.Add(&dg1234);
	    		)
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TRAP(leaveCode,	
				rollBack1=testArray.AddTLC(&dg1234);		//oops! Already added!
				testArray.CommitP(rollBack1);
				)
			TEST(rollBack1==NULL);
	    	ReportNegativeResultfail(__LINE__,leaveCode,KErrAlreadyExists);
	    break;
		case 9:	//Add overwrites: expected result: debug: does not panic, but throws KErrAlreadyExists.
	    	TRAP(leaveCode,
	    		errCode=testArray.Add(&dg1234);
	    		)
			TEST(errCode==KErrNone);
			TEST(leaveCode==KErrNone);
			TRAP(leaveCode,
					testArray.AddLC(&dg1234);		//oops! Already added! Should leave.
					User::Leave(1234);  //Should leave before here! No leak if cleanup is taken.
				);	
	    	ReportNegativeResultfail(__LINE__,leaveCode,KErrAlreadyExists);
	    break;
		//
		case 10:	//Swap empty slot: expected result:  returns KErrNotFound
	    	TRAP(leaveCode,
	    		errCode=testArray.Swap(&dg1234) 		//oops! Nothing to swap with!
	    		)
			TEST(leaveCode==KErrNone);
	    	ReportNegativeResultfail(__LINE__,errCode,KErrNotFound);
	    break;
		case 11:	//Swap empty slot: expected result: throws KErrNotFound
			TRAP(leaveCode,
				rollBack1=testArray.SwapTLC(&dg1234);		//oops! Nothing to swap with!
				testArray.CommitP(rollBack1);
				)
			TEST(rollBack1==NULL);
	    	ReportNegativeResultfail(__LINE__,leaveCode,KErrNotFound);
	    break;
		case 12:	//Swap empty slot: expected result: debug: panic. In release doesn't return any information!
			if (!aWrappedParams && mayPanic)
				{
			    LaunchNegTestCall(failcase,EWsGraphicDrawerPanicBadArgument,KCategory);
				}
			else
				{
				TRAP(leaveCode,
					errCode=testArray.SwapLC(&dg1234);		//oops! Nothing to swap with!
					User::Leave(1234);  //Panics before here in debug. No leak if cleanup is taken.
					);	
				TEST(leaveCode==1234);			//Panics before here in debug
				TEST(errCode==KErrNotFound);	//Panics before here in debug
				}
	    break;
	    
		//
		default:
			returnMoreTests=EFalse;
		}
	ResetArray(testArray);
	CleanupStack::Check(markerCleanup);
	__UHEAP_CHECK(0);
	__UHEAP_MARKENDC(0);
    testArray.Close();
//    CWsGraphicDrawerArray::testArrayValidator::ResetArray(&testArray);
	CleanupStack::PopAndDestroy(markerCleanup);
	return returnMoreTests;
	}

/**
   @SYMTestCaseID 	GRAPHICS-WSERV-LeakInService-0001
   @SYMDEF			DEF093926  
   @SYMTestCaseDesc Check for leaks over repeated re-assignments.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions 
   Repeatedly create the same bitmap instance
   After a few initial wobbles in the server-side HeapCount it should not increase
   5 calls are made without checking the level, then 5 more check the level.
   Note that as we are testing the main server heap, 
   other threads may interrupt and perform operations that change the memory figures.
   
   @SYMTestExpectedResults The CWsGraphicBitmap objects are created and no leaks are reported.
 */	
void CTWsGraphs::DoTestLeakInServiceL()
	{
		INFO_PRINTF1(_L("DoTestLeakInServiceL"));
		const TInt prepCount=5;
		const TInt testCount=5;
		
				
		TUid uid1 = {0x10000001};
		TUid uid2 = {0x10000002};
		 
		TWsGraphicId twsGraphicId1(uid1);
		TEST(twsGraphicId1.Uid()==uid1);
		
		TWsGraphicId twsGraphicId2(uid2);
		TEST(twsGraphicId2.Uid()==uid2);
		
		TWsGraphicId twsGraphicId3(twsGraphicId2);
		TEST(twsGraphicId3.Uid()==uid2);
		
		TWsGraphicId twsGraphicId4(1);
		twsGraphicId4.Set(uid1);
		TEST(twsGraphicId4.Uid()==uid1);		

		TSize screenSize = TheClient->iScreen->SizeInPixels();
		
		__UHEAP_RESET;
		__UHEAP_MARK;
		// Create local shared CWsGraphicBitmap	
		// Repeat operation for any sign of memory leak...	
		CFbsBitmap bitmap2;
		CFbsBitmap mask2;		
		
		bitmap2.Create(screenSize,TheClient->iScreen->DisplayMode());
		mask2.Create(bitmap2.SizeInPixels(),TheClient->iScreen->DisplayMode());
			
		TInt c0=TheClient->iWs.HeapCount();
		CWsGraphicBitmap* bTestX = CWsGraphicBitmap::NewL(twsGraphicId2.Uid(), &bitmap2,&mask2);		
		for (TInt i=0;i<prepCount;i++)
			{
			//TInt c2=TheClient->iWs.HeapCount();
			delete bTestX; 								
			//TInt c3=TheClient->iWs.HeapCount();
			bTestX = CWsGraphicBitmap::NewL(twsGraphicId2.Uid(), &bitmap2,&mask2);		
			//TInt c4=TheClient->iWs.HeapCount();
			}
		// Give WSERV a chance to settle.
		TheClient->iWs.Finish();
		User::After (1000000); //1s
		
		TInt c1=TheClient->iWs.HeapCount();
		TInt failures=0;
		for (TInt i=0;i<testCount;i++)
			{
			TInt c2=TheClient->iWs.HeapCount();
			delete bTestX;
			//TInt c3=TheClient->iWs.HeapCount();
			//The heap count doesn't go down after delete operation
			//because the delete message is buffered by the server, because it does not have a return value.
			//Aparrently, although CWsGraphicBitmap and TheClient terminate at the same server,
			//and use the same general heap (I have tested this under debug),
			//they do not share the same session, so flushing TheClient does not effect CWsGraphicBitmap
			bTestX = CWsGraphicBitmap::NewL(twsGraphicId2.Uid(), &bitmap2,&mask2);		
			
			// Give WSERV a chance to settle.
			TheClient->iWs.Finish();
			User::After (1000000); //1s

			TInt c4=TheClient->iWs.HeapCount();   
			//Can compare immediately after allocation as the server doesn't buffer the create command.
			if (!(c2==c4))
				{
				if (c4 > c2) // only fail the test if the count has increased
					{
					failures++;
					}
				INFO_PRINTF2(_L("Server Heap count change accross delete/new = %i"),c4-c2);
				}
			}
		// Outside of main loop to avoid client/wserv interaction during test.
		TEST(failures==0);
		TInt c5=TheClient->iWs.HeapCount();
		TEST((c1-c5)/testCount==0);	   //If every call leaked.
		if ((c1-c5)/testCount)
			{
			INFO_PRINTF3(_L("Server Heap count change accross %i delete/new cycles = %i"),testCount,c5-c1);
			INFO_PRINTF3(_L("Before %i / After %i"),c1,c5);
			}
		
		delete bTestX; 
		
		__UHEAP_CHECK(0);
		__UHEAP_MARKEND;
		
	}
/**
 @SYMTestCaseID				GRAPHICS-WSERV-0381 
 @SYMDEF					DEF095063
 @SYMTestCaseDesc			Test case for INC098114 CWsGraphicDrawer::SendMessage panics window server
 @SYMTestPriority			Medium
 @SYMTestStatus				Implemented
 @SYMTestActions			Enable the test flag and reproduce the defect
							Action step:
							-Instruct plugin to register event handler
							-Draw fullscreen window (plugin will receive window visibility changed event)
							-Set the test flag 
							-Query visibility region from plugin side
							-Instruct plugin to unregister event handler
							-Destroy fullscreen window (plugin will not receive window visibility changed event)
						
 @SYMTestExpectedResults	wserv should not panic with the fix
*/	

TestState CTWsGraphs::TestSuccessiveMessageL()
	{
	if (iSubState==0)
		{
		_LIT(KTestSuccessiveMessage, "TestSuccessiveMessage");
		INFO_PRINTF1(KTestSuccessiveMessage);

		++iSubState;
		iListen->Enable(ETrue);
		CreateWindowL();
		Mem::FillZ(&iListenInfo, sizeof(TListenerInfo));
		//Set the test flag to enable the reproduction of defect
		iListen->SetTestFlag();
		iListen->QueryPlugin(iListenInfo);
		iListen->Enable(EFalse);
		DestroyWindowL();
		return EWait;
		}

	++(iTest->iState);
	iSubState = 0;
	return ENext;
	}

TestState CTWsGraphs::TestWindowGroupChangeL()
	{
	if (iSubState==0)
		{
		_LIT(KTestWindowGroupChange, "TestWindowGroupChange");
		INFO_PRINTF1(KTestWindowGroupChange);

		++iSubState;
		iListen->Enable(ETrue);
		CreateWindowL();
		Mem::FillZ(&iListenInfo, sizeof(TListenerInfo));
		iListen->QueryPlugin(iListenInfo);
		return EWait;
		}

	if (iSubState==1)
		{
		++iSubState;
		iOriginalWindowGroupId = iListenInfo.iWindowGroupId;
		iNewWin = CCrWin::NewL(iTest->iScreenNumber, ETrue);
		iListen->QueryPlugin(iListenInfo);
		return EWait;
		}

	TEST(iListenInfo.iWindowGroupId != iOriginalWindowGroupId);
	
	delete iNewWin;
	iNewWin = NULL;
	iListen->Enable(EFalse);
	DestroyWindowL();
	++(iTest->iState);
	iSubState = 0;
	return ENext;
	}

/**
 @SYMTestCaseID				GRAPHICS-WSERV-0382
 @SYMDEF 					INC085451
 @SYMTestCaseDesc			Test Animation frame rate
 @SYMTestPriority			Medium
 @SYMTestStatus				Implemented
 @SYMTestActions			Simulate an Animation artwork by calling DrawWsGraphic and DoDraw.
							Action step:
							-Calls DrawWsGraphic for two different animations with different frame rates
							-Retrieve the frame count for two differnt frames per seconds
							-Test the two frame rate shouldn't be same.

 @SYMTestExpectedResults	Animation Frame rate should be different for different frames per second
*/
TestState CTWsGraphs::TestFrameRateL()
	{
	//Check for Transparency enabled in wsini.ini
	if (TransparencySupportedL()==KErrNotSupported)
		{
		++(iTest->iState);
		return ENext;
		}

	// flush transparent window destruction created in TransparencySupportedL before
	// proceeding with the test
	TheClient->iWs.Flush();

	//Here 	iSubState is 0, when this functions executes first time 
	//		iSubState is 1 means the call is from a callback function.
	if(iSubState == 0)
		{
		_LIT(KTestFrameRate, "TestFrameRate");
		INFO_PRINTF1(KTestFrameRate);

		++iSubState;
		iTestframerate = CGraphicTestFrameRate::NewL(iTest->iScreenNumber);

		//Set the callback function
		iTestframerate->SetCallBack(TCallBack(CTWsGraphs::PluginCallBack,this));

		//Create the window and call the graphic animation
		CreateWindowL(EFalse);
		iWin->DrawGraphic(iTestframerate->Id().Id());
		return EWait;
		}

	//PeterI wait a while for animations to redraw then query the plugin
	User::After(2000000);
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();

	//Invoke the plug-in and get the counter value
	Mem::FillZ(&iAnimCount, sizeof(TAnimRate));

	iTestframerate->QueryPlugin(iAnimCount);
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();

	//Compare and test the total frame rate for two different frame counts....
	TEST((iAnimCount.iAnim1>0 && iAnimCount.iAnim2>0) && iAnimCount.iAnim1 !=iAnimCount.iAnim2);
	if(iSubState == 1)
		{
		DestroyWindowL();
		delete iTestframerate;
		}
	++(iTest->iState);
	iSubState = 0;
	return ENext;
	}

/**
 @SYMTestCaseID				GRAPHICS-WSERV-0438 
 @SYMDEF					INC103472
 @SYMTestCaseDesc			CRedrawRegion::ContainsDrawers does not look for all drawers 
 @SYMTestPriority			Medium
 @SYMTestStatus				Implemented
 @SYMTestActions			Create ECom-plugins to enable the scenario, in which there is one simple and one container drawer. 
 							In the container drawer, two other simple drawers are enabled. Each drawer draws a coloured ellipse.
 							In this case, when the contained drawer is requested to change the colour of the ellipse, the screen will not be updated
 							without the fix.  
 							Action step:
							-Create four CRP graphics.
							-Call the simple drawer and container drawer to draw ellipses. The container drawer
							 also enables two other simple drawers to draw ellipses in different colors.
							-Update the color of each ellipse in turn. 
							-Check that the screen display is as required.						
 @SYMTestExpectedResults	The colour of each ellipse residing in the drawers is successfully updated. Those for the contained drawers wont be updated without the fix. 
*/	
void CTWsGraphs::TestNestedDrawerCRP()
	{
	_LIT(KTestContainDrawer, "Test INC103472: A Contained Drawer");
	INFO_PRINTF1(KTestContainDrawer);

	RWindow window1(TheClient->iWs);
	CleanupClosePushL(window1);
	User::LeaveIfError(window1.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	
	window1.EnableRedrawStore(ETrue);	// Force to enable the redraw storing
	window1.SetRequiredDisplayMode(EColor256);	 // Do not set window size here to avoid hardware test failure
	window1.SetBackgroundColor(KRgbDarkGreen);
	window1.Activate();
	
	// A simple graphic
	CWsSimpleGraphicBitmap* wsGraphic1 = CWsSimpleGraphicBitmap::NewL(KSimpleDrawerInterfaceId);
	CleanupStack::PushL(wsGraphic1);
	// A container graphic
	CWsContainGraphicBitmap* wsGraphic2 = CWsContainGraphicBitmap::NewL(KContainDrawerInterfaceId);
	CleanupStack::PushL(wsGraphic2);
	// A contained graphic residing in the container graphic wsGraphic2 
 	CWsSimpleGraphicBitmap* wsGraphic3 = CWsInvisibleGraphicBitmap1::NewL(KInvisibleDrawerInterfaceId1);
 	CleanupStack::PushL(wsGraphic3);
	// A contained graphic residing in the container graphic wsGraphic2
 	CWsSimpleGraphicBitmap* wsGraphic4 = CWsInvisibleGraphicBitmap2::NewL(KInvisibleDrawerInterfaceId2);
 	CleanupStack::PushL(wsGraphic4);

    window1.Invalidate();
	window1.BeginRedraw();
	TheClient->iGc->Activate(window1);
	TheClient->iGc->Clear();
	// Call CRP drawer to draw the coloured ellipses	
	TheClient->iGc->DrawWsGraphic(wsGraphic1->Id(),TRect(TPoint(20,20),TSize(300,100)));
	TheClient->iGc->DrawWsGraphic(wsGraphic2->Id(),TRect(TPoint(20,100),TSize(300,100)));	

    TheClient->iGc->Deactivate();
	window1.EndRedraw();
	TheClient->Flush();			   
	
	// Update the colour of four ellipses residing in four CRP drawers. 
	TInt err = wsGraphic1->UpdateColor(KRgbRed);
	TEST(KErrNone == err);
	err = wsGraphic2->UpdateColor(KRgbDarkBlue);
	TEST(KErrNone == err);
	// If the fix is not inserted, the colour of the third and fourth ellipses residing in the contained drawers wont be updated
	err = wsGraphic3->UpdateColor(KRgbDarkMagenta);
	TEST(KErrNone == err);
	err = wsGraphic4->UpdateColor(KRgbDarkCyan);	  //won't change the displayed color if there is a right place for flush()
	TEST(KErrNone == err);
	
	TheClient->Flush();
 	// Force some delays to wait until the color change
    User::After(2000000);
    // Test whether the screen content is changed as required
    CheckResult();
   	CleanupStack::PopAndDestroy(5,&window1);
	}

// Check the screen display with the reference bitmap to ensure the color to be updated correctly
void CTWsGraphs::CheckResult()
	{
    TSize size = TSize(320,200);//The maximum size of the screen content we are looking at

    // Create a reference bitmap
    CFbsBitmap* bitmapRef = new(ELeave) CFbsBitmap;
   	CleanupStack::PushL(bitmapRef);
	User::LeaveIfError(bitmapRef->Create(size, EColor256));
	CFbsBitGc* gc;
	CFbsBitmapDevice* bitmapDev = CFbsBitmapDevice::NewL(bitmapRef);
	TEST(bitmapDev!=NULL);
   	CleanupStack::PushL(bitmapDev);
	User::LeaveIfError(bitmapDev->CreateContext(gc));
   	CleanupStack::PushL(gc);
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetBrushColor(KRgbDarkGreen);
	gc->Clear(TRect(TPoint(0,0), size));//background dark green
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc->SetBrushColor(KRgbRed);
	gc->DrawEllipse(TRect(TPoint(20,20),TSize(300,100)));  //map to the simple drawer
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetBrushColor(KRgbDarkBlue);
	gc->DrawEllipse(TRect(TPoint(20,100),TSize(300,100)));	//map to the container drawer
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetBrushColor(KRgbDarkMagenta);
	gc->DrawEllipse(TRect(TPoint(100,150),TSize(50,50)));	//map to the contained drawer
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
 	gc->SetBrushColor(KRgbDarkCyan);
	gc->DrawEllipse(TRect(TPoint(200,150),TSize(50,50)));	//map to the contained drawer
    TInt bitmapHeight = bitmapRef->SizeInPixels().iHeight;
	TInt bitmapWidth = bitmapRef->SizeInPixels().iWidth;
  
   	// Copy the screen content to bitmap
	INFO_PRINTF1(_L("Capture screen content."));
	CFbsBitmap* screenBitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(screenBitmap);
	User::LeaveIfError(screenBitmap->Create(size, TheClient->iScreen->DisplayMode()));
	TRect rct = TRect(TPoint(0,0), size);
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(screenBitmap,rct));

	// Compare the displayed bitmap against the reference one
	INFO_PRINTF1(_L("Compare the displayed bitmap against the expected one."));
	TInt lineLength=bitmapRef->ScanLineLength(bitmapWidth, EColor256);
	HBufC8* compareLineBuf=HBufC8::NewLC(lineLength);
	TPtr8 compareLinePtr(compareLineBuf->Des());
	HBufC8* screenLineBuf=HBufC8::NewLC(lineLength);
	TPtr8 screenLinePtr(screenLineBuf->Des());
	for (TInt index=0; index<bitmapHeight; index++)
		{
		bitmapRef->GetScanLine(compareLinePtr, TPoint(0,index), bitmapWidth, EColor256);
		screenBitmap->GetScanLine(screenLinePtr, TPoint(0,index),bitmapWidth, EColor256);
		TInt compareResult=compareLinePtr.Compare(screenLinePtr);
		if (compareResult!=0)
			{
			INFO_PRINTF2(_L("Scanline compare failed: %d"),index);
			TEST(EFalse);
			break;
			}
		}
	CleanupStack::PopAndDestroy(6,bitmapRef);  

	} 

void ResetScreenMode(TAny* aAny)
	{
	CWsScreenDevice* screen=static_cast<CWsScreenDevice*>(aAny);
	screen->SetScreenMode(0);
	screen->SetAppScreenMode(0);
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

void CTWsGraphs::TestScreenModeChangeL()
	{
	_LIT(KLog,"Screen Doesn't match bitmap.  Size=(%d,%d) winSizeMode=%d redrawMode=%d modeIndex=%d");
// A simple CRP graphic to draw in the test
	CWsSimpleGraphicBitmap* wsGraphic1=CWsSimpleGraphicBitmap::NewL(KSimpleDrawerInterfaceId);
	CleanupStack::PushL(wsGraphic1);
	RWindow testWin(TheClient->iWs);
	CleanupClosePushL(testWin);
	User::LeaveIfError(testWin.Construct(*TheClient->iGroup->GroupWin(),0xbadbad));
	testWin.SetRequiredDisplayMode(EColor64K);
	testWin.Activate();
// Cleanup display mode by setting back to 0 if we leave in the tests
	CleanupStack::PushL(TCleanupItem(ResetScreenMode,TheClient->iScreen));
	TInt numScreenModes=TheClient->iScreenModes.Count();
	TBool match;

	for(TInt winSizeMode=0;winSizeMode<2;winSizeMode++)
		{ // Two size modes, fullScreen and non-full screen
		for(TInt redrawMode=0;redrawMode<2;redrawMode++)
			{ // Two redraw modes to test drawing inside and outside of a redraw.
			const TBool drawInsideRedraw=(redrawMode==0);
			for(TInt modeIndex=0;modeIndex<numScreenModes;modeIndex++)
				{
				const TInt screenMode=TheClient->iScreenModes[modeIndex];
				const TPoint origin=TheClient->iScreen->GetScreenModeScaledOrigin(screenMode);
				if (origin.iX!=0 || origin.iY!=0)
					continue;
				// Enable redirection before changing screen mode as this is what we are testing
				iRedir->Redirect(CWsRedir::EFrontBuffer, ETrue);
				TheClient->iScreen->SetAppScreenMode(screenMode);
				TheClient->iScreen->SetScreenMode(screenMode);

				TPixelsAndRotation sizeAndRotation;
				TheClient->iScreen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
//
				TSize screenSize(TheClient->iScreen->SizeInPixels());
				if(sizeAndRotation.iRotation == CFbsBitGc::EGraphicsOrientationRotated90 || 
						sizeAndRotation.iRotation == CFbsBitGc::EGraphicsOrientationRotated270)
					{
					screenSize.iWidth = Min(screenSize.iWidth, 240);//to make sure we won't exceed physical screen size
					}
				TSize winSize(screenSize);
				TPoint winPos;
				if (winSizeMode==1)
					{
					winSize.iWidth=winSize.iWidth*2/3;
					winSize.iHeight=winSize.iHeight*3/4;
					winPos.iX=(screenSize.iWidth-winSize.iWidth)/4;
					winPos.iY=(screenSize.iHeight-winSize.iHeight)*3/4;
					}
				testWin.SetExtent(winPos,winSize);
				TSize halfSize(winSize.iWidth/2,winSize.iHeight);
				TRect leftHalf(halfSize);
				TRect rightHalf(TPoint(halfSize.iWidth,0),halfSize);
				TRect leftEllipse(leftHalf);
				leftEllipse.Shrink(4,4);
				TRect rightEllipse(rightHalf);
				rightEllipse.Shrink(4,4);
				// Draw half the screen with redirection on, should only go to redirection test bitmap
				// Then draw again with redirection off, this time should go to the screen
				// The two steps are drawn with the color of the left/right rectangles swapped.
				for(TInt drawStep=0;drawStep<2;drawStep++)
					{
					TRgb leftColor;
					TRgb rightColor;
					if (drawStep==0)
						{
						leftColor=KRgbGreen;
						rightColor=KRgbRed;
						}
					else
						{ // Turn re-direction off for second time around loop
						iRedir->Redirect(CWsRedir::EFrontBuffer, EFalse);
						leftColor=KRgbRed;
						rightColor=KRgbGreen;
						}
					testWin.Invalidate();
					testWin.BeginRedraw();
					if (!drawInsideRedraw)
						testWin.EndRedraw();
					CWindowGc* testWinGc=TheClient->iGc;
					testWinGc->Activate(testWin);
					testWinGc->SetBrushColor(leftColor);
					testWinGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
					testWinGc->DrawRect(leftHalf);
					// Call CRP drawer to draw the coloured ellipses
					TheClient->iGc->DrawWsGraphic(wsGraphic1->Id(),leftEllipse);
					testWinGc->SetBrushColor(rightColor);
					testWinGc->DrawRect(rightHalf);
					TheClient->iGc->DrawWsGraphic(wsGraphic1->Id(),rightEllipse);
					testWinGc->Deactivate();
					if (drawInsideRedraw)
						testWin.EndRedraw();
					TheClient->iWs.Flush();
					}
				// We now check that the left rect of the re-directed drawing matches the right half
				// of the on-screen drawing
				CFbsBitmap* screenCopy=new(ELeave) CFbsBitmap;
				CleanupStack::PushL(screenCopy);
				User::LeaveIfError(screenCopy->Create(screenSize, EColor64K));
				TheClient->iScreen->CopyScreenToBitmap(screenCopy);
				match=CompareBitmapArea16Bpp(iFrontCopy,winPos,screenCopy,TPoint(winPos.iX+rightHalf.iTl.iX,winPos.iY),halfSize);
				TEST(match);
				if (!match)
					LOG_MESSAGE6(KLog,screenSize.iWidth,screenSize.iHeight,winSizeMode,redrawMode,modeIndex);
				// As a double check also check the right half of the off-screen drawing matches the
				// on-screen left half.
				match=CompareBitmapArea16Bpp(iFrontCopy,TPoint(winPos.iX+rightHalf.iTl.iX,winPos.iY),screenCopy,winPos,halfSize);
				TEST(match);
				if (!match)
					LOG_MESSAGE6(KLog,screenSize.iWidth,screenSize.iHeight,winSizeMode,redrawMode,modeIndex);
				CleanupStack::PopAndDestroy(screenCopy);
				}
			}
		}
 	CleanupStack::PopAndDestroy(3,wsGraphic1);
 	
	TEST(iNotify1->iResult);
	if(iNotify1->iResult==EFalse)
		{
		INFO_PRINTF1(iNotify1->iError);
		}
	TEST(iNotify2->iResult);
	if(iNotify1->iResult==EFalse)
		{
		INFO_PRINTF1(iNotify2->iError);
		}
	}

#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

/**
@SYMTestCaseID 		GRAPHICS-WSERV-0443

@SYMDEF     		INC109263

@SYMTestCaseDesc	TWindowServerEvent::NotifyDrawer can refer to a deleted array index causing a crash

@SYMTestPriority 	High

@SYMTestStatus		Implemented

@SYMTestActions		Cause an event that will generate the following behaviour through TWindowServerEvent::NotifyDrawer()
		
					The for loop performs the following:
					0) Drawer handler 1: Remove handle 1 ; 					Drawer handler 2: Do nothing
					1) Drawer handler 1: Remove handle 1,Add handle 1 ;		Drawer handler 2: Do nothing
					2) Drawer handler 1: Add handle 1 ; 					Drawer handler 2: Do nothing
					3) Drawer handler 1: Remove handle 2 ; 					Drawer handler 2: Do nothing
					4) Drawer handler 1: Remove handle 2,Add handle 2 ; 	Drawer handler 2: Do nothing
					5) Drawer handler 1: Add handle 2 ; 					Drawer handler 2: Do nothing
					6) Drawer handler 1: Remove handle 1,Remove handle 2 ; 	Drawer handler 2: Do nothing
			
					Repeat with handlers 1 and 2 swapped

@SYMTestExpectedResults		Loops through TWindowServerEvent::NotifyDrawer() should complete without crashing wserv
*/

void CTWsGraphs::TestNotifyRemoval()
	{
	_LIT(KTestEventNotification, "TestDrawerEventHandler");
	INFO_PRINTF1(KTestEventNotification);
	++iSubState;

	for (TInt ii = 0; ii<KNotifyDoNothing; ii++)
		{
		INFO_PRINTF2(_L("For loop %d"), ii);
		iNotify1->SetBehaviour(ii);					//Enable this plugin and set it to an event handling method
		iNotify2->SetBehaviour(KNotifyDoNothing);	//Add a second drawer handler which is enabled but does nothing
		CreateWindowL();							//Change visibility activating the event handlers - Fails if wserv crashes!
		iNotify1->SetBehaviour(KNotifyDisable);		//Disable plugin if still enabled
		iNotify2->SetBehaviour(KNotifyDisable);		//Disable plugin if still enabled
		DestroyWindowL();
		}
	INFO_PRINTF1(_L("Swap handlers"));
	for (TInt ii = 0; ii<KNotifyDoNothing; ii++)
		{
		INFO_PRINTF2(_L("For loop %d"), ii);
		iNotify2->SetBehaviour(KNotifyDoNothing);	//Add a first drawer handler which is enabled but does nothing
		iNotify1->SetBehaviour(ii);					//Enable this plugin and set it to an event handling method
		CreateWindowL();							//Change visibility activating the event handlers - Fails if wserv crashes!
		iNotify2->SetBehaviour(KNotifyDisable);		//Disable plugin if still enabled
		iNotify1->SetBehaviour(KNotifyDisable);		//Disable plugin if still enabled
		DestroyWindowL();
		}

	TEST(ETrue);		// If the test has failed WServ will have paniced.
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
/**
@SYMTestCaseID           GRAPHICS-WSERV-0491
@SYMPREQ                 PREQ39
@SYMTestPriority         High
@SYMTestCaseDesc         Draw using a ECom-plugin which has direct access to the screen/OSB buffer 
@SYMTestActions          Create the plugin
						 Draw using the plugin
						 Update the position of the white line to line 70 and test
						 Update the position of the white line to line 80 and test
@SYMTestExpectedResults  White lines are drawn on the correct positions.
*/
void CTWsGraphs::TestMWsUiBufferL()
	{
	const TInt KWhiteLinePos = 70;
	const TRect KBlueRect(TPoint(50,50),TSize(100,100));
		
	// Construct and setup window to be drawn to
	RWindow window1 = RWindow(TheClient->iWs);
	CleanupClosePushL(window1);
    User::LeaveIfError(window1.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
    TSize winSize=TSize(TheClient->iScreen->SizeInPixels());
    window1.SetExtent(TPoint(0,0),winSize);
    window1.Activate();
    
	CWsBufferGraphic* graphic = CWsBufferGraphic::NewL();
	CleanupStack::PushL(graphic);
    
	// Draw inital drawing with a Crp which is blue rect and a white line at line 0
	TheGc->Activate(window1);
	TheGc->Clear();
	window1.BeginRedraw();
	TheGc->DrawWsGraphic(graphic->Id(),KBlueRect);
	window1.EndRedraw();
	TheGc->Deactivate();
	
    TheClient->iWs.Finish();
    User::After(2000000);
	
	// Update the position of the white line to line 70
	INFO_PRINTF1(_L("Update position of white line to line 70"));
	graphic->UpdateWhiteLinePos(KWhiteLinePos);
	TheClient->iWs.Finish();
	// Force some delays to wait until the line position changes
    User::After(2000000);
    //Test white line has been drawn and is in the correct postion
    TBool res1 = IsWhiteLine(KWhiteLinePos);
    TEST(res1);
    
	// Update the position of the white line to line 80
    INFO_PRINTF1(_L("Update position of white line to line 80"));
	graphic->UpdateWhiteLinePos(KWhiteLinePos+10);
	TheClient->iWs.Finish();
	// Force some delays to wait until the line position changes
	User::After(2000000);
	// Test white line has been drawn and is in the correct postion
    TBool res2 = IsWhiteLine(KWhiteLinePos+10);
    TEST(res2);
   
	graphic->Destroy();
	CleanupStack::PopAndDestroy(2, &window1);
	}

// Test whether a line is completely white
TBool CTWsGraphs::IsWhiteLine(TInt aWhiteLinePos)
	{
    TRgb color;
    TPoint pixel;
    
    for(TInt xPos = 0; xPos < TheClient->iScreen->SizeInPixels().iWidth; xPos++)
    	{
    	pixel = TPoint(xPos,aWhiteLinePos);
    	TheClient->iScreen->GetPixel(color,pixel);
    	if(color.Red() != 255 && color.Blue() != 255 && color.Green() != 255)
    		{
    		return EFalse;
    		}
    	}
    return ETrue;
	}
#endif //TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA

/**
	DoTestL() method, called by the WSERV Test Framework.
*/
void CTWsGraphs::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest1,"DoTestWsGraphics");
	_LIT(KTest2,"DoTestOOMWsGraphics");
	_LIT(KTest3,"Interface Extension");
	_LIT(KTest4,"Screen Redirection");
	_LIT(KTest5,"TextCursor1");
	_LIT(KTest6,"TextCursor2");
	_LIT(KTest7,"Flicker Redirection");
	_LIT(KTest8,"Event Notification");
	_LIT(KTest9,"Successive Message");
	_LIT(KTest10,"Redirection Using WsBackBuffer");
	_LIT(KTest11,"Group Change");
	_LIT(KTest12,"Frame Rate");
	_LIT(KTest13,"Leak In Service");
	_LIT(KTest14,"Add/Swap GDArray");
	_LIT(KTest15,"Nested Drawer CRP");
	_LIT(KTest16,"Notify Removal");
	_LIT(KTest17,"Screen Mode Change");
	_LIT(KTest18,"UI Buffer");
	_LIT(KTest19,"Graphics Drawer Coverage");
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	CWsGraphicBitmap* bTest=NULL;
	TSize screenSize=TheClient->iScreen->SizeInPixels();

	bitmap1.Create(screenSize,TheClient->iScreen->DisplayMode());
	mask1.Create(screenSize,TheClient->iScreen->DisplayMode());
	((CTWsGraphsStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(++iTest->iState)
		{
	case 1:
/**
	@SYMTestCaseID	GRAPHICS-WSERV-0528
*/
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0528"));
		// Launch new process with PROTSERV capability to run CWSGraphics tests in
		iTest->LogSubTest(KTest1);
		// This process only launches succesfully when _DEBUG is defined for the build, because it depends
		// on the existance of debug macros such as _UHEAP_MARK, _UHEAP_MARKEND, _UHEAP_FAILNEXT, ... etc
		LaunchNewProcess(KTestExe);
		break;
	case 2:
		{
		/**
   		@SYMTestCaseID GRAPHICS-WSERV-0017

		@SYMPREQ	PREQ1246

		@SYMDEF		DEF081259

		@SYMTestCaseDesc Out of memery test when creating a CWsGraphic.

		@SYMTestPriority High

		@SYMTestStatus Implemented

		@SYMTestActions Out of memory test when creating a CWsGraphic.

		@SYMTestExpectedResults Whenever an API call fails, it should leave the number
				of allocated heap cells unchanged.
		*/
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0017"));

		iTest->LogSubTest(KTest2);
		TInt failRate;
		for(failRate=1;;failRate++)
			{
			__UHEAP_RESET;
			__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
			__UHEAP_MARK;

			TRAPD(ret,bTest=CWsGraphicBitmap::NewL(&bitmap1,&mask1));
			TEST((ret==KErrNone || ret==KErrNoMemory));
			if (ret!=KErrNone && ret!=KErrNoMemory)
				{
				_LIT(KLog,"Failed to create CWsGraphicBitmap  error=%d");
				LOG_MESSAGE2(KLog,ret);
				}
			if (ret!=KErrNone)
				{
				__UHEAP_MARKEND;
				}
			else
				{
				TEST(bTest!=NULL);
				if (bTest==NULL)
					{
					_LIT(KLog,"Object creation didn't leave but returned NULL");
					LOG_MESSAGE(KLog);
					}
				delete bTest;
				bTest=NULL;
				__UHEAP_MARKEND;
				TLogMessageText logMessageText;
				_LIT(KSet,"OOM test succeds after %d allocations.");
				logMessageText.Format(KSet,failRate);
				LOG_MESSAGE(logMessageText);
				break;
				}
			}
		__UHEAP_RESET;
		}
		break;
	case 3:
		iTest->LogSubTest(KTest3);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0371"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			
			if (TestInterfaceExtensionL()==EWait)
				--iTest->iState;
#endif
		break;
	case 4:
		iTest->LogSubTest(KTest4);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0372"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA			
			if (TestScreenRedirectionL()==EWait)
				--iTest->iState;
#endif
		break;
	case 5:
		iTest->LogSubTest(KTest5);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0363"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA	
			if (TestTextCursorUnderRedirectionL(ETestStandardTextCursor) == EWait)
				--iTest->iState;
#endif
		break;
	case 6:
		iTest->LogSubTest(KTest6);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0363"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA	
			if (TestTextCursorUnderRedirectionL(ETestCustomTextCursor) == EWait)
				--iTest->iState;
#endif
		break;
	case 7:
		iTest->LogSubTest(KTest7);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0376"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA			
			if (TestFlickerRedirectionL()==EWait)
				--iTest->iState;
#endif
		break;
	case 8:
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0378"));
		iTest->LogSubTest(KTest8);
		if (TestEventNotificationL()==EWait)
			--iTest->iState;
		break;
	case 9:
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0381"));
		iTest->LogSubTest(KTest9);
		if (TestSuccessiveMessageL()==EWait)
			--iTest->iState;
		break;
	case 10:
		iTest->LogSubTest(KTest10);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0527"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		if(TestRedirectionUsingWsBackBufferL()==EWait)
			--iTest->iState;
#endif
		break;
	case 11:
/**
	@SYMTestCaseID	GRAPHICS-WSERV-0529
*/
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0529"));
		iTest->LogSubTest(KTest11);
		if(TestWindowGroupChangeL()==EWait)
			--iTest->iState;
		break;
	case 12:
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0382"));
		iTest->LogSubTest(KTest12);
		if(TestFrameRateL()==EWait)
			--iTest->iState;
		break;
	case 13:
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-LeakInService-0001"));
		iTest->LogSubTest(KTest13);
		DoTestLeakInServiceL();
		break;
	case 14:
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-NegAddSwapGDArray-0001"));
		iTest->LogSubTest(KTest14);
		TestAddSwapGDArrayL();
		break;
	case 15:
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0438"));
		iTest->LogSubTest(KTest15);
		TestNestedDrawerCRP();
		break;
	case 16:
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0443"));
		iTest->LogSubTest(KTest16);
		TestNotifyRemoval();
		break;
	case 17:
		iTest->LogSubTest(KTest17);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0530"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
/**
	@SYMTestCaseID	GRAPHICS-WSERV-0530
*/
			
			TestScreenModeChangeL();
#endif
		break;
	case 18:
		iTest->LogSubTest(KTest18);
		((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0491"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
			
			TestMWsUiBufferL();
#endif
			break;
		case 19:
			iTest->LogSubTest(KTest19);
/**
	@SYMTestCaseID	GRAPHICS-WSERV-0531
*/
			((CTWsGraphsStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0531"));
			TestGraphicDrawerCoverage();
			break;
		default:
			((CTWsGraphsStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTWsGraphsStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTWsGraphsStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(WsGraphs)
