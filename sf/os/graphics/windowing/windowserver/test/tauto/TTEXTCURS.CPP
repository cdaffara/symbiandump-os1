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
// Test the text cursor.
//
// This suite of tests checks to see if the TextCursors are operating
// correctly for a number of use case scenarios; see doxygen comments
// for each sub-test.  This test suite is applicable on both winscw
// emulator and armv5 target hardware.  However, it must be noted that
// text cursors are special due to their timeliness.  The text cursor
// must flash every second: half a second ON, half a second OFF.  One
// consequence of this is that when the test suite is run on emulator,
// the PC must be otherwise quiescent.  No other IO or CPU intensive
// activities may occur on the system, because these will cause delays
// to the flashing of the text cursor giving unreliable results.
// Where timeliness is a consideration, we use TEST_SOFTFAIL_WINSCW so
// that if the test fails and we are running on the PC emulator, we only
// record the fact, but don't mark the test as failing.

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TTEXTCURS.H"
#include "graphics/windowserverconstants.h"

const TInt KNumberOfCustoTextCursors	= 3;
const TInt KTextCursorInitialIdValue	= 1001;
const TInt KTextCursorPanicUid1			= 200;
const TInt KTextCursorPanicUid2			= 2000;
const TInt KTextCursorPanicUid3			= 3000;
const TInt KTextCursorPanicUid4			= 4000;
const TInt KTextCursorPanicUid5			= 5000;

CTestBase* CTCursorTest::iStaticTest = NULL;
const TInt kWinWidth=400;
const TInt kWinHeight=100;
const TSize kWinSize(kWinWidth,kWinHeight);
const TInt kWinXPos=150;
const TInt kCursorWidth = 10;
const TInt kCursorHeight = 20;
const TSize kCursorSize(kCursorWidth,kCursorHeight);
const TPoint kWin1TopLeft(kWinXPos,0);
const TPoint kWin2TopLeft(kWinXPos,kWinHeight+10);

LOCAL_D void DeleteSpriteMember(TAny* aSpriteMember)
	{
	TSpriteMember* member=reinterpret_cast<TSpriteMember*>(aSpriteMember);
	delete member->iBitmap;
	member->iBitmap=NULL;
	delete member->iMaskBitmap;
	member->iMaskBitmap=NULL;
	}

CCustomTextCursor::~CCustomTextCursor()
	{
	const TInt count = iSpriteMemberArray.Count();
	for (TInt index=0; index<count; ++index)
		{
		DeleteSpriteMember(&iSpriteMemberArray[index]);
		}
	iSpriteMemberArray.Close();
	}
	
CCustomTextCursor::CCustomTextCursor(CTestBase* aTest)
	: iTest(aTest)
	{	
	}

void CCustomTextCursor::ConstructL(TInt aScreenNumber,TInt aBmpIndex)
	{
	ASSERT(aBmpIndex < KNumberOfCustoTextCursors);
	

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

	// create unique-id accross screens
	//
	iIdentifier = KTextCursorInitialIdValue + aScreenNumber*KNumberOfCustoTextCursors + aBmpIndex;
	iAlignment = (RWsSession::TCustomTextCursorAlignment)(aBmpIndex);
	}

CCustomTextCursor* CCustomTextCursor::CreateCustomTextCursorL(TInt aScreenNumber,TInt aBmpIndex,CTestBase* aTest)
	{
	CCustomTextCursor* customTextCursor = new (ELeave) CCustomTextCursor(aTest);
	CleanupStack::PushL(customTextCursor);
	customTextCursor->ConstructL(aScreenNumber,aBmpIndex);
	CleanupStack::Pop(customTextCursor);
	return customTextCursor;
	}

/*
 * Wrapper class for a list of custom text cursor.
 */
class CCustomTextCursorsWrapper : public CBase
	{
public:
	static CCustomTextCursorsWrapper* NewLC(TInt aScreenNumber,CTestBase* aTest);
	~CCustomTextCursorsWrapper();
	inline RPointerArray<CCustomTextCursor>& CustomTextCursorsArray();
	inline CCustomTextCursor& CustomTextCursor(TInt aIndex);
private:
	void ConstructL(TInt aScreenNumber,CTestBase* aTest);
private:
	RPointerArray<CCustomTextCursor> iCustomTextCursors;
	};

inline RPointerArray<CCustomTextCursor>& CCustomTextCursorsWrapper::CustomTextCursorsArray()
		{
		return iCustomTextCursors;
		}

inline CCustomTextCursor& CCustomTextCursorsWrapper::CustomTextCursor(TInt aIndex)
		{
		return *(iCustomTextCursors[aIndex]);
		}

CCustomTextCursorsWrapper* CCustomTextCursorsWrapper::NewLC(TInt aScreenNumber,CTestBase* aTest)
	{
	CCustomTextCursorsWrapper* self = new(ELeave) CCustomTextCursorsWrapper();
	CleanupStack::PushL(self);
	self->ConstructL(aScreenNumber,aTest);
	return self;
	}

CCustomTextCursorsWrapper::~CCustomTextCursorsWrapper()
	{
	iCustomTextCursors.ResetAndDestroy();
	iCustomTextCursors.Close();
	}

void CCustomTextCursorsWrapper::ConstructL(TInt aScreenNumber, CTestBase* aTest)
	{
	for (TInt index=0; index<KNumberOfCustoTextCursors; ++index)
		{
		CCustomTextCursor* customTextCursor=CCustomTextCursor::CreateCustomTextCursorL(aScreenNumber,index, aTest);
		CleanupStack::PushL(customTextCursor);
		User::LeaveIfError(iCustomTextCursors.Append(customTextCursor));
		CleanupStack::Pop(customTextCursor);
		}
	}

CTCursorTest::CTCursorTest(CTestStep* aStep) :
	CTWsGraphicsBase(aStep)
	{
	iCursorType = TTextCursor::ETypeFirst;
	iStaticTest=iTest;
	}
	
CTCursorTest::~CTCursorTest()
	{
	delete iWorkInProgress;
	delete iComparisonWindow;
	}

TInt CTCursorTest::DoPanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	if (ws.Connect()==KErrNone)
		{
		// use correct screen
		CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
		User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
		RWindowGroup group(ws);
		if (group.Construct(444)==KErrNone)
			{
			group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
			RWindow wnd(ws);
			if (wnd.Construct(group, TInt32(&ws))==KErrNone)
				{
				TTextCursor tc;
				tc.iHeight=10;
				tc.iAscent=5;
				tc.iWidth=10;
				tc.iFlags=0;
				tc.iColor=TRgb(0,0,0);
				switch(aInt)
					{
					case 0:
						{
						/* TESTCASE:	6.1
						* TITLE:		Invalid use of a custom text cursor ID (basic text cursor).
						* IMPORTANCE:	1
						* REQUIREMENT:	Unknown.
						* 
						* ACTION:		This test tries to set a text cursor using an ID which is invalid.
						* 
						* CHECK:		The thread should panic with the exit reason EWservPanicInvalidTextCursor
						*/ 
						tc.iType=(TTextCursor::EType)KTextCursorPanicUid1;
						group.SetTextCursor(wnd,TPoint(10,10),tc);
						}
						break;
					case 1:
						{
						/* TESTCASE:	6.2
						* TITLE:		Invalid use of a window for a text cursor.
						* IMPORTANCE:	1
						* REQUIREMENT:	REQ 1079, CR RDEF-5F7Q24 (10/04/2003).
						* 
						* ACTION:		This test tries to set a text cursor using a window which is not part
						*				of the window group calling the setting API.
						* 
						* CHECK:		The thread should panic with the exit reason EWservPanicWindow
						*/ 
						tc.iType=(TTextCursor::EType)KTextCursorPanicUid2;
						group.SetTextCursor(*TestWin->Win(),TPoint(10,10),tc);
						}
						break;
					case 2:
						{
						/* TESTCASE:	6.3
						* TITLE:		Invalid use of a custom text cursor ID.
						* IMPORTANCE:	1
						* REQUIREMENT:	REQ 1079, CR RDEF-5F7Q24 (10/04/2003).
						* 
						* ACTION:		This test tries to set a text cursor using an ID which is associated to
						*				an non-existing custom text cursor.
						* 
						* CHECK:		The thread should panic with the exit reason EWservPanicNoCustomTextCursor
						*/ 
						tc.iType=(TTextCursor::EType)KTextCursorPanicUid3;
						group.SetTextCursor(wnd,TPoint(10,10),tc);
						}
						break;
					case 3:
						{
						/* TESTCASE:	6.4
						* TITLE:		Invalid use of a custom text cursor ID.
						* IMPORTANCE:	1
						* REQUIREMENT:	REQ 1079, CR RDEF-5F7Q24 (10/04/2003).
						* 
						* ACTION:		This test tries to set a custom text cursor which has been set to use
						*				an invalid alignment.
						* 
						* CHECK:		The thread should panic with the exit reason EWservPanicCustomTextCursorAlign
						*/ 
						CCustomTextCursor* customTextCursor=NULL;
						TRAPD(error, customTextCursor=CCustomTextCursor::CreateCustomTextCursorL((TInt)aScreenNumber,0,iStaticTest));
						if (error==KErrNone)
							{
							error = ws.SetCustomTextCursor(KTextCursorPanicUid4, customTextCursor->iSpriteMemberArray.Array(), 0, (RWsSession::TCustomTextCursorAlignment)(RWsSession::ECustomTextCursorAlignBottom+1));
							if (error==KErrNone || error==KErrAlreadyExists)
								{
								tc.iType=(TTextCursor::EType)KTextCursorPanicUid4;
								group.SetTextCursor(wnd,TPoint(10,10),tc);
								}
							}
						delete customTextCursor;
						}
						break;
					case 4:
						{
						/* TESTCASE:	6.5
						* TITLE:		Use of an invalid custom text cursor
						* IMPORTANCE:	1
						* REQUIREMENT:	REQ 1079, CR RDEF-5F7Q24 (10/04/2003).
						* 
						* ACTION:		This test tries to set a custom text cursor which does not have
						*				any sprite member set.
						* 
						* CHECK:		The thread should panic with the exit reason EWservPanicNoSpriteMember
						*/ 
						RArray<TSpriteMember> spriteMemberArray;
						const TInt error = ws.SetCustomTextCursor(KTextCursorPanicUid5, spriteMemberArray.Array(), 0, (RWsSession::TCustomTextCursorAlignment)(RWsSession::ECustomTextCursorAlignBottom));
						if (error==KErrNone || error==KErrAlreadyExists)
							{
							tc.iType=(TTextCursor::EType)KTextCursorPanicUid5;
							group.SetTextCursor(wnd,TPoint(10,10),tc);
							}
						}
						break;
					case 5:
						{
						// Uncover set.cursor.iType < TTextCursor::ETypeFirst code path
						tc.iType=(TTextCursor::EType)TTextCursor::ETypeFirst - 1;
						group.SetTextCursor(wnd,TPoint(10,10),tc);
						}
						break;
					case 6:
						{
						// Uncover (set.cursor.iFlags&static_cast<TUint>(TTextCursor::EPrivateFlags) code path
						tc.iFlags=ETextCursorPrivateFlags;
						group.SetTextCursor(wnd,TPoint(10,10),tc);
						}
						break;
					case 7:
						{
						// Uncover (iGroupWin != searchWin) i.e. bogus group window
						tc.iType=(TTextCursor::EType)TTextCursor::ETypeRectangle;
						RWindow windowNotAssociatedWithAGroup(ws);
						group.SetTextCursor(windowNotAssociatedWithAGroup, TPoint(10,10),tc);
						}
						break;
					}
				}
			ws.Flush();
			}
		}
	return(EWsExitReasonBad);
	}

