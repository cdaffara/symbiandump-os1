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
//

#include <fntstore.h>
#include <bitdraw.h>
#include <bitstd.h>
#include <bitdev.h>
#include "BITPANIC.H"
#include <shapeinfo.h>
#include <graphics/fbsrasterizer.h>

#include <bmalphablend.h>
#include <bitdrawinterfaceid.h>
#include "bitgcextradata.h"

/** Draws text at the last print position and then rotates it into a vertical position.

@param aText The text string to be drawn.
@param aUp ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is 
rotated 90 degrees clockwise. */
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,TBool aUp)
	{
	DrawTextVertical(aText,iLastPrintPosition,aUp);
	}

/** Draws text vertically from the specified position.

@param aText The text string to be drawn.
@param aPosition A point specifying the position of the left end of the text.
@param aUp  ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text 
is rotated 90 degrees clockwise. */
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,const TPoint& aPosition,TBool aUp)
	{
	TBrushStyle brushstyle=iBrushStyle;
	iBrushStyle=CGraphicsContext::ENullBrush;
	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	TInt ascent = metrics.MaxHeight();
	TInt height = ascent + metrics.MaxDepth();
	// The next few lines do much the same as TextWidthInPixels but pass
	// the text in visual order instead of logical order and also take
	// full account of left and right side bearings on the text
	CFont::TMeasureTextOutput output;
	CFont::TMeasureTextInput input;
	input.iFlags = CFont::TMeasureTextInput::EFVisualOrder;
	TInt advance = iFont.MeasureText(aText,&input,&output);
	
	/* expand the clipping rectangle to take account of any underline */
	if (iUnderline == EUnderlineOn)
		{
		TInt underline_top = 0, underline_bottom = 0;
		TInt err = GetUnderlineMetrics(underline_top,underline_bottom);
		TInt underlineStrikeoutOffset;
		underlineStrikeoutOffset= BaselineCorrection();
		underline_top+=underlineStrikeoutOffset;
		underline_bottom+=underlineStrikeoutOffset;
		BG_ASSERT_DEBUG(err == KErrNone, EBitgdiPanicInvalidArg);
		output.iBounds.iTl.iY = Min(output.iBounds.iTl.iY,underline_top);
		output.iBounds.iBr.iY = Max(output.iBounds.iBr.iY,underline_bottom);
		}

	TInt leftBearing = output.iBounds.iTl.iX;
	TInt rightBearing = advance - output.iBounds.iBr.iX;
	TRect box;
	TInt margin=0;
	if(aUp)
		{
		box.iTl.iX=aPosition.iX-ascent;
		box.iTl.iY=aPosition.iY-advance;
		box.iBr.iX=aPosition.iX+output.iBounds.iBr.iY+1;//was +height-ascent+1;
		box.iBr.iY=aPosition.iY;
		if(leftBearing<0)
			{
			box.iBr.iY-=leftBearing;
			margin=-leftBearing;
			}
		if(rightBearing<0)
			{
			box.iTl.iY+=rightBearing;
			}
   		}
	else
		{
		box.iTl.iX=aPosition.iX-output.iBounds.iBr.iY;//was +ascent-height;
		box.iTl.iY=aPosition.iY;
		box.iBr.iX=aPosition.iX+ascent+1;
		box.iBr.iY=aPosition.iY+advance;
		if(leftBearing<0)
			{
			box.iTl.iY+=leftBearing;
			margin=-leftBearing;
			}
		if(rightBearing<0)
			{
			box.iBr.iY-=rightBearing;
			}
		}
	DrawTextVertical(aText,box,ascent,aUp,ELeft,margin);
	iBrushStyle=brushstyle;
	}


/** Draws text clipped to the specified rectangle and then rotates it into a vertical 
position.

@param aText The text string to be drawn 
@param aBox The clipping rectangle.
@param aUp ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is 
rotated 90 degrees clockwise. */
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,const TRect& aBox,TBool aUp)
	{
	TRect boxcpy(aBox);
	boxcpy.Move(iOrigin);
	TRect oldcliprect(iUserClipRect);
	iUserClipRect.Intersection(boxcpy);
	DrawTextVertical(aText,aUp);
	iUserClipRect=oldcliprect;
	}


/** Draws text vertically, clipped to a specified rectangle,
using a baseline offset, alignment and margin.

@param aText The text string to be drawn.
@param aBox  A rectangle to clip the text to.
@param aBaselineOffset Number of pixels to offset the baseline by.
@param aUp ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is 
rotated 90 degrees clockwise.
@param aVert Verticaly alignment of the text relative to the specified 
rectangle.
@param aMargin Offset of the text from the position within the rectangle, 
using the specified alignment. */
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,
										  const TRect& aBox,
										  TInt aBaselineOffset,
										  TBool aUp,
										  TTextAlign aVert,
										  TInt aMargin)
    {
	DrawTextVertical(aText,aBox,aBaselineOffset,-1,aUp,aVert,aMargin);
	}


