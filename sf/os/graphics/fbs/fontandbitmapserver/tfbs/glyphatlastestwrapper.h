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

#ifndef GLYPHATLASTESTWRAPPER_H
#define GLYPHATLASTESTWRAPPER_H

#include "glyphatlas.h"


/**
Class to provide access to glyph atlas metrics for test purposes
*/
NONSHARABLE_CLASS(CGlyphAtlasTestWrapper)
    {
public:
	~CGlyphAtlasTestWrapper();
	static CGlyphAtlasTestWrapper* NewL(TInt aMaxCacheSizeInBytes);
	static CGlyphAtlasTestWrapper* NewLC(TInt aMaxCacheSizeInBytes);
	TInt GetGlyph(const CBitmapFont& aFont, TUint aGlyphCode, TGlyphImageInfo& aGlyphImageInfo);
	TInt AddGlyph(const CBitmapFont& aFont, const CGlyphAtlas::TAddGlyphArgs& aArgs, TGlyphImageInfo& aGlyphImageInfo);
	void FontReleased(const CBitmapFont& aFont);
	
	// Utility functions providing access to internals of glyph atlas
	TInt SizeInBytes() const;
	TInt GlyphCountByFont(const CBitmapFont* aFont);
	TInt GlyphCount();
	TInt FontCount() const;
	TBool LruPageContainsGlyph(TUint aGlyphCode) const;
private:
	CGlyphAtlasTestWrapper();
	void ConstructL(TInt aMaxCacheSizeInBytes);
private:
	CGlyphAtlas* iAtlas;
    };

#endif /* GLYPHATLASTESTWRAPPER_H */
