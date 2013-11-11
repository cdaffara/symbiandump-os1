// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <gdi.h>
#include <bidi.h>
#include "GDIPANIC.h"
#include "gdistructs.h"
#include "gdiconsts.h"


// Global panic function

_LIT(KGdiPanicCategory,"GDI");

void Panic(TGdiPanic aError)
	{
	User::Panic(KGdiPanicCategory,aError);
	}
	
_LIT(KGDIPanicDesc1, "Gdi internal Panic %S, in file %S @ line %i");
_LIT(KGDIPanicDesc2, "Assert condition = \"%S\"");
_LIT(KGDIPanicDesc3, "Gdi internal %S, in file %S @ line %i");

void PanicWithCondAndInfo(TGdiPanic aError, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KGDIPanicDesc1, &aPanicName, &aFileName, aLine);
	RDebug::Print(buf);

	buf.Format(KGDIPanicDesc2, &aCondition);
	RDebug::Print(buf);
	Panic(aError);
	}
	
void PanicLogWithInfo(const TDesC& aCommand, const TDesC& aCondition, const TDesC& aFileName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KGDIPanicDesc3, &aCommand, &aFileName, aLine);
	RDebug::Print(buf);
	
	buf.Format(KGDIPanicDesc2, &aCondition);
	RDebug::Print(buf);
	}


//
// MGraphicsDeviceMap
//


EXPORT_C MGraphicsDeviceMap::MGraphicsDeviceMap()
/** Default constructor. */
	{}


EXPORT_C MGraphicsDeviceMap::~MGraphicsDeviceMap()
/** Destructor. */
	{}


EXPORT_C TPoint MGraphicsDeviceMap::TwipsToPixels(const TPoint& aTwipPoint) const
/** Converts a point in twips to a point in pixels.

@param aTwipPoint A point on the graphics device in twips. 
@return A point on the graphics device in pixels. */
	{
	return TPoint(HorizontalTwipsToPixels(aTwipPoint.iX),VerticalTwipsToPixels(aTwipPoint.iY));
	}


EXPORT_C TRect MGraphicsDeviceMap::TwipsToPixels(const TRect& aTwipRect) const
/** Converts a rectangle in twips to a rectangle in pixels.

@param aTwipRect A rectangle on the graphics device in twips 
@return A rectangle on the graphics device in pixels. */
	{
	return TRect(TwipsToPixels(aTwipRect.iTl),TwipsToPixels(aTwipRect.iBr));
	}


EXPORT_C TPoint MGraphicsDeviceMap::PixelsToTwips(const TPoint& aPixelPoint) const
/** Converts a point in pixels to a point in twips.

@param aPixelPoint A point on the graphics device in pixels. 
@return A point on the graphics device in twips. */
	{
	return TPoint(HorizontalPixelsToTwips(aPixelPoint.iX),VerticalPixelsToTwips(aPixelPoint.iY));
	}


EXPORT_C TRect MGraphicsDeviceMap::PixelsToTwips(const TRect& aPixelRect) const
/** Converts a rectangle in pixels to a rectangle in twips.

@param aPixelRect A rectangle on the graphics device in pixels. 
@return A rectangle on the graphics device in twips. */
	{
	return TRect(PixelsToTwips(aPixelRect.iTl),PixelsToTwips(aPixelRect.iBr));
	}

//
// CGraphicsContext
//

