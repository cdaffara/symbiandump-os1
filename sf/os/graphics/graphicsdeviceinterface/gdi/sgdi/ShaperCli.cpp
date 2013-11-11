// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains client-side shaper functionality.
// 
//

#include "ShapeImpl.h"
#include "ShapeInfo.h"
#include <gdi.h>
#include "GDIPANIC.h"

GLREF_C void Panic(TInt aError);

/** Construct an RShapeInfo. */
/** @internalComponent */
EXPORT_C RShapeInfo::RShapeInfo()
	: iFont(0), iHeader(0), iEndOfShapedText(-1), iContextualProcessFunc(0) {}

/** Perform shaping on the text in aText between aStartOfTextToShape and
aEndOfTextToShape, based on the script conventions implied by aScriptCode.
@param aFont The font to use for the shaping.
@param aText The text, including context.
@param aStartOfTextToShape
	The start position within aText of the text to be shaped.
@param aEndOfTextToShape
	The end position within aText of the text to be shaped.
@param aScriptCode The script code for the script being shaped.
@param aLanguageCode The language code for the language being shaped.
@return
	KErrNone if the text was successfully shaped, KErrNotSupported if aFont has
	no shaper, KErrCouldNotConnect if the font bitmap server has not been
	started.
*/
TInt RShapeInfo::Open(const CFont* aFont, const TDesC& aText,
	TInt aStartOfTextToShape, TInt aEndOfTextToShape,
	TInt aScriptCode, TInt aLanguageCode)
	{
	GDI_ASSERT_DEBUG(0 <= aStartOfTextToShape,
		EGdiPanic_InvalidInputParam);
	GDI_ASSERT_DEBUG(aStartOfTextToShape <= aEndOfTextToShape,
		EGdiPanic_InvalidInputParam);
	GDI_ASSERT_DEBUG(aEndOfTextToShape <= aText.Length(),
		EGdiPanic_InvalidInputParam);
	iFont = aFont;
	TFontShapeFunctionParameters param;
	param.iText = &aText;
	param.iStart = aStartOfTextToShape;
	param.iEnd = aEndOfTextToShape;
	param.iScript = aScriptCode;
	param.iLanguage = aLanguageCode;
	const TInt r = aFont->ExtendedFunction(KFontGetShaping, &param);
	iHeader = r == KErrNone ? param.iShapeHeaderOutput : 0;
	if(iHeader)
		iEndOfShapedText = aEndOfTextToShape;
	return r;
	}

/** Frees the memory associated with this shaping information. */
/** @internalComponent */
EXPORT_C void RShapeInfo::Close()
	{
	if (iHeader)
		{
		TFontShapeDeleteFunctionParameters param;
		param.iShapeHeader = iHeader;
		iFont->ExtendedFunction(KFontDeleteShaping, &param);
		iHeader = NULL;
		iEndOfShapedText = -1;
		// We don't reset iSingleContextChar because we want the context to remain throughout,
		// even when the session is closed. It would eventually simply go out of scope.
		}
	}

/** Returns the number of glyphs in the shaped output.
@return The number of glyphs. Also equal to the size of the Glyphs() array and
the GlyphPositions() array. */
TInt RShapeInfo::GlyphCount() const
	{
	GDI_ASSERT_ALWAYS(iHeader, EGdiPanic_Invariant);
	return iHeader->iGlyphCount;
	}

/** Returns the array of glyphs. These must be ORed with 0x80000000 to make
glyph numbers that functions like CFbsFont::Rasterize can accept to avoid
confusing glyph numbers with Unicode character numbers.
@return The glyph array. The size of this array is RShapeInfo::GlyphCount
@see GlyphCount */
const TInt32* RShapeInfo::Glyphs() const
	{
	GDI_ASSERT_ALWAYS(iHeader, EGdiPanic_Invariant);
	return reinterpret_cast<const TInt32*>(iHeader->iBuffer);
	}

/** Returns the array of positions for the glyphs returned by Glyphs, and the
total advance for the text.
@return
	Array of glyph positions in pixels, relative to the pen position before
	the glyphs are drawn. The array has GlyphCount() + 1 entries, as the
	last entry represents the total advance of the text. */
const RShapeInfo::TPoint16* RShapeInfo::GlyphPositions() const
	{
	GDI_ASSERT_ALWAYS(iHeader, EGdiPanic_Invariant);
	return reinterpret_cast<const RShapeInfo::TPoint16*>(iHeader->iBuffer
		+ ((iHeader->iGlyphCount) << 2));
	}

/** Returns the pen advance these glyphs require.
@return The pen advance; where to move the pen after drawing all the glyphs. */
RShapeInfo::TPoint16 RShapeInfo::Advance() const
	{
	GDI_ASSERT_ALWAYS(iHeader, EGdiPanic_Invariant);
	RShapeInfo::TPoint16 r;
	r.iX = *reinterpret_cast<const TInt16*>(iHeader->iBuffer
		+ (iHeader->iGlyphCount << 3));
	r.iY = *reinterpret_cast<const TInt16*>(iHeader->iBuffer
		+ (iHeader->iGlyphCount << 3) + 2);
	return r;
	}

/** Returns the array of indices.
@return
	Indices[n] is the position in the input text that produced Glyphs[n].
*/
const TInt16* RShapeInfo::Indices() const
	{
	GDI_ASSERT_ALWAYS(iHeader, EGdiPanic_Invariant);
	return reinterpret_cast<const TInt16*>(iHeader->iBuffer
		+ (iHeader->iGlyphCount << 3) + 4);
	}
	
TInt RShapeInfo::EndOfShapedText()
	{
	return iEndOfShapedText;
	}

/** Checks if this shaping information is still occupying memory. */
EXPORT_C TBool RShapeInfo::IsOpen()
	{
	if(iHeader && iHeader->iGlyphCount >= 0 && iHeader->iCharacterCount >=0)
		return ETrue;
	else
		return EFalse;
		
	}

void RShapeInfo::SetContext(TAny* aContextualProcessFunc)
	{
	iContextualProcessFunc = aContextualProcessFunc;
	}
	
TAny* RShapeInfo::GetContext()
	{
	return iContextualProcessFunc;
	}