/** Draws text vertically, clipped to a specified rectangle, using a baseline 
offset, alignment and margin.

@param aText The text string to be drawn.
@param aBox A rectangle to clip the text to.
@param aBaselineOffset Number of pixels to offset the baseline by.
@param aTextWidth Number of pixels to clip the text to.
@param aUp ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text 
is rotated 90 degrees clockwise.
@param aVert Verticaly alignment of the text relative to the specified 
rectangle.
@param aMargin Offset of the text from the position within the rectangle, 
using the specified alignment. */	
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,
										  const TRect& aBox,
										  TInt aBaselineOffset,
										  TInt aTextWidth,
										  TBool aUp,
										  TTextAlign aVert,
										  TInt aMargin)
    {
	CheckDevice();
	BG_ASSERT_ALWAYS(iFont.Handle()!=0,EBitgdiPanicNoFontSelected);
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	TRect boxcpy(aBox);
	boxcpy.Move(iOrigin);
	AddRect(boxcpy);
	if (boxcpy.Intersects(iUserClipRect))
		{
		boxcpy.Intersection(iUserClipRect);
		}
	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);
	const CBitmapFont* fontaddress=iFont.Address();
	if(!fontaddress)
		{
		if (brushRasterizer)
			{
			brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
			}
		iDevice->DrawingEnd(&iBrushBitmap);
		BG_PANIC_ALWAYS(EBitgdiPanicNoFontSelected);
		}

	TInt width = iFont.MeasureText(aText);
	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	TInt textlength=aText.Length();
	if(aTextWidth<0) aTextWidth=width;
	TPoint coords;
	coords.iX=aBox.iTl.iX;
	TInt dirmult=aUp?-1:1;
	coords.iY=aUp?aBox.iBr.iY-1:aBox.iTl.iY;
	//
	// iX calculation, for example: ascent(a)=18 descent(d)=2 size=boxwidth=fontheight(h)=20 baseline=ascent
	// pre: iX = 0
	//
	// hhhhhhhhhhhhhhhhhhhh
	// 01234567890123456789
	// aaaaaaaaaaaaaaaaaadd	aUp=ETrue
	//                   ^
	//                   iX = 18 (baseline)
	//
	// ddaaaaaaaaaaaaaaaaaa aUp=EFalse
	//  ^
	//  iX = 1 (instead of 2 ie 20-18-1 which is boxwidth-baseline-1)
	//
	coords.iX+=aUp?aBaselineOffset:aBox.Width()-aBaselineOffset-1;
	switch(aVert)
		{
	case ELeft:
		coords.iY+=aMargin*dirmult;
		break;
	case ECenter:
		coords.iY+=(((aBox.iBr.iY-aBox.iTl.iY-aTextWidth)>>1)+aMargin)*dirmult;
		break;
	case ERight:
		coords.iY+=(aBox.iBr.iY-aBox.iTl.iY-aTextWidth-aMargin)*dirmult;
		break;
		}
	iLastPrintPosition=coords;
	coords.iX+=fontaddress->iAlgStyle.iBaselineOffsetInPixels*dirmult;
	coords+=iOrigin;
	TInt prewidth=width+iCharJustExcess+iWordJustExcess;
	iLastPrintPosition.iY-=aUp?prewidth-1:-prewidth;
	if(boxcpy.IsEmpty())
		{
		if (brushRasterizer)
			{
			brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
			}
		iDevice->DrawingEnd(&iBrushBitmap);
		if (iAutoUpdateJustification)
			UpdateJustificationVertical(aText,aUp);
		return;
		}
	RectFill(boxcpy);
	TBool userintersect=boxcpy.Intersects(iUserClipRect);
	if(!userintersect || !textlength || !width)
		{
		if (brushRasterizer)
			{
			brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
			}
		iDevice->DrawingEnd(&iBrushBitmap);
		if (iAutoUpdateJustification)
			UpdateJustificationVertical(aText,aUp);
		return;
		}
	boxcpy.Intersection(iUserClipRect);
	TInt charjustexcess=iCharJustExcess;
	TInt charjustnum=iCharJustNum;
	TInt wordjustexcess=iWordJustExcess;
	TInt wordjustnum=iWordJustNum;
	TInt limit=iDefaultRegionPtr->Count();
	for(TInt count=0;count<limit;count++)
		{
		/*
		Initialise the justification parameters to the original values at the start of each pass.
		This means that any number of passes have exactly the same effect on the parameters as one pass;
		that is, to reduce them by the numbers of items and pixels consumed.
		*/
		iCharJustExcess = charjustexcess;
		iCharJustNum = charjustnum;
		iWordJustExcess = wordjustexcess;
		iWordJustNum = wordjustnum;

		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(boxcpy))
			continue;
