// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <openfont.h>
#include "GlyphSel.h"
#include "FontThai.h"
#include "FontArabic.h"
#include "FontIndic.h"
#include "GDIPANIC.h"
#include "glyphsample.h"
#include "gdiinline.inl"
#include "gdistructs.h"
#include "gdiconsts.h"
#include "gdiplatapi.h"

/**
 Names holds the types & data associated with the glyph selection
 algorithm in CFont::GetCharacterPosition().
@internalComponent
*/
namespace GlyphSelection
    {

    typedef TBool (*ProcessFunc)(TGlyphSelectionState& aGss, RShapeInfo&);

    /**
     This structure defines the fields present in each row of the GlyphTable
     datat table below. 
    @internalComponent
    */
    struct TTableEntry 
    	{
    	TUint            iLow;
    	TUint            iHigh;
    	ProcessFunc	 iProcessFunc;
    	};

    /**
     This table encodes the Unicode character ranges and the glyph selector
     classes to be used for each character range when processing characters
     into glyph clusters in CFont::GetCharacterPosition().
     New glyph selection classes must make sure they are listed in this 
     table to ensure they are invoked as required.
     A '0' iProcessFunc entry tells the algorithm to skip the character.
    @internalComponent
    */
    static const TTableEntry Table[] =
    	{
    	//    iLow,     iHigh,    iProcessFunc
    		{ 0x0000,   0x00AC,   GlyphSelector_Default::Process},
    		{ 0x00AD,   0x00AD,   GlyphSelector_SoftHyphen::Process},
    		{ 0x00AE,   0x05FF,   GlyphSelector_Default::Process},
    		{ 0x0600,   0x06FF,   GlyphSelector_Arabic::Process},
    		{ 0x0700,   0x08FF,   GlyphSelector_Default::Process},
    		{ 0x0900,   0x0970,   GlyphSelector_Devanagari::Process},
    		{ 0x0980,   0x09FF,   GlyphSelector_Bengali::Process},
    		{ 0x0A00,   0x0A7F,   GlyphSelector_Gurmukhi::Process},
    		{ 0x0A80,   0x0AFF,   GlyphSelector_Gujarati::Process},
    		{ 0x0B80,   0x0BFF,   GlyphSelector_Tamil::Process},
    		{ 0x0C00,	0x0C7F,   GlyphSelector_Telugu::Process},
    		{ 0x0C80,	0x0CFF,   GlyphSelector_Kannada::Process},
    		{ 0x0D00,   0x0D7F,   GlyphSelector_Malayalam::Process},    		
    		{ 0x0D80,   0x0DFF,   GlyphSelector_Default::Process},
    		{ 0x0E00,   0x0E32,   GlyphSelector_Thai::Process},
    		{ 0x0E33,   0x0E33,   GlyphSelector_ThaiSaraAm::Process},
    		{ 0x0E34,   0x0E7F,   GlyphSelector_Thai::Process},
    		{ 0x0E80,   0x200B,   GlyphSelector_Default::Process},
    		{ 0x200C,   0x200F,   0},
    		{ 0x2010,   0x2029,   GlyphSelector_Default::Process},
    		{ 0x202A,   0x202E,   0},
    		{ 0x202F,   0xFFFD,   GlyphSelector_Default::Process},
    		{ 0xFFFE,   0xFFFF,   0},
    		{ 0x10000,	0x10FFFF, GlyphSelector_Default::Process},
    		{0xFFFFFFFF,0xFFFFFFFF, 0}
    	};
    }

/** Find appropriate processor function for the given character.
@param aChar Character for processing.
@return processor function or 0 if the character is to be skipped.
@internalComponent */
GlyphSelection::ProcessFunc CharacterToProcessFunction(TInt aChar)
	{
	for (const GlyphSelection::TTableEntry* glyphSel = GlyphSelection::Table;
		glyphSel->iLow != 0xFFFFFFFF; glyphSel++)
		{
		if ((glyphSel->iLow <= aChar) && (aChar <= glyphSel->iHigh))
			return glyphSel->iProcessFunc;
		}
	return 0;
	}

/**
@internalTechnology For use by TFontStyle/TOpenFontSpec.
*/
EXPORT_C TBool FontEffect::IsEffectOn(TEffect aEffect, TUint32 aFontEffect)
	{
	return aEffect & aFontEffect;
	}

/**
@internalTechnology For use by TFontStyle/TOpenFontSpec.
*/
EXPORT_C void FontEffect::SetEffect(TEffect aEffect, TBool aOn, TUint32& aFontEffect)
	{
	if (aOn)
		aFontEffect |= aEffect;
	else
		aFontEffect &= ~aEffect;
	}


//
// TFontStyle
//

/** Default C++ constructor. */
EXPORT_C TFontStyle::TFontStyle():
	iFlags(0), iReserved1(0), iReserved2(0)
	{}


/** Constructs a TFontStyle object with the specified attributes.
@param aPost The posture attribute. 
@param aStrWgt The stroke weight attribute. 
@param aPrintPos The print position attribute. */
EXPORT_C TFontStyle::TFontStyle(TFontPosture aPostr,TFontStrokeWeight aWgt,TFontPrintPosition aPos):
	iFlags(0), iReserved1(0), iReserved2(0)
	{
	if (aPostr == EPostureItalic)
		{
		iFlags |= EItalic;
		}
	if (aWgt == EStrokeWeightBold)
		{
		iFlags |= EBold;
		}
	if (aPos == EPrintPosSuperscript)
		{
		iFlags |= ESuper;
		}
	else if (aPos == EPrintPosSubscript)
		{
		iFlags |= ESub;
		}
	}


EXPORT_C void TFontStyle::InternalizeL(RReadStream& aStream)
/** Internalises a font style from a read stream.

The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

@param aStream The stream from which the font style is to be internalised 
@leave KErrNoMemory If there is a problem reading from the stream.
If internalisation causes an out of memory error. */
	{
	iFlags = aStream.ReadUint32L();
	}


EXPORT_C void TFontStyle::ExternalizeL(RWriteStream& aStream) const
/** Externalises the font style to a write stream.

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class.

@param aStream The stream to which the font style is to be externalised.
@leave KErrNoMemory This function may leave, if the write action causes the 
stream's resources to be exhausted. */
	{
	aStream.WriteUint32L(iFlags);
	}


EXPORT_C TFontPosture TFontStyle::Posture() const
/** Gets the posture attribute.

@return The font style's posture. */
	{
	if(iFlags&EItalic) return(EPostureItalic);
	return(EPostureUpright);
	}


EXPORT_C TFontStrokeWeight TFontStyle::StrokeWeight() const
/** Gets the stroke weight attribute.

@return The font style's stroke weight. */
	{
	if(iFlags&EBold) return(EStrokeWeightBold);
	return(EStrokeWeightNormal);
	}


EXPORT_C TFontPrintPosition TFontStyle::PrintPosition() const
/** Gets the print position attribute.

@return The font style's print position. */
	{
	if((iFlags&ESuper) && !(iFlags&ESub)) return(EPrintPosSuperscript);
	else if((iFlags&ESub) && !(iFlags&ESuper)) return(EPrintPosSubscript);
	return(EPrintPosNormal);
	}


EXPORT_C void TFontStyle::SetPosture(TFontPosture aPosture)
/** Sets the posture attribute.

@param aPosture The posture to be set. */
	{
	if(aPosture==EPostureItalic) iFlags|=EItalic;
	else iFlags&=~EItalic;
	}


EXPORT_C void TFontStyle::SetStrokeWeight(TFontStrokeWeight aStrokeWeight)
/** Sets the stroke weight attribute.

@param aStrokeWeight The stroke weight to be set. */
	{
	if(aStrokeWeight==EStrokeWeightBold) iFlags|=EBold;
	else iFlags&=~EBold;
	}


EXPORT_C void TFontStyle::SetPrintPosition(TFontPrintPosition aPrintPosition)
/** Sets the print position attribute.

@param aPrintPosition The print position to be set. */
	{
	switch(aPrintPosition)
		{
		case EPrintPosSuperscript:
			{
			iFlags|=ESuper;
			iFlags&=~ESub;
			break;
			}
		case EPrintPosSubscript:
			{
			iFlags&=~ESuper;
			iFlags|=ESub;
			break;
			}
		default:
			{
			iFlags&=~ESuper;
			iFlags&=~ESub;
			}
	 	}
	}

