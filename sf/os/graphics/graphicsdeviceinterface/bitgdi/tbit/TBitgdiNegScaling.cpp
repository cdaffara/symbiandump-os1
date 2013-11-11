// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TBitgdiScaling.h"
//#pragma warning(disable: 4127)

//X and Y scaling factors.
const TInt KScalingFactorX = 2;
const TInt KScalingFactorY = 3;
//Test bitmap
_LIT(KTestBmp, "z:\\system\\data\\BmCTest.mbm");
typedef CArrayFixFlat<TPoint> CPointArray;


//Choises one of the installed fonts and sets it as a default font for the graphics context.
static CFont* SetFontL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	TTypefaceSupport typeFaceSupport;
	aScreenDevice->TypefaceSupport(typeFaceSupport, 0);
	TFontSpec fontSpec;
	fontSpec.iTypeface = typeFaceSupport.iTypeface;
	fontSpec.iHeight = 10;
	CFont* font = NULL;
	User::LeaveIfError(aScreenDevice->GetNearestFontToDesignHeightInPixels(font, fontSpec));
	aGraphicsContext->UseFont(font);
	return font;
	}

//Clears the screen and displays after that the scaling origin point.
static void Clear(CFbsBitGc* aGraphicsContext)
	{
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(KRgbWhite);
	aGraphicsContext->Clear();
	aGraphicsContext->SetPenSize(TSize(2, 2));
	aGraphicsContext->SetPenColor(TRgb(0xFF, 0x00, 0x00));
	aGraphicsContext->Plot(TPoint(0, 0));
	aGraphicsContext->SetPenColor(TRgb(0x00, 0x00, 0x00));
	aGraphicsContext->DrawText(_L("Scaling Origin"), TPoint(0, 0));
	}

//CFbsBitGc::DrawBitmap() test
void CTBitgdiScaling::TestDrawBitmapL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawBitmap() - (0, 0) coord"));
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);

	bitmap->Reset();
	User::LeaveIfError(bitmap->Load(KTestBmp, 0));

	::Clear(aGraphicsContext);
	TSize size = bitmap->SizeInPixels();
	//RDebug::Print(_L("DrawBitmap %d, %d, %d, %d\r\n"), 0, 0, size.iWidth, size.iHeight);
	aGraphicsContext->DrawBitmap(TRect(0, 0, size.iWidth, size.iHeight), bitmap);
	aScreenDevice->Update();

	CleanupStack::PopAndDestroy(bitmap);
	}