void CTCursorTest::TestPanicsL()
	{	
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicInvalidTextCursor, 0, (TAny*)iTest->iScreenNumber));
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicWindow, 1, (TAny*)iTest->iScreenNumber));
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicNoCustomTextCursor, 2, (TAny*)iTest->iScreenNumber));
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicCustomTextCursorAlign, 3, (TAny*)iTest->iScreenNumber));
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicNoSpriteMember, 4, (TAny*)iTest->iScreenNumber));
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicInvalidTextCursor, 5, (TAny*)iTest->iScreenNumber));
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicInvalidTextCursor, 6, (TAny*)iTest->iScreenNumber));
		TEST(iTest->TestWsPanicL(DoPanicTest, EWservPanicWindow, 7, (TAny*)iTest->iScreenNumber));
		iTest->CloseAllPanicWindows();
	}

void CTCursorTest::TextCursorSetLCoverageTests()
	{
	ValidateWin(BaseWin,TRgb::Gray256(255));
	ValidateWin(TestWin,TRgb::Gray256(255));	
	TTextCursor textCursor;
	textCursor.iHeight = 10;
	textCursor.iAscent = 0;
	textCursor.iWidth = 10;
	textCursor.iFlags = 0;
	textCursor.iColor = KRgbBlack;
	textCursor.iType = (TTextCursor::EType)TTextCursor::ETypeRectangle;
	TPoint position(10, 10);
	TRect clipRect0(10, 10, 10, 10);
	TRect clipRect1(10, 10, 5, 5);
	RWindowGroup *group = TheClient->iGroup->GroupWin(); 
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	/*
	 * Duplicate the previous SetTextCursor command to uncover the code which checks for any delta in SetL
	 * compared to the current settings.
	 */
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	/*
	 * Change the type only to pick up that difference in SetL.
	 */
	textCursor.iType++;
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	textCursor.iType--;
	/*
	 * Vary the clipping rectangle.
	 */
	group->SetTextCursor(*TestWin->Win(), position, textCursor, clipRect0);
	group->SetTextCursor(*TestWin->Win(), position, textCursor, clipRect1);
	/*
	 * Vary the color.
	 */
	textCursor.iColor = KRgbGreen;
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	textCursor.iColor = KRgbBlack;
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	/*
	 * Vary the target Window.
	 */
	group->SetTextCursor(*BaseWin->Win(), position, textCursor);
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	/*
	 * Vary the size of the cursor.
	 */
	textCursor.iWidth++;
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	textCursor.iWidth--;
	/*
	 * Set different custom cursors.
	 */
	CCustomTextCursorsWrapper* customTextCursorsWrapper = CCustomTextCursorsWrapper::NewLC(iTest->iScreenNumber, iTest);
	const TInt count = customTextCursorsWrapper->CustomTextCursorsArray().Count();
	for (TInt index=0; index<count; ++index)
		{
		CCustomTextCursor& customTextCursor = customTextCursorsWrapper->CustomTextCursor(index);
		textCursor.iType = customTextCursor.iIdentifier;
		group->SetTextCursor(*TestWin->Win(), position, textCursor);
		}			
	CleanupStack::PopAndDestroy(customTextCursorsWrapper);
	/*
	 * Set the last custom cursor from the above loop again so the
	 * product code sees the same Custom Text Cursor settings come
	 * in a second time.
	 */
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	textCursor.iType = (TTextCursor::EType)TTextCursor::ETypeRectangle;
	/*
	 * Vary the horizontal clipping.
	 */
	textCursor.iFlags = TTextCursor::EFlagClipHorizontal;
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	/*
	 * Vary the horizontal clipping.
	 */
	textCursor.iFlags = TTextCursor::EFlagClipVertical;
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	/*
	 * Try both horizontal and vertical clipping.
	 */
	textCursor.iFlags = TTextCursor::EFlagClipVertical|TTextCursor::EFlagClipHorizontal;
	group->SetTextCursor(*TestWin->Win(), position, textCursor);
	textCursor.iFlags = 0;
	
	TheClient->iWs.Flush();
	CancelTextCursor();
	}

void CTCursorTest::SetCursor(const TPoint &aPos,const TSize &aSize,TRgb aColor, const TRect &aRect, TUint aFlags)
	{
	TTextCursor tc;
	tc.iType=iCursorType;
    tc.iHeight=aSize.iHeight;
    tc.iAscent=aSize.iHeight*4/5;
    tc.iWidth=aSize.iWidth;
    tc.iFlags=aFlags;
	tc.iColor=aColor;
	TheClient->iGroup->GroupWin()->SetTextCursor(*TestWin->Win(),TPoint(aPos.iX,aPos.iY+tc.iAscent),tc,aRect);
	}

void CTCursorTest::SetCursor(const TPoint &aPos,const TSize &aSize,TRgb aColor, TUint aFlags)
	{
	TTextCursor tc;
	tc.iType=iCursorType;
    tc.iHeight=aSize.iHeight;
    tc.iAscent=aSize.iHeight*4/5;
    tc.iWidth=aSize.iWidth;
    tc.iFlags=aFlags;
	tc.iColor=aColor;
	TheClient->iGroup->GroupWin()->SetTextCursor(*TestWin->Win(),TPoint(aPos.iX,aPos.iY+tc.iAscent),tc);
	}

void CTCursorTest::SetCursorPlusBox(const TPoint &aPos,const TSize &aSize,TRgb aColor, const TRect *aClipRect, TUint aFlags)
	{
	if (aClipRect)
		SetCursor(aPos,aSize,aColor,*aClipRect,aFlags);
	else
		SetCursor(aPos,aSize,aColor,aFlags);
	TRect rect(aPos,aSize);
	if (aClipRect)
		rect.Intersection(*aClipRect);
	rect.Grow(2,2);
	
	TheClient->iGc->Activate(*(TestWin->Win()));
	TestWin->Invalidate(rect);
	TestWin->Win()->BeginRedraw(rect);
	TheClient->iGc->SetPenColor(aColor);
	TheClient->iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);

	TheClient->iGc->DrawRect(rect);
	TheClient->iGc->Deactivate();
	TestWin->Win()->EndRedraw();
	
	}

void CTCursorTest::CancelTextCursor()
	{
	TheClient->iGroup->GroupWin()->CancelTextCursor();
	}

void CTCursorTest::ConstructL()
	{
	// for allocating some cached memory
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Load(TEST_BITMAP_NAME, 0));
	CleanupStack::PopAndDestroy(bitmap);

	CCustomTextCursorsWrapper* customTextCursorsWrapper = CCustomTextCursorsWrapper::NewLC(iTest->iScreenNumber, iTest);
	const TInt count = customTextCursorsWrapper->CustomTextCursorsArray().Count();
	for (TInt index=0; index<count; ++index)
		{
		CCustomTextCursor& customTextCursor = customTextCursorsWrapper->CustomTextCursor(index);
		TInt err = TheClient->iWs.SetCustomTextCursor(customTextCursor.iIdentifier, customTextCursor.iSpriteMemberArray.Array(), customTextCursor.iSpriteFlags, customTextCursor.iAlignment);
		TEST(err == KErrNone || err == KErrAlreadyExists);
		if (err!=KErrNone && err != KErrAlreadyExists)
			INFO_PRINTF4(_L("TheClient->iWs.SetCustomTextCursor return value  - Expected: %d or %d, Actual: %d"), KErrNone, KErrAlreadyExists, err);

		__UHEAP_MARK;
		err = TheClient->iWs.SetCustomTextCursor(customTextCursor.iIdentifier, customTextCursor.iSpriteMemberArray.Array(), customTextCursor.iSpriteFlags, customTextCursor.iAlignment);
		__UHEAP_MARKEND;
		TEST(err == KErrAlreadyExists);
		if (err != KErrAlreadyExists)
			INFO_PRINTF3(_L("TheClient->iWs.SetCustomTextCursor return value  - Expected: %d, Actual: %d"), KErrAlreadyExists, err);

		}
	_LIT(KLog,"Text Cursor: Loaded %d Custom Cursors");
	TLogMessageText buf;
	buf.Format(KLog,count);
	TheClient->LogMessage(buf);
	CleanupStack::PopAndDestroy(customTextCursorsWrapper);
//
	ValidateWin(BaseWin,TRgb::Gray256(204));
	ValidateWin(TestWin,TRgb::Gray256(204));
