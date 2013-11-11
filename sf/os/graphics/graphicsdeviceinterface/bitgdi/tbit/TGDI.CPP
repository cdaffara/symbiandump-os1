// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test wrapper to bitgdi component of graphics.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
#include <tgdi.mbg>

#include "TGDI.H"
#include "TBMP.H"
#include <bitdev.h>

#define MAXCOLORS 255

_LIT(KT23BitmapFileName,"z:\\system\\data\\tgdi.mbm");

// TestGdi methods

EXPORT_C TestGdi::TestGdi():
	iDev(NULL),
	iCon(NULL),
	iSize(0,0),
	iSection(NULL)
	{}

// This function is modified. contents are moved to below function
EXPORT_C void TestGdi::Construct(CBitmapDevice* aDevice,CFbsBitGc* aContext,TDes& aFailureSection)
	{
	TRAP_IGNORE(ConstructL(aDevice, aContext, aFailureSection));
	}

// This is additional function with suffix L
EXPORT_C void TestGdi::ConstructL(CBitmapDevice* aDevice,CFbsBitGc* aContext,TDes& aFailureSection)
	{
	__ASSERT_ALWAYS(aFailureSection.MaxLength() == KFailureSectionNameBufferLength,User::Panic(_L("TGDI"),KErrBadDescriptor));

	iDev = aDevice;
	iCon = aContext;
	iSize = iDev->SizeInPixels();
	iSection = &aFailureSection;

	iCon->SetPenSize(TSize(1,1));
	iCon->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iCon->SetPenColor(KRgbBlack);
	iCon->SetPenStyle(CGraphicsContext::ESolidPen);
	iCon->SetBrushColor(KRgbBlack);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);

	iRect.iTl.iX=iSize.iWidth/3;
	iRect.iTl.iY=iSize.iHeight/3;
	iRect.iBr.iX=iSize.iWidth*2/3;
	iRect.iBr.iY=iSize.iHeight*2/3;
	iBitmapWidth=50;
	iBitmapHeight=87;
	iZeroRect.SetRect(iRect.iTl,iRect.iTl);
	iUnitRect.SetRect(iRect.iTl,iRect.iTl+TPoint(1,1));
	iUnNormRect.iTl=iRect.iBr;
	iUnNormRect.iBr=iRect.iTl;
	iLargeRect.SetRect(-iSize.iWidth,-iSize.iHeight,iSize.iWidth*2,iSize.iHeight*2);
	iCentredRect.iTl=TPoint(0,0)-iRect.iTl;
	iCentredRect.iBr=iRect.iTl;
	for(TInt count=0;count<8;count++)
		iOffScreenRects[count]=iRect;
	iOffScreenRects[0].Move(-iSize.iWidth,-iSize.iHeight);
	iOffScreenRects[1].Move(0,-iSize.iHeight);
	iOffScreenRects[2].Move(iSize.iWidth,-iSize.iHeight);
	iOffScreenRects[3].Move(iSize.iWidth,0);
	iOffScreenRects[4].Move(iSize.iWidth,iSize.iHeight);
	iOffScreenRects[5].Move(0,iSize.iHeight);
	iOffScreenRects[6].Move(-iSize.iWidth,iSize.iHeight);
	iOffScreenRects[7].Move(-iSize.iWidth,0);

	TInt ret = iBitmap.Load(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTgdigen,EFalse);
	User::LeaveIfError(ret);

	ret = iBitmap2.Load(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTblank,EFalse);
	User::LeaveIfError(ret);
	}

EXPORT_C TBool TestGdi::Test()
	{
	// Get FBSERV to create its long lived objects to avoid trigering memory leaks.
	CFbsBitmap* srcBmp = new CFbsBitmap;
	if (srcBmp==NULL || srcBmp->Create(iSize,EColor16MU)!=KErrNone)
		{
		return EFalse;
		}
	delete srcBmp;
	
	CFbsBitmap* t23SrcBmp = new (ELeave) CFbsBitmap();
	TInt ret = t23SrcBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_sourcebitmap);
	delete t23SrcBmp;
	if(ret != KErrNone)
		{
		return EFalse;
		}
	
	TRAPD(err,DoTestL());
	if (err != KErrNone)
		iSection->AppendFormat(_L(", error %d"), err);

	iBitmap.Reset();
	iBitmap2.Reset();

	return err == KErrNone;
	}

void TestGdi::DoTestL()
	{
	*iSection = _L("Testing graphics functions");
	CheckL(iSize!=TSize(0,0));
	Clear();
	TRgb col;
	iDev->GetPixel(col,TPoint(iSize.iWidth/2,iSize.iHeight/2));
	CheckL(col==KRgbWhite);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	Clear();
	CheckClearL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0001
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0001"));
	DoArcL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0002
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0002"));
	DoEllipseL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0003
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0003"));
	DoRoundRectL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0004
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0004"));
	DoPieL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0005
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0005"));
	DoModeL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0006
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0006"));
	DoColorL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0007
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0007"));
	DoLineL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0008
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0008"));
	DoWideLineL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0009
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0009"));
	DoRectL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0010
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0010"));
	DoPolygonArrayL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0011
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0011"));
	DoPolygonPtrL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0012
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0012"));
	DoPolygonPtr0();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0013
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0013"));
	DoText();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0014
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0014"));
	DoPaintL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0015
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0015"));
	DoCopyL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0016
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0016"));
	DoBltL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0017
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0017"));
	DoBltMaskedL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0018
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0018"));
	DoShadowL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0019
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0019"));
	DoFadeL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0020
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0020"));
	DoDrawBitmapSemiTransparentMaskedL();
	RecordTestResultL();
	// If DoDrawBitmapMaskedL causes a User Panic 21, it might be a positive failure of case #24. 
	SetTestStepID(_L("GRAPHICS-TestGdi-DoDrawBitmapMaskedL-0001"));	
	DoDrawBitmapMaskedL(ETrue);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-TestGdi-DoDrawBitmapMaskedL-0001"));
	DoDrawBitmapMaskedL(EFalse);
	RecordTestResultL();
	//test bitmap width 52 (wordalignable)
	iBitmapWidth=52;
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0021
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0021"));
	DoBltL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0022
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0022"));
	DoBltMaskedL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-TestGdi-DoDrawBitmapMaskedL-0001"));
	DoDrawBitmapMaskedL(ETrue);
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-TestGdi-DoDrawBitmapMaskedL-0001"));
	DoDrawBitmapMaskedL(EFalse);
	RecordTestResultL();
	//test with compressed bitmaps
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0023
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0023"));
	DoBltCompressedL();
	RecordTestResultL();
/**
 @SYMTestCaseID GRAPHICS-TestGdi-0024
*/
	SetTestStepID(_L("GRAPHICS-TestGdi-0024"));
	DoBoxTextVertical();
	RecordTestResultL();
	}

void TestGdi::CheckL(TBool aValue)
	{
	if (!aValue)
		User::Leave(KErrGeneral);
	}

void TestGdi::Clear()
	{
	iCon->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iCon->SetBrushColor(KRgbWhite);
	iCon->DrawRect(TRect(TPoint(0,0),iSize));
	iCon->SetPenStyle(CGraphicsContext::ESolidPen);
	iCon->SetPenColor(KRgbBlack);
	iCon->SetBrushColor(KRgbBlack);
	}

LOCAL_C void FreeAlloc(TAny* aPtr)
	{
	TRgb* scanline = reinterpret_cast <TRgb*> (aPtr);
	User::Free(scanline);
	}
	
void TestGdi::ScanAreaL(TInt x,TInt y,TInt length,TInt height,TRgb Color)
	{
	if(x>iSize.iWidth)
		return;
	if(x<0)
		{
		length=length+x;
		x=0;
		}
	if(x+length>iSize.iWidth)
		length=iSize.iWidth-x;
	if(length<=0)
		return;
	TRgb* scanline=(TRgb*)User::Alloc(sizeof(TRgb)*length);
	CleanupStack::PushL(TCleanupItem(FreeAlloc, scanline));
	TPtr8 scln((TUint8*)scanline,length*sizeof(TRgb));
	for(TInt row=0;row<height;row++)
		{
		if(y+row>=0 && y+row<iSize.iHeight)
			{
			iDev->GetScanLine(scln,TPoint(x,y+row),length,ERgb);
			for(TInt index=0;index<length;index++)
				{
				CheckL(scanline[index] == Color);
				}
				
			}
		}
	for(TInt i=0;i<height;i++)
		{
		scanline[i].~TRgb();
		}
	CleanupStack::PopAndDestroy(scanline);
	}

void TestGdi::CheckClearL()
	{
	TInt quarht=iSize.iHeight>>2;
	TInt halfht=iSize.iHeight>>1;
	TInt thrqht=quarht+halfht;
	ScanAreaL(0,0,iSize.iWidth,1,KRgbWhite);
	ScanAreaL(0,quarht,iSize.iWidth,quarht+1,KRgbWhite);
	ScanAreaL(0,halfht,iSize.iWidth,halfht+1,KRgbWhite);
	ScanAreaL(0,thrqht,iSize.iWidth,thrqht+1,KRgbWhite);
	ScanAreaL(0,iSize.iHeight-1,iSize.iWidth,iSize.iHeight,KRgbWhite);
	ScanAreaL(iRect.iTl.iX,iRect.iTl.iY,1,1,KRgbWhite);
	}

void TestGdi::CheckPlotL(const TPoint& pt)
	{
	ScanAreaL(pt.iX,pt.iY,1,1,KRgbBlack);
	ScanAreaL(pt.iX-1,pt.iY-1,3,1,KRgbWhite);
	ScanAreaL(pt.iX-1,pt.iY+1,3,1,KRgbWhite);
	ScanAreaL(pt.iX-1,pt.iY,1,1,KRgbWhite);
	ScanAreaL(pt.iX+1,pt.iY,1,1,KRgbWhite);
	Clear();
	}

void TestGdi::CheckRectInsideL(const TRect& rect)
	{
	ScanAreaL(rect.iTl.iX,rect.iTl.iY,rect.iBr.iX-rect.iTl.iX,1,KRgbBlack);
	ScanAreaL(rect.iTl.iX,rect.iBr.iY-1,rect.iBr.iX-rect.iTl.iX,1,KRgbBlack);
	}

void TestGdi::CheckRectOutsideL(const TRect& rect)
	{
	ScanAreaL(rect.iTl.iX-1,rect.iTl.iY-1,rect.iBr.iX-rect.iTl.iX,1,KRgbWhite);
	ScanAreaL(rect.iTl.iX-1,rect.iBr.iY,rect.iBr.iX-rect.iTl.iX+1,1,KRgbWhite);
	}

void TestGdi::CheckQuadOutsideL(const TRect& rect)
	{
	ScanAreaL(rect.iTl.iX-1,rect.iTl.iY-6,rect.iBr.iX-rect.iTl.iX+1,1,KRgbWhite);
	ScanAreaL(rect.iTl.iX-1,rect.iBr.iY-5,rect.iBr.iX-rect.iTl.iX+1,1,KRgbWhite);
	}

void TestGdi::CheckQuadInsideL(const TRect& rect)
	{
	if(rect.iBr.iX-rect.iTl.iX-11>0)
		ScanAreaL(rect.iTl.iX+5,(rect.iTl.iY+rect.iBr.iY)/2,rect.iBr.iX-rect.iTl.iX-11,1,KRgbBlack);
	}