/** Gets the font effects flags.
@publishedAll
@released
@return The font effects flags.
@see TFontStyle::SetEffects()
*/
EXPORT_C TUint32 TFontStyle::Effects() const
	{
	return 0xFFF0 & iFlags;
	}

/** Checks if a font effect is on.
@publishedAll
@released
@return True represents the specified font effect is on, otherwise off.
@param aEffect The font effect to be checked.
@see TFontStyle::SetEffects()
*/
EXPORT_C TBool TFontStyle::IsEffectOn(FontEffect::TEffect aEffect) const
	{
	return FontEffect::IsEffectOn(aEffect, iFlags);
	}

/** Sets the font effects flags.
@publishedAll
@released
@param aEffect The font effects flags to be set.
@see TFontStyle::Effects()
*/
EXPORT_C void TFontStyle::SetEffects(TUint32 aEffects)
	{
	iFlags &= 0xFFFF000F;
	iFlags |= 0xFFF0 & aEffects;
	}

/** Sets a font effect to the given state.
@publishedAll
@released
@param aEffect The font effect to be set.
@param aOn True represents on, otherwise off.
@see TFontStyle::IsEffectOn()
*/
EXPORT_C void TFontStyle::SetEffects(FontEffect::TEffect aEffect, TBool aOn)
	{
	FontEffect::SetEffect(aEffect, aOn, iFlags);
	}

/** Compares a font style for equality.
@publishedAll
@released
@param aFontStyle The font style to be compared with this font style.
@return ETrue, if this TFontStyle is equal to aFontStyle, EFalse, otherwise.
*/
EXPORT_C TBool TFontStyle::operator==(const TFontStyle& aFontStyle) const
	{
	return iFlags == aFontStyle.iFlags;
	}

//
// TFontSpec
//
EXPORT_C TFontSpec::TFontSpec():
	iTypeface(),
	iHeight(0),
	iFontStyle()
/** Default constructor.

The object's font style is set to the default: EPostureUpright, EStrokeWeightNormal, 
and EPrintPosNormal. */
	{}


EXPORT_C TFontSpec::TFontSpec(const TDesC& aTypefaceName,TInt aHeight):
	iTypeface(),
	iHeight(aHeight),
	iFontStyle(EPostureUpright,EStrokeWeightNormal,EPrintPosNormal)
/** Constructs a TFontSpec object with the specified typeface and height. 

The object's font style is set to the default: EPostureUpright, EStrokeWeightNormal, 
and EPrintPosNormal.

@param aTypefaceName The name of the typeface (e.g. "Roman"). It should be no
	longer than KMaxTypefaceNameLength characters in length.
@param aHeight The height of the typeface, in twips. 
@panic GDI 6, if aTypefaceName is more than KMaxTypefaceNameLength characters long.
*/
	{
	iTypeface.SetName(aTypefaceName);
	}


EXPORT_C TBool TFontSpec::operator==(const TFontSpec& aFontSpec) const
/** Compares this font specification with another.
@param aFontSpec The font specification to be compared with this one. 
@return ETrue, if the TFontSpecs are identical, EFalse otherwise.
*/
	{
	return
		iHeight		== aFontSpec.iHeight &&
		iFontStyle	== aFontSpec.iFontStyle &&
		iTypeface	== aFontSpec.iTypeface;
	}


EXPORT_C void TFontSpec::InternalizeL(RReadStream& aStream)
/** Internalises a font specification from a read stream.

The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

@param aStream The stream from which the font specification is to be internalised.
@leave KErrNoMemory If internalisation causes an out of memory error. */
	{
	iTypeface.InternalizeL(aStream);
	iHeight=aStream.ReadUint16L();
	iFontStyle.InternalizeL(aStream);
	}


EXPORT_C void TFontSpec::ExternalizeL(RWriteStream& aStream) const
/** Externalises the font specification to a write stream.

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class.

@param aStream The stream to which the font specification is to be externalised 

@leave KErrNoMemory If the write action causes the stream's resources to be 
exhausted. */
	{
	iTypeface.ExternalizeL(aStream);
	aStream.WriteUint16L(iHeight);
	iFontStyle.ExternalizeL(aStream);
	}

EXPORT_C void TFontSpec::SetScriptTypeForMetrics(TLanguage aLanguage)
/** Specifies the script with which font metrics calculation will be based on.
@param aLanguage The language used to derive the required script.
@publishedAll
@released
*/
	{
	iTypeface.SetScriptTypeForMetrics(aLanguage);
	}

EXPORT_C TInt TFontSpec::ScriptTypeForMetrics() const
/** Returns the script with which font metrics calculation will be based on.
@internalTechnology
*/
	{
	return iTypeface.ScriptTypeForMetrics();
	}

//
// TTypeface
//
static const TInt KTTypefaceBitsNumAttrib = 3;
static const TInt KTTypefaceBitsNumScript = 4;
static const TInt KTTypefaceMaskAttrib = (1 << KTTypefaceBitsNumAttrib) - 1;
static const TInt KTTypefaceMaskScript = ((1 << KTTypefaceBitsNumScript) - 1) << KTTypefaceBitsNumAttrib;
EXPORT_C TTypeface::TTypeface():
	iName(),
	iFlags(0)
/** Default C++ constructor. */
	{}

/**
@internalComponent
*/
void TTypeface::ResetAttributes()
	{
	iFlags &= KTTypefaceMaskScript;
	}

/**
@internalComponent
*/
void TTypeface::ResetScriptType()
	{
	iFlags &= KTTypefaceMaskAttrib;
	}

EXPORT_C void TTypeface::InternalizeL(RReadStream& aStream)
/** Internalises a typeface from a read stream. 

The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

@param aStream Stream from which the typeface is to be internalised. */
	{
	TBuf<KMaxTypefaceNameLength> tempname;
	aStream >> tempname;
	new(&iName) TBufC<KMaxTypefaceNameLength>(tempname);
	iFlags = aStream.ReadInt8L();
	}


EXPORT_C void TTypeface::ExternalizeL(RWriteStream& aStream) const
/** Externalises a typeface to a write stream. 

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class.

@param aStream The stream to which the typeface is to be externalised. */
	{
	aStream << iName;
	aStream.WriteInt8L(static_cast<TInt8>(iFlags));
	}


EXPORT_C TBool TTypeface::operator==(const TTypeface& aTypeface) const
/** Compares two typefaces for equality.

@param aTypeface The typeface to be compared with. 
@return ETrue, if this TTypeface is equal to aTypeface, otherwise EFalse. */
	{
	return
		iFlags == aTypeface.iFlags &&
		iName == aTypeface.iName;
	}

EXPORT_C void TTypeface::SetAttributes(TInt aAttributes)
/** Set the combination of attributes for this typeface.

@param aAttributes A bitmap defining the combination of attributes. */
	{
	ResetAttributes();
	iFlags |= KTTypefaceMaskAttrib & aAttributes & (EProportional | ESerif | ESymbol);
	}


EXPORT_C void TTypeface::SetIsProportional(TBool aIsProportional)
/** Sets the typeface's proportional attribute.

@param aIsProportional ETrue if the typeface is a proportional typeface, otherwise 
EFalse. */
	{
	if (aIsProportional)
		{
		iFlags |= EProportional;
		}
	else
		{
		iFlags &= ~EProportional;
		}
	}


EXPORT_C void TTypeface::SetIsSerif(TBool aIsSerif)
/** Sets the typeface's serif attribute.

@param aIsSerif ETrue if the typeface is a serif typeface, otherwise EFalse. */
	{
	if (aIsSerif)
		{
		iFlags |= ESerif;
		}
	else
		{
		iFlags &= ~ESerif;
		}
	}