//
	SetCursor(TPoint(10,90),TSize(80,100),TRgb(255,255,255));
//
	iWinState=0;
	iWinPos=TPoint(2*TheClient->iGroup->Size().iWidth/3,0);
//
	iMoveWin=new(ELeave) CBlankWindow(TRgb::Gray256(220));
	TDisplayMode mode=EGray16;
	TInt testWinWidth = TestWin->Size().iWidth;
	TInt halfTestWinWidth = testWinWidth/2;
	TInt halfTestWinHeight = TestWin->Size().iHeight/2;
	
	iMoveWin->SetUpL(iWinPos,TSize(halfTestWinHeight,halfTestWinHeight),
			TheClient->iGroup,*TheClient->iGc,&mode);

	iCheckWin=new(ELeave) CBlankWindow(TRgb::Gray256(220));
	iCheckWin->SetUpL(TPoint(testWinWidth+halfTestWinWidth,halfTestWinHeight),
			TSize(halfTestWinWidth,halfTestWinHeight),
			TheClient->iGroup,*TheClient->iGc,&mode);
	}

void CTCursorTest::DeleteMoveWindows()
	{
	delete iMoveWin;
	delete iCheckWin;
	CancelTextCursor();
	}

void CTCursorTest::ResetMoveWindowsL()
	{
	SetCursor(TPoint(10,90),TSize(80,100),TRgb(255,255,255));
	iWinState=0;
	iWinPos=TPoint(2*TheClient->iGroup->Size().iWidth/3,0);
	iMoveWin->SetExtL(iWinPos,TSize(TestWin->Size().iWidth/2,TestWin->Size().iHeight/2));
	iCheckWin->SetExtL(TPoint(TestWin->Size().iWidth+(TestWin->Size().iWidth>>1),TestWin->Size().iHeight>>1),
														TSize(TestWin->Size().iWidth/2,TestWin->Size().iHeight/2));
	}

TBool CTCursorTest::MoveWindow()
	{
	TSize scrSize(TheClient->iScreen->SizeInPixels());
	iWinState++;
	if (iWinState<20)
		iWinPos+=TPoint((4*scrSize.iWidth)/640,(4*scrSize.iHeight)/240);
	else if (iWinState<40)
		iWinPos+=TPoint((1*scrSize.iWidth)/640,(-3*scrSize.iHeight)/240);
	else if (iWinState<60)
		iWinPos+=TPoint((-6*scrSize.iWidth)/640,(3*scrSize.iHeight)/240);
	else
		iWinPos+=TPoint((1*scrSize.iWidth)/640,(-2*scrSize.iHeight)/240);
	iMoveWin->SetPos(iWinPos);
	return (iWinState==80);
	}

void CTCursorTest::ValidateWin(TestWindow *aWin, TRgb aColor)
	{
	aWin->Win()->Invalidate();
	RedrawWin(*aWin->Win(),aColor);
	}

void CTCursorTest::RedrawWin(RWindow &aWin, TRgb aColor)
	{
	aWin.BeginRedraw();
	TheClient->iGc->Activate(aWin);
	TheClient->iGc->SetBrushColor(aColor);
	TheClient->iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheClient->iGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheClient->iGc->Clear();
	TheClient->iGc->Deactivate();
	aWin.EndRedraw();
	}

void CTCursorTest::ScrollTest()
	{
	const TSize size(20,40);
	ValidateWin(TestWin,TRgb::Gray256(255));

	SetCursor(TPoint(10,20),size,TRgb::Gray256(255),TTextCursor::EFlagNoFlash);
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();

	for(TInt ii=0;ii<20;ii++)
		{
		TInt dist=(ii&3)*2;
		TInt nx=ii&0x1?1:-1;
		TInt ny=ii&0x2?1:-1;
		TestWin->Win()->Scroll(TPoint(dist*nx,dist*ny),TRect(10,20,30,40));
		TheClient->iWs.Flush();
		}
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();

	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheClient->iGc->Activate(*(BaseWin->Win()));
	TheClient->iGc->Clear();
	TheClient->iGc->SetBrushColor(TRgb::Gray256(255));
	TheClient->iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheClient->iGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheClient->iGc->Clear(TRect(TPoint(10,20),size));
	TheClient->iGc->Deactivate();
	BaseWin->Win()->EndRedraw();

	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();

	/*
	 * NOTE: Reason for removal of COMPARE_WINDOWS_SOFTFAIL_WINSCW
	 * Due to the new implementation of sprites in wserv2, the sprites no longer keep a 
	 * backup bitmap of what the screen looks like beneath them. As it is not possible to 
	 * move the sprites associated with the custom text cursors that were created in second 
	 * phase construction of the CTCursorTest object, the COMPARE_WINDOWS_SOFTFAIL_WINSCW; 
	 * macro function has been removed. Otherwise the test case is going to subject to the
	 * timing of the flashing sprite. An alternative solution would be to assign NULL values
	 * to the sprite bitmaps in second phase construction, but this is avoided as it would
	 * trigger failures in some test cases later on (that are depended on these "embedded"
	 * sprite images).
	 */	
	CancelTextCursor();
	}

void DrawTestSprite(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 2));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : 0));
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 3 : 1));
	aGc->DrawEllipse(TRect(aSize));
	}

CTSprite *CTCursorTest::CreateTestSpriteLC(RWindowTreeNode &aWindow, const TPoint &aPos, TInt aCount)
//
// At the moment aCount must be 1 or 2
//
	{
	TSpriteCreateParams params(TSize(30,70),TPoint(0,0),DrawTestSprite);
	TSpriteCreateParams paramarray[2];
	params.iInterval=TTimeIntervalMicroSeconds32(200000);

	paramarray[0]=params;
	paramarray[1]=params;
	paramarray[1].iSize=TSize(100,10);
	CTSprite *sprite=new(ELeave) CTSprite(TheClient->iWs);
	CleanupStack::PushL(sprite);
	sprite->ConstructL(aWindow,aPos,aCount,&paramarray[0],0);
	return(sprite);
	}

void CTCursorTest::doMoveWindowTestL()
	{
	RBlankWindow blankwin(TheClient->iWs);
	User::LeaveIfError(blankwin.Construct(*TestWin->Win(),1));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&blankwin));
//
	blankwin.SetExtent(TPoint(35,165),TSize(40,40));
	blankwin.SetColor(TRgb::Gray256(220));
	blankwin.Activate();
	TheClient->iWs.SetAutoFlush(ETrue);
	User::After(500000);
	blankwin.SetPosition(TPoint(25,55));
	User::After(500000);
	blankwin.SetPosition(TPoint(30,160));
	User::After(500000);
	blankwin.SetPosition(TPoint(12,22));	// Almost totally covering sprite
	User::After(500000);
	blankwin.SetPosition(TPoint(-100,-100));	// Totally off the sprite
	User::After(500000);
	blankwin.SetPosition(TPoint(10,20));	// Write on top of sprite
	User::After(500000);
	blankwin.SetPosition(TPoint(24,24));	// moving off...
	User::After(500000);
	blankwin.SetPosition(TPoint(38,28));	// ...
	User::After(500000);
	blankwin.SetPosition(TPoint(58,48));	// ...
	User::After(500000);
	blankwin.SetPosition(TPoint(92,62));	// ... off
	User::After(500000);
	CleanupStack::PopAndDestroy();	// blank window
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();

	TheClient->iWs.SetAutoFlush(EFalse);
	}

void CTCursorTest::MoveWindowTest1L()
	{
	ValidateWin(TestWin,TRgb::Gray256(255));
// Check it with a static sprite
	CTSprite * sprite_static = CreateTestSpriteLC(*TestWin->Win(), TPoint(10,20), 1);
	doMoveWindowTestL();
	(sprite_static->Sprite()).SetPosition(TPoint(500,500)); //move the sprite out of the viewing area before the window comparison 
	CleanupStack::PopAndDestroy(1);	// sprite
// Check it an animated sprite
	CTSprite * sprite_anim = CreateTestSpriteLC(*TestWin->Win(), TPoint(10,20), 2);
	doMoveWindowTestL();
	(sprite_anim->Sprite()).SetPosition(TPoint(500,500)); //move the sprite out of the viewing area before the window comparison
	CleanupStack::PopAndDestroy(1);	// sprite
	}

void CTCursorTest::MoveWindowTest2L()
	{
	const TSize size(20,40);
// Check it with a text cursor
	ValidateWin(TestWin,TRgb::Gray256(255));
	SetCursor(TPoint(10,25),size,TRgb::Gray256(255),TTextCursor::EFlagNoFlash);
	doMoveWindowTestL();
	CancelTextCursor();
// Check it with an anaimated sprite and a text cursor
	ValidateWin(TestWin,TRgb::Gray256(255));
	CTSprite * sprite_anim = CreateTestSpriteLC(*TestWin->Win(), TPoint(10,20), 2);
	SetCursor(TPoint(10,45),size,TRgb::Gray256(255),TTextCursor::EFlagNoFlash);
	doMoveWindowTestL();
	(sprite_anim->Sprite()).SetPosition(TPoint(500,500));
	CancelTextCursor();
	CleanupStack::PopAndDestroy(1);	// sprite
	}

TBool CTCursorTest::IncrementCursorType()
	{
	// each screen has it own set of cursor
	//
	// the values would be ETypeLast=2 ETypeLastBasic=1009
	//
	if (iCursorType == TTextCursor::ETypeFirst)
		{
		iCursorType = (TTextCursor::EType)(TTextCursor::ETypeLastBasic + 1 + iTest->iScreenNumber*KNumberOfCustoTextCursors);
		return ETrue;
		}
	else if (iCursorType >= TTextCursor::ETypeLastBasic + (iTest->iScreenNumber+1)*KNumberOfCustoTextCursors)
		{
		iCursorType = TTextCursor::ETypeFirst;
		return EFalse;
		}
	else
		{
		iCursorType = (TTextCursor::EType)(iCursorType + 1);
		return ETrue;
		}
	}

