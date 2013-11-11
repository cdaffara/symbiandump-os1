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
#include <bmalphablend.h>
#include <bitdrawinterfaceid.h>
#include <graphics/fbsrasterizer.h>
#include "bitgcextradata.h"
#include <graphics/gdi/gdiconsts.h>
#include <graphics/gdi/gdistructs.h>


/** Draws text at the last print position.

@param aText The text string to be drawn. */
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText)
	{
	TTextParameters* param = NULL;
	DrawText(aText,param,iLastPrintPosition,ELeft,CFont::EHorizontal);
	}

/** Draws text at the specified position and updates the print position.

@param  aText The text string to be drawn
@param aCoords Coordinates to draw the text at. */
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TPoint& aCoords)
	{
	TTextParameters* param = NULL;
	DrawText(aText,param,aCoords,ELeft,CFont::EHorizontal);
	}


/** Draws text clipped to the specified rectangle.

@param aText The text string to be drawn 
@param aBox The clipping rectangle. */
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TRect& aBox)
	{
	TTextParameters* param = NULL;
	DrawText(aText,param,aBox);
	}


/** Draws text clipped to the specified rectangle using a baseline offset,
horizontal alignment and a margin.

@param  aText The text string to be drawn
@param aBox The clipping rectangle.
@param  aBLOff An offset in pixels for the baseline from the normal position
        (bottom of the rectangle minus the descent of the font).
@param  aHrz Horizontal alignment option relative to the specified rectangle.
@param aMargin Offset to add to the position as calculated using specified 
		rectangle. */	
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TRect& aBox,TInt aBLOff,
								  TTextAlign aHrz,TInt aMargin)
    {
    TTextParameters* param = NULL;
	DrawText(aText,param,aBox,aBLOff,-1,aHrz,aMargin);
	}

/** Draws text clipped to the specified rectangle.

@param  aText The text string to be drawn
@param aBox The clipping rectangle. */	
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TRect& aBox,
								  TInt aBLOff,TInt /*aTextWidth*/,
								  TTextAlign aHrz,TInt aMargin)
    {
// aTextWidth is not used here - try to abolish this - I think it is unneeded
	TPoint p(aBox.iTl);
	p.iY += aBLOff;
	switch (aHrz)
		{
		case ELeft: p.iX += aMargin; break;
		case ERight: p.iX = aBox.iBr.iX - aMargin; break;
		case ECenter: p.iX += aBox.Width() / 2 + aMargin; break;
		}
	TTextParameters* param = NULL;
	DrawText(aText,param,p,aHrz,CFont::EHorizontal,&aBox);
	}