//CFbsBitGc::Clear(), CFbsBitGc::Clear(const TRect& aRect), CFbsBitGc::DrawLine() test
void CTBitgdiScaling::TestClear(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("Clear(), DrawLine() - Neg.coord."));
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	//RDebug::Print(_L("DrawRect %d, %d, %d, %d\r\n"), drawRect.iTl.iX, drawRect.iTl.iY, drawRect.iBr.iX, drawRect.iBr.iY);
	aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));
	aGraphicsContext->DrawRect(drawRect);
	aScreenDevice->Update();

	//RDebug::Print(_L("Clear\r\n"));
	aGraphicsContext->SetBrushColor(KRgbWhite);
	::Clear(aGraphicsContext);
	aScreenDevice->Update();

	//RDebug::Print(_L("DrawRect %d, %d, %d, %d\r\n"), drawRect.iTl.iX, drawRect.iTl.iY, drawRect.iBr.iX, drawRect.iBr.iY);
	aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));
	aGraphicsContext->DrawRect(drawRect);
	aScreenDevice->Update();

	//RDebug::Print(_L("Clear(TRect) %d, %d, %d, %d\r\n"), drawRect.iTl.iX, drawRect.iTl.iY, drawRect.iBr.iX, drawRect.iBr.iY);
	aGraphicsContext->SetBrushColor(KRgbWhite);
	drawRect.Shrink(1, 1);
	aGraphicsContext->Clear(drawRect);
	aScreenDevice->Update();

	drawRect.Grow(1, 1);
	CGraphicsContext::TPenStyle penStyle[] = 
		{
		CGraphicsContext::ENullPen, 
		CGraphicsContext::ESolidPen, 
		CGraphicsContext::EDottedPen, 
		CGraphicsContext::EDashedPen, 
		CGraphicsContext::EDotDashPen, 
		CGraphicsContext::EDotDotDashPen
		};
	struct TLineProps
		{
		TLineProps(const TPoint& aPt1, const TPoint& aPt2) : 
			iPt1(aPt1), 
			iPt2(aPt2) 
			{
			}
		TPoint iPt1;
		TPoint iPt2;
		};
	TLineProps lineProps[] = 
		{
		TLineProps(drawRect.iTl, drawRect.iBr - TPoint(1, 1)), 
		TLineProps(TPoint(drawRect.iTl.iX, drawRect.iBr.iY - 1), TPoint(drawRect.iBr.iX - 1, drawRect.iTl.iY))
		};
	for(TInt jj=0;jj<TInt(sizeof(lineProps)/sizeof(lineProps[0]));++jj)
		{
		for(TInt ii=0;ii<TInt(sizeof(penStyle)/sizeof(penStyle[0]));++ii)
			{
			aGraphicsContext->SetPenStyle(penStyle[ii]);

			//RDebug::Print(_L("DrawLine %d, %d, %d, %d\r\n"), lineProps[jj].iPt1.iX, lineProps[jj].iPt1.iY, lineProps[jj].iPt2.iX, lineProps[jj].iPt2.iY);
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenColor(TRgb(0x00, 0xFF, 0x00));
			aGraphicsContext->SetPenSize(TSize(1, 1));
			aGraphicsContext->DrawLine(lineProps[jj].iPt1, lineProps[jj].iPt2);
			aScreenDevice->Update();

			//RDebug::Print(_L("DrawLine %d, %d, %d, %d\r\n"), lineProps[jj].iPt1.iX, lineProps[jj].iPt1.iY, lineProps[jj].iPt2.iX, lineProps[jj].iPt2.iY);
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenColor(TRgb(0x00, 0xFF, 0x00));
			aGraphicsContext->SetPenSize(TSize(2, 2));
			aGraphicsContext->DrawLine(lineProps[jj].iPt1, lineProps[jj].iPt2);
			aScreenDevice->Update();
			}
		}
	}