void TestGdi::DoModeL()
	{
	if(iDev->DisplayMode()==EGray2) return;
	*iSection = _L("Drawing mode");
	Clear();
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);

	iCon->SetDrawMode(CGraphicsContext::EDrawModeNOTPEN);
	iCon->SetPenColor(KRgbWhite);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);

	iCon->SetPenColor(KRgbDarkGray);
	iCon->Plot(iRect.iTl);
	iCon->SetDrawMode(CGraphicsContext::EDrawModeAND);
	iCon->SetPenColor(KRgbGray);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->SetPenColor(KRgbGray);
	iCon->Plot(iRect.iTl);
	iCon->SetDrawMode(CGraphicsContext::EDrawModeANDNOT);
	iCon->SetPenColor(KRgbGray);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->Plot(iRect.iTl);
	iCon->SetDrawMode(CGraphicsContext::EDrawModeOR);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->Plot(iRect.iTl);
	iCon->SetDrawMode(CGraphicsContext::EDrawModeORNOT);
	iCon->SetPenColor(KRgbWhite);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->SetPenColor(KRgbGray);
	iCon->Plot(iRect.iTl);
	iCon->SetDrawMode(CGraphicsContext::EDrawModeXOR);
	iCon->SetPenColor(KRgbGray);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->SetPenColor(KRgbDarkGray);
	iCon->Plot(iRect.iTl);
	iCon->SetDrawMode(CGraphicsContext::EDrawModeNOTXOR);
	iCon->SetPenColor(KRgbGray);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->SetDrawMode(CGraphicsContext::EDrawModeNOTSCREEN);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->SetDrawMode(CGraphicsContext::EDrawModeNOTAND);
	iCon->SetPenColor(KRgbWhite);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->Plot(iRect.iTl);
	iCon->SetDrawMode(CGraphicsContext::EDrawModeNOTANDNOT);
	iCon->SetPenColor(KRgbWhite);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	
	iCon->SetDrawMode(CGraphicsContext::EDrawModeNOTOR);
	iCon->SetPenColor(KRgbBlack);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);

	iCon->SetDrawMode(CGraphicsContext::EDrawModeNOTORNOT);
	iCon->SetPenColor(KRgbWhite);
	iCon->Plot(iRect.iTl);
	CheckPlotL(iRect.iTl);
	}

void TestGdi::DoColorL()
	{
	*iSection = _L("Colors");
	Clear();
	for(TInt count=0;count<256;count++)
		{
		iCon->SetPenColor(TRgb::Gray256(count));
		iCon->Plot(iRect.iTl);
		TRgb screencolor;
		iDev->GetPixel(screencolor,iRect.iTl);
		switch(iDev->DisplayMode())
			{
		case EGray2:
			CheckL(screencolor.Gray2()==count>>7);
			break;
		case EGray4:
			CheckL(Abs(screencolor.Gray4()-(count>>6))<2);
			break;
		case EGray16:
			if(screencolor.Gray16()!=(count>>4))
				CheckL(Abs(screencolor.Gray4()-(count>>6))<2);
			break;
		case EGray256:
			CheckL(screencolor.Gray256()==count);
			break;
		default:
			break;
			}
		iCon->SetPenColor(KRgbWhite);
		iCon->Plot(iRect.iTl);
		iDev->GetPixel(screencolor,iRect.iTl);
		CheckL(screencolor==KRgbWhite);
		}
	}

void TestGdi::DoLineL()
	{
	*iSection = _L("Line drawing");
	Clear();
	iCon->DrawLine(iRect.iTl,iRect.iTl);
	iCon->DrawLine(iRect.iTl,iRect.iTl+TPoint(1,1));
	CheckPlotL(iRect.iTl);
	Clear();
	iCon->DrawLine(iRect.iTl,TPoint(iRect.iBr.iX,iRect.iTl.iY));
	CheckRectInsideL(TRect(iRect.iTl,TPoint(iRect.iBr.iX,iRect.iTl.iY+1)));
	CheckRectOutsideL(TRect(iRect.iTl,TPoint(iRect.iBr.iX,iRect.iTl.iY+1)));
	Clear();
	iCon->DrawLine(TPoint(-iRect.iTl.iX,iRect.iTl.iY),TPoint(iRect.iBr.iX*2,iRect.iTl.iY));
	CheckRectInsideL(TRect(-iRect.iTl.iX,iRect.iTl.iY,iRect.iBr.iX*2,iRect.iTl.iY+1));
	CheckRectOutsideL(TRect(-iRect.iTl.iX,iRect.iTl.iY,iRect.iBr.iX*2,iRect.iTl.iY+1));
	Clear();
	TInt y=iRect.iTl.iY;
	for(;y<iRect.iBr.iY;y++)
		iCon->DrawLine(TPoint(iRect.iTl.iX,y),TPoint(iRect.iBr.iX,y));
	CheckRectInsideL(iRect);
	CheckRectOutsideL(iRect);
	Clear();
	for(TInt count=0;count<8;count++)
		for(y=iOffScreenRects[count].iBr.iY;y<iOffScreenRects[count].iTl.iY;y++)
			iCon->DrawLine(TPoint(iOffScreenRects[count].iTl.iX,y),TPoint(iOffScreenRects[count].iBr.iX,y));
	CheckClearL();
	iCon->SetPenSize(TSize(100,100));
	iCon->DrawLine(TPoint(iRect.iTl.iX,iRect.iTl.iY),TPoint(iRect.iBr.iX,iRect.iTl.iY));
	CheckRectInsideL(TRect(iRect.iTl.iX+50,iRect.iTl.iY-49,iRect.iBr.iX-50,iRect.iTl.iY+50));
	CheckRectOutsideL(TRect(iRect.iTl.iX,iRect.iTl.iY-49,iRect.iBr.iX,iRect.iTl.iY+51));
	Clear();
	iCon->DrawLine(TPoint(-iRect.iTl.iX,iRect.iTl.iY),TPoint(iRect.iBr.iX*2,iRect.iTl.iY));
	CheckRectInsideL(TRect(-iRect.iTl.iX,iRect.iTl.iY-49,iRect.iBr.iX*2,iRect.iTl.iY+50));
	CheckRectOutsideL(TRect(-iRect.iTl.iX,iRect.iTl.iY-49,iRect.iBr.iX*2,iRect.iTl.iY+51));
	Clear();
	iCon->SetPenSize(TSize(1,1));
	}

void TestGdi::DoWideLineL()
	{
	*iSection = _L("Wide Line drawing");

	Clear();
	iCon->SetPenStyle(CFbsBitGc::ESolidPen);
	iCon->SetPenColor(KRgbBlack);
	iCon->SetPenSize(TSize(4,4));
	TPoint pos(-24,10);
	TSize size(288,216);
	
	// Black arrow, pointing up.
	TPoint midLeft(pos.iX, size.iHeight / 2 + pos.iY);
	TPoint midTop(pos.iX + size.iWidth / 2, pos.iY);
	TPoint midRight(size.iWidth, size.iHeight / 2 + pos.iY);
	TPoint midBottom(pos.iX + size.iWidth / 2, size.iHeight);	
	iCon->DrawLine(midLeft, midTop);
	iCon->DrawLine(midTop, midRight);
	iCon->DrawLine(midTop, midBottom);

	// Check halfway along left side of arrowhead
	TPoint checkPt(midLeft);
	checkPt += midTop;
	checkPt.iX /= 2;
	checkPt.iY /= 2;
	TRgb pixel;
	iDev->GetPixel(pixel, checkPt);
	if (pixel.Internal() != KRgbBlack.Internal())
		{
		User::Panic(_L("Line not found!"), 1);
		}
	
	Clear();
	iCon->SetPenSize(TSize(1,1));	
	}

void TestGdi::DoArcL()
	{
	*iSection = _L("Arcs");
	Clear();
	iCon->DrawArc(iRect,iRect.iTl,iRect.iTl);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawArc(iLargeRect,iRect.iTl,iRect.iTl);
	iCon->DrawArc(iUnNormRect,iRect.iTl,iRect.iTl);
	CheckClearL();
	iCon->DrawArc(iCentredRect,iCentredRect.iTl,iCentredRect.iTl);
	Clear();
	TInt count=0;
	for(;count<8;count++)
		iCon->DrawArc(iOffScreenRects[count],iOffScreenRects[count].iTl,iOffScreenRects[count].iTl);
	CheckClearL();
	#include "TGDIGEN.H"
	for(count=0;count<numellipses;count++)
		iCon->DrawArc(ellrect[count],ellrect[count].iTl,ellrect[count].iTl);
	TSize size=iBitmap.SizeInPixels();
	if(size.iWidth>iSize.iWidth)
		size.iWidth=iSize.iWidth;
	if(size.iHeight>iSize.iHeight)
		size.iHeight=iSize.iHeight;
	for(TInt yy=0;yy<size.iHeight>>1;yy++)
		{
		for(TInt xx=0;xx<size.iWidth>>1;xx++)
			{
			TPoint pt(xx,yy);
			TRgb bmprgb,scrrgb;
			iBitmap.GetPixel(bmprgb,pt);
			iDev->GetPixel(scrrgb,pt);
			if(bmprgb==KRgbBlack)
				CheckL(scrrgb==KRgbBlack);
			}
		}
	}

void TestGdi::DoRectL()
	{
	*iSection = _L("Rectangles");
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawRect(iRect);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iCon->DrawRect(iRect);
	CheckRectInsideL(iRect);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iCon->DrawRect(iRect);
	CheckRectInsideL(iRect);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->DrawRect(iUnitRect);
	CheckPlotL(iUnitRect.iTl);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawRect(iUnitRect);
	CheckPlotL(iUnitRect.iTl);
	Clear();
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->DrawRect(iUnitRect);
	CheckPlotL(iUnitRect.iTl);
	Clear();
	iCon->DrawRect(iCentredRect);
	CheckRectInsideL(iCentredRect);
	CheckRectOutsideL(iCentredRect);
	Clear();
	iCon->DrawRect(iZeroRect);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iCon->DrawRect(iZeroRect);
	iCon->DrawRect(iUnNormRect);
	for(TInt count=0;count<8;count++)
		iCon->DrawRect(iOffScreenRects[count]);
	CheckClearL();
	iCon->DrawRect(TRect(iRect.iTl.iX/2,iRect.iTl.iY,iRect.iTl.iX,iRect.iBr.iY));
	iCon->CopyRect(TPoint(iSize.iWidth/2,iSize.iHeight/6),TRect(iRect.iTl.iX/2,iRect.iTl.iY,iRect.iTl.iX,iRect.iBr.iY));
	CheckRectInsideL(TRect(iRect.iTl.iX/2+iSize.iWidth/2,iRect.iTl.iY+iSize.iHeight/6,iRect.iTl.iX+iSize.iWidth/2,iRect.iBr.iY+iSize.iHeight/6));
	CheckRectOutsideL(TRect(iRect.iTl.iX/2+iSize.iWidth/2,iRect.iTl.iY+iSize.iHeight/6,iRect.iTl.iX+iSize.iWidth/2,iRect.iBr.iY+iSize.iHeight/6));
	Clear();
	}