/** The general DrawText routine that implements all the others.

@param aText the text to be drawn
@param aPosition the origin of the text
@param aAlignment left, centred or right, around aPosition; not used if drawing vertically
@param aDirection direction: left to right, right to left, or top to bottom
@param aBox if non-null, filled before the text is drawn
@param aTextBounds if non-null returns the bounding box of the text
*/ 
void CFbsBitGc::DrawText(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition,TTextAlign aAlignment,
						 CFont::TTextDirection aDirection,const TRect* aBox)
   {
	// sanity checks
	BG_ASSERT_ALWAYS(iDevice,EBitgdiPanicNoDevicePresent);
	BG_ASSERT_ALWAYS(iFont.Handle() != 0,EBitgdiPanicNoFontSelected);
	const CBitmapFont* bitmap_font = iFont.Address();
	BG_ASSERT_ALWAYS(bitmap_font != 0,EBitgdiPanicNoFontSelected);

	// anything to do?
	if (!aBox && !aText.Length())
		return;

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	TRect deviceRect;
	drawDevice->GetDrawRect(deviceRect);

	// measure the text
	CFont::TMeasureTextInput measure_text_input;
	measure_text_input.iCharJustNum = iCharJustNum;
	measure_text_input.iCharJustExcess = iCharJustExcess;
	measure_text_input.iWordJustNum = iWordJustNum;
	measure_text_input.iWordJustExcess = iWordJustExcess;
	measure_text_input.iFlags |= CFont::TMeasureTextInput::EFVisualOrder;
	if (aParam)
		{
		BG_ASSERT_ALWAYS(aParam->iStart < aParam->iEnd ,EBitgdiPanicInvalidParameter);
		measure_text_input.iStartInputChar = aParam->iStart;
		measure_text_input.iEndInputChar = Min(aText.Length(),aParam->iEnd);
		}
	CFont::TMeasureTextOutput measure_text_output;
	const int advance = iFont.MeasureText(aText,&measure_text_input,&measure_text_output);
	TRect text_bounds = measure_text_output.iBounds;
	TInt underlineStrikeoutOffset;
	
	//this call is only needed in the case of linked fonts
	//should this pass by one structure?
	underlineStrikeoutOffset= BaselineCorrection();
	
	if (iUnderline == EUnderlineOn)
		{
		TInt underline_top = 0, underline_bottom = 0;
		TInt err = GetUnderlineMetrics(underline_top,underline_bottom);
		underline_top+=underlineStrikeoutOffset;
		underline_bottom+=underlineStrikeoutOffset;
		BG_ASSERT_DEBUG(err == KErrNone, EBitgdiPanicInvalidArg);
		text_bounds.iTl.iY = Min(text_bounds.iTl.iY,underline_top);
		text_bounds.iBr.iY = Max(text_bounds.iBr.iY,underline_bottom);
		}
	if (iStrikethrough == EStrikethroughOn)
		{
		TInt strike_top = 0,strike_bottom = 0;
		GetStrikethroughMetrics(strike_top,strike_bottom);
		strike_top+=underlineStrikeoutOffset;
		strike_bottom+=underlineStrikeoutOffset;
		text_bounds.iTl.iY = Min(text_bounds.iTl.iY,strike_top);
		text_bounds.iBr.iY = Max(text_bounds.iBr.iY,strike_bottom);
		}
	if (iUnderline == EUnderlineOn || iStrikethrough == EStrikethroughOn)
		{
		if (aDirection == CFont::EHorizontal)
			{
			text_bounds.iTl.iX = Min(text_bounds.iTl.iX, 0);
			text_bounds.iBr.iX = Max(text_bounds.iBr.iX, advance);
			}
		else
			{
			text_bounds.iTl.iY = Min(text_bounds.iTl.iY, 0);
			text_bounds.iBr.iY = Max(text_bounds.iBr.iY, advance);
			}
		}

	// work out the text origin and new drawing position
 	TPoint text_origin = aPosition;
 	if (aDirection != CFont::EVertical)
 		{
 		const TInt leftSideBearing = Min(text_bounds.iTl.iX, 0);
 		const TInt rightSideBearing = Max(text_bounds.iBr.iX, advance);
 		switch (aAlignment)
 			{
			// We are forbidding side-bearings to leak over the sides here,
			// but still keeping the start and end pen positions within bounds.
			case ELeft:
				text_origin.iX -= leftSideBearing;
				break;
			case ERight:
				text_origin.iX -= rightSideBearing;
				break;
			case ECenter:
				// Centre is the average of left and right
				text_origin.iX -= (leftSideBearing + rightSideBearing) >> 1;
				break;
			default:
				break;
 			}
		}
	iLastPrintPosition = text_origin;
	if (aDirection == CFont::EHorizontal)
		iLastPrintPosition.iX += advance;
	else
		iLastPrintPosition.iY += advance;
	text_origin.iY += bitmap_font->iAlgStyle.iBaselineOffsetInPixels;
	text_bounds.Move(text_origin);
	text_origin += iOrigin;

	// determine clipping rectangle
	TRect clip_rect = aBox ? *aBox : text_bounds;
	AddRect(clip_rect);
	clip_rect.Move(iOrigin);

	if (UserClipRect(clip_rect))
		{
		if (iAutoUpdateJustification)
			UpdateJustification(aText,aParam);
		return; // nothing to do
		}

	SetupDevice();
	iDevice->DrawingBegin(&iBrushBitmap);
	CFbsRasterizer* brushRasterizer = PrepareRasterizerForExtendedBitmap(iBrushBitmap);

	// fill the box if necessary
	if (aBox)
		{
		TRect fill_box = *aBox;
		fill_box.Move(iOrigin);
		RectFill(fill_box);
		}

	// decide which drawing routine to call
	const TBool bold = bitmap_font->iAlgStyle.IsBold();
	const TBool italic = bitmap_font->iAlgStyle.IsItalic();
	const TBool multiw = bitmap_font->iAlgStyle.WidthFactor() > 1;
	const TBool multih = bitmap_font->iAlgStyle.HeightFactor() > 1;

	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	const TInt maxwidth = metrics.MaxWidth();
	// extext will be TRUE, if font is bold/italic/underline/strikethrough/anti-aliased or it 
	// has shadow/outline effects ON. Depending on these properties it will call the proper draw routine.
	TBool extext = FALSE;
	TBool normaltext = FALSE;
	const TBool anti_aliased = (bitmap_font->GlyphBitmapType() == EAntiAliasedGlyphBitmap);
	const TBool outlineAndShadow = (bitmap_font->GlyphBitmapType() == EFourColourBlendGlyphBitmap);
	if (anti_aliased || outlineAndShadow)
		{
		if ((outlineAndShadow) && !(iBrushStyle == ENullBrush || iBrushStyle == ESolidBrush))
			{
			//For future compatibility it is better if brush style of ENullBrush or ESolidBrush is used 
			//when drawing outline and shadow fonts.
			BG_PANIC_ALWAYS(EBitgdiPanicInvalidBrushStyle);
			}
		extext = TRUE;
		}
	else if (maxwidth > 30 || text_bounds.Height() > 32 || multiw || multih)
		{
		}
	else if (iUnderline == EUnderlineOn || bold || italic || iStrikethrough == EStrikethroughOn || 
			 iCharJustNum > 0 || iWordJustNum > 0)
		extext = TRUE;
	else
		normaltext = TRUE;

	const TInt charjustexcess = iCharJustExcess;
	const TInt charjustnum = iCharJustNum;
	const TInt wordjustexcess = iWordJustExcess;
	const TInt wordjustnum = iWordJustNum;

	// draw the text to all clip rectangles in turn
	int clip_rects = iDefaultRegionPtr->Count();
	for (int i = 0; i < clip_rects; i++)
		{
		iClipRect = (*iDefaultRegionPtr)[i];
		if (!iClipRect.Intersects(clip_rect))
			continue;

#ifdef _DEBUG
		BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);
#endif

		iClipRect.Intersection(clip_rect);

		// Set up the parameter block for character positioning.
		CFont::TPositionParam param;
		param.iDirection = static_cast<TInt16>(aDirection);
		param.iText.Set(aText);
		TInt endDraw = aText.Length();
		if (aParam)
			{
			param.iPosInText = aParam->iStart;
			endDraw = Min(aText.Length(),aParam->iEnd);
			}
		else
			param.iPosInText = 0;
		param.iPen = text_origin;

		// Draw the text.
		if (normaltext)
			DoDrawText(param,endDraw);
		else if (extext)
			DoDrawTextEx(param,bitmap_font,endDraw,underlineStrikeoutOffset);
		else
			DoDrawTextLarge(param,bitmap_font,endDraw);
		
		/*
		Reset the justification parameters to their original values.
		These will be updated as required later in code.
		*/
		iCharJustExcess = charjustexcess;
		iCharJustNum = charjustnum;
		iWordJustExcess = wordjustexcess;
		iWordJustNum = wordjustnum;

		drawDevice->UpdateRegion(iClipRect);
		}

	if (brushRasterizer)
		{
		brushRasterizer->EndBitmap(iBrushBitmap.SerialNumber());
		}
	iDevice->DrawingEnd(&iBrushBitmap);
	if (iAutoUpdateJustification)
		UpdateJustification(aText,aParam);
	}

