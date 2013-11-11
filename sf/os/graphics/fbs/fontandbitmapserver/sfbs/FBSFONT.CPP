// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <fbs.h>
#include <openfont.h>
#include <graphics/shapeimpl.h>
#include "UTILS.H"
#include <graphics/shaperparams.h>
#include "FbsMessage.H"
#include <graphics/gdi/gdiconsts.h>
#include <graphics/gdi/gdistructs.h>

GLREF_C void Panic(TFbsPanic aPanic);

/** Helper function for converting a pointer to an offset from the passed
heap base. Use OffsetToPointer() to convert the returned offset back to a
useable pointer.
@param aAny A pointer to be converted to an offset.
@param aHeapBase A pointer to the heap base of the current process.
@return An offset representing the passed pointer that can be converted
back to a pointer using the function OffsetToPointer(). 
@see OffsetToPointer()
 */
LOCAL_C TInt PointerToOffset(const TAny* aAny, TUint8* aHeapBase)
	{
	if (aAny && aHeapBase)
		{
		return reinterpret_cast<TInt>(aAny) - reinterpret_cast<TInt>(aHeapBase);
		}
	return 0;
	}

/** Helper function for converting an offset (that was calculated using
PointerToOffset()) back to a pointer relative to the passed heap base.
@param aOffset The offset to be converted to a pointer.
@param aHeapBase A pointer to the heap base of the current process.
@return A pointer relative to the passed heap base.
@see PointerToOffset()
 */
LOCAL_C TAny* OffsetToPointer(TInt aOffset, TUint8* aHeapBase)
	{
	if (aOffset && aHeapBase)
		{
		return reinterpret_cast<TAny*>(aOffset + reinterpret_cast<TInt>(aHeapBase));
		}
	return NULL;
	}

EXPORT_C CFbsFont::CFbsFont():
	CFont(),
	iFbs(RFbsSession::GetSession()),
	iAddressPointer(NULL),
	iHandle(0),
	iServerHandle(0)
	{
	}
	
EXPORT_C CFbsFont::CFbsFont(const CFbsFont& aFont):
	CFont(),
	iFbs(aFont.iFbs),
	iAddressPointer(NULL),
	iHandle(0),
	iServerHandle(0)
	{
	}
	
EXPORT_C CFbsFont::~CFbsFont()
	{
	Reset();
	}
	
EXPORT_C void CFbsFont::Reset()
	{
	if (iHandle)
	    {
		iFbs->SendCommand(EFbsMessClose,iHandle);
	    }
	iHandle = 0;
	}
	
EXPORT_C CBitmapFont* CFbsFont::Address() const
	{
	__ASSERT_DEBUG(iHandle != NULL,Panic(EFbsFontAddressViolation));
	__ASSERT_DEBUG(iAddressPointer != NULL,Panic(EFbsFontAddressViolation));
	return iAddressPointer;
	}

/** Duplicates a font.
This function does not create a copy of the font. It just assigns another 
handle to the bitmap in the font and bitmap server, and sets this object's 
handle to that.

@param aFontHandle The handle to an existing CFbsFont.
@return KErrNone if successful; KErrCouldNotConnect if no connection to the 
font and bitmap server could be made; KErrUnknown if no font could be found 
with the specified handle number.
@publishedAll
@released
*/
EXPORT_C TInt CFbsFont::Duplicate(TInt aFontHandle)
	{
	if (!iFbs)
		return KErrCouldNotConnect;
	if (!aFontHandle)
		return KErrUnknown;
	// close any existing handle
	Reset();	
	// ask server to create the duplicate handle
	TPckgBuf<TFontInfo> tfpckg;
	TIpcArgs args(aFontHandle,&tfpckg);
	TInt ret = iFbs->SendCommand(EFbsMessFontDuplicate,args);
	if (ret != KErrNone || !tfpckg().iHandle)
		return ret;
	// created
	iHandle = tfpckg().iHandle;
	iServerHandle = tfpckg().iServerHandle;
	iAddressPointer = (CBitmapFont*)(iFbs->HeapBase()+tfpckg().iAddressOffset);
	return KErrNone;
	}