#if defined(_DEBUG)
		TRect deviceRect;
		drawDevice->GetDrawRect(deviceRect);
		BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);
#endif
		iClipRect.Intersection(boxcpy);
		/*
		Set up the parameter block for character positioning.
		Draw left to right, because although the text is being drawn vertically,
		it is done by rotating the baseline 90 degrees and drawing in the ordinary way, not by drawing
		the characters in their normal orientation but in a vertical column.
		*/
		CFont::TPositionParam param;
		param.iText.Set(aText);
		param.iPen = coords;

		// Draw the text.
		DoDrawTextVertical(param,fontaddress,aUp,aText.Length());

		drawDevice->UpdateRegion(iClipRect);
		}
	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);
	if(iAutoUpdateJustification)
		UpdateJustificationVertical(aText,aUp);
	}

// Transform a vector, defined by a point relative to an origin, from left-to-right to up or down.
static void Rotate(TPoint& aPoint,const TPoint aOrigin,TBool aUp)
	{
	int dx = aPoint.iX - aOrigin.iX;
	int dy = aPoint.iY - aOrigin.iY;
	if (aUp)
		{
		aPoint.iX = aOrigin.iX + dy;
		aPoint.iY = aOrigin.iY - dx;
		}
	else
		{
		aPoint.iX = aOrigin.iX - dy;
		aPoint.iY = aOrigin.iY + dx;
		}
	}