void CTCursorTest::GeneralTestsL()
	{
	const TInt winColor=255;		//Best to use Light Grey that is 170, but this code is bugged and doing so shows them up.
	ValidateWin(BaseWin,TRgb::Gray256(255));
	ValidateWin(TestWin,TRgb::Gray256(255));
	SetCursor(TPoint(-1000,10),TSize(10,30),TRgb::Gray256(255));
	TheClient->iWs.Flush();
	SetCursor(TPoint(10,10),TSize(10,30),TRgb::Gray256(255));
	TheClient->iWs.Flush();
	TRect rect(15,15,18,25);
	SetCursorPlusBox(TPoint(10,10),TSize(10,30),TRgb::Gray256(255), &rect);
	CancelTextCursor();
//
	ValidateWin(BaseWin,TRgb::Gray256(255));
	ValidateWin(TestWin,TRgb::Gray256(255));
	TheClient->iWs.Flush();
	for(TInt winType=0;winType<3;winType++)
		{
		RWindowBase *cursorwin=NULL;
		RBackedUpWindow backcursorwin(TheClient->iWs);
		RWindow backwindow(TheClient->iWs);
		RBlankWindow backblankwin(TheClient->iWs);
		switch(winType)
			{
			case 0:
				cursorwin=&backcursorwin;
				User::LeaveIfError(backcursorwin.Construct(*TestWin->BaseWin(),EGray4,1));
				break;
			case 1:
				cursorwin=&backwindow;
				User::LeaveIfError(backwindow.Construct(*TestWin->BaseWin(),1));
				break;
			case 2:
				cursorwin=&backblankwin;
				User::LeaveIfError(backblankwin.Construct(*TestWin->BaseWin(),1));
				break;
			}
		CleanupStack::PushL(TCleanupItem(CleanUpWindow,cursorwin));
		User::LeaveIfError(cursorwin->SetSizeErr(TestWin->BaseWin()->Size()));
		cursorwin->Activate();
//
		TTextCursor tc;
		tc.iType=iCursorType;
		tc.iHeight=30;
		tc.iAscent=0;
		tc.iWidth=50;
		tc.iFlags=0;
		tc.iColor=TRgb::Gray256(255);
		TheClient->iGroup->GroupWin()->SetTextCursor(*cursorwin,TPoint(10,10),tc);
//
		CreateTestSpriteLC(*cursorwin, TPoint(10,20), 2);
//
		if (cursorwin==&backwindow)
			RedrawWin(backwindow,TRgb::Gray256(255));
		for(TInt count=0;count<9;count++)
			{
			RWindowBase *pwin=NULL;
			RBackedUpWindow backedup(TheClient->iWs);
			RWindow window(TheClient->iWs);
			RBlankWindow blankwin(TheClient->iWs);
			switch(count%3)
				{
				case 0:
					pwin=&window;
					window.Construct(*cursorwin,2);
					window.SetBackgroundColor(TRgb(winColor,winColor,winColor));
					break;
				case 1:
					pwin=&backedup;
					backedup.Construct(*cursorwin,EGray4,2);
					break;
				case 2:
					pwin=&blankwin;
					blankwin.Construct(*cursorwin,2);
					blankwin.SetColor(TRgb(winColor,winColor,winColor));
					break;
				}
			CleanupStack::PushL(TCleanupItem(CleanUpWindow,pwin));
			pwin->SetExtentErr(TPoint(30,30),TSize(50,80));
			pwin->Activate();
			TheClient->iWs.Flush();
			CleanupStack::PopAndDestroy();	// window
			if (cursorwin==&backwindow)
				RedrawWin(backwindow,TRgb::Gray256(255));
			TheClient->iWs.Flush();
			TheClient->WaitForRedrawsToFinish();
			COMPARE_WINDOWS_SOFTFAIL_WINSCW;
			User::After(200000);	// Wait a fifth of a second to make sure the test is run during different states of flashing
			}
		for(TInt count2=0;count2<4;count2++)
			{
			cursorwin->SetPosition(TPoint(10,5));
			TheClient->iWs.Flush();
			User::After(100000);
			cursorwin->SetPosition(TPoint(5,10));
			TheClient->iWs.Flush();
			User::After(100000);
			cursorwin->SetPosition(TPoint(0,0));
			TheClient->iWs.Flush();
			User::After(100000);
			TheClient->WaitForRedrawsToFinish();
			COMPARE_WINDOWS_SOFTFAIL_WINSCW;
			}
		CleanupStack::PopAndDestroy(2);	// sprite & window containing sprite and cursor
		}
	CancelTextCursor();
	}

void CTCursorTest::INC040489L()
	{
	INFO_PRINTF1(_L("AUTO_TCur INC040489 "));
	RWindowGroup group1(TheClient->iWs);
	PushWindowL(&group1);
	User::LeaveIfError(group1.Construct(ENullWsHandle));
	RBlankWindow blank1(TheClient->iWs);
	PushWindowL(&blank1);
	User::LeaveIfError(blank1.Construct(group1,ENullWsHandle));
	blank1.SetRequiredDisplayMode(EColor4K);
	blank1.SetColor(TRgb(250,150,0));
	blank1.Activate();
	RWindowGroup group2(TheClient->iWs);
	PushWindowL(&group2);
	User::LeaveIfError(group2.Construct(ENullWsHandle));
	RBlankWindow blank2(TheClient->iWs);
	PushWindowL(&blank2);
	User::LeaveIfError(blank2.Construct(group2,ENullWsHandle));
	blank2.SetRequiredDisplayMode(EColor4K);
	blank2.SetColor(TRgb(75,200,125));
	blank2.Activate();
	TheClient->Flush();
	INFO_PRINTF1(_L(" Created Windows "));
	TTextCursor tc;
	tc.iType=KTextCursorInitialIdValue + iTest->iScreenNumber*KNumberOfCustoTextCursors;
	tc.iHeight=80;
	tc.iAscent=10;
	tc.iWidth=30;
	tc.iFlags=0;
	tc.iColor=TRgb::Gray256(255);
	INFO_PRINTF1(_L(" About to Set Text Cursor 1 "));
	group2.SetTextCursor(blank2,TPoint(20,20),tc);
	TheClient->Flush();
	INFO_PRINTF1(_L(" Set Text Cursor 1 "));
	User::After(2000000);		//2sec
	TheClient->iWs.PrepareForSwitchOff();
	TheClient->Flush();
	User::After(2000000);		//2sec
	group1.SetOrdinalPosition(0);
	group2.CancelTextCursor();
	TheClient->Flush();
	INFO_PRINTF1(_L(" Canceled Text Cursor "));
	User::After(2000000);		//2sec
	//
	// Before applying the fix, the following operations makes the Custom Text 
	// Cursor Sprite invisible (happens even without wserv heartbeat suppression)
	INFO_PRINTF1(_L(" About to Set Text Cursor 2 "));
	group1.SetOrdinalPosition(2);
	group2.SetTextCursor(blank2,TPoint(20,20),tc);
	TheClient->Flush();
	INFO_PRINTF1(_L(" Set Text Cursor 2 "));
	User::After(2000000);		//2sec
	TRawEvent event;
	event.Set(TRawEvent::EActive);
	TheClient->iWs.SimulateRawEvent(event);
	TheClient->Flush();
	INFO_PRINTF1(_L(" Simulated Active Event "));
	User::After(2000000);		//2sec
	CleanupStack::PopAndDestroy(4, &group1);
	INFO_PRINTF1(_L(" End of test "));
	}

void CTCursorTest::CursorUpdatedBeforeWindowRenderedL()
	{
	INFO_PRINTF1(_L("CursorUpdatedBeforeWindowRenderedL"));
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0);
	// We use some unique looking colors otherwise its harder
	// to spot which test is which
	TRgb kAqua(134, 242, 251);
	
	iWorkInProgress = new(ELeave) CBlankWindow(kAqua);
	iComparisonWindow = new(ELeave) CBlankWindow(kAqua);
	CancelTextCursor();
	
	const TSize screenSize=TheClient->iGroup->Size();
	const TInt kPad = 5;
	const TInt kThirdOfScreenWidth = screenSize.iWidth/3;
	const TInt kWinWidth = kThirdOfScreenWidth - 2*kPad;
	const TInt kWinHeight = screenSize.iHeight - 2*kPad;
	const TSize kWinSize(kWinWidth, kWinHeight);
	const TPoint kCursorPos(30, 30);
	iComparisonWindow->SetUpL(TPoint(2*kThirdOfScreenWidth + kPad, kPad), kWinSize, TheClient->iGroup, *TheClient->iGc);
	iWorkInProgress->SetUpL(  TPoint(  kThirdOfScreenWidth + kPad, kPad), kWinSize, TheClient->iGroup, *TheClient->iGc);
	
	TTextCursor nonFlashingCursor;
	nonFlashingCursor.iType = TTextCursor::ETypeRectangle;
	nonFlashingCursor.iHeight=kCursorHeight;
	nonFlashingCursor.iAscent=0;
	nonFlashingCursor.iWidth=kCursorWidth;
	nonFlashingCursor.iFlags=TTextCursor::EFlagNoFlash;
	nonFlashingCursor.iColor = KRgbBlack; 
	TheClient->iGroup->GroupWin()->SetTextCursor(*iWorkInProgress->BaseWin(), kCursorPos, nonFlashingCursor);
	
	// Up till this point, there has not been a CWsWindow::Render() for iWorkInProgress
	// because the window has not been invalid
	
	/*
	 * Here is the crux of the test.  We want to create the following condition in a window group:
	 * 1) None of its windows have yet been Rendered using CWsWindow::Render()
	 * 2) A text cursor is present
	 * 3) Focus is lost then received
	 * 
	 * It used to be the case that Wserv picked up the handle to the Render Stage Text Cursor
	 * drawer upon a Refresh caused by Rendering the window.  But drawing the Text Cursor could
	 * come either from a Window Render or a change to the state of the Text Cursor, such as
	 * receiving focus in the window.  A bug was experienced when the Text Cursor was drawn in
	 * a window which never had been rendered.  This meant that the handle was not set up causing
	 * an assert.
	 * 
	 * The code has been modified since then, to setup the handle to the Render Stage Text
	 * Cursor during Wserv initialisation.  However, to guard against future changes, its
	 * worthwhile to have this corner case test to ensure it is possible to receive focus
	 * in a window which has never been rendered.  That is because the text cursor state
	 * is updated in such circumstances, and that might trigger a draw of the cursor in a
	 * future version of the text cursor code.
	 */
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(1);  // lose focus
	TheClient->iWs.Finish();
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0); // gain focus
	TheClient->iWs.Finish();
	
	// If we get this far without a panic or assert, we have passed this test.
	
	// Now allow CWsWindow::Render() to occur in iWorkInProgress
	// The reason for doing this is so that when you watch the test
	// progress you can see visual confirmation via the progress message and
	// the coloured windows appear on the screen.  Otherwise you would think
	// that the test had either been skipped or had broken.
	iWorkInProgress->Invalidate();
	iWorkInProgress->Redraw();
	iComparisonWindow->Invalidate();
	iComparisonWindow->Redraw();
	TheClient->iWs.Finish();
	delete iWorkInProgress;
	iWorkInProgress = NULL;
	delete iComparisonWindow;
	iComparisonWindow = NULL;
	INFO_PRINTF1(_L("End of test"));
	}

