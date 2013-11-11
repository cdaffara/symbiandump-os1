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

#include "tdirectgdicontext.h"
#include <s32mem.h>

#include <graphics/directgdicontext.h>


_LIT16(KAbcd,"Abcd efgh ijkl mnop");

CTDirectGdiContext::CTDirectGdiContext()
	{
	SetTestStepName(KTDirectGdiContextStep);
	}

CTDirectGdiContext::~CTDirectGdiContext()
	{
	if(iFont)
		{
		ReleaseFont(iFont);		
		}	
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-CONTEXT-0001

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Use case - Single context and single target.

@SYMTestActions		
	Single context is created and activated on a single target.
	Then some draw operations are done.

@SYMTestExpectedResults
	It should be able to draw using the correct context and target.
	
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContext::OneContextOneTarget()
	{		
	_LIT(KTestName, "Context-OneContextOneTarget"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	ResetGc();		
	
	iGc->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(KTestName)));
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXT-0002

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Use case - Draw to two separate targets using a single context. 	

@SYMTestActions		
	Test that a single context can be activated on two different targets.
	- A single context is created and activated on a target then a green circle is drawn. 
	- The same context then activated on a second target and a red circle is drawn.  

@SYMTestExpectedResults
	It should be able to draw using the correct context and target and context should 
	be able to switch between the targets. The first target should have a green circle 
	only drawn on it, the second target should have a red circle only drawn on it.

@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContext::OneContextTwoTargets()
	{	
	TBuf<KTestCaseLength> testCaseNameA;
	TBuf<KTestCaseLength> testCaseNameB;
	_LIT(KTestCase, "Context-OneContextTwoTargets");
	testCaseNameA.Append(KTestCase);
	testCaseNameB.Append(KTestCase);
	if(iCaseNum == EOneContextTwoTargets_SamePixelType)
		{
		testCaseNameA.Append(KSamePixelType);
		testCaseNameB.Append(KSamePixelType);
		}
	else
		{
		testCaseNameA.Append(KDifferentPixelType);
		testCaseNameB.Append(KDifferentPixelType);
		}
	testCaseNameA.Append(KTarget1);
	testCaseNameB.Append(KTarget2);
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestCase);
		}
	ResetGc();
	iGc->SetPenColor(TRgb(0, 255, 0));
	TEST(iGc->PenColor() == TRgb(0, 255, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameA)));
	
	iGdiTarget2->Activate(iGc);
	
	ResetGc();
	iGc->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameB),iGdiTarget2));
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXT-0003

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Use case - draw to a single target using two different contexts.

@SYMTestActions	
	Test context's creation and activation when drawing using two different contexts to a single target.
	- Activate the first context on the target and draw a red circle.
	- Write out the contents of the target.
	- Activate the second context on the target, clear it and draw a smaller black circle.
	- Write out the contents of the target.

@SYMTestExpectedResults
	It should be able to draw using the correct context and target. The first image written out
	should contain a red circle, the second image written out should contain a smaller black circle.
	
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContext::TwoContextsOneTarget()
	{	
	TBuf<KTestCaseLength> testCaseNameA;
	TBuf<KTestCaseLength> testCaseNameB;
	_LIT(KTestCase, "Context-TwoContextsOneTarget");
	testCaseNameA.Append(KTestCase);
	testCaseNameB.Append(KTestCase);
	testCaseNameA.Append(KContext1);
	testCaseNameB.Append(KContext2);
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestCase);
		}
	iGdiTarget->Activate(iGc);
	ResetGc();
	iGc->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameA)));	
		
	iGdiTarget->Activate(iGc2);
	iGc2->Reset();
	iGc2->Clear();
	iGc2->SetPenColor(TRgb(0, 0, 0));
	TEST(iGc2->PenColor() == TRgb(0, 0, 0));
	iGc2->DrawEllipse(TRect(0, 0, 30, 30));
	TESTNOERROR(iGc2->GetError());	
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameB)));
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXT-0004

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Use case - Two contexts and two targets, but there is no sharing

@SYMTestActions		
	Two contexts are created and activated on two different targets. There is no sharing between the contexts and targets, they
	are completely isolated.
	Then some draw operations are done. 