EXPORT_C TInt CGraphicsContext::JustificationInPixels(TInt aExcessPixels,TInt aTotalUnits,TInt aFirstUnit,TInt aNumUnits)
/** Gets the amount of space in pixels by which to adjust letter or word spacing, 
given the total number of words and spaces, a start space, and the number 
of units to be adjusted.

The first two arguments are the number of pixels (character groups) and the 
number of units (word spaces) over which justification is to occur. The third 
argument specifies the current character group or word space, while the final 
argument specifies the number of units that are to be adjusted.

A panic occurs if aExcessPixels is 0, aTotalUnits is not greater than 0, or 
aFirstUnit is not less than aTotalUnits. 

@param aExcessPixels The number of pixels by which the width of the text is 
to be changed. It may be positive, in which case the text is stretched, or 
negative, in which case it is shrunk. 
@param aTotalUnits The number of word spaces over which the change in width 
is to be distributed. 
@param aFirstUnit The current unit — the character group or word space we 
are 'on'. 
@param aNumUnits The number of units that are to be adjusted — starting 
at aFirstUnit. 
@return The number of pixels to be added to the width of the current unit. 
@see SetWordJustification()
@see SetCharJustification() */
	{
	if(aExcessPixels==0 || aTotalUnits<=0 || aFirstUnit>=aTotalUnits)
		return(0);
	TInt noExtra=Abs(aExcessPixels%aTotalUnits);
	TInt extraPixel=aExcessPixels/Abs(aExcessPixels);
	GDI_ASSERT_DEBUG_GENERAL( aFirstUnit>=0 , User::Panic(KGdiPanicCategory,KErrArgument) ) ;
	GDI_ASSERT_DEBUG_GENERAL( aNumUnits>=0 , User::Panic(KGdiPanicCategory,KErrArgument) ) ;
	if(aFirstUnit+aNumUnits>aTotalUnits)
		aNumUnits=aTotalUnits-aFirstUnit;
	TInt clip=aNumUnits*(aExcessPixels/aTotalUnits);
	if(aFirstUnit>=noExtra)
		return(clip);
	if(aFirstUnit+aNumUnits>noExtra)
		aNumUnits=noExtra-aFirstUnit;
	return(clip+aNumUnits*extraPixel);
	}


EXPORT_C TInt CGraphicsContext::JustificationInPixels(TInt& aExcessPixels,TInt& aTotalUnits)
/** Gets the amount of space in pixels by which to adjust the current letter or 
word spacing, and also retrieves the number of excess pixels and word spaces 
remaining after the adjustment is performed. 

The arguments are the number of remaining pixels (character groups) and units 
(word spaces) over which justification is to occur. The function can be called 
repetitively until the number of units is zero, and hence justification is 
complete. A panic occurs if the number of units is less than one or the amount 
of pixels is zero.

@param aExcessPixels The number of pixels by which the width of the text is 
to be changed. It may be positive, in which case the text is stretched, or 
negative, in which case it is shrunk. On return, this is equal to its old 
value minus the return value. 
@param aTotalUnits The number of word spaces over which the change in width 
is to be distributed. On return, this is reduced by one. 
@return The number of pixels to be added to the width of the current unit. 
@see SetWordJustification()
@see SetCharJustification() */
	{
	GDI_ASSERT_DEBUG_GENERAL(aExcessPixels!=0,User::Panic(KGdiPanicCategory,KErrArgument));
	GDI_ASSERT_DEBUG_GENERAL(aTotalUnits>0,User::Panic(KGdiPanicCategory,KErrArgument));
	TInt justification=aExcessPixels/aTotalUnits;
	if(aExcessPixels%aTotalUnits)
		{
		if(aExcessPixels>0)
			justification++;
		else
			justification--;
		}
	aTotalUnits--;
	aExcessPixels-=justification;
	return(justification);
	}


EXPORT_C TInt CGraphicsContext::DrawTextExtended(const TDesC& aText,const TPoint& aPosition,
												 const TDrawTextExtendedParam& aParam)
/** Draws text, optionally changing its direction (right-to-left / left-to-right).

Apart from reordering the text, the function is the same as the two parameter 
variant of DrawText(), described above.

@param aText The text string to be drawn, optionally changing its direction 
(right-to-left / left-to-right).
@param aPosition A point specifying the position of the left end of the text.
@param aParam Indicates whether the text should be drawn from right-to-left 
(for scripts like Arabic and Hebrew) or left-to-right.
@return KErrNoMemory indicates there was an OOM error when reordering the text; 
KErrNone if the reordering was successful. */
	{
	// Reorder the text bidirectionally.
	TText* reordered_text = NULL;
	int error = TBidirectionalState::ReorderText(aText.Ptr(),aText.Length(),aParam.iParRightToLeft,reordered_text);
	TPtrC p(reordered_text,aText.Length());
	DrawText(p,aPosition,aParam);
	if (reordered_text != aText.Ptr())
		delete [] reordered_text;
	return error;
	}


EXPORT_C void CGraphicsContext::DrawText(const TDesC& aText,const TPoint& aPosition,const TDrawTextParam& /*aParam*/)
/** Draws the specified text at the given position using the parameters supplied.

@param   aText  The text to be drawn.
@param  aPosition The position to draw the text at.
@param   aParam Parameters to use for text drawing. */	
	{
	DrawText(aText,aPosition);
	}
	