EXPORT_C void TTypeface::SetIsSymbol(TBool aIsSymbol)
/** Sets the typeface's symbol attribute.

@param aIsSymbol ETrue if the typeface is a symbol typeface, otherwise EFalse. */
	{
	if (aIsSymbol)
		{
		iFlags |= ESymbol;
		}
	else
		{
		iFlags &= ~ESymbol;
		}
	}


EXPORT_C TInt TTypeface::Attributes() const
/** Gets the combination of attributes of the typeface.

@return The combination of attributes of the typeface. */
	{
	return KTTypefaceMaskAttrib & iFlags;
	}


EXPORT_C TBool TTypeface::IsProportional() const
/** Gets the typeface's proportional attribute.

@return ETrue if the typeface is proportional, EFalse otherwise. */
	{
	return KTTypefaceMaskAttrib & iFlags & EProportional;
	}


EXPORT_C TBool TTypeface::IsSerif() const
/** Gets the typeface's serif attribute.

@return ETrue if the typeface is a serif typeface, EFalse otherwise */
	{
	return KTTypefaceMaskAttrib & iFlags & ESerif;
	}


EXPORT_C TBool TTypeface::IsSymbol() const
/** Gets the typeface's symbol attribute.

@return ETrue if the typeface is a symbol typeface, EFalse otherwise */
	{
	return KTTypefaceMaskAttrib & iFlags & ESymbol;
	}


/** Specifies the script with which font metrics calculation will be based on.
@param aLanguage The language used to derive the required script.
@internalTechnology
*/
EXPORT_C void TTypeface::SetScriptTypeForMetrics(TLanguage aLanguage)
	{
	SetScriptTypeForMetrics(GlyphSample::TLanguage2TScript(aLanguage));
	}

/** Specifies the script with which font metrics calculation will be based on.
@param aScript The script.
@internalTechnology
*/
EXPORT_C void TTypeface::SetScriptTypeForMetrics(TInt aScript)
	{
	ResetScriptType();
	iFlags |= KTTypefaceMaskScript & (aScript << KTTypefaceBitsNumAttrib);
	}

/** Gets the script with which font metrics calculation will be based on.
@return The script.
@internalTechnology
*/
EXPORT_C TInt TTypeface::ScriptTypeForMetrics() const
	{
	return (KTTypefaceMaskScript & iFlags) >> KTTypefaceBitsNumAttrib;
	}

/**
Sets the name of the typeface. This method should be used rather than
directly accessing the iName public member.
@param aName The name of the typeface (e.g. "Roman"). It should be no 
	longer than KMaxTypefaceNameLength characters in length.
@panic GDI 6, if aName is more than KMaxTypefaceNameLength characters
	long.
*/
EXPORT_C void TTypeface::SetName(const TDesC& aName)
	{
	GDI_ASSERT_ALWAYS(aName.Length() <= KMaxTypefaceNameLength, EGdiPanic_TypefaceNameOverflow);
	iName=aName;
	}

/**
Returns the name of the typeface.
@return The name of the typeface.
*/
EXPORT_C const TDesC& TTypeface::Name() const
	{
	return iName;
	}


//
// CFont
//

/** Default destructor. */
EXPORT_C CFont::~CFont()
	{}

_LIT(KGdiZeroCharacter,"0");

/** Gets the width of the zero character of this font in pixels. 

This function is provided as the "0" character is roughly the average width 
of the characters of any font.

@return The width of the "0" character, in pixels. */
EXPORT_C TInt CFont::WidthZeroInPixels() const
	{
	return(TextWidthInPixels(KGdiZeroCharacter));
	}


/** Gets the font descent in pixels.
It is defined to be HeightInPixels() minus AscentInPixels().
Note that this deprecated function is replaced by the new @c FontMaxDescent()
or in some cases @c FontStandardDescent().

@return The font descent in pixels.
@see FontStandardDescent() 
@see FontMaxDescent()
@deprecated */
EXPORT_C TInt CFont::DoDescentInPixels() const
	{
	return HeightInPixels() - AscentInPixels();
	}


/** Checks to see if the pen position needs to be included in the bounds
calculation for purposes of considering side-bearings in the line break point

@param aInput The input block. Contains the check flag and maxbounds.
@param aPenPos The current value of the pen position.
@param aBoundsBR Bottom-right bounds value.
@param aBoundsTL Top-left bounds value.
@return Whether or not MaxBounds has been exceeded
*/
LOCAL_C TBool BoundsExceeded(const CFont::TMeasureTextInput& aInput,
	const TInt& aPenPos, TInt& aBoundsBR, TInt& aBoundsTL)
	{
	if (aInput.iFlags & CFont::TMeasureTextInput::EFIncludePenPositionInBoundsCheck)
		{
		if (aInput.iFlags & CFont::TMeasureTextInput::EFVisualOrderRightToLeft)
			{
			aBoundsTL = Min(aBoundsTL, aPenPos);
			}
		else
			{
		 	aBoundsBR = Max(aBoundsBR, aPenPos);
			}
		}
 	return (aBoundsBR - aBoundsTL > aInput.iMaxBounds);
	}


