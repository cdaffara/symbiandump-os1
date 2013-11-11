// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/


#include <gdi.h>
#include "FontThai.h"


//
// ThaiGlyph Namespace definition
//


/**
 This namespace holds functions used to evaluate a glyph character code
 against a given Thai related prediciate. The 'code' argument is a glyph
 from the current output cluster and so may be a Thai glyph, Thai PUA glyph, 
 the dotted circle glyph or 0xffff. Therefore it was decided not to implement
 these routines using a data driven table approach as it would be inefficient.
@internalComponent.
*/
namespace ThaiGlyph
    {
    const TText16 KYoYing		= 0x0E0D;
    const TText16 KYoYingPua	= 0xF70F;
    const TText16 KThoThan		= 0x0E10;
    const TText16 KThoThanPua	= 0xF700;
    const TText16 KNikhahit	    = 0x0E4D;
    const TText16 KNikhahitPua	= 0xF711;
    const TText16 KSaraAa		= 0x0E32;
    const TText16 KSaraAm		= 0x0E33;

    
     TBool IsThaiGlyph(TUint code)
    	{
       	return ((code > 0x0E00 && code < 0x0E3B) ||
    			(code > 0x0E3E && code < 0x0E5C) ||
    			(code >= 0xF700 && code <= 0xF71A));
    	}

     TBool IsThaiConsonant(TUint code)
    	{
    	return (code >= 0x0E01 && code <= 0x0E2E);
    	}

     TBool IsThaiTallConsonant(TUint code)
    	{
    	return (//code == 0x0E0A ||	// CHO CHANG not tall at all
    			//code == 0x0E0B ||	// SO SO not tall at all
    			code == 0x0E1B ||	// PO PLA
    			code == 0x0E1D ||	// FO FA
    			code == 0x0E1F ||	// FO FAN
    			code == 0x0E2C);	// LO CHULA in some fonts, Unicode tables show it as tall
    	}

     TBool IsThaiShortConsonant(TUint code)
    	{
    	return (((code >= 0x0E01 && code <= 0x0E2E) || (code == KUnicodeDottedCircle)) &&
    			code != 0x0E1B &&	// PO PLA
    			code != 0x0E1D &&	// FO FA
    			code != 0x0E1F &&	// FO FAN
    			code != 0x0E2C);	// LO CHULA in some fonts, Unicode tables show it as tall
    	}

     TBool IsThaiConsonantWithDisjointDescender(TUint code)
    	{
    	return (code == ThaiGlyph::KYoYing || code == ThaiGlyph::KThoThan);
    	}

     TBool IsThaiConsonantWithJointDescender(TUint code)
    	{
    	return (code == 0x0E0E || // DO CHADA
    			code == 0x0E0F || // PO PATAK
    			code == 0x0E24 || // RU
       			code == 0x0E26);  // LU
    	}

     TBool IsThaiVowel(TUint code)
    	{
    	return ((code >= 0x0E30 && code <= 0x0E3A) ||
    			(code >= 0x0E40 && code <= 0x0E44) ||
    			code == 0x0E47);	// MAITAIKHU
    	}

     TBool IsThaiDepVowel(TUint code)
    	{
    	return (code == 0x0E31 ||	// MAI HAN-AKAT
    			(code >= 0x0E34 && code <= 0x0E3A) ||
    			code == 0x0E47);	// MAITAIKHU
    	}

     TBool IsThaiDepVowelAbove(TUint code)
    	{
    	return (code == 0x0E31 ||	// MAI HAN-AKAT
    			(code >= 0x0E34 && code <= 0x0E37) ||
    			code == 0x0E47);	// MAITAIKHU
    	}

     TBool IsThaiDepVowelAbovePUA(TUint code)
    	{
    	return (code == 0xF710 ||	// MAI HAN-AKAT
    			(code >= 0xF701 && code <= 0xF704) ||
    			code == 0xF712);	// MAITAIKHU
    	}

     TBool IsThaiDepVowelBelow(TUint code)
    	{
    	return (code >= 0x0E38 && code <= 0x0E3A);
    	}

     TBool IsThaiIndepVowel(TUint code)
    	{
    	return (code == 0x0E30 ||	// SARA A
      			code == 0x0E32 ||	// SARA AA
    			code == 0x0E33 ||	// SARA AM
    			(code >= 0x0E40 && code <= 0x0E44));
    	}

     TBool IsThaiToneMark(TUint code)
    	{
    	return (code >= 0x0E48 && code <= 0x0E4B);
    	}
    }

    
