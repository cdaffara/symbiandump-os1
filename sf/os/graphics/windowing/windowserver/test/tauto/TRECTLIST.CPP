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
// Generate a list of rectangles to be used as parameters for graphics tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "TRECTLIST.H"

CTRectList::CTRectList(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

CTRectList::~CTRectList()
	{
	}

void CTRectList::ConstructL()
	{
	TheGc->Activate(*BaseWin->Win());
	TheGc->SetBrushColor(TRgb::Gray16(14));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(BaseWin->Win()->Size()));
	TInt scale=16;
	TSize size(TestWin->Size());
	TSize scaleSize(size.iWidth/scale,size.iHeight/scale);
	iOffset1=TPoint((size.iWidth-scaleSize.iWidth)/2,(size.iWidth-scaleSize.iHeight)/2);
	TheGc->DrawRect(TRect(iOffset1,scaleSize));
	TheGc->Deactivate();
//
	TheGc->Activate(*TestWin->Win());
	TheGc->SetBrushColor(TRgb::Gray16(15));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(TestWin->Win()->Size()));
	{
	TInt scale=1;
	TSize size(TestWin->Size());
	TSize scaleSize(size.iWidth/scale,size.iHeight/scale);
	iOffset2=TPoint((size.iWidth-scaleSize.iWidth)/2,(size.iWidth-scaleSize.iHeight)/2);
	TheGc->DrawRect(TRect(iOffset2,scaleSize));
	TheGc->Deactivate();
	}
	}
	
void CTRectList::RunTestCaseL(TInt /*aCurTestCase*/)
	{	
	((CTRectListStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
	/**
@SYMTestCaseID		GRAPHICS-WSERV-0028

@@SYMDEF            DEF081259

@SYMTestCaseDesc    Draw rectangles from a list in two different positions
					on the screen

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw a list of rectangles to the screen 

@SYMTestExpectedResults Rectangles are drawn without error
*/
	case 1:
		((CTRectListStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0028"));
		iTest->LogSubTest(_L("RectListCheck"));	
			{
			TheGc->Activate(*BaseWin->Win());
			TheGc->SetPenColor(TRgb::Gray16(7));
			TInt scale=16;
			for(TInt index=0;index<iTest->iTestRects.Count3();index++)
				{
				TRect rect=iTest->iTestRects[index];
				rect.iTl.iX/=scale;
				rect.iTl.iY/=scale;
				rect.iBr.iX/=scale;
				rect.iBr.iY/=scale;
				rect.Move(iOffset1);
				TheGc->DrawRect(rect);
				}
			TheGc->Deactivate();
			}
			{
			TheGc->Activate(*TestWin->Win());
			TheGc->SetPenColor(TRgb::Gray16(7));
			TInt scale=1;
			for(TInt index=0;index<iTest->iTestRects.Count3();index++)
				{
				TRect rect=iTest->iTestRects[index];
				rect.iTl.iX/=scale;
				rect.iTl.iY/=scale;
				rect.iBr.iX/=scale;
				rect.iBr.iY/=scale;
				rect.Move(iOffset2);
				TheGc->DrawRect(rect);
				}
			TheClient->iWs.Flush();
			TheGc->Deactivate();
			}
			TEST(ETrue);
			break;
	case 2:
		((CTRectListStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		iTest->LogSubTest(_L("Panic"));
		break;
	case 3:
		((CTRectListStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTRectListStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTRectListStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(RectList)