void CFbsBitGc::DoDrawTextVertical(CFont::TPositionParam& aParam,
								   const CBitmapFont* font,
								   TBool aUp,const TInt aEnd)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	TInt charclipping=iClipRect.iTl.iY;
	TPoint start_pen = aParam.iPen;
	TInt underline_top = 0, underline_bottom = 0;
	TInt underlineStrikeoutOffset;

	//note using measured text here, but the full text string
	underlineStrikeoutOffset= BaselineCorrection();

	if (iUnderline == EUnderlineOn)
		{
		TInt err = GetUnderlineMetrics(underline_top,underline_bottom);
		underline_top+=underlineStrikeoutOffset;
		underline_bottom+=underlineStrikeoutOffset;
		BG_ASSERT_DEBUG(err == KErrNone, EBitgdiPanicInvalidArg);
		}

	const TBool outlineAndShadow = (font->GlyphBitmapType() == EFourColourBlendGlyphBitmap);
	if ((outlineAndShadow) && !(iBrushStyle == ENullBrush || iBrushStyle == ESolidBrush))
		{
		//For future compatibility it is better if brush style of ENullBrush or ESolidBrush is used 
		//when drawing outline and shadow fonts.
		BG_PANIC_ALWAYS(EBitgdiPanicInvalidBrushStyle);
		}

	TInt strike_top = 0, strike_bottom = 0;
	TBool italic = font->iAlgStyle.IsItalic();
	if (italic || iStrikethrough == EStrikethroughOn)
		{
		GetStrikethroughMetrics(strike_top,strike_bottom);
		if (iStrikethrough == EStrikethroughOn) //not for italic only, mimimum change
			{
			strike_top+=underlineStrikeoutOffset;
			strike_bottom+=underlineStrikeoutOffset;
			}
		}
	TInt italicheight=start_pen.iX;
	italicheight += aUp ? -strike_top : strike_top;
	TBool bold = font->iAlgStyle.IsBold();
	TInt widthfactor = font->iAlgStyle.WidthFactor();
	TInt heightfactor = font->iAlgStyle.HeightFactor();
	TGlyphBitmapType glyphBitmapTypeForFont = font->GlyphBitmapType();
	TBool outlineShadowOrAntialiased = (glyphBitmapTypeForFont == EAntiAliasedGlyphBitmap || 
			glyphBitmapTypeForFont == EFourColourBlendGlyphBitmap);

	RShapeInfo shapeInfo;
	while (aParam.iPosInText < aEnd)
		{
		TPoint start_pen = aParam.iPen;
	
		if (!iFont.GetCharacterPosition2(aParam, shapeInfo))
			continue;
		Rotate(aParam.iPen,start_pen,aUp);
		TInt adjustment = 0;
		if(iCharJustExcess && iCharJustNum>0) // character clipping/justification
			{
			adjustment=CGraphicsContext::JustificationInPixels(iCharJustExcess,iCharJustNum);
			if (adjustment < 0)
				iClipRect.iTl.iY = aParam.iPen.iY + (aUp ? -adjustment : adjustment);
			}

		
		
		CFont::TPositionParam::TOutput* output = aParam.iOutput;
		
		for (int i = 0; i < aParam.iOutputGlyphs; i++, output++)
			{			
			TOpenFontCharMetrics characterParams;
			const TUint8* bitmap;
			TSize size;
			//note may now be using a glyph code, and not a character
			iFont.GetCharacterData(aParam.iOutput[i].iCode,characterParams,bitmap,size);
			TGlyphBitmapType glyphType = characterParams.GlyphType();
			
			Rotate(output->iBounds.iTl,start_pen,aUp);
			Rotate(output->iBounds.iBr,start_pen,aUp);
			
			switch (glyphType)
				{
				//the glyphType from the character takes precidence over the glyphType from the fon
				case EAntiAliasedGlyphBitmap:
				case EFourColourBlendGlyphBitmap:
					DoDrawCharacterVerticalAntiAliased(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,aUp,glyphType);
					break;

				case EDefaultGlyphBitmap:
				case EMonochromeGlyphBitmap:
					DoDrawCharacterVertical(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,
							bold,italic,italicheight,widthfactor,heightfactor,aUp);
					break;

				default:
					//if the outline or shadow is not specified for the character, then use the font setting
					if (outlineShadowOrAntialiased)
						{
						DoDrawCharacterVerticalAntiAliased(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,aUp,glyphBitmapTypeForFont);		
						}
					else
						{
						DoDrawCharacterVertical(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,
								bold,italic,italicheight,widthfactor,heightfactor,aUp);
						}			 
					break;
				} 
			}

		iClipRect.iTl.iY = charclipping;
		if (adjustment)
			aParam.iPen.iY += aUp ? -adjustment : adjustment;
		if (iWordJustExcess > 0 && iWordJustNum > 0 && aParam.iOutput[0].iCode == 0x0020) // word justification
			{
			adjustment=CGraphicsContext::JustificationInPixels(iWordJustExcess,iWordJustNum);
			aParam.iPen.iY += aUp ? -adjustment : adjustment;
			}
		}
	if (shapeInfo.IsOpen())
		shapeInfo.Close();
	if (iUnderline == EUnderlineOn || iStrikethrough == EStrikethroughOn)
		{
		TRect ul; // underline
		TRect st; // strikethrough
		if (aUp)
			{
			ul.SetRect(start_pen.iX+underline_top,aParam.iPen.iY,start_pen.iX+underline_bottom,start_pen.iY+1);
			st.SetRect(start_pen.iX+strike_top,aParam.iPen.iY,start_pen.iX+strike_bottom,start_pen.iY+1);
			ul.iTl.iY= ul.iBr.iY-ul.Height()*widthfactor;
 			st.iTl.iY= st.iBr.iY-st.Height()*widthfactor;
			}
		else
			{
			ul.SetRect(start_pen.iX-underline_bottom,start_pen.iY,start_pen.iX-underline_top,aParam.iPen.iY);
			st.SetRect(start_pen.iX-strike_bottom,start_pen.iY,start_pen.iX-strike_top,aParam.iPen.iY);
			ul.iBr.iY=ul.iTl.iY+ul.Height()*widthfactor;
			st.iBr.iY=st.iTl.iY+st.Height()*widthfactor;
			ul.iTl.iX++; // adjust for rect not including last line
			ul.iBr.iX++;
			st.iTl.iX++;
			st.iBr.iX++;
			}

		if (iUnderline == EUnderlineOn)
			if (ul.Intersects(iClipRect)) // checks for empty aRect as well
				{
				ul.Intersection(iClipRect);
				drawDevice->WriteRgbMulti(ul.iTl.iX,ul.iTl.iY,ul.iBr.iX-ul.iTl.iX,ul.iBr.iY-ul.iTl.iY,iPenColor,iDrawMode);
				}

		if (iStrikethrough == EStrikethroughOn)
			if(st.Intersects(iClipRect)) // checks for empty aRect as well
				{
				st.Intersection(iClipRect);
				drawDevice->WriteRgbMulti(st.iTl.iX,st.iTl.iY,st.iBr.iX-st.iTl.iX,st.iBr.iY-st.iTl.iY,iPenColor,iDrawMode);
				}
		}
	}