void TestGdi::DoEllipseL()
	{
	*iSection = _L("Ellipses");
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawEllipse(iRect);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawEllipse(iRect);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->DrawEllipse(iRect);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->DrawEllipse(iLargeRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->DrawEllipse(iCentredRect);
	CheckQuadInsideL(iCentredRect);
	CheckRectOutsideL(iCentredRect);
	Clear();
	iCon->DrawEllipse(iUnNormRect);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawEllipse(iLargeRect);
	TInt count=0;
	for(;count<8;count++)
		iCon->DrawEllipse(iOffScreenRects[count]);
	CheckClearL();
	#include "TGDIGEN.H"
	for(count=0;count<numellipses;count++)
		iCon->DrawEllipse(ellrect[count]);
	TSize size=iBitmap.SizeInPixels();
	if(size.iWidth>iSize.iWidth)
		size.iWidth=iSize.iWidth;
	if(size.iHeight>iSize.iHeight)
		size.iHeight=iSize.iHeight;
	for(TInt yy=0;yy<size.iHeight>>1;yy++)
		{
		for(TInt xx=0;xx<size.iWidth>>1;xx++)
			{
			TPoint pt(xx,yy);
			TRgb bmprgb,scrrgb;
			iBitmap.GetPixel(bmprgb,pt);
			iDev->GetPixel(scrrgb,pt);
			if(bmprgb==KRgbBlack)
				CheckL(scrrgb==KRgbBlack);
			}
		}
	}

void TestGdi::DoPieL()
	{
	*iSection = _L("Pie segments");
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPie(iRect,iRect.iTl,iRect.iTl);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawPie(iRect,iRect.iTl,iRect.iTl);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->DrawPie(iRect,iRect.iTl,iRect.iTl);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->DrawPie(iLargeRect,iRect.iTl,iRect.iTl);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->DrawPie(iCentredRect,iCentredRect.iTl,iCentredRect.iTl);
	CheckQuadInsideL(iCentredRect);
	CheckRectOutsideL(iCentredRect);
	Clear();
	iCon->DrawPie(iUnNormRect,iRect.iTl,iRect.iTl);
	TInt count=0;
	for(;count<8;count++)
		iCon->DrawPie(iOffScreenRects[count],iOffScreenRects[count].iTl,iOffScreenRects[count].iTl);
	CheckClearL();
	#include "TGDIGEN.H"
	for(count=0;count<numellipses;count++)
		iCon->DrawPie(ellrect[count],ellrect[count].iTl,ellrect[count].iTl);
	TSize size=iBitmap.SizeInPixels();
	if(size.iWidth>iSize.iWidth)
		size.iWidth=iSize.iWidth;
	if(size.iHeight>iSize.iHeight)
		size.iHeight=iSize.iHeight;
	for(TInt yy=0;yy<size.iHeight>>1;yy++)
		{
		for(TInt xx=0;xx<size.iWidth>>1;xx++)
			{
			TPoint pt(xx,yy);
			TRgb bmprgb,scrrgb;
			iBitmap.GetPixel(bmprgb,pt);
			iDev->GetPixel(scrrgb,pt);
			if(bmprgb==KRgbBlack)
				CheckL(scrrgb==KRgbBlack);
			}
		}
	}

void TestGdi::DoRoundRectL()
	{
	*iSection = _L("RoundRects");
	TSize rrsize(20,10);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawRoundRect(iRect,rrsize);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawRoundRect(iRect,rrsize);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->DrawRoundRect(iRect,rrsize);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->DrawRoundRect(iLargeRect,rrsize);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->DrawRoundRect(iCentredRect,rrsize);
	CheckQuadInsideL(iCentredRect);
	CheckRectOutsideL(iCentredRect);
	Clear();
	iCon->DrawRoundRect(iUnNormRect,rrsize);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawRoundRect(iLargeRect,rrsize);
	TInt count=0;
	for(;count<8;count++)
		iCon->DrawRoundRect(iOffScreenRects[count],rrsize);
	CheckClearL();
	#include "TGDIGEN.H"
	for(count=0;count<numellipses;count++)
		iCon->DrawRoundRect(ellrect[count],TSize((ellrect[count].iBr.iX-ellrect[count].iTl.iX)>>1,(ellrect[count].iBr.iY-ellrect[count].iTl.iY)>>1));
	TSize size=iBitmap.SizeInPixels();
	if(size.iWidth>iSize.iWidth)
		size.iWidth=iSize.iWidth;
	if(size.iHeight>iSize.iHeight)
		size.iHeight=iSize.iHeight;
	for(TInt yy=0;yy<size.iHeight>>1;yy++)
		{
		for(TInt xx=0;xx<size.iWidth>>1;xx++)
			{
			TPoint pt(xx,yy);
			TRgb bmprgb,scrrgb;
			iDev->GetPixel(scrrgb,pt);
			pt.iY+=size.iHeight>>1;
			iBitmap.GetPixel(bmprgb,pt);
			if(bmprgb==KRgbBlack)
				CheckL(scrrgb==KRgbBlack);
			}
		}
	}

void TestGdi::DoPolygonArrayL()
	{
	*iSection = _L("Polygon array");

	CArrayFixFlat<TPoint>* poly=new CArrayFixFlat<TPoint>(4);
	poly->AppendL(iRect.iTl);
	TPoint temp(iRect.iBr.iX-1,iRect.iTl.iY);
	poly->AppendL(temp);
	temp.SetXY(iRect.iBr.iX-1,iRect.iBr.iY-1);
	poly->AppendL(temp);
	temp.SetXY(iRect.iTl.iX,iRect.iBr.iY-1);
	poly->AppendL(temp);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawPolygon(poly);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->DrawPolygon(poly);
	delete poly;
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();

	poly=new CArrayFixFlat<TPoint>(4);
	poly->AppendL(iLargeRect.iTl);
	temp.SetXY(iLargeRect.iBr.iX-1,iLargeRect.iTl.iY);
	poly->AppendL(temp);
	temp.SetXY(iLargeRect.iBr.iX-1,iLargeRect.iBr.iY-1);
	poly->AppendL(temp);
	temp.SetXY(iLargeRect.iTl.iX,iLargeRect.iBr.iY-1);
	poly->AppendL(temp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly);
	CheckClearL();
	Clear();
	iCon->DrawPolygon(poly);
	delete poly;
	CheckQuadInsideL(iRect);
	Clear();

	poly=new CArrayFixFlat<TPoint>(4);
	temp.SetXY(iUnNormRect.iTl.iX-1,iUnNormRect.iTl.iY-1);
	poly->AppendL(temp);
	temp.SetXY(iUnNormRect.iBr.iX,iUnNormRect.iTl.iY-1);
	poly->AppendL(temp);
	poly->AppendL(iUnNormRect.iBr);
	temp.SetXY(iUnNormRect.iTl.iX-1,iUnNormRect.iBr.iY);
	poly->AppendL(temp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawPolygon(poly);
	delete poly;
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();

	poly=new CArrayFixFlat<TPoint>(4);
	poly->AppendL(iCentredRect.iTl);
	temp.SetXY(iCentredRect.iBr.iX-1,iCentredRect.iTl.iY);
	poly->AppendL(temp);
	temp.SetXY(iCentredRect.iBr.iX-1,iCentredRect.iBr.iY-1);
	poly->AppendL(temp);
	temp.SetXY(iCentredRect.iTl.iX,iCentredRect.iBr.iY-1);
	poly->AppendL(temp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly);
	CheckRectOutsideL(iCentredRect);
	Clear();
	iCon->DrawPolygon(poly);
	delete poly;
	CheckRectOutsideL(iCentredRect);
	CheckQuadInsideL(iCentredRect);
	Clear();

	for(TInt count=0;count<8;count++)
		{
		poly=new CArrayFixFlat<TPoint>(4);
		poly->AppendL(iOffScreenRects[count].iTl);
		temp.SetXY(iOffScreenRects[count].iBr.iX,iOffScreenRects[count].iTl.iY);
		poly->AppendL(temp);
		poly->AppendL(iOffScreenRects[count].iBr);
		temp.SetXY(iOffScreenRects[count].iTl.iX,iOffScreenRects[count].iBr.iY);
		poly->AppendL(temp);
		iCon->DrawPolygon(poly);
		delete poly;
		}

	poly=new CArrayFixFlat<TPoint>(4);
	const TInt offset = 10000;
	const TInt size = 50;
	TRect ppRect(0,0,size + 1,size + 1);
	TPoint pp(offset,offset);
	poly->AppendL(pp);
	pp.iX += size;
	poly->AppendL(pp);
	pp.iY += size;
	poly->AppendL(pp);
	pp.iX -= size;
	poly->AppendL(pp);
	Clear();
	iCon->SetOrigin(TPoint(-offset,-offset));
	iCon->DrawPolygon(poly);
	iCon->SetOrigin(TPoint(0,0));
	delete poly;
	CheckRectOutsideL(ppRect);
	CheckQuadInsideL(ppRect);
	Clear();

	CheckClearL();
	}

void TestGdi::DoPolygonPtr0()
	{
	*iSection = _L("Polygon ptr 0 points");

	TPoint poly[4];
	poly[0] = iRect.iTl;
	poly[1].SetXY(iRect.iBr.iX-1,iRect.iTl.iY);
	poly[2].SetXY(iRect.iBr.iX-1,iRect.iBr.iY-1);
	poly[3].SetXY(iRect.iTl.iX,iRect.iBr.iY-1);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,0);
	iCon->DrawPolygon(poly,0);
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->DrawPolygon(poly,0);

	poly[0] = iLargeRect.iTl;
	poly[1].SetXY(iLargeRect.iBr.iX-1,iLargeRect.iTl.iY);
	poly[2].SetXY(iLargeRect.iBr.iX-1,iLargeRect.iBr.iY-1);
	poly[3].SetXY(iLargeRect.iTl.iX,iLargeRect.iBr.iY-1);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,0);
	iCon->DrawPolygon(poly,0);

	poly[0].SetXY(iUnNormRect.iTl.iX-1,iUnNormRect.iTl.iY-1);
	poly[1].SetXY(iUnNormRect.iBr.iX,iUnNormRect.iTl.iY-1);
	poly[2] = iUnNormRect.iBr;
	poly[3].SetXY(iUnNormRect.iTl.iX-1,iUnNormRect.iBr.iY);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,0);
	iCon->DrawPolygon(poly,0);

	poly[0] = iCentredRect.iTl;
	poly[1].SetXY(iCentredRect.iBr.iX-1,iCentredRect.iTl.iY);
	poly[2].SetXY(iCentredRect.iBr.iX-1,iCentredRect.iBr.iY-1);
	poly[3].SetXY(iCentredRect.iTl.iX,iCentredRect.iBr.iY-1);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,0);
	iCon->DrawPolygon(poly,0);

	for(TInt count=0;count<8;count++)
		{
		poly[0] = iOffScreenRects[count].iTl;
		poly[1].SetXY(iOffScreenRects[count].iBr.iX,iOffScreenRects[count].iTl.iY);
		poly[2] = iOffScreenRects[count].iBr;
		poly[3].SetXY(iOffScreenRects[count].iTl.iX,iOffScreenRects[count].iBr.iY);
		iCon->DrawPolygon(poly,0);
		}

	const TInt offset = 10000;
	const TInt size = 50;
	TPoint pp(offset,offset);
	poly[0] = pp;
	pp.iX += size;
	poly[1] = pp;
	pp.iY += size;
	poly[2] = pp;
	pp.iX -= size;
	poly[3] = pp;
	iCon->SetOrigin(TPoint(-offset,-offset));
	iCon->DrawPolygon(poly,0);
	iCon->SetOrigin(TPoint(0,0));

	poly[0] = iRect.iTl;
	poly[1].SetXY(iRect.iBr.iX-1,iRect.iTl.iY);
	poly[2].SetXY(iRect.iBr.iX-1,iRect.iBr.iY-1);
	poly[3].SetXY(iRect.iTl.iX,iRect.iBr.iY-1);
	iCon->DrawPolygon(NULL,4);
	iCon->DrawPolygon(poly,0);
	iCon->DrawPolygon(NULL,0);
	TPoint *ptr=(TPoint *)0x1111;
	iCon->DrawPolygon(ptr,0);
	iCon->DrawPolygon(NULL,4,CGraphicsContext::EWinding	);
	iCon->DrawPolygon(poly,0,CGraphicsContext::EWinding	);
	iCon->DrawPolygon(NULL,0,CGraphicsContext::EWinding	);
	iCon->DrawPolygon(ptr,0,CGraphicsContext::EWinding	);
	}

