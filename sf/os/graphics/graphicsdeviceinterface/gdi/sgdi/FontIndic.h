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

#ifndef __FONTINDIC_H__
#define __FONTINDIC_H__

#include "GlyphSel.h"

/** Defines the maximum ammount of text thats can be shaped at one time.
Change this number to suit optimisation, as increasing it or decreasing it may 
make Devanagari text faster/slower.*/
const TInt KMaxTextToShape = 16;

enum TIndicCodeType
	{
	// These values must not change without also changing
	// KStateResponse table accordingly
	EIndicCodeOther = 0,
	EIndicCodeCombiningSign = 1,
	EIndicCodeIndependentVowel = 2,
	EIndicCodeConsonant = 3,
	EIndicCodeDependentVowel = 4,
	EIndicCodeVirama = 5,
	EIndicCodeNukta = 6,
	EIndicCodeZWJ = 7,
	EIndicCodeNuktaConsonant = 8,
	EIndicCodeIndependentVowelA = 9,
	EIndicCodeDependentVowelCandraE = 10,
	EIndicCodeConsonantKannadaBengaliRa = 11,
	EIndicCodeSplitVowel1 = 12,
	EIndicCodeSplitVowel2 = 13,
	EIndicCodeTamilLengthMarkAU = 14,
	//Gurmukhi specific types: EIndicCodeGurBeraer* for specific bearer
	//EIndicCodeGurDependentVowel* for dependent vowles which can be combined 
	//with corresponding bearer to form an independent vowlel
	EIndicCodeGurBearerA = 15,
	EIndicCodeGurBearerU = 16,
	EIndicCodeGurBearerI = 17,
	EIndicCodeGurDependentVowelA = 18,
	EIndicCodeGurDependentVowelU = 19,
	EIndicCodeGurDependentVowelI = 20,
	EIndicCodeConsonantMalaChillu = 21,
	EIndicCodeConsonantMalaChilluNA = 22,
	EIndicCodeConsonantMalaRRA = 23
	};
	
struct TIndicCodeMapping
	{
	TInt16 iFirstCodeInRange;
	TInt16 iCodeType;	// From TIndicCodeType
	};

