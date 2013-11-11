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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TSPRITE.H"

CTTSprite::CTTSprite(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Testing TSprite functions"));
	}

void CTTSprite::SetUpMember(TSpriteMember &aMember)
	{
	aMember.iMaskBitmap=NULL;
	aMember.iInvertMask=EFalse;
	aMember.iDrawMode=CGraphicsContext::EDrawModePEN;
	aMember.iOffset=TPoint();
	aMember.iInterval=TTimeIntervalMicroSeconds32(0);
	aMember.iBitmap=&iBitmap;
	}

void CTTSprite::SetUpPointerCursorL(RWsPointerCursor &aCursor, RWsSession &aSession)
	{
	aCursor=RWsPointerCursor(aSession);
	TSpriteMember member;
	SetUpMember(member);
	User::LeaveIfError(aCursor.Construct(0));
	User::LeaveIfError(aCursor.AppendMember(member));
	User::LeaveIfError(aCursor.Activate());
	}

void CTTSprite::SetUpSpriteL(RWsSprite &aSprite, RWsSession &aSession, RWindowTreeNode &aWindow,TInt aFlags/*=0*/)
	{
	aSprite=RWsSprite(aSession);
	User::LeaveIfError(aSprite.Construct(aWindow,TPoint(),aFlags));
	TSpriteMember member;
	SetUpMember(member);
	User::LeaveIfError(aSprite.AppendMember(member));
	User::LeaveIfError(aSprite.Activate());
	}

void CTTSprite::ConstructL()
	{
  	User::LeaveIfError(iBitmap.Load(TEST_BITMAP_NAME,0));
	}
	
CTTSprite::~CTTSprite()
	{
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0018

@SYMDEF  			DEF081259

@SYMTestCaseDesc    General cursor tests involving a sprite

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Carries out cursor and window tests before updating
					a cursor's member with a sprite

@SYMTestExpectedResults When the cursor member is update returns KErrArgument
*/

void CTTSprite::GeneralTestsL()
	{
//
// Close cursor while still active on a window
//
	
	RWindow win(TheClient->iWs);
	win.Construct(*TheClient->iGroup->GroupWin(),1);
	win.Activate();
	SetUpPointerCursorL(iCursor1,TheClient->iWs);
	win.SetCustomPointerCursor(iCursor1);
	iCursor1.Close();
//
// Close window while cursor active on it
//
	SetUpPointerCursorL(iCursor1,TheClient->iWs);
	win.SetCustomPointerCursor(iCursor1);
	win.Close();
	iCursor1.Close();
	
//
// Close session with:
// An open cursor active on a window & A closed cursor active on another window
//
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	ws.SetFocusScreen(iTest->iScreenNumber);
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(0));

	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(123));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RWindow win2(ws);
	User::LeaveIfError(win2.Construct(group, 1));
	win2.Activate();
	RWindow win3(ws);
	User::LeaveIfError(win3.Construct(group, 2));
	win3.Activate();
//
	SetUpPointerCursorL(iCursor1,ws);
	SetUpPointerCursorL(iCursor2,ws);
	win2.SetCustomPointerCursor(iCursor1);
	win3.SetCustomPointerCursor(iCursor2);
//
	TSpriteMember member;
	member.iBitmap=member.iMaskBitmap=NULL;
	TInt err = iCursor1.UpdateMember(9999,member);
	TEST(err==KErrArgument);
	if (err!=KErrArgument)
		INFO_PRINTF3(_L("iCursor1.UpdateMember(9999,member) return value  - Expected: %d, Actual: %d"), KErrArgument, err);

	iCursor1.Close();

	CleanupStack::PopAndDestroy(screen);	
	ws.Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0019

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Construct a sprite in a group window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a sprite in a group window

