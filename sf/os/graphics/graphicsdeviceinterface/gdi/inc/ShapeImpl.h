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
//

#ifndef SHAPEIMPL_H_
#define SHAPEIMPL_H_

#include <e32cmn.h>

/**@file
@internalTechnology
*/

class CFont;
class TDesC16;

const TUid KFontGetShaping = {0x10274246};
const TUid KFontDeleteShaping = {0x102744D7};

/** Internal class describing shaping information. */
class TShapeHeader
	{
public:
	/** Number of glyphs output. */
	TInt iGlyphCount;
	/** Number of characters consumed from the input. */
	TInt iCharacterCount;
	/** Reserved for future expansion. */
	TInt iReserved0;
	/** Reserved for future expansion. */
	TInt iReserved1;
	/** The glyph, position and indices data.

	The first iGlyphCount * 4 bytes contain the glyph codes
	as 2-byte values. The next iGlyphCount * 4 + 4 bytes contain the
	positions in which these glyphs are to be drawn, in pixels, taking
	the original pen position as origin, with the x axis going right and
	the y axis going down. The next iGlyphCount * 2 bytes contain the
	indices of the characters in the input string that correspond to the
	glyphs in the output.
	
	The final entry in the position array is the total advance of the text.

	Therefore this buffer is actually iGlyphCount * 10 + 4 bytes long. */
	TInt8 iBuffer[1];
	};

/** For CFont::ExtendedFunctions aParam argument, with KFontGetShaping
function ID. */
class TFontShapeFunctionParameters
	{
public:
	/** The text, including context. */
	const TDesC16* iText;
	/** The start of the meat within iText. */
	TInt iStart;
	/** The end of the meat within iText. */
	TInt iEnd;
	/** Script code. */
	TInt iScript;
	/** Language code. 0 for default. */
	TInt iLanguage;
	/** Output from the shaper; a TShapeHeader on the shared heap. */
	const TShapeHeader* iShapeHeaderOutput;
	};

/** For CFont::ExtendedFunctions aParam argument, with KFontDeleteShaping
function ID. */
class TFontShapeDeleteFunctionParameters
	{
public:
	const TShapeHeader* iShapeHeader;
	};

#endif