void CTCursorTest::INC097774()
	{
	TTimeIntervalMicroSeconds32 initialRepeatRate;
	TTimeIntervalMicroSeconds32 repeatRate;
	TheClient->iWs.GetKeyboardRepeatRate(initialRepeatRate,repeatRate);
	
	//simulates a text cursor moving across the screen as if a user was holding down
	//a key to scroll the cursor through a section of text.
	//before applying the fix the cursor only shows up intermittently instead of smoothly
	//scrolling across the screen.
	const TSize cursorSize(3,20);
	const TInt moveInterval=10;
	SetCursor(TPoint(0,20),cursorSize,TRgb::Gray256(255),TTextCursor::EFlagNoFlash);
	TheClient->Flush();
	User::After(initialRepeatRate);
	for(TInt offset=10;offset<=100;offset+=moveInterval)
		{
		SetCursor(TPoint(offset,20),cursorSize,TRgb::Gray256(255),TTextCursor::EFlagNoFlash);
		TheClient->Flush();
		User::After(repeatRate);
		}
		
	//simulate clipped text cursor moving accross the screen
	TRect rect(0,20,3,40);
	SetCursor(TPoint(0,20),cursorSize,TRgb::Gray256(255),rect,TTextCursor::EFlagNoFlash);
	TheClient->Flush();
	User::After(initialRepeatRate);
	for(TInt offset=10;offset<=100;offset+=moveInterval)
		{
		rect.Move(moveInterval,0);
		SetCursor(TPoint(offset,20),cursorSize,TRgb::Gray256(255),rect,TTextCursor::EFlagNoFlash);
		TheClient->Flush();
		User::After(repeatRate);
		}	
	}
/** What happens when a cursor becomes off-screen when the screen is resized/rotated? 
 * 
 * 
 **/
void CTCursorTest::INC117232()
	{
	const TInt initialRepeatRate=300000;	// 3/10 seconds should be long enough to update everything!
	TInt currMode=TheClient->iScreen->CurrentScreenMode();
	TInt testMode=currMode;
	TPixelsTwipsAndRotation currModeSize;
	TheClient->iScreen->GetScreenModeSizeAndRotation(currMode, currModeSize);
	//find a (rotated) mode where the dimensions of the screen shrank
	for (TInt mode=0;mode<TheClient->iScreenModes.Count();mode++)
		{
		TPixelsTwipsAndRotation testModeSize;
		TheClient->iScreen->GetScreenModeSizeAndRotation(mode,testModeSize);
		if (	testModeSize.iPixelSize.iWidth<currModeSize.iPixelSize.iWidth-10
			||	testModeSize.iPixelSize.iHeight<currModeSize.iPixelSize.iHeight-10
			)
			{
			testMode=mode;
			break;
			}
		}
	if (testMode==currMode)
		{
		_LIT(KLog,"No smaller screen-size modes available - INC117232 test skipped");
		LOG_MESSAGE(KLog);
		//iStep->SetTestStepResult(EInconclusive);		//With this line the whole test fails which is too drastic
		return;
		}
	//enable a cursor on the bottom right corner of the screen
	TestWin->SetFullScreenExtL();
	TheClient->Flush();
	iCursorType=TTextCursor::ETypeRectangle;
	SetCursor(TPoint(-20,-20)+TestWin->Size(),TSize(40,40),KRgbDarkMagenta,TTextCursor::EFlagNoFlash);
	TheClient->Flush();
	User::After(initialRepeatRate);
	//shrink the screen
	TheClient->iScreen->SetScreenMode(testMode);
	TheClient->iScreen->SetAppScreenMode(testMode);
	//The defect was that WServ would now crash! 
	TheClient->Flush();
	User::After(initialRepeatRate);
	//Set everything back
	TheClient->iScreen->SetScreenMode(currMode);
	TheClient->iScreen->SetAppScreenMode(currMode);
	TheClient->Flush();
	User::After(initialRepeatRate);
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
/**
 * Sets up a text cursor whose attributes indicate it should not be flashing
 * and then checks to ensure this is the actual behaviour.
 */
void CTCursorTest::TextCursorNoFlashTestL()
	{
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0);
	// We use some unique looking colors otherwise its harder
	// to spot which test is which
	TRgb kGentleYellow(251, 249, 198);
	TRgb kGentlePink(253, 196, 221);
	
	iWorkInProgress = new(ELeave) CBlankWindow(kGentleYellow);
	iComparisonWindow = new(ELeave) CBlankWindow(kGentleYellow);
	CancelTextCursor();
	TheClient->Flush();
	
	const TSize screenSize=TheClient->iGroup->Size();
	const TInt kPad = 5;
	const TInt kThirdOfScreenWidth = screenSize.iWidth/3;
	const TInt kWinWidth = kThirdOfScreenWidth - 2*kPad;
	const TInt kWinHeight = screenSize.iHeight - 2*kPad;
	const TSize kWinSize(kWinWidth, kWinHeight);
	const TPoint kCursorPos(30, 30);
	iWorkInProgress->SetUpL(  TPoint(  kThirdOfScreenWidth + kPad, kPad), kWinSize, TheClient->iGroup, *TheClient->iGc);
	iComparisonWindow->SetUpL(TPoint(2*kThirdOfScreenWidth + kPad, kPad), kWinSize, TheClient->iGroup, *TheClient->iGc);
	
	TTextCursor nonFlashingCursor;
	
	nonFlashingCursor.iType = TTextCursor::ETypeRectangle;
	nonFlashingCursor.iHeight=kCursorHeight;
	nonFlashingCursor.iAscent=0;
	nonFlashingCursor.iWidth=kCursorWidth;
	nonFlashingCursor.iFlags=TTextCursor::EFlagNoFlash;
	nonFlashingCursor.iColor = kGentlePink; // We expect a Flicker Buffer Render Stage to ignore this color
	
	iWorkInProgress->Invalidate();
	iWorkInProgress->Redraw();
	iComparisonWindow->Invalidate();
	iComparisonWindow->Redraw();
	
	DrawTextCursorSimilarToRenderStage(*TheClient->iGc, *iComparisonWindow->Win(), kCursorPos, nonFlashingCursor);
	TheClient->iGroup->GroupWin()->SetTextCursor(*iWorkInProgress->BaseWin(), kCursorPos, nonFlashingCursor);
	TheClient->Flush();
		
	CheckCursorDoesNotFlash(iWorkInProgress->BaseWin()->Size());
	
	delete iWorkInProgress;
	iWorkInProgress = NULL;
	delete iComparisonWindow;
	iComparisonWindow = NULL;
	}

void CTCursorTest::TextCursorFlashTestL()
	{
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0);
	// We use some unique looking colors otherwise its harder
	// to spot which test is which
	TRgb kMildPurple(218, 155, 244);
	TRgb kPaleGreen(146, 190, 12);
	
	iWorkInProgress = new(ELeave) CBlankWindow(kMildPurple);
	iComparisonWindow = new(ELeave) CBlankWindow(kMildPurple);
	CancelTextCursor();
	TheClient->Flush();
	
	const TSize screenSize=TheClient->iGroup->Size();
	const TInt kPad = 5;
	const TInt kThirdOfScreenWidth = screenSize.iWidth/3;
	const TInt kWinWidth = kThirdOfScreenWidth - 2*kPad;
	const TInt kWinHeight = screenSize.iHeight - 2*kPad;
	const TSize kWinSize(kWinWidth, kWinHeight);
	const TPoint kCursorPos(30, 30);
	iWorkInProgress->SetUpL(  TPoint(  kThirdOfScreenWidth + kPad, kPad), kWinSize, TheClient->iGroup, *TheClient->iGc);
	iComparisonWindow->SetUpL(TPoint(2*kThirdOfScreenWidth + kPad, kPad), kWinSize, TheClient->iGroup, *TheClient->iGc);
	
	TTextCursor flashingCursor;
	
	flashingCursor.iType = TTextCursor::ETypeRectangle;
	flashingCursor.iHeight=kCursorHeight;
	flashingCursor.iAscent=0;
	flashingCursor.iWidth=kCursorWidth;
	flashingCursor.iFlags=0; // implies that cursor SHOULD flash
	flashingCursor.iColor = kPaleGreen; // We expect a Flicker Buffer Render Stage to ignore this color

	iWorkInProgress->Invalidate();
	iWorkInProgress->Redraw();
	iComparisonWindow->Invalidate();
	iComparisonWindow->Redraw();
	
	DrawTextCursorSimilarToRenderStage(*TheClient->iGc, *iComparisonWindow->Win(), kCursorPos, flashingCursor);
	TheClient->iGroup->GroupWin()->SetTextCursor(*iWorkInProgress->BaseWin(), kCursorPos, flashingCursor);
	TheClient->Flush();
	
	CheckCursorDoesFlash(kCursorPos, flashingCursor, kMildPurple);
	CancelTextCursor();
	TheClient->Flush();
	
	delete iWorkInProgress;
	iWorkInProgress = NULL;
	delete iComparisonWindow;
	iComparisonWindow = NULL;
	}