void CFbsBitGc::DoDrawCharacterVertical(const TPoint& aTopLeft,
										const TSize& aDataSize,
										const TUint8* aData,
										TBool aBold,
										TBool aItalic,
										TInt aItalicPos,
										TInt aWidthFactor,
										TInt aHeightFactor,
										TBool aUp)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	TPoint printpos(aTopLeft);
	TInt datalength = aDataSize.iWidth;
	TInt dataheight = aDataSize.iHeight;
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TInt xinc=aUp?1:-1,yinc=aUp?-1:1;
	TUint32* slbuffer=drawDevice->ScanLineBuffer();
	TInt slwords=(drawDevice->ScanLineBytes())<<3;
	if(aItalic && aTopLeft.iY<aItalicPos)
		printpos.iY+=yinc;
	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		TInt signedrepeatcount=aUp?repeatcount:-repeatcount;
		if(multilineflag)
			{
			for(TInt currentline=0;currentline<repeatcount;currentline++)
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
				OutputCharLineVertical(printpos,slbuffer,datalength,1,aBold,aWidthFactor,aHeightFactor,aUp);
				bitindex+=datalength;
				TBool aboveitalicjump=EFalse;
				if(aItalic && ((aUp && printpos.iX<aItalicPos) || (!aUp && printpos.iX>aItalicPos)))
					aboveitalicjump=ETrue;
				printpos.iX+=xinc*aHeightFactor;
				if(aboveitalicjump && ((aUp && printpos.iX>=aItalicPos) || (!aUp && printpos.iX<=aItalicPos)))
					printpos.iY-=yinc*aWidthFactor;
				}
			}
		else
			{
			if(aItalic)
				{
				for(TInt currentline=0;currentline<repeatcount;currentline++)
					{
					CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
					OutputCharLineVertical(printpos,slbuffer,datalength,1,aBold,aWidthFactor,aHeightFactor,aUp);
					TBool aboveitalicjump=EFalse;
					if((aUp && printpos.iX<aItalicPos) || (!aUp && printpos.iX>aItalicPos))
						aboveitalicjump=ETrue;
					printpos.iX+=xinc*aHeightFactor;
					if(aboveitalicjump && ((aUp && printpos.iX>=aItalicPos) || (!aUp && printpos.iX<=aItalicPos)))
						printpos.iY-=yinc*aWidthFactor;
					}
				}
			else
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
				OutputCharLineVertical(printpos,slbuffer,datalength,repeatcount,aBold,aWidthFactor,aHeightFactor,aUp);
				printpos.iX+=signedrepeatcount*aHeightFactor;
				}
			bitindex+=datalength;
			}
		}
	}

/** 
@internalTechnology

This function draws an anti-aliased character.

@param aTopLeft The position to output the character
@param aDataSize The size of the bitmap
@param aData the bitmap for outputt in their TOpenFontCharMetrics
@param aUp the direction of the text
@param aGlyphType the glyph type for the character
*/
void CFbsBitGc::DoDrawCharacterVerticalAntiAliased(const TPoint& aTopLeft,
												   const TSize& aDataSize,
												   const TUint8* aData,
												   TBool aUp,
												   TGlyphBitmapType aGlyphBitmapType)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	const int KBufferSize = 32;
	TUint8 mask_buffer[KBufferSize];
	int top_row = 0;
	int bottom_row = 0;
	int left_col = 0;
	int right_col = 0;
	const TUint32 penColor = iPenColor.Internal();
	const TUint32 shadowColor = iFbsBitGcExtraData->ShadowColor().Internal();
	const TUint32 brushColor = iBrushColor.Internal();
	
	MOutlineAndShadowBlend* outlineAndShadow = NULL;
	if (EFourColourBlendGlyphBitmap == aGlyphBitmapType)
		{
		TInt err = iDevice->iDrawDevice->GetInterface(KOutlineAndShadowInterfaceID, reinterpret_cast <TAny*&> (outlineAndShadow));
		if(err != KErrNone)
			{
			// Assert if MOutlineAndShadowBlend interface is not implemented
			BG_ASSERT_DEBUG(outlineAndShadow, EBitgdiPanicInvalidInterfaceHandle);
			}
		}
	
	if (aUp)
		{
		top_row = Max(0,iClipRect.iTl.iX - aTopLeft.iX);
		bottom_row = Min(aDataSize.iHeight,iClipRect.iBr.iX - aTopLeft.iX);
		left_col = Max(0,aTopLeft.iY - iClipRect.iBr.iY + 1);
		right_col = Min(aDataSize.iWidth,aTopLeft.iY - iClipRect.iTl.iY + 1);
		int y = aTopLeft.iY - (right_col - 1);
		for (int col = right_col - 1; col >= left_col; col--, y++)
			{
			int x = aTopLeft.iX + top_row;
			for (int row = top_row; row < bottom_row; row += KBufferSize, x += KBufferSize)
				{
				int length = KBufferSize;
				if (length > bottom_row - row)
					length = bottom_row - row;
				const TUint8* p = aData + row * aDataSize.iWidth + col;
				for (int i = 0; i < length; i++, p += aDataSize.iWidth)
					mask_buffer[i] = *p;
				if (EFourColourBlendGlyphBitmap == aGlyphBitmapType)
					{
					//There is a support for the interface with KOutlineAndShadowInterface id.
					outlineAndShadow->WriteRgbOutlineAndShadow(x, y, length, penColor, shadowColor,	brushColor, mask_buffer);
					}
				else
					{
					drawDevice->WriteRgbAlphaMulti(x,y,length,iPenColor,mask_buffer);
					}
				}
			}
		}
	else
		{
		top_row = Max(0,aTopLeft.iX - iClipRect.iBr.iX + 1);
		bottom_row = Min(aDataSize.iHeight,aTopLeft.iX - iClipRect.iTl.iX + 1);
		left_col = Max(0,iClipRect.iTl.iY - aTopLeft.iY);
		right_col = Min(aDataSize.iWidth,iClipRect.iBr.iY - aTopLeft.iY);
		int y = aTopLeft.iY + left_col;
		for (int col = left_col; col < right_col; col++, y++)
			{
			int x = aTopLeft.iX - (bottom_row - 1);
			for (int row = bottom_row; row > top_row; row -= KBufferSize, x += KBufferSize)
				{
				int length = KBufferSize;
				if (length > row - top_row)
					length = row - top_row;
				const TUint8* p = aData + (row - 1) * aDataSize.iWidth + col;
				for (int i = 0; i < length; i++, p -= aDataSize.iWidth)
					mask_buffer[i] = *p;
				if (EFourColourBlendGlyphBitmap == aGlyphBitmapType)
					{
					//There is a support for the interface with KOutlineAndShadowInterface id.
					outlineAndShadow->WriteRgbOutlineAndShadow(x, y, length, penColor, shadowColor,	brushColor, mask_buffer);
					}
				else
					{
					drawDevice->WriteRgbAlphaMulti(x,y,length,iPenColor,mask_buffer);
					}
				}
			}
		}
	}