/** Gets the Font and Bitmap server handle of the font.
@return The handle of the font. 
@publishedAll
@released
*/
EXPORT_C TInt CFbsFont::Handle() const
	{
	if (!iHandle)
		{
		return 0;
		}
	return iServerHandle;
	}

/** Gets how much of the specified descriptor can be displayed in this font without 
exceeding the specified width.

Note:
This function does not display any of the descriptor itself.  It is used 
before display, to test whether the whole descriptor can be displayed.
@param aText The descriptor. 
@param aWidthInPixels The available width for character display 
@return The number of characters (starting from the beginning of the descriptor) 
which will be able to be displayed without exceeding the specified width. 
@see CFont::TextCount() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoTextCount(const TDesC& aText,TInt aWidthInPixels) const
	{
	TInt dummy;
	return DoTextCount(aText, aWidthInPixels, dummy);
	}

/** Gets how much of the specified descriptor can be displayed in this font without 
exceeding the specified width.
It also returns the excess width defined as the specified available width 
minus the width of the portion of the descriptor which can be displayed without 
exceeding the available width.
@param aText The descriptor. 
@param aWidthInPixels The available width for character display. 
@param aExcessWidthInPixels The excess width after displaying the portion of 
the descriptor, in pixels. 
@return The number of characters (starting from the beginning of the descriptor) 
which will be able to be displayed without exceeding the specified width. 
@see CFont::TextCount()
@see TextCount() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoTextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidth) const
	{
	TMeasureTextInput input;
	input.iMaxAdvance = aWidthInPixels;
	TMeasureTextOutput output;
	aExcessWidth = aWidthInPixels - MeasureText(aText,&input,&output);
	return output.iChars;
	}

/** Gets the width of the specified character in this font, in pixels.

Note: For OpenType fonts this function returns the horizontal advance of
the character, which may be different from the actual width.

@param aChar The character whose width should be determined. 
@return The width of the specified character in this font, in pixels. 
@see CFont::CharWidthInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoCharWidthInPixels(TChar aChar) const
	{
	TOpenFontCharMetrics metrics;
	const TUint8* bitmap;
	TSize size;
	if (GetCharacterData(aChar,metrics,bitmap,size) != ENoCharacterData)
		{
		return metrics.HorizAdvance();
		}
	return 0;
	}

/** Gets the width of the specified descriptor when displayed in this font, in 
pixels.
@param aText The descriptor whose width should be determined. 
@return The width of the specified descriptor when displayed in this font, 
in pixels 
@see CFont::TextWidthInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoTextWidthInPixels(const TDesC& aText) const
	{
	TMeasureTextInput* dummy = NULL;
	return DoTextWidthInPixels(aText,dummy);
	}

/** Gets the width of the specified descriptor when displayed in this font, in 
pixels.
@param aText The descriptor whose width should be determined. 
@param aParam Parameter block that controls how much of aText is measured
@return The width of the specified descriptor when displayed in this font, 
in pixels 
*/
TInt CFbsFont::DoTextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam) const
	{
	TMeasureTextOutput output;
	TInt advance_width = MeasureText(aText,aParam,&output);
	return Max(advance_width,output.iBounds.Width());
	}