@SYMTestExpectedResults The sprite is created without error
*/
void CTTSprite::GroupWindowSpritesL()
	{
	RWsSprite sprite1;
	RWsSprite sprite2;
	SetUpSpriteL(sprite1,TheClient->iWs,*TheClient->iGroup->GroupWin());
	SetUpSpriteL(sprite2,TheClient->iWs,*TheClient->iGroup->GroupWin());
	TheClient->iWs.Flush();
	sprite2.Close();
	sprite1.Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0020

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Constructs a number of different sprites in different windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates many sprites in three different windows and the checks
					the sprites can be manipulated

@SYMTestExpectedResults The sprite are created and manipulated without error
*/
#define NUM_SPRITES 8
void CTTSprite::LotsSpritesL()
	{
	CTBlankWindow* win=new(ELeave) CTBlankWindow();
	CTBlankWindow* win2=new(ELeave) CTBlankWindow();
	TSize size(100,120);
	win->ConstructL(*TheClient->iGroup);
	win2->ConstructL(*win);
	win2->SetExt(TPoint(100,100),size);
	win2->SetColor(TRgb::Gray4(2));
	RWindowBase window=*win->BaseWin();
	RWindowBase window2=*win2->BaseWin();
	window.Activate();
	window2.Activate();
	RWsSprite sprite[NUM_SPRITES];
	TInt ii;
	//TheClient->iWs.SetAutoFlush(ETrue);
	for (ii=0;ii<NUM_SPRITES;ii++)
		SetUpSpriteL(sprite[ii],TheClient->iWs,window);
	sprite[4].Close();
	sprite[2].SetPosition(TPoint(20,20));
	sprite[0].Close();
	win2->SetExt(TPoint(80,100),size);
	sprite[6].SetPosition(TPoint(60,120));
	sprite[7].Close();
	sprite[5].SetPosition(TPoint(100,120));
	sprite[3].Close();
	SetUpSpriteL(sprite[7],TheClient->iWs,window);
	sprite[7].SetPosition(TPoint(80,150));
	sprite[1].Close();
	win2->SetExt(TPoint(60,110),size);
	sprite[5].SetPosition(TPoint(50,40));
	sprite[7].Close();
	SetUpSpriteL(sprite[0],TheClient->iWs,window);
	sprite[0].SetPosition(TPoint(55,65));
	sprite[6].Close();
	win2->SetExt(TPoint(40,90),size);
	sprite[2].SetPosition(TPoint(80,45));
	sprite[5].Close();
	sprite[0].SetPosition(TPoint(90,60));
	sprite[2].Close();
	SetUpSpriteL(sprite[2],TheClient->iWs,window);
	sprite[2].SetPosition(TPoint(70,80));
	sprite[0].Close();
	win2->SetExt(TPoint(20,80),size);
	sprite[2].SetPosition(TPoint(600,200));
	sprite[2].Close();
	SetUpSpriteL(sprite[0],TheClient->iWs,window2,ESpriteFlash);
	sprite[0].SetPosition(TPoint(0,25));
	SetUpSpriteL(sprite[1],TheClient->iWs,window2,ESpriteFlash);
	SetUpSpriteL(sprite[2],TheClient->iWs,window2,ESpriteFlash);
	sprite[2].SetPosition(TPoint(25,0));
	win2->SetExt(TPoint(40,70),size);
	CTBlankWindow* win3=new(ELeave) CTBlankWindow();
	win3->ConstructL(*TheClient->iGroup);
	win3->SetExt(TPoint(30,60),TSize(30,30));
	win3->SetColor(TRgb::Gray4(1));
	win3->BaseWin()->SetShadowHeight(10);
	win3->BaseWin()->Activate();
	User::After(1000000);		//1 sec so sprites has time to flash
	delete win2;
	delete win;
	delete win3;
	sprite[0].Close();
	sprite[1].Close();
	sprite[2].Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0021

@SYMDEF  			DEF081259

@SYMTestCaseDesc    General PointerCursor Tests

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Exercise the different pointercursor methods of a Window Server Session

@SYMTestExpectedResults The methods are called without error
*/
void CTTSprite::GeneralPointerCursor()
	{
	if (!TestBase()->ConfigurationSupportsPointerEventTesting())
	    {
	    INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
	    return;
	    }
	TInt currentSMode=TheClient->iScreen->CurrentScreenMode();
	TInt altSMode=-1;
	if (TheClient->iScreen->NumScreenModes()>1)
		altSMode=(currentSMode==1?0:1);
	RWsSession &ws=TheClient->iWs;
	TRect rect=ws.PointerCursorArea();
	TRect testRect1(TPoint(rect.iBr.iX/4,rect.iBr.iY/4),TSize(rect.Width()/2,rect.Height()/2));
	TRect testRect2(TPoint(rect.iBr.iX/3,rect.iBr.iY/3),TSize(2*rect.Width()/3,2*rect.Height()/3));
	ws.SetPointerCursorArea(testRect1);
	TEST(ws.PointerCursorArea()==testRect1);
	TEST(ws.PointerCursorArea(currentSMode)==testRect1);
	ws.SetPointerCursorArea(currentSMode,testRect2);
	TEST(ws.PointerCursorArea()==testRect2);
	TEST(ws.PointerCursorArea(currentSMode)==testRect2);
	ws.SetPointerCursorArea(rect);
	TEST(ws.PointerCursorArea()==rect);
	
	if (altSMode>=0)
		{
		rect=ws.PointerCursorArea(altSMode);
		testRect1.iTl.iX=rect.iBr.iX/4;
		testRect1.iTl.iY=rect.iBr.iY/4;
		testRect1.SetWidth(rect.Width()/2);
		testRect1.SetHeight(rect.Height()/2);
		ws.SetPointerCursorArea(altSMode,testRect1);
		TEST(ws.PointerCursorArea(altSMode)==testRect1);
		ws.SetPointerCursorArea(altSMode,rect);
		TEST(ws.PointerCursorArea(altSMode)==rect);
		}
	TPointerCursorMode currentMode=ws.PointerCursorMode();
	TInt ii;
	TInt err1;
	for(ii=EPointerCursorFirstMode;ii<=EPointerCursorLastMode;ii++)
		{	
		ws.SetPointerCursorMode(STATIC_CAST(TPointerCursorMode,ii));
		err1 = ws.PointerCursorMode();
		TEST(ii==err1);
		if (ii!=err1)
			INFO_PRINTF3(_L("ws.PointerCursorMode() return value  - Expected: %d, Actual: %d"), ii, err1);		
		}
	ws.SetPointerCursorMode(currentMode);
	TEST(currentMode==ws.PointerCursorMode());
	TPoint point1(10,12);
	TPoint point2(24,20);
	ws.PointerCursorPosition();
	ws.SetPointerCursorPosition(point1);
	TEST(ws.PointerCursorPosition()==point1);
	ws.SetPointerCursorPosition(point2);
	TEST(ws.PointerCursorPosition()==point2);
	}


/**
@SYMTestCaseID      GRAPHICS-WSERV-0498

@SYMDEF             PDEF137614 - Propagation to TB92

@SYMTestCaseDesc    Test activating a pointer cursor

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Construct and activate a pointer cursor. 
                    Check it is visible.
                    Deactivate it
                    Check it is removed
                    

@SYMTestExpectedResults 
                    The pointer cursor bitmp should be visible when activated and removed when deactivated.
*/

void CTTSprite::PointerCursorVisibleL()
    {
    if (!TestBase()->ConfigurationSupportsPointerEventTesting())
        {
        INFO_PRINTF1(_L("Test skipped because config does not support pointer event testing"));
        return;
        }
    
    // The pointer events need time to have an affect on the wserv
    static const TInt eventPropagationDelay = 100 * 1000; // 100 ms
    
    TInt screenNumber = TheClient->iScreen->GetScreenNumber();
    
    if(screenNumber != 0) // pointer events only supported on emulator screen 0
        {
        LOG_MESSAGE(_L("Pointer Cursor Visible only runs on screen 0"));
        return;
        }
    
    // set up objects used in test
    // 50x50 red rectangle colour 24
    CFbsBitmap bitmap;
    User::LeaveIfError(bitmap.Load(TEST_BITMAP_NAME, 8));
    
    TSize bmSize = bitmap.SizeInPixels();
    TPoint bmSample = TPoint(bmSize.iWidth / 2, bmSize.iHeight / 2);
    TRgb bmColour;
    bitmap.GetPixel(bmColour, bmSample);
    TEST(bmColour == KRgbRed);

    // single window, size of screen
    RWindow win(TheClient->iWs);
    User::LeaveIfError(win.Construct(*TheClient->iGroup->GroupWin(),1));
    win.Activate();

    // setup cursor mode
    TheClient->iWs.SetPointerCursorMode(EPointerCursorWindow);
    
    // setup cursor to contain single 50x50 red bitmap
    RWsPointerCursor iCursor1 = RWsPointerCursor(TheClient->iWs);
    TSpriteMember member;
    SetUpMember(member);
    member.iBitmap=&bitmap;
    User::LeaveIfError(iCursor1.Construct(0));
    User::LeaveIfError(iCursor1.AppendMember(member));
    User::LeaveIfError(iCursor1.Activate());
    win.SetCustomPointerCursor(iCursor1);
    iCursor1.UpdateMember(0);

    // draw a green rect, size of screen as defined background and wait till it is rendered
    win.BeginRedraw();
    TheGc->Activate(win);
    TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    TheGc->SetBrushColor(KRgbGreen);
    TSize wSize = win.Size();
    TheGc->DrawRect(TRect(TPoint(0,0), wSize));
    TheGc->Deactivate();
    win.EndRedraw();
    TheClient->iWs.Finish();
    
    // #### do test ####
    // define locations of simulated pointer events and sample positions of where we expect to see the cursor
    // The cursor will be moved and a check will be made to see if this has actually happened
    
    TPoint pos1(wSize.iWidth / 2, wSize.iHeight / 2); // top left of cursor at centre screen
    TPoint sample1(pos1 + bmSample); // centre of sprite at pos1
    TPoint pos2 = pos1 - bmSize; // bottom right of cursor at centre screen
    TPoint sample2 = pos2 + bmSample;  // centre of sprite at pos2

    TRgb pixel;
    
    // check initial state of screen at both sample positions
    TheClient->iScreen->GetPixel(pixel, sample1);
    TEST(pixel == KRgbGreen);

    TheClient->iScreen->GetPixel(pixel, sample2);
    TEST(pixel == KRgbGreen);
    
    TRawEvent e; // to simulate pointer events

    // simulate button 1 down event at pos1
    e.Set(TRawEvent::EButton1Down, pos1.iX, pos1.iY);
    e.SetDeviceNumber(screenNumber);
    UserSvr::AddEvent(e);

    User::After(eventPropagationDelay);
    
    // check red cursor visible on top of background
    TheClient->iScreen->GetPixel(pixel, sample1);
    TEST(pixel == KRgbRed);
    
    // simulate button 1 up event
    e.Set(TRawEvent::EButton1Up, pos1.iX, pos1.iY);
    UserSvr::AddEvent(e);
    User::After(eventPropagationDelay);
    
    // Move cursor away to pos2 
    e.Set(TRawEvent::EButton1Down, pos2.iX, pos2.iY);
    e.SetDeviceNumber(screenNumber);
    UserSvr::AddEvent(e);
    
    User::After(eventPropagationDelay);
    
    // check cursor has left this position ...
    TheClient->iScreen->GetPixel(pixel, sample1);
    TEST(pixel == KRgbGreen);
    // and arrived at the correct place
    TheClient->iScreen->GetPixel(pixel, sample2);
    TEST(pixel == KRgbRed);

    // simulate button 1 up event
    e.Set(TRawEvent::EButton1Up, pos2.iX, pos2.iY);
    UserSvr::AddEvent(e);
    User::After(eventPropagationDelay);
    
    // remove the cursor
    win.ClearPointerCursor();
    User::After(eventPropagationDelay);
    
    // check it has gone
    TheClient->iScreen->GetPixel(pixel, sample2);
    TEST(pixel == KRgbGreen);
    
    // #### clean up ####
    iCursor1.Close();
    win.Close();
    }


/**
	@SYMTestCaseID GRAPHICS-CODEBASE-WSERV-0053-0001
  
	@SYMPREQ PGM027
  
	@SYMTestCaseDesc Tests RWsPointerCursor::UpdateMember APIs. 
   
	@SYMTestPriority 1 
  
	@SYMTestStatus Implemented
   
	@SYMTestActions This test calls RWsPointerCursor::UpdateMember
		
	@SYMTestExpectedResults Should run properly with out any Panics.

  */
void CTTSprite::SpriteUpdateMemberTestsL()
	{
	RWsPointerCursor iCursor1;
	
	CFbsBitmap bitmap;
	bitmap.Load(TEST_BITMAP_NAME,0);
		
	RWindow win(TheClient->iWs);
	win.Construct(*TheClient->iGroup->GroupWin(),1);
	win.Activate();

	iCursor1=RWsPointerCursor(TheClient->iWs);
	TSpriteMember member;
	SetUpMember(member);
	member.iBitmap=&bitmap;
	User::LeaveIfError(iCursor1.Construct(0));
	User::LeaveIfError(iCursor1.AppendMember(member));
	User::LeaveIfError(iCursor1.Activate());
	win.SetCustomPointerCursor(iCursor1);
	iCursor1.UpdateMember(0);
	
	RWsPointerCursor iCursor2;
	bitmap.Load(TEST_NEW_BITMAP_NAME,0);
	iCursor2=RWsPointerCursor(TheClient->iWs);
	User::LeaveIfError(iCursor2.Construct(0));
	User::LeaveIfError(iCursor2.AppendMember(member));
	User::LeaveIfError(iCursor2.Activate());
	win.SetCustomPointerCursor(iCursor2);
	iCursor2.UpdateMember(1);
	
	iCursor1.Close();
	iCursor2.Close();
	win.Close();
	}

/**
	@SYMTestCaseID GRAPHICS-CODEBASE-WSERV-0054-0001
  
	@SYMPREQ PGM027
  
	@SYMTestCaseDesc Negative Tests RWsSpriteBase::UpdateMember API. 
   
	@SYMTestPriority 1 
  
	@SYMTestStatus Implemented
   
	@SYMTestActions This test calls RWsPointerCursor::UpdateMember
		
	@SYMTestExpectedResults Should run properly with out any Panics.

 */
void CTTSprite::SpriteUpdateMemberNegTestsL()
	{
	RWsPointerCursor iCursor1;
	
	CFbsBitmap bitmap;
	bitmap.Load(TEST_BITMAP_NAME,0);
		
	RWindow win(TheClient->iWs);
	win.Construct(*TheClient->iGroup->GroupWin(),1);
	win.Activate();

	iCursor1=RWsPointerCursor(TheClient->iWs);
	TSpriteMember member;
	SetUpMember(member);
	member.iBitmap=&bitmap;
	User::LeaveIfError(iCursor1.Construct(0));
	User::LeaveIfError(iCursor1.AppendMember(member));
	User::LeaveIfError(iCursor1.Activate());
	win.SetCustomPointerCursor(iCursor1);
	iCursor1.UpdateMember(-100);
	
	RWsPointerCursor iCursor2;
	bitmap.Load(TEST_NEW_BITMAP_NAME,0);
	iCursor2=RWsPointerCursor(TheClient->iWs);
	User::LeaveIfError(iCursor2.Construct(0));
	User::LeaveIfError(iCursor2.AppendMember(member));
	User::LeaveIfError(iCursor2.Activate());
	win.SetCustomPointerCursor(iCursor2);
	iCursor2.UpdateMember(10000);
	
	iCursor1.Close();
	iCursor2.Close();
	win.Close();
	}

/**
	@SYMTestCaseID GRAPHICS-CODEBASE-WSERV-0055-0001
  
	@SYMPREQ PGM027
  
	@SYMTestCaseDesc Tests FindWindowGroupIdentifier API. 
   
	@SYMTestPriority 1 
  
	@SYMTestStatus Implemented
   
	@SYMTestActions Create windows session and call FindWindowGroupIdentifier \n
	with different thread Id's both with valid thread ids and in valid thread id's.
	
	@SYMTestExpectedResults Should run properly.

 */	
void CTTSprite::FindWindowGroupThreadTestsL()	
	{
	RThread proc;
	TInt ident;
	TUint64 id=proc.Id();
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());
	CleanupClosePushL(ws1);
	//Positive test for FindWindowGroupIdentifier
	ident=ws1.FindWindowGroupIdentifier(0,id);
	TEST(ws1.SetWindowGroupOrdinalPosition(ident,0)==KErrNone);
	TEST(ws1.SetWindowGroupOrdinalPosition(ident,1)==KErrNone);
	//Negative test for FindWindowGroupIdentifier
    TInt ret=ws1.FindWindowGroupIdentifier(0,id+200);
	TEST(ret==KErrNotFound);
	#if defined __WINS__ || defined __WINSCW__
	ret=ws1.FindWindowGroupIdentifier(0,-200);
	TEST(ret==KErrNotFound);
	#endif//defined __WINS__ || defined __WINSCW__
	CleanupStack::PopAndDestroy(1, &ws1);
	ws1.Close();
	}	

/**
@SYMTestCaseID		GRAPHICS-WSERV-0462

@SYMDEF  			PDEF114190

@SYMTestCaseDesc    Test sprite list cleanup when a window is deleted in low memory conditions

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Have a loop which increases the number of allocations in the server thread before failure;
					Within the loop:
					1) Create a parent window and a child window of the parent;
					2) Create a sprite on the child window; 
					3) Delete the parent window only, but not the child window; 
					4) Create a testWindow and do redraw on this window; 
					5) Do redraw on the testWindow. This testWindow's redraw will force checking the sprite list. 
					This would panic the client thread due to this defect because the sprite on the orphaned 
					window (which is the previous child window) is still in the sprite list;
					6) Delete all the windows involved.

@SYMTestExpectedResults The sprite should be disabled when a window is deleted;
						The client thread should not be panic'd.
						The test should pass.
*/
void CTTSprite::SpriteOnWindowOrphanedTestsL()
	{
	TInt handles = 344;
	TInt loop = 0;
	TInt allocFailRate = 0;
	TInt err = KErrNone;

	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(500, 500), EColor16MU));
	TSpriteMember spritemember;
	spritemember.iBitmap = bitmap;
	spritemember.iMaskBitmap = NULL;	
	
	RWindowGroup group(TheClient->iWs);
	RWindow parent1(TheClient->iWs);
	RWindow child1(TheClient->iWs);
	RWindow testWindow(TheClient->iWs);
	RWsSprite sprite(TheClient->iWs);
	
	while (loop < 5)
		{
		TRect mainRect(TPoint(0,0), TSize(500,500));
		err = group.Construct(++handles, EFalse);	
		if (err == KErrNone)
			{
			TheClient->iWs.HeapSetFail(RHeap::EDeterministic, allocFailRate);

			//Create parent 1
			err = parent1.Construct(group,++handles);
			if (err == KErrNone)
				{
				parent1.SetExtent(mainRect.iTl, mainRect.Size());
				parent1.EnableRedrawStore(ETrue);
				parent1.Activate();
				}			
			}
		
		//Create child 1
		if (err == KErrNone)
			{
			TRect childRect(TPoint (10, 10), TSize (200, 150));
			err = child1.Construct(parent1,++handles);
			if (err == KErrNone)
				{
				child1.SetExtent(childRect.iTl, childRect.Size());
				child1.SetBackgroundColor(TRgb(128,100,255,20));
				child1.Activate();
				}
			}
		
		//Add sprite to child 1
		if (err == KErrNone) 
			{
			err = sprite.Construct(child1,TPoint(10,10), 0);
			if (err == KErrNone)
				{
				err = sprite.AppendMember(spritemember);
				if (err == KErrNone)
					err = sprite.Activate();	
				}
			}

		//Only delete parent 1, but not child 1	
		parent1.Close();		

		if (err == KErrNone) 
			{
			TRect testRect(TPoint(10, 30), TSize(200, 150));
			//Create testWindow
			err = testWindow.Construct(group,++handles);
			if (err == KErrNone)
				{
				testWindow.SetExtent(testRect.iTl, testRect.Size());
				testWindow.SetBackgroundColor(TRgb(128,100,255,20));
				testWindow.EnableRedrawStore(ETrue);
				testWindow.Activate();
				testWindow.BeginRedraw();
				testWindow.EndRedraw();	
				TheClient->Flush();			
				}
			}
		TheClient->iWs.HeapSetFail(RAllocator::ENone, 0);

		sprite.Close();
		child1.Close();
		testWindow.Close();
		group.Close();		
		++allocFailRate;
		loop = (err == KErrNone) ? loop + 1 : 0;
		}
	CleanupStack::PopAndDestroy(bitmap);
	}