void CFbsBitGc::DoDrawText(CFont::TPositionParam& aParam, const TInt aEnd)
	{
	//const int n = aParam.iText.Length();
	RShapeInfo shapeInfo;
	while (aParam.iPosInText < /*n*/aEnd)
		{
		if (iFont.GetCharacterPosition2(aParam, shapeInfo))
			{
			const CFont::TPositionParam::TOutput* output = aParam.iOutput;
			for (int i = 0; i < aParam.iOutputGlyphs; i++, output++)
				DoDrawCharacter(output->iBounds.iTl,output->iBitmapSize,output->iBitmap);
			}
		}
	if (shapeInfo.IsOpen())
		shapeInfo.Close();
		
	}

void CFbsBitGc::DoDrawCharacter(const TPoint& aTopLeft,
								const TSize& aDataSize,
								const TUint8* aData)
	{
	/*
	Divert if the character is larger than expected; the criterion
	for choosing this function is only a heuristic, because it's perfectly legal for
	a character's bitmap to be wider than its escapement.

	Use a dummy value (0) for semi-ascent because this character is not italic and so semi-ascent
	is irrelevant; it's used for pseudo-italic slanting.
	*/
	TInt dataheight = aDataSize.iHeight;
	TInt datalength = aDataSize.iWidth;
	if (datalength > 30 || dataheight > 32)
		{
		DoDrawCharacterExLarge(aTopLeft,aDataSize,aData,FALSE,FALSE,0,1,1);
		return;
		}
	
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32 binarydata[32];
	TUint32* binarydataptr=binarydata;
	TUint32* binarydataptrlimit;
	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		binarydataptrlimit=binarydata+charline+repeatcount;
		if(multilineflag)
			{
			while(binarydataptr<binarydataptrlimit)
				{
				TInt chardataoffsetptr=TInt(aData)+(bitindex>>3);
				TUint32* chardataword=(TUint32*)(chardataoffsetptr&~3);
				TInt bitshift=bitindex&7;
				bitshift+=(chardataoffsetptr&3)<<3;
				*binarydataptr=(*chardataword++)>>bitshift;
				if(bitshift) *binarydataptr|=(*chardataword<<(32-bitshift));
				bitindex+=datalength;
				binarydataptr++;
				}
			}
		else
			{
			TInt chardataoffsetptr=TInt(aData)+(bitindex>>3);
			TUint32* chardataword=(TUint32*)(chardataoffsetptr&~3);
			TInt bitshift=bitindex&7;
			bitshift+=(chardataoffsetptr&3)<<3;
			TUint32 data=(*chardataword++)>>bitshift;
			if(bitshift) data|=(*chardataword<<(32-bitshift));
			while(binarydataptr<binarydataptrlimit)
				*binarydataptr++=data;
			bitindex+=datalength;
			}
		}
	TPoint topleft(aTopLeft);
	binarydataptr=ClipBinaryArray(binarydata,binarydata+dataheight,1,datalength,dataheight,topleft);
	if(datalength>0 && dataheight>0)
		iDevice->iDrawDevice->WriteBinary(topleft.iX,topleft.iY,binarydataptr,datalength,dataheight,iPenColor,iDrawMode);
	}

/** 
@internalTechnology

This function retrieves the baseline offset from the metrics of the text currently being drawn.
This is used to alter the positioning of underline and strikethrough on a linked font

@return The baseline correction associated with the currently used font.
*/	
TInt CFbsBitGc::BaselineCorrection()
	{
	TOpenFontMetrics metrics;
	if (iFont.GetFontMetrics(metrics))
		return metrics.BaselineCorrection();
	else
		return 0;
	}

