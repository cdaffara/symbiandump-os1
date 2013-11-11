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
// GlyphSample class.

#ifndef GLYPHSAMPLE_H
#define GLYPHSAMPLE_H

/**
Provides methods to map a TLanguage type to some pre-defined glyph samples.
Samples are defined per writing script, e.g. specifying ELangFrench or
ELangGerman will return the same set of samples.
These samples are typically the tallest and 'deep'est characters w.r.t
various scripts, useful for font rasterizers when calculating metrics.
@internalTechnology
*/
NONSHARABLE_CLASS(GlyphSample)
	{
public:
	/**
	Writing scripts defined according to Unicode.
	*/
	enum TScript
		{
		EScriptDefault			= 0x0,	// No script, can't be bothered
		EScriptNone				= 0x1,	// No script, really
		EScriptOther			= 0x2,	// Unsupported scripts
		EScriptLatin			= 0x3,	// European
		EScriptGreek			= 0x4,	// European
		EScriptCyrillic			= 0x5,	// European
		EScriptHebrew			= 0x6,	// Middle Eastern
		EScriptArabic			= 0x7,	// Middle Eastern
		EScriptDevanagari		= 0x8,	// Indic
		EScriptThai				= 0x9,	// SE Asian
		EScriptHanIdeographs	= 0xA,	// E Asian
		};
public:
	IMPORT_C static TInt TLanguage2TScript(TLanguage aLanguage);
	IMPORT_C static const TPtrC TScript2GlyphSample(TInt aScript);
private:
	static const TInt KTLanguage2TScript[];
	static const TText* const KTScript2GlyphSample[];
	};

#endif /* GLYPHSAMPLE_H */