/*
Can be used to find out the top and bottom of an underline for the active font.
This allows correct calculation of the area required in which to draw text with underline.

@param TInt& aTop The top of the underline position
@param TInt& aBottom The bottom of the underline position
@return TInt KErrNone if successful, else a standard system wide error value. 
*/
EXPORT_C TInt CGraphicsContext::GetUnderlineMetrics(TInt& aTop,TInt& aBottom)
	{

	TTwoTInt outputPackage;
	TTwoTInt* outputPtr = &outputPackage;
	TInt err = APIExtension(KGetUnderlineMetrics, (TAny*&) outputPtr, NULL);
	aTop = outputPackage.iTop;
	aBottom = outputPackage.iBottom;
	return err;
	}

EXPORT_C TInt CGraphicsContext::SetShadowColor(const TRgb& aShadowColor)
	{
	TRgb shadowColor = aShadowColor;
	TInt *dummy = NULL;
	return APIExtension(KSetShadowColor, (TAny*&)dummy, (TAny*)&shadowColor);
	}

EXPORT_C TInt CGraphicsContext::GetShadowColor(TRgb& aShadowColor)
	{
	TRgb* shadowColor = &aShadowColor;
	return APIExtension(KGetShadowColor, (TAny*&)shadowColor, NULL);
	}

EXPORT_C TBool CGraphicsContext::IsFbsBitGc() const
	{
	TBool isFbsBitGc=EFalse;
	TBool* isFbsBitGcPtr=&isFbsBitGc;

	//Have a non const this since want the published API to be const
	CGraphicsContext *nonConstThis = const_cast<CGraphicsContext*>(this);

	//The API extension function is non-const, and this is const function
	TInt err= nonConstThis->APIExtension(KUidIsFbsBitmapGc, (TAny*&)isFbsBitGcPtr, NULL);

	//on error, return EFalse
	return (!err ? isFbsBitGc : EFalse);
	}

EXPORT_C void CGraphicsContext::DrawText(const TDesC& aText,const TTextParameters* iParam,const TPoint& aPosition)
	{
	TInt *dummy = NULL;
	
	TDrawTextInContextInternal context;
	TDrawTextInContextInternal* contextPtr = &context;
	contextPtr->iText.Set(aText);
	contextPtr->iPosition.SetXY(0,0);
	contextPtr->iPosition += aPosition;
	contextPtr->iParam.iStart = iParam->iStart;
	contextPtr->iParam.iEnd = iParam->iEnd;
	if (KErrNotSupported == APIExtension(KDrawTextInContextUid, (TAny*&)dummy, (TAny*)contextPtr))
		{
		DrawText(aText,aPosition);
		}
	}

EXPORT_C void CGraphicsContext::DrawText(const TDesC& aText,const TTextParameters* iParam,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHrz,TInt aMargin)
	{
	TInt *dummy = NULL;

	TDrawTextInContextInternal context;
	TDrawTextInContextInternal* contextPtr = &context;
	contextPtr->iText.Set(aText);
	contextPtr->iBox.SetRect(aBox.iTl, aBox.iBr);
	contextPtr->iBaselineOffset = aBaselineOffset;
	contextPtr->iAlign = aHrz;
	contextPtr->iMargin = aMargin;
	contextPtr->iParam.iStart = iParam->iStart;
	contextPtr->iParam.iEnd = iParam->iEnd;
	if (KErrNotSupported == APIExtension(KDrawBoxTextInContextUid, (TAny*&)dummy, (TAny*)contextPtr))
		{
		DrawText(aText,aBox,aBaselineOffset,aHrz,aMargin);
		}
	}
	
EXPORT_C void CGraphicsContext::DrawText(const TDesC& aText,const TTextParameters* iParam,const TPoint& aPosition,const TDrawTextParam& /*aParam*/)
	{
	TInt *dummy = NULL;

	TDrawTextInContextInternal context;
	TDrawTextInContextInternal* contextPtr = &context;
	contextPtr->iText.Set(aText);
	contextPtr->iPosition.SetXY(0,0);
	contextPtr->iPosition += aPosition;
	contextPtr->iParam.iStart = iParam->iStart;
	contextPtr->iParam.iEnd = iParam->iEnd;
	if (KErrNotSupported == APIExtension(KDrawTextInContextUid, (TAny*&)dummy, (TAny*)contextPtr))
		{
		DrawText(aText,aPosition);
		}
	}
	