//
//
// ThaiCharRules Namespace definition
//
//


/**
 ThaiCharRules namespace holds the data and lookup methods
 implementing the WTT 2.0 input/output validation matrix.
@internalComponent
*/
namespace ThaiCharRules
	{
  	const TUint KThaiCodePageStart      = 0x0E00;
	const TUint KThaiCodePageEnd        = 0x0E5C;
	const TUint KNumThaiCharacters      = KThaiCodePageEnd-KThaiCodePageStart;

	enum Wtt2Rule
		{
		EUndefined,
		EAccept,
		EComposite,
		EReject,
		ERejectStrict,
		};

	/**
	This enumeration holds the set of classification values a Thai
	character can be categorised as in the WTT2.0 specification.
	*/
	enum CharClassification
		{
		ENull,
		EControl,
		ENonPrintable,
		EConsonant,
		ELeadingVowel,
		EOrdinaryFollowingVowel,
		EDependentFollowingVowel,
		ESpecialFollowingVowel,
		EShortBelowVowel,
		ELongBelowVowel,
		EBelowDiacritic,
		EToneMark,
		EAboveDiacritic0,
		EAboveDiacritic1,
		EAboveDiacritic2,
		EAboveDiacritic3,
		EAboveVowel1,
		EAboveVowel2,
		EAboveVowel3,
		// marker for end
		EMaxClassification
		};


	/**
	 Data table holding the classification of each character.
	*/
	static const TUint8 iCharClassifications[KNumThaiCharacters] = 
        {
    	ENull,			// No entry in code page
    	EConsonant,		// 0x0E01
    	EConsonant,		// 0x0E02
    	EConsonant,		// 0x0E03
    	EConsonant,		// 0x0E04
    	EConsonant,		// 0x0E05
    	EConsonant,		// 0x0E06
    	EConsonant,		// 0x0E07
    	EConsonant,		// 0x0E08
    	EConsonant,		// 0x0E09
    	EConsonant,		// 0x0E0A
    	EConsonant,		// 0x0E0B
    	EConsonant,		// 0x0E0C
    	EConsonant,		// 0x0E0D
    	EConsonant,		// 0x0E0E
    	EConsonant,		// 0x0E0F

    	EConsonant,		// 0x0E10
    	EConsonant,		// 0x0E11
    	EConsonant,		// 0x0E12
    	EConsonant,		// 0x0E13
    	EConsonant,		// 0x0E14
    	EConsonant,		// 0x0E15
    	EConsonant,		// 0x0E16
    	EConsonant,		// 0x0E17
    	EConsonant,		// 0x0E18
    	EConsonant,		// 0x0E19
    	EConsonant,		// 0x0E1A
    	EConsonant,		// 0x0E1B
    	EConsonant,		// 0x0E1C
    	EConsonant,		// 0x0E1D
    	EConsonant,		// 0x0E1E
    	EConsonant,		// 0x0E1F

    	EConsonant,		// 0x0E20
    	EConsonant,		// 0x0E21
    	EConsonant,		// 0x0E22
    	EConsonant,		// 0x0E23
    	EConsonant,		// 0x0E24
    	EConsonant,		// 0x0E25
    	EConsonant,		// 0x0E26
    	EConsonant,		// 0x0E27
    	EConsonant,		// 0x0E28
    	EConsonant,		// 0x0E29
    	EConsonant,		// 0x0E2A
    	EConsonant,		// 0x0E2B
    	EConsonant,		// 0x0E2C
    	EConsonant,		// 0x0E2D
    	EConsonant,		// 0x0E2E
    	ENonPrintable,	// 0x0E2F

    	EOrdinaryFollowingVowel,// 0x0E30
    	EAboveVowel2,			// 0x0E31
    	EOrdinaryFollowingVowel,// 0x0E32
    	EOrdinaryFollowingVowel,// 0x0E33
    	EAboveVowel1,			// 0x0E34
    	EAboveVowel3,			// 0x0E35
    	EAboveVowel2,			// 0x0E36
    	EAboveVowel3,			// 0x0E37
    	EShortBelowVowel,		// 0x0E38
    	ELongBelowVowel,		// 0x0E39
    	EBelowDiacritic,		// 0x0E3A
    	ENull,					// 0x0E3B
    	ENull,					// 0x0E3C
    	ENull,					// 0x0E3D
    	ENull,					// 0x0E3E
    	ENonPrintable,			// 0x0E3F

    	ELeadingVowel,			// 0x0E40
    	ELeadingVowel,			// 0x0E41
    	ELeadingVowel,			// 0x0E42
    	ELeadingVowel,			// 0x0E43
    	ELeadingVowel,			// 0x0E44
    	EDependentFollowingVowel,//0x0E45
    	ENonPrintable,			// 0x0E46
    	EAboveDiacritic2,		// 0x0E47
    	EToneMark,				// 0x0E48
    	EToneMark,				// 0x0E49
    	EToneMark,				// 0x0E4A
    	EToneMark,				// 0x0E4B
    	EAboveDiacritic1,		// 0x0E4C
    	EAboveDiacritic0,		// 0x0E4D
    	EAboveDiacritic3,		// 0x0E4E
    	ENonPrintable,			// 0x0E4F

    	ENonPrintable,			// 0x0E50
    	ENonPrintable,			// 0x0E51
    	ENonPrintable,			// 0x0E52
    	ENonPrintable,			// 0x0E53
    	ENonPrintable,			// 0x0E54
    	ENonPrintable,			// 0x0E55
    	ENonPrintable,			// 0x0E56
    	ENonPrintable,			// 0x0E57
    	ENonPrintable,			// 0x0E58
    	ENonPrintable,			// 0x0E59
    	ENonPrintable,			// 0x0E5A
    	ENonPrintable,			// 0x0E5B

    	// Value at last measurement was 92 bytes. 27/6/2003
        };


	/**
	 WTT 2.0 Rules data table of prev to next character
	*/
	static const TUint8 iInputRules[EMaxClassification][EMaxClassification] =
        {
    	/* Previous character ENull */
    	    {
    		EUndefined, EUndefined, EUndefined, EUndefined, EUndefined, 
    		EUndefined, EUndefined, EUndefined, EUndefined, EUndefined,
    		EUndefined, EUndefined, EUndefined, EUndefined, EUndefined,
    		EUndefined, EUndefined, EUndefined, EUndefined 
    	    },

    	/* Previous character EControl */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		EAccept, EAccept, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },

    	/* Previous character ENonPrintable */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },

    	/* Previous character EConsonant */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		EAccept, ERejectStrict, EAccept, EComposite, EComposite,
    		EComposite, EComposite, EComposite, EComposite, EComposite, EComposite,
    		EComposite, EComposite, EComposite,
    	    },

    	/* Previous character ELeadingVowel */
    	    {
    		EUndefined, EUndefined, ERejectStrict, EAccept, ERejectStrict, 
    		ERejectStrict, ERejectStrict, ERejectStrict, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },

    	/* Previous character EOrdinaryFollowingVowel */
    	    {
    		EUndefined, EUndefined, ERejectStrict, EAccept, ERejectStrict, 
    		EAccept, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },

    	/* Previous character EDependentFollowingVowel */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		EAccept, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },

    	/* Previous character ESpecialFollowingVowel */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, EAccept, ERejectStrict, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },

    	/* Previous character EShortBelowVowel */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		EAccept, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EComposite, EComposite, EComposite, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    		
    	/* Previous character ELongBelowVowel */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EComposite, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },

    	/* Previous character EBelowDiacritic */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
    	/* Previous character EToneMark */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		EAccept, EAccept, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
	   	/* Previous character EAboveDiacritic0 */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
    	/* Previous character EAboveDiacritic1 */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
    	/* Previous character EAboveDiacritic2 */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
    	/* Previous character EAboveDiacritic3 */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EReject, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
    	/* Previous character EAboveVowel1 */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EComposite, EReject, EComposite, EReject, EReject,