void CFbsBitGc::DoDrawTextEx(CFont::TPositionParam& aParam,const CBitmapFont* font, const TInt aEnd, const TInt aUnderlineStrikethroughOffset)
	{
	const TInt charclipping = iClipRect.iBr.iX;
	TPoint start_pen = aParam.iPen;
	const TBool bold = font->iAlgStyle.IsBold();
	const TBool italic = font->iAlgStyle.IsItalic();
	const TGlyphBitmapType glyphBitmapTypeForFont = font->GlyphBitmapType();
	const TBool outlineShadowOrAntialiased = ((glyphBitmapTypeForFont == EAntiAliasedGlyphBitmap) ||
												(glyphBitmapTypeForFont == EFourColourBlendGlyphBitmap));

	TInt underline_top = 0, underline_bottom = 0;
	if (iUnderline == EUnderlineOn)
		{
		TInt err = GetUnderlineMetrics(underline_top,underline_bottom);
		underline_top+=aUnderlineStrikethroughOffset;
		underline_bottom+=aUnderlineStrikethroughOffset;
		BG_ASSERT_DEBUG(err == KErrNone, EBitgdiPanicInvalidArg);
		}
	TInt strike_top = 0, strike_bottom = 0;
	if (italic || iStrikethrough == EStrikethroughOn)
		{
		GetStrikethroughMetrics(strike_top,strike_bottom);
		strike_top+=aUnderlineStrikethroughOffset;
		strike_bottom+=aUnderlineStrikethroughOffset;
		}

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	RShapeInfo shapeInfo;
	while (aParam.iPosInText < aEnd)
		{
		if (!iFont.GetCharacterPosition2(aParam, shapeInfo))
			continue;
		TInt adjustment = 0;
		if(iCharJustExcess && iCharJustNum > 0) // character clipping/justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iCharJustExcess,iCharJustNum);
			if(adjustment < 0)
				iClipRect.iBr.iX = Min(aParam.iPen.iX + adjustment,iClipRect.iBr.iX);
			}

		const CFont::TPositionParam::TOutput* output = aParam.iOutput;
		int semi_ascent = start_pen.iY + strike_top;

		for (int i = 0; i < aParam.iOutputGlyphs; i++, output++)
			{
			//get the character metrics for the glyph type
			TOpenFontCharMetrics characterParams;
			const TUint8* bitmap;
			TSize size;
			//note may now be using a glyph code, and not a character
			iFont.GetCharacterData(aParam.iOutput[i].iCode,characterParams,bitmap,size);
			TGlyphBitmapType glyphType = characterParams.GlyphType();
			
			switch (glyphType)
				{
				case EAntiAliasedGlyphBitmap:
				case EFourColourBlendGlyphBitmap:
					DoDrawCharacterAntiAliased(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,glyphType);
					break;

				case EDefaultGlyphBitmap:
				case EMonochromeGlyphBitmap:
					DoDrawCharacterEx(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,bold,italic,semi_ascent);
					break;

				default:
					//if the outline or shadow is not specified for the character, then use the font setting
					if (outlineShadowOrAntialiased)
						{
						DoDrawCharacterAntiAliased(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,glyphBitmapTypeForFont);			
						}
					else
						{
						DoDrawCharacterEx(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,bold,italic,semi_ascent);
						}			 
					break;
				}
			}

		iClipRect.iBr.iX = charclipping;
		if (adjustment)
			aParam.iPen.iX += adjustment;
		if (iWordJustExcess > 0 && iWordJustNum > 0 && aParam.iOutput[0].iCode == 0x0020) // word justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iWordJustExcess,iWordJustNum);
			aParam.iPen.iX += adjustment;
			}
		}
	if (shapeInfo.IsOpen())
		shapeInfo.Close();

	if (iUnderline == EUnderlineOn)
		{
		TRect ul(start_pen.iX,start_pen.iY + underline_top,aParam.iPen.iX,start_pen.iY + underline_bottom);
		if(ul.Intersects(iClipRect)) // checks for empty aRect as well
			{
			ul.Intersection(iClipRect);
			drawDevice->WriteRgbMulti(ul.iTl.iX,ul.iTl.iY,ul.iBr.iX-ul.iTl.iX,ul.iBr.iY-ul.iTl.iY,iPenColor,iDrawMode);
			}
		}

	if (iStrikethrough == EStrikethroughOn)
		{
		TRect st(start_pen.iX,start_pen.iY + strike_top,aParam.iPen.iX,start_pen.iY + strike_bottom);
		if(st.Intersects(iClipRect)) // checks for empty aRect as well
			{
			st.Intersection(iClipRect);
			drawDevice->WriteRgbMulti(st.iTl.iX,st.iTl.iY,st.iBr.iX-st.iTl.iX,st.iBr.iY-st.iTl.iY,iPenColor,iDrawMode);
			}
		}
	}

void CFbsBitGc::DoDrawCharacterEx(const TPoint& aTopLeft,
								  const TSize& aDataSize,
								  const TUint8* aData,
								  TBool aBold,TBool aItalic,TInt aSemiAscent)
	{
	/*
	Divert if the character is larger than expected; the criterion
	for choosing this function is only a heuristic, because it's perfectly legal for
	a character's bitmap to be wider than its escapement.
	*/
	TInt datalength = aDataSize.iWidth;
	TInt dataheight = aDataSize.iHeight;
	if (datalength > 30 || dataheight > 32)
		{
		DoDrawCharacterExLarge(aTopLeft,aDataSize,aData,aBold,aItalic,aSemiAscent,1,1);
		return;
		}

	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32 binarydata[32];
	TUint32* binarydataptr=binarydata;
	TUint32* binarydataptrlimit;
	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		binarydataptrlimit=binarydata+charline+repeatcount;
		if(multilineflag)
			{
			while(binarydataptr<binarydataptrlimit)
				{
				CopyCharWord(binarydataptr,aData+(bitindex>>3),bitindex&7);
				bitindex+=datalength;
				binarydataptr++;
				}
			}
		else
			{
			TUint32 data=0;
			CopyCharWord(&data,aData+(bitindex>>3),bitindex&7);
			while(binarydataptr<binarydataptrlimit)
				*binarydataptr++=data;
			bitindex+=datalength;
			}
		}
	binarydataptr=binarydata;
	binarydataptrlimit=binarydata+dataheight;
	if(aBold)
		{
		TInt sparemask=(0xffffffff>>(32-datalength));
		while(binarydataptr<binarydataptrlimit)
			{
			*binarydataptr&=sparemask;
			*binarydataptr|=(*binarydataptr<<1);
			++binarydataptr;
			}
		binarydataptr=binarydata;
		datalength++;
		}
	if(aItalic)
		{
		TInt skewlevel=aSemiAscent-aTopLeft.iY;
		TUint32 sparemask=(0xffffffff>>(32-datalength));
		binarydataptrlimit=binarydata+skewlevel;
		while(binarydataptr<binarydataptrlimit)
			*binarydataptr++<<=1;
		binarydataptrlimit=binarydata+dataheight;
		while(binarydataptr<binarydataptrlimit)
			*binarydataptr++&=sparemask;
		binarydataptr=binarydata;
		datalength++;
		}
	TPoint topleft(aTopLeft);
	binarydataptr=ClipBinaryArray(binarydata,binarydata+dataheight,1,datalength,dataheight,topleft);
	if(datalength>0 && dataheight>0)
		iDevice->iDrawDevice->WriteBinary(topleft.iX,topleft.iY,binarydataptr,datalength,dataheight,iPenColor,iDrawMode);
	}