void TestGdi::DoPolygonPtrL()
	{
	*iSection = _L("Polygon ptr");

	TPoint poly[4];
	poly[0] = iRect.iTl;
	poly[1].SetXY(iRect.iBr.iX-1,iRect.iTl.iY);
	poly[2].SetXY(iRect.iBr.iX-1,iRect.iBr.iY-1);
	poly[3].SetXY(iRect.iTl.iX,iRect.iBr.iY-1);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,4);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawPolygon(poly,4);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->DrawPolygon(poly,4);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();

	poly[0] = iLargeRect.iTl;
	poly[1].SetXY(iLargeRect.iBr.iX-1,iLargeRect.iTl.iY);
	poly[2].SetXY(iLargeRect.iBr.iX-1,iLargeRect.iBr.iY-1);
	poly[3].SetXY(iLargeRect.iTl.iX,iLargeRect.iBr.iY-1);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,4);
	CheckClearL();
	Clear();
	iCon->DrawPolygon(poly,4);
	CheckQuadInsideL(iRect);
	Clear();

	poly[0].SetXY(iUnNormRect.iTl.iX-1,iUnNormRect.iTl.iY-1);
	poly[1].SetXY(iUnNormRect.iBr.iX,iUnNormRect.iTl.iY-1);
	poly[2] = iUnNormRect.iBr;
	poly[3].SetXY(iUnNormRect.iTl.iX-1,iUnNormRect.iBr.iY);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,4);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->DrawPolygon(poly,4);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();

	poly[0] = iCentredRect.iTl;
	poly[1].SetXY(iCentredRect.iBr.iX-1,iCentredRect.iTl.iY);
	poly[2].SetXY(iCentredRect.iBr.iX-1,iCentredRect.iBr.iY-1);
	poly[3].SetXY(iCentredRect.iTl.iX,iCentredRect.iBr.iY-1);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawPolygon(poly,4);
	CheckRectOutsideL(iCentredRect);
	Clear();
	iCon->DrawPolygon(poly,4);
	CheckRectOutsideL(iCentredRect);
	CheckQuadInsideL(iCentredRect);
	Clear();

	for(TInt count=0;count<8;count++)
		{
		poly[0] = iOffScreenRects[count].iTl;
		poly[1].SetXY(iOffScreenRects[count].iBr.iX,iOffScreenRects[count].iTl.iY);
		poly[2] = iOffScreenRects[count].iBr;
		poly[3].SetXY(iOffScreenRects[count].iTl.iX,iOffScreenRects[count].iBr.iY);
		iCon->DrawPolygon(poly,4);
		}

	const TInt offset = 10000;
	const TInt size = 50;
	TRect ppRect(0,0,size + 1,size + 1);
	TPoint pp(offset,offset);
	poly[0] = pp;
	pp.iX += size;
	poly[1] = pp;
	pp.iY += size;
	poly[2] = pp;
	pp.iX -= size;
	poly[3] = pp;
	Clear();
	iCon->SetOrigin(TPoint(-offset,-offset));
	iCon->DrawPolygon(poly,4);
	iCon->SetOrigin(TPoint(0,0));
	CheckRectOutsideL(ppRect);
	CheckQuadInsideL(ppRect);
	Clear();

	poly[0] = iRect.iTl;
	poly[1].SetXY(iRect.iBr.iX-1,iRect.iTl.iY);
	poly[2].SetXY(iRect.iBr.iX-1,iRect.iBr.iY-1);
	poly[3].SetXY(iRect.iTl.iX,iRect.iBr.iY-1);
	iCon->DrawPolygon(NULL,4);
	Clear();
	iCon->DrawPolygon(poly,0);
	Clear();
	iCon->DrawPolygon(NULL,0);
	Clear();
	TPoint *ptr=(TPoint *)0x1111;
	iCon->DrawPolygon(ptr,0);
	Clear();
	iCon->DrawPolygon(NULL,4,CGraphicsContext::EWinding	);
	Clear();
	iCon->DrawPolygon(poly,0,CGraphicsContext::EWinding	);
	Clear();
	iCon->DrawPolygon(NULL,0,CGraphicsContext::EWinding	);
	Clear();
	iCon->DrawPolygon(ptr,0,CGraphicsContext::EWinding	);
	Clear();

	CheckClearL();
	}

void TestGdi::DoText()
	{
	*iSection = _L("Text");
	Clear();
	iCon->DrawText(_L("Text printing..."),TPoint(0,iRect.iTl.iY));
	iCon->DrawText(_L("Text printing..."),TPoint(0,iRect.iBr.iY));
	Clear();
	iCon->DrawText(_L("Out of bounds checking"),TPoint(-50,iRect.iBr.iY>>1));
	iCon->DrawText(_L("Out of bounds checking"),TPoint(0,0));
	iCon->DrawText(_L("Out of bounds checking"),TPoint(0,iRect.iBr.iY));
	iCon->DrawText(_L("Out of bounds checking"),TPoint(iRect.iBr.iX,iRect.iBr.iY>>1));
	}

void TestGdi::DoBoxTextVertical()
	{	
	Clear(); 
	
	// Creates a font large enough to notice the text on screen
	_LIT(KMyFontName,"Swiss");
	CFont* myFont = NULL;
	TFontSpec myFontSpec(KMyFontName,15);
	User::LeaveIfError(iDev->GetNearestFontInPixels(myFont,myFontSpec));
	iCon->UseFont(myFont);
	
	TInt boxWidth = iSize.iWidth/4;
	TInt boxHeightOffset = iSize.iHeight-5;
		
	TRect box1 (5, 5, boxWidth-5, boxHeightOffset);
	TRect box2 (boxWidth+5, 5, 2*boxWidth-5, boxHeightOffset);
	TRect box3 (2*boxWidth+5, 5, 3*boxWidth-5, boxHeightOffset);
	TRect box4 (3*boxWidth+5, 5, iSize.iWidth-25-2*myFont->HeightInPixels(), boxHeightOffset);
	TRect box5 (iSize.iWidth-15-2*myFont->HeightInPixels(), 5, iSize.iWidth-15-myFont->HeightInPixels(), boxHeightOffset);
	TRect box6 (iSize.iWidth-5-myFont->HeightInPixels(), 5, iSize.iWidth-5, boxHeightOffset);

	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->SetPenStyle(CGraphicsContext::ESolidPen);
	iCon->DrawRect(box1);
	iCon->DrawRect(box2);
	iCon->DrawRect(box3);
	iCon->DrawRect(box4);
	iCon->DrawRect(box5);
	iCon->DrawRect(box6);
	
	/* Text drawn when baseline = font ascent.
	   Expected result: The top of the text will be aligned with the borders of the
	   box.
	*/
	// Upper right corner of the box
    iCon->SetPenColor(KRgbRed);
    iCon->DrawTextVertical(_L("Testing"), box1, myFont->AscentInPixels() , EFalse, CGraphicsContext::ELeft);
	// Lower right corner of the box
    iCon->SetPenColor(KRgbBlue);
    iCon->DrawTextVertical(_L("Testing"), box1, myFont->AscentInPixels() , EFalse, CGraphicsContext::ERight);
	// Upper left corner of the box
    iCon->SetPenColor(KRgbGreen);
    iCon->DrawTextVertical(_L("Testing"), box1, myFont->AscentInPixels() , ETrue, CGraphicsContext::ERight);
	// Lower right corner of the box
    iCon->SetPenColor(KRgbBlack);
    iCon->DrawTextVertical(_L("Testing"), box1, myFont->AscentInPixels() , ETrue, CGraphicsContext::ELeft);

	/* Text drawn when baseline = 0.
	   Expected result: The text will be hidden in this case as it is drawn outside 
	   the borders of the box. Only the parts in the font descent will be shown. 
	*/
    iCon->SetPenColor(KRgbRed);
    iCon->DrawTextVertical(_L("Testing"), box2, 0, EFalse, CGraphicsContext::ELeft);
    iCon->SetPenColor(KRgbBlue);
    iCon->DrawTextVertical(_L("Testing"), box2, 0, EFalse, CGraphicsContext::ERight);
    iCon->SetPenColor(KRgbGreen);
    iCon->DrawTextVertical(_L("Testing"), box2, 0, ETrue, CGraphicsContext::ERight);
    iCon->SetPenColor(KRgbBlack);
    iCon->DrawTextVertical(_L("Testing"), box2, 0, ETrue, CGraphicsContext::ELeft);

	/* Text drawn when baseline = box width.
	   Expected results: Text will be drawn on the side of the opposite edge and its
	   descent part will be hidden.
	*/
    iCon->SetPenColor(KRgbRed);
    iCon->DrawTextVertical(_L("Testing"), box3, box3.Width(), EFalse, CGraphicsContext::ELeft);
    iCon->SetPenColor(KRgbBlue);
    iCon->DrawTextVertical(_L("Testing"), box3, box3.Width(), EFalse, CGraphicsContext::ERight);
    iCon->SetPenColor(KRgbGreen);
    iCon->DrawTextVertical(_L("Testing"), box3, box3.Width(), ETrue, CGraphicsContext::ERight);
    iCon->SetPenColor(KRgbBlack);
    iCon->DrawTextVertical(_L("Testing"), box3, box3.Width(), ETrue, CGraphicsContext::ELeft);

	/* Text drawn when baseline = box width + font ascent.
	   Expected result: text will not be shown at all as it lies outside the box totally.
	*/
    iCon->SetPenColor(KRgbRed);
    iCon->DrawTextVertical(_L("Testing"), box4, box4.Width() + myFont->AscentInPixels(), EFalse, CGraphicsContext::ELeft);
    iCon->SetPenColor(KRgbBlue);
    iCon->DrawTextVertical(_L("Testing"), box4, box4.Width() + myFont->AscentInPixels(), EFalse, CGraphicsContext::ERight);
    iCon->SetPenColor(KRgbGreen);
    iCon->DrawTextVertical(_L("Testing"), box4, box4.Width() + myFont->AscentInPixels(), ETrue, CGraphicsContext::ERight);
    iCon->SetPenColor(KRgbBlack);
    iCon->DrawTextVertical(_L("Testing"), box4, box4.Width() + myFont->AscentInPixels(), ETrue, CGraphicsContext::ELeft);

	/* Text drawn when baseline = font ascent while the width of teh box = font height
	   Expected result: text will be aligned in the center of the box
	*/
    iCon->SetPenColor(KRgbRed);
    iCon->DrawTextVertical(_L("Testing"), box5, myFont->AscentInPixels(), EFalse, CGraphicsContext::ELeft);
    iCon->SetPenColor(KRgbBlack);
    iCon->DrawTextVertical(_L("Testing"), box5, myFont->AscentInPixels(), EFalse, CGraphicsContext::ERight);
    iCon->SetPenColor(KRgbRed);
    iCon->DrawTextVertical(_L("Testing"), box6, myFont->AscentInPixels(), ETrue, CGraphicsContext::ELeft);
    iCon->SetPenColor(KRgbBlack);
    iCon->DrawTextVertical(_L("Testing"), box6, myFont->AscentInPixels(), ETrue, CGraphicsContext::ERight);

	iCon->DiscardFont();
	iDev->ReleaseFont(myFont);
	}
	
void TestGdi::DoPaintL()
	{
	*iSection = _L("Painting");
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iCon->DrawRect(iRect);
	CheckRectInsideL(iRect);
	CheckRectOutsideL(iRect);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iCon->DrawRect(iCentredRect);
	CheckRectInsideL(iCentredRect);
	CheckRectOutsideL(iCentredRect);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	for(TInt count=0;count<8;count++)
		iCon->DrawRect(iOffScreenRects[count]);
	CheckClearL();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iCon->DrawEllipse(iRect);
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iCon->DrawRoundRect(iRect,TSize(20,10));
	CheckRectOutsideL(iRect);
	CheckQuadInsideL(iRect);
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iCon->DrawRect(iLargeRect);
	CheckRectInsideL(TRect(TPoint(0,0),iSize));
	CheckQuadInsideL(TRect(TPoint(0,0),iSize));
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iCon->DrawEllipse(iLargeRect);
	CheckQuadInsideL(TRect(TPoint(0,0),iSize));
	Clear();
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iCon->DrawRoundRect(iLargeRect,TSize(20,10));
	CheckQuadInsideL(TRect(TPoint(0,0),iSize));
	Clear();
	}