void CTCursorTest::DrawTextCursorSimilarToRenderStage(CWindowGc& aGc, RWindow& aWin, const TPoint& aPos, const TTextCursor& aTextCursor)
	{
	// This method duplicates the way in which the default FlickerBuffer Render
	// Stage draws a Text Cursor of ETypeRectangle.  @see CFbRenderStage::DrawTextCursor
	// This code must be kept in sync with the FlickerBuffer Render Stage
	
	ASSERT(aTextCursor.iType == TTextCursor::ETypeRectangle);
	const TRect updatedRegion(aPos,TSize(aTextCursor.iWidth,aTextCursor.iHeight));
	aWin.Invalidate();
	aWin.BeginRedraw();
	aGc.Activate(aWin);
	aGc.Clear();
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbBlack);
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.Clear(updatedRegion);
	aGc.Deactivate();
	aWin.EndRedraw();
	}

void CTCursorTest::CheckCursorDoesNotFlash(const TSize& aSize)
	{
	const TInt kSampleTime = 100000; // one tenth of a second
	const TInt kSampleLimit = 100;
	TInt sampleIteration = 0;
	TBool comparisonOkay = EFalse;
	
	while (sampleIteration < kSampleLimit)
		{	
		comparisonOkay = DoCheckRect(iWorkInProgress, iComparisonWindow, TRect(TPoint(), aSize), CWsScreenDevice::EIncludeTextCursor);
		if (!comparisonOkay)
			{
			INFO_PRINTF2(_L("CheckCursorDoesNotFlash difference found after %d milliseconds"), sampleIteration*100);
			break;
			}
		sampleIteration++;
		User::After(kSampleTime);
		}
	TEST(comparisonOkay);
	}

void CTCursorTest::UpdateCountersOnCursorTransition(
		const TBool	aTransitionedToOn,
		TTime& 		aNow,
		TInt64&		aDeltaTime,
		TTime&		aLastDeltaTime,
		TInt&		aWarmUpIterations,
		const TInt&	aFlashChangeTime,
		const TInt&	aToleranceMargin,
		TInt&		aSampleNumber,
		TInt&		aToleranceViolations
		)
	{
	_LIT(KTxtOn,	" On");
	_LIT(KTxtOff,	"Off");
	TBufC<3> transitionType;	
	transitionType = aTransitionedToOn ? KTxtOn : KTxtOff;
	
	aNow.UniversalTime();
	aDeltaTime = aNow.MicroSecondsFrom(aLastDeltaTime).Int64();
	aLastDeltaTime = aNow;
	
	if (aWarmUpIterations > 0)
		{
		aWarmUpIterations--;
		}
	else
		{
		if (aDeltaTime > aFlashChangeTime + aToleranceMargin ||
				aDeltaTime < aFlashChangeTime - aToleranceMargin)
			{
			INFO_PRINTF5(_L(" Iteration %d, Cursor %S after %d, errorDelta %d microseconds"),
					aSampleNumber, &transitionType, I64INT(aDeltaTime), I64INT(aDeltaTime - aFlashChangeTime));
			aToleranceViolations++;
			}
		}
	}
void CTCursorTest::CheckCursorDoesFlash(const TPoint& aPos, const TTextCursor& aTextCursor, TRgb /* aBackgroundColor */)
	{	
	
	/**
	 * Quality of Service based thresholding
	 * 
	 * The idea behind this test is to identify tolerances which would either
	 * cause the test to fail when the user would perceive the text cursor as
	 * not flashing uniformly, or would point to an unexpected delay outside
	 * the way the flashing (and scheduling of animations) is supposed to work.
	 * 
	 * Potentially the cursor can be late if we miss a V-SYNC from hardware.  In
	 * such cases we expect to see the cursor on the next frame.  Since the V-SYNC
	 * is typically 1/50 second, a tolerance of two frames, or 1/25 second is reasonable.
	 * 
	 * If the cursor is delayed longer than this, say a long time of 1 second, but this
	 * does not happen too often, then the user is likely to still be happy.  So we
	 * set the period of testing to 60 seconds, and set the violations limit to 2.
	 */
	const TInt kOneSecond = 1000000;
	const TInt kFlashPeriod = kOneSecond; 		// comprises one "ON" and one "OFF"
	const TInt kToleranceFactor = 25; 			// meaning 1/25 of a Flash Period
	const TInt kNumberTestFlashPeriods = 60;	// meaning 60 Flash Periods worth of testing
	const TInt kMaximumToleranceViolations = 2;	// number of times an occassional flash may be late or early
	
	const TInt kToleranceMargin = kFlashPeriod / kToleranceFactor;
	const TInt kNumberSamples = kNumberTestFlashPeriods * kToleranceFactor;
	const TInt kFlashChangeTime = kFlashPeriod / 2;
	
	// The first couple of changes to the cursor should be ignored because
	// when the test is started, the cursor may have been on for a while.
	// Then when the cursor goes off, it appears to have switched to the
	// off state too early.  We therefore ignore the first two changes
	// so we start cleanly.
	TInt warmUpIterations = 2;
	
	// Empirically we see that cursors mostly flash with good timeliness apart from
	// occasional events causing them to be either early or late.  In order to keep
	// a tight tolerance (1/50 second is around the screen refresh time) but still
	// allow for the occasional variance (seen to be 1/23 second) we use a counter
	// toleranceViolations < kMaximumToleranceViolations
	TInt toleranceViolations = 0;
	TBool cursorShownLastTime = EFalse;
	TBool cursorShown = EFalse;
	TTime lastDeltaTime;
	TTime now;
	TInt64 deltaTime = 0;
	lastDeltaTime.UniversalTime();
	now.UniversalTime();

	TRect textCursorRect(TRect(aPos, TSize(aTextCursor.iWidth, aTextCursor.iHeight)));

	for (TInt sampleNumber = 0; sampleNumber < kNumberSamples; sampleNumber++)
		{
		cursorShown = DoCheckRect(iWorkInProgress, iComparisonWindow, textCursorRect, CWsScreenDevice::EIncludeTextCursor);

		if (cursorShown && !cursorShownLastTime)
			{
			cursorShownLastTime = ETrue;
			UpdateCountersOnCursorTransition(
					cursorShownLastTime, now, deltaTime, lastDeltaTime, warmUpIterations, kFlashChangeTime,
					kToleranceMargin, sampleNumber, toleranceViolations);
			}
		else if (!cursorShown && cursorShownLastTime)
			{
			cursorShownLastTime = EFalse;
			UpdateCountersOnCursorTransition(
					cursorShownLastTime, now, deltaTime, lastDeltaTime, warmUpIterations, kFlashChangeTime,
					kToleranceMargin, sampleNumber, toleranceViolations);
			}

		if (toleranceViolations > kMaximumToleranceViolations)
			break;

		User::After(kToleranceMargin);
		}
	// Check was some flashing
	TEST_SOFTFAIL_WINSCW(warmUpIterations == 0);
	// Check cursor flashed on and off, regularly and on-time
	TEST_SOFTFAIL_WINSCW(toleranceViolations <= kMaximumToleranceViolations);
	}
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA

void CTCursorTest::MakeCursors(TTextCursor& aTextCursor, TTextCursor& aCustomCursor)
	{
	aCustomCursor.iType = KTextCursorInitialIdValue + iTest->iScreenNumber*KNumberOfCustoTextCursors; // custom text cursor
	aCustomCursor.iAscent=0;
	aCustomCursor.iHeight=kCursorHeight;
	aCustomCursor.iWidth=kCursorWidth;
	aCustomCursor.iFlags=TTextCursor::EFlagNoFlash; 
	aCustomCursor.iColor=TRgb::Color256(217);	
		
	// Create a standard cursor for the tests
	aTextCursor.iType = TTextCursor::ETypeRectangle; // Normal rectangular text cursor
	aTextCursor.iHeight=kCursorHeight;
	aTextCursor.iAscent=0;
	aTextCursor.iWidth=kCursorWidth;
	aTextCursor.iFlags=TTextCursor::EFlagNoFlash;
	}

void CTCursorTest::StartDoubleCursorTestL(TInt aTestNumber)
	{
	// general setup
	CBlankWindow* win1=new(ELeave) CBlankWindow(KRgbWhite);
	CleanupStack::PushL(win1);

	win1->SetUpL(kWin1TopLeft,kWinSize,TheClient->iGroup,*TheClient->iGc,EColor64K);
	
	win1->Redraw();
	
	// Create the second window
	CBlankWindow* win2=new(ELeave) CBlankWindow(KRgbWhite);
	CleanupStack::PushL(win2);

	win2->SetUpL(kWin2TopLeft,kWinSize,TheClient->iGroup,*TheClient->iGc,EColor64K);
	win2->Redraw();

	// Create normal and custom cursor for the tests
	TTextCursor textCursor;
	TTextCursor customCursor;
	MakeCursors(textCursor, customCursor);

	TheClient->Flush();
	CWindowGc* winGc = TheClient->iGc;	

	switch(aTestNumber)
		{
		case 1:
		CheckNoDoubleCursorTest1L(win1, win2, textCursor, customCursor, winGc);
		break;
		
		case 2:
		CheckNoDoubleCursorTest2L(win1, win2, textCursor, customCursor, winGc);
		break;
		
		case 3:
		CheckNoDoubleCursorTest3L(win1, win2, textCursor, customCursor, winGc);
		break;
		
		default:
		TEST(EFalse);
		break;
		}
	CleanupStack::PopAndDestroy(2); 	
	}