//CFbsBitGc::Bitblt(), CFbsBitGc::BitbltMasked(), CFbsBitGc::DrawBitmap() test
void CTBitgdiScaling::TestBitbltL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("Bitblt(), BitbltMasked(), DrawBitmap() - Neg.coord."));
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	bitmap->Reset();
	User::LeaveIfError(bitmap->Load(KTestBmp, 0));
	TSize bmpSize = bitmap->SizeInPixels();
	TInt ii,jj;

	CFbsBitmap* maskBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBitmap);
	User::LeaveIfError(maskBitmap->Create(bmpSize, EGray256));
	TBitmapUtil bmpUtil(maskBitmap);
	bmpUtil.Begin(TPoint(0, 0));
	for(ii=0;ii<bmpSize.iWidth;++ii)
		{
		for(jj=0;jj<bmpSize.iHeight;++jj)
			{
			bmpUtil.SetPos(TPoint(ii,jj));
			bmpUtil.SetPixel(0xAA555555);
			}
		}
	bmpUtil.End();

	CFbsBitmap* maskBitmap2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBitmap2);
	User::LeaveIfError(maskBitmap2->Create(bmpSize, EGray16));
	TBitmapUtil bmpUtil2(maskBitmap2);
	bmpUtil2.Begin(TPoint(0, 0));
	for(ii=0;ii<bmpSize.iWidth;++ii)
		{
		for(jj=0;jj<bmpSize.iHeight;++jj)
			{
			bmpUtil2.SetPos(TPoint(ii,jj));
			bmpUtil2.SetPixel(0xBBBBBB);
			}
		}
	bmpUtil2.End();

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	for(TInt x=drawRect.iTl.iX;x<(drawRect.iBr.iX-bmpSize.iWidth);x+=10)
		{
		for(TInt y=drawRect.iTl.iY;y<(drawRect.iBr.iY-bmpSize.iHeight);y+=10)
			{
			::Clear(aGraphicsContext);

			//RDebug::Print(_L("BitBlt %d, %d\r\n"), x, y);
			aGraphicsContext->BitBlt(TPoint(x, y), bitmap);

			TRect rc(TPoint(x + bmpSize.iWidth + 1, y + bmpSize.iHeight + 1), bmpSize);
			//RDebug::Print(_L("DrawBitmap %d, %d, %d, %d\r\n"), rc.iTl.iX, rc.iTl.iY, rc.iBr.iX, rc.iBr.iY);
			aGraphicsContext->DrawBitmap(rc, bitmap);

			TRect rc2(TPoint(x + bmpSize.iWidth * 2 + 1, y + bmpSize.iHeight * 2 + 1), bmpSize);
			//RDebug::Print(_L("BitBltMasked %d, %d, %d, %d\r\n"), rc2.iTl.iX, rc2.iTl.iY, rc2.iBr.iX, rc2.iBr.iY);
			aGraphicsContext->BitBltMasked(rc2.iTl, bitmap, TRect(0, 0, bmpSize.iWidth, bmpSize.iHeight), maskBitmap, EFalse);

			TRect rc3(TPoint(x + bmpSize.iWidth * 3 + 1, y + bmpSize.iHeight * 3 + 1), bmpSize);
			//RDebug::Print(_L("BitBltMasked %d, %d, %d, %d\r\n"), rc3.iTl.iX, rc3.iTl.iY, rc3.iBr.iX, rc3.iBr.iY);
			aGraphicsContext->BitBltMasked(rc3.iTl, bitmap, TRect(0, 0, bmpSize.iWidth, bmpSize.iHeight), maskBitmap2, EFalse);

			aScreenDevice->Update();
			}
		}
	CleanupStack::PopAndDestroy(3);//bitmap & maskBitmap & maskBitmap2
	}

//CFbsBitGc::DrawArc(), CFbsBitGc::DrawPie(), CFbsBitGc::DrawRoundRect() test
void CTBitgdiScaling::TestArcPie(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawArc(), DrawPie(), DrawRoundRect() - Neg.coord."));

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	const TSize arcSize(20, 20);

	for(TInt x=drawRect.iTl.iX;x<(drawRect.iBr.iX-arcSize.iWidth);x+=10)
		{
		for(TInt y=drawRect.iTl.iY;y<(drawRect.iBr.iY-arcSize.iHeight);y+=10)
			{
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenStyle(CGraphicsContext::ESolidPen);
			aGraphicsContext->SetPenColor(TRgb(0x00, 0x00, 0x00));
			aGraphicsContext->SetPenSize(TSize(1, 1));
			TPoint pt(x, y);
			TRect rc(pt, arcSize);
			TPoint pt1(x, y + arcSize.iHeight / 2);
			TPoint pt2(x + arcSize.iWidth, y + arcSize.iHeight / 2);
			//RDebug::Print(_L("DrawArc %d, %d, %d, %d || %d, %d, %d, %d\r\n"), rc.iTl.iX, rc.iTl.iY, rc.iBr.iX, rc.iBr.iY, pt1.iX, pt1.iY, pt2.iX, pt2.iY);
			aGraphicsContext->DrawArc(rc, pt1, pt2);
			//RDebug::Print(_L("DrawArc %d, %d, %d, %d || %d, %d, %d, %d\r\n"), rc.iTl.iX, rc.iTl.iY, rc.iBr.iX, rc.iBr.iY, pt2.iX, pt2.iY, pt1.iX, pt1.iY);
			aGraphicsContext->DrawArc(rc, pt2, pt1);
			//RDebug::Print(_L("DrawPie %d, %d, %d, %d || %d, %d, %d, %d\r\n"), rc.iTl.iX, rc.iTl.iY, rc.iBr.iX, rc.iBr.iY, pt1.iX, pt1.iY, pt2.iX, pt2.iY);
			aGraphicsContext->DrawPie(rc, pt1, pt2);
			//RDebug::Print(_L("DrawPie %d, %d, %d, %d || %d, %d, %d, %d\r\n"), rc.iTl.iX, rc.iTl.iY, rc.iBr.iX, rc.iBr.iY, pt2.iX, pt2.iY, pt1.iX, pt1.iY);
			aGraphicsContext->DrawPie(rc, pt2, pt1);
			
			TRect rc2(TPoint(x + arcSize.iWidth + 1, y), arcSize);
			
			//RDebug::Print(_L("DrawRoundRect %d, %d, %d, %d\r\n"), rc.iTl.iX, rc.iTl.iY, rc.iBr.iX, rc.iBr.iY);
			aGraphicsContext->DrawRoundRect(rc2, TSize(4, 4));
			aScreenDevice->Update();
			}
		}
	}