//    		EReject, EComposite, EComposite, EComposite, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
    	/* Previous character EAboveVowel2 */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EComposite, EReject, EReject, EReject, EReject,
    		EReject, EReject, EReject,
    	    },
    	
    	/* Previous character EAboveVowel3 */
    	    {
    		EUndefined, EUndefined, EAccept, EAccept, EAccept, 
    		ERejectStrict, ERejectStrict, EAccept, EReject, EReject,
    		EReject, EComposite, EReject, EReject, EReject, EReject,
//			EReject, EComposite, EReject, EComposite, EReject,
    		EReject, EReject, EReject,
    	    },

    	// Value at last measurement was 324 bytes. 27/6/2003
        };


    /**
     This routine looks up the WTT 2.0 rule for the given 
     Thai character codes provided in the WTT 2.0 data table.
    @param aPrevChar 
     Unicode character code preceding the assumed position.
    @param aChar
     Unicode character code proceeding the assumed position.
    @return Wtt2Rule
     The rule value found in data table.
    */
    Wtt2Rule LookupWtt2Rule(TUint aPrevChar, TUint aChar)
        {
    	const CharClassification prevCharClassification = 
    	    static_cast<CharClassification>(
    		(aPrevChar > KThaiCodePageStart && aPrevChar < KThaiCodePageEnd) ?
    			iCharClassifications[aPrevChar - KThaiCodePageStart] :
    			ENonPrintable);
    	const CharClassification charClassification = 
    	    static_cast<CharClassification>(
    		(aChar > KThaiCodePageStart && aChar < KThaiCodePageEnd) ?
    			iCharClassifications[aChar - KThaiCodePageStart] :
    			ENonPrintable);

    	return static_cast<Wtt2Rule>
    	    (iInputRules[prevCharClassification][charClassification]);
        }

    }