void CFbsBitGc::OutputCharLineVertical(TPoint aPrintPos,
									   TUint32* aBuffer,
									   TInt aDataLength,
									   TInt aNum,
									   TBool aBold,
									   TInt aWidthFactor,
									   TInt aHeightFactor,
									   TBool aUp)
	{
	TInt xinc=aUp?1:-1;
	if(aDataLength<=0) return;
	TInt bufferwords=(aDataLength+31)>>5;
	TUint32* bufferlimit=aBuffer+bufferwords;
	if(aBold)
		{
		TInt sparemask=(0xffffffff>>(32-(aDataLength&0x1f)));
		if((aDataLength&0x1f)==0) sparemask=0xffffffff;
		*(bufferlimit-1)&=sparemask;
		TUint32* bufferptr=aBuffer;
		TUint32 extrabit=0;
		while(bufferptr<bufferlimit)
			{
			extrabit=*bufferptr>>31;
			*bufferptr|=(*bufferptr<<1);
			++bufferptr;
			if(bufferptr<bufferlimit)
				*bufferptr|=extrabit;
			}
		aDataLength++;
		if((aDataLength&0x1f)==1)
			{
			bufferwords++;
			*bufferlimit=extrabit;
			bufferlimit++;
			}
		}
	if(aWidthFactor>1)
		{
		BitMultiply(aBuffer,aDataLength,aWidthFactor);
		aDataLength*=aWidthFactor;
		bufferwords=(aDataLength+31)>>5;
		bufferlimit=aBuffer+bufferwords;
		}
	if(aUp)
		{
		if(aPrintPos.iY>=iClipRect.iBr.iY)
			{
			TInt pixelexcess=aPrintPos.iY-iClipRect.iBr.iY+1;
			while(pixelexcess>=32)
				{
				aBuffer++;
				bufferwords--;
				aDataLength-=32;
				pixelexcess-=32;
				}
			if(aDataLength<=0) return;
			if(pixelexcess>0)
				{
				TInt shiftup=32-pixelexcess;
				TUint32* bufferptr=aBuffer;
				while(bufferptr<bufferlimit)
					{
					*bufferptr>>=pixelexcess;
					if(bufferptr<bufferlimit-1)
						*bufferptr|=(*(bufferptr+1)<<shiftup);
					bufferptr++;
					}
				aDataLength-=pixelexcess;
				if(aDataLength<=0) return;
				}
			aPrintPos.iY=iClipRect.iBr.iY-1;
			}
		if(aPrintPos.iY-aDataLength<iClipRect.iTl.iY-1)
			{
			TInt pixelexcess=iClipRect.iTl.iY-1-aPrintPos.iY+aDataLength;
			aDataLength-=pixelexcess;
			if(aDataLength<=0) return;
			}
		}
	else
		{
		if(aPrintPos.iY<iClipRect.iTl.iY)
			{
			TInt pixelexcess=iClipRect.iTl.iY-aPrintPos.iY;
			while(pixelexcess>=32)
				{
				aBuffer++;
				bufferwords--;
				aDataLength-=32;
				pixelexcess-=32;
				}
			if(aDataLength<=0) return;
			if(pixelexcess>0)
				{
				TInt shiftup=32-pixelexcess;
				TUint32* bufferptr=aBuffer;
				while(bufferptr<bufferlimit)
					{
					*bufferptr>>=pixelexcess;
					if(bufferptr<bufferlimit-1)
						*bufferptr|=(*(bufferptr+1)<<shiftup);
					bufferptr++;
					}
				aDataLength-=pixelexcess;
				if(aDataLength<=0) return;
				}
			aPrintPos.iY=iClipRect.iTl.iY;
			}
		if(aPrintPos.iY+aDataLength>iClipRect.iBr.iY)
			{
			TInt pixelexcess=aPrintPos.iY+aDataLength-iClipRect.iBr.iY;
			aDataLength-=pixelexcess;
			if(aDataLength<=0) return;
			}
		}
	aNum*=aHeightFactor;
	while(aNum>0)
		{
		if(aPrintPos.iX>=iClipRect.iTl.iX && aPrintPos.iX<iClipRect.iBr.iX)
			iDevice->iDrawDevice->WriteBinaryLineVertical(aPrintPos.iX,aPrintPos.iY,aBuffer,aDataLength,iPenColor,iDrawMode,aUp);
		aPrintPos.iX+=xinc;
		aNum--;
		}
	}


EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp)
	{
	DrawTextVertical(aText,aParam,iLastPrintPosition,aUp);
	}
	
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition,TBool aUp)
	{
	TBrushStyle brushstyle=iBrushStyle;
	iBrushStyle=CGraphicsContext::ENullBrush;
	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	TInt ascent = metrics.MaxHeight();
	TInt height = ascent + metrics.MaxDepth();
	// The next few lines do much the same as TextWidthInPixels but pass
	// the text in visual order instead of logical order and also take
	// full account of left and right side bearings on the text
	CFont::TMeasureTextOutput output;
	CFont::TMeasureTextInput input;
	input.iFlags = CFont::TMeasureTextInput::EFVisualOrder;
	if (aParam)
		{
		BG_ASSERT_ALWAYS(aParam->iStart < aParam->iEnd ,EBitgdiPanicInvalidParameter);
		input.iStartInputChar = aParam->iStart;
		input.iEndInputChar = Min(aText.Length(),aParam->iEnd);
		}
	TInt advance = iFont.MeasureText(aText,&input,&output);
	
	//the box is not expanded by the underline position, as it is for horizontal text.
	TInt leftBearing = output.iBounds.iTl.iX;
	TInt rightBearing = advance - output.iBounds.iBr.iX;
	TRect box;
	TInt margin=0;
	if(aUp)
		{
		box.iTl.iX=aPosition.iX-ascent;
		box.iTl.iY=aPosition.iY-advance;
		box.iBr.iX=aPosition.iX+height-ascent+1;
		box.iBr.iY=aPosition.iY;
		if(leftBearing<0)
			{
			box.iBr.iY-=leftBearing;
			margin=-leftBearing;
			}
		if(rightBearing<0)
			{
			box.iTl.iY+=rightBearing;
			}
   		}
	else
		{
		box.iTl.iX=aPosition.iX+ascent-height;
		box.iTl.iY=aPosition.iY;
		box.iBr.iX=aPosition.iX+ascent+1;
		box.iBr.iY=aPosition.iY+advance;
		if(leftBearing<0)
			{
			box.iTl.iY+=leftBearing;
			margin=-leftBearing;
			}
		if(rightBearing<0)
			{
			box.iBr.iY-=rightBearing;
			}
		}
	DrawTextVertical(aText,aParam,box,ascent,aUp,ELeft,margin);
	iBrushStyle=brushstyle;
	}
	
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TBool aUp)
	{
	TRect boxcpy(aBox);
	boxcpy.Move(iOrigin);
	TRect oldcliprect(iUserClipRect);
	iUserClipRect.Intersection(boxcpy);
	DrawTextVertical(aText,aParam,aUp);
	iUserClipRect=oldcliprect;
	}
	
EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert,TInt aMargin)
	{
	DrawTextVertical(aText,aParam,aBox,aBaselineOffset,-1,aUp,aVert,aMargin);
	}