//CFbsBitGc::DrawPolyLine(), CFbsBitGc::DrawPolygon() test
void CTBitgdiScaling::TestPolyL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawPolyLine(), DrawPolygon() - Neg.coord."));

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	for(TInt x=drawRect.iTl.iX;x<drawRect.iBr.iX;x+=10)
		{
		for(TInt y=drawRect.iTl.iY;y<drawRect.iBr.iY;y+=10)
			{
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenStyle(CGraphicsContext::ESolidPen);
			aGraphicsContext->SetPenColor(TRgb(0x00, 0x00, 0x00));
			aGraphicsContext->SetPenSize(TSize(1, 1));
			
			CPointArray* points = new (ELeave) CPointArray (10);
			CleanupStack::PushL(points);
			TPoint pt(x, y);
			points->AppendL(pt);
			pt.SetXY(x + 20, y + 20);
			points->AppendL(pt);
			pt.SetXY(x + 20, y + 40);
			points->AppendL(pt);
			pt.SetXY(x - 5, y + 14);
			points->AppendL(pt);
			pt.SetXY(x, y);
			points->AppendL(pt);
			
			TPoint pointList[3];
			pointList[0].SetXY(10,10);
			pointList[1].SetXY(50,50);
			pointList[2].SetXY(75,75);
			aGraphicsContext->DrawPolyLine(pointList,3);
			//RDebug::Print(_L("DrawPolyLine %d, %d\r\n"), x, y);
			aGraphicsContext->DrawPolyLine(points);
			aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));
			//RDebug::Print(_L("DrawPolygon %d, %d\r\n"), x, y);
			aGraphicsContext->DrawPolygon(points, CGraphicsContext::EWinding);
			
			aScreenDevice->Update();
			CleanupStack::PopAndDestroy(points);
			}
		}
	}

//CFbsBitGc::DrawEllipse(), CFbsBitGc::DrawLine(), CFbsBitGc::DrawRect() test
void CTBitgdiScaling::TestEllipseLineRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawEllipse(), DrawLine() - Neg.coord."));

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	for(TInt x=drawRect.iTl.iX;x<drawRect.iBr.iX;x+=10)
		{
		for(TInt y=drawRect.iTl.iY;y<drawRect.iBr.iY;y+=10)
			{
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenStyle(CGraphicsContext::ESolidPen);
			aGraphicsContext->SetPenColor(TRgb(0x00, 0x00, 0x00));
			aGraphicsContext->SetPenSize(TSize(1, 1));
			aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));
			//RDebug::Print(_L("DrawEllipse %d, %d %d %d\r\n"), x, y, x + 20, y + 10);
			aGraphicsContext->DrawEllipse(TRect(x, y, x + 20, y + 10));
			//RDebug::Print(_L("DrawEllipse %d, %d %d %d\r\n"), x + 20, y + 10, x + 40, y);
			aGraphicsContext->DrawLine(TPoint(x + 20, y + 10), TPoint(x + 40, y));
			//RDebug::Print(_L("DrawRect %d, %d %d %d\r\n"), x + 42, y, x + 52, y + 20);
			aGraphicsContext->DrawRect(TRect(x + 42, y, x + 52, y + 20));
			aScreenDevice->Update();
			}
		}
	}