void TestGdi::DoCopyL()
	{
	*iSection = _L("Copying");
	Clear();
	TInt a=iSize.iWidth;
	TInt b=iSize.iHeight;
	TInt c=a>>1;
	TInt d=b>>1;
	iCon->CopyRect(TPoint(-100,-100),TRect(-100,-100,-50,-50));
	iCon->CopyRect(TPoint(100,100),TRect(-100,-100,-50,-50));
	iCon->CopyRect(TPoint(100,100),TRect(-50,-50,50,50));
	iCon->CopyRect(TPoint(-100,-100),TRect(-50,-50,50,50));
	iCon->CopyRect(TPoint(-50,-50),TRect(0,0,50,50));
	iCon->CopyRect(TPoint(1000,1000),TRect(0,0,50,50));
	Clear();

	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	TRect center(c-10,d-10,c+10,d+10);
	iCon->DrawRect(center);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);

	iCon->CopyRect(TPoint(0,-25),center);
	iCon->CopyRect(TPoint(25,-25),center);
	iCon->CopyRect(TPoint(25,0),center);
	iCon->CopyRect(TPoint(25,25),center);
	iCon->CopyRect(TPoint(0,25),center);
	iCon->CopyRect(TPoint(-25,25),center);
	iCon->CopyRect(TPoint(-25,0),center);
	iCon->CopyRect(TPoint(-25,-25),center);

	for(TInt y=0;y<20;y++)
		for(TInt x=0;x<20;x++)
			{
			TRgb pelcol;
			iDev->GetPixel(pelcol,TPoint(c-10+x,d-10+y)); // centre
			TRgb samplecol;
			iDev->GetPixel(samplecol,TPoint(c-10+x,d-35+y)); // top
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c+15+x,d-35+y)); // top right
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c+15+x,d-10+y)); // right
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c+15+x,d+15+y)); // bottom right
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c-10+x,d+15+y)); // bottom
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c-35+x,d+15+y)); // bottom left
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c-35+x,d-10+y)); // left
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c-35+x,d-35+y)); // top left
			CheckL(samplecol==pelcol);
			}
	Clear();
	}

void TestGdi::DoBltL()
	{
	*iSection = _L("BitBlt");
	TRect sample(0,0,8,8);
	Clear();
	TInt a=iSize.iWidth;
	TInt b=iSize.iHeight;
	TInt c=a>>1;
	TInt d=b>>1;
	iCon->BitBlt(TPoint(20,20),&iBitmap2,sample);
	iCon->BitBlt(TPoint(c-20,20),&iBitmap2,sample);
	iCon->BitBlt(TPoint(a-20,20),&iBitmap2,sample);
	iCon->BitBlt(TPoint(a-20,d-20),&iBitmap2,sample);
	iCon->BitBlt(TPoint(a-20,b-20),&iBitmap2,sample);
	iCon->BitBlt(TPoint(c-20,b-20),&iBitmap2,sample);
	iCon->BitBlt(TPoint(20,b-20),&iBitmap2,sample);
	iCon->BitBlt(TPoint(20,d-20),&iBitmap2,sample);
	iCon->SetBrushOrigin(TPoint(c-20,d-20));
	iCon->DrawRect(TRect(c-20,d-20,c+20,d+20));
	for(TInt y=0;y<8;y++)
		for(TInt x=0;x<8;x++)
			{
			TRgb pelcol,samplecol;
			iDev->GetPixel(pelcol,TPoint(c-20+x,d-20+y));
			iDev->GetPixel(samplecol,TPoint(20+x,20+y));
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c-20+x,20+y));
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(a-20+x,20+y));
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(a-20+x,d-20+y));
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(a-20+x,b-20+y));
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(c-20+x,b-20+y));
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(20+x,b-20+y));
			CheckL(samplecol==pelcol);
			iDev->GetPixel(samplecol,TPoint(20+x,d-20+y));
			CheckL(samplecol==pelcol);
			}
	Clear();
	for(TInt count=0;count<8;count++)
		iCon->BitBlt(iOffScreenRects[count].iTl,&iBitmap2,sample);
	CheckClearL();
	}

void TestGdi::DoBltMaskedL()
	{
	*iSection = _L("BitBltMasked");
	TSize bmpsize(iBitmap.SizeInPixels());
	TRect bmprect(bmpsize);
	TInt x,y;
	TRgb bmpcol,samplecol;

	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iCon->SetBrushColor(KRgbBlack);
	iCon->Clear();
	iCon->BitBltMasked(TPoint(0,0),&iBitmap,bmprect,&iBitmap,ETrue);
	for(y=0;y<bmprect.iBr.iY;y++)
		{
		for(x=0;x<bmprect.iBr.iX;x++)
			{
			iBitmap.GetPixel(bmpcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(x,y));
			if(bmpcol.Gray2())
				CheckL(samplecol.Gray2()==0);
			else
				CheckL(samplecol.Gray2()==bmpcol.Gray2());
			}
		}
	iCon->Clear();
	iCon->BitBltMasked(TPoint(0,0),&iBitmap,bmprect,&iBitmap,EFalse);
	for(y=0;y<bmprect.iBr.iY;y++)
		{
		for(x=0;x<bmprect.iBr.iX;x++)
			{
			iBitmap.GetPixel(bmpcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(x,y));
			if(bmpcol.Gray2())
				CheckL(samplecol.Gray2()==bmpcol.Gray2());
			else
				CheckL(samplecol.Gray2()==0);
			}
		}
	iCon->SetBrushColor(KRgbWhite);
	iCon->Clear();
	iCon->BitBltMasked(TPoint(0,0),&iBitmap,bmprect,&iBitmap,ETrue);
	for(y=0;y<bmprect.iBr.iY;y++)
		{
		for(x=0;x<bmprect.iBr.iX;x++)
			{
			iBitmap.GetPixel(bmpcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(x,y));
			if(bmpcol.Gray2())
				CheckL(samplecol.Gray2()==1);
			else
				CheckL(samplecol.Gray2()==bmpcol.Gray2());
			}
		}
	iCon->Clear();
	iCon->BitBltMasked(TPoint(0,0),&iBitmap,bmprect,&iBitmap,EFalse);
	for(y=0;y<bmprect.iBr.iY;y++)
		{
		for(x=0;x<bmprect.iBr.iX;x++)
			{
			iBitmap.GetPixel(bmpcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(x,y));
			if(bmpcol.Gray2())
				CheckL(samplecol.Gray2()==bmpcol.Gray2());
			else
				CheckL(samplecol.Gray2()==1);
			}
		}
	Clear();
	CheckClearL();
	TPoint offscreenpoint(0,iSize.iHeight<<1);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	offscreenpoint.iX-=(bmpsize.iWidth<<1);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	offscreenpoint.iX=(iSize.iWidth<<1);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	offscreenpoint.SetXY(0,-(bmpsize.iHeight<<1));
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	offscreenpoint.iX-=(bmpsize.iWidth<<1);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	offscreenpoint.iX=(iSize.iWidth<<1);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	offscreenpoint.SetXY(0,0);
	offscreenpoint.iX-=(bmpsize.iWidth<<1);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	offscreenpoint.iX=(iSize.iWidth<<1);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,EFalse);
	iCon->BitBltMasked(offscreenpoint,&iBitmap,bmprect,&iBitmap,ETrue);
	CheckClearL();
	}

void TestGdi::DoBltCompressedL()
	{
	//NOTE: max assumed screen size is 100x100 the same as EMbmTgdiDrawbitmap
	
	*iSection = _L("DoBltCompressedL");
	CFbsBitmap bitmap;
	TInt ret = bitmap.Load(_L("z:\\system\\data\\tgdi.mbm"),EMbmTgdiDrawbitmap,EFalse);
	CFbsBitmap compressedBitmap;
	ret = compressedBitmap.LoadAndCompress(_L("z:\\system\\data\\tgdi.mbm"),EMbmTgdiDrawbitmap,EFalse);
	User::LeaveIfError(ret);
	
	//bitblt
	*iSection = _L("DoBltCompressedL - bitblt first part of lines");
	Clear();
	TInt startx=20;
	TInt starty=20;
	TInt width=iSize.iWidth;
	TInt height=iSize.iHeight;
	TInt repeatx=width>>1;
	TInt repeaty=height>>1;
	TRect sample1(startx,0,repeatx-2*startx,repeaty);
	TRgb pelcol,samplecol;
	iCon->BitBlt(TPoint(startx,starty),&bitmap,sample1);
	iCon->BitBlt(TPoint(repeatx-startx,starty),&compressedBitmap,sample1);
	for(TInt y=0;y<repeaty;y++)
		{
		for(TInt x=startx;x<repeatx-2*startx;x++)
			{
			iDev->GetPixel(pelcol,TPoint(repeatx-startx+x,starty+y));
			iDev->GetPixel(samplecol,TPoint(startx+x,starty+y));
			CheckL(samplecol==pelcol);
			}
		}
	*iSection = _L("DoBltCompressedL - bitblt multiple complete lines - columns");
	Clear();
	TRect sample2(0,0,bitmap.SizeInPixels().iWidth,50);
	iCon->BitBlt(TPoint(0,0),&bitmap,sample2);
	iCon->BitBlt(TPoint(0,50),&compressedBitmap,sample2);
	for(TInt y=0;y<50;y++)
		{
		for(TInt x=0;x<bitmap.SizeInPixels().iWidth;x++)
			{
			iDev->GetPixel(pelcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(x,50+y));
			CheckL(samplecol==pelcol);
			}
		}
	
	*iSection = _L("DoBltCompressedL - bitblt final part of lines");
	Clear();
	TRect sample3(0,0,50,100);
	iCon->BitBlt(TPoint(0,0),&bitmap,sample3);
	iCon->BitBlt(TPoint(50,0),&compressedBitmap,sample3);
	for(TInt y=0;y<100;y++)
		{
		for(TInt x=0;x<50;x++)
			{
			iDev->GetPixel(pelcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(50+x,y));
			CheckL(samplecol==pelcol);
			}
		}

	*iSection = _L("DoBltCompressedL - bitbltmasked flickerv2");
	Clear();
	TRect sample4(0,0,50,100);
	// Due to inconsistent handling of solid brushes between different colour modes, the
	// screen or a bitmap, accelerated or non-accelerated do this test with a null brush.
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->BitBltMasked(TPoint(0,0),&bitmap,sample4,&iBitmap,ETrue);
	iCon->BitBltMasked(TPoint(50,0),&compressedBitmap,sample4,&iBitmap,ETrue);
	for(TInt y=0;y<100;y++)
		{
		for(TInt x=0;x<50;x++)
			{
			iDev->GetPixel(pelcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(50+x,y));
			CheckL(samplecol==pelcol);
			}
		}

	*iSection = _L("DoBltCompressedL - bitbltmasked needbitmaps of same mode");
	Clear();
	TRect sample5(0,0,50,100);
	iCon->BitBltMasked(TPoint(0,0),&bitmap,sample5,&compressedBitmap,ETrue);
	iCon->BitBltMasked(TPoint(50,0),&compressedBitmap,sample5,&bitmap,ETrue);
	for(TInt y=0;y<100;y++)
		{
		for(TInt x=0;x<50;x++)
			{
			iDev->GetPixel(pelcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(50+x,y));
			CheckL(samplecol==pelcol);
			}
		}

	*iSection = _L("DoBltCompressedL - bitbltmasked same bitmap as mask");
	Clear();
	TRect target6(0,0,50,100);
	TRect sample6(0,0,50,100);
	iCon->DrawBitmapMasked(target6,&bitmap,sample6,&bitmap,ETrue);
	target6.Move(50,0);
	iCon->DrawBitmapMasked(target6,&compressedBitmap,sample6,&compressedBitmap,ETrue);
	for(TInt y=0;y<100;y++)
		{
		for(TInt x=0;x<50;x++)
			{
			iDev->GetPixel(pelcol,TPoint(x,y));
			iDev->GetPixel(samplecol,TPoint(50+x,y));
			CheckL(samplecol==pelcol);
			}
		}
	}