using namespace ThaiCharRules;
    
    
//
//
// ThaiGlyphPUASubstitution Namespace definition
//
//


/**
 This utility namespace holds the data and lookup mechanisms to support
 the GlyphSelector_Thai glyph selection class in choosing Private User
 Area (PUA) Thai character positional variant glyphs. Use of the PUA glyphs
 results in a satisfactory rendition of Thai writing in Symbian OS.
@internalComponent
*/
namespace ThaiGlyphPUASubstitution
	{
	
	typedef TBool (*UnicodeCharValidator)(const TGlyphSelectionState& aGss);

	struct PUASubstTableEntry
		{
		TUint	iOrigGlyph;
		TUint	iPUAGlyph;
		UnicodeCharValidator	iRuleFunc;
		};


    /**
     ThaiGlyphPUASubstitution rule method which checks the context for
     short consonant preceding char OR 
     short consonant & dependent vowel below preceding char.
    @param aGss
     Container object holds the glyph selection context for the method.
    @return TBool
     ETrue when the rule context is satisfied, EFalse if not. 
    */
     TBool RuleShortConsonant(const TGlyphSelectionState& aGss)
   	{
   		if (aGss.iParam.iOutputGlyphs == 1) {
   		//check the context for short consonant preceding char
    		TUint consonantGss = aGss.iParam.iOutput[0].iCode;
    		if (ThaiGlyph::IsThaiShortConsonant(consonantGss) ||
    			consonantGss == ThaiGlyph::KYoYingPua ||
    			consonantGss == ThaiGlyph::KThoThanPua)
    			return ETrue;
    		else 
    			return EFalse;
    	}
    	if (aGss.iParam.iOutputGlyphs == 2) {
    	//check the context for short consonant & dependent vowel below preceding char
    		TUint consonantGss = aGss.iParam.iOutput[0].iCode;
    		TUint depVowelGss = aGss.iParam.iOutput[1].iCode;
    		if ((ThaiGlyph::IsThaiShortConsonant(consonantGss) ||
    			consonantGss == ThaiGlyph::KYoYingPua ||
    			consonantGss == ThaiGlyph::KThoThanPua) && 
    		  (ThaiGlyph::IsThaiDepVowelBelow(depVowelGss) ||
    			(depVowelGss >= 0xF718 &&
    			 depVowelGss <= 0xF71A)))
    			return ETrue;
    		else
    			return EFalse;
    	}
    	return EFalse;
    }

    /**
     ThaiGlyphPUASubstitution rule method which checks the context for
     tall consonant preceding char.
    @param aGss
     Container object holds the glyph selection context for the method.
    @return TBool
     ETrue when the rule context is satisfied, EFalse if not. 
    */
    TBool RuleTallConsonant(const TGlyphSelectionState& aGss)
    	{
    	if ((aGss.iParam.iOutputGlyphs == 1) &&
    	 	ThaiGlyph::IsThaiTallConsonant(aGss.iParam.iOutput[0].iCode))
    		return ETrue;
    	else
    		return EFalse;
    	}

    /**
     ThaiGlyphPUASubstitution rule method which checks the context for a tall
     consonant which does not have a dependent vowel above or a nikhahit or a
     following sara am.
    @param aGss
     Container object holds the glyph selection context for the method.
    @return TBool
     ETrue when the rule context is satisfied, EFalse if not.
    */
    TBool RuleTallConsonantNoVowelAbove(const TGlyphSelectionState& aGss)
    	{
    	if (aGss.iParam.iOutputGlyphs == 0)
    		return EFalse;
    	if (!ThaiGlyph::IsThaiTallConsonant(aGss.iParam.iOutput[0].iCode))
    		return EFalse;
    	if (aGss.iParam.iOutputGlyphs == 1)
    		return ETrue;
    	if (aGss.iParam.iOutputGlyphs != 2)
    		return EFalse;
    	TUint wantDepVowel = aGss.iParam.iOutput[1].iCode;
    	if (ThaiGlyph::IsThaiDepVowelAbove(wantDepVowel)
    		|| ThaiGlyph::IsThaiDepVowelAbovePUA(wantDepVowel)
    		|| wantDepVowel == ThaiGlyph::KNikhahit
    		|| wantDepVowel == ThaiGlyph::KNikhahitPua)
    		return EFalse;
	return ETrue;
    	}

    /**
     ThaiGlyphPUASubstitution rule method which checks the context for tall
     consonant with either a dependent vowel above or nikhahit.
    @param aGss
     Container object holds the glyph selection context for the method.
    @return TBool
     ETrue when the rule context is satisfied, EFalse if not.
    */
    TBool RuleTallConsonantVowelAbove(const TGlyphSelectionState& aGss)
    	{
    	if ((aGss.iParam.iOutputGlyphs == 2) &&
    	 	ThaiGlyph::IsThaiTallConsonant(aGss.iParam.iOutput[0].iCode) &&
    		(ThaiGlyph::IsThaiDepVowelAbovePUA(aGss.iParam.iOutput[1].iCode))
    		|| aGss.iParam.iOutput[1].iCode == ThaiGlyph::KNikhahitPua)
    		return ETrue;
    	else
    		return EFalse;
    	}

    /**
     ThaiGlyphPUASubstitution rule method which checks the context for
     consonant with joined descender preceding char.
    @param aGss
     Container object holds the glyph selection context for the method.
    @return TBool
     ETrue when the rule context is satisfied, EFalse if not. 
    */
     TBool RuleConsonantWithJointDescender(const TGlyphSelectionState& aGss)
   	{
    	if ((aGss.iParam.iOutputGlyphs == 1) &&
    	 	ThaiGlyph::IsThaiConsonantWithJointDescender(aGss.iParam.iOutput[0].iCode))
    		return ETrue;
    	else
    		return EFalse;
    	}


    const PUASubstTableEntry RuleTable[] = {
    /**
     This data member of the ThaiGlyphPUASubstitution class holds rules
     on when a given PUA glyph should be substituted for the original
     0x0Exx glyph. Table lookup returns the first match found from the
     start of the table, therefore duplicate match situations must be 
     avoided in the rule set logic.
    */
    /*    iOrigGlyph, iPUAGlyph, iRuleFunc                                     */

    	// Substitutions for a tone or sign mark above a short consonant
    	{ 0x0E48,     0xF70A,    RuleShortConsonant },
    	{ 0x0E49,     0xF70B,    RuleShortConsonant },
    	{ 0x0E4A,     0xF70C,    RuleShortConsonant },
    	{ 0x0E4B,     0xF70D,    RuleShortConsonant },
    	{ 0x0E4C,     0xF70E,    RuleShortConsonant },

    	// Substitutions for a vowel or sign mark above a tall consonant
    	{ 0x0E34,	  0xF701,	 RuleTallConsonant },
    	{ 0x0E35,	  0xF702,	 RuleTallConsonant },
    	{ 0x0E36,	  0xF703,	 RuleTallConsonant },
    	{ 0x0E37,	  0xF704,	 RuleTallConsonant },
    	{ 0x0E31,	  0xF710,	 RuleTallConsonant },
    	{ 0x0E4D,	  0xF711,	 RuleTallConsonant },
    	{ 0x0E47,	  0xF712,	 RuleTallConsonant },

    	// Substitutions for a tone or sign mark above a tall consonant
    	{ 0x0E48,	  0xF705,	 RuleTallConsonantNoVowelAbove },
    	{ 0x0E49,	  0xF706,	 RuleTallConsonantNoVowelAbove },
    	{ 0x0E4A,	  0xF707,	 RuleTallConsonantNoVowelAbove },
    	{ 0x0E4B,	  0xF708,	 RuleTallConsonantNoVowelAbove },
    	{ 0x0E4C,	  0xF709, 	 RuleTallConsonantNoVowelAbove },	

    	// Substitutions for a tone or sign mark above a vowel which is 
    	// above a tall consonant
    	{ 0x0E48,	  0xF713,	 RuleTallConsonantVowelAbove },
    	{ 0x0E49,	  0xF714,	 RuleTallConsonantVowelAbove },
    	{ 0x0E4A,	  0xF715,	 RuleTallConsonantVowelAbove },
    	{ 0x0E4B,	  0xF716,	 RuleTallConsonantVowelAbove },
    	{ 0x0E4C,	  0xF717,	 RuleTallConsonantVowelAbove },	

    	// Substitutions for a vowel or sign mark below a consonant with a  
    	// joined descender
    	{ 0x0E38,	  0xF718,	 RuleConsonantWithJointDescender },
    	{ 0x0E39,	  0xF719,	 RuleConsonantWithJointDescender },
    	{ 0x0E3A,	  0xF71A,	 RuleConsonantWithJointDescender },

    	{ 0, 0, 0}

    	// Size of table at last measurement was 312 bytes. 27/6/2003
    	};


    /**
     This is the lookup method to determine if the current character being 
     processed needs to be substituted for a glyph in the PUA area given the 
     supplied context. It scans the rule table and returns when it finds it's
     first match. Therefore duplicate match situations must be avoided in
     the rule set logic.
    @param aCode
     On input it is the character to lookup, on exit it is either unchanged
     or a code in the PUA 0xF700..0xF71A.
    @param aGss
     Container object holds the glyph selection context for the method.
    @return TBool
     ETrue when a match is found and aCode has changed, EFalse otherwise.
    */
    TBool Lookup(TUint& aCode, const TGlyphSelectionState& aGss)
    	{
    	const PUASubstTableEntry* tablePtr = RuleTable;
    	while (tablePtr->iOrigGlyph)
    		{
    		if ((aCode == tablePtr->iOrigGlyph) && tablePtr->iRuleFunc(aGss))
    			{
    			aCode = tablePtr->iPUAGlyph;
    			return ETrue; // Rule match, substitute glyph code
    			}
    		tablePtr++;
    		}
    	return EFalse; // No match in table
    	}
    }