//CFbsBitGc::DrawText() test
void CTBitgdiScaling::TestText(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, CFont*)
	{
	INFO_PRINTF1(_L("DrawText() - Neg.coord."));

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	_LIT(KText,"T1234567890QWERTY");
	for(TInt xx=(drawRect.iTl.iX-40);xx<(drawRect.iBr.iX+40);xx+=22)
		{
		for(TInt yy=(drawRect.iTl.iY-40);yy<(drawRect.iBr.iY+40);yy+=17)
			{
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenStyle(CGraphicsContext::ESolidPen);
			aGraphicsContext->SetPenColor(TRgb::Gray256(0x00));
			aGraphicsContext->SetPenSize(TSize(1,1));

			//RDebug::Print(_L("DrawText %d, %d\r\n"), xx, yy);
			aGraphicsContext->DrawText(KText, TPoint(xx,yy));
			aScreenDevice->Update();
			}
		}
	}

//CFbsBitGc::DrawTextVertical() test
void CTBitgdiScaling::TestTextVertical(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, CFont*)
	{
	INFO_PRINTF1(_L("DrawTextVertical() - Neg.coord."));

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	for(TInt x=(drawRect.iTl.iX-40);x<(drawRect.iBr.iX+40);x+=22)
		{
		for(TInt y=(drawRect.iTl.iY-40);y<(drawRect.iBr.iY+40);y+=17)
			{
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenStyle(CGraphicsContext::ESolidPen);
			aGraphicsContext->SetPenColor(TRgb(0x00, 0x00, 0x00));
			aGraphicsContext->SetPenSize(TSize(1, 1));
			
			//RDebug::Print(_L("DrawTextVertical %d, %d Down\r\n"), x, y);
			aGraphicsContext->DrawTextVertical(_L("Test text"), TPoint(x, y), ETrue);
			//RDebug::Print(_L("DrawTextVertical %d, %d Up\r\n"), x + 10, y);
			aGraphicsContext->DrawTextVertical(_L("Test text"), TPoint(x + 10, y), EFalse);
			aGraphicsContext->DrawTextVertical(_L("Test text "),drawRect, EFalse);
			aGraphicsContext->DrawTextVertical(_L("Test text "),drawRect, ETrue);
			
			aScreenDevice->Update();
			}
		}
	}

//CFbsBitGc::MapColors() test
void CTBitgdiScaling::TestMapColors(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("MapColors() - Neg.coord."));

	::Clear(aGraphicsContext);

	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));

	TRect rc(-10, -10, 10, 10);
	aGraphicsContext->DrawRect(rc);
	aScreenDevice->Update();

	TRgb colors[] = {TRgb(0xCC, 0x33, 0x66), TRgb(0x00, 0x00, 0xFF)};
	//RDebug::Print(_L("MapColors %d, %d, %d, %d\r\n"), rc.iTl.iX, rc.iTl.iY, rc.iBr.iX, rc.iBr.iY);
	aGraphicsContext->MapColors(rc, colors);
	aScreenDevice->Update();
	}