void CTTSprite::ResizeMemberL()
	{
#ifndef __WINS__
	CTBlankWindow* win=new(ELeave) CTBlankWindow();
	CleanupStack::PushL(win);
	win->ConstructL(*TheClient->iGroup);
	win->SetExt(TPoint(),TSize(640,240));
	RWindowBase& window=*win->BaseWin();
	window.Activate();
	RWsSprite sprite(TheClient->iWs);
	CleanupClosePushL(sprite);
	User::LeaveIfError(sprite.Construct(window,TPoint(),0));
	TSpriteMember member;
	member.iMaskBitmap=NULL;
	member.iInvertMask=EFalse;
	member.iDrawMode=CGraphicsContext::EDrawModePEN;
	member.iOffset=TPoint();
	member.iInterval=TTimeIntervalMicroSeconds32(250000);
	member.iBitmap=&iBitmap;
	User::LeaveIfError(sprite.AppendMember(member));
	User::LeaveIfError(sprite.AppendMember(member));
	User::LeaveIfError(sprite.Activate());
	User::After(1000000); // // Interval is 1 sec.
	User::LeaveIfError(iBitmap.Resize(iBitmap.SizeInPixels() + TSize(100,100)));
	User::After(1000000);  // Interval is 1 sec.
	CleanupStack::PopAndDestroy(&sprite);
	CleanupStack::PopAndDestroy(win);
#endif // __WINS__
	}