/** Gets the width of the specified descriptor when displayed in this font, in 
pixels. Override of the base class to resolve name clash with other
TextWidthInPixels variant.
@param aText The descriptor whose width should be determined. 
@return The width of the specified descriptor when displayed in this font, 
in pixels 
@see CFont::TextWidthInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::TextWidthInPixels(const TDesC& aText) const
	{
	return DoTextWidthInPixels(aText);
	}

/** Gets the width of the specified descriptor when displayed in this font, in 
pixels. Override of the base class to resolve name clash with other
TextWidthInPixels variant.
@param aText The descriptor whose width should be determined. 
@param aParam Parameter block that controls how much of aText is measured
@return The width of the specified descriptor when displayed in this font, 
in pixels 
@see CFont::TextWidthInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::TextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam) const
	{
	return DoTextWidthInPixels(aText,aParam);
	}

/** Gets the text width, move and adjusts of the specified descriptor when displayed 
in this font.
@param aText The descriptor whose width should be determined. 
@param aCharWidth The width of the specified descriptor when displayed in this 
font, in pixels (including information on the width, move and adjusts of the 
descriptor). 
@publishedAll 
@released
*/
EXPORT_C void CFbsFont::TextWidthInPixels(const TDesC& aText,SCharWidth& aCharWidth) const
	{
	TMeasureTextInput* dummy = NULL;
	TextWidthInPixels(aText,dummy,aCharWidth);
	}

/** Gets the text width, move and adjusts of the specified descriptor when displayed 
in this font.
@param aText The descriptor whose width should be determined. 
@param aParam Parameter block that controls how much of aText is measured
@param aCharWidth The width of the specified descriptor when displayed in this 
font, in pixels (including information on the width, move and adjusts of the 
descriptor). 
@publishedAll 
@released
*/
EXPORT_C void CFbsFont::TextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam, SCharWidth& aCharWidth) const
	{
	TMeasureTextOutput output;
	aCharWidth.iMove = MeasureText(aText,aParam,&output);
	aCharWidth.iLeftAdjust = output.iBounds.iTl.iX;
	aCharWidth.iRightAdjust = aCharWidth.iMove - output.iBounds.iBr.iX;
	aCharWidth.iWidth = output.iBounds.Width();
	}

/** Gets the raw width of the text in the descriptor, in pixels. 
 DEPRECATED: Same as MeasureText(const TDesC&).
This is the width of the text without adjusting for side bearings, algorithmic 
style etc.
@deprecated
@param aText Any text descriptor (TPtrC, TPtr, _LIT, TBuf etc.). 
@return The width (in pixels) of the text in the descriptor. */
EXPORT_C TInt CFbsFont::RawTextWidthInPixels(const TDesC& aText) const
	{
	return MeasureText(aText);
	}

/** Gets the baseline offset, in pixels.
The offset is how far a font is raised or lowered from its normal baseline.
@return Offset from normal baseline, in pixels. 
@see CFont::BaselineOffsetInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoBaselineOffsetInPixels() const
	{
	if (!iHandle)
		return 0;
	return Address()->iAlgStyle.iBaselineOffsetInPixels;
	}

/** Gets the width of the widest character in this font, in pixels.
@return The width of the maximum width character, in pixels. 
@see CFont::MaxCharWidthInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoMaxCharWidthInPixels() const
	{
	if (!iHandle)
		return 0;
	TInt width = Address()->CBitmapFont::DoMaxCharWidthInPixels();
	if (Address()->iAlgStyle.IsBold())
		width += Address()->iAlgStyle.WidthFactor();
	return width;
	}

/** Gets the width of the widest normal character in this font, in pixels.
Normal characters include all character in a character set except non-alphabetic 
characters (e.g. the copyright symbol, or a block graphics symbol, for example).
@return The width of the maximum width normal character, in pixels. 
@see CFont::MaxNormalCharWidthInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoMaxNormalCharWidthInPixels() const
	{
	if (!iHandle)
		return 0;
	TInt width = Address()->CBitmapFont::DoMaxNormalCharWidthInPixels();
	if (Address()->iAlgStyle.IsBold())
		width += Address()->iAlgStyle.WidthFactor();
	return width;
	}

/** Gets the font height in pixels.
@return The font height in pixels.
@see CFont::HeightInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoHeightInPixels() const
	{
	if (!iHandle)
		return 0;
	return Address()->CBitmapFont::DoHeightInPixels();
	}

/** Gets the font ascent in pixels.
@return The font ascent in pixels.
@see CFont::AscentInPixels() 
@publishedAll 
@released
*/
EXPORT_C TInt CFbsFont::DoAscentInPixels() const
	{
	if (!iHandle)
		return 0;
	return Address()->CBitmapFont::DoAscentInPixels();
	}