@SYMTestExpectedResults
	It should be able to draw using the correct context and target.
	
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContext::TwoContextsTwoTargets_WithoutSharingL()
	{	
	TBuf<KTestCaseLength> testCaseNameA;
	TBuf<KTestCaseLength> testCaseNameB;
	_LIT(KTestCase, "Context-TwoContextsTwoTargets_NotSharing");
	testCaseNameA.Append(KTestCase);
	testCaseNameB.Append(KTestCase);
	if(iCaseNum == ETwoContextsTwoTargets_WithoutSharing_SamePixelType)
		{
		testCaseNameA.Append(KSamePixelType);
		testCaseNameB.Append(KSamePixelType);
		}
	else
		{
		testCaseNameA.Append(KDifferentPixelType);
		testCaseNameB.Append(KDifferentPixelType);
		}
	testCaseNameA.Append(KTarget1);
	testCaseNameB.Append(KTarget2);
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestCase);
		}
	iGdiTarget->Activate(iGc);
	ResetGc();
	iGc->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());

	iGdiTarget2->Activate(iGc2);
	iGc2->Reset();
	iGc2->Clear();
	iGc2->SetPenColor(TRgb(0, 255, 0));
	TEST(iGc2->PenColor() == TRgb(0, 255, 0));
	iGc2->DrawEllipse(TRect(0, 0, 30, 30));
	TESTNOERROR(iGc2->GetError());	
	
	// Draw to both again twice to make sure that context switching works.
	iGc->DrawEllipse(TRect(0,0,60,60));
	TESTNOERROR(iGc->GetError());
	iGc2->DrawEllipse(TRect(0, 0, 30, 30));
	TESTNOERROR(iGc2->GetError());
	
	iGc->DrawEllipse(TRect(0,0,60,60));
	TESTNOERROR(iGc->GetError());
	iGc2->DrawEllipse(TRect(0, 0, 30, 30));
	TESTNOERROR(iGc2->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameA)));
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameB),iGdiTarget2));
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXT-0005

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Use case - Draw using two contexts and two targets, swapping the targets over half way through the test.

@SYMTestActions
	Test that a context can draw to a target that has been previously drawn to using a different context.		
	Two contexts are created and activated on two different targets. Then some draw operations are done, 
	a red circle is drawn on the first target and a black circle is drawn on the second target.
	Then these contexts are activated on the opposite targets. There is sharing between the contexts 
	and targets (contexts are activated on each other's targets).
	Then again some draw operations are done, the targets are cleared and a red circle is drawn on each target.
 

@SYMTestExpectedResults
	It should be able to draw using the correct context and target and context should be able to switch between 
	the targets. 
	The target 1 drawn to with context 1 should show a red circle, 
	target 2 drawn to with context2 should show a black circle, 
	target 1 drawn to with context 2 should show a red circle,
	target 2 drawn to with context 1 should show a red circle. 
	
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContext::TwoContextsTwoTargets_WithSharingL()
	{	
	TBuf<KTestCaseLength> testCaseNameA;
	TBuf<KTestCaseLength> testCaseNameB;
	TBuf<KTestCaseLength> testCaseNameAB;
	TBuf<KTestCaseLength> testCaseNameBA;

	_LIT(KTestCase, "Context-TwoContextsTwoTargets_Sharing");
	testCaseNameA.Append(KTestCase);
	testCaseNameB.Append(KTestCase);
	testCaseNameAB.Append(KTestCase);
	testCaseNameBA.Append(KTestCase);

	if(iCaseNum == ETwoContextsTwoTargets_WithSharing_SamePixelType)
		{
		testCaseNameA.Append(KSamePixelType);
		testCaseNameB.Append(KSamePixelType);
		testCaseNameAB.Append(KSamePixelType);
		testCaseNameBA.Append(KSamePixelType);
		}
	else
		{
		testCaseNameA.Append(KDifferentPixelType);
		testCaseNameB.Append(KDifferentPixelType);
		testCaseNameAB.Append(KDifferentPixelType);
		testCaseNameBA.Append(KDifferentPixelType);
		}
	testCaseNameA.Append(KContext1);
	testCaseNameA.Append(KTarget1);
	testCaseNameB.Append(KContext2);
	testCaseNameB.Append(KTarget2);
	testCaseNameAB.Append(KContext1);
	testCaseNameAB.Append(KTarget2);
	testCaseNameBA.Append(KContext2);
	testCaseNameBA.Append(KTarget1);
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestCase);
		}
	iGdiTarget->Activate(iGc);
	ResetGc();
	iGc->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameA)));	
	
	iGdiTarget2->Activate(iGc2);
	iGc2->Reset();
	iGc2->Clear();
	iGc2->SetPenColor(TRgb(0, 0, 0));
	TEST(iGc2->PenColor() == TRgb(0, 0, 0));
	iGc2->DrawEllipse(TRect(0, 0, 30, 30));
	TESTNOERROR(iGc2->GetError());	
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameB),iGdiTarget2));
	
	iGdiTarget->Activate(iGc2);
	iGdiTarget2->Activate(iGc);

	ResetGc();
	iGc->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameAB),iGdiTarget2));
	
	iGc2->Reset();
	iGc2->Clear();
	iGc2->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc2->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc2->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameBA)));	
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXT-0006

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Use case - Single context and single target, but calling activate twice.