EXPORT_C void CFbsBitGc::DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TInt aTextWidth,TBool aUp,TTextAlign aVert,TInt aMargin)
	{
	CheckDevice();
	BG_ASSERT_ALWAYS(iFont.Handle()!=0,EBitgdiPanicNoFontSelected);
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	TRect boxcpy(aBox);
	boxcpy.Move(iOrigin);
	AddRect(boxcpy);
	if (boxcpy.Intersects(iUserClipRect))
		{
		boxcpy.Intersection(iUserClipRect);
		}
	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);
	const CBitmapFont* fontaddress=iFont.Address();
	if(!fontaddress)
		{
		if (brushRasterizer)
			{
			brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
			}
		iDevice->DrawingEnd(&iBrushBitmap);
		BG_PANIC_ALWAYS(EBitgdiPanicNoFontSelected);
		}
	CFont::TMeasureTextInput input;
	//CFont::TMeasureTextOutput
	if (aParam)
		{
		BG_ASSERT_ALWAYS(aParam->iStart < aParam->iEnd ,EBitgdiPanicInvalidParameter);
		input.iStartInputChar = aParam->iStart;
		input.iEndInputChar = Min(aText.Length(),aParam->iEnd);
		}
	TInt width = iFont.MeasureText(aText,&input);
	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	TInt textlength=aText.Length();
	if(aTextWidth<0) aTextWidth=width;
	TPoint coords;
	coords.iX=aBox.iTl.iX;
	TInt dirmult=aUp?-1:1;
	coords.iY=aUp?aBox.iBr.iY-1:aBox.iTl.iY;
	//
	// iX calculation, for example: ascent(a)=18 descent(d)=2 size=boxwidth=fontheight(h)=20 baseline=ascent
	// pre: iX = 0
	//
	// hhhhhhhhhhhhhhhhhhhh
	// 01234567890123456789
	// aaaaaaaaaaaaaaaaaadd	aUp=ETrue
	//                   ^
	//                   iX = 18 (baseline)
	//
	// ddaaaaaaaaaaaaaaaaaa aUp=EFalse
	//  ^
	//  iX = 1 (instead of 2 ie 20-18-1 which is boxwidth-baseline-1)
	//
	coords.iX+=aUp?aBaselineOffset:aBox.Width()-aBaselineOffset-1;
	switch(aVert)
		{
	case ELeft:
		coords.iY+=aMargin*dirmult;
		break;
	case ECenter:
		coords.iY+=(((aBox.iBr.iY-aBox.iTl.iY-aTextWidth)>>1)+aMargin)*dirmult;
		break;
	case ERight:
		coords.iY+=(aBox.iBr.iY-aBox.iTl.iY-aTextWidth-aMargin)*dirmult;
		break;
		}
	iLastPrintPosition=coords;
	coords.iX+=fontaddress->iAlgStyle.iBaselineOffsetInPixels*dirmult;
	coords+=iOrigin;
	TInt prewidth=width+iCharJustExcess+iWordJustExcess;
	iLastPrintPosition.iY-=aUp?prewidth-1:-prewidth;
	if(boxcpy.IsEmpty())
		{
		if (brushRasterizer)
			{
			brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
			}
		iDevice->DrawingEnd(&iBrushBitmap);
		if (iAutoUpdateJustification)
			UpdateJustificationVertical(aText,aParam,aUp);
		return;
		}
	RectFill(boxcpy);
	TBool userintersect=boxcpy.Intersects(iUserClipRect);
	if(!userintersect || !textlength || !width)
		{
		if (brushRasterizer)
			{
			brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
			}
		iDevice->DrawingEnd(&iBrushBitmap);
		if (iAutoUpdateJustification)
			UpdateJustificationVertical(aText,aParam,aUp);
		return;
		}
	boxcpy.Intersection(iUserClipRect);
	TInt charjustexcess=iCharJustExcess;
	TInt charjustnum=iCharJustNum;
	TInt wordjustexcess=iWordJustExcess;
	TInt wordjustnum=iWordJustNum;
	TInt limit=iDefaultRegionPtr->Count();
	for(TInt count=0;count<limit;count++)
		{
		/*
		Initialise the justification parameters to the original values at the start of each pass.
		This means that any number of passes have exactly the same effect on the parameters as one pass;
		that is, to reduce them by the numbers of items and pixels consumed.
		*/
		iCharJustExcess = charjustexcess;
		iCharJustNum = charjustnum;
		iWordJustExcess = wordjustexcess;
		iWordJustNum = wordjustnum;

		iClipRect=(*iDefaultRegionPtr)[count];
		if(!iClipRect.Intersects(boxcpy))
			continue;
#if defined(_DEBUG)
		TRect deviceRect;
		drawDevice->GetDrawRect(deviceRect);
		BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);
#endif
		iClipRect.Intersection(boxcpy);
		/*
		Set up the parameter block for character positioning.
		Draw left to right, because although the text is being drawn vertically,
		it is done by rotating the baseline 90 degrees and drawing in the ordinary way, not by drawing
		the characters in their normal orientation but in a vertical column.
		*/
		CFont::TPositionParam param;
		param.iText.Set(aText);
		param.iPen = coords;
		TInt endDraw = aText.Length();
		if (aParam)
			{
			param.iPosInText = aParam->iStart;
			endDraw = Min(aText.Length(),aParam->iEnd);
			}
		else
			param.iPosInText = 0;

		// Draw the text.
		DoDrawTextVertical(param,fontaddress,aUp,endDraw);

		drawDevice->UpdateRegion(iClipRect);
		}
	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);
	if(iAutoUpdateJustification)
		UpdateJustificationVertical(aText,aParam,aUp);
	}