// DEF098704
void CTCursorTest::CheckNoDoubleCursorTest1L(CBlankWindow* aWin1, CBlankWindow* aWin2, TTextCursor& /*aTextCursor*/, TTextCursor& aCustomCursor, CWindowGc* aWinGc)
	{
	// Test that changing the focus of a custom text cursor does not leave that cursor drawn where it was (INC093898)

	TheClient->iGroup->GroupWin()->SetTextCursor(*aWin1->BaseWin(),TPoint(),aCustomCursor);
		
	// Bit blit the TEST_BITMAP_NAME image to the second window to use as a comparison
	// this is the same image that the custom cursor is using
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	TEST(KErrNone == bitmap->Load(TEST_BITMAP_NAME,0,ETrue)); 

	aWinGc->Activate(*aWin2->Win());
	TRect updateArea(TPoint(0,0), bitmap->SizeInPixels());
	aWin2->Win()->Invalidate(updateArea);
	aWin2->Win()->BeginRedraw(updateArea);
	aWinGc->BitBlt(TPoint(0,0),bitmap); 
	aWinGc->Deactivate();
	aWin2->Win()->EndRedraw();
	TheClient->Flush();
	doCheckNoDoubleCursor(aWin1,aWin2,kWin1TopLeft,kWin2TopLeft,aCustomCursor,bitmap->SizeInPixels(),CWsScreenDevice::EIncludeSprite);
	CleanupStack::PopAndDestroy(1); // bitmap
	}

// DEF098704
void CTCursorTest::CheckNoDoubleCursorTest2L(CBlankWindow* aWin1, CBlankWindow* aWin2, TTextCursor& /*aTextCursor*/, TTextCursor& aCustomCursor, CWindowGc* aWinGc)
	{
	//TEST 2: Checks that no artifacts are left behind when a text cursor is moved from under a transparent sprite
	
	// Construct the window win1 with a transparent sprite
	
	// Clear the top and bottom windows
	ResetWindows(aWinGc,aWin1,aWin2);	
	
	// Create a bitmap and a corresponding bitmap mask
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	TEST(KErrNone == bitmap->Load(TEST_BITMAP_NAME,0,ETrue)); 
	CBitmap* spriteBitmap=CBitmap::NewL(kWinSize,EColor256);
	CleanupStack::PushL(spriteBitmap);
	spriteBitmap->Gc().SetBrushColor(KRgbBlack);
	spriteBitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	spriteBitmap->Gc().SetPenStyle(CGraphicsContext::ESolidPen); 
	spriteBitmap->Gc().DrawRect(TRect(kWinSize));
	CBitmap* mask=CBitmap::NewL(kWinSize,EColor256);
	CleanupStack::PushL(mask);
	mask->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	mask->Gc().SetBrushColor(KRgbBlack);
	mask->Gc().DrawRect(TRect(kWinSize));	
	
	// Create a sprite
	RWsSprite sprite = RWsSprite(TheClient->iWs);
	CleanupClosePushL(sprite);
	TEST(KErrNone == sprite.Construct(*aWin1->BaseWin(),TPoint(),0));
	
	// Add the bitmap to the sprite
	TSpriteMember member;
	member.iInvertMask=EFalse;
	member.iDrawMode=CGraphicsContext::EDrawModePEN;
	member.iOffset=TPoint();
	member.iInterval=TTimeIntervalMicroSeconds32(0);
	member.iBitmap = &spriteBitmap->Bitmap();
	member.iMaskBitmap = &mask->Bitmap(); 
	TEST(KErrNone == sprite.AppendMember(member));
	
	// Activate the sprite in win1
	TEST(KErrNone == sprite.Activate());
	
	// Put a cursor in win1
	TheClient->iGroup->GroupWin()->SetTextCursor(*aWin1->BaseWin(),TPoint(),aCustomCursor);
	
	// Bit blit the matching bitmap to the bottom window
	TRect bitmapArea(TPoint(0,0), bitmap->SizeInPixels());
	aWin2->Win()->Invalidate(bitmapArea);
	aWin2->Win()->BeginRedraw(bitmapArea);
	aWinGc->Activate(*aWin2->Win());
	aWinGc->BitBlt(TPoint(),bitmap); 
	aWinGc->Deactivate();
	aWin2->Win()->EndRedraw();
		
	TheClient->Flush();
	doCheckNoDoubleCursor(aWin1,aWin2,kWin1TopLeft,kWin2TopLeft,aCustomCursor,bitmap->SizeInPixels(),CWsScreenDevice::EIncludeSprite);
	CleanupStack::PopAndDestroy(4); // sprite, mask, spriteBitmap, bitmap
	}

// DEF098704		
void CTCursorTest::CheckNoDoubleCursorTest3L(CBlankWindow* aWin1, CBlankWindow* aWin2, TTextCursor& aTextCursor,TTextCursor& aCustomCursor, CWindowGc* aWinGc)
	{
	//
	// TEST 3: Test a flashing text cursor does not leave artifacts when a redraw + change position happens during
	// the time the cursor flashing 'off'
	// 
	// This test moves a flashing cursor a number of times over a two second period to a seconds position.
	// it does it a number of times so some of the redraws will occur during the 'flash off' period
	// We then compare the 'after' position to what we expect
	// 
	// There are four possible outcomes when we look at the bitmap after the redraw + move
	// Position 1 is the original position, position 2 is the new position after the redraw
	// 
	// cursor artifact @ pos1 and flashed on cursor @ pos2
	// cursor artifact at pos1 and flashed off cursor @ pos2
	// no artifact at pos1 and flashed off cursor @ pos2
	// no artifact at pos1 and flashed on cursor @ pos2
	// 
	// any artifacts left over will cause the complete test to fail.
	//
	
	//
	// PART A: 
	aTextCursor.iFlags=0; 	// flashing

	ResetWindows(aWinGc,aWin1,aWin2);	
	TestForArtifacts(aWin1, aTextCursor);
	
	//
	// PART B - For a non-custom text cursor
	aCustomCursor.iFlags=0; // flashing

	ResetWindows(aWinGc,aWin1,aWin2);	
	TestForArtifacts(aWin1, aCustomCursor);	
	}
	
// moves the cursor between two positions while flashing and tests no artifacts are left at the position it moved from
// The moves take place in a loop so it is tested happening when the cursor is flashed on and also off
void CTCursorTest::TestForArtifacts(CBlankWindow* aWin1, TTextCursor& aCursor)
	{
	const TInt KIterations = 30;
	const TPoint kStartPos(0,0);
	const TPoint kMoveToPos(200,0);
	TRect r1(kWin1TopLeft,kCursorSize);
	TRect r2(kWin2TopLeft,kCursorSize);
	const TPoint kWin1TopLeft; 
	const TPoint kWin2TopLeft; 
	const TSize aCursorSize;

	TheClient->iGroup->GroupWin()->SetTextCursor(*aWin1->BaseWin(),kStartPos,aCursor);
	TheClient->Flush();

	TInt initialRepeatRate = 1000000; 
	const TInt KIncrement = 30000;
	TInt i=0;

	for(i=0; i<KIterations; i++)		
		{
		// move the cursor to its new position
		TheClient->iGroup->GroupWin()->SetTextCursor(*aWin1->BaseWin(),kMoveToPos,aCursor);
		TheClient->Flush();

		User::After(initialRepeatRate);
		
		// check no artifact was left in position 1 by comparing against (blank) win2
		if(!TheClient->iScreen->RectCompare( r1,r2, CWsScreenDevice::EIncludeTextCursor))
			{
			break; // detected an artifact remaining, test failed
			}

		// move the cursor back to its start position, this resets the flash timer which is why we increment initialRepeatRate 
		TheClient->iGroup->GroupWin()->SetTextCursor(*aWin1->BaseWin(),kStartPos,aCursor);
		TheClient->Flush();
		initialRepeatRate += KIncrement;
		}
		
		// if all went well i should equal KIterations, if it doesnt its because we detected 
		// an artifact and quit the test early
	TEST_SOFTFAIL_WINSCW(i==KIterations);
	}
	

// Tests the two windows match, moves the cursor off win 1 then tests they no longer match 		
void CTCursorTest::doCheckNoDoubleCursor(CBlankWindow* aWin1,
											CBlankWindow* aWin2,
											const TPoint& aWin1Tl,
											const TPoint& aWin2Tl,
											const TTextCursor& aCursor,
											const TSize& aCursorSize,
											CWsScreenDevice::TSpriteInCompare aFlags)
	{
	TRect r1(aWin1Tl,aCursorSize);
	TRect r2(aWin2Tl,aCursorSize);
	
	TInt compareTries = 0;
	const TInt compareLimit = 5;
	
	TBool correctComparison = EFalse;
	while (!correctComparison && compareTries < compareLimit)
		{
		compareTries++;
		User::After(500000);
		correctComparison = TheClient->iScreen->RectCompare(r1,r2, aFlags);
		}

	INFO_PRINTF3(_L("Result Before %d (attempts %d)"), correctComparison, compareTries);
	TEST_SOFTFAIL_WINSCW(correctComparison);

	
	// Change the focus off win1, by drawing the text cursor on the second window
	TheClient->iGroup->GroupWin()->SetTextCursor(*aWin2->BaseWin(),aWin2Tl,aCursor);
	TheClient->Flush();

	// Cause a redraw
	aWin1->CTWin::DrawNow();
	TheClient->WaitForRedrawsToFinish();

	// make sure any cursor has actually moved	
	User::After(1000000); // 1 sec
	
	TBool resultAfter;
	resultAfter = !TheClient->iScreen->RectCompare(r1,r2, aFlags);
	INFO_PRINTF2(_L("Result After %d"), resultAfter);
	TEST_SOFTFAIL_WINSCW(resultAfter);
	}	

// resets the windows to their initial state
void CTCursorTest::ResetWindows(CWindowGc* aWinGc,CBlankWindow* aWin1,CBlankWindow* aWin2)
	{
	TheClient->iGroup->GroupWin()->CancelTextCursor();
	aWinGc->Activate(*aWin1->Win());
	aWinGc->Reset();
	aWinGc->Clear();
	aWinGc->Deactivate();

	aWin1->Invalidate();
	aWin1->Redraw();
	aWin2->Invalidate();
	aWin2->Redraw();
	TheClient->Flush();
	}
	