@SYMTestActions		
	Single context is created and activated twice on the same target, just to prove that there is no affect of calling
	Activate() twice on the same target by the same context, in succession.

@SYMTestExpectedResults
	It should be able to draw using the correct context and target.
	
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContext::OneContextOneTarget_TwiceActivateL()
	{	
	_LIT(KTestName, "Context-OneContextOneTargetTwiceActivate");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	_LIT(KTestNameA, "Context-OneContextOneTarget_TwiceActivateA"); 
	_LIT(KTestNameB, "Context-OneContextOneTarget_TwiceActivateB"); 
	ResetGc();
	iGc->SetPenColor(TRgb(255, 0, 0));
	TEST(iGc->PenColor() == TRgb(255, 0, 0));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TEST(KErrNone == WriteTargetOutput(iTestParams, TPtrC(KTestNameA)));	
	
	iGdiTarget->Activate(iGc);
	
	ResetGc();
	iGc->SetPenColor(TRgb(0, 0, 255));
	TEST(iGc->PenColor() == TRgb(0, 0, 255));
	iGc->DrawEllipse(TRect(0, 0, 60, 60));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(KTestNameB)));
	}

LOCAL_C void ContextReset(TAny* aPtr)
	{
	CTContextBase* gc = reinterpret_cast <CTContextBase*> (aPtr);
	gc->Reset();
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-CONTEXT-0007

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195 
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestStatus
	Complete

@SYMTestCaseDesc
	Externalize and internalize a context.

@SYMTestActions
	Test the use case where we:
	Create a new GC for the purpose of the test.
	Set Pen and Brush colour non-default colours.
	Set the Pen and Brush styles to non-default styles.
	Set the clipping regions.
	Set a font (DirectGdi only, BitGdi causes the font to be cached and UHEAP failures occur).
	Set the origin and the internal drawing position to non-defaults.
	Externalise.
	Reset the context, and set the state to other values.
	Internalise.
	Draw a line from the current internal drawing position.
	Draw a rectangle using current pen and brush styles/colours/size.
	Destroy the temporary GC and reinstate the original.

@SYMTestStatus 
	Implemented
 */
void CTDirectGdiContext::ExternalizeInternalizeTestL()
	{
	__UHEAP_MARK;
	_LIT(KTestName, "Context-ExternalizeInternalize"); 	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	const TInt width = iGdiTarget->SizeInPixels().iWidth;
	const TInt height = iGdiTarget->SizeInPixels().iHeight;

	// For this test, create a temporary GC which we can delete later.
	// This is to avoid UHEAP mismatches if we used the original iGc.
	// Set this to be iGc for this test.	
	CTContextBase *oldGc = iGc;
	iGc = NULL;
	CleanupStack::PushL(oldGc);
	TESTNOERRORL(iGdiTarget->CreateContext(iGc));
	ResetGc();

	// Create some clipping regions. These should not be externalised/internalised.
	RRegion clippingRegion(2);
	clippingRegion.AddRect(TRect(width*3/10,0,width*7/10,height));
	clippingRegion.AddRect(TRect(0,height*3/10,width,height*7/10));
	CleanupClosePushL(clippingRegion);
	iGc->SetClippingRegion(clippingRegion);
	CleanupStack::PushL(TCleanupItem(ContextReset, iGc));

	// Create a bitmap to use as a brush pattern
	CFbsBitmap* pattern = CreateCheckedBoardBitmapL(iTestParams.iTargetPixelFormat, TSize(100,100));
	CleanupStack::PushL(pattern);

	// Set non-default state.
	iGc->SetOrigin(TPoint(10,10));
	iGc->SetPenSize(TSize(3,3));
	iGc->SetPenColor(TRgb(255,0,255));
	iGc->SetPenStyle(DirectGdi::EDashedPen);
	iGc->SetBrushColor(TRgb(0,255,0));
	iGc->SetBrushPattern(*pattern);
	iGc->SetBrushStyle(DirectGdi::EPatternedBrush);
	iGc->SetBrushOrigin(TPoint(3, 3));
	iGc->MoveTo(TPoint(10, height/2));
	iGc->SetDrawMode(DirectGdi::EDrawModePEN);

	if (iUseDirectGdi)
		{
		// Internalizing/externalizing a font in BitGdi will cause the font to be cached and 
		// memory UHEAP mismatch occurs, so only do it for DirectGdi. We wont draw any text in the image.
		TESTL(iFont != NULL);
		iGc->SetFont(iFont);
		}

	// create buffer for save/restore
	CBufFlat* buf = NULL;
	TRAPD(err, buf=CBufFlat::NewL(512));
	if(KErrNone != err)
		{
		iGc->ResetClippingRegion();
		User::LeaveIfError(err);
		}
	CleanupStack::PushL(buf);

	// save and then restore
	RBufWriteStream ws(*buf,0);
	iGc->ExternalizeL(ws);
	ws.Close();

	// Make some modifications to iGc.
	// The clipping regions should be persistant even after the Internalize().
	iGc->Reset();
	iGc->ResetClippingRegion();
	iGc->ResetFont();
	iGc->SetPenColor(TRgb(0,0,0));
	RRegion tempClippingRegion(1);
	tempClippingRegion.AddRect(TRect(width/10, height/10, width*9/10, height*9/10));
	iGc->SetClippingRegion(tempClippingRegion);	
	CleanupClosePushL(tempClippingRegion);

	// Internalise the state.
	RBufReadStream rs(*buf,0);
	iGc->InternalizeL(rs);
	rs.Close();

	// Draw using the newly internalised context.
	// We don't draw any text as we don't internalize/externalize text for BitGdi.
	iGc->Clear();
	iGc->DrawRect(TRect(width/4, height/4, width*3/4, height*3/4));
	iGc->DrawLineTo(TPoint(width, height/2));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(KTestName)));

	CleanupStack::PopAndDestroy(5);
	CleanupStack::Pop(1, oldGc);

	// Destroy the temporary Gc and reinstate the original one.
	iGdiTarget->Activate(oldGc);
	delete iGc;
	iGc = oldGc;
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-CONTEXT-0008

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Use case - two contexts and one target. 
  