/** Text measurement function.

This is a powerful text measurement function underlying all the
other text measurement functions. It takes optional input and output
parameter blocks, which may be null, and returns the advance 
width (change in pen position when drawn horizontally) of the text, or the advance
height, if the text is drawn vertically.

Some of the functions that can be performed using this
function are listed below. Many of them are used by the Text Views
API to do its typographic layout.
- Get the advance width or advance height (return value).
The advance width is the amount by which the pen advances when drawing
the text horizontally, while the advance height is the amount by which 
the pen advances when drawing the text vertically.
- Measure some text in context, so that shaping behaviour
(e.g. in Arabic) can be affected by what comes before and after the
text. Do this using TMeasureTextInput::iStartInputChar and 
TMeasureTextInput::iEndInputChar to tell the function where to start and end 
in the supplied descriptor.
- Determine how much text fits a given size by setting 
TMeasureTextInput::iMaxAdvance or TMeasureTextInput::iMaxBounds. 
- Specify letter spacing and word spacing using TMeasureTextInput::iCharJustNum,
TMeasureTextInput::iCharJustExcess, 
TMeasureTextInput::iWordJustNum and 
TMeasureTextInput::iWordJustExcess. 
- Get the number of characters drawn in TMeasureTextOutput::iChars 
when applying the various constraints in TMeasureTextInput. 
- Get the number of glyphs drawn in TMeasureTextOutput::iGlyphs. 
- Get the number of groups (formed by ligation or diacritic placement) in 
TMeasureTextOutput::iGroups. Groups are units of cursor
movement: the cursor hops over a character-plus-accent group or an
Arabic or other ligature in one go.
- Get the number of word spaces in TMeasureTextOutput::iSpaces. 
- Get the bounds of the inked-in pixels in TMeasureTextOutput::iBounds. 
- Get the size of the biggest glyph that would be drawn in TMeasureTextOutput::iMaxGlyphSize.

@param aText The text to be measured.
@param aInput The input block. This may be NULL.
@param aOutput The output block. This may be NULL.
@return The advance width if the text is drawn horizontally or the advance 
height if the text is drawn vertically. 

@panic GDI 1 In debug builds only, if TMeasureTextInput::iStartInputChar is negative.
*/
EXPORT_C TInt CFont::MeasureText(const TDesC& aText,const TMeasureTextInput* aInput,TMeasureTextOutput* aOutput) const
	{
	TMeasureTextInput input;
	if (aInput)
		input = *aInput;
	if (aOutput)
		{
		Mem::FillZ(aOutput,sizeof(*aOutput));
		aOutput->iChars = input.iStartInputChar;
		}
	TPositionParam param;
	param.iDirection = input.iDirection;
 
 	TBool vertical = param.iDirection == EVertical;
 	TBool penMovesLeft = EFalse;
 	if (input.iFlags & TMeasureTextInput::EFVisualOrderRightToLeft)
 		{
 		if (!vertical)
 			penMovesLeft = ETrue;
 		param.iFlags |= TPositionParam::EFLogicalOrder;
 		}
 	else if (!(input.iFlags & TMeasureTextInput::EFVisualOrder))
   		param.iFlags |= TPositionParam::EFLogicalOrder;


	param.iText.Set(aText);

	int advance = 0;
	int groups = 0;
	int spaces = 0;
	param.iPosInText = input.iStartInputChar;
	int end_char = Min(aText.Length(),input.iEndInputChar);
	TRect bounds;
	// Total advance if pen is moving left. Positive.
 	TInt rightToLeftAdvance = 0;
	// Shaping information of the text
	RShapeInfo shapeInfo;
	while (param.iPosInText < end_char)
		{
		if (!GetCharacterPosition2(param, shapeInfo))
			{
			if (aOutput)
				aOutput->iChars = param.iPosInText;
			continue;
			}

		int new_advance = vertical ? param.iPen.iY : param.iPen.iX;
		if (input.iCharJustExcess != 0)
			new_advance += CGraphicsContext::JustificationInPixels(input.iCharJustExcess,input.iCharJustNum,groups,1);
		groups++;
		// Allow justification to occur at spaces
		if (param.iOutput[0].iCode == 0x0020)
			{
			if (input.iWordJustExcess != 0)
				new_advance += CGraphicsContext::JustificationInPixels(input.iWordJustExcess,input.iWordJustNum,spaces,1);
			spaces++;
			}
		if (vertical)
			param.iPen.iY = new_advance;
		else
			param.iPen.iX = new_advance;
		
		if (penMovesLeft)
 			{
 			// If the pen is moving left, we will begin each cluster at (0,0)
 			// and shift the bounds to the right to compensate.
 			bounds.iTl.iX += param.iPen.iX;
 			bounds.iBr.iX += param.iPen.iX;
 			bounds.iTl.iY += param.iPen.iY;
 			bounds.iBr.iY += param.iPen.iY;
 			rightToLeftAdvance += param.iPen.iX;
 			new_advance = rightToLeftAdvance;
 			param.iPen.iX = 0;
 			param.iPen.iY = 0;
 			}

		if (aInput || aOutput)
			{
			const TPositionParam::TOutput* output = param.iOutput;
			for (int i = 0; i < param.iOutputGlyphs; i++, output++)
				{
				//if (!output->iBounds.IsEmpty()) -- optimized to:
				if (output->iBounds.iTl.iX != output->iBounds.iBr.iX
					|| output->iBounds.iTl.iY != output->iBounds.iBr.iY)
					{
					if (aOutput)
						{
						// increase iMaxGlyphSize if either dimension smaller than
						// current glyph
						TInt boundsDim = output->iBounds.iBr.iX - output->iBounds.iTl.iX;
						aOutput->iMaxGlyphSize.iWidth = aOutput->iMaxGlyphSize.iWidth < boundsDim?
							boundsDim : aOutput->iMaxGlyphSize.iWidth;
						boundsDim = output->iBounds.iBr.iY - output->iBounds.iTl.iY;
						aOutput->iMaxGlyphSize.iHeight = aOutput->iMaxGlyphSize.iHeight < boundsDim?
							boundsDim : aOutput->iMaxGlyphSize.iHeight;
						}
					//bounds.BoundingRect(output->iBounds); -- optimized to:
					if (output->iBounds.iTl.iX < bounds.iTl.iX)
						bounds.iTl.iX = output->iBounds.iTl.iX;
					if (bounds.iBr.iX < output->iBounds.iBr.iX)
						bounds.iBr.iX = output->iBounds.iBr.iX;
					if (output->iBounds.iTl.iY < bounds.iTl.iY)
						bounds.iTl.iY = output->iBounds.iTl.iY;
					if (bounds.iBr.iY < output->iBounds.iBr.iY)
						bounds.iBr.iY = output->iBounds.iBr.iY;
					}
				}

			// Would any limits be exceeded by adding this group?
			if (param.iPosInText > end_char)
				break;
			if (new_advance > input.iMaxAdvance)
				break;
			if (vertical)
				{
				if (BoundsExceeded(input, param.iPen.iY, bounds.iBr.iY, bounds.iTl.iY))
					break;
				}
			else
				{
				if (BoundsExceeded(input, param.iPen.iX, bounds.iBr.iX, bounds.iTl.iX))
					break;
				}

			if (aOutput)
				{
				aOutput->iChars = param.iPosInText;		// should this not be aOutput->iChars = param.iPosInText - input.iShartInputChar;?
				aOutput->iGlyphs += param.iOutputGlyphs;
				aOutput->iGroups = groups;
				aOutput->iSpaces = spaces;
				aOutput->iBounds = bounds;
				}
			}

		advance = new_advance;
		}
	if(shapeInfo.IsOpen())
		shapeInfo.Close();
	return advance;
	}

// These 3 functions should probably be moved to E32/Euser as part of TChar or
// similar as there seem to be several local copies of similar functions in
// various OS modules so we should remove duplication

TUint16 HighSurrogate(TUint aCode)
	{
	GDI_ASSERT_DEBUG(aCode  > 0xFFFF, EGdiPanic_InvalidInputParam);
	return STATIC_CAST(TUint16, 0xD7C0 + (aCode >> 10));
	}
	
TUint16 LowSurrogate(TUint aCode)
	{
	GDI_ASSERT_DEBUG(aCode  > 0xFFFF, EGdiPanic_InvalidInputParam);
	return STATIC_CAST(TUint16, 0xDC00 | (aCode & 0x3FF));
	}
	
TUint CombineSurrogates(TUint aHighSurrogate, TUint aLowSurrogate)
	{
	GDI_ASSERT_DEBUG((0xD800 == (aHighSurrogate & 0xF800)), EGdiPanic_InvalidInputParam);
	GDI_ASSERT_DEBUG((0xD800 == (aHighSurrogate & 0xFC00)), EGdiPanic_InvalidInputParam);
	GDI_ASSERT_DEBUG((0xDC00 == (aLowSurrogate & 0xFC00)), EGdiPanic_InvalidInputParam);
	return ((aHighSurrogate - 0xD7F7) << 10) + aLowSurrogate;
	}


/** Overridable function innards of GetCharacterPosition and
GetCharacterPosition2. It is generally not useful to override this function.
@publishedAll
@see GetCharacterPosition
@see GetCharacterPosition2
*/
EXPORT_C TBool CFont::DoGetCharacterPosition(TPositionParam& aParam) const
	{
	RShapeInfo shapeInfo;
	TBool r = GetCharacterPosition2(aParam, shapeInfo);
	if (shapeInfo.IsOpen())
		shapeInfo.Close();
	return r;
	}

// Find the script (and hence the correct process function) that any punctuation or digit may belong to
LOCAL_C GlyphSelection::ProcessFunc FindContextualProcessFunc(RShapeInfo& aShapeInfo, const TGlyphSelectionState aGss)
	{
	GlyphSelection::ProcessFunc processFunc = CharacterToProcessFunction(aGss.iCodeChar);
	GlyphSelection::ProcessFunc contextProcessFunc = (GlyphSelection::ProcessFunc)aShapeInfo.GetContext();
	
	// If context or prevCode is NULL, use processFunc,
	// else use function of context or prevCode
	if ((aGss.iCodeChar.IsDigit() || aGss.iCodeChar.IsPunctuation()) && !QuoteOrBracketPair(aGss.iCodeChar) && processFunc!=GlyphSelector_SoftHyphen::Process)
		{
		// If context is not set, check the previous char for context.
		if (contextProcessFunc == NULL)
			{
			if (aGss.iParam.iPosInText > 0)
				{
				TChar prevCode = aGss.iText.Get(-1);
				GlyphSelection::ProcessFunc prevProcessFunc = CharacterToProcessFunction(prevCode);
				if (prevProcessFunc != NULL && (prevCode.IsAlpha() || prevProcessFunc != GlyphSelector_Default::Process))
					{
					aShapeInfo.SetContext((TAny *)prevProcessFunc);
					return prevProcessFunc;
					}
				}
			} 
		else 
			return contextProcessFunc;
		
		return processFunc;
		}
	
	// set the context with current processFunc only if current char is not ignored for context.
 	if (processFunc != NULL && (aGss.iCodeChar.IsAlpha() || processFunc != GlyphSelector_Default::Process))
 			aShapeInfo.SetContext((TAny *)processFunc);
 	return processFunc;
	}