/** Holds the types of all the symbols in the devanagari range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KDevCodeMap[] =
	{
	{0, EIndicCodeOther},
	{0x901, EIndicCodeCombiningSign},
	{0x904, EIndicCodeIndependentVowel},
	{0x905, EIndicCodeIndependentVowelA},
	{0x906, EIndicCodeIndependentVowel},
	{0x915, EIndicCodeConsonant},
	{0x929, EIndicCodeNuktaConsonant},
	{0x92A, EIndicCodeConsonant},
	{0x931, EIndicCodeNuktaConsonant},
	{0x932, EIndicCodeConsonant},
	{0x934, EIndicCodeNuktaConsonant},
	{0x935, EIndicCodeConsonant},
	{0x93A, EIndicCodeOther},
	{0x93C, EIndicCodeNukta},
	{0x93D, EIndicCodeOther},
	{0x93E, EIndicCodeDependentVowel},
	{0x945, EIndicCodeDependentVowelCandraE},
	{0x946, EIndicCodeDependentVowel},
	{0x94D, EIndicCodeVirama},
	{0x94E, EIndicCodeOther},
	{0x951, EIndicCodeCombiningSign},
	{0x955, EIndicCodeConsonant},
	{0x958, EIndicCodeNuktaConsonant},
	{0x960, EIndicCodeIndependentVowel},
	{0x962, EIndicCodeDependentVowel},
	{0x964, EIndicCodeOther},
	{0x200D, EIndicCodeZWJ},
	{0x200E, EIndicCodeOther}
	};
	
/** Holds the types of all the symbols in the Kannada range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KKanCodeMap[] =
	{
	{0, EIndicCodeOther},
	{0xC82, EIndicCodeCombiningSign},
	{0xC85, EIndicCodeIndependentVowel},
	{0xC95, EIndicCodeConsonant},
	{0xCB0, EIndicCodeConsonantKannadaBengaliRa},
	{0xCB1, EIndicCodeConsonant},
	{0xCB4, EIndicCodeOther},
	{0xCB5, EIndicCodeConsonant},
	{0xCBA, EIndicCodeOther},
	{0xCBC, EIndicCodeNukta},
	{0xCBD, EIndicCodeOther},
	{0xCBE, EIndicCodeDependentVowel},
	{0xCC5, EIndicCodeOther},
	{0xCC6, EIndicCodeDependentVowel},
	{0xCCD, EIndicCodeVirama},
	{0xCD5, EIndicCodeCombiningSign},
	{0xCDE, EIndicCodeConsonant},
	{0xCE0, EIndicCodeIndependentVowel},
	{0xCE2, EIndicCodeDependentVowel},
	{0xCE4, EIndicCodeOther},
	{0x200D, EIndicCodeZWJ},
	{0x200E, EIndicCodeOther}
	};

/** Holds the types of all the symbols in the Gujarati range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KGujCodeMap[] =
	{
	{0, EIndicCodeOther},
	{0xA81, EIndicCodeCombiningSign},
	{0xA85, EIndicCodeIndependentVowel},
	{0xA8E, EIndicCodeOther},
	{0xA8F, EIndicCodeIndependentVowel},
	{0xA92, EIndicCodeOther},
	{0xA93, EIndicCodeIndependentVowel},
	{0xA95, EIndicCodeConsonant},
	{0xAA9, EIndicCodeOther},
	{0xAAA, EIndicCodeConsonant},
	{0xAB1, EIndicCodeOther},
	{0xAB2, EIndicCodeConsonant},
	{0xAB4, EIndicCodeOther},
	{0xAB5, EIndicCodeConsonant},
	{0xABC, EIndicCodeNukta},
	{0xABD, EIndicCodeOther},
	{0xABE, EIndicCodeDependentVowel},
	{0xAC6, EIndicCodeOther},
	{0xAC7, EIndicCodeDependentVowel},
	{0xACA, EIndicCodeOther},
	{0xACB, EIndicCodeDependentVowel},
	{0xACD, EIndicCodeVirama},
	{0xACE, EIndicCodeOther},
	{0xAE0, EIndicCodeIndependentVowel},
	{0xAE2, EIndicCodeDependentVowel},
	{0xAE4, EIndicCodeOther},
	{0x200D, EIndicCodeZWJ},
	{0x200E, EIndicCodeOther}
	};
	
/** Holds the types of all the symbols in the Gurmukhi range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KGurCodeMap[] =
	{
	{0, EIndicCodeOther},
	{0xA01, EIndicCodeCombiningSign},
	{0xA04, EIndicCodeOther},
	{0xA05, EIndicCodeGurBearerA},
	{0xA06, EIndicCodeIndependentVowel},
	{0xA0B, EIndicCodeOther},
	{0xA0F, EIndicCodeIndependentVowel},
	{0xA11, EIndicCodeOther},
	{0xA13, EIndicCodeIndependentVowel},
	{0xA15, EIndicCodeConsonant},
	{0xA29, EIndicCodeOther},
	{0xA2A, EIndicCodeConsonant},
	{0xA31, EIndicCodeOther},
	{0xA32, EIndicCodeConsonant},
	{0xA33, EIndicCodeNuktaConsonant},
	{0xA34, EIndicCodeOther},
	{0xA35, EIndicCodeConsonant},
	{0xA36, EIndicCodeNuktaConsonant},
	{0xA37, EIndicCodeOther},
	{0xA38, EIndicCodeConsonant},
	{0xA3A, EIndicCodeOther},
	{0xA3C, EIndicCodeNukta},
	{0xA3D, EIndicCodeOther},
	{0xA3E, EIndicCodeGurDependentVowelA},
	{0xA3F, EIndicCodeGurDependentVowelI},
	{0xA41, EIndicCodeGurDependentVowelU},
	{0xA43, EIndicCodeOther},
	{0xA47, EIndicCodeGurDependentVowelI},
	{0xA48, EIndicCodeGurDependentVowelA},
	{0xA49, EIndicCodeOther},
	{0xA4B, EIndicCodeGurDependentVowelU},
	{0xA4C, EIndicCodeGurDependentVowelA},
	{0xA4D, EIndicCodeVirama},
	{0xA4E, EIndicCodeOther},
	{0xA59, EIndicCodeNuktaConsonant},
	{0xA5C, EIndicCodeConsonant},
	{0xA5D, EIndicCodeOther},
	{0xA5E, EIndicCodeNuktaConsonant},
	{0xA5F, EIndicCodeOther},
	{0xA70, EIndicCodeCombiningSign},
	{0xA71, EIndicCodeCombiningSign},
	{0xA72, EIndicCodeGurBearerI},
	{0xA73, EIndicCodeGurBearerU},
	{0xA74, EIndicCodeOther},
	{0x200D, EIndicCodeZWJ},
	{0x200E, EIndicCodeOther}
	};

/** Holds the types of all the symbols in the Bengali range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KBenCodeMap[] =
	{
	{0, EIndicCodeOther},
	{0x981 , EIndicCodeCombiningSign},
	{0x985 , EIndicCodeIndependentVowel},
	{0x995 , EIndicCodeConsonant},
	{0x9B0 , EIndicCodeConsonantKannadaBengaliRa},
	{0x9B1 , EIndicCodeConsonant},
	{0x9BA , EIndicCodeOther},
	{0x9BC , EIndicCodeNukta},
	{0x9BD , EIndicCodeOther},
	{0x9BE , EIndicCodeDependentVowel},
	{0x9C9 , EIndicCodeOther},
	{0x9CB , EIndicCodeDependentVowel},
	{0x9CD , EIndicCodeVirama},
	{0x9CE , EIndicCodeOther},
	{0x9D7 , EIndicCodeDependentVowel},
	{0x9DC , EIndicCodeConsonant},
	{0x9E0 , EIndicCodeIndependentVowel},
	{0x9E2 , EIndicCodeDependentVowel},
	{0x9E4 , EIndicCodeOther},
	{0x200D, EIndicCodeZWJ},
	{0x200E, EIndicCodeOther}
	};

/** Holds the types of all the symbols in the Tamil range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KTamCodeMap[] =
	{
	{0, EIndicCodeOther},
	{0xB82 , EIndicCodeCombiningSign},
	{0xB83 , EIndicCodeIndependentVowel},
	{0xB84 , EIndicCodeOther},
	{0xB85 , EIndicCodeIndependentVowel},
	{0xB92 , EIndicCodeIndependentVowelA},
	{0xB93 , EIndicCodeIndependentVowel},
	{0xB95 , EIndicCodeConsonant},
	{0xBBE , EIndicCodeSplitVowel2},
	{0xBBF , EIndicCodeDependentVowel},
	{0xBC6 , EIndicCodeSplitVowel1},
	{0xBC8 , EIndicCodeDependentVowel},
	{0xBCD , EIndicCodeVirama},
	{0xBCE , EIndicCodeOther},
	{0xBD7 , EIndicCodeTamilLengthMarkAU},
	{0xBE4 , EIndicCodeOther},
	{0x200D, EIndicCodeZWJ},
	{0x200E, EIndicCodeOther}
	};

/** Holds the types of all the symbols in the Telugu range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KTelCodeMap[] =
	{
	{0, EIndicCodeOther},
	{0xC01, EIndicCodeCombiningSign},
	{0xC04, EIndicCodeOther},
	{0xC05, EIndicCodeIndependentVowel},
	{0xC15, EIndicCodeConsonant},
	{0xC3A, EIndicCodeOther},
	{0xC3E, EIndicCodeDependentVowel},
	{0xC4D, EIndicCodeVirama},
	{0xC4E, EIndicCodeOther},
	{0xC55, EIndicCodeDependentVowel},
	{0xC57, EIndicCodeOther},
	{0xC60, EIndicCodeIndependentVowel},
	{0xC62, EIndicCodeOther}
	};

/** Holds the types of all the symbols in the Malayalam range, so that we can
work out where the syllable boundaries begin and end. */
static const TIndicCodeMapping KMalaCodeMap[] =
	{
	{0, EIndicCodeOther}, // NOT USED D00 - D01
	{0xD02, EIndicCodeCombiningSign}, // Modifiers
	{0xD04, EIndicCodeOther}, // NOT USED
	{0xD05, EIndicCodeIndependentVowel},
//	{0xD0D, EIndicCodeOther}, // Reserved
//	{0xD0E, EIndicCodeIndependentVowel},
//	{0xD11, EIndicCodeOther}, // Reserved
//	{0xD12, EIndicCodeIndependentVowel},
	{0xD15, EIndicCodeConsonant},
	{0xD23, EIndicCodeConsonantMalaChillu}, // NNA
	{0xD24, EIndicCodeConsonant},
	{0xD28, EIndicCodeConsonantMalaChilluNA}, // NA
	{0xD29, EIndicCodeOther}, // Reserved
	{0xD2A, EIndicCodeConsonant},
	{0xD30, EIndicCodeConsonantMalaChillu}, // RA
	{0xD31, EIndicCodeConsonantMalaRRA}, // RRA
	{0xD32, EIndicCodeConsonantMalaChillu}, // LA,  LLA
	{0xD34, EIndicCodeConsonant},
	{0xD3A, EIndicCodeOther}, // NOT USED D3A - D3D
	{0xD3E, EIndicCodeSplitVowel2},
	{0xD3F, EIndicCodeDependentVowel},
//	{0xD44, EIndicCodeOther}, // Reserved D44 - D45
	{0xD46, EIndicCodeSplitVowel1},
	{0xD48, EIndicCodeDependentVowel},
//	{0xD49, EIndicCodeOther}, // Reserved D49
//	{0xD4A, EIndicCodeDependentVowel},
	{0xD4D, EIndicCodeVirama},
	{0xD4E, EIndicCodeOther}, //Reserved D4E - D56
	{0xD57, EIndicCodeTamilLengthMarkAU}, // EIndicCodeDependentVowel for Telugu ???
	{0xD58, EIndicCodeOther}, // NOT USED D58 - D5F
	{0xD60, EIndicCodeIndependentVowel}, // RR, LL
	{0xD62, EIndicCodeOther}, // NOT USED D62 - D65, digits D66 - D6F	
	{0x200D, EIndicCodeZWJ},
	{0x200E, EIndicCodeOther}
	};