/** 
@internalTechnology

This function draws an anti-aliased character.

@param aTopLeft The position to output the character
@param aDataSize The size of the bitmap
@param aData the bitmap for outputt in their TOpenFontCharMetrics
@param aGlyphType the glyph type for the character
*/
void CFbsBitGc::DoDrawCharacterAntiAliased(const TPoint& aTopLeft,
										   const TSize& aDataSize,
										   const TUint8* aData,
										   const TGlyphBitmapType aGlyphType)
	{
	const int top_row = Max(0,iClipRect.iTl.iY - aTopLeft.iY);
	const int bottom_row = Min(aDataSize.iHeight,iClipRect.iBr.iY - aTopLeft.iY);
	const int left_col = Max(0,iClipRect.iTl.iX - aTopLeft.iX);
	const int right_col = Min(aDataSize.iWidth,iClipRect.iBr.iX - aTopLeft.iX);
	const TUint8* p = aData + top_row * aDataSize.iWidth + left_col;
	const int x = aTopLeft.iX + left_col;
	int y = aTopLeft.iY + top_row;
	const int cols = right_col - left_col;
	const TUint32 penColor = iPenColor.Internal();
	const TUint32 shadowColor = iFbsBitGcExtraData->ShadowColor().Internal();
	const TUint32 brushColor = iBrushColor.Internal();

	if (EFourColourBlendGlyphBitmap == aGlyphType)
		{
		MOutlineAndShadowBlend* outlineAndShadow = NULL;
		const TInt err = iDevice->iDrawDevice->GetInterface(KOutlineAndShadowInterfaceID, reinterpret_cast <TAny*&> (outlineAndShadow));
		if(err == KErrNone) 
			{
			//There is a support for the interface with KOutlineAndShadowInterface id.
			for (int row = top_row; row < bottom_row; row++, p += aDataSize.iWidth, y++)
				outlineAndShadow->WriteRgbOutlineAndShadow(x, y, cols, penColor, shadowColor, brushColor, p);
			}
		else
			{
			// Assert if MOutlineAndShadowBlend interface is not implemented
			BG_ASSERT_DEBUG(outlineAndShadow, EBitgdiPanicInvalidInterfaceHandle);
			}
		}
	else
		{
		for (int row = top_row; row < bottom_row; row++, p += aDataSize.iWidth, y++)
			iDevice->iDrawDevice->WriteRgbAlphaMulti(x,y,cols,iPenColor,p);
		}
	}

void CFbsBitGc::DoDrawTextLarge(CFont::TPositionParam& aParam,const CBitmapFont* font,const TInt aEnd)
	{
	const TInt charclipping = iClipRect.iBr.iX;
	TPoint start_pen = aParam.iPen;
	const TInt strikeheight = start_pen.iY - (font->CBitmapFont::DoAscentInPixels() * 5/12) - 1;
	const TBool bold = font->iAlgStyle.IsBold();
	const TBool italic = font->iAlgStyle.IsItalic();
	const TInt widthfactor=font->iAlgStyle.WidthFactor();
	const TInt heightfactor=font->iAlgStyle.HeightFactor();

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

	RShapeInfo shapeInfo;
	while (aParam.iPosInText < aEnd)
		{
		if (!iFont.GetCharacterPosition2(aParam, shapeInfo))
			continue;
		TInt adjustment = 0;
		if(iCharJustExcess && iCharJustNum > 0) // character clipping/justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iCharJustExcess,iCharJustNum);
			if(adjustment < 0)
				iClipRect.iBr.iX = Min(aParam.iPen.iX + adjustment,iClipRect.iBr.iX);
			}

		const CFont::TPositionParam::TOutput* output = aParam.iOutput;
		for (int i = 0; i < aParam.iOutputGlyphs; i++, output++)
			DoDrawCharacterLarge(output->iBounds.iTl,output->iBitmapSize,output->iBitmap,bold,italic,strikeheight,
								 widthfactor,heightfactor);

		iClipRect.iBr.iX = charclipping;
		if (adjustment)
			aParam.iPen.iX += adjustment;
		if (iWordJustExcess > 0 && iWordJustNum > 0 && aParam.iOutput[0].iCode == 0x0020) // word justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iWordJustExcess,iWordJustNum);
			aParam.iPen.iX += adjustment;
			}
		}
	if (shapeInfo.IsOpen())
		shapeInfo.Close();
	const TInt ulwidth = Max(font->CBitmapFont::DoHeightInPixels() / 10, 1);
	if (iUnderline == EUnderlineOn)
		{
		TInt ulstart = start_pen.iY + 1 + ulwidth / 2;
		TRect ul(start_pen.iX,ulstart,aParam.iPen.iX,ulstart + ulwidth);
		if(ul.Intersects(iClipRect)) // checks for empty aRect as well
			{
			ul.Intersection(iClipRect);
			drawDevice->WriteRgbMulti(ul.iTl.iX,ul.iTl.iY,ul.iBr.iX-ul.iTl.iX,ul.iBr.iY-ul.iTl.iY,iPenColor,iDrawMode);
			}
		}

	if (iStrikethrough == EStrikethroughOn)
		{
		TRect st(start_pen.iX,strikeheight,aParam.iPen.iX,strikeheight + ulwidth);
		if(st.Intersects(iClipRect)) // checks for empty aRect as well
			{
			st.Intersection(iClipRect);
			drawDevice->WriteRgbMulti(st.iTl.iX,st.iTl.iY,st.iBr.iX-st.iTl.iX,st.iBr.iY-st.iTl.iY,iPenColor,iDrawMode);
			}
		}
	}