void TestGdi::DoShadowL()
	{
	*iSection = _L("Shadow");
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TInt halfwidth=iSize.iWidth>>1;
	TRect lhs(TSize(halfwidth,iSize.iHeight));
	TRect rhs(TPoint(halfwidth,0),TSize(iSize.iWidth,iSize.iHeight));
	TRegionFix<1> area(lhs);
	TPoint lhspt(halfwidth>>1,iSize.iHeight>>1);
	TPoint rhspt(halfwidth+(halfwidth>>1),iSize.iHeight>>1);
	TRgb lhsrgb,rhsrgb;

	for (TInt count = 0; count < 32; count++)
		{
		TRgb brushColor;
		if (count < 16)
			brushColor = TRgb::Gray16(count);
		else
			brushColor = TRgb::Color16(count - 16);

		iCon->DrawRect(lhs);
		iCon->ShadowArea(&area);
		iCon->SetShadowMode(ETrue);
		iCon->DrawRect(rhs);
		iDev->GetPixel(lhsrgb,lhspt);
		iDev->GetPixel(rhsrgb,rhspt);
		CheckL(lhsrgb==rhsrgb);
		iCon->SetShadowMode(EFalse);
		}
	Clear();
	}

/**
 Auxiliary Fn to check the Pixel values 

 This method checks the pixel values with respective calculated values based on the call 
 from DoDrawBitmapMaskedL

*/
void TestGdi::CheckMaskedResultsL(CFbsBitmap* aBgBmp,CFbsBitmap* aSrcBmp,CFbsBitmap* aMaskBmp,TRect& aTarRect,TBool aInvertMask,TBool aAlphaBlend, TBool aSemiTransparentMask/*=EFalse*/)
	{
	TInt width=0;
	TInt height=0;
	TInt rowIndex=0;
	TInt colIndex=0;
	width=aTarRect.Width();
	height=aTarRect.Height();
	// Check the size of the target rectangle is negative or zero
	if (width<0 || !width || height<0 || !height)
		{
		return;
		}
	// Check the size of the target rectangle falls out side the display screen size
	if (width>iSize.iWidth)
		{
		width=iSize.iWidth;
		}
	if (height>iSize.iHeight)
		{
		height=iSize.iHeight;
		}
	// Rectangle used locally and it starts top left corner.(0,0)
	TRect rect(0,0,width,height);
	TSize size(rect.Size());
	TDisplayMode devDisplayMode=iDev->DisplayMode();
	//Create background bitmap with the size equal to target rectangle
	CFbsBitmap* backgroundBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(backgroundBmp);
	User::LeaveIfError(backgroundBmp->Create(size,devDisplayMode));
	//Create src bitmap with the size equal to target rectangle
	CFbsBitmap* srcBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(srcBmp);
	User::LeaveIfError(srcBmp->Create(size,iDev->DisplayMode()));
	//Create mask bitmap with the size equal to target rectangle
	CFbsBitmap* maskBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBmp);
	TDisplayMode maskDisplayMode;
	if (aSrcBmp==aMaskBmp) // When the source and target are same
		{
		maskDisplayMode=devDisplayMode;
		}
	else if (aAlphaBlend) // display mode based on the aAlphablend flag
		{
		maskDisplayMode=EGray256;
		}
	else
		{
		maskDisplayMode=aMaskBmp->DisplayMode();
 		}
	User::LeaveIfError(maskBmp->Create(size,maskDisplayMode));
	//Creating bitmap device for calculation and comparison
	CFbsBitmapDevice *bgDevice=CFbsBitmapDevice::NewL(backgroundBmp);
	CleanupStack::PushL(bgDevice);
	CFbsBitGc* bgGc=NULL;
	User::LeaveIfError(bgDevice->CreateContext(bgGc));
	CleanupStack::PushL(bgGc);
	if (width==iBitmapWidth && height==iBitmapHeight || aTarRect.Width()<=iSize.iWidth && aTarRect.Height()<=iSize.iHeight)
		{
		bgGc->DrawBitmap(rect,aBgBmp);
		}
	else
		{
		bgGc->DrawBitmap(aTarRect,aBgBmp);
		}
	CFbsBitmapDevice* maskDevice=CFbsBitmapDevice::NewL(maskBmp);	//For masked bitmap
	CleanupStack::PushL(maskDevice);
	CFbsBitGc* maskGc=NULL;
	User::LeaveIfError(maskDevice->CreateContext(maskGc));
	CleanupStack::PushL(maskGc);
	if (width==iBitmapWidth && height==iBitmapHeight || aTarRect.Width()<=iSize.iWidth && aTarRect.Height()<=iSize.iHeight)
		{
		maskGc->DrawBitmap(rect,aMaskBmp);
		}
	else
		{
		maskGc->DrawBitmap(aTarRect,aMaskBmp);
		}
	CFbsBitmapDevice* srcDevice=CFbsBitmapDevice::NewL(srcBmp);	//For source bitmap
	CleanupStack::PushL(srcDevice);
	CFbsBitGc* srcGc=NULL;
	User::LeaveIfError(srcDevice->CreateContext(srcGc));
	CleanupStack::PushL(srcGc);
	if (width==iBitmapWidth && height==iBitmapHeight || aTarRect.Width()<=iSize.iWidth && aTarRect.Height()<=iSize.iHeight)
		{
		srcGc->DrawBitmap(rect,aSrcBmp);
		}
	else
		{
		srcGc->DrawBitmap(aTarRect,aSrcBmp);
		}

	for (rowIndex=0;rowIndex<height;++rowIndex)
		{
		TInt redinversemask=0;
		TInt redmaskvalue=0;
		TInt greeninversemask=0;
		TInt greenmaskvalue=0;
		TInt blueinversemask=0;
		TInt bluemaskvalue=0;
		TRgb bmpcol;
		TRgb samplecol;
		TRgb maskcol;
		for (colIndex=0;colIndex<width;++colIndex)
			{
			srcDevice->GetPixel(bmpcol,TPoint(colIndex,rowIndex));
			iDev->GetPixel(samplecol,aTarRect.iTl+TPoint(colIndex,rowIndex));
			TRgb backgroundcolor;
			bgDevice->GetPixel(backgroundcolor,TPoint(colIndex,rowIndex));
			maskDevice->GetPixel(maskcol,TPoint(colIndex,rowIndex));
			if (aInvertMask && (maskDisplayMode!=EGray256 || aAlphaBlend))
				{
				redinversemask=maskcol.Red(); // green and blue should have the same value for gray-scale bitmap
				redmaskvalue=MAXCOLORS-redinversemask;
				blueinversemask=maskcol.Blue(); 
				bluemaskvalue=MAXCOLORS-blueinversemask;
				greeninversemask=maskcol.Green(); 
				greenmaskvalue=MAXCOLORS-greeninversemask;
				}
			else
				{
				redmaskvalue=maskcol.Red();
				redinversemask=MAXCOLORS-redmaskvalue;
				bluemaskvalue=maskcol.Blue();
				blueinversemask=MAXCOLORS-bluemaskvalue;
				greenmaskvalue=maskcol.Green();
				greeninversemask=MAXCOLORS-greenmaskvalue;
				}
			TInt red;
			TInt green;
			TInt blue;
			if (aSemiTransparentMask && devDisplayMode!=EGray256)
				{
				if (devDisplayMode==EColor64K || devDisplayMode==EColor256 || devDisplayMode==EColor4K || devDisplayMode==EColor16M || aInvertMask)
					{
					red=(bmpcol.Red()*redmaskvalue+backgroundcolor.Red()*redinversemask)>>8;
					green=(bmpcol.Green()*greenmaskvalue+backgroundcolor.Green()*greeninversemask)>>8;
					blue=(bmpcol.Blue()*bluemaskvalue+backgroundcolor.Blue()*blueinversemask)>>8;
					}
				else
					{
					red=(bmpcol.Red()*(redmaskvalue+1))>>8;
					red+=(backgroundcolor.Red()*(redinversemask+1))>>8;
					green=(bmpcol.Green()*(greenmaskvalue+1))>>8;
					green+=(backgroundcolor.Green()*(greeninversemask+1))>>8;
					blue=(bmpcol.Blue()*(bluemaskvalue+1))>>8;
					blue+=(backgroundcolor.Blue()*(blueinversemask+1))>>8;
					}
				}
			else
				{
				red=((bmpcol.Red()*redmaskvalue)+(backgroundcolor.Red()*redinversemask));
				green=((bmpcol.Green()*greenmaskvalue)+(backgroundcolor.Green()*greeninversemask));
				blue=((bmpcol.Blue()*bluemaskvalue)+(backgroundcolor.Blue()*blueinversemask));
				red   = red / MAXCOLORS;
				green = green / MAXCOLORS;
				blue  = blue / MAXCOLORS;
				}

			TRgb rgb(red,green,blue);
			TRgb resultantColor;
			TUint index;
			switch(devDisplayMode)
				{
				case EGray2:
					{
					index = TRgb::Gray2(blue).Gray2();
					resultantColor = TRgb::Gray2(index);
					break;
					}
				case EGray256:
					{
					index = rgb.Gray256();
					resultantColor = TRgb::Gray256(index);
					break;
					}
				case EColor256:
					{
					index = rgb.Color256();
					resultantColor = TRgb::Color256(index);
					break;
					}
				case EColor4K:
					{
					index = rgb.Color4K();
					resultantColor = TRgb::Color4K(index);
					break;
					}
				case EColor64K:
					{
					index = rgb.Color64K();
					resultantColor = TRgb::Color64K(index);
					break;
					}
				case EColor16M:
					{
					index = rgb.Color16M();
					resultantColor = TRgb::Color16M(index);
					break;
					}
				case EColor16MU:
					{
					index = rgb.Color16MU();
					resultantColor = TRgb::Color16MU(index);
					break;
					}
				}
			CheckL(samplecol.Red()==resultantColor.Red()); 
			CheckL(samplecol.Green()==resultantColor.Green());
			CheckL(samplecol.Blue()==resultantColor.Blue());
			}
		}
	CleanupStack::PopAndDestroy(9,backgroundBmp);
	}

/**
 @SYMTestCaseID GRAPHICS-TestGdi-DoDrawBitmapMaskedL-0001
 
 @SYMPREQ 642
 
 @SYMREQ 
   
 @SYMTestCaseDesc  Test DrawMaskedBitmap (Transperency Stretched Bitmap).\n
 Automated Tests to test DrawBitmapMasked with different display modes.
 
 @SYMTestPriority High
  
 @SYMTestStatus Implemented
   
 @SYMTestActions Creates bitmaps programmatically and calls API DrawBitmapMasked() with respective parameters
 Drawn pixels value with calculated pixels and verified using CheckL(), All the test cases specified in \n
 test specifications are implemented(22 test cases).
   
 @SYMTestExpectedResults KErrNone 
   
 */
