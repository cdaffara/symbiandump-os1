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

#ifndef SHAPEINFO_H_
#define SHAPEINFO_H_

#include <e32cmn.h>

class CFont;
class TDesC16;
class TShapeHeader;

/** Holds shaping information for text that requires it (for example Indic
text).
@publishedAll
@released
*/
class RShapeInfo
	{
public:
	struct TPoint16
		{
		TInt16 iX;
		TInt16 iY;
		};
	IMPORT_C RShapeInfo();
	TInt Open(const CFont* aFont, const TDesC16& aText,
		TInt aStartOfTextToShape, TInt aEndOfTextToShape,
		TInt aScriptCode, TInt aLanguageCode);
	IMPORT_C void Close();
	TInt GlyphCount() const;
	const TInt32* Glyphs() const;
	const TPoint16* GlyphPositions() const;
	TPoint16 Advance() const;
	const TInt16* Indices() const;
	TInt EndOfShapedText();
	IMPORT_C TBool IsOpen();
	void SetContext(TAny* aContextualProcessFunc);
	TAny* GetContext();
private:
	const CFont* iFont;
	const TShapeHeader* iHeader;
	TInt iEndOfShapedText;
	TAny* iContextualProcessFunc;
	};

#endif