//CFbsBitGc::ShadowArea(), CFbsBitGc::FadeArea() test
void CTBitgdiScaling::TestShadowFade(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("ShadowArea(), FadeArea() - Neg.coord."));

	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);

	for(TInt x=(drawRect.iTl.iX - 20);x<(drawRect.iBr.iX+37);x+=17)
		{
		for(TInt y=(drawRect.iTl.iY-20);y<(drawRect.iBr.iY+37);y+=19)
			{
			TRect rc1(x, y, x + 20, y + 25);
			TRect rc2(x + 20, y + 5, x + 45, y + 20);

			::Clear(aGraphicsContext);
			aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));
			//RDebug::Print(_L("MapColors %d, %d, %d, %d\r\n"), rc1.iTl.iX, rc1.iTl.iY, rc1.iBr.iX, rc1.iBr.iY);
			aGraphicsContext->DrawRect(rc1);
			//RDebug::Print(_L("MapColors %d, %d, %d, %d\r\n"), rc2.iTl.iX, rc2.iTl.iY, rc2.iBr.iX, rc2.iBr.iY);
			aGraphicsContext->DrawRect(rc2);

			RRegion shadowRgn;
			shadowRgn.AddRect(rc1);
			shadowRgn.AddRect(rc2);
			//RDebug::Print(_L("ShadowArea\r\n"));
			aGraphicsContext->ShadowArea(&shadowRgn);
			shadowRgn.Close();

			aScreenDevice->Update();

			::Clear(aGraphicsContext);
			aGraphicsContext->SetBrushColor(TRgb(0xFF, 0x00, 0xFF));
			//RDebug::Print(_L("MapColors %d, %d, %d, %d\r\n"), rc1.iTl.iX, rc1.iTl.iY, rc1.iBr.iX, rc1.iBr.iY);
			aGraphicsContext->DrawRect(rc1);
			//RDebug::Print(_L("MapColors %d, %d, %d, %d\r\n"), rc2.iTl.iX, rc2.iTl.iY, rc2.iBr.iX, rc2.iBr.iY);
			aGraphicsContext->DrawRect(rc2);

			RRegion fadeRgn;
			fadeRgn.AddRect(rc1);
			fadeRgn.AddRect(rc2);
			//RDebug::Print(_L("FadeArea\r\n"));
			aGraphicsContext->FadeArea(&fadeRgn);
			fadeRgn.Close();

			aScreenDevice->Update();
			}
		}
	}

/**
	@SYMTestCaseID GRAPHICS-CODEBASE-BITGDI-0004
 
	@SYMPREQ PGM027
   
	@SYMTestCaseDesc  TestMove
 
	@SYMTestPriority 1
  
	@SYMTestStatus Implemented
   
	@SYMTestActions Draw the line using using DrawLine and move the line using MoveLine(),MoveBy()
	Then test the same for all orientaion.
   
	@SYMTestExpectedResults Test should perform graphics operations succesfully.

 */

void CTBitgdiScaling::TestMove(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("CODEBASE TEST 1 - TestMove Started"));
	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);
	for(TInt x=drawRect.iTl.iX;x<drawRect.iBr.iX;x+=10)
		{
		for(TInt y=drawRect.iTl.iY;y<drawRect.iBr.iY;y+=10)
			{
			::Clear(aGraphicsContext);
			aGraphicsContext->SetPenStyle(CGraphicsContext::ESolidPen);
			aGraphicsContext->SetPenColor(TRgb(0x00, 0x00, 0x00));
			aGraphicsContext->SetPenSize(TSize(1, 1));
			aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));
			aGraphicsContext->DrawLine(TPoint(x + 20, y + 10), TPoint(x + 40, y));
			aGraphicsContext->MoveTo(TPoint(100,100));
			aGraphicsContext->DrawLine(TPoint(x + 40, y + 50), TPoint(x + 70, y));
			aGraphicsContext->MoveTo(TPoint(100,100));
			aGraphicsContext->MoveBy(TPoint(0,-70));
			aGraphicsContext->DrawLineBy(TPoint(0,60));
			aGraphicsContext->MoveBy(TPoint(-70,70));
			aGraphicsContext->DrawLineBy(TPoint(60,0));
			aScreenDevice->Update();

			}
		}
	INFO_PRINTF1(_L("CODEBASE TEST 1 - TestMove Finished"));
	}

/**
	@SYMTestCaseID GRAPHICS-CODEBASE-BITGDI-0005
 
	@SYMPREQ PGM027
   
	@SYMTestCaseDesc  TestEllipseRect
 
	@SYMTestPriority 1
  
	@SYMTestStatus Implemented
   
	@SYMTestActions Draw the Rectangle with black color and pass the same rectagle in to the 
	DrawEllipse() with red color brush then check for Ellipse and Rectangle color are properly
	drawn respectively then test the same for all orientaion.
   
	@SYMTestExpectedResults Test should perform graphics operations succesfully.

 */
 