void TestGdi::DoDrawBitmapMaskedL(TBool aColorMask)
	{
	_LIT(KBitmapMasked,"DrawBitmapMasked - Test Case ");
	*iSection = KBitmapMasked;
	TInt rowIndex;
	TInt colIndex;
	TSize rectSize(iBitmapWidth,iBitmapHeight);
	TRect srcRect;
	TRect tarRect;
	
	CFbsBitmap* backgroundBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(backgroundBmp);
	User::LeaveIfError(backgroundBmp->Create(rectSize,iDev->DisplayMode()));
	srcRect.SetSize(rectSize);
	TBitmapUtil bmpUtil(backgroundBmp);
	bmpUtil.Begin(TPoint(0,0));
	for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
		{
		TInt red=0;
		TInt blue=255;
		if(iDev->DisplayMode()==EGray2)
			{	
			blue=0;
			}	
		TInt green=0;
		bmpUtil.SetPos(TPoint(0,rowIndex));
		for (colIndex=0;colIndex<iBitmapWidth;++colIndex)
			{
			TRgb rgb(red,green,blue);
			switch(iDev->DisplayMode())
				{
				case EGray2:
					{
					bmpUtil.SetPixel(TRgb::Gray2(blue).Gray2());
					bmpUtil.IncXPos();
					if (colIndex%4==0)
						{
						blue=255-blue;
						}
					continue;
					}
				case EGray256:
					{
					bmpUtil.SetPixel(rgb.Gray256());
					break;
					}
				case EColor256:
					{
					bmpUtil.SetPixel(rgb.Color256());
					break;
					}
				case EColor4K:
					{
					bmpUtil.SetPixel(rgb.Color4K());
					break;
					}
				case EColor64K:
					{
					bmpUtil.SetPixel(rgb.Color64K());
					break;
					}
				case EColor16M:
					{
					bmpUtil.SetPixel(rgb.Color16M());
					break;
					}
				case EColor16MU:
					{
					bmpUtil.SetPixel(rgb.Color16MU());
					break;
					}
				}
			bmpUtil.IncXPos();
			red+=10;
			blue-=30;
			green+=20;
			if(red > 255)
				{
				red-=255;
				green+=1;
				}
			if(green > 255)
				{
				green-=255;
				blue-=1;
				}
			if(blue < 0)
				{
				blue+=255;
				}
			}
		}
	bmpUtil.End();
	//Create source bitmap with various color scale
	CFbsBitmap* srcBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(srcBmp);
	User::LeaveIfError(srcBmp->Create(rectSize,iDev->DisplayMode()));
	TBitmapUtil bmpUtil1(srcBmp);
	bmpUtil1.Begin(TPoint(0,0));
	TInt gray2=0;
	for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
		{
		TInt red=255;
		TInt blue=0;
		TInt green=255;
		bmpUtil1.SetPos(TPoint(0,rowIndex));
		gray2=!gray2?0:255;
		for (colIndex=0;colIndex<iBitmapWidth;++colIndex)
			{
			TRgb rgb(red,green,blue);
			switch(iDev->DisplayMode())
				{
				case EGray2:
					{
					bmpUtil1.SetPixel(TRgb::Gray2(gray2).Gray2());
					bmpUtil1.IncXPos(); 				
					red=0;
					green=0;
					blue=!blue?255:0;
					continue;
					}
				case EGray256:
					{
					bmpUtil1.SetPixel(rgb.Gray256());
					break;
					}
				case EColor256:
					{
					bmpUtil1.SetPixel(rgb.Color256());
					break;
					}
				case EColor4K:
					{
					bmpUtil1.SetPixel(rgb.Color4K());
					break;
					}
				case EColor64K:
					{
					bmpUtil1.SetPixel(rgb.Color64K());
					break;
					}
				case EColor16M:
					{
					bmpUtil1.SetPixel(rgb.Color16M());
					break;
					}
				case EColor16MU:
					{
					bmpUtil1.SetPixel(rgb.Color16MU());
					break;
					}
				}
			bmpUtil1.IncXPos();
			red-=10;
			blue+=10;
			green-=10;
			if(red <= 0)
				{
				red+=255;
				}
			if(blue >= 255)
				{
				blue-=255;
				}
			if(green <= 0)
				{
				green+=255;
				}
			}
		}
	bmpUtil1.End();
	//Masked bitmap with various color scale
	CFbsBitmap* maskBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBmp);
	TBitmapUtil bmpUtil2(maskBmp);
	if(iDev->DisplayMode() == EGray2)
		{
		User::LeaveIfError(maskBmp->Create(rectSize,EGray2));
		bmpUtil2.Begin(TPoint(0,0));
		for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
			{
			bmpUtil2.SetPos(TPoint(0,rowIndex));
			TInt blue=0;
			for(colIndex=0;colIndex<iBitmapWidth;++colIndex)
				{
				bmpUtil2.SetPixel(blue);
				bmpUtil2.IncXPos();
				blue=!blue?255:0;
				}
			}
		bmpUtil2.End();
		}
	else
		{
		User::LeaveIfError(maskBmp->Create(rectSize,aColorMask?EColor256:EGray256));
		bmpUtil2.Begin(TPoint(0,0));
		for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
			{
			bmpUtil2.SetPos(TPoint(0,rowIndex));
			TInt red=0;
			TInt blue=0;
			TInt green=0;
			for(colIndex=0;colIndex<iBitmapWidth;++colIndex)
				{
				TRgb rgb(red,green,blue);
				bmpUtil2.SetPixel(rgb.Color256());
				bmpUtil2.IncXPos();
				red=!red?255:0;
				blue=!blue?255:0;
				green=!green?255:0;
				}
			}
		bmpUtil2.End();
		}
	
	//Masked bitmap with various gray scale (For Alphablending)
	CFbsBitmap* maskblendBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskblendBmp);
	if(iDev->DisplayMode() == EGray2)
		{
		User::LeaveIfError(maskblendBmp->Create(rectSize,EGray2));
		TBitmapUtil bmpUtil3(maskblendBmp);
		bmpUtil3.Begin(TPoint(0,0));
		TInt blue=0;
		for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
			{
			bmpUtil3.SetPos(TPoint(0,rowIndex));
			blue=0;
			for(colIndex=0;colIndex<iBitmapWidth;++colIndex)
				{
				bmpUtil3.SetPixel(blue);
				bmpUtil3.IncXPos();
				blue=!blue?255:0;
				}
			}
		bmpUtil3.End();	
		}
	else
		{
		User::LeaveIfError(maskblendBmp->Create(rectSize,EGray256));
		TBitmapUtil bmpUtil3(maskblendBmp);
		bmpUtil3.Begin(TPoint(0,0));
		for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
			{
			bmpUtil3.SetPos(TPoint(0,rowIndex));
			TInt red=0;
			TInt blue=0;
			TInt green=0;
			for(colIndex=0;colIndex<iBitmapWidth;++colIndex)
				{
				TRgb rgb(red,green,blue);
				bmpUtil3.SetPixel(rgb.Gray256());
				bmpUtil3.IncXPos();
				red=!red?255:0;
				blue=!blue?255:0;
				green=!green?255:0;
				}
			}
		bmpUtil3.End();
		}	
	/**
	  Test Cases 1 to 10 	 (AlphaBlending)
	 
	  Test Case # 1

	  Desc	  : Test the functionality of DrawBitmapMasked() without 
	  Stretching or compressing and with a normal mask.
	 */
	iSection->AppendNum(1);
	iCon->DrawBitmap(srcRect,backgroundBmp,srcRect);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(srcRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,srcRect,EFalse,ETrue);
	/**
	  Test Case # 2

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  when it stretches the bitmap and with a normal mask.
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(2);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth*2);
	tarRect.SetHeight(iBitmapHeight*2);
	if (iDev->SizeInPixels().iWidth>=tarRect.Width() && iDev->SizeInPixels().iHeight>=tarRect.Height())
		{
		iCon->DrawBitmap(tarRect,backgroundBmp);
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);	
		CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);
		}
	/**
	  Test Case # 3

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  when it compresses the bitmap and with a normal mask.
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(3);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth/2);
	tarRect.SetHeight(iBitmapHeight/2);
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);
	/** 
	  Test Case # 4
	 
	  Desc	  : To test the functionality of DrawBitmapMasked() with  Destination Rectangle as zero.
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(4);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(0);
	tarRect.SetHeight(0);
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);
	/**
	  Test Case # 5

	  Desc	  : To test the functionality of DrawBitmapMasked() 
	  with  destination rectangle large in size. (more than test screen size)
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(5);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth*5); 
	tarRect.SetHeight(iBitmapHeight*4); 
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);
	/**
	  Test Case # 6
	  
	  Desc	  : To test the functionality of DrawBitmapMasked()
	  passing the target rectangle targeted to right top corner of the screen.
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(6);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth);
	tarRect.SetHeight(iBitmapHeight);
	tarRect.Move(iSize.iWidth-iBitmapWidth,0);
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);	
	/**
	  Test Case # 7

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  passing the target rectangle targeted to bottom left corner of the screen.
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(7);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0); // Restoring rectangle back to the original position
	tarRect.SetWidth(iBitmapWidth);
	tarRect.SetHeight(iBitmapHeight);
	tarRect.Move(0,(iSize.iHeight-iBitmapHeight));
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);
	/**
	  Test Case # 8

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  passing the target rectangle targeted to bottom right corner of the screen
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(8);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0); // Restoring rectangle back to the original position
	tarRect.SetWidth(iBitmapWidth);
	tarRect.SetHeight(iBitmapHeight);
	tarRect.Move((iSize.iWidth-iBitmapWidth),(iSize.iHeight-iBitmapHeight));
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);
	/**
	  Test Case # 9

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  passing the target rectangle has negative coordinates
	 */
	iSection->Delete(iSection->Length() - 1, 1);
	iSection->AppendNum(9);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0); // Restoring rectangle back to the original position
	tarRect.SetWidth(-iBitmapWidth);
	tarRect.SetHeight(-iBitmapHeight);
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue);
	/**
	  Test Case # 10

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  passing the same bitmap for both the source and mask
	 */
	if (iDev->DisplayMode()==EGray2 || iDev->DisplayMode()==EGray4 || iDev->DisplayMode()==EGray16)
		{
		iSection->Delete(iSection->Length() - 1, 1);
		iSection->AppendNum(10);
		iCon->Clear();
		tarRect.iTl.SetXY(0,0); // Restoring rectangle back to the original position
		tarRect.SetWidth(iBitmapWidth);
		tarRect.SetHeight(iBitmapHeight);
		iCon->DrawBitmap(tarRect,backgroundBmp);
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,srcBmp,EFalse);
		CheckMaskedResultsL(backgroundBmp,srcBmp,srcBmp,tarRect,EFalse,EFalse);
		}
	/**
	  Test Cases: 11 - 22 (BitBltMasked)

	  Test Case # 11

	  Desc	  : Test the functionality of DrawBitmapMasked() without 
	  Stretching or compressing and with a normal mask.
	 */
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(11);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth);
	tarRect.SetHeight(iBitmapHeight);
	iCon->DrawBitmap(srcRect,backgroundBmp,srcRect);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(srcRect,srcBmp,srcRect,maskBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,srcRect,EFalse,EFalse);
	//  Test Case # 12 with a reverse mask.
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(12);
	iCon->Clear();
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,ETrue);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,ETrue,EFalse);
	/**
	  Test Case # 13

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  when it stretches the bitmap and with a normal mask.
	 */
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(13);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(Min(iBitmapWidth*2,iSize.iWidth));
	tarRect.SetHeight(Min(iBitmapHeight*2,iSize.iHeight));
	if (iDev->SizeInPixels().iWidth>=tarRect.Width() && iDev->SizeInPixels().iHeight>=tarRect.Height())
		{
		iCon->DrawBitmap(tarRect,backgroundBmp);
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,EFalse);
		CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,EFalse,EFalse);
		}
	//  Test Case # 14 with a reverse mask.
	if (iDev->SizeInPixels().iWidth>=tarRect.Width() && iDev->SizeInPixels().iHeight>=tarRect.Height())
		{
		iSection->Delete(iSection->Length() - 2, 2);
		iSection->AppendNum(14);
		iCon->Clear();
		iCon->DrawBitmap(tarRect,backgroundBmp);
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,ETrue);
		CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,ETrue,EFalse);
		}
	/**
	  Test Case # 15

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  when it compresses the bitmap and with a normal mask.
	 */
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(15);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth/2);
	tarRect.SetHeight(iBitmapHeight/2);
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,EFalse);
    CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,EFalse,EFalse);
    //  Test Case # 16 with a reverse mask.
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(16);
	iCon->Clear();
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,ETrue);
  	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,ETrue,EFalse);
	/**
	  Test Case # 17

	  Desc	  : To test the functionality of DrawBitmapMasked() 
	  with  destination rectangle large in size. (more than test screen size)
	 */
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(17);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth*5);  
	tarRect.SetHeight(iBitmapHeight*4);
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,EFalse,EFalse);
	//  Test Case # 18  with a reverse mask.
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(18);	
	iCon->Clear();
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,ETrue);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,ETrue,EFalse);	
	/**
	  Test Case # 19

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  passing the target rectangle targeted to right top corner of the screen.
	 */
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(19);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth);
	tarRect.SetHeight(iBitmapHeight);
	tarRect.Move(iSize.iWidth-iBitmapWidth,0);
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,EFalse,EFalse);
	//  Test Case # 20  with a reverse mask.
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(20);
	iCon->Clear();
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,ETrue);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,ETrue,EFalse);
	/**
	  Test Case # 21

	  Desc	  : To test the functionality of DrawBitmapMasked()
	  passing the target rectangle targeted to bottom right corner of the screen
	 */
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(21);
	iCon->Clear();
	tarRect.iTl.SetXY(0,0); // Restoring rectangle back to the original position
	tarRect.SetWidth(iBitmapWidth);
	tarRect.SetHeight(iBitmapHeight);
	tarRect.Move((iSize.iWidth-iBitmapWidth),(iSize.iHeight-iBitmapHeight));
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,EFalse);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,EFalse,EFalse);
	//  Test Case # 22  with a reverse mask.
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(22);
	iCon->Clear();
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
	iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskBmp,ETrue);
	CheckMaskedResultsL(backgroundBmp,srcBmp,maskBmp,tarRect,ETrue,EFalse);
	
	/** 
	  Test Case # 23
	  
	  Desc: To ensure that DEF064071 is fixed.  Test that drawing a scaled
	  bitmap with a tiled mask gives the correct result.
	*/
	if(iSize.iWidth > 100 && iDev->DisplayMode() > EGray4)
	// Only do this test for screen devices
		{
		iSection->Delete(iSection->Length() - 2, 2);
		iSection->AppendNum(23);
		Clear();
		iCon->Clear();
		// We need a bigger rect than the rect in other tests
		TRect t23Rect(TSize(150,150));
		// Open source bitmap and mask
		CFbsBitmap* t23SrcBmp = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(t23SrcBmp);
		User::LeaveIfError(t23SrcBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_sourcebitmap));
		CFbsBitmap* t23SrcBmpMask = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(t23SrcBmpMask);
		User::LeaveIfError(t23SrcBmpMask->Load(KT23BitmapFileName,EMbmTgdiDef064071_sourcebitmapmask));
		
		// Open bitmap that contains the expected result
		CFbsBitmap* t23ExpectedBmp = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(t23ExpectedBmp);
		switch(iDev->DisplayMode())
			{
			case EGray16:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_egray16));
				break;
			case EGray256:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_egray256));
				break;
			case EColor16:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_ecolor16));
				break;
			case EColor256:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_ecolor256));
				break;
			case EColor4K:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_ecolor4k));
				break;
			case EColor64K:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_ecolor64k));
				break;
			case EColor16M:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_ecolor16m));
				break;
			case EColor16MU:
				User::LeaveIfError(t23ExpectedBmp->Load(KT23BitmapFileName,EMbmTgdiDef064071_expected_ecolor16mu));
				break;
			}

		// Draw masked bitmap to screen
		iCon->DrawBitmapMasked(t23Rect,t23SrcBmp,t23SrcBmp->SizeInPixels(),t23SrcBmpMask,ETrue);
		TRect miniTarRect(20,20,80,80);
		// Draw small version of the bitmap
		iCon->DrawBitmapMasked(miniTarRect,t23SrcBmp,t23SrcBmp->SizeInPixels(),t23SrcBmpMask,ETrue);
		// Set the clipping region to draw either side of the small bitmap that we've just drawn
		TRegionFix<2> t23Region;
		t23Region.AddRect(TRect(t23Rect.iTl.iX,miniTarRect.iTl.iY,
								miniTarRect.iTl.iX,miniTarRect.iBr.iY));
		t23Region.AddRect(TRect(miniTarRect.iBr.iX,miniTarRect.iTl.iY,
								t23Rect.iBr.iX,miniTarRect.iBr.iY));
		iCon->SetClippingRegion(&t23Region);
		iCon->Clear();
		iCon->DrawBitmapMasked(t23Rect,t23SrcBmp,t23SrcBmp->SizeInPixels(),t23SrcBmpMask,ETrue);
		iCon->SetClippingRect(iDev->SizeInPixels());
		
		// Compare pixels on device with those from the expected bitmap
		TPoint pixel;
		TRgb deviceRgb;
		TRgb expectedRgb;
		TSize t23RectSize(t23Rect.Size());
		CheckL(t23ExpectedBmp->SizeInPixels() == t23RectSize);
		for(pixel.iY = t23Rect.iTl.iY; pixel.iY < t23RectSize.iHeight; ++pixel.iY)
			{
			for(pixel.iX = t23Rect.iTl.iX; pixel.iX < t23RectSize.iWidth; ++pixel.iX)
				{
				iDev->GetPixel(deviceRgb,pixel);
				t23ExpectedBmp->GetPixel(expectedRgb,pixel);
				CheckL(deviceRgb == expectedRgb);
				}
			}
		iCon->CancelClippingRect();
		iCon->CancelClippingRegion();
		
		CleanupStack::PopAndDestroy(t23ExpectedBmp);
		CleanupStack::PopAndDestroy(t23SrcBmpMask);
		CleanupStack::PopAndDestroy(t23SrcBmp);
		}

	/**
	  Test Case # 24 (PDEF118404, INC115741)

	  Desc	  : To test the functionality of DrawBitmapMasked() 
	  with  destination rectangle multiple of 256. 
	  INC115741 issue with boundary conditions. CFbsBitGc::DoDrawBitmapMasked has KScanLineLength = 256.
	  Tests fix for PDEF118404. If fix is not present, this test will fail with User Panic 21. 
	 */
	iSection->Delete(iSection->Length() - 2, 2);
	iSection->AppendNum(24);	
	iCon->Clear();
	iCon->DrawBitmap(tarRect,backgroundBmp);
	iCon->SetBrushStyle(CGraphicsContext::ENullBrush);

	// This constant is associated to the value used in CFbsBitGc::DoDrawBitmapMasked.
	// If that value is changed, then this one must be updated as well otherwise the test will no longer be valid.	
	const TInt KScanLineLength = 256;

	tarRect.iTl.SetXY(0,0);
	tarRect.SetHeight(2);
	for (TInt ii = 1; ii < 5; ++ii)
		{
		tarRect.SetWidth(KScanLineLength * ii); 
		iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);
		}

	CleanupStack::PopAndDestroy(4,backgroundBmp);
	}