/** Takes Unicode text and produces the glyph cluster for the first character
in that text plus any combining mark characters, or for the first indic
syllable. It is responsible for contextual glyph selection, ligature creation
and diacritic placement.

@param aParam
	The input/output parameter of the text/glyph data for the algorithm.
@param aShapeInfo
	The function will cache "shaped" text (e.g. complex scripts such as
	Devanagari) here. aShapeInfo must be freshly-constructed or closed for each
	new piece of text in aParam.iText. If aParam.iText is unchanged between
	calls, aShapeInfo should be passed back in unchanged as well.
@return
	ETrue if glyphs for supplied text have been produced, EFalse in failure.
@see CFont::TPositionParam
@publishedAll
@released */
EXPORT_C TBool CFont::GetCharacterPosition2(TPositionParam& aParam, RShapeInfo& aShapeInfo) const
	{
	GDI_ASSERT_DEBUG(aParam.iPosInText>=0, EGdiPanic_InvalidInputParam);
	GDI_ASSERT_DEBUG(aParam.iText.Ptr(), EGdiPanic_InvalidInputParam);

	aParam.iOutputGlyphs = 0;
	TInt textLen = aParam.iText.Length();
	TBool outputOk = ETrue;
	TPoint penCopy = aParam.iPen;

	// Verify input parameters are sane
	if (aParam.iPosInText >= textLen)
		return EFalse;

	// Setup glyph selection algorithm data
	TUtf32Iterator textIter(aParam.iText.Ptr(), aParam.iText.Ptr()+textLen, aParam.iPosInText);
	if (textIter.AtEnd())
		{
		aParam.iPosInText = textIter.LengthToStart();
		return outputOk;
		}

	// Process each character in the text in turn until we reach the end of
	// the iterator, the next base (non-mark/combining) character or reach
	// the limit in a glyph cluster.
	GlyphSelection::ProcessFunc firstProcessFn = 0;
	TGlyphSelectionState gss(textIter, this, aParam);
	do
		{
		// Retrieve character info for processing.
		gss.iCodePt = gss.iCodeChar = textIter.Get();
		gss.iCombCls = gss.iCodeChar.GetCombiningClass();
		gss.iCats = gss.iCodeChar.GetCategory();
		gss.iClusterState = TGlyphSelectionState::EGClusterNotComplete;
		gss.iPen = TGlyphSelectionState::EPenAdvance_No;

		// Find the correct processesing function for the script being used.
		// If gss.iCodePt is a strongly directional character, then simply map it in TTableEntry Table[]
		// and use the returned process function pointer.
		// If gss.iCodePt is a punctuation or a digit, then use a context character in the text (if
		// available) to find the contextual script being rendered and use its process function pointer.
		GlyphSelection::ProcessFunc processFn = FindContextualProcessFunc(aShapeInfo, gss);

		if (!firstProcessFn)
			firstProcessFn = processFn;

		if (processFn)
			{
			if (firstProcessFn == processFn)
				outputOk = processFn(gss, aShapeInfo);
			else
				break;
			}
		else
			{
			// Table entry blank, unicode char to be skipped
			outputOk = ETrue;
			textIter.Next();
			gss.iClusterState = 
				(!textIter.AtEnd() &&
				((textIter.Get().GetCategory() & 0xF0) 
				== TChar::EMarkGroup)) ?
					TGlyphSelectionState::EGClusterNotComplete : TGlyphSelectionState::EGClusterComplete;
			}

		// Abort if no class was available to process the character or if
		// processing failed.
		if (!outputOk)
			{
			aParam.iPosInText = textIter.LengthToStart();
			return outputOk;
			}

		// Did the glyph selector that processed the character want the 
		// pen to advance?
		if (gss.iPen == TGlyphSelectionState::EPenAdvance_Yes)
			{
			aParam.iPen.iX += gss.iAdvance.iWidth;
			aParam.iPen.iY += gss.iAdvance.iHeight;
			gss.iPen = TGlyphSelectionState::EPenAdvance_No;
			}

		// Here we assume the Process() methods have advanced the iterator as
		// they consume characters they handle so that it now points to the
		// character to process next time around the loop.
		}
	while (!textIter.AtEnd()	// We still have more text to process
		&& (gss.iClusterState == TGlyphSelectionState::EGClusterNotComplete) // Glyph cluster !complete
		&& (aParam.iOutputGlyphs < TPositionParam::EMaxOutputGlyphs)); // Room for another glyph entry

	// If a complete glyph cluster has been identified then we should try to
	// compose it as fully as possible. Obviously, if it only contains one
	// character then it is already fully composed so we can ignore it.
	// Skip this if any language-specific processing has taken place.
	if (gss.iGlyphPostCombine == TGlyphSelectionState::EGPostCombine_Yes
		&& gss.iClusterState == TGlyphSelectionState::EGClusterComplete)
		{
		// Leave room to handle surrogates - Decompose() outputs UTF-16
		// The max that can come out of the previous stage is TPositionParam::EMaxOutputGlyphs
		// long with only one base char at the start. Even if that base char decomposed to the
		// max it could only be MaxOutputGlyphs long, giving a total of (2 * MaxOutputGlyphs)-1
		// Conceivably the use of surrogates throughout could double that when converting to UTF-16
		TBuf<TPositionParam::EMaxOutputGlyphs * 4> decomposeArray;
		TBool success = ETrue;
		// Go through the glyph cluster one char at a time
		for (TInt i = 0; i < aParam.iOutputGlyphs; i++)
			{
			TChar singleChar(aParam.iOutput[i].iCode);
			// If first character try to decompose it otherwise just take the character
			TBool decomposed = EFalse;
			TPtrC16 decomposition;
			if (i == 0)
				decomposed = singleChar.Decompose(decomposition);
			if (decomposed)
				{ // Pick up the sequence of characters
				decomposeArray.Append(decomposition);
				}
			else
				{ // Keep the original character
				if (singleChar > 0xFFFF)
					{ // Above the BMP so we need a surrogate pair for UTF-16
					// This calculation really ought to go into a separate routine - probably part of TChar
					decomposeArray.Append(HighSurrogate(singleChar));
					decomposeArray.Append(LowSurrogate(singleChar));
					}
				else
					{ // It's not a surrogate so we just need to cast it down (since it's safe)
					decomposeArray.Append(singleChar);
					}
				}
			// Guard against bad input overflowing the array and causing a panic
			if (decomposeArray.Length() > (TPositionParam::EMaxOutputGlyphs * 4) - 2)
				{ // too long to be a viable composition so don't try
				success = EFalse;
				break;
				}
			}
		TUint composedChar = 0;
		TOpenFontCharMetrics metrics;
		TPositionParam::TOutput output;
		TSize advance; // gets initialized to 0,0
		if (success)
			{
			//Now try and compose the string to a single character
			success = TChar::Compose(composedChar, decomposeArray);
			}
		if (success)
			{
			// if single char is not in font or can't get char metrics for it
			// N.B. This will probably always return metrics because if the
			// char is not in the font this will usually return the substitute
			// "missing" glyph (and its metrics). There should be a function to
			// really tell you if a glyph is in the font - but there isn't.
			if (GetCharacterData(composedChar, metrics, output.iBitmap, output.iBitmapSize) == CFont::ENoCharacterData)
				success = EFalse;
			}
		if (success)
			{
			// We should replace the glyph cluster made from multiple chars
			// with the correct single char and fix up the rest of the output
			// parameters as well
			output.iCode = composedChar;
			// Set the glyph's bounds and record pen advancement.
			if (aParam.iDirection == CFont::EVertical)
				{
				metrics.GetVertBounds(output.iBounds);
				advance.iHeight = metrics.VertAdvance();
				}
			else
				{
				metrics.GetHorizBounds(output.iBounds);
				advance.iWidth = metrics.HorizAdvance();
				}
			// Adjust the glyph's bounding box to offset it from the pen
			// position (origin of drawing). For speed increment directly.
			output.iBounds.iTl.iX += penCopy.iX;
			output.iBounds.iBr.iX += penCopy.iX;
			output.iBounds.iTl.iY += penCopy.iY;
			output.iBounds.iBr.iY += penCopy.iY;
			// Set penCopy, the copy of aParam.iPen that we made
			penCopy.iX += advance.iWidth;
			penCopy.iY += advance.iHeight;
			// Overwrite the original output parameters for the glyph cluster
			// with the values for the single composed character
			aParam.iOutput[0] = output;
			aParam.iOutputGlyphs = 1;
			aParam.iPen = penCopy;	
			}
		}

	// Exit routine with result and increment position in text to 
	// where we reached during processing to avoid any caller loops from 
	// infinite execution.
	aParam.iPosInText = textIter.LengthToStart();
	return outputOk;
	}