void CTBitgdiScaling::TestEllipseRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L(" TestEllipseRect Started"));
	TRect drawRect;
	aScreenDevice->GetDrawRect(drawRect);
	aGraphicsContext->SetPenStyle(CGraphicsContext::ESolidPen);
	aGraphicsContext->SetPenColor(TRgb(0x00, 0x00, 0x00));
	aGraphicsContext->SetPenSize(TSize(4,4));
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x00, 0x00, 0x255));
	TRect rrect1(20, 20, 40, 40);
	aGraphicsContext->DrawRect(rrect1);
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x255, 0x00, 0x00));
	TRect erect1(20, 20, 40, 40);
	aGraphicsContext->DrawEllipse(erect1); 
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x00, 0x00, 0x255));
	TRect rrect2(40, 40, 60, 60);
	aGraphicsContext->DrawRect(rrect2);
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x255, 0x00, 0x00));
	TRect erect2(40, 40, 60, 60);
	aGraphicsContext->DrawEllipse(erect2);
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x00, 0x00, 0x255));
	TRect rllrect3(60, 60, 80, 80);
	aGraphicsContext->DrawRect(rllrect3);
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x255, 0x00, 0x00));
	TRect ellrect3(60, 60, 80, 80);
	aGraphicsContext->DrawEllipse(ellrect3);
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x00, 0x00, 0x255));
	TRect rllrect4(80, 80,100,100);
	aGraphicsContext->DrawRect(rllrect4);
	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0x255, 0x00, 0x00));
	TRect ellrect4(80, 80,100,100);
	aGraphicsContext->DrawEllipse(ellrect4);
	aScreenDevice->Update();
	INFO_PRINTF1(_L(" TestEllipseRect Finished"));

	}
	
/**
	@SYMTestCaseID GRAPHICS-CODEBASE-BITGDI-0006
 
	@SYMPREQ PGM027
   
	@SYMTestCaseDesc  TestDrawBitmapNegL
 
	@SYMTestPriority 1
  
	@SYMTestStatus Implemented
   
	@SYMTestActions Draw the Bitmap using DrawBitmap() with the TRect passing negative values.
	then test the same for all orientaion.
   
	@SYMTestExpectedResults Test should perform graphics operations succesfully.

 */
 
void CTBitgdiScaling::TestDrawBitmapNegL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L(" TestDrawBitmapNegL Started"));
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);

	bitmap->Reset();
	User::LeaveIfError(bitmap->Load(KTestBmp, 0));

	::Clear(aGraphicsContext);
	TSize size = bitmap->SizeInPixels();
	//RDebug::Print(_L("DrawBitmap %d, %d, %d, %d\r\n"), 0, 0, size.iWidth, size.iHeight);
	aGraphicsContext->DrawBitmap(TRect(-100,-100, size.iWidth, size.iHeight), bitmap);
	aScreenDevice->Update();

	CleanupStack::PopAndDestroy(bitmap);
	INFO_PRINTF1(_L(" TestDrawBitmapNegL Finished"));
	}
/**
	@SYMTestCaseID GRAPHICS-CODEBASE-BITGDI-0007
 
	@SYMPREQ PGM027
   
	@SYMTestCaseDesc  TestMapColorsZero
 
	@SYMTestPriority 1
  
	@SYMTestStatus Implemented
   
	@SYMTestActions Mapthe pixels in the specified rectangle which size is zero using MapColors() with the TRect passing negative values.
	then test the same for all orientaion.
   
	@SYMTestExpectedResults Test should perform graphics operations succesfully.

 */	