// Used to fill the bitmap with a given color for the current display mode.
void TestGdi::FillBitmap(CFbsBitmap* aBitmap, TRgb aRgb)
	{
	TBitmapUtil bmpUtil(aBitmap);
	bmpUtil.Begin(TPoint(0,0));
	for (TInt rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
		{
		bmpUtil.SetPos(TPoint(0,rowIndex));
		for (TInt colIndex=0;colIndex<iBitmapWidth;++colIndex)
			{
			switch(iDev->DisplayMode())
				{
				case EGray2:
					{
					bmpUtil.SetPixel(TRgb::Gray2(aRgb.Blue()).Gray2());
					break;
					}
				case EGray256:
					{
					bmpUtil.SetPixel(aRgb.Gray256());
					break;
					}
				case EColor256:
					{
					bmpUtil.SetPixel(aRgb.Color256());
					break;
					}
				case EColor4K:
					{
					bmpUtil.SetPixel(aRgb.Color4K());
					break;
					}
				case EColor64K:
					{
					bmpUtil.SetPixel(aRgb.Color64K());
					break;
					}
				case EColor16M:
					{
					bmpUtil.SetPixel(aRgb.Color16M());
					break;
					}
				case EColor16MU:
					{
					bmpUtil.SetPixel(aRgb.Color16MU());
					break;
					}
				}
			bmpUtil.IncXPos();
			}
		}
	bmpUtil.End();
	}

/**
This test function is added to reproduce the defect INC097853
(CFbsBitGc::DoDrawBitmapMasked function) in which it 
 was drawing the first scanline twice.
 The steps to reproduce the defect are:
 1) Create a Black Background bitmap
 2) Create a White Source Bitmap and
 3) Create a Gray Masked Bitmap (50% white).
 4) Call the function DrawBitmap to draw the Background bitmap
 5) Call the function DrawBitmapMasked to draw the Source bitmap with Masked Bitmap
 6) Store the screen into a bitmap
 */
void TestGdi::DoDrawBitmapSemiTransparentMaskedL()
	{
	Clear();
	iCon->Clear();
	_LIT(KBitmapMasked,"DrawBitmapSemiTransparentMasked");
	*iSection = KBitmapMasked;
	TInt rowIndex;
	TInt colIndex;
	TSize rectSize(iBitmapWidth,iBitmapHeight);
	TRect srcRect;
	TRect tarRect;

	// Create a Black background bitmap
	CFbsBitmap* backgroundBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(backgroundBmp);
	User::LeaveIfError(backgroundBmp->Create(rectSize,iDev->DisplayMode()));
	srcRect.SetSize(rectSize);
	FillBitmap(backgroundBmp, KRgbBlack);

	//Create a White source bitmap
	CFbsBitmap* srcBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(srcBmp);
	User::LeaveIfError(srcBmp->Create(rectSize,iDev->DisplayMode()));
	FillBitmap(srcBmp, KRgbWhite);

	//Create a semi transparent Masked bitmap 
	CFbsBitmap* maskblendBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskblendBmp);

	if(iDev->DisplayMode() == EGray2)
		{
		User::LeaveIfError(maskblendBmp->Create(rectSize,EGray2));
		TBitmapUtil bmpUtil(maskblendBmp);
		bmpUtil.Begin(TPoint(0,0));
		for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
			{
			bmpUtil.SetPos(TPoint(0,rowIndex));
			for(colIndex=0;colIndex<iBitmapWidth;++colIndex)
				{
				bmpUtil.SetPixel(128);
				bmpUtil.IncXPos();
				}
			}
		bmpUtil.End();
		}
	else 
		{
		User::LeaveIfError(maskblendBmp->Create(rectSize,EGray256));
		TRgb rgb(128,128,128);
		TBitmapUtil bmpUtil(maskblendBmp);
		bmpUtil.Begin(TPoint(0,0));
		for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
			{
			bmpUtil.SetPos(TPoint(0,rowIndex));
			for(colIndex=0;colIndex<iBitmapWidth;++colIndex)
				{
				bmpUtil.SetPixel(rgb.Gray256());
				bmpUtil.IncXPos();
				}
			}
		bmpUtil.End();
		}

	tarRect.iTl.SetXY(0,0);
	tarRect.SetWidth(iBitmapWidth*2);
	tarRect.SetHeight(iBitmapHeight*2);
	if (iDev->SizeInPixels().iWidth>=tarRect.Width() && iDev->SizeInPixels().iHeight>=tarRect.Height())
		{
		iCon->DrawBitmap(tarRect,backgroundBmp);
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->DrawBitmapMasked(tarRect,srcBmp,srcRect,maskblendBmp,EFalse);

		CheckMaskedResultsL(backgroundBmp,srcBmp,maskblendBmp,tarRect,EFalse,ETrue, ETrue);
		}

	CleanupStack::PopAndDestroy(3,backgroundBmp);
	}

void TestGdi::DoFadeL()
	{
	*iSection = _L("Fade");
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TInt halfwidth=iSize.iWidth>>1;
	TRect lhs(TSize(halfwidth,iSize.iHeight));
	TRect rhs(TPoint(halfwidth,0),TSize(iSize.iWidth,iSize.iHeight));
	TRegionFix<1> area(lhs);
	TPoint lhspt(halfwidth>>1,iSize.iHeight>>1);
	TPoint rhspt(halfwidth+(halfwidth>>1),iSize.iHeight>>1);
	TRgb lhsrgb,rhsrgb;

	for(TInt count=0;count<32;count++)
		{
		TRgb brushColor;
		if (count < 16)
			brushColor = TRgb::Gray16(count);
		else
			brushColor = TRgb::Color16(count - 16);

		if (iDev->DisplayMode() == EGray4)
			{
			brushColor = TRgb::Gray4(brushColor.Gray4());
			iCon->SetFadingParameters(255,0);
			}

		iCon->SetBrushColor(brushColor);
		iCon->DrawRect(lhs);
		iCon->FadeArea(&area);
		iCon->SetFadeMode(ETrue);
		iCon->DrawRect(rhs);
		iDev->GetPixel(lhsrgb,lhspt);
		iDev->GetPixel(rhsrgb,rhspt);
		CheckL(lhsrgb==rhsrgb);
		iCon->SetFadeMode(EFalse);
		}
	Clear();
	}