/** Gets the font specification of this font in twips.
@return The font specification of this font (in twips). 
@see CFont::FontSpecInTwips() 
@publishedAll 
@released
*/
EXPORT_C TFontSpec CFbsFont::DoFontSpecInTwips() const
	{
	TFontSpec fs;
	if (!iHandle)
		return fs;
	fs = Address()->CBitmapFont::DoFontSpecInTwips();
	TPckgBuf<TInt> tfpckg;
	TIpcArgs args(iHandle,&tfpckg);
	TInt ret = iFbs->SendCommand(EFbsMessGetTwipsHeight,args);
	fs.iHeight = tfpckg();
	return fs;
	}

/** Gets the character metrics and a pointer to the compressed glyph bitmap for 
the specified character. 
This function is deprecated, because TCharacterMetrics cannot store metrics 
larger than 127 or less than 127  use GetCharacterData() instead.
@param aCode The code for the character to be checked. 
@param aBytes On return, contains a pointer to the compressed glyph bitmap. 
@return The character metrics for the font. 
@publishedAll 
@released
@deprecated
*/
EXPORT_C TCharacterMetrics CFbsFont::CharacterMetrics(TInt aCode,const TUint8*& aBytes) const
	{
	TCharacterMetrics metrics;
	// Save time by not converting from TCharacterMetrics to TOpenFontCharMetrics and back if this is a real bitmap font.
	if (iHandle)
		{	
 		CBitmapFont* bitmap_font = Address();

 		if (!bitmap_font->IsOpenFont())
			metrics = bitmap_font->CharacterMetrics(aCode,aBytes);
		else
			{
			TOpenFontCharMetrics new_metrics;
			aBytes = NULL;
			TSize size;
			if (GetCharacterData(aCode,new_metrics,aBytes,size) != ENoCharacterData)
				new_metrics.GetTCharacterMetrics(metrics);
			}
		}
	return metrics;
	}