void CFbsBitGc::DoDrawCharacterLarge(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
									 TBool aBold,TBool aItalic,TInt aSemiAscent,TInt aWidthFactor,TInt aHeightFactor)
	{
	TInt datalength = aDataSize.iWidth;
	TInt dataheight = aDataSize.iHeight;
	
	if (aWidthFactor > 2 || aHeightFactor > 2)
		{
		DoDrawCharacterMultiplied(aTopLeft,aDataSize,aData,aBold,aItalic,aSemiAscent,aWidthFactor,aHeightFactor);
		return;
		}
	
	if (datalength > 30 || dataheight > 32 || (aWidthFactor == 2 && datalength > 14))
		{
		DoDrawCharacterExLarge(aTopLeft,aDataSize,aData,aBold,aItalic,aSemiAscent,aWidthFactor,aHeightFactor);
		return;
		}
	
	TInt italicheight=aSemiAscent-aTopLeft.iY;
	if(aHeightFactor==2) italicheight>>=1;
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32 binarydata[64];
	TUint32* binarydataptr=binarydata;
	TUint32* binarydataptrlimit;

	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		binarydataptrlimit=binarydata+charline+repeatcount;
		if(multilineflag)
			{
			while(binarydataptr<binarydataptrlimit)
				{
				CopyCharWord(binarydataptr,aData+(bitindex>>3),bitindex&7);
				bitindex+=datalength;
				binarydataptr++;
				}
			}
		else
			{
			TUint32 data=0;
			CopyCharWord(&data,aData+(bitindex>>3),bitindex&7);
			while(binarydataptr<binarydataptrlimit)
				*binarydataptr++=data;
			bitindex+=datalength;
			}
		}
	binarydataptr=binarydata;
	binarydataptrlimit=binarydata+dataheight;
	if(aBold)
		{
		TInt sparemask=(0xffffffff>>(32-datalength));
		while(binarydataptr<binarydataptrlimit)
			{
			*binarydataptr&=sparemask;
			*binarydataptr|=(*binarydataptr<<1);
			++binarydataptr;
			}
		binarydataptr=binarydata;
		datalength++;
		}
	if(aItalic)
		{
		TUint32 sparemask=(0xffffffff>>(32-datalength));
		binarydataptrlimit=Min(binarydataptrlimit,binarydata+italicheight);
		while(binarydataptr<binarydataptrlimit)
			{
			*binarydataptr<<=1;
			*binarydataptr++&=0xfffffffe;
			}
		binarydataptrlimit=binarydata+dataheight;
		while(binarydataptr<binarydataptrlimit)
			*binarydataptr++&=sparemask;
		binarydataptr=binarydata;
		datalength++;
		}
	if(aWidthFactor==2)
		{
		BG_ASSERT_DEBUG(datalength<=16,EBitgdiPanicCharacterTooBig);
		while(binarydataptr<binarydataptrlimit)
			{
			TUint32 singlemask=0x8000;
			TUint32 doublemask=0xc0000000;
			TUint32 newdata=0;
			while(singlemask)
				{
				if(*binarydataptr&singlemask)
					newdata|=doublemask;
				singlemask>>=1;
				doublemask>>=2;
				}
			*binarydataptr++=newdata;
			}
		datalength<<=1;
		binarydataptr=binarydata;
		}
	if(aHeightFactor==2)
		{
		binarydataptr=binarydata+dataheight-1;
		TUint32* tempptr=binarydataptr+dataheight;
		while(binarydataptr>=binarydata)
			{
			*tempptr--=*binarydataptr;
			*tempptr--=*binarydataptr--;
			}
		dataheight<<=1;
		binarydataptr=binarydata;
		}
	TPoint startpos=aTopLeft;
	binarydataptr=ClipBinaryArray(binarydata,binarydata+dataheight,1,datalength,dataheight,startpos);
	if(datalength>0 && dataheight>0)
		iDevice->iDrawDevice->WriteBinary(startpos.iX,startpos.iY,binarydataptr,datalength,dataheight,iPenColor,iDrawMode);
	}

void CFbsBitGc::DoDrawCharacterExLarge(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
									   TBool aBold,TBool aItalic,TInt aSemiAscent,TInt aWidthFactor,TInt aHeightFactor)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	TPoint printpos(aTopLeft);
	const TInt datalength = aDataSize.iWidth;
	const TInt dataheight = aDataSize.iHeight;
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32* slbuffer=drawDevice->ScanLineBuffer();
	const TInt slwords=(drawDevice->ScanLineBytes())<<3;
	if(aItalic && aTopLeft.iY<aSemiAscent)
		printpos.iX++;
	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		const TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		if(multilineflag)
			{
			for(TInt currentline=0;currentline<repeatcount;currentline++)
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
				OutputCharLineMultiplied(printpos,slbuffer,datalength,1,aBold,aWidthFactor,aHeightFactor);
				bitindex+=datalength;
				printpos.iY++;
				if(printpos.iY==aSemiAscent && aItalic) printpos.iX--;
				if(aHeightFactor==2)
					{
					printpos.iY++;
					if(printpos.iY==aSemiAscent && aItalic) printpos.iX--;
					}
				}
			}
		else
			{
			if(aItalic)
				{
				for(TInt currentline=0;currentline<repeatcount;currentline++)
					{
					CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
					OutputCharLineMultiplied(printpos,slbuffer,datalength,1,aBold,aWidthFactor,aHeightFactor);
					printpos.iY++;
					if(printpos.iY==aSemiAscent && aItalic) printpos.iX--;
					if(aHeightFactor==2)
						{
						printpos.iY++;
						if(printpos.iY==aSemiAscent && aItalic) printpos.iX--;
						}
					}
				}
			else
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
				OutputCharLineMultiplied(printpos,slbuffer,datalength,repeatcount,aBold,aWidthFactor,aHeightFactor);
				printpos.iY+=repeatcount;
				if(aHeightFactor==2) printpos.iY+=repeatcount;
				}
			bitindex+=datalength;
			}
		}
	}