@SYMTestActions		
	Two contexts are created and activated on the same target.
	Then some draw operations are done. 
	CopySettings() is called to copy settings from first context to the second one.
	Then again some draw operations are done to verify that settings have been copied properly.

@SYMTestExpectedResults
	It should be able to draw using the correct settings of context and target.
	
@SYMTestStatus 		
	Implemented
*/
void CTDirectGdiContext::TestCopySettings()
	{	
	TBuf<KTestCaseLength> testCaseNameA;
	TBuf<KTestCaseLength> testCaseNameB;
	_LIT(KTestCase, "Context-CopySettings");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestCase);
		}
	testCaseNameA.Append(KTestCase);
	testCaseNameB.Append(KTestCase);
	testCaseNameA.Append(KContext1);
	testCaseNameB.Append(KContext2);	

	// Draw to the first graphics context
	iGdiTarget->Activate(iGc);
	ResetGc();		
	iGc->SetPenColor(KRgbRed);
	iGc->SetPenSize(TSize(3,3));
	iGc->SetBrushColor(KRgbYellow);
	iGc->SetBrushStyle(DirectGdi::ESolidBrush);
	iGc->SetTextShadowColor(KRgbGreen);
	TEST(iGc->PenColor() == KRgbRed);
	
	// Do some drawing
	iGc->DrawEllipse(TRect(5, 5, 50, 50));	
	iGc->SetFont(iFont);	
	iGc->SetStrikethroughStyle(DirectGdi::EStrikethroughOn);
	iGc->SetCharJustification(10, 10);
	iGc->SetWordJustification(20, 20);
	iGc->DrawText(KAbcd, NULL, TPoint(10, 100));
	TESTNOERROR(iGc->GetError());
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameA)));	
		
	// Activate the second context and reset the context settings
	iGdiTarget->Activate(iGc2);
	iGc2->Reset();
	iGc2->Clear();
	TEST(iGc2->PenColor() == TRgb(0, 0, 0));
	// Copy the settings from the first context to the second one
	iGc2->CopySettings(*iGc);
	TEST(iGc2->PenColor() == TRgb(255, 0, 0));
	
	// Do some drawing, the ellipse is drawn deliberately smaller this time,
	// the text should look the same as before 
	iGc2->DrawEllipse(TRect(0, 0, 30, 30));
	iGc->DrawText(KAbcd, NULL, TPoint(10, 100));
	TESTNOERROR(iGc2->GetError());	
	TESTNOERROR(WriteTargetOutput(iTestParams, TPtrC(testCaseNameB)));
	
	// Test that the settings from iGc have been copied to iGc2
	TEST(iGc->PenColor() == iGc2->PenColor());
	TEST(iGc->BrushColor() == iGc2->BrushColor());
	TEST(iGc->TextShadowColor() == iGc2->TextShadowColor());
	
	iGc->ResetFont();
	iGc2->ResetFont();
	
	// Force a cleanup of the pen array when testing the SW version of DirectGdi, 
	// if this is not done the OOM tests fail when running this test.
	iGc->SetPenSize(TSize(1,1));
	iGc2->SetPenSize(TSize(1,1));
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTDirectGdiContext::doTestStepL()
	{			
	// Test for each pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{	
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		for(iCaseNum = EOneContextOneTarget; iCaseNum < ELast; ++iCaseNum)
			{			
			SetTargetL(iTestParams.iTargetPixelFormat, static_cast<TContextTestCase>(iCaseNum));							
			
			// Create fonts outside of the OOM tests to avoid cached fonts being included
			iFont = GetFont();
			User::LeaveIfNull(iFont);					
	
			RunTestsL();
			// only run OOM tests for one target pixel format to prevent duplication of tests
			if (targetPixelFormatIndex == 0)
				{
				RunOomTestsL();  //from base class
				}		
			if(iFont)
				{
				ReleaseFont(iFont);
				iFont = NULL;
				}			
			}
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDirectGdiContext::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDirectGdiContext::RunTestsL()
	{
	SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iCaseNum)
		{
		case EOneContextOneTarget:
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0001"));
			OneContextOneTarget();
			if (!iRunningOomTests)
				{
				RecordTestResultL();
				SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0007"));
				ExternalizeInternalizeTestL();
				}
			break;
		case EOneContextTwoTargets_SamePixelType:
		case EOneContextTwoTargets_DifferentPixelType:
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0002"));
			OneContextTwoTargets();
			break;
		case ETwoContextsOneTarget:
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0003"));
			TwoContextsOneTarget();			
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0008"));
			TestCopySettings();			
			break;
		case ETwoContextsTwoTargets_WithoutSharing_SamePixelType:
		case ETwoContextsTwoTargets_WithoutSharing_DifferentPixelType:
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0004"));
			TwoContextsTwoTargets_WithoutSharingL();
			break;
		case ETwoContextsTwoTargets_WithSharing_SamePixelType:
		case ETwoContextsTwoTargets_WithSharing_DifferentPixelType:
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0005"));
			TwoContextsTwoTargets_WithSharingL();
			break;
		case EOneContextOneTarget_TwiceActivate:
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-CONTEXT-0006"));
			OneContextOneTarget_TwiceActivateL();
			break;				
		}
	RecordTestResultL();
	
	}