// 
//
// GlyphSelector_Thai Class definition
//
//


/**
 This is the default glyph processing method for the Thai characters in the
 range 0x0E00..0x0E7F and is invoked from the Glyph selection algorithm in 
 CFont::GetCharacterPosition() method. It is capable of processing base
 Thai characters as well as Thai combining vowels, signs a tone marks.
@param aGss
 Container object holds the input/output parameters of the method.
@return TBool
 ETrue when glyph cluster updated successfully, EFalse on error condition.
@see 
 The method GlyphSelector_Thai::Process() also handles it for other cases.
*/
TBool GlyphSelector_Thai::Process(TGlyphSelectionState& aGss, RShapeInfo&) 
	{
	// Get the Unicode character codes we need to process the current 
	// glyph and increment the iterator onto th next character.
	TUint prevCode = (aGss.iText.LengthToStart() > 0) ? aGss.iText.Get(-1) : 0xFFFF;
	TUint code = aGss.iText.GetThenNext(); // Inc to next char
	TUint nextCode = !aGss.iText.AtEnd() ? aGss.iText.Get(0) : 0xFFFF;
	
	// Is it a Thai base char or a mark (combining) char?
	if ((aGss.iCats & 0xF0) == TChar::EMarkGroup)
		{
    
		// Thai character is combining mark but first check to see if it
		// follows a Thai base character before processing it.
		if ((aGss.iParam.iOutputGlyphs > 0) && 
			!ThaiGlyph::IsThaiGlyph(prevCode))
			{
			(void) aGss.iText.Prev();
			aGss.iClusterState = TGlyphSelectionState::EGClusterComplete;
			return ETrue;
			}
		
		// Missing base glyph? Insert a dotted circle glyph if true.
		if (aGss.iParam.iOutputGlyphs == 0) 
			{
			if (!aGss.AppendGlyphToCluster(KUnicodeDottedCircle))
				return EFalse;
			aGss.iParam.iPen += aGss.iAdvance;
			}

		// Test if SARA AM follows this current Thai mark, since it is
		// a SPECIAL CASE. If present we need NIKHAHIT glyph before this 
		// current Thai mark char.
		if (nextCode == ThaiGlyph::KSaraAm &&
			(aGss.iParam.iOutputGlyphs == 1) && ThaiGlyph::IsThaiToneMark(code))
			{
			TUint nikhahit = ThaiGlyph::KNikhahit;
			// Check and do PUA glyph substitution on Nikhahit
			ThaiGlyphPUASubstitution::Lookup(nikhahit, aGss);

			if (!aGss.AppendGlyphToCluster(nikhahit))
				return EFalse;
	
			// Check and do PUA glyph substitution on combining mark
			ThaiGlyphPUASubstitution::Lookup(code, aGss);

			// Append the curernt Thai Mark to the output stack of glyphs.
			if (!aGss.AppendGlyphToCluster(code))
				return EFalse;

			// We now need to add SARA AA glyph after the current Thai mark char.
			aGss.iAdvance.iWidth = aGss.iAdvance.iHeight = 0;
			if (!aGss.AppendGlyphToCluster(ThaiGlyph::KSaraAa))
				return EFalse;
			
			// Skip the following SARA AM character since we've added
			// its glyphs to it's previous character's glyph cluster.
			// As we've added a base char to the end of the glyph cluster
			// make sure the pen is moved on by the caller.
			(void) aGss.iText.Next();
			aGss.iPen = TGlyphSelectionState::EPenAdvance_Yes;
			}
		else
			{
			// Check and do PUA glyph substitution on combining mark
			ThaiGlyphPUASubstitution::Lookup(code, aGss);

			// Append the curernt Thai Mark to the output stack of glyphs.
			if (!aGss.AppendGlyphToCluster(code))
				return EFalse;

			aGss.iPen = TGlyphSelectionState::EPenAdvance_No;
			}
		}
	else

		{
		// Thai character is an independent consonant, digit or sign

		// Handle disjoint descender consonants followed by below vowel.
		// In these two cases we substitute consonant with PUA 
		// consonant that the descender removed. Check code not last one.
		if (code == ThaiGlyph::KYoYing && nextCode != 0xffff &&
			        ThaiGlyph::IsThaiDepVowelBelow(nextCode))
			code = ThaiGlyph::KYoYingPua;
		else if (code == ThaiGlyph::KThoThan &&  nextCode != 0xffff &&
			        ThaiGlyph::IsThaiDepVowelBelow(nextCode))
		  	code = ThaiGlyph::KThoThanPua;
			
		// Append the glyph details for the Thai character onto the output 
		// stack of glyphs.
		if (!aGss.AppendGlyphToCluster(code))
			return EFalse;

		// Make sure the caller advances the pen for a base char!
		aGss.iPen = TGlyphSelectionState::EPenAdvance_Yes;
		}

	// Lookup in rule table to determine if the current glyph and cluster is 
	// now complete? 
	if (ThaiCharRules::LookupWtt2Rule(aGss.iCodePt, nextCode) == ThaiCharRules::EComposite)
		aGss.iClusterState = TGlyphSelectionState::EGClusterNotComplete;
	else
		aGss.iClusterState = TGlyphSelectionState::EGClusterComplete;

	return ETrue;
	}


