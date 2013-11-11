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
// The fix solves the problem of bitmap drawing command being added to the window server
// command buffer without the bitmap handle(s) being added to the command buffer bitmap array.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tw32cmdbuf.h"

const TInt KMaxTestIterations = 300;
const TInt KMinTestIterations = 100;

CTW32CmdBuf::CTW32CmdBuf(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	}

CTW32CmdBuf::~CTW32CmdBuf()
	{
	}

void CTW32CmdBuf::ConstructL()
	{
	}

void CTW32CmdBuf::RunTestCaseL(TInt aCurTestCase)
	{
	((CTW32CmdBufStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
		case 1:
			((CTW32CmdBufStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0452"));
			INFO_PRINTF1(_L("Test all the drawing commands involving a CFbsBitmap\n"));
			DoCmdBufTestsL();
			break;
		default:
			((CTW32CmdBufStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTW32CmdBufStep*)iStep)->CloseTMSGraphicsStep();
			INFO_PRINTF1(_L("Test complete\n"));
			TestComplete();
		}
	((CTW32CmdBufStep*)iStep)->RecordTestResultL();
	}

/**
* TTestFunctionPtr pointer-to-function type definition
*/
typedef	void (*TTestFunctionPtr)(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* aMask);


/**
* Calls CWindowGc::DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aDevice)
*/
void CallDrawBitmap1(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* /*aMask*/)
	{
	aGc->DrawBitmap(TPoint(0, 0), aBitmap);
	}

/**
* Calls CWindowGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice)
*/
void CallDrawBitmap2(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* /*aMask*/)
	{
	aGc->DrawBitmap(TRect(0, 0, 100, 100), aBitmap);
	}
	
/**
* Calls CWindowGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice, const TRect &aSourceRect)
*/
void CallDrawBitmap3(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* /*aMask*/)
	{
	aGc->DrawBitmap(TRect(0, 0, 100, 100), aBitmap, TRect(0, 0, 100, 100));
	}
	
/**
* Calls CWindowGc::DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask)
*/
void CallDrawBitmapMasked(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* aMask)
	{
	aGc->DrawBitmapMasked(TRect(0, 0, 100, 100), aBitmap, TRect(0, 0, 100, 100), aMask, EFalse);
	}

/**
* Calls CWindowGc::UseBrushPattern(const CFbsBitmap *aDevice)
*/	
void CallUseBrushPattern(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* /*aMask*/)
	{
	aGc->UseBrushPattern(aBitmap);
	}

/**
* Calls CWindowGc::BitBlt(const TPoint &aPos, const CFbsBitmap *aBitmap)
*/
void CallBitBlt1(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* /*aMask*/)
	{
	aGc->BitBlt(TPoint(0, 0), aBitmap);
	}

/**
* Calls CWindowGc::BitBlt(const TPoint &aDestination, const CFbsBitmap *aBitmap, const TRect &aSource)
*/
void CallBitBlt2(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* /*aMask*/)
	{
	aGc->BitBlt(TPoint(0, 0), aBitmap, TRect(0, 0, 100, 100));
	}

/**
* Calls CWindowGc::BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask)
*/
void CallBitBltMasked(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* aMask)
	{
	aGc->BitBltMasked(TPoint(0, 0), aBitmap, TRect(0, 0, 100, 100), aMask, EFalse);
	}

/**
* Calls CWindowGc::AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt)
*/	
void CallAlphaBlendBitmaps(CWindowGc* aGc, CFbsBitmap* aBitmap, CFbsBitmap* aMask)
	{
	aGc->AlphaBlendBitmaps(TPoint(0, 0), aBitmap, TRect(0, 0, 100, 100), aMask, TPoint(0, 0));
	}

/**
* Drawing command function array.
*/	
const TTestFunctionPtr KTestFunctions[] =
		{
		CallDrawBitmap1,
		CallDrawBitmap2,
		CallDrawBitmap3,
		CallDrawBitmapMasked,
		CallUseBrushPattern,
		CallBitBlt1,
		CallBitBlt2,
		CallBitBltMasked,
		CallAlphaBlendBitmaps,
		};

/**
 * @SYMTestCaseID			GRAPHICS-WSERV-0452
 * @SYMTestCaseDesc			Tests drawing commands with bitmap handles.
 * @SYMDEF					INC111655
 * @SYMFssID				CWindowGc::DrawBitmap() \n 
 * 							CWindowGc::DrawBitmapMasked()\n 
 * 							CWindowGc::UseBrushPattern()\n 
 * 							CWindowGc::BitBlt()\n 
 * 							CWindowGc::AlphaBlendBitmaps()
 * @SYMTestPriority			Critical
 * @SYMTestType				Unit Test
 * @SYMTestPurpose			To ensure drawing commands with bitmap handles are added to the
 							command buffer successfully when the buffer is full.
 * @SYMTestActions			Fill the command buffer with CWindowGc::Clear() commands in a loop until
 							the buffer is full, create bitmap(s), then call the draw command and then 
 							delete the bitmap handle(s). All tests are done in a second thread.
 * @SYMTestExpectedResults	The function should not panic. Without the fix the functions will panic
 							with WSERV 7. 
 * @SYMTestStatus			Implemented 
 */	
void CTW32CmdBuf::DoCmdBufTestsL()
	{
	CreateSecondThreadAndDoTestL(ECallDrawBitmap1);
	CreateSecondThreadAndDoTestL(ECallDrawBitmap2);
	CreateSecondThreadAndDoTestL(ECallDrawBitmap3);
	CreateSecondThreadAndDoTestL(ECallDrawBitmapMasked);
	CreateSecondThreadAndDoTestL(ECallUseBrushPattern);	
	CreateSecondThreadAndDoTestL(ECallBitBlt1);
	CreateSecondThreadAndDoTestL(ECallBitBlt2);
	CreateSecondThreadAndDoTestL(ECallBitBltMasked);
	CreateSecondThreadAndDoTestL(ECallAlphaBlendBitmaps);
	}

/**
* Creates a second thread to run the test.
* 
* @param aFunctionIndex The drawing function command to be executed. All commands are defined in TestFunctionIndex.
*/
void CTW32CmdBuf::CreateSecondThreadAndDoTestL(TTestFunctionIndex aFunctionIndex)
	{	
	RThread thread;
	TBuf<30> threadName(KTW32CmdBufSecondThread);
	static TInt threadSerialNumber = 0;
	threadName.AppendNum(++threadSerialNumber);
	User::LeaveIfError(thread.Create(threadName, TestCmdBufFunction, KDefaultStackSize, KMinHeapSize, 0x4000, &aFunctionIndex));
	TRequestStatus status;
	thread.Logon(status);
	thread.Resume();
	User::WaitForRequest(status);
	TEST(thread.ExitType()==EExitKill);
	TEST(thread.ExitReason() == KErrNone);
	thread.Close();	
	}

/**
* Runs the test in a second thread.
*
* @param aFunctionIndex The drawing function command to be executed. All commands are defined in TestFunctionIndex.
*/
TInt CTW32CmdBuf::DoTestCmdBufFunctionL(TTestFunctionIndex aFunctionIndex)
	{
	RWsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	CWsScreenDevice *device = new(ELeave) CWsScreenDevice(session);
	CleanupStack::PushL(device);
	User::LeaveIfError(device->Construct(CTestBase::iScreenNo));
	CWindowGc* gc;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	RWindowGroup group(session);
	User::LeaveIfError(group.Construct(1, EFalse));
	CleanupClosePushL(group);
	RWindow window(session);
	User::LeaveIfError(window.Construct(group, 2));
	CleanupClosePushL(window);
	window.SetExtent(TPoint(0,0), TSize(200, 200));
	User::LeaveIfError(window.SetRequiredDisplayMode(EColor64K));
	window.Activate();	
	gc->Activate(window);
	session.SetAutoFlush(EFalse);
	window.Invalidate();
	window.BeginRedraw();
	for(TInt i=KMinTestIterations; i<KMaxTestIterations; ++i)
		{
		for(TInt j=0; j<i; ++j)
			{
			gc->Clear();
			}

		CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmap);
		User::LeaveIfError(bitmap->Create(TSize(100, 100), EColor64K));
		CFbsBitmap* mask = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(mask);
		User::LeaveIfError(mask->Create(TSize(100, 100), EColor64K));
		KTestFunctions[aFunctionIndex](gc, bitmap, mask);
		CleanupStack::PopAndDestroy(2);
		session.Flush();	
		}	
	window.EndRedraw();	
	gc->Deactivate();
	CleanupStack::PopAndDestroy(5);
	return KErrNone;
	}

/**
* Second thread entry function.
*
* @param aInfo The parameter(s) passed to the second thread in this case the function index.
*/
TInt CTW32CmdBuf::TestCmdBufFunction(TAny* aInfo)
	{	
	if(!aInfo)
		{
		return KErrArgument;
		}
	TTestFunctionIndex functionIndex = *(TTestFunctionIndex*)aInfo;
	CTrapCleanup *trap = CTrapCleanup::New();
	__ASSERT_ALWAYS(trap, User::Invariant());
	
	TRAPD(err, DoTestCmdBufFunctionL(functionIndex));	
	
	delete trap;
	return err;
	}

__CONSTRUCT_STEP__(W32CmdBuf)