/** Gets the character metrics for a character.
	
@param aCode The character code.
@param aMetrics On return, contains the character bitmap.
@param aBitmap On return, this points to NULL.
@param aBitmapSize On return, this has a size of (0,0).
@return ECharacterWidthOnly 
*/
EXPORT_C CFont::TCharacterDataAvailability CFont::DoGetCharacterData(TUint aCode,TOpenFontCharMetrics& aMetrics,
		const TUint8*& aBinaryData,TSize& aBitmapSize) const
	{
	int width = CharWidthInPixels(aCode);
	aMetrics.SetHorizAdvance(width);
	aBinaryData = NULL;
	// For speed set to 0 directly rather than call SetSize()
	aBitmapSize.iWidth = 0;
	aBitmapSize.iHeight = 0;

	/*
	Set the other metrics using the width and font metrics.
	This allows derived classes that don't override this function, like CInfoFont,
	to give usable results for TextWidthInPixels and MeasureText.
	*/
	aMetrics.SetWidth(width);
	int height = HeightInPixels();
	aMetrics.SetHeight(height);
	aMetrics.SetVertAdvance(height);
	aMetrics.SetHorizBearingX(0);
	aMetrics.SetHorizBearingY(AscentInPixels());
	aMetrics.SetVertBearingX(0);
	aMetrics.SetVertBearingY(0);

	return CFont::ECharacterWidthOnly;
	}


/** Determines if aLeftCharacter and aRightCharacter affect each other's
contextual glyph form if placed next to each other. If either character
is a combining character, EFalse will be returned, which is not generally
useful information. Pass in base characters ignoring intervening combining
characters.
@param aLeftCharacter Unicode code for the character that stands on the left.
@param aRightCharacter Unicode code for the character that stands on the right.
@return EFalse if the characters do not affect the contextual glyphs that are
be chosen when the two are rendered together, compared to being separated 
(for example by a space). */	
EXPORT_C TBool CFont::CharactersJoin(TInt aLeftCharacter, TInt aRightCharacter)
	{
	return GlyphSelector_Arabic::CharactersJoin(aLeftCharacter, aRightCharacter);
	}

/** API extension system that enables the caller to access a particular API
extension function. N.B. Any overload of this function in a derived class
should call its immediate parent implementation for any extension function Uid
that it does not recognize and handle.
@param aInterfaceId UID of the required extension function
@param aParam Pointer to an arbitrary parameter block that can be used to
provide and/or return information to/from the particular extension function,
defaults to NULL.
@return Integer return value from extension function
@internalTechnology
@released
*/
EXPORT_C TInt CFont::DoExtendedFunction(TUid aFunctionId, TAny* /* aParam */) const
	{
	if (KFontCapitalAscent	== aFunctionId ||
		KFontMaxAscent		== aFunctionId)
		{
		return AscentInPixels();
		}
	else if (KFontStandardDescent	== aFunctionId ||
			 KFontMaxDescent		== aFunctionId)
		{
		return DescentInPixels();
		}
	else if (KFontLineGap == aFunctionId)
		{ // 1.2 of em height (rounded) is reasonable approximation of interline gap
		return (HeightInPixels() * 12 + 5) / 10;
		}
	return KErrNotFound;
	}

EXPORT_C TUid CFont::TypeUid() const
	{
	return DoTypeUid();
	}

EXPORT_C TInt CFont::HeightInPixels() const
	{
	return DoHeightInPixels();
	}

EXPORT_C TInt CFont::AscentInPixels() const
	{
	return DoAscentInPixels();
	}

EXPORT_C TInt CFont::DescentInPixels() const
	{
	return DoDescentInPixels();
	}

EXPORT_C TInt CFont::CharWidthInPixels(TChar aChar) const
	{
	return DoCharWidthInPixels(aChar);
	}

EXPORT_C TInt CFont::TextWidthInPixels(const TDesC& aText) const
	{
	return DoTextWidthInPixels(aText);
	}

EXPORT_C TInt CFont::BaselineOffsetInPixels() const
	{
	return DoBaselineOffsetInPixels();
	}

EXPORT_C TInt CFont::TextCount(const TDesC& aText,TInt aWidthInPixels) const
	{
	return DoTextCount(aText, aWidthInPixels);
	}

EXPORT_C TInt CFont::TextCount(const TDesC& aText, TInt aWidthInPixels, TInt& aExcessWidthInPixels) const
	{
	return DoTextCount(aText, aWidthInPixels, aExcessWidthInPixels);
	}

EXPORT_C TInt CFont::MaxCharWidthInPixels() const
	{
	return DoMaxCharWidthInPixels();
	}

EXPORT_C TInt CFont::MaxNormalCharWidthInPixels() const
	{
	return DoMaxNormalCharWidthInPixels();
	}

EXPORT_C TFontSpec CFont::FontSpecInTwips() const
	{
	return DoFontSpecInTwips();
	}
	
/** Gets the character metrics for a character.
	
@param aCode The character code.
@param aMetrics On return, contains the character bitmap.
@param aBitmap On return, this points to NULL.
@param aBitmapSize On return, this has a size of (0,0).
@return ECharacterWidthOnly 
*/
EXPORT_C CFont::TCharacterDataAvailability CFont::GetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics, const TUint8*& aBitmap, TSize& aBitmapSize) const
	{
	return DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
	}

/** Transforms one cluster of characters (base character plus combining marks,
ligature or indic syllable) into one cluster of glyphs together with their
positions. Repeated calls of this function (for the same input text) are
considerably slower than repeated calls of GetCharacterPosition2 for Indic text
(such as Hindi), as GetCharacterPosition2 can cache information between calls.
@param aParam Input and output parameters
@return True for success
@see GetCharacterPosition2
@publishedAll */
EXPORT_C TBool CFont::GetCharacterPosition(TPositionParam& aParam) const
	{
	return DoGetCharacterPosition(aParam);
	}

/** Enables the caller to access a particular API
extension function. N.B. Any overload of this function in a derived class
should call its immediate parent implementation for any extension function UID
that it does not recognize and handle.
@param aFunctionId UID of the required extension function
@param aParam Pointer to an arbitrary parameter block that can be used to
provide and/or return information to/from the particular extension function,
defaults to NULL.
@return Integer return value from extension function 
*/
EXPORT_C TInt CFont::ExtendedFunction(TUid aFunctionId, TAny* aParam) const
	{
	return DoExtendedFunction(aFunctionId, aParam);
	}
	
EXPORT_C TInt CFont::TextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam) const
	{
	TTextWidthInternal context;
	TTextWidthInternal* contextPtr = &context;
	contextPtr->iText.Set(aText);
	contextPtr->iParam.iStartInputChar = aParam->iStartInputChar;
	contextPtr->iParam.iEndInputChar = aParam->iEndInputChar;
	return DoExtendedFunction(KTextInContextWidthInPixelsUid, (TAny*)contextPtr);
	}