/* Holds all parameters specific to scripts */
struct TIndicScriptProcessorParameters{
	TUint32 iLanguageCode;
	TUint32 iScriptCode;
	TUint iUCLow;
	TUint iUCHigh;
	TUint iMapLen;
	const TIndicCodeMapping *iCodeMap;
};

static const TIndicScriptProcessorParameters KIndicDevParameters = {
	0x48494E20, //KHindiLanguageCode,	
	0x64657661, //KDevanagariScriptCode,	
	0x0900, //KDevLow,
	0x0970, //KDevHigh,	
	sizeof(KDevCodeMap)/sizeof(KDevCodeMap[0]),
	KDevCodeMap
};

static const TIndicScriptProcessorParameters KIndicKanParameters = {
	0x4B414E20, //KKannadaLanguageCode,	
	0x6B6E6461, //KKannadaScriptCode,	
	0x0C80, //KKanLow,
	0x0CFF, //KKanHigh,	
	sizeof(KKanCodeMap)/sizeof(KKanCodeMap[0]),
	KKanCodeMap
};

static const TIndicScriptProcessorParameters KIndicGujParameters = {
	0x47554A20, //KGujaratiLanguageCode,	
	0x67756A72, //KGujaratiScriptCode,	
	0x0A80, //KGujLow,
	0x0AFF, //KGujHigh,	
	sizeof(KGujCodeMap)/sizeof(KGujCodeMap[0]),
	KGujCodeMap
};