void CTCursorTest::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	TBool deleteMove=EFalse;
	((CTCursorTestStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0241

@SYMDEF				DEF081259

@SYMTestCaseDesc    Test the text cursor functions properly as a window is moved

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Move a window about the screen and meanwhile check the text
					cursor functions correctly

@SYMTestExpectedResults The text cursor functions correctly as the window is moved
*/
	case 1:
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0241"));
		while (!deleteMove)
			{
			if (iCursorType==TTextCursor::ETypeFirst)
				iTest->LogSubTest(_L("Cursor 1"));
			if (MoveWindow())
				{
				if (IncrementCursorType())
					{
					ResetMoveWindowsL();
					}
				else
					{
					DeleteMoveWindows();
					deleteMove =true;
					}
				}
			}
		break;

/**
@SYMTestCaseID		GRAPHICS-WSERV-0242

@SYMDEF				DEF081259

@SYMTestCaseDesc    Test the text cursor functions properly as the window is scrolled

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Scroll the window and meanwhile check the text cursor functions correctly

@SYMTestExpectedResults The text cursor functions correctly as the window is scrolled
*/
		case 2:
			((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0242"));
			iTest->LogSubTest(_L("Cursor 2"));
			iCursorType=TTextCursor::ETypeRectangle;
			ScrollTest();
			iCursorType=TTextCursor::ETypeFirst;
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0244

@SYMDEF				DEF081259

@SYMTestCaseDesc    Test the text cursor functions properly as a blank window is moved

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Move a blank window about the screen and meanwhile check the text
					cursor functions correctly

@SYMTestExpectedResults The text cursor functions correctly as the blank window is moved
*/
		case 3:
			((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0244"));
			iTest->LogSubTest(_L("Move window1"));
			MoveWindowTest1L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0245

@SYMDEF				DEF081259

@SYMTestCaseDesc    Test that a non flashing text cursor functions properly as a 
					blank window is moved

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Move a blank window about the screen and meanwhile check that a non
					flashing text cursor functions correctly

@SYMTestExpectedResults The text cursor functions correctly as the blank window is moved
*/
		case 4:
			((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0245"));
			while (IncrementCursorType())
				{
				iTest->LogSubTest(_L("Move window2"));
				MoveWindowTest2L();
				}
			break;		
/**
@SYMTestCaseID		GRAPHICS-WSERV-0247

@SYMDEF				DEF081259

@SYMTestCaseDesc    Tests a Custom Text Cursor Sprite visibility after wserv hearbeat suppression
					REQUIREMENT:	INC040489.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create a window group with an associated window for it, then activate it.
					Create another window group with an associated window along with a custom cursor, 
					then active it. Make a call to suppress the wserv hearbeat, which will stop 
					the custom cursor from flashing with flash ON state. Bring the first window 
					group to the foreground and cancel the custom test cursor in the second window group.
					After that, put the first window group to the background and set the custom test 
					cursor in the second window group. Simulate a raw key event to start 
					the wserv heartbeat, which will make the custom cursor flashing.

@SYMTestExpectedResults The Custom text cursor in the second window group should be visible and flashing, 
						when it comes to the foreground after the first window group sent to the background.
*/
		case 5:
			((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0247"));
			iTest->LogSubTest(_L("Custom Text Cursor Sprite visibility"));
			INC040489L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-2095-0015

@SYMTestCaseDesc    Text Cursor allows Update before Rendering the owning window

@SYMTestPriority    Normal

@SYMTestStatus      Implemented

@SYMTestActions     Create a window group and two windows, one with a Text Cursor.
					Stimulate a state change in the Text Cursor by receiving
					window focus, but before the Window has ever been Rendered.
					This shakes out logic in RWsTextCursor which depends on
					side effects arising from having executed CWsWindow::Render()
					
@SYMTestExpectedResults 
					There should be no panic or assertion.
*/
		case 6:
			((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-2095-0015"));
			iTest->LogSubTest(_L("Update before Render"));
			CursorUpdatedBeforeWindowRenderedL();
			break;

/**
@SYMTestCaseID		GRAPHICS-WSERV-0248

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Tests a bad use of text cursor functionality from a client.
					REQ 1079, CR RDEF-5F7Q24 (10/04/2003).

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     This test case checks whether the window server is able to detect a bad
					use of the text cursor functionality (including the custom text cursor)
					by a client.
					This test case launches several threads and each of them will try
					to use the text cursor functionality in a non-proper way.

@SYMTestExpectedResults Each new thread has panic code associated to it. This is the expected panic when
						the thread dies.
						The thread once launched is expected to panic and the returning panic code should
						match the expected one.
*/
		case 7:
			((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0248"));
			iTest->LogSubTest(_L("Panic"));
			TestPanicsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0400

@SYMDEF				PDEF099013

@SYMTestCaseDesc    Cursor moves slowly in text editors. 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     This test case is a VISUAL TEST only on whether a scrolling text cursor is drawn to the
					screen correctly. 2 types of cursor are checked both clipped and non clipped cursors.
					In each case the test simulates a text cursor moving across the screen as if a user were
					holding down a key to scroll the cursor through a section of text.

@SYMTestExpectedResults The text cursor in both cases should scroll smoothly to the centre of the screen.
						In versions prior to this fix the cursor was not correctly drawn and appeared to move 	
						slowly.
*/
		case 8:
			((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0400"));
			iTest->LogSubTest(_L("Text Cursor Update Tests"));
			INC097774();
			break;	
		
/**
@SYMTestCaseID		GRAPHICS-WSERV-0401

@SYMDEF  			DEF098704

@SYMTestCaseDesc    Test code for implemented fix to remove double cursors 
					
@SYMTestPriority    Normal

@SYMTestStatus      Implemented

@SYMTestActions     This test case tests for artifacts left over when normal/custon flashing/non-flashing cursors are drawn.
					Test that changing the focus of a custom text cursor does not leave that cursor drawn where it was (INC093898)

@SYMTestExpectedResults 
*/			
	case 9:
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0401"));
		iTest->LogSubTest(_L("Double cursors test 1"));
		StartDoubleCursorTestL(1);
		break;

	/**
	@SYMTestCaseID		GRAPHICS-WSERV-0402

	@SYMDEF				DEF098704

	@SYMTestCaseDesc    Test code for implemented fix to remove double cursors 

	@SYMTestPriority    Normal

	@SYMTestStatus      Implemented

	@SYMTestActions     Checks that no artifacts are left behind when a text cursor is moved from under a transparent sprite

	@SYMTestExpectedResults 
	*/
	case 10:
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0402"));
		iTest->LogSubTest(_L("Double cursors test 2"));
		StartDoubleCursorTestL(2);
		break;

	/**
	@SYMTestCaseID		GRAPHICS-WSERV-0403

	@SYMDEF				DEF098704

	@SYMTestCaseDesc    Test code for implemented fix to remove double cursors 
						
	@SYMTestPriority    Normal

	@SYMTestStatus      Implemented

	@SYMTestActions     This test moves a flashing cursor a number of times over a two second period to a seconds position.
						it does it a number of times so some of the redraws will occur during the 'flash off' period
						We then compare the 'after' position to what we expect

	@SYMTestExpectedResults 
	*/
	case 11:
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0403"));
		iTest->LogSubTest(_L("Double cursors test 3"));
		StartDoubleCursorTestL(3);
		break;

	/**
	@SYMTestCaseID		GRAPHICS-WSERV-0483

	@SYMDEF				INC117232

	@SYMTestCaseDesc    Test code: Refreshing cursors becoming offscreen due to screen size change should not panic 

	@SYMTestPriority    Normal

	@SYMTestStatus      Implemented

	@SYMTestActions     Create a cursor on bottom right of screen, change to a screen mode that excludes that coordinate

	@SYMTestExpectedResults 
						The server should not panic.
	*/
	case 12:
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0483"));
		iTest->LogSubTest(_L("Screen resize invalidate cursor off-screen"));
		INC117232();
		break;

	/**
	@SYMTestCaseID		GRAPHICS-WSERV-2095-0016

	@SYMTestCaseDesc    Text Cursor flag TTextCursor::EFlagNoFlash honored 

	@SYMTestPriority    Normal

	@SYMTestStatus      Implemented

	@SYMTestActions     Create a text cursor with the TTextCursor::EFlagNoFlash setting.  Observe the screen over
						a short period of time to verify that it remains continuously in the Flash ON state.

	@SYMTestExpectedResults 
						The text cursor should be always shown.
	*/
	case 13:
	#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-2095-0016"));
		iTest->LogSubTest(_L("Text cursor EFlagNoFlash test"));
		TextCursorNoFlashTestL();
	#endif
		break;

	/**
	@SYMTestCaseID		GRAPHICS-WSERV-2095-0009

	@SYMTestCaseDesc    Text Cursor flashes when flag value is 0

	@SYMTestPriority    Normal

	@SYMTestStatus      Implemented

	@SYMTestActions     Create a text cursor with the 0 flag setting.  Observe the screen over
						a short period of time to verify that the cursor flashes ON and OFF at
						the correct period of one second.

	@SYMTestExpectedResults 
						The text cursor should flash.
	*/
	case 14:
	#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-2095-0009"));
		iTest->LogSubTest(_L("Text cursor will Flash test"));
		TextCursorFlashTestL();
	#endif
		break;

	/**
	@SYMTestCaseID		GRAPHICS-WSERV-2095-0017

	@SYMTestCaseDesc    Text Cursor handles different valid Cursor Setttings

	@SYMTestPriority    Normal

	@SYMTestStatus      Implemented

	@SYMTestActions     Create a text cursor and then issue a SetTextCursor.  Then repeatedly
						call SetTextCursor, varying the arguments relating to the Text Cursor
						as well as keeping the arguments the same on one occassion.

	@SYMTestExpectedResults 
						The system should not panic as the arguments supplied are never invalid.
	*/
	case 15:
		((CTCursorTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-2095-0017"));
		iTest->LogSubTest(_L("SetTextCursor test"));
		TextCursorSetLCoverageTests();
		break;

		default:
			((CTCursorTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTCursorTestStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTCursorTestStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(CursorTest)