/**
Maps TLanguage to TScript.

EScriptOther represents languages not yet supported in KTScript2GlyphSample.
This array does not handle ELangNone and ELangMaximum to save storage space.
*/
const TInt GlyphSample::KTLanguage2TScript[] = 
	{
	EScriptNone,			// 00 ELangTest
	EScriptLatin,			// 01 ELangEnglish
	EScriptLatin,			// 02 ELangFrench
	EScriptLatin,			// 03 ELangGerman
	EScriptLatin,			// 04 ELangSpanish
	EScriptLatin,			// 05 ELangItalian
	EScriptLatin,			// 06 ELangSwedish
	EScriptLatin,			// 07 ELangDanish
	EScriptLatin,			// 08 ELangNorwegian
	EScriptLatin,			// 09 ELangFinnish
	EScriptLatin,			// 10 ELangAmerican
	EScriptLatin,			// 11 ELangSwissFrench
	EScriptLatin,			// 12 ELangSwissGerman
	EScriptLatin,			// 13 ELangPortuguese
	EScriptLatin,			// 14 ELangTurkish
	EScriptLatin,			// 15 ELangIcelandic
	EScriptCyrillic,		// 16 ELangRussian
	EScriptLatin,			// 17 ELangHungarian
	EScriptLatin,			// 18 ELangDutch
	EScriptLatin,			// 19 ELangBelgianFlemish
	EScriptLatin,			// 20 ELangAustralian
	EScriptLatin,			// 21 ELangBelgianFrench
	EScriptLatin,			// 22 ELangAustrian
	EScriptLatin,			// 23 ELangNewZealand
	EScriptLatin,			// 24 ELangInternationalFrench
	EScriptLatin,			// 25 ELangCzech
	EScriptLatin,			// 26 ELangSlovak
	EScriptLatin,			// 27 ELangPolish
	EScriptLatin,			// 28 ELangSlovenian
	EScriptHanIdeographs,	// 29 ELangTaiwanChinese
	EScriptHanIdeographs,	// 30 ELangHongKongChinese
	EScriptHanIdeographs,	// 31 ELangPrcChinese
	EScriptHanIdeographs,	// 32 ELangJapanese
	EScriptThai,			// 33 ELangThai
	EScriptLatin,			// 34 ELangAfrikaans
	EScriptLatin,			// 35 ELangAlbanian
	EScriptOther,			// 36 ELangAmharic
	EScriptArabic,			// 37 ELangArabic
	EScriptOther,			// 38 ELangArmenian
	EScriptOther,			// 39 ELangTagalog
	EScriptCyrillic,		// 40 ELangBelarussian
	EScriptOther,			// 41 ELangBengali
	EScriptCyrillic,		// 42 ELangBulgarian
	EScriptOther,			// 43 ELangBurmese
	EScriptLatin,			// 44 ELangCatalan
	EScriptLatin,			// 45 ELangCroatian
	EScriptLatin,			// 46 ELangCanadianEnglish
	EScriptLatin,			// 47 ELangInternationalEnglish
	EScriptLatin,			// 48 ELangSouthAfricanEnglish
	EScriptLatin,			// 49 ELangEstonian
	EScriptArabic,			// 50 ELangFarsi
	EScriptLatin,			// 51 ELangCanadianFrench
	EScriptLatin,			// 52 ELangScotsGaelic
	EScriptOther,			// 53 ELangGeorgian
	EScriptGreek,			// 54 ELangGreek
	EScriptGreek,			// 55 ELangCyprusGreek
	EScriptOther,			// 56 ELangGujarati
	EScriptHebrew,			// 57 ELangHebrew
	EScriptDevanagari,		// 58 ELangHindi
	EScriptLatin,			// 59 ELangIndonesian
	EScriptLatin,			// 60 ELangIrish
	EScriptLatin,			// 61 ELangSwissItalian
	EScriptOther,			// 62 ELangKannada
	EScriptCyrillic,		// 63 ELangKazakh
	EScriptOther,			// 64 ELangKhmer
	EScriptHanIdeographs,	// 65 ELangKorean
	EScriptOther,			// 66 ELangLao
	EScriptLatin,			// 67 ELangLatvian
	EScriptLatin,			// 68 ELangLithuanian
	EScriptCyrillic,		// 69 ELangMacedonian
	EScriptLatin,			// 70 ELangMalay
	EScriptOther,			// 71 ELangMalayalam
	EScriptDevanagari,		// 72 ELangMarathi
	EScriptLatin,			// 73 ELangMoldavian
	EScriptOther,			// 74 ELangMongolian
	EScriptLatin,			// 75 ELangNorwegianNynorsk
	EScriptLatin,			// 76 ELangBrazilianPortuguese
	EScriptOther,			// 77 ELangPunjabi
	EScriptLatin,			// 78 ELangRomanian
	EScriptCyrillic,		// 79 ELangSerbian
	EScriptOther,			// 80 ELangSinhalese
	EScriptLatin,			// 81 ELangSomali
	EScriptLatin,			// 82 ELangInternationalSpanish
	EScriptLatin,			// 83 ELangLatinAmericanSpanish
	EScriptLatin,			// 84 ELangSwahili
	EScriptLatin,			// 85 ELangFinlandSwedish
	EScriptNone,			// 86 ELangReserved1
	EScriptOther,			// 87 ELangTamil
	EScriptOther,			// 88 ELangTelugu
	EScriptOther,			// 89 ELangTibetan
	EScriptOther,			// 90 ELangTigrinya
	EScriptLatin,			// 91 ELangCyprusTurkish
	EScriptCyrillic,		// 92 ELangTurkmen
	EScriptCyrillic,		// 93 ELangUkrainian
	EScriptArabic,			// 94 ELangUrdu
	EScriptNone,			// 95 ELangReserved2
	EScriptLatin,			// 96 ELangVietnamese
	EScriptLatin,			// 97 ELangWelsh
	EScriptLatin,			// 98 ELangZulu
	};

/**
Maps TScript to glyph samples.

The order of samples definition has to follow the script order in TScript.

Supported scripts	Fonts used to experiment/determine glyph samples

Latin				Arial, Times, Century
Greek				Ditto
Cyrillic			Ditto
Hebrew				Aharoni, David, FrankRuehl, Levenim MT, Miriam, Narkisim, Rod
Arabic				Andalus, Arabic Transparent, Simplified Arabic, Traditional Arabic
Devanagari			Mangal
Thai				Angsana New, Browallia, Cordia New, DilleniaUPC, EucrosiaUPC,
					FreesiaUPC, IrisUPC, JasmineUPC, KodchiangUPC, LilyUPC
HanIdeographs		Chinese	: SimSun, SimHei (Simplified) / MingLiU (Traditional)
					Japanese: MS Mincho, MS Gothic
					Korean	: Batang, Gulim
*/
const TText* const GlyphSample::KTScript2GlyphSample[] = 
	{
	//
	// 02 EScriptLatin
	//
	//	0x00C0 - Ascent : Capital letter A with grave (Latin-1 Supplement)
	//	0x013A - Ascent : Small letter l with acute (Latin Extended A)
	//	0x1EA2 - Ascent : Capital letter A with hook above (Latin Extended Additional)
	//	0x00C7 - Descent: Capital letter C with cedilla (Latin-1 Supplement)
	//	0x0163 - Descent: Small letter t with cedilla (Latin Extended A)
	//
	_S("\x00C0\x013A\x1EA2\x00C7\x0163"),
	//
	// 03 EScriptGreek
	//
	//	0x03AA - Ascent : Capital letter iota with dialytika
	//	0x03AB - Ascent : Capital letter upsilon with dialytika
	//	0x03AE - Descent: Small letter eta with tonos
	//	0x03B2 - Descent: Small letter beta
	//	0x03C8 - Descent: Small letter psi
	//
	_S("\x03AA\x03AB\x03AE\x03B2\x03C8"),
	//
	// 04 EScriptCyrillic
	//
	//	0x0403 - Ascent : Capital letter gje
	//	0x0419 - Ascent : Capital letter short i
	//	0x0440 - Descent: Small letter er
	//	0x0452 - Descent: Small letter dje
	//	0x0458 - Descent: Small letter je
	//
	_S("\x0403\x0419\x0440\x0452\x0458"),
	//
	// 05 EScriptHebrew
	//
	//	0x05BE - Ascent : Punctuation maqaf
	//	0x05DC - Ascent : Letter lamed
	//	0x05B0 - Descent: Point sheva
	//	0x05BD - Descent: Point meteg
	//	0x05E7 - Descent: Letter qof
	//
	_S("\x05BE\x05DC\x05B0\x05BD\x05E7"),
	//
	// 06 EScriptArabic
	//
	// 	0x0670 - Ascent : Alef above (Arabic)
	// 	0x0671 - Ascent : Hamzat Wasl on Alef  isolated form
	// 	0x064D - Descent: Kasratan (Arabic)
	// 	0xFB7B - Descent: Final form of 0686
	// 	0xFBF2 - Descent: Final form of 064A
	//
	//PDEF120737: EScriptArabic value has been changed for this defect & tested using the font file provided by client (i.e. kamelion arabic font).
	//The client's font file can't be used for IPR reasons. Thus the test code to demonstrate this defect
	//is not added. Also, there was no other font file available that reproduces this defect. 
	//
	_S("\x0670\x0671\x064D\xFB7B\xFBF2"),
	//
	// 07 EScriptDevanagari
	//
	//	0x0914 - Ascent : Letter au
	//	0x0951 - Ascent : Stress sign udatta
	//	0x0941 - Descent: Vowel sign u
	//	0x0944 - Descent: Vowel sign rr
	//	0x0963 - Descent: Vowel sign vocalic ll
	//
	_S("\x0914\x0951\x0941\x0944\x0963"),
	//
	// 08 EScriptThai
	//
	//	0x0E49 - Ascent : Character mai tho
	//	0x0E4B - Ascent : Character mai chattawa
	//	0x0E0E - Descent: Character do chada
	//	0x0E24 - Descent: Character ru
	//	0x0E39 - Descent: Character sara uu
	//
	_S("\x0E49\x0E4B\x0E0E\x0E24\x0E39"),
	//
	// 09 EScriptHanIdeographs
	//
	//	0x1100 - Ascent/Descent: Korean
	//	0x4E1C - Ascent/Descent: Chinese Simplified
	//	0x5283 - Ascent/Descent: Japanese
	//	0x758A - Ascent : Chinese Traditional
	//	0x7BEA - Descent: Chinese Traditional
	//
	_S("\x1100\x4E1C\x5283\x758A\x7BEA"),
	};