static const TIndicScriptProcessorParameters KIndicBenParameters = {
	0x42454E20, //KBengaliLanguageCode,	
	0x62656E67, //KBengaliScriptCode,	
	0x0980, //KBenLow,
	0x09FF, //KBenHigh,	
	sizeof(KBenCodeMap)/sizeof(KBenCodeMap[0]),
	KBenCodeMap
};

static const TIndicScriptProcessorParameters KIndicTamParameters = {
	0x54414D20, //KTamilLanguageCode,	
	0x74616D6C, //KTamilScriptCode,
	0x0B80, //KTamLow,
	0x0BFF, //KTamHigh,	
	sizeof(KTamCodeMap)/sizeof(KTamCodeMap[0]),
	KTamCodeMap
};

static const TIndicScriptProcessorParameters KIndicTelParameters = {
	0x54454C20, //KTeluguLanguageCode,	
	0x74656C75, //KTeluguScriptCode,
	0x0C00, //KTelLow,
	0x0C7F, //KTelHigh,	
	sizeof(KTelCodeMap)/sizeof(KTelCodeMap[0]),
	KTelCodeMap
};

static const TIndicScriptProcessorParameters KIndicGurParameters = {
	0x00000000,	//KGurmukhiLanguageCode,
	0x67757275, //KGurmukhiScriptCode,
	0x0A00, //KGurLow,
	0x0A7F, //KGurHigh,
	sizeof(KGurCodeMap)/sizeof(KGurCodeMap[0]),
	KGurCodeMap
};