/** Gets the character metrics and the glyph bitmap. 
@param aCode The character code in Unicode. 
@param aMetrics On return, contains the character metrics. 
@param aBitmap On return, contains a pointer to the compressed glyph bitmap. 
@param aBitmapSize The size of the returned glyph bitmap in pixels. This is 
not necessarily the same as the size implied by the returned metrics, which 
may incorporate algorithmic multiplication. 
@publishedAll 
@released
*/
EXPORT_C CFont::TCharacterDataAvailability CFbsFont::DoGetCharacterData(TUint aCode,TOpenFontCharMetrics& aMetrics,
		const TUint8*& aBitmap,TSize& aBitmapSize) const
	{
	aBitmap = NULL;
	if (!iHandle)
		return CFont::ENoCharacterData;

	CBitmapFont* bitmap_font = Address();

	if (!bitmap_font->GetCharacterData(iFbs->ServerSessionHandle(),aCode,aMetrics,aBitmap))
		{
		TPckgBuf<TRasterizeParams> paramsBuf;
		TIpcArgs args(iHandle, aCode, &paramsBuf);
		
		if(iFbs->SendCommand(EFbsMessRasterize, args))
			{				
			// Translate the offsets sent to the server back to pointers relative to
			// the heap base of the current process
			const TOpenFontCharMetrics* metrics = (const TOpenFontCharMetrics*)OffsetToPointer(paramsBuf().iMetricsOffset, iFbs->HeapBase());
			if (metrics)
				{
				aMetrics = *metrics;
				}
			aBitmap = static_cast<TUint8*>(OffsetToPointer(paramsBuf().iBitmapPointerOffset, iFbs->HeapBase()));			
			}
		else
			{
			return CFont::ENoCharacterData;
			}
		}

	aBitmapSize.SetSize(aMetrics.Width(),aMetrics.Height());

	if (!bitmap_font->IsOpenFont())
		{
		TAlgStyle null_style;
		if (!(bitmap_font->iAlgStyle == null_style))
			{
			const int width_factor = bitmap_font->iAlgStyle.WidthFactor();
			const int height_factor = bitmap_font->iAlgStyle.HeightFactor();
			const int bold_addition =	bitmap_font->iAlgStyle.IsBold() ? width_factor : 0;
			const int italic_addition = bitmap_font->iAlgStyle.IsItalic() ? width_factor : 0;

			aMetrics.SetWidth(aMetrics.Width() * width_factor + bold_addition + italic_addition);
			aMetrics.SetHeight(aMetrics.Height() * height_factor);
			aMetrics.SetHorizBearingX(aMetrics.HorizBearingX() * width_factor);
			aMetrics.SetHorizBearingY(aMetrics.HorizBearingY() * height_factor);
			aMetrics.SetVertBearingX(aMetrics.VertBearingX() * width_factor);
			aMetrics.SetVertBearingY(aMetrics.VertBearingY() * height_factor);
			if (bitmap_font->iAlgStyle.IsMono())
				aMetrics.SetHorizAdvance(bitmap_font->CBitmapFont::DoMaxNormalCharWidthInPixels() + bold_addition);
			else
				aMetrics.SetHorizAdvance(aMetrics.HorizAdvance() * width_factor + bold_addition);
			aMetrics.SetVertAdvance(aMetrics.VertAdvance() * height_factor);
			}
		}
	return CFont::EAllCharacterData;
	}

/** Gets the open font metrics. If the metrics cannot be obtained the function 
returns EFalse.
@param aMetrics On return, contains the font metrics 
@return EFalse if the metrics cannot be obtained 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsFont::GetFontMetrics(TOpenFontMetrics& aMetrics) const
	{
	if (iHandle)
		{
		CBitmapFont* bitmap_font = Address();
		bitmap_font->GetFontMetrics(aMetrics);
		return TRUE;
		}
	else
		return FALSE;
	}

/** Gets the typeface attributes of Open Font System fonts.
Notes: 
Typeface attributes are different from the font metrics; they are not metrics, 
which are different for every different size, but size-independent attributes 
of the typeface, like name and style. 
This function can be used if IsOpenFont() returns true i.e. the font is 
an Open Font.
@param aAttrib On return, contains the typeface attributes. 
@return EFalse if the attributes cannot be obtained, or if the font is not an 
Open Font (IsOpenFont() returns EFalse). 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsFont::GetFaceAttrib(TOpenFontFaceAttrib& aAttrib) const
	{
	if (!iHandle)
		{
		return EFalse;
		}
	TPckgBuf<TOpenFontFaceAttrib> package;
	TIpcArgs args(iHandle,&package);
	if (iFbs->SendCommand(EFbsMessFaceAttrib,args))
		{
		aAttrib = package();
		return ETrue;
		}
	return EFalse;
	}

/** Tests whether the font is an Open Font system font.
Note: 
If this function returns ETrue, the function GetFaceAttrib() will work.
@return ETrue if font is an Open Font system font (e.g. TrueType). EFalse if 
the font is a bitmap font loaded from a GDR file. 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsFont::IsOpenFont() const
	{
	if (iHandle)
		{
		CBitmapFont* bitmap_font = Address();
		return bitmap_font->IsOpenFont();
		}
	else
		return FALSE;
	}

/** Tests whether the font contains a particular character.
@param aCode Character code to be tested. This code is in the code page 1252 
encoding in v5, otherwise it is in Unicode 
@return ETrue if the font contains aCode. 
@publishedAll 
@released
*/
EXPORT_C TBool CFbsFont::HasCharacter(TInt aCode) const
	{
	if (iHandle)
		{
		return iFbs->SendCommand(EFbsMessHasCharacter,iHandle,aCode);
		}
	return EFalse;
	}