void CFbsBitGc::DoDrawCharacterMultiplied(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
										  TBool aBold,TBool aItalic,TInt aSemiAscent,TInt aWidthFactor,TInt aHeightFactor)
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	TPoint printpos(aTopLeft);
	const TInt datalength = aDataSize.iWidth;
	const TInt dataheight = aDataSize.iHeight;
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32* slbuffer=drawDevice->ScanLineBuffer();
	const TInt slwords=(drawDevice->ScanLineBytes())<<3;
	if(aItalic && aTopLeft.iY<aSemiAscent)
		printpos.iX++;
	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		const TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		if(multilineflag)
			{
			for(TInt currentline=0;currentline<repeatcount;currentline++)
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
				OutputCharLineMultiplied(printpos,slbuffer,datalength,1,aBold,aWidthFactor,aHeightFactor);
				bitindex+=datalength;
				TBool aboveitalicjump=EFalse;
				if(aItalic && printpos.iY<aSemiAscent) aboveitalicjump=ETrue;
				printpos.iY+=aHeightFactor;
				if(aboveitalicjump && printpos.iY>=aSemiAscent) printpos.iX-=aWidthFactor;
				}
			}
		else
			{
			if(aItalic)
				{
				for(TInt currentline=0;currentline<repeatcount;currentline++)
					{
					CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
					OutputCharLineMultiplied(printpos,slbuffer,datalength,1,aBold,aWidthFactor,aHeightFactor);
					TBool aboveitalicjump=EFalse;
					if(printpos.iY<aSemiAscent) aboveitalicjump=ETrue;
					printpos.iY+=aHeightFactor;
					if(aboveitalicjump && printpos.iY>=aSemiAscent) printpos.iX-=aWidthFactor;
					}
				}
			else
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength);
				OutputCharLineMultiplied(printpos,slbuffer,datalength,repeatcount,aBold,aWidthFactor,aHeightFactor);
				printpos.iY+=repeatcount*aHeightFactor;
				}
			bitindex+=datalength;
			}
		}
	}

void CFbsBitGc::OutputCharLineMultiplied(TPoint aPrintPos,TUint32* aBuffer,TInt aDataLength,TInt aNum,TBool aBold,TInt aWidthFactor,TInt aHeightFactor)
	{
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
	if(aPrintPos.iX<iClipRect.iTl.iX)
		{
		TInt pixelexcess=iClipRect.iTl.iX-aPrintPos.iX;
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
		aPrintPos.iX=iClipRect.iTl.iX;
		}
	if(aPrintPos.iX+aDataLength>iClipRect.iBr.iX)
		{
		TInt pixelexcess=aPrintPos.iX+aDataLength-iClipRect.iBr.iX;
		aDataLength-=pixelexcess;
		if(aDataLength<=0) return;
		}
	aNum*=aHeightFactor;
	while(aNum>0)
		{
		if(aPrintPos.iY>=iClipRect.iTl.iY && aPrintPos.iY<iClipRect.iBr.iY)
			iDevice->iDrawDevice->WriteBinaryLine(aPrintPos.iX,aPrintPos.iY,aBuffer,aDataLength,iPenColor,iDrawMode);
		aPrintPos.iY++;
		aNum--;
		}
	}

void CFbsBitGc::CopyCharWord(TUint32* aBinaryDataPtr,const TUint8* aData,TInt aBitShift)
	{
	const TUint32* dataword=(TUint32*)(TInt(aData)&~3);
	aBitShift+=(TInt(aData)-TInt(dataword))<<3;
	*aBinaryDataPtr=*dataword++;
	if(aBitShift<32) *aBinaryDataPtr>>=aBitShift;
	if(aBitShift) *aBinaryDataPtr|=(*dataword<<(32-aBitShift));
	}
	
void CFbsBitGc::CopyCharLine(TUint32* aBinaryDataPtr,TInt aBufferWords,const TUint8* aData,TInt aBitShift,TInt aCharWidth)
	{
	aBitShift&=7;
	TInt wordstocopy=(aCharWidth+31)>>5;
	if(wordstocopy>aBufferWords) wordstocopy=aBufferWords;
	TUint32* ptrlimit=aBinaryDataPtr+wordstocopy;
	TUint32* dataword=(TUint32*)(TInt(aData)&~3);
	aBitShift+=(TInt(aData)-TInt(dataword))<<3;
	while(aBinaryDataPtr<ptrlimit)
		{
		*aBinaryDataPtr=*dataword++;
		*aBinaryDataPtr>>=aBitShift;
		if(aBitShift) *aBinaryDataPtr|=(*dataword<<(32-aBitShift));
		aBinaryDataPtr++;
		}
	}
	
TUint32* CFbsBitGc::ClipBinaryArray(TUint32* aArray,TUint32* aArrayLimit,TInt aArrayWordWd,TInt& aDataWd,TInt& aDataHt,TPoint& aPos)
	{
	TUint32* arrayptr=aArray;
	TInt clipdiff=iClipRect.iTl.iX-aPos.iX;
	if(aArrayWordWd==1)
		{
		if(clipdiff>0)
			{
			while(arrayptr<aArrayLimit)
				*arrayptr++>>=clipdiff;
			aDataWd-=clipdiff;
			aPos.iX=iClipRect.iTl.iX;
			arrayptr=aArray;
			}
		if(aPos.iX+aDataWd>iClipRect.iBr.iX && aDataWd>0)
			aDataWd=iClipRect.iBr.iX-aPos.iX;
		clipdiff=iClipRect.iTl.iY-aPos.iY;
		if(clipdiff>0)
			{
			aDataHt-=clipdiff;
			arrayptr+=clipdiff;
			aPos.iY=iClipRect.iTl.iY;
			}
		if(aPos.iY+aDataHt>iClipRect.iBr.iY && aDataHt>0)
			aDataHt=iClipRect.iBr.iY-aPos.iY;
		}
	return(arrayptr);
	}

void CFbsBitGc::BitMultiply(TUint32* aBinaryDataPtr,TInt aBitLength,TInt aFactor)
	{
	TInt bitpos=aBitLength-1;
	TUint32* wordpos=aBinaryDataPtr+(bitpos>>5);
	TInt bitoffset=bitpos&0x1f;
	TInt multbitpos=(aBitLength*aFactor)-1;
	TUint32* multwordpos=aBinaryDataPtr+(multbitpos>>5);
	TInt multbitoffset=multbitpos&0x1f;
	while(bitpos>=0)
		{
		TUint32 bit=((*wordpos)>>bitoffset)&1;
		TInt next=multbitpos-aFactor;
		while(multbitpos>next)
			{
			if(bit) *multwordpos|=(1<<multbitoffset);
			else *multwordpos&=~(1<<multbitoffset);
			multbitpos--;
			multbitoffset--;
			if(multbitoffset<0)
				{
				multbitoffset=31;
				multwordpos--;
				}
			}
		bitpos--;
		bitoffset--;
		if(bitoffset<0)
			{
			bitoffset=31;
			wordpos--;
			}
		}
	}