void CTBitgdiScaling::TestMapColorsZero(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L(" TestMapColorsZero Started"));

	::Clear(aGraphicsContext);

	aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));

	TRect rc(0, 0, 0, 0);
	aGraphicsContext->DrawRect(rc);
	aScreenDevice->Update();

	TRgb colors[] = {TRgb(0xCC, 0x33, 0x66), TRgb(0x00, 0x00, 0xFF)};

	aGraphicsContext->MapColors(rc, colors);
	aScreenDevice->Update();
	INFO_PRINTF1(_L(" TestMapColorsZero Finished"));
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0037
 
  @SYMDEF             

  @SYMTestCaseDesc tests drawing various graphic primitives to the screen device set to different orientations
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Iterates through a series of screen rotations and draws graphic primitives to the screen
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/	
void CTBitgdiScaling::RunTests2L()
	{
	CFont* font = ::SetFontL(iScreenDevice, iGraphicsContext);
	iGraphicsContext->Clear();
	//Scaling origin
	const TPoint ptOrigin(73, 53);
	TInt err = iScreenDevice->SetScalingFactor(ptOrigin, KScalingFactorX, KScalingFactorY, 1, 1);
	TEST2(err, KErrNone);
	iGraphicsContext->Activate(iScreenDevice);
	
	const CFbsBitGc::TGraphicsOrientation KOrientation[] = 
		{
		CFbsBitGc::EGraphicsOrientationNormal,
		CFbsBitGc::EGraphicsOrientationRotated90,
		CFbsBitGc::EGraphicsOrientationRotated180,
		CFbsBitGc::EGraphicsOrientationRotated270
		};

	for(TInt ii=0;ii<TInt(sizeof(KOrientation)/sizeof(KOrientation[0]));++ii)
		{
		if(!iGraphicsContext->SetOrientation(KOrientation[ii]))
			{
			continue;
			}
		_LIT(KRotation,"===EOrientation%S===");
		INFO_PRINTF2(KRotation,&RotationName(KOrientation[ii]));
		RDebug::Print(KRotation,&RotationName(KOrientation[ii]));
		TRect drawRect;
		iScreenDevice->GetDrawRect(drawRect);
		INFO_PRINTF5(_L("Drawing rectangle: %d %d %d %d"), 
			drawRect.iTl.iX, drawRect.iTl.iY, drawRect.iBr.iX, drawRect.iBr.iY);

		TestDrawBitmapL(iScreenDevice, iGraphicsContext);
		TestClear(iScreenDevice, iGraphicsContext);
		TestBitbltL(iScreenDevice, iGraphicsContext);
		TestArcPie(iScreenDevice, iGraphicsContext);
		TestPolyL(iScreenDevice, iGraphicsContext);
		TestEllipseLineRect(iScreenDevice, iGraphicsContext);
		TestText(iScreenDevice, iGraphicsContext, font);
		iGraphicsContext->SetUnderlineStyle(EUnderlineOn);
		TestText(iScreenDevice, iGraphicsContext, font);
		iGraphicsContext->SetStrikethroughStyle(EStrikethroughOn);
		TestText(iScreenDevice, iGraphicsContext, font);
		iGraphicsContext->SetUnderlineStyle(EUnderlineOff);
		iGraphicsContext->SetStrikethroughStyle(EStrikethroughOff);
		TestTextVertical(iScreenDevice, iGraphicsContext, font);
		TestMapColors(iScreenDevice, iGraphicsContext);
		TestShadowFade(iScreenDevice, iGraphicsContext);
		((CTBitgdiScalingStep*)iStep)->RecordTestResultL();
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-BITGDI-0004"));	
		TestMove(iScreenDevice, iGraphicsContext);
		((CTBitgdiScalingStep*)iStep)->RecordTestResultL();
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-BITGDI-0005"));
		TestEllipseRect(iScreenDevice, iGraphicsContext);
		((CTBitgdiScalingStep*)iStep)->RecordTestResultL();
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-BITGDI-0007"));
		TestMapColorsZero(iScreenDevice, iGraphicsContext);
		((CTBitgdiScalingStep*)iStep)->RecordTestResultL();
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-BITGDI-0007"));
		TestDrawBitmapNegL(iScreenDevice, iGraphicsContext);
		((CTBitgdiScalingStep*)iStep)->RecordTestResultL();
		iGraphicsContext->Clear();
		}

	iGraphicsContext->Clear();
	iScreenDevice->ReleaseFont(font);
	iGraphicsContext->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	}
