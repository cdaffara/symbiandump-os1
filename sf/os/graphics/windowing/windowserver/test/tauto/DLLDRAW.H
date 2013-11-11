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
// Lots of different Drawing Commands
// 
//

	gc->SetBrushColor(TRgb::Gray4(2));
	gc->Clear();
	gc->SetBrushColor(TRgb::Gray4(0));
	gc->Clear(TRect(2,2,20,5));
	gc->DrawRect(TRect(size));
	gc->SetBrushOrigin(TPoint(1,2));
	gc->SetBrushStyle(CGraphicsContext::EForwardDiagonalHatchBrush);
	gc->DrawRect(TRect(30,2,60,8));
	gc->SetBrushOrigin(TPoint(0,0));
	gc->UseBrushPattern(aBitmap);
	gc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	gc->DrawRect(TRect(60,2,90,8));
	gc->SetBrushStyle(CGraphicsContext::ENullBrush);
	gc->DiscardBrushPattern();
//
	gc->SetPenColor(TRgb::Gray4(1));
	gc->DrawLine(TPoint(10,12),TPoint(22,20));
	gc->SetPenSize(TSize(4,2));
	gc->DrawLine(TPoint(22,10),TPoint(40,20));
	gc->SetPenStyle(CGraphicsContext::EDottedPen);
	gc->SetPenSize(TSize(1,1));
	gc->SetPenColor(TRgb::Gray4(0));
	gc->DrawLine(TPoint(40,11),TPoint(60,20));
	gc->SetPenStyle(CGraphicsContext::ESolidPen);
	gc->Plot(TPoint(60,10));
//
	gc->BitBlt(TPoint(size.AsPoint()-TPoint(20,20)),aBitmap);
	gc->BitBlt(TPoint(10,30),aBitmap,TRect(0,1,8,9));
	gc->BitBltMasked(TPoint(10,40),aBitmap,TRect(1,2,8,9),aMaskBitmap,ETrue);
	gc->BitBltMasked(TPoint(20,40),aBitmap,TRect(0,0,9,9),aMaskBitmap,EFalse);
 	gc->SetOrigin(TPoint(5,50));
	gc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
	gc->SetPenColor(TRgb::Gray4(3));
 	gc->SetClippingRect(TRect(0,0,20,15));
	//gc->DrawArc(TRect(0,0,20,15),TPoint(10,0),TPoint(1,8));
 	gc->CancelClippingRect();
	gc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	gc->SetPenColor(TRgb::Gray4(0));
	gc->MoveTo(TPoint(25,0));
	gc->DrawLineTo(TPoint(40,5));
	gc->DrawLineBy(TPoint(-20,10));
//
	CArrayFixFlat<TPoint>* polyLineArray=new(ELeave) CArrayFixFlat<TPoint>(4);
	CleanupStack::PushL(polyLineArray);
	polyLineArray->AppendL(TPoint(8,0));
	polyLineArray->AppendL(TPoint(16,10));
	polyLineArray->AppendL(TPoint(0,3));
	polyLineArray->AppendL(TPoint(16,3));
	polyLineArray->AppendL(TPoint(0,10));
	polyLineArray->AppendL(TPoint(8,0));
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetOrigin(TPoint(50,50));
	gc->DrawPolyLine(polyLineArray);
	gc->SetOrigin(TPoint(75,50));
	gc->DrawPolyLine(&(polyLineArray->At(0)),polyLineArray->Count());
	polyLineArray->Delete(5);
	gc->SetOrigin(TPoint(0,65));
	gc->DrawPolygon(polyLineArray,CGraphicsContext::EWinding);
	gc->SetOrigin(TPoint(25,65));
	gc->DrawPolygon(&(polyLineArray->At(0)),polyLineArray->Count(),CGraphicsContext::EWinding);
	gc->SetOrigin(TPoint(50,65));
	gc->DrawPolygon(polyLineArray,CGraphicsContext::EAlternate);
	gc->SetOrigin(TPoint(75,65));
	gc->DrawPolygon(&(polyLineArray->At(0)),polyLineArray->Count(),CGraphicsContext::EAlternate);
	CleanupStack::PopAndDestroy();
	gc->SetBrushStyle(CGraphicsContext::ENullBrush);
	gc->SetOrigin(TPoint(0,0));
//
	gc->DrawPie(TRect(5,80,30,100),TPoint(10,65),TPoint(35,80));
	gc->DrawEllipse(TRect(35,80,60,100));
	gc->DrawRoundRect(TRect(65,80,80,100),TSize(5,3));
	gc->DrawBitmap(TRect(10,105,40,120),aBitmap);
	gc->DrawBitmap(TRect(45,105,105,125),aBitmap,TRect(5,5,25,20));
//gc->DrawBitmap(TRect(110,40,205,125),aBitmap,TRect(5,5,25,20));
	if (aExtraDrawBitMap)
		gc->DrawBitmap(TPoint(110,70),aBitmap);
//
	gc->UseFont(aFont);
	TInt height=aFont->HeightInPixels();
	TInt ascent=aFont->AscentInPixels();
	TInt ypos=5;
	TInt xpos=100;
	gc->DrawText(_L("Test1"),TPoint(xpos,ypos+ascent));
	gc->DrawText(_L("Test2"),TRect(xpos,ypos+height,xpos+50,5+height*2),ascent);
	gc->DrawText(_L("Test3"),TRect(xpos,ypos+height*2,xpos+80,5+height*3),ascent,CGraphicsContext::ERight,10);
//
	ypos+=ascent+height*3;
	gc->SetUnderlineStyle(EUnderlineOn);
	gc->DrawText(_L("ULine"),TPoint(xpos,ypos));
	gc->SetUnderlineStyle(EUnderlineOff);
//
	gc->SetStrikethroughStyle(EStrikethroughOn);
	gc->DrawText(_L("Strike"),TPoint(xpos+50,ypos));
	gc->SetStrikethroughStyle(EStrikethroughOff);
	gc->SetCharJustification(15,3);
	gc->DrawText(_L("abc"),TPoint(xpos+100,ypos));
//
	gc->SetWordJustification(10,1);
	gc->DrawText(_L("Two Words"),TPoint(xpos+5,ypos+height));
//
	xpos = 80;
	ypos = 40;
	RRegion clipRegion(2);
//
	clipRegion.AddRect(TRect(xpos, ypos+10, xpos+30, ypos+20));
	clipRegion.AddRect(TRect(xpos, ypos+30, xpos+30, ypos+40));
	gc->SetClippingRegion(clipRegion);
//
	gc->DrawTextVertical(_L("Vertical Text"), TPoint(xpos+10, ypos+10), EFalse);
	gc->DrawTextVertical(_L("Vertical Text Box"), TRect(xpos+40, ypos+10, xpos+60, ypos+100), 15, EFalse);
//
	gc->CancelClippingRegion();
	clipRegion.Close();
//
	gc->AlphaBlendBitmaps(TPoint(xpos+60, ypos+100), aBitmap, TRect(0, 0, 20,20), aMaskBitmap, TPoint(0, 0));
//
	gc->SetPenColor(TRgb::Gray4(3));
	gc->SetBrushColor(TRgb::Gray4(0));
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	xpos = 0;
	ypos = 120;
	TRect mapColorRect(xpos, ypos, xpos+20, ypos+20);
	gc->DrawRect(mapColorRect);
	TRgb colours[4] = { TRgb::Gray4(3), TRgb::Gray4(0), TRgb::Gray4(0), TRgb::Gray4(3) };
	gc->MapColors(mapColorRect, colours, 2, ETrue);
//
	gc->DiscardFont();