void CFbsBitGc::GetStrikethroughMetrics(TInt& aTop,TInt& aBottom)
/** Get the top and bottom of a strikethrough line for the current font, relative to the baseline.*/
	{
	aTop = -(iFont.AscentInPixels() * 5/12) - 1;
	aBottom = aTop + Max(iFont.HeightInPixels() / 10,1);
	}

/** APIExtension can contain as many additional methods as is required by 
CGraphicsContext after its original conception. It takes 3 parameters.
Function is exported due to constrains of retaining BC with earlier versions.
This is not used directly by external methods, instead it is called by a named 
method in CGraphicsContext which passes the relivant arguements including an 
unique identifier for the required action.
@param aUid The unique identifier for the method that is required. Selected 
internally by a series of "if" statements. 
@see Valid Uid identifiers are listed in header gdi.h
@see CGraphicsContext
@param aOutput is a TAny pointer to a reference. Used to output data as the structure
does not need to be instantiated before the function call this adds greater 
flexibility.
@param aInput is a TAny pointer used to input data.
*/	
EXPORT_C TInt CFbsBitGc::APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput)
	{
	if (aUid == KGetUnderlineMetrics)
		{		
		APIExGetUnderlineMetrics(aOutput);
		return KErrNone;	
		}
	else if (aUid == KSetShadowColor)
		{
		return APIExSetShadowColor(aInput);
		}
	else if (aUid == KGetShadowColor)
		{
		return APIExGetShadowColor(aOutput);
		}
	else if (aUid == KDrawTextInContextUid)
		{
		TDrawTextInContextInternal* contextParam = (TDrawTextInContextInternal*)aInput;
		DrawText(contextParam->iText, &contextParam->iParam, contextParam->iPosition);
		return KErrNone;
		}
	else if (aUid == KDrawBoxTextInContextUid)
		{
		TDrawTextInContextInternal* contextParam = (TDrawTextInContextInternal*)aInput;
		DrawText(contextParam->iText,&contextParam->iParam,contextParam->iBox,contextParam->iBaselineOffset,contextParam->iAlign,contextParam->iMargin);
		return KErrNone;
		}
	else if (aUid == KDrawTextInContextVerticalUid)
		{
		TDrawTextInContextInternal* contextParam = (TDrawTextInContextInternal*)aInput;
		DrawTextVertical(contextParam->iText, &contextParam->iParam, contextParam->iPosition,contextParam->iUp);
		return KErrNone;
		}
	else if (aUid == KDrawBoxTextInContextVerticalUid)
		{
		TDrawTextInContextInternal* contextParam = (TDrawTextInContextInternal*)aInput;
		DrawTextVertical(contextParam->iText,&contextParam->iParam,contextParam->iBox,contextParam->iBaselineOffset,contextParam->iUp,contextParam->iAlign,contextParam->iMargin);
		return KErrNone;
		}
	else if (aUid == KUidIsFbsBitmapGc)
		{
		return APIExIsFbsBitGc(aOutput);
		}
	/* Future cases may be placed here later.*/
	else
		return CBitmapContext::APIExtension(aUid, aOutput, aInput);
	}
	
	
	
	
		
	
	
	
	
//The methods listed above in APIExtension follow here with the prefix APIEx.
void CFbsBitGc::APIExGetUnderlineMetrics(TAny*& aOutput)
	{
	const TInt width = Max(iFont.HeightInPixels() / 10,1);
	TTwoTInt* ptr = (TTwoTInt*)aOutput;
	ptr->iTop = 1 + width / 2;
	ptr->iBottom = (ptr->iTop) + width;
	}

TInt CFbsBitGc::APIExSetShadowColor(TAny* aShadowColor)
	{
	iFbsBitGcExtraData->SetShadowColor(*(reinterpret_cast<TRgb*>(aShadowColor)));
	return KErrNone;
	}

TInt CFbsBitGc::APIExGetShadowColor(TAny*& aShadowColor)
	{
	TRgb* output = reinterpret_cast<TRgb*>(aShadowColor);
	*output = iFbsBitGcExtraData->ShadowColor();
	return KErrNone;
	}
	
TInt CFbsBitGc::APIExIsFbsBitGc(TAny*& aIsCFbsBitGc)
	{
	TBool *output = reinterpret_cast<TBool*>(aIsCFbsBitGc);
	*output = ETrue;
	return KErrNone;
	}

EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TTextParameters* aParam)
	{
	DrawText(aText,aParam,iLastPrintPosition,ELeft,CFont::EHorizontal);
	}

EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition)
	{
	DrawText(aText,aParam,aPosition,ELeft,CFont::EHorizontal);
	}
	
	
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox)
	{
	TRect boxcpy(aBox);
	boxcpy.Move(iOrigin);
	if(!iUserClipRect.Intersects(boxcpy)) return;
	TRect oldcliprect(iUserClipRect);
	iUserClipRect.Intersection(boxcpy);
	DrawText(aText,aParam);
	iUserClipRect=oldcliprect;
	}
	
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHrz,TInt aMargin)
	{
	DrawText(aText,aParam,aBox,aBaselineOffset,-1,aHrz,aMargin);
	}
	
EXPORT_C void CFbsBitGc::DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TInt /*aTextWidth*/,TTextAlign aHrz,TInt aMargin)
	{
	// aTextWidth is not used here - try to abolish this - I think it is unneeded
	TPoint p(aBox.iTl);
	p.iY += aBaselineOffset;
	switch (aHrz)
		{
		case ELeft: p.iX += aMargin; break;
		case ERight: p.iX = aBox.iBr.iX - aMargin; break;
		case ECenter: p.iX += aBox.Width() / 2 + aMargin; break;
		}
	DrawText(aText,aParam,p,aHrz,CFont::EHorizontal,&aBox);
	}