/** help DoExtendedFunction to perform KFontGetShaping function
@param aParam Input & output parameter block, 
if successful aParam->iShapeHeaderOutput points to the shape data.
@return KErrNone if successful, otherwise a system wide error code.
*/
TInt CFbsFont::DoFontGetShaping(TFontShapeFunctionParameters* aParam) const
	{
	if (!iHandle)
		{
		return KErrGeneral;
		}
	TPckgBuf<TShapeMessageParameters> sp;
	sp().iStart = aParam->iStart;
	sp().iEnd = aParam->iEnd;
	sp().iScript = aParam->iScript;
	sp().iLanguage = aParam->iLanguage;

	TInt offset = iFbs->SendCommand( EFbsMessShapeText,TIpcArgs(iHandle, aParam->iText, &sp));

	// Convert the returned offset to pointer relative to the heap base of the current process
	aParam->iShapeHeaderOutput = reinterpret_cast<const TShapeHeader*>(OffsetToPointer(offset, iFbs->HeapBase()));
	return aParam->iShapeHeaderOutput? KErrNone : KErrGeneral;
	}


/** help DoExtendedFunction to perform KFontDeleteShaping function
@param aParam Input parameter block
@return KErrNone if successful, KErrGeneral if the font does not have a valid handle.
*/
TInt CFbsFont::DoFontDeleteShaping(TFontShapeDeleteFunctionParameters* aParam) const
	{
	if (!iHandle)
		{
		return KErrGeneral;
		}
	// Convert the address of the shape header to an offset from the heap base
	// of this process before the offset is sent to the server
	iFbs->SendCommand(EFbsMessShapeDelete,iHandle,PointerToOffset(aParam->iShapeHeader, iFbs->HeapBase()));
	return KErrNone;
	}


TInt CFbsFont::DoGetFontTable(TGetFontTableParam* aParam) const
    {
    TInt ret = KErrGeneral;

    TPckgBuf<TOffsetLen> retBuf;
    ret = iFbs->SendCommand(EFbsMessGetFontTable, 
            TIpcArgs(iHandle, aParam->iTag, &retBuf));
    
    if (KErrNone == ret)
        {
        aParam->iLength = retBuf().iLen;
        aParam->iContent = OffsetToPointer(retBuf().iOffset, iFbs->HeapBase());
        }
    return ret;
    }


TInt CFbsFont::DoGetGlyphOutline(TGetGlyphOutlineParam* aParam) const
    {
    TInt ret = KErrGeneral;
    
    TPckgBuf<TFBSGlyphOutlineParam> paramsBuf;
    TInt count = aParam->iCount;
    paramsBuf().iCount = aParam->iCount;
    paramsBuf().iHinted = aParam->iHinted;
    paramsBuf().iHandle = iHandle;

    TOffsetLen* offsetLen = (TOffsetLen *)User::Alloc(count * sizeof(TOffsetLen));
    if (NULL == offsetLen)
        {
        return KErrNoMemory;
        }
    TPtr8 retBuf((TUint8 *)offsetLen, count * sizeof(TOffsetLen),
            count * sizeof(TOffsetLen));
    TPtr8 codes((TUint8 *)(aParam->iCodes), count * sizeof(TUint), 
            count * sizeof(TUint));
    
    ret = iFbs->SendCommand( EFbsMessGetGlyphOutline, 
            TIpcArgs(&paramsBuf, &codes, &retBuf));
    
    if (KErrNone == ret)
        {
        // server writes the offsets back to client, convert them
        // to local pointers.
        for (TInt i = 0; i < aParam->iCount; ++i)
            {
            aParam->iOutlines[i] = OffsetToPointer(offsetLen[i].iOffset, 
                    iFbs->HeapBase());
            aParam->iLengths[i] = offsetLen[i].iLen;
            }
        }
    User::Free(offsetLen);
    return ret;
    }