void CTTSprite::SetUpSpriteL(RWsSprite &aSprite, RWindowTreeNode &aWindow,TPoint aPoint, TSpriteMember aMember, TInt aFlags/*=0*/)
	{
	User::LeaveIfError(aSprite.Construct(aWindow,aPoint,aFlags));
	User::LeaveIfError(aSprite.AppendMember(aMember));
	User::LeaveIfError(aSprite.Activate());
	}
/**
@SYMTestCaseID		GRAPHICS-WSERV-00-0001

@SYMDEF  			PDEF117721

@SYMTestCaseDesc    Test Screenrotation does not cause panic by sprite with NULL bitmap and NULL maskbitmap

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Defines two sprites and one with Null iBitmap and Null iMaskBitmap, then rotate the screen to 90 degrees

@SYMTestExpectedResults the screen rotation will not cause wserv a panic
*/
void CTTSprite::RotateNullSpriteL()
	{
	if (TheClient->iScreen->NumScreenModes() < 2)
		{
		LOG_MESSAGE(_L("WARNING: Unable to rotate screen"));
		return;
		}
// Sprite Members		
	TSpriteMember spriteMember1,spriteMember2;
	SetUpMember(spriteMember1);
	SetUpMember(spriteMember2);
	spriteMember2.iBitmap = NULL;
	
// Create windows
	RWindowGroup group(TheClient->iWs);
	User::LeaveIfError(group.Construct(ENullWsHandle,EFalse));	//Creates a Group Window
	CleanupClosePushL(group);
	RBlankWindow win(TheClient->iWs);
	User::LeaveIfError(win.Construct(group,ENullWsHandle));	//Creates a Blank Window
	CleanupClosePushL(win);
	win.SetVisible(ETrue);
	win.SetColor(TRgb::Gray4(2));
	win.Activate();
	TheClient->Flush();
	
// Create Sprites
	RWsSprite sprite1(TheClient->iWs);
	CleanupClosePushL(sprite1);
	SetUpSpriteL(sprite1, win,TPoint(20,70),spriteMember1,0);
	RWsSprite sprite2(TheClient->iWs);
	CleanupClosePushL(sprite2);
	SetUpSpriteL(sprite2, win,TPoint(80,130),spriteMember2,0);
	RWsSprite sprite3(TheClient->iWs);
	CleanupClosePushL(sprite3);
	SetUpSpriteL(sprite3, win,TPoint(2800,2130),spriteMember1,0);
	TheClient->Flush();

// Get the original screen mode	
	CWsScreenDevice* device = TheClient->iScreen;
	TInt originalScreenMode = device->CurrentScreenMode();
	TPixelsTwipsAndRotation originalModeSettings;
	device->GetScreenModeSizeAndRotation(originalScreenMode,originalModeSettings);

// Rotate screens	
	TPixelsAndRotation pixelsAndRotation;
	device->SetScreenMode( 1 );
	device->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
	device->SetCurrentRotations(1,pixelsAndRotation.iRotation);
	device->SetScreenSizeAndRotation(pixelsAndRotation);
	User::After(1000000);
	
	CleanupStack::PopAndDestroy(5,&group);
//	Restore the original screen mode	
	device->SetScreenMode(originalScreenMode);
	device->SetCurrentRotations(originalScreenMode,originalModeSettings.iRotation);
	device->SetScreenSizeAndRotation(originalModeSettings);
	TheClient->Flush();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-GCE-0700

@SYMDEF  			DEF123129

@SYMTestCaseDesc    Test activating a sprite twice does not cause the system to hang

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Construct a sprite and add a member to it. Activate twice.

@SYMTestExpectedResults The test should terminate smoothly and it should not hang the system.
*/
void CTTSprite::DoubleActivateL()
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(890, EFalse));
	CleanupClosePushL(group);
	
	RWsSprite sprite = RWsSprite(ws);
	User::LeaveIfError(sprite.Construct(group,TPoint(),0));
	CleanupClosePushL(sprite);
	CFbsBitmap* bitmap=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(10,12),EColor256));
	TSpriteMember member;
	member.iMaskBitmap=NULL;
	member.iInvertMask=EFalse;
	member.iDrawMode=CGraphicsContext::EDrawModePEN;
	member.iOffset=TPoint();
	member.iInterval=TTimeIntervalMicroSeconds32(0);
	member.iBitmap=bitmap;
	User::LeaveIfError(sprite.AppendMember(member));
	User::LeaveIfError(sprite.Activate());
	User::LeaveIfError(sprite.Activate());
	
	sprite.Close();
	CleanupStack::PopAndDestroy(4, &ws);
	
	group.Close();
	ws.Close();
	}