/**
Maps a TLanguage type to the TScript type.
@internalTechnology
@param aLanguage The language.
@return A TInt representing the script, or 
EScriptNone if its not defined for aLanguage.
*/
EXPORT_C TInt GlyphSample::TLanguage2TScript(TLanguage aLanguage)
	{
	if (ELangNone == aLanguage || ELangMaximum == aLanguage || aLanguage >= (sizeof(KTLanguage2TScript)/sizeof(KTLanguage2TScript[0])))
		{
		return EScriptNone;
		}
	return KTLanguage2TScript[aLanguage];
	}

/**
Maps a TScript type to some glyph samples which are stored as Unicode.
@internalTechnology
@param aScript The script.
@return A TPtrC pointing to the first glyph sample,
or empty if no samples is defined for aScript.
*/
EXPORT_C const TPtrC GlyphSample::TScript2GlyphSample(TInt aScript)
	{
	if (EScriptOther >= aScript)
		{
		return TPtrC();
		}
	// -3 to offset EScriptDefault, EScriptNone and EScriptOther
	// being the first three elements in TScript.
	return TPtrC(KTScript2GlyphSample[aScript - 3]);
	}


EXPORT_C RFontTable::RFontTable():iTableContent(0), iLength(0),
        iFont(NULL), iTag(0)
    {
    // a null constructor.
    }

EXPORT_C TInt
RFontTable::Open(CFont& aFont, TUint32 aTag) 
    {
    TGetFontTableParam param;
    param.iTag = aTag;
    
    // remember the parameters, to be used when releasing the font table.
    iFont = &aFont;
    iTag = aTag;
    
    TInt ret = aFont.ExtendedFunction(KFontGetFontTable, (TAny *)&param);
    if (KErrNone == ret)
        {
        iTableContent = (TAny *)param.iContent;
        iLength = param.iLength;
        }
    return ret;
    }

EXPORT_C TInt 
RFontTable::TableLength() const
    {
    return iLength;
    }

EXPORT_C const TUint8*
RFontTable::TableContent() const 
    {
    return (TUint8*)iTableContent;
    }

EXPORT_C void
RFontTable::Close()
    {
    if (NULL != iFont)
        {
        (void)iFont->ExtendedFunction(KFontReleaseFontTable, (TAny *)&iTag);
        }
    iTableContent = 0;
    iLength = 0;
    iFont = NULL;
    iTag = 0;
    }

EXPORT_C 
RGlyphOutlineIterator::RGlyphOutlineIterator():iOutlines(0), iLengths(0), 
    iCursor(-1), iCount(0), iFont(NULL), iCodes(NULL), iHinted(EFalse)
    {
    // a null constructor.
    }

EXPORT_C TInt
RGlyphOutlineIterator::Open(CFont& aFont, const TUint* aCodes, TInt aCount, TBool aHinted) 
    {
    if (NULL == aCodes || 0 == aCount)
        {
        return KErrArgument;
        }
    TGetGlyphOutlineParam param;
    iLengths = (TInt *)User::Alloc(sizeof(TInt) * aCount);
    if (NULL == iLengths) 
        {
        return KErrNoMemory;
        }
    iOutlines = (TAny **)User::Alloc(sizeof(TAny *) * aCount);
    if (NULL == iOutlines)
        {
        User::Free(iLengths);
        iLengths = NULL;
        return KErrNoMemory;
        }
    
    param.iLengths = iLengths; 
    param.iCount = aCount;
    param.iCodes = aCodes; 
    param.iHinted = aHinted;
    param.iOutlines = iOutlines; 
        
    /* information needed in Close() */
    iCodes = (TUint *)User::Alloc(sizeof(TUint) * aCount);
    if (NULL == iCodes) 
        {
        User::Free(iLengths);
        User::Free(iOutlines);
        iLengths = NULL;
        iOutlines = NULL;
        return KErrNoMemory;
        }
    Mem::Copy(iCodes, aCodes, aCount*sizeof(TUint));
    iFont = &aFont;
    iHinted = aHinted;
    iCount = aCount;
    
    TInt ret = aFont.ExtendedFunction(KFontGetGlyphOutline, (TAny *)&param);
    if (KErrNone != ret)
        {
        User::Free(iLengths);
        User::Free(iOutlines);
        User::Free(iCodes);
        iLengths = NULL;
        iOutlines = NULL;
        iCodes = NULL;
        iFont = NULL;
        }
    else 
        {
        iCursor = 0;
        }

    return ret;
    }

EXPORT_C const TUint8*
RGlyphOutlineIterator::Outline() const
    {
    GDI_ASSERT_ALWAYS(iCursor >= 0, EGdiPanic_Unknown);
    
    if (iLengths[iCursor] < 0) 
        {
        return NULL;
        }
    else 
        {
        return (const TUint8*)iOutlines[iCursor];
        }
    }

EXPORT_C TInt 
RGlyphOutlineIterator::OutlineLength() const 
    {
    GDI_ASSERT_ALWAYS(iCursor >= 0, EGdiPanic_Unknown);
    
    if (iLengths[iCursor] < 0) 
        {
        return KErrGeneral;
        }
    else 
        {
        return iLengths[iCursor];
        }
    }

EXPORT_C TInt
RGlyphOutlineIterator::Next()
    {
    if (iCursor >= 0 && iCursor < iCount-1) 
        {
        ++iCursor;
        return KErrNone;
        }
    else
        {
        iCursor = -1; 
        // if the iterator goes beyond the last element [when
        // Next() returns KErrNotFound], the next call
        // to Outline() or OutlineLength() will panic.
        
        return KErrNotFound;
        }
    }

EXPORT_C void
RGlyphOutlineIterator::Close()
    {
    TReleaseGlyphOutlineParam param;
    param.iCount = iCount;
    param.iHinted = iHinted;
    param.iCodes = iCodes;
    
    if (NULL != iFont)
        {
        iFont->ExtendedFunction(KFontReleaseGlyphOutline, (TAny *)&param);
        }
    
    iFont = NULL;
    iCount = 0;
    User::Free(iLengths);
    iLengths = NULL;
    iCursor = -1;
    User::Free(iCodes);
    iCodes = NULL;
    User::Free(iOutlines);
    iOutlines = NULL;
    }