TInt CFbsFont::DoReleaseGlyphOutline(TReleaseGlyphOutlineParam* aParam) const 
    {
    TInt ret = KErrGeneral;
    
    TPckgBuf<TFBSGlyphOutlineParam> params;
    TInt count = aParam->iCount;
    params().iCount = count;
    params().iHinted = aParam->iHinted;
    params().iHandle = iHandle;

    TPtr8 codes((unsigned char *)aParam->iCodes, count * sizeof(TUint), count * sizeof(TUint));
    
    ret = iFbs->SendCommand(EFbsMessReleaseGlyphOutline, 
            TIpcArgs(&params, &codes));
        
    return ret;
    }   

TInt CFbsFont::DoReleaseFontTable(TUint32* aParam) const 
    {
    TInt ret = KErrGeneral;
    
    ret = iFbs->SendCommand(EFbsMessReleaseFontTable, 
            TIpcArgs(iHandle, *aParam));
        
    return ret;
    }   
/** API extension system that enables the caller to access a particular API
extension function. As an overload of this function in a derived class
it calls its immediate parent implementation for any extension function Uid
that it does not recognize and handle.
@param aInterfaceId UID of the required extension function
@param aParam Pointer to an arbitrary parameter block that can be used to
provide and/or return information to/from the particular extension function,
defaults to NULL.
@return Integer return value from extension function, a system wide error code.
@panic FBSCLI 31, in debug builds only, if iExtra is NULL when it must not be.
@panic FBSCLI 38, in debug builds only, if a reserved error code is returned 
	from an extended function.
@internalTechnology
@released
*/
EXPORT_C TInt CFbsFont::DoExtendedFunction(TUid aFunctionId, TAny* aParam) const
	{
	if (iHandle)
		{
		if (aFunctionId == KFontGetShaping)
			{
			return DoFontGetShaping(reinterpret_cast<TFontShapeFunctionParameters*>(aParam));
			}
		else if (aFunctionId == KFontDeleteShaping)
			{
			return DoFontDeleteShaping(reinterpret_cast<TFontShapeDeleteFunctionParameters*>(aParam));
			}
		else if ( (aFunctionId == KFontCapitalAscent)
			|| (aFunctionId == KFontMaxAscent)
			|| (aFunctionId == KFontStandardDescent)
			|| (aFunctionId == KFontMaxDescent)
			|| (aFunctionId == KFontLineGap) )
			{
			// Call the version on the CBitmapFont instance
			return Address()->CBitmapFont::DoExtendedFunction(aFunctionId, aParam);
			}
		else if (aFunctionId == KTextInContextWidthInPixelsUid)
			{
			TTextWidthInternal* contextParam = (TTextWidthInternal*)aParam;
			return DoTextWidthInPixels(contextParam->iText,&contextParam->iParam);
			}
		else if (aFunctionId == KFontGetFontTable) 
		    {
            return DoGetFontTable(reinterpret_cast<TGetFontTableParam *>(aParam));
		    }
		else if (aFunctionId == KFontGetGlyphOutline)
		    {
		    return DoGetGlyphOutline(reinterpret_cast<TGetGlyphOutlineParam *>(aParam));
		    }
		else if (aFunctionId == KFontReleaseGlyphOutline)
		    {
		    return DoReleaseGlyphOutline(reinterpret_cast<TReleaseGlyphOutlineParam *>(aParam));
		    }
		else if (aFunctionId == KFontReleaseFontTable)
            {
            return DoReleaseFontTable(reinterpret_cast<TUint32 *>(aParam));
            }
		}
	return CFont::DoExtendedFunction(aFunctionId, aParam);
	}