EXPORT_C void CGraphicsContext::DrawTextVertical(const TDesC& aText,const TTextParameters* iParam,const TPoint& aPos,TBool aUp)
	{
	TInt *dummy = NULL;

	TDrawTextInContextInternal context;
	TDrawTextInContextInternal* contextPtr = &context;
	contextPtr->iText.Set(aText);
	contextPtr->iPosition.SetXY(0,0);
	contextPtr->iPosition += aPos;
	contextPtr->iUp = aUp;
	contextPtr->iParam.iStart = iParam->iStart;
	contextPtr->iParam.iEnd = iParam->iEnd;
	if (KErrNotSupported == APIExtension(KDrawTextInContextVerticalUid, (TAny*&)dummy, (TAny*)contextPtr))
		{
		DrawTextVertical(aText,aPos,aUp);
		}
	}
	
EXPORT_C void CGraphicsContext::DrawTextVertical(const TDesC& aText,const TTextParameters* iParam,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert,TInt aMargin)
	{
	TInt *dummy = NULL;

	TDrawTextInContextInternal context;
	TDrawTextInContextInternal* contextPtr = &context;
	contextPtr->iText.Set(aText);
	contextPtr->iBox.SetRect(aBox.iTl, aBox.iBr);
	contextPtr->iBaselineOffset = aBaselineOffset;
	contextPtr->iAlign = aVert;
	contextPtr->iMargin = aMargin;
	contextPtr->iUp = aUp;
	contextPtr->iParam.iStart = iParam->iStart;
	contextPtr->iParam.iEnd = iParam->iEnd;
	if (KErrNotSupported == APIExtension(KDrawBoxTextInContextVerticalUid, (TAny*&)dummy, (TAny*)contextPtr))
		{
		DrawTextVertical(aText,aBox,aBaselineOffset,aUp,aVert,aMargin);
		}
	}

EXPORT_C TInt CGraphicsContext::DrawTextExtended(const TDesC& aText,const TTextParameters* aTextParam,const TPoint& aPosition,
												 const TDrawTextExtendedParam& aParam)
/** Draws text, optionally changing its direction (right-to-left / left-to-right).

Apart from reordering the text, the function is the same as the two parameter 
variant of DrawText(), described above.

@param aText The text string to be drawn, optionally changing its direction 
(right-to-left / left-to-right).
@param aPosition A point specifying the position of the left end of the text.
@param aParam Indicates whether the text should be drawn from right-to-left 
(for scripts like Arabic and Hebrew) or left-to-right.
@return KErrNoMemory indicates there was an OOM error when reordering the text; 
KErrNone if the reordering was successful. */
	{
	// Reorder the text bidirectionally.
	TText* reordered_text = NULL;
	int error = TBidirectionalState::ReorderText(aText.Ptr(),aText.Length(),aParam.iParRightToLeft,reordered_text);
	TPtrC p(reordered_text,aText.Length());
	DrawText(p,aTextParam,aPosition,aParam);
	if (reordered_text != aText.Ptr())
		delete [] reordered_text;
	return error;
	}
EXPORT_C void CGraphicsContext::Reserved()
/**Reserved function for future use. */	
	{
	}

/**
An API extension for CGraphics context replacing a reserved virtual method.
Effectively allows multiple methods to use just one ordinal number.

@param TUid aUid A unique identifier for the internal method required
@param TAny*& aOutput The output parameter
@param TAny* aInput The input argument. Notably not const.
@return KErrNone If a successful derived function is found, if the
default is used then KErrNotSupported is returned.
*/
EXPORT_C TInt CGraphicsContext::APIExtension(TUid /*aUid*/, TAny*& /*aOutput*/, TAny* /*aInput*/)
	{
	return KErrNotSupported;
	}

//Default implementation of reserved virtual
EXPORT_C void CGraphicsContext::Reserved_CGraphicsContext_2()
	{
	}

EXPORT_C TInt CBitmapContext::APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput)
  	{
  	return CGraphicsContext::APIExtension(aUid, aOutput, aInput);
  	}

//Default implementation of reserved virtual
EXPORT_C void CBitmapContext::Reserved_CGraphicsContext_2()
	{
	CGraphicsContext::Reserved_CGraphicsContext_2();
	}

//Default implementation of reserved virtual
EXPORT_C void CBitmapContext::Reserved_CBitmapContext_1()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CBitmapContext::Reserved_CBitmapContext_2()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CBitmapContext::Reserved_CBitmapContext_3()
	{
	}