static const TIndicScriptProcessorParameters KIndicMalParameters = {
	0x4D4C5220, //KMalayalamLanguageCode,	
	0x6D6C796D, //KMalayalamScriptCode,
	0x0D00, //KMalaLow,
	0x0D7F, //KMalaHigh,	
	sizeof(KMalaCodeMap)/sizeof(KMalaCodeMap[0]),
	KMalaCodeMap
};

/** This is the default glyph selector class for Devanagari characters in the
range 0x0900..0x097F.
@internalComponent
*/
class GlyphSelector_Devanagari
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};


/** This is the default glyph selector class for Kannada characters in the
range 0x0C80..0x0CFF.
@internalComponent
*/
class GlyphSelector_Kannada
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};
	

/** This is the default glyph selector class for Gujarati characters in the
range 0x0A80..0x0AFF.
@internalComponent
*/
class GlyphSelector_Gujarati
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};
	
/** This is the default glyph selector class for Bengali characters in the
range 0x0980..0x09FF.
@internalComponent
*/
class GlyphSelector_Bengali
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};
	
/** This is the default glyph selector class for Tamil characters in the
range 0x0B80..0x0BFF.
@internalComponent
*/
class GlyphSelector_Tamil
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};

/** This is the default glyph selector class for Telugu characters in the
range 0x0C00..0x0C7F.
@internalComponent
*/
class GlyphSelector_Telugu
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};

/** This is the default glyph selector class for Gurmukhi characters in the
range 0x0A00..0x0A7F.
@internalComponent
*/
class GlyphSelector_Gurmukhi
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};

/** This is the default glyph selector class for Malayalam characters in the
range 0x0D00..0x0D7F.
@internalComponent
*/
class GlyphSelector_Malayalam
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};

#endif