// 
//
// GlyphSelector_ThaiSaraAm Class definition
//
//


/**
 This is the glyph processing method for the Thai SARA AM (U+0E33) character
 which is handled as a special case since it is decomposed into two glyphs 
 - the combining NIKHAHIT mark & SARA AA following vowel in some cases.
 It is invoked from the Glyph selection algorithm in 
 CFont::GetCharacterPosition() method for all cases where SARA AM is not 
 following a tone mark and thus be a glyph cluster of its own.
@param aGss
 Container object holds the input/output parameters of the method.
@return TBool
 ETrue when glyph cluster updated successfully, EFalse on error condition.
@see 
 The method GlyphSelector_Thai::Process() also handles it for other cases.
*/ 
TBool GlyphSelector_ThaiSaraAm::Process(TGlyphSelectionState& aGss, RShapeInfo&)
	{
	if (aGss.iCodePt != ThaiGlyph::KSaraAm) //could have got here via
		{                    //FindLocalisedProcessFunc in font.cpp
		RShapeInfo dummy;
		return GlyphSelector_Thai::Process(aGss, dummy);
		}
	
	// Pen advance accumulator local variable
	TSize compoundAdvance;

	if (aGss.iText.LengthToStart() == 0)
		{
		// If at the start of a line then render it with a preceding 
		// dotted circle as this is invalid positioning for SARA AM. 
		
		if (!aGss.AppendGlyphToCluster(KUnicodeDottedCircle))
			return EFalse;
		aGss.iParam.iPen += aGss.iAdvance;

		aGss.iAdvance.iWidth = aGss.iAdvance.iHeight = 0;
		if (!aGss.AppendGlyphToCluster(ThaiGlyph::KSaraAm))
			return EFalse;
		compoundAdvance += aGss.iAdvance;
		}
	else
		{
		// Normal condition - text iterator now some way into the text line
		// being processed.
		
		TUint prevChar = aGss.iText.Get(-1);
		if (ThaiGlyph::IsThaiShortConsonant(prevChar))
			{
			// SARA AM is following normal height consonant so we can output
			// non-decomposed SARA AM glyph.

			if (!aGss.AppendGlyphToCluster(ThaiGlyph::KSaraAm))
				return EFalse;
			compoundAdvance = aGss.iAdvance;
			}
		else if (ThaiGlyph::IsThaiTallConsonant(prevChar))
			{
			// SARA AM is following tall consonant so we output decomposed
			// version of SARA AM but with NIKHAHIT taken from the PUA.

			if (!aGss.AppendGlyphToCluster(ThaiGlyph::KNikhahitPua))
				return EFalse;
			compoundAdvance = aGss.iAdvance;
			aGss.iAdvance.iWidth = aGss.iAdvance.iHeight = 0;
			if (!aGss.AppendGlyphToCluster(ThaiGlyph::KSaraAa))
				return EFalse;
			compoundAdvance += aGss.iAdvance;
			}
		else
			{
			// SARA AM is a following vowel but is not following a valid
			// consonant char and so default is to render with dotted circle.
			if (!aGss.AppendGlyphToCluster(KUnicodeDottedCircle))
				return EFalse;
			aGss.iParam.iPen += aGss.iAdvance;

			aGss.iAdvance.iWidth = aGss.iAdvance.iHeight = 0;
			if (!aGss.AppendGlyphToCluster(ThaiGlyph::KSaraAm))
				return EFalse;
			compoundAdvance += aGss.iAdvance;
			}

		}

	// Update output parameters resulting from above processing.
	// Move text iterator onto next character to process.
	aGss.iText.Next();

	// Advance pen just for the SARA AA char as advance for dotted 
	// circle is done above.
	aGss.iAdvance = compoundAdvance;
	aGss.iPen = TGlyphSelectionState::EPenAdvance_Yes;

	if (!aGss.iText.AtEnd() &&
		(ThaiCharRules::LookupWtt2Rule(aGss.iCodePt, aGss.iText.Get()) == 
		 ThaiCharRules::EComposite))
		aGss.iClusterState = TGlyphSelectionState::EGClusterNotComplete;
	else
		aGss.iClusterState = TGlyphSelectionState::EGClusterComplete;

	return ETrue;
	}