void CTTSprite::RunTestCaseL(TInt /*aCurTestCase*/)
	{	
	_LIT(KTest1, "General Tests");
	_LIT(KTest2, "Group Window Sprites Tests");
	_LIT(KTest3, "Lots Sprites Tests");
	_LIT(KTest4, "General Pointer Cursor Tests");
    _LIT(KTest5, "Pointer Cursor Visible Tests");
    _LIT(KTest6, "Update member tests");
    _LIT(KTest7, "Negative tests for Update member");
    _LIT(KTest8, "Window Group with Thread");
    _LIT(KTest9, "Resize Sprite Member Tests");
    _LIT(KTest10, "Rotate a NULL sprite");
    _LIT(KTest11, "Sprite On Window Orphaned Tests");
    _LIT(KTest12, "Sprite Double Activation Test");
	
	((CTTSpriteStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
	case 1:		
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0018"));
		TheClient->iWs.SetFocusScreen(0);
		iTest->LogSubTest(KTest1);
		GeneralTestsL();
		break;
	case 2:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0019"));
		iTest->LogSubTest(KTest2);
		GroupWindowSpritesL();
		break;
	case 3:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0020"));
		iTest->LogSubTest(KTest3);
		LotsSpritesL();
		break;
	case 4:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0021"));
		iTest->LogSubTest(KTest4);
		GeneralPointerCursor();
		break;
    case 5:
        ((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0498"));
        iTest->LogSubTest(KTest5);
        TRAPD(err, PointerCursorVisibleL());
        TEST(err == KErrNone);
        break;
	case 6:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-WSERV-0053-0001"));
		iTest->LogSubTest(KTest6);
		TRAP(err, SpriteUpdateMemberTestsL());
		TEST(err == KErrNone);
		break;
	case 7:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-WSERV-0054-0001"));
		iTest->LogSubTest(KTest7);
		TRAP(err, SpriteUpdateMemberNegTestsL());
		TEST(err == KErrNone);
		break;
	case 8:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-WSERV-0055-0001"));
		iTest->LogSubTest(KTest8);
		TRAP(err, FindWindowGroupThreadTestsL());
		TEST(err == KErrNone);
		break;
	case 9:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0526"));
		iTest->LogSubTest(KTest9);
		ResizeMemberL();
		break;
	case 10:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-00-0001"));
		iTest->LogSubTest(KTest10);
		RotateNullSpriteL();
		break;
	case 11:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0462"));
		iTest->LogSubTest(KTest11);
		SpriteOnWindowOrphanedTestsL();
		break;
	case 12:
		((CTTSpriteStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-GCE-0700"));
		iTest->LogSubTest(KTest12);
		DoubleActivateL();
		break;
	default:
		((CTTSpriteStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTTSpriteStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTTSpriteStep*)iStep)->RecordTestResultL();
	}

//--------------
__WS_CONSTRUCT_STEP__(TSprite)

