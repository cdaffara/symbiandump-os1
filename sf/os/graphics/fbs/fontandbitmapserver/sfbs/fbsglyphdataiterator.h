// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef FBSGLYPHDATAITERATOR_H
#define FBSGLYPHDATAITERATOR_H

#include <e32def.h>
#include <fbs.h>
#include <sgresource/sgimage.h>

class CGlyphDataIteratorImpl;
class TOpenFontCharMetrics;

/**
An iterator class to provide access to glyph image data and metrics, 
one glyph per iteration.
@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(RFbsGlyphDataIterator)
	{
public:
	IMPORT_C RFbsGlyphDataIterator();
	IMPORT_C TInt Open(CFbsFont& aFont, const TUint* aGlyphCodes, TInt aCount);
	IMPORT_C TInt Next();
	IMPORT_C void Close();
	IMPORT_C const RSgImage& Image() const;
	IMPORT_C const TRect& Rect() const;
	IMPORT_C const TOpenFontCharMetrics& Metrics() const;
	IMPORT_C TUint GlyphCode() const;
	
private:
	RFbsGlyphDataIterator(const RFbsGlyphDataIterator&);
	const RFbsGlyphDataIterator& operator =(const RFbsGlyphDataIterator&);
	
private:
    CGlyphDataIteratorImpl* iImpl;
	};

#endif /* FBSGLYPHDATAITERATOR_H */
