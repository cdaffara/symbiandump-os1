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

#ifndef __GDI_H__
#define __GDI_H__

#include <e32base.h>
#include <f32file.h>
#include <s32std.h>
#include <displaymode.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS	
#include <graphics/gdi/glyphsample.h>
#include <graphics/gdi/gdiconsts.h>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS	

class TOpenFontCharMetrics;
class RShapeInfo;
class CGraphicsContext;
class TTextParameters;

/**
Number of twips per inch. 
@publishedAll
@released
*/
const TInt KTwipsPerInch=1440;

/**
Number of twips per point.
@publishedAll
@released
*/
const TInt KTwipsPerPoint=20;

/**
Number of points per inch. 
@publishedAll
@released
*/
const TInt KPointsPerInch=72;

/**
Number of twips per cm.
@publishedAll
@released
*/
const TInt KTwipsPerCm=567;
#if defined(__NO_CLASS_CONSTS__)
/**
A4 paper size in twips. 
@publishedAll
@released
*/
#define KA4PaperSizeInTwips TSize(11906,16838)

/** Legal paper size in twips.
@publishedAll
@released
*/
#define KLegalPaperSizeInTwips TSize(12240,20160)

/**
Executive paper size in twips. 
@publishedAll
@released
*/
#define KExecutivePaperSizeInTwips TSize(10440,15120)
/** 
Letter paper size in twips. 
@publishedAll
@released
*/
#define KLetterPaperSizeInTwips TSize(12240,15840)

/**
Com-10 paper size in twips. 
@publishedAll
@released
*/
#define KCom_10PaperSizeInTwips TSize(5940,13680)

/**
Monarch paper size in twips. 
@publishedAll
@released
*/
#define KMonarchPaperSizeInTwips TSize(5580,10800)

/**
DL paper size in twips. 
@publishedAll
@released
*/
#define KDLPaperSizeInTwips TSize(6236,12472)

/**
C5 paper size in twips. 
@publishedAll
@released
*/
#define KC5PaperSizeInTwips TSize(9184,12983)
#else
/**
@publishedAll
@released
*/
const TSize KA4PaperSizeInTwips(11906,16838);
const TSize KLegalPaperSizeInTwips(12240,20160);
const TSize KExecutivePaperSizeInTwips(10440,15120);
const TSize KLetterPaperSizeInTwips(12240,15840);
const TSize KCom_10PaperSizeInTwips(5940,13680);
const TSize KMonarchPaperSizeInTwips(5580,10800);
const TSize KDLPaperSizeInTwips(6236,12472);
const TSize KC5PaperSizeInTwips(9184,12983);
#endif


/**
This enumeration holds the possible panic codes that may be raised 
by the GDI API on detecting an unrecoverable error. */
enum TGdiPanic
	{
	/** Not used */
	EGdiPanic_Unknown				= 0,
	/** One or more of the input parameters to the interface were invalid */
	EGdiPanic_InvalidInputParam		= 1,
	/** Insufficient text for successful completion of the method */
	EGdiPanic_OutOfText				= 2,
	/** Internal failure. */
	EGdiPanic_Invariant				= 3,
	/** Unused panic codes. Can be reused if needed. */
	EGdiPanic_Unused1				= 4,
	EGdiPanic_Unused2				= 5,
	/** Setting a typeface name that is too long */
	EGdiPanic_TypefaceNameOverflow	= 6,
	};


/** 24-bit RGB colour value with 8 bits each for red, green and blue.

All Graphics drawing functions are specified in terms of a 32-bit TRgb colour 
containing the three colour values plus 8 bits for alpha channel. For hardware which 
does not support 24-bit colour, a mapping from TRgb to display colours is 
performed. 

Generally, the convention for the alpha blending fact is 0 = transparent, 
255 = opaque, unless otherwise stated.  The exception to this are the TRgb constructor 
taking a single value, where the top byte of the passed in parameter is used for 
alpha information and the function Value(), which returns alpha information in the top byte.
In both these cases, 0 means opaque, 255 means transparent.

The supported display modes are enumerated in the TDisplayMode type. In each 
display mode a unique index can represent each physical colours supported, 
and which can be mapped onto a full RGB value. The mappings are as follows: 

16-colour displays use the EGA colour set: black, white, and then both light 
and dark versions of grey, red, green, blue, cyan, magenta and yellow

256-colour displays support 216 colours made up of 6x6x6 RGB values, each 
containing all possible multiples of 51 for R,G,B values. Additionally, all 
remaining 10 shades of pure red, green, blue and grey are represented, by 
adding all remaining multiples of 17. This use of 256 colours is sometimes 
known as the Netscape colour cube.

4096-colour displays effectively support RGB values with 4 bits per primary 
colour

64k-colour displays effectively support RGB values with 5 bits allocated to 
red, 6 to green and 5 to blue

16 million-colour displays support true colour with 8 bits allocated to each 
primary colour

@publishedAll
@released
@see TDisplayMode
@see DynamicPalette */
class TRgb
	{
public:
	inline TRgb();
	inline TRgb(TUint32 aValue);
	inline TRgb(TUint32 aInternalValue, TInt aAlpha);
	inline TRgb(TInt aRed,TInt aGreen,TInt aBlue);
	inline TRgb(TInt aRed, TInt aGreen, TInt aBlue, TInt aAlpha);
	inline TInt Red() const;
	inline TInt Green() const;
	inline TInt Blue() const;
	inline TInt Alpha() const;
	IMPORT_C void SetRed(TInt aRed);
	IMPORT_C void SetGreen(TInt aGreen);
	IMPORT_C void SetBlue(TInt aBlue);
	IMPORT_C void SetAlpha(TInt aAlpha);
	IMPORT_C static TRgb Gray2(TInt aGray2);
	IMPORT_C static TRgb Gray4(TInt aGray4);
	IMPORT_C static TRgb Gray16(TInt aGray16);
	IMPORT_C static TRgb Gray256(TInt aGray256);
	IMPORT_C static TRgb Color16(TInt aColor16);
	IMPORT_C static TRgb Color256(TInt aColor256);
	IMPORT_C static TRgb Color4K(TInt aColor4K);
	IMPORT_C static TRgb Color64K(TInt aColor64K);
	IMPORT_C static TRgb Color16M(TInt aColor16M);
	IMPORT_C TInt Gray2() const;
	IMPORT_C TInt Gray4() const;
	IMPORT_C TInt Gray16() const;
	IMPORT_C TInt Gray256() const;
	IMPORT_C TInt Color16() const;
	IMPORT_C TInt Color256() const;
	IMPORT_C TInt Color4K() const;
	IMPORT_C TInt Color64K() const;
	IMPORT_C TInt Color16M() const;
	inline TBool operator==(const TRgb& aColor) const;
	inline TBool operator!=(const TRgb& aColor) const;
	inline TRgb operator~() const;
	inline TRgb operator&(const TRgb& aColor);
	inline TRgb operator|(const TRgb& aColor);
	inline TRgb operator^(const TRgb& aColor);
	inline TRgb& operator&=(const TRgb& aColor);
	inline TRgb& operator|=(const TRgb& aColor);
	inline TRgb& operator^=(const TRgb& aColor);
	inline TUint32 Value() const;
	inline TUint32 Internal() const;
	inline void SetInternal(TUint32 aInternal);
	IMPORT_C TInt Difference(const TRgb& aColor) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C static TRgb Color16MU(TInt a0RGB);
	IMPORT_C TInt Color16MU() const;
	IMPORT_C static TRgb Color16MA(TUint aARGB);
	IMPORT_C TUint Color16MA() const;
	IMPORT_C static TRgb Color16MAP(TUint aARGB);
	IMPORT_C TUint Color16MAP() const;
	IMPORT_C TUint _Color16MAP() const;
	IMPORT_C static TRgb _Color16MAP(TUint aARGB);
	inline TInt _Gray2() const;
	inline TInt _Gray4() const;
	inline TInt _Gray16() const;
	inline TInt _Gray256() const;
	inline TInt _Color4K() const;
	inline TInt _Color64K() const;
	inline TInt _Color16M() const;
	inline TInt _Color16MU() const;
	inline TUint _Color16MA() const;
	inline static TRgb _Gray2(TInt aGray2);
	inline static TRgb _Gray4(TInt aGray4);
	inline static TRgb _Gray16(TInt aGray16);
	inline static TRgb _Gray256(TInt aGray256);
	inline static TRgb _Color4K(TInt aColor4K);
	inline static TRgb _Color64K(TInt aColor64K);
	inline static TRgb _Color16M(TInt aColor16M);
	inline static TRgb _Color16MU(TInt a0RGB);
	inline static TRgb _Color16MA(TUint aARGB);
private:
	TUint32 iValue;
	};


/**
@publishedAll
@released
*/
#define KRgbBlack		TRgb(0x000000)
#define KRgbDarkGray	TRgb(0x555555)
#define KRgbDarkRed		TRgb(0x000080)
#define KRgbDarkGreen	TRgb(0x008000)
#define KRgbDarkYellow	TRgb(0x008080)
#define KRgbDarkBlue	TRgb(0x800000)
#define KRgbDarkMagenta	TRgb(0x800080)
#define KRgbDarkCyan	TRgb(0x808000)
#define KRgbRed			TRgb(0x0000ff)
#define KRgbGreen		TRgb(0x00ff00)
#define KRgbYellow		TRgb(0x00ffff)
#define KRgbBlue		TRgb(0xff0000)
#define KRgbMagenta		TRgb(0xff00ff)
#define KRgbCyan		TRgb(0xffff00)
#define KRgbGray		TRgb(0xaaaaaa)
#define KRgbWhite		TRgb(0xffffff)
#define KRgbTransparent	TRgb(0x000000,0x00)

/** A set of static utility functions to get information about a display mode. 
@publishedAll 
@released
*/
class TDisplayModeUtils
	{
public:
	IMPORT_C static TBool IsDisplayModeColor(TDisplayMode aDispMode);
	IMPORT_C static TBool IsDisplayModeValid(TDisplayMode aDispMode);
	IMPORT_C static TInt NumDisplayModeColors(TDisplayMode aDispMode);
	IMPORT_C static TInt NumDisplayModeBitsPerPixel(TDisplayMode aDispMode);
	};

/** Provides user-definable palette support to the GDI.

A palette is a user-defined set of colours, which is a subset of the full 
range of 24-bit colours. This allows users the advantages of having a low 
bpp colour mode whilst being able to specify the colours available in that 
mode. To give an example, the EColor16 mode provides a palette of 16 colours 
as it provides a mapping between an integer index and a TRgb colour (see the 
table EGA Low-colour constants). Only a palette of 16 colour enables you to 
change the palette. Palettes are also used to allow 24-bit bitmaps to be stored 
in a more compressed form by finding the actual number of different colours 
used in the bitmap, creating a palette to allow the mapping of these colours 
to a smaller index space, and encoding the bitmaps pixels using indexes 
to this new index space. 

A palette has a size which is set at its creation and cannot be altered 
the number of entries in the palette. Each entry in a palette is a mapping 
between that entrys index and a TRgb value. Palette entries can be got 
and set at any time between the palettes creation and destruction. The 
GDIs palette support also provides functions to find the nearest palette 
colour to a requested TRgb colour. 
@publishedAll
@released
*/
class CPalette : public CBase
	{
public:
	IMPORT_C static CPalette* NewL(TInt aNumberOfEntries);
	IMPORT_C static CPalette* NewDefaultL(TDisplayMode aDispMode);
	IMPORT_C ~CPalette();
	IMPORT_C void Clear();
	inline TInt Entries() const;
	IMPORT_C TRgb GetEntry(TInt aPaletteIndex) const;
	IMPORT_C TRgb NearestEntry(const TRgb& aColor) const;
	IMPORT_C TInt NearestIndex(const TRgb& aColor) const;
	IMPORT_C void SetEntry(TInt aPaletteIndex,const TRgb& aPaletteEntry);
	IMPORT_C void GetDataPtr(TInt aFirstColor,TInt aNumColors,TPtr8& aPtr);
protected:
	IMPORT_C CPalette();
	void ConstructL(TInt aNumberOfEntries);
protected:
	TRgb* iArray;
	TInt iNumEntries;
	};


/** Enables conversion, in both directions, between a TRgb object and an index 
into an arbitrary 256 colour palette. 
@publishedAll 
@released
*/
class TColor256Util
	{
public:
	IMPORT_C void Construct(const CPalette& aPalette);
	IMPORT_C TInt Color256(TRgb aRgb) const;
	IMPORT_C void Color256(TUint8* aDestination,const TRgb* aSource,TInt aNumPixels) const;
	inline TRgb Color256(TInt aColor256) const;
	IMPORT_C static const TColor256Util* Default();
public:
	/** 256 colour lookup table.
	
	Each entry is a 32 bit value which corresponds to a TRgb value in the 
	palette passed to Construct(). If there are more than 256 colours in the 
	palette, the first 256 colours are used in this table. If there are fewer 
	than 256 entries, the remaining entries in the table are set to zero. */
	TUint32	iColorTable[256];

	/** Inverse colour lookup table.
	
	It has 4096 entries. Each entry is the index of a colour in the palette 
	that the object was created with (see Construct()) that most closely 
	matches the 4096 degrees of intensity of red, green and blue on a uniform 
	16x16x16 colour cube.
	
	It is called "inverse" because iColorTable maps indices (0..255) to TRgb 
	values, but this table maps TRgb values to palette indices. */
	TUint8	iInverseColorTable[0x1000];
	};


/** Linear digital differential analyser.

This is used to calculate the pixels which most closely approximate a specified 
straight line, or when scaling a bitmap. Note that a line is infinitely thin, 
and can only be approximated by pixels with real width and height.

Functions are provided for: pixel line traversing; jumping to a rectangle or 
co-ordinate 
@publishedAll 
@released
*/
class TLinearDDA
	{
public:
	/** LDDA Line mode. */
	enum TLineMode
		{
		/** Centres scan-lines in the pixel line */
		ECenter,
		/** Starts at the beginning of a complete scan line. Used for bitmap 
		scaling. */
		ELeft
		};
public:
	IMPORT_C TLinearDDA();
	IMPORT_C TLinearDDA(const TLinearDDA& aLine);
	IMPORT_C void Construct(const TPoint& aStart,const TPoint& aFinish,TLineMode aMode=ECenter);
	IMPORT_C TBool SingleStep(TPoint& aPosition);
	IMPORT_C TBool SingleScanline(TPoint& aStartPosition,TPoint& aEndPosition);
	IMPORT_C TBool NextStep(TPoint& aPosition);
	IMPORT_C void JumpToRect(const TRect& aRect);
	IMPORT_C void JumpToXCoord(const TInt aXCoord,TInt& aYCoord);
	IMPORT_C void JumpToYCoord(TInt& aXCoord,const TInt aYCoord);
	IMPORT_C void JumpToXCoord2(TInt aXCoord,TInt& aYCoord);
	IMPORT_C void JumpToYCoord2(TInt& aXCoord,TInt aYCoord);
private:
	void UpdatePosition();
private:
	enum TLineStatus
		{
		EInitialised,
		ECurrent,
		EComplete
		};
private:
	TInt iCount;
	TSize iDifference;
	TPoint iFinish;
	TInt iGradient;
	TPoint iInc;
	TPoint iPos;
	TPoint iStart;
	TRect iBoundingRect;
	TBool iBoundingRectSet;
	TInt16 iInsideX; // boolean, defined as TInt16 to maintain binary compatibility
	TInt16 iInsideY; // boolean, defined as TInt16 to maintain binary compatibility
	TLineStatus iStatus;
	};


/**
Font posture flags.
Fonts can be either upright or italic. 
@publishedAll	
@released
*/
enum TFontPosture
	{
	/** Font posture is normal (upright). */
	EPostureUpright,
	/** Font posture is italic. */
	EPostureItalic
	};

/**
Font stroke weight flags. 
@publishedAll	
@released
*/
enum TFontStrokeWeight
	{
	/** Font stroke weight is normal. */
	EStrokeWeightNormal,
	/** Font stroke weight is bold. */
	EStrokeWeightBold
	};

/**
Font print position flags.
Fonts can be normal, superscript or subscript. 
@publishedAll	
@released
*/
enum TFontPrintPosition
	{
	/** Font is normal. */
	EPrintPosNormal,
	/** Font is superscript. */
	EPrintPosSuperscript,
	/** Font is subscript. */
	EPrintPosSubscript
	};

/**
Font underline flags. 
@publishedAll	
@released
*/
enum TFontUnderline
	{
	/** Font is not underlined. */
	EUnderlineOff,
	/** Font is underlined. */
	EUnderlineOn
	};

/**
Font strike-through flags. 
@publishedAll	
@released
*/
enum TFontStrikethrough
	{
	/** Font is not struck-through. */
	EStrikethroughOff,
	/** Font is struck-through. */
	EStrikethroughOn
	};

/**
The maximum length of a typeface name (in characters). 
@publishedAll	
@released
*/
const TInt KMaxTypefaceNameLength=0x18;


/** Typeface name and attributes.

This class identifies a typeface by name, and contains the combination of 
attributes of the typeface. These attributes define whether it is a symbol 
typeface, whether the typeface is proportional, and whether it is serif or 
sans-serif. 

The combination of attributes for a typeface are stored in a bitmask, with 
the various bits indicating different attributes. The bitmask is calculated 
for any particular attribute combination by ORing the enumerated value for 
each individual attribute. 
@publishedAll 
@released
*/
class TTypeface
    {
public:
	enum
		{
		/** Typeface is a proportional typeface (e.g. Swiss)
		*/
		EProportional = 1,
		/** Typeface is a serif typeface (e.g. Times)
		*/
		ESerif = 2,
		/** Typeface is a symbol typeface (e.g. Symbol)
		*/
		ESymbol = 4,
		};
public:
	IMPORT_C TTypeface();
	IMPORT_C TBool operator==(const TTypeface& aTypeface) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void SetAttributes(TInt aAttributes);
	IMPORT_C void SetIsProportional(TBool aIsProportional);
	IMPORT_C void SetIsSerif(TBool aIsSerif);
	IMPORT_C void SetIsSymbol(TBool aIsSymbol);
	IMPORT_C TInt Attributes() const;
	IMPORT_C TBool IsProportional() const;
	IMPORT_C TBool IsSerif() const;
	IMPORT_C TBool IsSymbol() const;
	IMPORT_C void SetScriptTypeForMetrics(TLanguage aLanguage);
	IMPORT_C void SetScriptTypeForMetrics(TInt aScript);
	IMPORT_C TInt ScriptTypeForMetrics() const;
	IMPORT_C void SetName(const TDesC& aName);
	IMPORT_C const TDesC& Name() const;
private:
	void ResetAttributes();
	void ResetScriptType();
public:
	/** The typeface name. */
    TBufC<KMaxTypefaceNameLength> iName;
private:
    TUint32 iFlags;
    };


/**
An enumerated type for the format of a glyph bitmap. This type is currently 
used to indicate whether glyph bitmaps for scalable fonts are drawn anti-aliased. 
Additional values may be defined in the future.

@see TFontStyle::SetBitmapType()
@see CFbsTypefaceStore::SetDefaultBitmapType() 
@publishedAll
@released	
*/
enum TGlyphBitmapType
	{
	/** The font store's default glyph bitmap format is used. */
	EDefaultGlyphBitmap = 0,
	/** The standard monochrome format: no anti-aliasing, 1 bit per pixel, 
	run-length encoded. */
	EMonochromeGlyphBitmap,
	/** Standard 8-bits-per-pixel with anti-aliasing. */
	EAntiAliasedGlyphBitmap,
	/** The format used when sub-pixel font rendering is used. */
	ESubPixelGlyphBitmap,
	/** The format used when outline and shadow font rendering is used. 

	If the raterizer supports the outline and shadow fonts, it will set the bitmaptype as 
	EFourColourBlendGlyphBitmap but only when glyph bitmap type is set as EAntiAliasedGlyphBitmap and 
	when any of the EDropShadow or EOutline effect is on. Only rasterizer providers can use this enum.
	*/
	EFourColourBlendGlyphBitmap,
	/**
	This is used for glyphs, and not fonts, and is needed to inform the font drawing routines
	that the character should be drawn using the overall font setting. 
	For Internal Use Only.
	*/
	EGlyphBitmapTypeNotDefined,
	/**
	This is used to inform the rasterizer that the best match should be
	found for the bitmap type based upon its knowledge.
	For Internal Use Only.
	*/
	EAntiAliasedOrMonochromeGlyphBitmap,
	};

/**
Defines a set of font effects flags.

@publishedAll 
@released
WARNING: This Class is for use by system/UI software ONLY.
*/
NONSHARABLE_CLASS(FontEffect)
	{
public:
	enum TEffect
		{
		ENone			= 0x0,		// No effects.
		EAlgorithmicBold= 0x10,		// Font is algorithmic bold (a.k.a pseudo bold.)
		EDropShadow		= 0x20,		// Font has a drop shadow.
		EOutline		= 0x40,		// Font is an outline font.
		EEmbossed		= 0x80,		// Font is embossed.
		EEngraved		= 0x100,	// Font is engraved.
		ESoftEdge		= 0x200,	// Font is soft edged.
		EReserved1		= 0x400,	// Reserved for Symbian use.
		EReserved2		= 0x800,	// Reserved for Symbian use.
		EReserved3		= 0x1000,	// Reserved for Symbian use.
		EReserved4		= 0x2000,	// Reserved for Symbian use.
		EReserved5		= 0x4000,	// Reserved for Symbian use.
		EReserved6		= 0x8000,	// Reserved for Symbian use.
		};
public:
	IMPORT_C static TBool IsEffectOn(TEffect aEffect, TUint32 aFontEffect);
	IMPORT_C static void SetEffect(TEffect aEffect, TBool aOn, TUint32& aFontEffect);
	};

/** Encapsulates a font style. 

The font style information is comprised of:

the posture of the font upright or italic

the stroke weight of the font  normal or bold

the print position of the font normal, subscript or superscript

Note that the underline and strike-through attributes are not included in 
this class, but are set in the graphics context.

@see CGraphicsContext::SetUnderlineStyle()
@see CGraphicsContext::SetStrikethroughStyle() 
@publishedAll
@released
*/
class TFontStyle
	{
public:
	IMPORT_C TFontStyle();
	IMPORT_C TFontStyle(TFontPosture aPost,TFontStrokeWeight aStrWgt,TFontPrintPosition aPrintPos);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TFontPosture Posture() const;
	IMPORT_C TFontStrokeWeight StrokeWeight() const;
	IMPORT_C TFontPrintPosition PrintPosition() const;
	IMPORT_C void SetPosture(TFontPosture aPosture);
	IMPORT_C void SetStrokeWeight(TFontStrokeWeight aStrokeWeight);
	IMPORT_C void SetPrintPosition(TFontPrintPosition aPrintPosition);
	inline TGlyphBitmapType BitmapType() const;
	inline void SetBitmapType(TGlyphBitmapType aBitmapType);
	IMPORT_C TBool operator==(const TFontStyle& aFontStyle) const;
	IMPORT_C TUint32 Effects() const;
	IMPORT_C TBool IsEffectOn(FontEffect::TEffect aEffect) const;
	IMPORT_C void SetEffects(TUint32 aEffects);
	IMPORT_C void SetEffects(FontEffect::TEffect aEffect, TBool aOn);
private:
	enum
		{
		EItalic=0x1,
		EBold=0x2,
		ESuper=0x4,
		ESub=0x8
		};
private:
	TUint32 iFlags; // bitmap type - 16 bits (high), font effects - 12 bits (middle), style - 4 bits (low)
	TAny* iReserved1;
	TAny* iReserved2;
	};



/**
Specifies the font specification in device independent terms.
@publishedAll 
@released
*/
class TFontSpec
	{
public:
	IMPORT_C TFontSpec();
	IMPORT_C TFontSpec(const TDesC& aTypefaceName,TInt aHeight);
	IMPORT_C TBool operator==(const TFontSpec& aFontSpec) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void SetScriptTypeForMetrics(TLanguage aLanguage);
	IMPORT_C TInt ScriptTypeForMetrics() const;
public:
	/** The typeface. */
	TTypeface iTypeface;
	/** The height of the typeface (in twips). */
	TInt iHeight;
	/** The font style of the typeface. */
	TFontStyle iFontStyle;
	};



/** Typeface family support information.

This data-only class includes the name and attributes of a typeface, how many 
font heights are available, its minimum and maximum heights, and whether or 
not it is scaleable  a typeface is scaleable if it supports heights at 
fixed intervals between the minimum and maximum heights. 
@publishedAll 
@released
*/
class TTypefaceSupport
    {
public:
	/** The name and attributes of the typeface. */
    TTypeface iTypeface;
	/** The number of distinct font heights available in the typeface. */
    TInt iNumHeights;
	/** The typeface's minimum font height, in twips. */
    TInt iMinHeightInTwips;
	/** The typeface's maximum font height, in twips. */
    TInt iMaxHeightInTwips;
	/** Whether the typeface is scaleable. ETrue if it is scaleable, otherwise 
	EFalse. */
    TBool iIsScalable; // supports heights from min to max at fixed interval
    };

/**
The percentage used to multiply a normal font height when calculating its 
superscript or subscript height. 
@publishedAll	
@released
*/
const TInt KSuperSubScalingPercentage=67;

/**
The percentage of a font height used to calculate its baseline offset for a 
superscript print position. 
@publishedAll	
@released
*/
const TInt KSuperscriptOffsetPercentage=-28;

/**
The percentage of a font height used to calculate its baseline offset for a 
subscript print position. 
@publishedAll	
@released
*/
const TInt KSubscriptOffsetPercentage=14;

class CFont;

/** Typeface store abstract base interface.

This class provides the interface to a store for typefaces.

See also CFontStore. 
@publishedAll
@released
*/
class CTypefaceStore : public CBase
	{
public:
	IMPORT_C ~CTypefaceStore();

	/**
	Gets the font which is the nearest to the given font specification.

	When the font is no longer needed, call @c ReleaseFont().

	Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInTwips() 
	yielding (virtually) the same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of twips, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@deprecated Use GetNearestFontToDesignHeightInTwips
	*/
	virtual TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec) = 0;

	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().

	This new function replaces the deprecated @c GetNearestFontInTwips() yielding (virtually) the 
	same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of twips, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec) = 0;
	
	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().

	The font and bitmap server returns a pointer to the nearest matching font 
	from those available. Matches to max height of font - this does its best 
	to return a font that will fit within the maximum height specified (but 
	note that variations due to hinting algorithms may rarely result in this 
	height being exceeded by up to one pixel). Problems can also be 
	encountered with bitmap fonts where the typeface exists but doesn't have 
	a font small enough.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@param aMaxHeight The maximum height within which the font must fit.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight) = 0;

	/** Gets the number of typefaces supported by the typeface store.
	
	@return The number of supported typefaces. */
	virtual TInt NumTypefaces() const=0;

	/** Gets typeface information for a specified typeface index.
 
 	This information is returned in aTypefaceSupport, and
	includes the typeface name and typeface attributes, the number of font
 	heights, the maximum and minimum font heights, and whether it is a
	scaleable typeface.
 
 	@param aTypefaceSupport On return, if the function executed successfully, 
 	this object contains the typeface information.
  	@param aTypefaceIndex A typeface index number, in the range: zero to
             (NumTypefaces() - 1). */
	virtual void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const=0;

	/** Gets the height of the font with specified height and typeface indices, 
	in twips.
	
	The value returned is rounded up or down to the nearest font height in twips.
	
	@param aTypefaceIndex A typeface index number, in the range: 0 to 
	(NumTypefaces() - 1). 
	@param aHeightIndex A font height index number.
	@return The height of the font, in twips. */
	virtual TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const=0;
	IMPORT_C void ReleaseFont(CFont* aFont);
	IMPORT_C static TInt BaselineOffset(TInt aHeight,TFontPrintPosition aPos);
	IMPORT_C static TInt SuperSubHeight(TInt aHeight,TFontPrintPosition aPos);
protected:
	IMPORT_C CTypefaceStore();
	IMPORT_C void ConstructL();
	IMPORT_C void AddFontL(CFont* aFont);
	IMPORT_C TBool IncrementFontCount(const CFont* aFont);
private:
	TBool FindFont(const CFont* aFont, TInt& aIdx) const;
	NONSHARABLE_CLASS(TFontAccess)
	/**
	Pairs a font with a count of how many clients of the typeface store 
	are accessing that font. 
    */
		{
	public:
		/** A device specific font. */
		CFont* iFont;
		/** The number of clients accessing the font. */
		TInt iAccessCount;
		};
protected:
	/** A list of fonts accessed by clients of the typeface store, which pairs 
	a font with a count of the number of clients accessing the font.
	
	Implemented as an array of TFontAccess objects.
	
	An object is added to this array for every font accessed. If the font is 
	released by all clients, and the access count drops to zero, the font is 
	removed from the list. */
	CArrayFixFlat<TFontAccess>* iFontAccess;
	};

/** The maximum number of entries in the font cache.

@see CFontCache */
const TInt KMaxFontCacheEntries=32;

/** Font cache. 

When a CFont* needs to be found for a particular TFontSpec, the cache can 
be searched to see if the TFontSpec is already in the cache. If the TFontSpec 
is in the cache, its corresponding CFont* can be returned. Otherwise 
GetNearestFontInTwips() must be used to search all of the available fonts for 
the nearest CFont- a procedure which takes much longer than a simple cache
search.

The current font cache should be destroyed and a new cache created whenever 
the zoom factor or device map changes, as these changes break the relation 
between CFont and TFontSpec. 
@publishedAll
@released
*/
class CFontCache : public CBase
	{
public:
	IMPORT_C CFontCache();
	IMPORT_C CFontCache(TInt aMaxEntries);
	IMPORT_C ~CFontCache();
	IMPORT_C CFont* Search(const TFontSpec& aFontSpec);
	IMPORT_C CFont* AddEntryL(CFont* aFont,const TFontSpec& aFontSpec);
	IMPORT_C CFont* RemoveFirstEntry();
public:
	/** The number of cache hits since the font cache was created i.e. 
	successful results from CFontCache::Search(). */
	TInt iNumHits;
	/** The number of cache misses since the font cache was created i.e. 
	unsuccessful results from CFontCache::Search(). */
	TInt iNumMisses;
private:
	class CFontCacheEntry : public CBase
		{
	public:
		CFontCacheEntry(CFont* aFont,const TFontSpec& aFontSpec,CFontCacheEntry* aNext);
	public:
		CFont* iFont;
		TFontSpec iSpec;
		CFontCacheEntry* iNext;
		};
private:
	TInt iNumEntries;
	TInt iMaxEntries;
	CFontCacheEntry* iFirst;
	};

/** Interface class for mapping between twips and device-specific units (pixels).

TZoomFactor is derived from MGraphicsDeviceMap.

@see CGraphicsDevice
@see TZoomFactor 
@publishedAll
@released
*/
class MGraphicsDeviceMap
    {
public:
	IMPORT_C MGraphicsDeviceMap();
	IMPORT_C virtual ~MGraphicsDeviceMap();
	IMPORT_C TPoint TwipsToPixels(const TPoint& aTwipPoint) const;
	IMPORT_C TRect TwipsToPixels(const TRect& aTwipRect) const;
	IMPORT_C TPoint PixelsToTwips(const TPoint& aPixelPoint) const;
	IMPORT_C TRect PixelsToTwips(const TRect& aPixelRect) const;

	/** Converts a horizontal dimension from twips to pixels.
	
	An implementation is supplied by a derived class.
	
	@param aTwips A horizontal dimension of a device in twips. 
	@return A horizontal dimension of a device in pixels. */
	virtual TInt HorizontalTwipsToPixels(TInt aTwips) const=0;

	/** Converts a vertical dimension from twips to pixels.
	
	An implementation is supplied by a derived class.
	
	@param aTwips A vertical dimension of a device in twips. 
	@return A vertical dimension of a device in pixels. */
	virtual TInt VerticalTwipsToPixels(TInt aTwips) const=0;

	/** Converts a horizontal dimension from pixels to twips.
	
	An implementation is supplied by a derived class.
	
	@param aPixels A horizontal dimension of a device in pixels. 
	@return A horizontal dimension of a device in twips. */
	virtual TInt HorizontalPixelsToTwips(TInt aPixels) const=0;

	/** Converts a vertical dimension from pixels to twips.
	
	An implementation is supplied by a derived class.
	
	@param aPixels A vertical dimension of a device in pixels. 
	@return A vertical dimension of a device in twips. */
	virtual TInt VerticalPixelsToTwips(TInt aPixels) const=0;

	/**
	Gets the font which is the nearest to the given font specification.

	When the font is no longer needed, call @c ReleaseFont().

	Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInTwips() 
	yielding (virtually) the same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of twips, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@deprecated Use GetNearestFontToDesignHeightInTwips
	*/
	virtual TInt GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec)=0;

	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().

	This new function replaces the deprecated @c GetNearestFontInTwips() yielding (virtually) the 
	same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of twips, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToDesignHeightInTwips(
		CFont*& /*aFont*/, const TFontSpec& /*aFontSpec*/) { return KErrNotSupported; }

	/**
	Gets the font which is the nearest to the given font specification.

	When the font is no longer needed, call @c ReleaseFont().

	The font and bitmap server returns a pointer to the nearest matching font 
	from those available. Matches to max height of font - this does its best 
	to return a font that will fit within the maximum height specified (but 
	note that variations due to hinting algorithms may rarely result in this 
	height being exceeded by up to one pixel). Problems can also be 
	encountered with bitmap fonts where the typeface exists but doesn't have 
	a font small enough.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@param aMaxHeight The maximum height within which the font must fit.
	This overrides the height specified in aFontSpec.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToMaxHeightInTwips(
		CFont*& /*aFont*/, const TFontSpec& /*aFontSpec*/, TInt /*aMaxHeight*/) { return KErrNotSupported; }

	/** Releases the specified font.
	
	It is used to indicate that the specified font is no longer needed for use 
	by the device map. As fonts can be shared between applications, this 
	function does not delete the copy of the font from RAM unless the font was 
	only being used by this particular device map.
	
	An implementation is supplied by a derived class.
	
	@param aFont A pointer to the font to be released. */
	virtual void ReleaseFont(CFont* aFont)=0;
    };

class CGraphicsContext;

/** Specifies the interface for concrete device classes.

It holds information on the capabilities and attributes of a graphics device. 
The CBitmapDevice and CPrinterDevice classes are derived from CGraphicsDevice.

@see CGraphicsDevice
@see CPrinterDevice 
@publishedAll
@released
*/
class CGraphicsDevice : public CBase , public MGraphicsDeviceMap
    {
public:
	/** Gets the display mode of the device.
	
	@return The display mode of the device. */
	virtual TDisplayMode DisplayMode() const=0;

	/** Gets the size of the device area in pixels.
	
	@return The width and height of the device area, in pixels */
	virtual TSize SizeInPixels() const=0;

	/** Gets the size of the device area in twips.
	
	@return The width and height of the device area, in twips */
	virtual TSize SizeInTwips() const=0;

 	/** Creates a graphics context for the device.
	
	@param aGC On return, contains a pointer to the created graphics context. 
	@return KErrNone, if successful; otherwise, another of the system-wide error 
	codes. */
    virtual TInt CreateContext(CGraphicsContext*& aGC)=0;

	/** Gets the number of typefaces supported by the graphics device.
	
	@return The number of typefaces supported. */
    virtual TInt NumTypefaces() const=0;

 	/** Gets typeface information for a specified typeface.
	
	This information is returned in aTypefaceSupport, and includes:
	
	the typeface name and typeface attributes
	
	the number of font heights
	
	the maximum and minimum font heights
	
	whether it is a scalable typeface
	
	@param aTypefaceSupport On return, contains the typeface information. 
	@param aTypefaceIndex A typeface index number, in the range: zero to 
	(NumTypefaces() - 1). 
	@see NumTypefaces() */
    virtual void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const=0;

	/** Get the height of a font in twips.
	
	The font is identified by typeface and height.
	
	The value returned is rounded up or down to the nearest font height in twips.
	
	@param aTypefaceIndex An index identifying the typeface, in the range: 0 
	to (NumTypefaces() - 1). 
	@param aHeightIndex An index identifying the font height, in the range: 0 
	to (iNumHeights - 1). Note that iNumHeights is in the TTypefaceSupport 
	object returned by TypefaceSupport(). 
	@return The height of the font, in twips. */
	virtual TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const=0;

	/** Gets the palette attributes of the device.
	
	@param aModifiable On return, holds information on whether or not the device's 
	palette is modifiable (ETrue) or fixed (EFalse). 
	@param aNumEntries On return, holds the number of entries in the device's 
	palette. */
	virtual void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const=0;

	/** Sets the device's palette.
	
	Setting the palette is only possible if the device has a modifiable palette, 
	which can be determined by calling PaletteAttributes().
	
	@param aPalette The new palette for the device. */
	virtual void SetPalette(CPalette* aPalette)=0;

	/** Gets the device's current palette.
	
	This function is only supported if the device has a modifiable palette, 
	which can be determined by calling PaletteAttributes().
	
	@param aPalette On return, holds the device's current palette.
	@return KErrNone, if successful; otherwise, another of the system-wide error 
	codes. */
	virtual TInt GetPalette(CPalette*& aPalette) const=0;
    };


/** 
Code section range information. 

A code section defines the bitmaps for characters in a specified range -
the range is stored in objects of this type. 
@publishedAll
@deprecated 	This is not used anywhere in version 6.0.
*/
class TCodeSection
	{
public:
	/** The beginning of the range. */
	TInt iStart;
	/** The end of the range. */
	TInt iEnd;
	};

/**
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
UIDs corresponding to the CFont API extension functions
@internalTechnology
*/
const TUid KFontCapitalAscent	= {0x1020498E};
const TUid KFontMaxAscent		= {0x10204B10};
const TUid KFontStandardDescent	= {0x10204B11};
const TUid KFontMaxDescent		= {0x10205AFC};
const TUid KFontLineGap			= {0x10204B12};
const TUid KFontGetFontTable    = {0x102872C1};
const TUid KFontGetGlyphOutline = {0x102872C2};
const TUid KFontReleaseGlyphOutline = {0x2002A1FD};
const TUid KFontReleaseFontTable = {0x2002AC24};


/** Abstract font interface.

The CFont class provides a device-independent interface to a device-dependent 
font usually obtained from a call to GetNearestFont...() on a graphics device. 
It is used as a handle in CGraphicsContext::UseFont() and to obtain 
device-dependent information about the font - notably the pixel width of a text 
string.

@see CFbsFont
@see CGraphicsContext::UseFont() 
@publishedAll 
@released
*/
class CFont : public CBase
	{
	friend class CTypefaceStore;
public:

	/** Text direction flags.

	This enum is used in TMeasureTextInput and determines whether the text is 
	drawn horizontally or vertically. Note: text is drawn vertically in some 
	styles of Japanese, etc.

	@see TMeasureTextInput */
	enum TTextDirection
		{
	/** Text is drawn horizontally. */
 		/** Text is drawn horizontally. */
   		EHorizontal,
 		/** Text is drawn vertically. */
		EVertical
		};

	/** Complicated parameter block used for contextual glyph selection, 
	ligature creation and diacritic placement when drawing text in complex 
	scripts

	This class declares a constructor, another scoped class, and several other 
	enums. However this class is unlikely to be useful to third party developers.

	@see CFont::GetCharacterPosition()
	@see CFont::GetCharacterPosition2()
	@publishedAll
	@released
	*/
	class TPositionParam
		{
	public:
		/** Standard constructor. */
		TPositionParam():
			iDirection(EHorizontal),
			iFlags(0),
			iPosInText(0),
			iOutputGlyphs(0)
			{
			}

		enum
			{
			EMaxInputChars = 18,	// ligatures cannot be made from more than 18 components
			EMaxOutputGlyphs = 8	// output can consist of up to 8 characters (one base and 7 combining characters)
			};

		/**Flags for TPositionParam::iFlags. */
		enum TFlags
			{
			/** Input text is logically ordered not visually ordered. */
			EFLogicalOrder = 1
			};

		/** Input: Orientation (EHorizontal or EVertical) in which to draw
		the text. */
		TInt16 iDirection;
		/** Input: Flags from TFlags. */
		TUint16 iFlags;
		/** Input: Text containing the characters to be positioned. */
		TPtrC iText;

		/** Input and output: Position within iText to shape. On exit
		it will index the first character not positioned */
		TInt iPosInText;
		/** Input and output: Pen position. */
		TPoint iPen;			

		/** Output of GetCharacterPosition and GetCharacterPosition2.
		@see CFont::GetCharacterPosition
		@see CFont::GetCharacterPosition2
		@publishedAll
		@released */
		class TOutput
			{
		public:
			/** Standard constructor. */
			TOutput() : iBitmapSize(TSize::EUninitialized),
				iBounds(TRect::EUninitialized) {}
			/** Character or glyph code. */
			TUint iCode;			
			/** Bitmap data for the glyph, if available */
			const TUint8* iBitmap;	
			/** Size of the bitmap before algorithmic bolding, size 
			multiplication, etc. */
			TSize iBitmapSize;		
			/** Bitmap bounds relative to the original pen position. */
			TRect iBounds;			
			};

		/** Information about the glyphs that were output. */
		TOutput iOutput[EMaxOutputGlyphs];	
		/** Number of glyphs actually output. */
		TInt iOutputGlyphs;					
		};	


	/** Input parameter block.

	This is optionally used by CFont::MeasureText(), which is the powerful text 
	measurement function underlying all the other text measurement functions.

	@see CFont::MeasureText() 
	@publishedAll
	@released
	*/
	class TMeasureTextInput
		{
	public:
		TMeasureTextInput():
			iStartInputChar(0),
			iEndInputChar(KMaxTInt),
			iDirection(EHorizontal),
			iFlags(0),
			iMaxAdvance(KMaxTInt),
			iMaxBounds(KMaxTInt),
			iCharJustNum(0),
			iCharJustExcess(0),
			iWordJustNum(0),
			iWordJustExcess(0)
			{
			}

		/**Flags for TMeasureTextInput::iFlags. */
		enum TFlags
			{
 			/** Input text is visually ordered left-to-right. */
 			EFVisualOrder = 1,
 			/** Input text is visually ordered right-to-left.
 			Overrides EFVisualOrder. */
 			EFVisualOrderRightToLeft = 2,
 			/** Flag to consider side bearings when checking bounds for line-break */
 			EFIncludePenPositionInBoundsCheck = 4
			};

		/** Starting index specifying first input character in iText.
	
		Together with iEndInputChar, this allows some context before and 
		after the measured text to be supplied so that shaping can work 
		properly. */
		TInt iStartInputChar;	
	
		/** Index specifying the final input character.
	
		Together with iStartInputChar, this allows some context before and 
		after the measured text to be supplied so that shaping can work 
		properly. */
		TInt iEndInputChar;		
	
		/** The direction in which to draw the text. */
		TUint16 iDirection;
	
		/** Flags from TFlags. */
		TUint16 iFlags;
	
		/** The maximum advance. */
		TInt iMaxAdvance;		
		
		/** The maximum width (or height if drawing vertically) of bounds. */
		TInt iMaxBounds;		
	
		/** The number of glyph groups to be letter-spaced. */
		TInt iCharJustNum;		
	
		/** The amount of space to be used for letter spacing. */
		TInt iCharJustExcess;	
		
		/** The number of spaces to be used for word spacing. */
		TInt iWordJustNum;		
	
		/** The amount of space to be used for word spacing. */
		TInt iWordJustExcess;	
		};

	/** Output parameter block.

	This is optionally used by CFont::MeasureText(), which is the powerful text 
	measurement function underlying all the other text measurement functions.

	@see CFont::MeasureText() 
	@publishedAll
	@released
	*/
	class TMeasureTextOutput
		{
		public:
		/** The number of input characters that would be drawn.
	
		This may be less than the length of the text if a maximum advance or bounding 
		box size is specified. */
		TInt iChars;			
		/** The number of glyphs that would be drawn. */
		TInt iGlyphs;			
		/** The number of groups that would be drawn.
	
		A group is a base glyph plus one or more combining characters. */
		TInt iGroups;			
		/** The number of word spaces (U+0020) that would be drawn. */
		TInt iSpaces;			
		/** The bounding box of all the glyphs that would be drawn. */
		TRect iBounds;			
		/** The maximum width and height of any glyph. */
		TSize iMaxGlyphSize;	
		};

	/**
	Data availability flags.

	Some fonts like printer fonts may only have width information and can return 
	ECharacterWidthOnly to show this: the text drawing routines in CFont synthesize 
	the rest of the data if necessary.

	@see GetCharacterData() 
	@publishedAll	
	@released
	*/
	enum TCharacterDataAvailability
		{
		/** No font information available. */
		ENoCharacterData,
		/** Width information only is available. */
		ECharacterWidthOnly,
		/** All character data is available. */
		EAllCharacterData
		};

private:
	// virtual functions have been made protected and public non-virtual ones
	// added to convert CFont to a handle-body pattern. SC is kept throught the
	// new functions and BC is kept by keeping the protected functions in the
	// same place in the class, and therefore in the same place in the vtable
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TUid DoTypeUid() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoHeightInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoAscentInPixels() const=0;
	IMPORT_C virtual TInt DoDescentInPixels() const;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoCharWidthInPixels(TChar aChar) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoTextWidthInPixels(const TDesC& aText) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoBaselineOffsetInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoMaxCharWidthInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TInt DoMaxNormalCharWidthInPixels() const=0;
	/**
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. Please see derived class for implementation.
	*/
	virtual TFontSpec DoFontSpecInTwips() const=0;

protected:	
	IMPORT_C virtual TCharacterDataAvailability DoGetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics,const TUint8*& aBitmap,TSize& aBitmapSize) const;
	IMPORT_C virtual TBool DoGetCharacterPosition(TPositionParam& aParam) const;
	IMPORT_C virtual TInt DoExtendedFunction(TUid aFunctionId, TAny* aParam = NULL) const;

protected:
	IMPORT_C virtual ~CFont();

public:
	inline TInt FontCapitalAscent() const;
	inline TInt FontMaxAscent() const;
	inline TInt FontStandardDescent() const;
	inline TInt FontMaxDescent() const;
	inline TInt FontLineGap() const;
	inline TInt FontMaxHeight() const;

public:
	/** Gets run-time identity of the actual font type. This enables safe casting to 
	a derived type.
	
	For example, if the derived type is a CFbsFont, the return value is KCFbsFontUid. 
	You would need to cast to a CFbsFont to get a character bounding box. Similarly, 
	a CBitmapFont returns KCBitmapFontUidVal.
	
	@return The font-type identifier. */
	IMPORT_C TUid TypeUid() const;

	/** Gets the font height in pixels.
	Note that this deprecated function is replaced by the new @c FontMaxHeight().

	@return The font height in pixels.
	@see FontMaxHeight()
	@deprecated */
	IMPORT_C TInt HeightInPixels() const;

	/** Gets the font ascent in pixels.
	Note that this deprecated function is replaced by the new @c FontMaxAscent()
	or in some cases @c FontCapitalAscent().	
	
	@return The font ascent in pixels.
	@see FontCapitalAscent()
	@see FontMaxAscent()
	@deprecated */
	IMPORT_C TInt AscentInPixels() const;
	
	/** Gets the font descent in pixels.
	Note that this deprecated function is replaced by the new @c FontMaxDescent()
	or in some cases @c FontStandardDescent().	
	
	@return The font descent in pixels.
	@see FontStandardDescent() 
	@see FontMaxDescent()
	@deprecated */
	IMPORT_C TInt DescentInPixels() const;

	/** Gets the width in pixels in this font of the specified character.
	
	Note: For OpenType fonts this function returns the horizontal advance of
	the character, which may be different from the actual width.
	
	@param aChar The character whose width should be determined. 
	@return The width in pixels of the specified character in this font. */
	IMPORT_C TInt CharWidthInPixels(TChar aChar) const;

	/** Gets the width in pixels of the specified descriptor when displayed in this 
	font.
	
	@param aText The descriptor whose width should be determined. 
	@return The width of the specified descriptor when displayed in this font, 
	in pixels. */
	IMPORT_C TInt TextWidthInPixels(const TDesC& aText) const;

	/** Gets the baseline offset in pixels. 
	
	The baseline offset is how far a font is raised or lowered from its normal 
	baseline.
	
	@return Offset from normal baseline, in pixels. */
	IMPORT_C TInt BaselineOffsetInPixels() const;

	/** Gets how much of the specified descriptor can be displayed in this font without 
	exceeding the specified width.
	
	Note:
	
	This function does not display any of the descriptor itself - it is used 
	before display, to test whether the whole descriptor can be displayed.
	
	@param aText The descriptor. 
	@param aWidthInPixels The available width for character display. 
	@return The number of characters which will be able to be displayed without 
	exceeding the specified width. The count starts from the beginning of the 
	descriptor. */
	IMPORT_C TInt TextCount(const TDesC& aText,TInt aWidthInPixels) const;

	/** Gets how much of the specified descriptor can be displayed in this font without 
	exceeding the specified width. 
	
	It also returns the excess width - defined as the specified available width 
	minus the width of the portion of the descriptor which can be displayed without 
	exceeding the available width.
	
	@param aText The descriptor. 
	@param aWidthInPixels The available width for character display. 
	@param aExcessWidthInPixels The excess width after displaying the portion of 
	the descriptor, in pixels. 
	@return The number of characters which will be able to be displayed without 
	exceeding the specified width. The count starts from the beginning of the 
	descriptor. */
	IMPORT_C TInt TextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const;

	/** Gets the width in pixels of the widest character in this font.
	
	@return The width of the maximum width character, in pixels. */
	IMPORT_C TInt MaxCharWidthInPixels() const;

	/** Gets the width in pixels of the widest normal character in this font.
	
	Normal characters include all character in a character set except non-alphabetic 
	characters (e.g. the copyright symbol, or a block graphics symbol, for example).
	
	@return The width of the maximum width normal character, in pixels. */
	IMPORT_C TInt MaxNormalCharWidthInPixels() const;

	/** Gets the font specification of this font in twips.
	
	@return The font specification of this font (in twips). */
	IMPORT_C TFontSpec FontSpecInTwips() const;
	IMPORT_C TCharacterDataAvailability GetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics,const TUint8*& aBitmap,TSize& aBitmapSize) const;
	IMPORT_C TBool GetCharacterPosition(TPositionParam& aParam) const;
	IMPORT_C TInt WidthZeroInPixels() const;
	IMPORT_C TInt MeasureText(const TDesC& aText, const TMeasureTextInput* aInput = NULL, TMeasureTextOutput* aOutput = NULL) const;
	IMPORT_C static TBool CharactersJoin(TInt aLeftCharacter, TInt aRightCharacter);
	IMPORT_C TInt ExtendedFunction(TUid aFunctionId, TAny* aParam = NULL) const;
	IMPORT_C TBool GetCharacterPosition2(TPositionParam& aParam, RShapeInfo& aShapeInfo) const;

	/** Gets the width in pixels of the specified descriptor when displayed in this 
	font.
	
	@param aText The descriptor whose width should be determined.
	@param aParam Parameter block that controls how much of aText is measured
	@return The width of the specified descriptor when displayed in this font, 
	in pixels. */
	IMPORT_C TInt TextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam) const;
	};

class CFbsBitmap;
class CWsBitmap;
/** Abstract base class for all graphics contexts.

Created by a CGraphicsDevice.

Provides the 'context' in which you are drawing to an associated device, in 
the sense that it holds the settings for drawing, such as the pen and brush 
settings (e.g. color, line styles) and the font settings (e.g. bold, underline, 
italic). These settings are device-independent.

Also provides the clipping region (the visible drawing area).

The settings and clipping area can be updated while drawing.

This class also contains the main drawing functions, and all drawing is done 
through a CGraphicsContext.

The graphics context deals with pixels of device-dependent size and uses fonts 
with device-dependent size and representation. The sizes and fonts to be passed 
to the class functions therefore need to be converted from size-independent 
units to size-dependent units first. This is done by an MGraphicsDeviceMap 
derived class. This may be a TZoomFactor or the CGraphicsDevice.

See CGraphicsContext::Reset() for the default graphics context settings immediately 
after construction.

@see CBitmapContext 
@publishedAll
@released
*/
class CGraphicsContext : public CBase
	{
public:


	/** Text alignment.
	@publishedAll
	@released
	*/
	enum TTextAlign
		{
		/** Text is left-aligned. */
		ELeft,
		/** Text is centred. */
		ECenter,
		/** Text is right-aligned. */
		ERight
		};

	/** 
	Drawing mode components.
	This enum is not intended to be used directly, but provides components for 
	the easy specification of drawing modes in the TDrawMode enum. 
	@publishedAll
	@released
	*/
	enum TDrawModeComponents
		{
		/** 1 */
		EInvertScreen=1,
		/** 2 */
		EXor=2,
		/** 4 */
		EOr=4,
		/** 8 */
		EAnd=8,
		/** 14 */
		ELogicalOp=14,
		/** 16 */
		EInvertPen=16,
		/** 32 */
		EPenmode=32,
		/** 64 */
		EWriteAlpha=64,
		};


	/** 
	Drawing modes.

	This enum builds on the drawing mode components in the TDrawModeComponents 
	enum.

	If the pen colour is p, brush colour is b and screen colour is s, the effect 
	of TDrawMode::EDrawModeAND is P=p&s and B=b&s. In other words, the effective 
	colour of the pen on the screen, P, is that produced by the bitwise ANDing 
	of the current screen colour and the current pen colour. The effect is similar 
	for the effective brush colour, B.

	The effective pen and brush colour are given in the table using the key
	Inputs: pen colour is p, brush colour is b and screen colour is s
	Outputs: effective brush colour is B, effective pen colour is P. 

	Some notes on using EDrawModeWriteAlpha:-

	- It is rare for client code to need to use this draw mode: see the documentation
	of SetDrawMode() for more information.
	- EDrawModeWriteAlpha should only be used with DrawRect(), Clear(), BitBlt(), and BitBltMasked() 
	with EGray2 mask (and DrawBitmap() and DrawBitmapMasked()). For other draw operations, it is not 
	supported, and may have unintended effects.
	- EDrawModeWriteAlpha has the same effect as EDrawModePEN, unless the brush colour has transparency 
	(DrawRect(), Clear()), or the source bitmap is EColor16MA (and has transparency) (BitBlt(), BitBltMasked())
	- EDrawModeWriteAlpha has the same effect as EDrawModePEN if the draw mode of the destination does not 
	support alpha blending. (Blending is only supported in 24bpp and 32bpp colour i.e. EColor16M, EColor16MU, EColor16MA)
	- In these cases, EDrawModePEN does alpha blending, whereas EDrawModeWriteAlpha means don't do alpha blending.

	@see SetDrawMode()
	@publishedAll
	@released
	*/
	enum TDrawMode
		{
		/** Bitwise ANDs the pen and brush colours with the screen colour. 
		P=p&s, B=b&s */
		EDrawModeAND=EAnd,
		/** Inverts the pen and brush colours before ANDing. P=(~p)&s, 
		B=(~b)&s */
		EDrawModeNOTAND=EInvertScreen|EAnd,
		/** Uses both pen and brush colour as they are. P=p, B=b */
		EDrawModePEN=EPenmode,
		/** Inverts the screen colour before ANDing. P=p&(~s), B=b&(~s) */
		EDrawModeANDNOT=EAnd|EInvertPen,
		/** Bitwise XORs the pen and brush colours with the screen colour. 
		P=p^s, B=b^s */
		EDrawModeXOR=EXor,
		/** Bitwise ORs the pen and brush colours with the screen colour. 
		P=p|s, B=b|s */
		EDrawModeOR=EOr,
		/** Inverts the screen and pen and brush colours before ANDing. 
		P=(~p)&(~s), B=(~b)&(~s) */
		EDrawModeNOTANDNOT=EInvertScreen|EAnd|EInvertPen,
		/** Inverts the pen and brush colours before XORing. P=(~p)^s, 
		B=(~b)^s */
		EDrawModeNOTXOR=EInvertScreen|EXor,
		/** Inverts the colour of each pixel that is drawn over, (pen and 
		brush attributes are ignored). P=~s, B=~s */
		EDrawModeNOTSCREEN=EInvertScreen,
		/** Inverts the screen colour before ORing. P=p|(~s), 
		B=b|(~s) */
		EDrawModeNOTOR=EInvertScreen|EOr,
		/** Inverts the pen and brush colours. P=~p, B=~b */
		EDrawModeNOTPEN=EInvertPen|EPenmode,
		/** Inverts the pen and brush colours before ORing. P=(~p)|s, 
		B=(~b)|s */
		EDrawModeORNOT=EOr|EInvertPen,
		/** Inverts the screen and pen and brush colours before ORing.
		P=(~p)|(~s), B=(~b)|(~s) */
		EDrawModeNOTORNOT=EInvertScreen|EOr|EInvertPen,
		/** Writes alpha information in the source directly into the destination, rather than blending. */
		EDrawModeWriteAlpha=EWriteAlpha,
		};

	/** 
	Pen styles. The screen pattern unit in each definition below describes the 
	pattern drawn by the line 1 represents a pixel drawn, 0 represents a 
	pixel that is not affected. 
	@publishedAll
	@released
	*/
	enum TPenStyle
		{
		/** The pen does not draw. Screen pattern unit = 00... */
		ENullPen,
		/** A solid line (default). Screen pattern unit = 11... */
		ESolidPen,
		/** A dotted line. Screen pattern unit = 1000... */
		EDottedPen,
		/** A dashed line. Screen pattern unit = 111000... */
		EDashedPen,
		/** A line of alternating dashes and dots. Screen pattern unit = 
		1111001100... */
		EDotDashPen,
		/** A line of alternating single dashes and pairs of dots. Screen 
		pattern unit = 11110011001100... */
		EDotDotDashPen
		};

	/** 
	Brush styles. 
	@publishedAll
	@released
	*/
	enum TBrushStyle
		{
		/** The brush fill has no effect (default). */
		ENullBrush,
		/** The brush fills with a solid single colour, determined by 
		SetBrushColor() and the drawing mode. */
		ESolidBrush,
		/** The brush fills with a selected bitmap pattern, set by 
		UseBrushPattern(). */
		EPatternedBrush,
		/** The brush fills with vertical hatching  lines going from top to 
		bottom. */
		EVerticalHatchBrush,
		/** The brush fills with diagonal hatching lines going from bottom 
		left to top right. */
		EForwardDiagonalHatchBrush,
		/** The brush fills with horizontal hatching  lines going from left 
		to right. */
		EHorizontalHatchBrush,
		/** The brush fills with rearward diagonal hatching lines going from top 
		left to bottom right. */
		ERearwardDiagonalHatchBrush,
		/** The brush fills with horizontal and vertical hatching  lines going 
		from left to right plus lines going from top to bottom  giving the 
		effect of a grid of small squares */
		ESquareCrossHatchBrush,
		/** The brush fills with forward diagonal and rearward diagonal hatching 
		lines going from bottom left to top right plus lines going from top left 
		to bottom right giving the effect of a grid of small diamonds. */
		EDiamondCrossHatchBrush
		};

	/** 
	Rules used to fill self crossing polygons. 

	The filling of a polygon proceeds as follows: for a given point in the 
	polygon, then

	if the rule is TFillRule::EAlternate (default) and it has an odd winding 
	number, then fill the surrounding area.

	if the rule is TFillRule::EWinding and it has a winding number greater than 
	zero, then fill the surrounding area. 
	@publishedAll
	@released
	*/
	enum TFillRule
		{
		/** Only fill areas with odd winding numbers. */
		EAlternate,
		/** Fill areas with winding numbers greater than zero. */
		EWinding
		};

	/** Parameters to control the drawing of text. */
	struct TDrawTextParam
		{
	public:
		TDrawTextParam():
			iDirection(CFont::EHorizontal),
			iCharJustNum(0),
			iCharJustExcess(0),
			iWordJustNum(0),
			iWordJustExcess(0)
	/** Reserved for future use. */
			{}
	public:
		/** the direction in which to draw the text. */
		CFont::TTextDirection iDirection;	
		/** number of glyph groups to be letterspaced */
		TInt iCharJustNum;					
		/** amount of space to be used for letterspacing */
		TInt iCharJustExcess;				
		/** number of spaces to be used for wordspacing*/
		TInt iWordJustNum;					
		/**  amount of space to be used for wordspacing*/
		TInt iWordJustExcess;				
		};


	/** Parameters for extended text drawing and measuring. It is used by 
	CGraphicsContext::DrawTextExtended() to indicate whether text should be 
	drawn from right-to-left or left-to-right. */
	struct TDrawTextExtendedParam : public TDrawTextParam
		{
	public:
		/** Constructor. Initialises iParRightToLeft to EFalse. */
		TDrawTextExtendedParam():
			iParRightToLeft(EFalse)
			{}
	public:
		/** ETrue if the text direction is right-to-left (for scripts like 
		Arabic and Hebrew). EFalse if left-to-right. */
		TBool iParRightToLeft;	
		};
	
	/**
	Parameters used in drawing text within supplied context.
	It is used by CGraphicsContext::DrawText() and CGraphicsContext::DrawTextVertical() family of API's
	to draw text from iStart to iEnd withing the supplied text descriptor.
	*/
	class TTextParameters
		{
	public:
		TTextParameters():
			iStart(0),
			iEnd(KMaxTInt),
			iFlags(0)
				{
				}
		TInt iStart;
		TInt iEnd;
		TUint16 iFlags;
		/* Reserved for future use */
		TAny* iReserved1;
		TAny* iReserved2;
		TAny* iReserved3;
		TAny* iReserved4;
		};
public:
	/** Gets a pointer to the graphics context's graphics device.
	
	@return A pointer to the graphics device. */
	virtual CGraphicsDevice* Device() const=0;

	/** Sets the position of the co-ordinate origin.
	
	All subsequent drawing operations are done relative to this origin.
	
	@param aPos The origin. The default origin is TPoint(0,0) the top left 
	corner of the screen. */
	virtual void SetOrigin(const TPoint& aPos=TPoint(0,0))=0;

	/** Sets the drawing mode. 
	
	The way that the pen and brush draw depends on the drawing mode. The drawing 
	mode affects the colour that is actually drawn, because it defines the way 
	that the current screen colour logically combines with the current pen colour 
	and brush colour. There are many drawing modes, each giving different logical 
	combinations of pen, brush and screen colours. Each mode is produced by ORing 
	together different combinations of seven drawing mode components.
	
	The three most important modes are TDrawMode::EDrawModePEN, TDrawMode::EDrawModeNOTSCREEN 
	and TDrawMode::EDrawModeXOR. The default drawing mode is TDrawMode::EDrawModePEN.
	
	The drawing mode is over-ridden for line and shape drawing functions when 
	a wide pen line has been selected. It is forced to TDrawMode::EDrawModePEN. 
	This is to prevent undesired effects at line joins (vertexes).
	
	Notes:
	
	TDrawMode::EDrawModeAND gives a "colour filter" effect. For example:
	
	- ANDing with white gives the original colour
	- ANDing with black gives black
	
	TDrawMode::EDrawModeOR gives a "colour boost" effect. For example:
	
	- ORing with black gives the original colour
	- ORing with white gives white
	
	TDrawMode::EDrawModeXOR gives an "Exclusive OR" effect. For example:
	
	- white XOR black gives white
	- white XOR white gives black
	- black XOR black gives black
	
	TDrawMode::EDrawModeWriteAlpha should not normally need to be used by client code. 
	The following are exceptions:-

	- When a client side EColor16MA bitmap needs to have a transparent background 
	(because you are intending to blend it onto something else), then you need to set 
	EDrawModeWriteAlpha to Clear() it.
	- When you want to BitBlt() with an EColor16MA source bitmap that is opaque everywhere, 
	then using EDrawModeWriteAlpha is more efficient than EDrawModePEN, because the bitmap 
	does not need to be blended. 

	Note that if you have a transparent brush or source bitmap and you are drawing to a window, 
	then it is a defect to use EDrawModeWriteAlpha.

	@param aDrawingMode The drawing mode.
	@see CGraphicsContext::TDrawMode
	@see CGraphicsContext::TDrawModeComponents */
	virtual void SetDrawMode(TDrawMode aDrawingMode)=0;

	/** Sets the clipping rectangle.	 
	
	The area of visible drawing depends on the clipping rectangle, any items 
	that fall outside the extent of the clipping rectangle will not be drawn. 
	The default clipping rectangle is the full device area.
	
	Note that clipping is additive. If a clipping region has been set using SetClippingRegion() 
	then clipping will be to the intersection of that region and this rectangle.
		
	@param aRect The rectangle to be used as the clipping rectangle. Note that 
	this rectangle is tranformed by the current co-ordinate origin before it is used. 
	The co-ordinate origin is set using SetOrigin().
	
	@see CGraphicsContext::SetClippingRegion() 
	@see CGraphicsContext::SetOrigin() */
	virtual void SetClippingRect(const TRect& aRect)=0;

	/** Cancels any clipping rectangle.
	
	Clipping thus reverts to the full device area, the default.
	
	@see SetClippingRect() */
	virtual void CancelClippingRect()=0;

	/** Resets the graphics context to its default settings:
	
	the drawing mode is TDrawMode::EDrawModePen (pen and brush colours used as 
	they are)
	
	there is no clipping rectangle
	
	the pen settings are: black, solid, single pixel size
	
	the brush style is null
	
	no text font is selected */
	virtual void Reset()=0;

   /** Sets the device font to be used for text drawing.
	
	If the font is already in memory, then that copy is shared.
	
	Notes:
	
	The CFont* argument must have been previously initialised by calling 
	MGraphicsDeviceMap::GetNearestFontInTwips() with the required 
	font-specification. If the CFont* has not been initialised 
	correctly, and therefore does not point to an available font-bitmap, 
	then a panic is raised.
	
	When the font is no longer required, use DiscardFont() to free up the 
	memory used. If UseFont() is used again without using DiscardFont() then 
	the previous font is discarded automatically.
	
	If no font has been selected, and an attempt is made to draw text with 
	DrawText(), then a panic is raised.
	
	@param aFont A device font 
	@see MGraphicsDeviceMap::GetNearestFontInTwips() */
	virtual void UseFont(const CFont* aFont)=0;

	/** Discards a font.
	
	This frees up the memory used, if the font is not being shared.
	
	The function can be called when no font is in use. */
	virtual void DiscardFont()=0;

	/** Sets the underline style.
	
	This is applied to all subsequently drawn text.
	
	@param aUnderlineStyle The underline style on or off. */
	virtual void SetUnderlineStyle(TFontUnderline aUnderlineStyle)=0;

	/** Sets the strikethrough style.
	
	This is applied to all subsequently drawn text.
	
	@param aStrikethroughStyle The strikethrough style on or off. */
	virtual void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)=0;
	IMPORT_C static TInt JustificationInPixels(TInt aExcessPixels,TInt aTotalUnits,TInt aFirstUnit,TInt aNumUnits);
	IMPORT_C static TInt JustificationInPixels(TInt& aExcessPixels,TInt& aTotalUnits);

	/** Adjusts the spaces between words to stretch or squeeze to a certain 
	width.
	
	The function is required by the Text Views API, and is not intended for 
	regular use by developers.
	
	The text line that is to be justified has a certain number of gaps (spaces) 
	between the words. It also has a distance (in pixels) between the end of 
	the last word and the actual end of the line (right hand margin, usually). 
	These excess width pixels are distributed amongst the gaps between the words 
	to achieve full justification of the text line. Spaces become fat spaces to 
	keep underlining/strikethrough consistent. Pixels are distributed to the 
	inter-word gaps starting from the left end of the string. The spacing 
	between characters in each word remains unchanged.
	
	After a call to SetWordJustification(), subsequent calls to either of the 
	two DrawText() functions are affected until the number of spaces specified 
	by aNumSpaces is used up.
	
	The easiest way to find out the excess width and number of spaces is to call 
	CFont::MeasureText(). This function can also perform counting, which is 
	finding how much of some text will fit into a given width.
	
	Use CFont::TextCount() to return the excess width.
	
	For example, in the string "To be, or not to be", there are five inter-word 
	gaps. If there are six excess pixels they will be distributed in the 
	proportion 	2, 1, 1, 1, 1 between the words. If there are nine excess pixels 
	they will be distributed in the proportion 2, 2, 2, 2, 1 between the words.
	
	Notes:
	
	If the excess width is zero, then calling SetWordJustification() has no 
	effect.
	
	At first sight it may appear that SetWordJustification() is not required 
	because you can simply call DrawText() for each word. However, underlined 
	justified text does not work using this strategy  you get a non-underlined 
	gap between the space and the beginning of the next word.
	
	@param aExcessWidth The width (in pixels) to be distributed between the 
	specified number of spaces. 
	@param aNumGaps The number of word spaces (characters with the code U+0020) 
	over which the change in width is distributed. */
	virtual void SetWordJustification(TInt aExcessWidth,TInt aNumGaps)=0;

	/** Sets character justification.
	
	This function is required by the Text Views API, and is not intended for 
	regular use by developers.
	
	It affects the strings of text used in the calls to DrawText() that follow, 
	until the number of characters drawn equals aNumChars.
	
	The text line that is to be justified has a certain number of characters
	this includes the spaces between the words. It also has a distance (in 
	pixels) between the end of the last word and the actual end of the line 
	(right hand margin, usually). These excess width pixels are distributed 
	amongst all the characters, increasing the gaps between them, to achieve 
	full justification of the text line.
	
	Use CFont::TextCount() to return the excess width.
	
	Notes:
	
	This function is provided to allow simulation of printer fonts on screen. 
	Due to the fact that fully-scalable fonts are not used before v5, large 
	printer fonts can be simulated by using the nearest smaller font and 
	widening it slightly.
	
	If the excess width is zero, then calling SetCharJustification() has no 
	effect.
	
	SetCharJustification() is required for WYSIWYG where the layout uses 
	printer font metrics but screen fonts have to be drawn on the screen. 
	Because continuously scalable typefaces (c.f. TrueType) are not used 
	before v5 and because screen fonts are coarser and less numerous in 
	their variety than the printer fonts, the best matching smaller screen 
	font must be used with character justification to simulate the printer 
	font on the screen.
	
	There is also a situation where the gaps between characters on screen have 
	to be reduced with character clipping. The screen font that best matches 
	the printer font may have the required height, but has characters that are 
	too wide. A line of text that works on the printer will then be too long 
	on the screen, unless it is squashed horizontally. The number of pixels 
	that overlap the end of the screen line must now be removed from the gaps 
	between the characters, i.e. there is a negative excess width. This 
	situation is especially important where adding a TAB on screen gives 
	perfectly acceptable printout, but would push the last character of the 
	line off the right hand side of the screen.
	
	In practice what you do in printer layout mode is:
	
	Calculate where the line breaks will come on the printer. To do this you 
	use a printer font (which in practice means a table of character widths 
	of the font that the printer will use).
	
	Now change to use a screen font that is the closest font which is no taller 
	that the printer font. In practice it will often be fatter maybe only for 
	certain characters such as 'i'.
	
	You have to recalculate the width of the characters using the screen fonts. 
	You can do this using CFont::TextWidth() as you have already determined how 
	many characters will fit on the line.
	
	If, in the screen font, the characters are not as wide as the line then you 
	can just use word justification to expand the line. You would only do this 
	if the text is to be justified.
	
	If, however, the characters are wider than the line then you would use 
	character justification to clip each character. You would need to do this 
	even if the line is not justified.
	
	Thus, in practice, character justification will only very rarely be used to 
	expand a line of characters.
	
	@param aExcessWidth The excess width (in pixels) to be distributed between 
	the specified number of characters. It may be positive, in which case the text is 
	stretched, or negative, in which case it is shrunk.
	@param aNumChars The number of characters involved. */
	virtual void SetCharJustification(TInt aExcessWidth,TInt aNumChars)=0;

	/** Sets the pen colour.
	
	The effective pen colour depends on the drawing mode. The default pen colour 
	is black.
	
	Note:
	
	The pen is used to draw lines, the outlines of filled shapes, and text. In case
	of outlined text, the pen is used to draw the outline of the font.
	
	The	class provides member functions to set the colour of the pen, the style of 
	line and the line size drawn.
	
	@param aColor An RGB colour for the pen. 
	@see CGraphicsContext::SetDrawMode() */
	virtual void SetPenColor(const TRgb& aColor)=0;
 
	/** Sets the line drawing style for the pen. 
	
	There are 6 pen styles. If no pen style is set, then the default is 
	TPenStyle::ESolidPen. To use a pen style, its full context must be given, 
	e.g. for a null pen:
	
	CGraphicsContext::TPenStyle::ENullPen
	Notes:
	
	The pen is used to draw lines, the outlines of filled shapes, and text. 
	CGraphicsContext member functions are provided to set the colour of the 
	pen, the style of line and the line size drawn.
	
	The TPenStyle::ENullPen style should be used if a border is not required 
	around a filled shape.
	
	Dotted and dashed pen styles have a device dependant implementation, always 
	give single-pixel size lines on the screen whatever the pen size set 
	by SetPenSize() and can only be used for straight lines, polylines, 
	non-rounded rectangles and polygons.
	
	The dotted/dashed pattern is continued, without re-starting, for all 
	consecutively drawn straight lines, i.e.
	
	the outlines of rectangles the pattern starts in the top left corner. 
	It is reset at the end of the function call.
	
	the outlines of polygons the pattern starts at the first point. It is 
	reset at the end of the function call.
	
	polylines and straight lines the pattern starts at the first point 
	initially. Consecutive calls to DrawLine() and/or DrawPolyLine(), whether 
	the lines are concatenated or not, continue the pattern. It can be reset 
	by a further call to SetPenStyle() using the same dotted/dashed style 
	parameter.
	
	@param aPenStyle A pen style. 
	@see CGraphicsContext::TPenStyle */
	virtual void SetPenStyle(TPenStyle aPenStyle)=0;

	/** Sets the line drawing size for the pen.
	
	Lines of size greater than one pixel:
	
	are drawn with rounded ends that extend beyond the end points, (as if the 
	line is drawn using a circular pen tip of the specified size).
	
	are always drawn in TDrawMode::EDrawModePEN mode, overriding whatever mode 
	has been set using SetDrawMode().
	
	Notes:
	
	The pen is used to draw lines, the outlines of filled shapes, and text. The 
	class provides member functions to set the colour of the pen, the style of 
	line and the line size drawn.
	
	Wide straight lines and arcs have rounded ends so that concatenated wide 
	lines have smoothly rounded corners at the vertexes.
	
	When lines are made wide, the extra strips of pixels are added equally to 
	both sides of the line. This works precisely for lines of odd pixel size 
	(3, 5, 7, etc.). Wide lines of even pixel size, (2, 4, 6, etc.), 
	have the extra strip of pixels added to the right and/or below the line.
	
	Wide outlines of ellipses and wide line arcs are drawn with the pixels 
	distributed either side of a thin (single pixel wide) true ellipse 
	constructed in the normal manner. Wide ellipses and arcs of even pixel 
	size have the extra strip of pixels added to the right and/or below the 
	curved line. This gives a slight asymmetry to ellipses.
	
	If the pen style is dotted or dashed, the size specification is ignored: a 
	single-pixel wide primitive is drawn, (this is device dependant).
	
	A line size of zero is handled as if the pen style had been set to 
	TPenStyle::ENullPen.
	
	@param aSize A line size. The default is 1 pixel. */
	virtual void SetPenSize(const TSize& aSize)=0;

	/** Sets the brush colour.
	
	The effective brush colour depends on the drawing mode.
	
	Notes:
	
	The brush is used for filling shapes and the background of text boxes. In 
	case of outlined text, the brush is used for filling the font. The brush 
	has colour, style, pattern and pattern origin parameters.
	
	If no brush colour has been set, it defaults to white. However the default 
	brush style is null, so when drawing to a window the default appears to be 
	the window's background colour.
	
	@param aColor An RGB colour for the brush. 
	@see SetDrawMode() */
	virtual void SetBrushColor(const TRgb& aColor)=0;

	/**	Sets the brush style.

	Ten brush styles are provided, including six built-in hatching patterns.
	Note: The brush is used for filling shapes and the background of text boxes.
	The brush has colour, style, pattern and pattern origin parameters.
	Note: Use TBrushStyle::ENullBrush to draw the outline of a fillable
	shape on its own, without filling.
	Note: If the TBrushStyle::EPatternedBrush style is set, but no bitmap 
	pattern has been selected using UseBrushPattern(), then the function panics.
	Note: Hatching lines are done in the current pen colour, set using SetPenColor(). 
	The hatching pattern starts at the brush origin, set using SetBrushOrigin().
	@see TBrushStyle::ENullBrush
	@see TBrushStyle::EPatternedBrush
	@see UseBrushPattern()
	@see SetPenColor()
	@see SetBrushOrigin()
	@publishedAll
	@released
	@param aBrushStyle A brush style. */
	virtual void SetBrushStyle(TBrushStyle aBrushStyle)=0;

	/** Sets the brush pattern origin.
	
	This specifies the top left-hand corner position for the pattern tile around 
	which copies of the pattern are tiled.
	
	The brush pattern may be a built-in style, or a bitmap. To use a bitmap, the 
	brush must have a pattern set and the brush style must be set to 
	TBrushStyle::EPatternedBrush.
	
	Notes
	
	The brush is used for filling shapes and the background of text boxes. The 
	brush has colour, style, pattern and pattern origin parameters.
	
	If SetBrushOrigin() is not used, then the origin defaults to (0,0).
	
	This brush origin remains in effect for all fillable shapes drawn 
	subsequently, until a new brush origin is set. Shapes can thus be 
	considered as windows onto a continuous pattern field (covering the whole 
	clipping region of a screen device, or the whole device area of a printer).
	
	@param aOrigin An origin point for the brush. The coordinates are relative 
	to the rectangle to fill, i.e. specify 0,0 to align the pattern flush with 
	the top and left hand sides of the rectangle.
	@see SetBrushStyle()
	@see UseBrushPattern() */
	virtual void SetBrushOrigin(const TPoint& aOrigin)=0;

	/** Sets the brush pattern to the specified bitmap.
	
	For the brush to actually use the bitmap, TBrushStyle::EPatternedBrush must 
	be used to set the brush style.
	
	When the brush pattern is no longer required, use DiscardBrushPattern() to 
	free up the memory used, if the bitmap is not being shared. 
	If UseBrushPattern() is used again without using DiscardBrushPattern() 
	then the previous pattern is discarded automatically.
	
	Notes:
	
	The brush is used for filling shapes and the background of text boxes. The 
	brush has colour, style, pattern and pattern origin parameters.
	
	When loading a bitmap, the bitmap is checked to see if it is already in 
	memory. If the bitmap is already there, then that copy is shared.
	
	The brush does not need to have a pattern set at all. There are several 
	built-in hatching patterns which can be selected using SetBrushStyle().
	
	@param aBitmap A bitmap pattern for the brush. 
	@see SetBrushStyle() */
	virtual void UseBrushPattern(const CFbsBitmap* aBitmap)=0;

	/** Discards a non-built-in brush pattern.
	
	This frees up the memory used by the bitmap, if it is not being shared by 
	another process.
	
	Notes:
	
	The brush is used for filling shapes and the background of text boxes. The 
	brush has colour, style, pattern and pattern origin parameters.
	
	If DiscardBrushPattern() is used, with no brush pattern set, then there is 
	no effect. */
	virtual void DiscardBrushPattern()=0;


	/** Sets the drawing point relative to the co-ordinate origin. 
	
	A subsequent call to DrawLineTo() or DrawLineBy() uses the new drawing 
	point as the start point for the line drawn.
	
	Notes
	
	The operations DrawLine(), DrawLineTo(), DrawLineBy() and DrawPolyline() 
	also change the internal drawing position to the last point of the drawn 
	line(s). 
	
	The internal drawing position is set to the co-ordinate origin if no drawing 
	or moving operations have yet taken place.
	
	@param aPoint The new internal drawing position. */
	virtual void MoveTo(const TPoint& aPoint)=0;

	/** Sets the drawing point relative to the current co-ordinates.
	
	A subsequent call to DrawLineTo() or DrawLineBy() uses the new drawing point 
	as the start point for the line drawn.
	
	Notes
	
	The operations DrawLine(), DrawLineTo(), DrawLineBy() and DrawPolyline() 
	also change the internal drawing position to the last point of the drawn 
	line(s). 
	
	The internal drawing position is set to the co-ordinate origin if no drawing 
	or moving operations have yet taken place.
	
	@param aVector The amount by which the internal drawing position is to move. */
	virtual void MoveBy(const TPoint& aVector)=0;

	/** Draws a single point. The point is drawn with the current pen settings 
	using the current drawing mode.
	
	Note:
	
	If the pen size is greater than one pixel, a filled circle of the current 
	pen colour is drawn, with the pen size as the diameter and the plotted point 
	as the centre. If the pen size is an even number of pixels, the extra pixels 
	are drawn below and to the right of the centre.
	
	@param aPoint The point to be drawn.
	@see SetPenSize() */
	virtual void Plot(const TPoint& aPoint)=0;

	/** Draws an arc.
	
	The arc is considered a portion of an ellipse. The ellipse is defined by the 
	TRect argument.
	
	The pixels at both the start point and the end point are drawn.
	
	The arc itself is the segment of the ellipse drawn in an anti-clockwise 
	direction from the start point to the end point.
	
	Notes:
	
	A rectangle is used in the construction of the ellipse of which the arc is 
	a segment. This rectangle is passed as an argument of type TRect.
	
	A wide line arc is drawn with the pixels distributed either side of a true 
	ellipse, in such a way that the outer edge of the line would touch the edge 
	of the construction rectangle. In other words, the ellipse used to 
	construct it is slightly smaller than that for a single pixel line size.
	
	If the specified start or end point is at the centre of the ellipse, then 
	the line that defines the start or end of the arc defaults to one extending 
	vertically above the centre point.
	
	If the start and end point are the same point or are points on the same line 
	through the ellipse centre then a complete unfilled ellipse is drawn.
	
	@param aRect A rectangle in which to draw the ellipse, of which the arc is 
	a segment. 
	@param aStart The point defining the start of the arc. It defines one end of 
	a line from the geometric centre of the ellipse. The point of intersection 
	between this line and the ellipse defines the start point of the arc.
	@param aEnd The point defining the end of the arc. It defines one end of a 
	second line from the geometric centre of the ellipse. The point of 
	intersection between this line and the ellipse defines the end point of the 
	arc.
	@see DrawEllipse() */
	virtual void DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)=0;

	/** Draws a straight line between two points.
	
	@param aPoint1 The point at the start of the line. 
	@param aPoint2 The point at the end of the line. */
	virtual void DrawLine(const TPoint& aPoint1,const TPoint& aPoint2)=0;

	/** Draws a straight line from the current drawing point to a specified 
	point.
	
	@param aPoint The point at the end of the line. 
	@see MoveTo()
	@see MoveBy() */
	virtual void DrawLineTo(const TPoint& aPoint)=0;

	/** Draws a straight line relative to the current drawing point, using a 
	vector.
	
	The start point of the line is the current drawing point. The specified 
	vector 
	is added to the drawing point to give the end point of the line
	
	@param aVector The vector to add to the current internal drawing position, 
	giving the end point of the line. 
	@see MoveTo()
	@see MoveBy() */
	virtual void DrawLineBy(const TPoint& aVector)=0;

	/** Draws a polyline from a set of points in an array.
	
	A polyline is a series of concatenated straight lines joining a set of 
	points.
	
	@param aPointList An array containing the points on the polyline. */
	virtual void DrawPolyLine(const CArrayFix<TPoint>* aPointList)=0;

	/** Draws a polyline from a set of points in a list.
	
	A polyline is a series of concatenated straight lines joining a set of 
	points.
	
	@param aPointList Pointer to a set of points on the polyline.
	@param aNumPoints Number of points in the list. */
	virtual void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)=0;

	/** Draws and fills a pie slice.
	
	The pie slice is an area bounded by:
	
	the arc of an ellipse drawn in an anticlockwise direction from the start 
	point to the end point
	
	the straight line drawn to the start point from the geometric centre of the 
	ellipse.
	
	the straight line to the end point from the geometric centre of the ellipse.
	
	Notes:
	
	A rectangle is used in the construction of the pie slice. This rectangle is 
	passed as an argument of type TRect. The curved edge of the pie slice is an 
	arc of an ellipse constructed within the rectangle.
	
	The line drawn by the pen goes inside the specified rectangle.
	
	The pixels at the end point of the arc are not drawn.
	
	A wide line edged pie slice has the arc drawn with the pixels distributed 
	either side of a true ellipse. This is done in such a way that the outer 
	edge of the line touches the edge of the construction rectangle. In other 
	words, the ellipse used to construct it is slightly smaller than that for 
	a single pixel line size.
	
	If the specified start or end point is at the centre of the ellipse, then 
	the line that defines the start or end of the arc defaults to one extending 
	vertically above the centre point.
	
	If the start and end point are the same point or are points on the same line 
	through the ellipse centre then a complete filled ellipse is drawn. A line 
	is also drawn from the edge to the ellipse centre.
	
	@param aRect A rectangle in which to draw the ellipse bounding the pie slice. 
	@param aStart A point defining the start of the arc bounding the pie slice. 
	It defines one end of a line from the geometrical centre of the ellipse. The 
	point of intersection between this line and the ellipse defines the start 
	point of the arc.
	@param aEnd A point to define the end of the arc bounding the pie slice. It 
	defines one end of a second line from the geometrical centre of the ellipse. 
	The point of intersection between this line and the ellipse defines the end 
	point of the arc. */
	virtual void DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)=0;

	/** Draws and fills an ellipse.
	
	The ellipse is drawn inside the rectangle defined by the TRect argument. Any 
	rectangle that has odd pixel dimensions, has the bottom right corner trimmed 
	to give even pixel dimensions before the ellipse is constructed.
	
	Note:
	
	A wide outline ellipse is drawn with the pixels distributed either side of 
	a true ellipse, in such a way that the outer edge of the line touches the 
	edge of the construction rectangle. In other words, the ellipse used to 
	construct it is smaller than that for a single pixel line size.
	
	@param aRect The rectangle in which the ellipse is drawn. */
	virtual void DrawEllipse(const TRect& aRect)=0;

	/** Draws and fills a rectangle.
	
	@param aRect The rectangle to be drawn. */
	virtual void DrawRect(const TRect& aRect)=0;

	/** Draws and fills a rectangle with rounded corners.
	
	The rounded corners are each constructed as an arc of an ellipse.
	
	The line drawn by the pen, if any, goes inside the specified rectangle.
	
	Notes:
	
	Dotted and dashed pen styles cannot be used for the outline of a rounded 
	rectangle.
	
	If either corner size dimension is greater than half the corresponding 
	rectangle length, the corner size dimension is reduced to half the 
	rectangle size.
	
	@param aRect The rectangle to be drawn. 
	@param aCornerSize The dimensions of each corner. 
	@see DrawArc() */
	virtual void DrawRoundRect(const TRect& aRect,const TSize& aCornerSize)=0;

	/** Draws and fills a polygon defined using an array of points.
	
	The first point in the array defines the start of the first side of the 
	polygon. The second point defines the second vertex (the end point of the 
	first side and the start point of the second side).
	
	The final side of the polygon is drawn using the last point from the array, 
	and the line is drawn to the start point of the first side.
	
	Self-crossing polygons are filled according to the specified fill rule.
	
	@param aPointList An array of points, specifying the vertices of the polygon.
	@param aFillRule The fill rule. By default, this is TFillRule::EAlternate. 
	@return KErrNone, if successful; otherwise, another of the system-wide error 
	codes. */
	virtual TInt DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule=EAlternate)=0;

	/** Draws and fills a polygon defined using a list of points.
	
	The first point in the list defines the start of the first side of the 
	polygon. The second point defines the second vertex (the end point of the 
	first side and the start point of the second side).
	
	The final side of the polygon is drawn using the last point from the list, 
	and the line is drawn to the start point of the first side.
	
	Self-crossing polygons are filled according to the specified fill rule.
	
	@param aPointList Pointer to list of points, specifying the vertices of the 
	polygon.
	@param aNumPoints The number of points in the list.
	@param aFillRule The fill rule. By default this is TFillRule::EAlternate. 
	@return KErrNone, if successful; otherwise, another of the system-wide error 
	codes. */
	virtual TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule=EAlternate)=0;

	/** Draws a bitmap at the specified point.
	
	The point specifies the top left hand corner of the bitmap. The bitmap is 
	compressed or stretched based on its internally stored size in twips.
	
	Notes:
	
	This member function uses the bitmap's size in twips and does a 
	stretch/compress blit using a linear DDA.
	
	As this function scales the bitmap, it is unavoidably slow. Therefore, where 
	possible, use CBitmapContext::BitBlt() instead. If the bitmap has to be 
	scaled, consider creating another bitmap along with an CFbsBitmapDevice etc, 
	doing DrawBitmap() once and using BitBlt() subsequently.
	
	Note that all bitmaps are clipped to the device boundaries.
	
	@param aTopLeft The point where the top left pixel of the bitmap is to be 
	drawn 
	@param aSource A source bitmap 
	@see TLinearDDA */
	virtual void DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource)=0;

	/** Draws a bitmap to fit a given rectangle. 
	
	The bitmap is compressed or stretched based on its internally stored size 
	in pixels.
	
	Notes:
	
	This member function uses the bitmap's size in pixels and does a 
	stretch/compress blit using a linear DDA.
	
	As this function scales the bitmap, it is unavoidably slow. Therefore, 
	where possible, use CBitmapContext::BitBlt() instead. If the bitmap has 
	to be scaled, consider creating another bitmap along with an 
	CFbsBitmapDevice etc., doing DrawBitmap() once and using BitBlt() 
	subsequently.
	
	Note that all bitmaps are clipped to the device boundaries.
	
	@param aDestRect The rectangle within which the bitmap is to be drawn. 
	@param aSource A source bitmap. 
	@see TLinearDDA */
	virtual void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource)=0;

	/** Draws a specified rectangle of a source bitmap to fit into a given 
	destination rectangle.
	
	Notes:
	
	This member function uses rectangle sizes in pixels and does a 
	stretch/compress blit using a linear DDA.
	
	As this function scales the bitmap, it is unavoidably slow. Therefore, 
	where possible, use CBitmapContext::BitBlt() instead. If the bitmap has 
	to be scaled, consider creating another bitmap along with an 
	CFbsBitmapDevice etc., doing DrawBitmap() once and using BitBlt() 
	subsequently.
	
	Note that all bitmaps are clipped to the device boundaries.
	
	@param aDestRect The rectangle within which the bitmap is to be drawn. 
	@param aSource A source bitmap. 
	@param aSourceRect The rectangle in the source bitmap that is copied to the 
	destination rectangle. 
	@see TLinearDDA */
	virtual void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,const TRect& aSourceRect)=0;

	/** Draws a specified rectangle of a source bitmap to fit into a given rectangle using a given mask.

	Notes:
	
	  This member function uses rectangle sizes in pixels and does a 
	stretch/compress blit using a linear DDA.
	
	
	@param aDestRect The rectangle within which the bitmap is to be drawn. 
	@param aBitmap The source bitmap
	@param aSourceRect The rectangle in the source bitmap that is to be drawn
	@param aMaskBitmap The mask to be applied to the source bitmap while drawing
	@param aInvertMask Flag to indicate if the mask should be inverted.
	*/
	virtual void DrawBitmapMasked(const TRect& aDestRect,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask)=0;

	/** Draws a specified rectangle from a wserv bitmap and its mask into 
	another rectangle.

	The function compresses/stretches the specified rectangle from the bitmap 
	to fit the destination rectangle. 
	The mask bitmap can be used as either a positive or negative mask. Masked 
	pixels are not mapped to the destination rectangle.

	A black and white (binary) mask bitmap is used. With aInvertMask=EFalse, black 
	pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
	being transferred to the destination rectangle. With aInvertMask=ETrue, white 
	pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
	being transferred to the destination rectangle.

	Note: this member function uses rectangle sizes in pixels and does a stretch/compress 
	blit using a linear DDA.

	@param aDestRect The rectangle within which the masked bitmap is to be drawn. 
	@param aBitmap A source wserv bitmap. 
	@param aSourceRect The rectangle in the source bitmap that is copied to the 
	destination rectangle.
	@param aMaskBitmap A mask wserv bitmap. 
	@param aInvertMask If false, a source pixel that is masked by a black pixel 
	is not transferred to the destination rectangle. If true, then a source pixel 
	that is masked by a white pixel is not transferred to the destination rectangle. */
	virtual void DrawBitmapMasked(const TRect& aDestRect,const CWsBitmap* aBitmap,const TRect& aSourceRect,const CWsBitmap* aMaskBitmap,TBool aInvertMask)=0;

	/** Draws text without a surrounding box. 
	
	The text baseline is aligned with the y co-ordinate of the specified point, 
	and the left end of the text is aligned with the x co-ordinate of the 
	specified point.
	
	Note:
	
	Text drawing is done with the pen, and is subject to the pen colour. The 
	effective text colour also depends on the drawing mode. The size and style 
	of the text depends on the font used. The layout of the text depends on the 
	justification mode set.
	
	@param aText The text string to be drawn. 
	@param aPosition A point specifying the position of the left end of the text. */
	virtual void DrawText(const TDesC& aText,const TPoint& aPosition) = 0;

	/** Draws text inside a box.
	
	The surrounding box is filled with the current brush colour (not a pattern) 
	and is drawn without any outline. The effective box colour depends on the 
	drawing mode - if a brush colour has not been set then the brush defaults 
	to white. The brush may be set to TBrushStyle::ENullBrush if text 
	positioning relative to a box is required, but the box should not be filled.
	
	The font used is that set by UseFont(). If no font is in use then a panic 
	occurs.
	
	The alignment of the text within the box can be specified.
	
	Text drawn within a box is also clipped to that box. Unless you intend to 
	clip the top off the text, aBaselineOffset should be greater than or equal 
	to the ascent of the current font.
	
	Offsets:
	
	If the offset is negative, zero, or less than font height this is handled 
	as would be expected, i.e. no text will be seen in the box in the first two 
	instances, and the top of the text will be clipped in the latter case.
	
	Margins:
	
	For the drawing of right-aligned text, aLeftMargin indicates the margin from 
	the right of aBox - where a positive value results in a leftwards offset. 
	
	Negative margins can be used to display portions of the text string clipped 
	by the box. A negative margin for left aligned text would clip the start of 
	the text string. Similarly, a negative margin for right aligned text would 
	clip the end of the text string.
	
	If the margin is greater than the width of the box then no text will be 
	visible.
	
	The margin is still honoured for centred text - centred text will not be 
	centred in the box, unless the margin is zero.
	
	Note:
	
	Text drawing is done with the pen, and is thus subject to the pen colour. 
	The effective text colour also depends on the drawing mode. The size and 
	style of the text depends on the used font. The layout of the text depends 
	on the justification mode set.
	
	@param aText The text string to be drawn. 
	@param aBox The box to draw the text in. 
	@param aBaselineOffset An offset from the top of the box to the text 
	baseline. 
	@param aAlignment The text alignment mode default is left aligned. 
	@param aLeftMargin The left margin for left-aligned text, or the right 
	margin 
	for right-aligned text default is zero. */
	virtual void DrawText(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TTextAlign aAlignment = ELeft,
		TInt aLeftMargin = 0) = 0;
	IMPORT_C virtual void DrawText(const TDesC& aText,const TPoint& aPosition,const TDrawTextParam& aParam);
	IMPORT_C virtual void Reserved();
	IMPORT_C TInt DrawTextExtended(const TDesC& aText,const TPoint& aPosition,const TDrawTextExtendedParam& aParam);
							
	/** Maps pixels in the specified rectangle.
	The function tries to match the colour of a pixel with one of the RGB values 
	in an array of RGB pairs. If there is a match, the colour is changed to the 
	value specified in the other RGB in the RGB pair.
	@param aRect The rectangle in which pixels are to be mapped.
	@param aColors A pointer to a set of RGB pairs. 
	@param aNumPairs The number of pairs 
	@param aMapForwards ETrue, mapping is done from the first RGB to the second 
	RGB in the pair; EFalse, mapping is done from the second RGB to the first 
	RGB in the pair. */	
	virtual void MapColors(const TRect &aRect,const TRgb *aColors,TInt aNumPairs,TBool aMapForwards) = 0;
	
	/** Sets the clipping region, any items that fall outside the extent of the clipping 
	region will not be drawn.
	
	Note that clipping is additive. If a clipping rectangle has been set using SetClippingRect() 
	then clipping will be to the intersection of that rectangle and this region.
	
	@param aRegion The new clipping region. Note that clipping region co-ordinates are
	used as absolute co-ordinates, they are not transformed by the current co-ordinate 
	origin before use (as occurs in SetClippingRect()).
	 
	@return KErrNone if successful; KErrArgument if the TRegion is not valid; KErrNoMemory if there is insufficient memory.
	
	@see CGraphicsContext::CancelClippingRegion() 
	@see CGraphicsContext::SetClippingRect() */		
	virtual TInt SetClippingRegion(const TRegion &aRegion) = 0;
	
	/** Cancels the current clipping region. 
	@see CGraphicsContext::SetClippingRegion()*/
	virtual void CancelClippingRegion() = 0;
	
	/** Draws vertical text in the specified direction.
	@param aText The text to be drawn. 
	@param aPos Point of origin of the text baseline. 
	@param aUp Direction. ETrue for up, EFalse for down. */
	virtual void DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp) = 0;
	
	/** Draws text vertically in the specified direction, within a box of the specified size.
	@param aText The text to be drawn. 
	@param aBox The bounding box within which the text should be drawn, and which it is clipped to.
	@param aBaselineOffset The height of the top of the characters from their text baseline. 
	@param aUp The direction. ETrue for up, EFalse for down.
	@param aVert The text alignment. 
	@param aMargin The margin. */	
	virtual void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0) = 0;
	
	IMPORT_C TInt GetUnderlineMetrics(TInt& aTop,TInt& aBottom);

	/** Set the font's shadow colour
	@param aShadowColor Shadow colour to be set.
	@return KErrNone, if successful; otherwise, another of the system-wide errors. */
	IMPORT_C TInt SetShadowColor(const TRgb& aShadowColor);

	/** Get the font's shadow colour
	@param aShadowColor Shadow colour of the font returned by the funtion.
	@return KErrNone, if successful; otherwise, another of the system-wide errors. */
	IMPORT_C TInt GetShadowColor(TRgb& aShadowColor);

	/** Determine if the Gc is a CFbsBitGc
	@return ETrue, if the Gc is a CFbsBitGc, EFalse otherwise 
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases.
	*/
	IMPORT_C TBool IsFbsBitGc() const;

	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* iParam,const TPoint& aPosition);
	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* iParam,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHrz=ELeft,TInt aMargin=0);
	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* iParam,const TPoint& aPosition,const TDrawTextParam& aParam);

	IMPORT_C void DrawTextVertical(const TDesC& aText,const TTextParameters* iParam,const TPoint& aPos,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TTextParameters* iParam,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	
	IMPORT_C TInt DrawTextExtended(const TDesC& aText,const TTextParameters* iParam,const TPoint& aPosition,const TDrawTextExtendedParam& aParam);
	
protected:

	/**
	An APIExtension method to allow the addition of new APIs to retain compatibility 
	with previous versions of gdi.dll
	@param aOutput is for output
	@param aInput is for input
	@see CGraphicsContext
	@publishedAll
	WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases.
	*/
	IMPORT_C virtual TInt APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput);	
	
	/**
	A reserved virtual function for future use.
	*/
	IMPORT_C virtual void Reserved_CGraphicsContext_2();		
	};


/** An abstract, device-independent, interface to bitmapped graphics contexts.

This holds the setting used to draw to a CBitmapDevice.

The default settings of a CBitmapContext object immediately after construction 
are:

drawing mode is EDrawModePen (pen and brush colours used as they are)

no clipping rectangle

pen settings are: black, solid, single pixel width

brush style is null

no text font selected

The classes CFbsBitGc and CWindowGc are derived from this class.

@see CFbsBitGc 
@publishedAll
@released
*/
class CBitmapContext : public CGraphicsContext
	{
public:
	/** Clears the whole bitmap.
	
	The cleared area is filled with the current brush colour.
	
	This pure virtual function is implemented in derived classes. */
	virtual void Clear()=0;

	/** Clears a rectangular area of a bitmap.
	
	The cleared area is filled with the current brush colour.
	
	This pure virtual function is implemented in derived classes.
	
	@param aRect The rectangle to clear. */
	virtual void Clear(const TRect& aRect)=0;

	/** Copies a rectangle.
	
	This pure virtual function is implemented in derived classes.
	
	@param aOffset The offset from the top left corner of the rectangle to be 
	copied to the top left corner of the copy.
	@param aRect The rectangular area to be copied. */
	virtual void CopyRect(const TPoint& aOffset,const TRect& aRect)=0;

	/** Performs a bitmap block transfer.
	
	This pure virtual function is implemented in derived classes.
	
	@param aPoint The destination for the top left corner of the transferred bitmap. 
	It is relative to the top left corner of the destination bitmap, which may be the screen.
	@param aBitmap A memory-resident bitmap. */
	virtual void BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap)=0;

	/** Performs a bitmap block transfer of a rectangular piece of a bitmap.
	
	If the specified rectangle is larger than the bitmap then the bitmap is 
	padded with white.
	
	This pure virtual function is implemented in derived classes.
	
	@param aPoint The destination for the top left corner of the transferred bitmap.
	It is relative to the top left corner of the destination bitmap, which may be the screen.
	
	@param aBitmap A memory-resident bitmap 
	@param aRect A rectangle defining the portion of the bitmap to transfer. 
	Its coordinates are relative to the top left corner of the source bitmap.   */
	virtual void BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aRect)=0;

	/** Performs a masked bitmap block transfer.
	
	The mask bitmap can be used as either a positive or negative mask. Masked 
	pixels are not mapped to the destination rectangle.
	
	This function uses either a black and white (binary) mask bitmap, or if 
	aMaskBitmap's display mode is EGray256, alpha blending is used. Use of 
	any other mode may result in unpredictable results
	
	With aInvertMask=EFalse, black pixels in the mask bitmap stop corresponding 
	pixels in the source bitmap from being transferred to the destination rectangle. 
	With aInvertMask=ETrue, white pixels in the mask bitmap stop corresponding 
	pixels in the source bitmap from being transferred to the destination 
	rectangle.
	
	Note�that if the mask bitmap is smaller than the source bitmap, then it is 
	tiled across the bitmap. Note that the mask is applied before the piece of 
	the bitmap is defined - the mask is tiled relative to the top left of the 
	original source bitmap rather than the top left of the bitmap piece.
	
	This pure virtual function is implemented in derived classes.
	
	@param aPoint The destination for the top left corner of the transferred bitmap. 
	It is relative to the top left corner of the destination bitmap, which may be the screen. 
	@param aBitmap A memory-resident source bitmap. 
	@param aSourceRect A rectangle defining the piece of the bitmap to be drawn, 
	with co-ordinates relative to the top left corner of the bitmap. 
	@param aMaskBitmap A mask bitmap 
	@param aInvertMask If  EFalse, a source pixel that is masked by a black pixel 
	is not transferred to the destination rectangle. If ETrue, then a source 
	pixel that is masked by a white pixel is not transferred to the destination 
	rectangle. */
	virtual void BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask)=0;

	/** Sets whether the graphics context is faded.
	
	@param aFaded ETrue to fade the GC; EFalse to unfade it. */
	virtual void SetFaded(TBool aFaded)=0;

	/** Sets the fading parameters.
	
	This function allows you to override the map used when drawing with a faded 
	graphics context (GC). However if you draw to a faded window with a faded 
	GC, then fading on the GC is ignored and the fading of the window is used.
	
	Fading is used to change the colour of a window to make other windows stand 
	out. Fading can either make a faded window closer to white or closer to 
	black.
	
	Fading re-maps colours in the faded GC to fall between the specified black 
	and white map values. If aBlackMap=0 and aWhiteMap=255 then the colours are 
	mapped unchanged. As the values converge the colours are mapped to a smaller 
	range - so the differences between colours in the faded GC decrease. If 
	the values are reversed then the colours are inverted (i.e. where the GC 
	would be black, it is now white). 
	
	@param aBlackMap Black map fading parameter. Unfaded this is 0. 
	@param aWhiteMap White map fading parameter. Unfaded this is 255. */
	virtual void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap)=0;

	/**
	Performs an alpha blending of the source data, aSrcBmp, with the CBitmapContext, using
	the data from aAlphaBmp as an alpha blending factor.
	The formula used is:
	(S * A + W * (255 - A)) / 255, where:
	- S - a pixel from aSrcBmp;
	- W - a pixel from the window;
	- A - a pixel from aAlphaBmp;
	The contents of source and alpha bitmap are preserved.
	The calculated alpha blended pixels are written to the destination CBitmapContext.
	@param aDestPt Position in the target the result should be drawn to.
	@param aSrcBmp A pointer to the source bitmap.
	@param aSrcRect The part of the source bitmap that should be used.
	@param aAlphaBmp A pointer to the bitmap used as an alpha blending factor.
	@param aAlphaPt Position of the first pixel in the alpha bitmap that should be used as a source 
	                for the alpha blending. The size of the area is the same as the 
	                source bitmap area - aSrcRect parameter.*/	
	
	virtual TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt) = 0;	
	
	/**
	The method performs an alpha blending of the source data, aSrcBmp, with the CBitmapContext, using
	the data from aAlphaBmp as an alpha blending factor.
	For information on how this function works, see the other overload.
	@param aDestPt Position in the target the result should be drawn to.
	@param aSrcBmp A pointer to the source bitmap.
	@param aSrcRect The part of the source bitmap that should be used.
	@param aAlphaBmp A pointer to the bitmap used as an alpha blending factor.
	@param aAlphaPt Position of the first pixel in the alpha bitmap that should be used as a source 
	                for the alpha blending. The size of the area is the same as the 
	                source bitmap area - aSrcRect parameter.*/
	virtual	TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp,	const TRect& aSrcRect, const CWsBitmap* aAlphaBmp, const TPoint& aAlphaPt) = 0;
	
protected:

	/*
	Implementations from CGraphicsContext
	@see CGraphicsContext::APIExtension(TUid, TAny*&, TAny*)
	*/
	IMPORT_C TInt APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput);	
	/*
	Implementations from CGraphicsContext
	@see CGraphicsContext::Reserved_CGraphicsContext_2()
	*/
	IMPORT_C void Reserved_CGraphicsContext_2();

	IMPORT_C virtual void Reserved_CBitmapContext_1();
	IMPORT_C virtual void Reserved_CBitmapContext_2();
	IMPORT_C virtual void Reserved_CBitmapContext_3();
	};

/** Defines an abstract interface for the capabilities and attributes of a 
bitmapped graphics device.

The class specialises the graphics device interface CGraphicsDevice for bitmaps 
graphics. The Window Server Client-Side API provides one implementation of 
the interface, CWsScreenDevice, for screen drawing. Another implementation, 
CFbsBitmapDevice, is used for drawing to in-memory bitmaps. A third, 
CFbsScreenDevice, is used (rarely) to access the screen directly, without the 
mediation of the window server. 
@publishedAll
@released
*/
class CBitmapDevice : public CGraphicsDevice
	{
public:
	/** Gets the RGB colour of an individual pixel on a bitmapped graphics 
	device. 
	
	This pure virtual function is implemented in derived classes.
	
	@param aColor On return, should contain the RGB colour of the pixel. 
	@param aPixel The (x,y) co-ordinates of the pixel. The top left pixel is 
	(0,0). */
	virtual void GetPixel(TRgb& aColor,const TPoint& aPixel) const=0;

	/** Copies a scanline into a buffer.
	
	The pixels are converted from the display mode format on the bitmapped 
	graphics device to the format of the specified device display mode.
	
	By specifying the start pixel and the number of pixels, either the whole or 
	a portion of a bitmapped graphics device pixel row may be copied.
	
	This pure virtual function is implemented in derived classes.
	
	@param aBuf An 8 bit modifiable descriptor buffer into which pixels are 
	copied; it must be sufficiently large to store all the scanline pixels. 
	@param aStartPixel The (x,y) co-ordinates of the first pixel of the bitmap 
	scanline to be put into the buffer. 
	@param aLength The number of pixels to put into the buffer. 
	@param aDispMode The display mode into which to convert the pixels. */
	virtual void GetScanLine(TDes8& aBuf,const TPoint& aStartPixel,TInt aLength,TDisplayMode aDispMode) const=0;

	/** Adds a font file to the device's typeface store. The specified font
	file must be accessible to any process, i.e. not located inside an
	application's private directory.
	
	This pure virtual function is implemented in derived classes.
	
	@param aName The name of the font file. 
	@param aId On return, contains an ID for the font file. 
	@return KErrNone, if successful; otherwise, another of the system-wide error 
	codes. */
	virtual TInt AddFile(const TDesC& aName,TInt& aId)=0;

	/** Removes a font file from the font store.
	
	This pure virtual function is implemented in derived classes.
	
	@param aId The ID of the font file to be removed. The default is 0. */
	virtual void RemoveFile(TInt aId=0)=0;

	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().
	
	Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInPixels() 
	yielding (virtually) the same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of pixels, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.
	
	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@deprecated Use GetNearestFontToDesignHeightInPixels
	*/
	virtual TInt GetNearestFontInPixels(CFont*& aFont, const TFontSpec& aFontSpec) = 0;

	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().
	
	This new function replaces the deprecated @c GetNearestFontInPixels() yielding (virtually) the 
	same result. However clients are strongly encouraged to use the new
	@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
	character within any given text string will fit within the given amount of pixels, whereas the design 
	height is an aesthetic unit decided by the font designer without strict physical meaning, which 
	may result in cropped characters.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToDesignHeightInPixels(
		CFont*& /*aFont*/, const TFontSpec& /*aFontSpec*/) { return KErrNotSupported; }

	/**
	Gets the font which is the nearest to the given font specification.
	
	When the font is no longer needed, call @c ReleaseFont().
	
	The font and bitmap server returns a pointer to the nearest matching font 
	from those available. Matches to max height of font - this does its best 
	to return a font that will fit within the maximum height specified (but 
	note that variations due to hinting algorithms may rarely result in this 
	height being exceeded by up to one pixel). Problems can also be 
	encountered with bitmap fonts where the typeface exists but doesn't have 
	a font small enough.

	@param aFont On return, contains a pointer to the nearest font.
	@param aFontSpec The specification of the font to be matched.
	@param aMaxHeight The maximum height within which the font must fit.
	This overrides the height specified in aFontSpec.
	@return KErrNone if successful; a system-wide error code otherwise.
	@publishedAll
	@released
	*/
	virtual TInt GetNearestFontToMaxHeightInPixels(
		CFont*& /*aFont*/, const TFontSpec& /*aFontSpec*/, TInt /*aMaxHeight*/) { return KErrNotSupported; }

	/** Gets the height, in pixels, of the specified typeface at one of its 
	defined heights.
	
	The typeface is identified by by an index. For a given typeface, there are 
	a discrete number of heights; the specific height is also identified by an 
	index.
	
	The value returned is rounded up or down to the nearest font height in 
	pixels.
	
	This pure virtual function is implemented in derived classes.
	
	@param aTypefaceIndex A typeface index number, in the range: 0 to 
	(NumTypefaces() - 1). 
	@param aHeightIndex A font height index number, in the range: 0 to 
	(iNumHeights - 1) where iNumHeights is a public data member of the 
	TTypefaceSupport object returned by TypefaceSupport(). 
	@return The height of the font, in pixels.
	@see TTypefaceSupport
	@see CGraphicsDevice::NumTypefaces()
	@see CGraphicsDevice::TypefaceSupport() */
	virtual TInt FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const=0;
    inline TInt CreateBitmapContext(CBitmapContext*& aGC);
	};

/** A set of margins used for cropping a picture. 

Margins are specified in twips or pixels. 
@publishedAll
@released
*/
class TMargins
    {
public:
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TBool operator==(const TMargins& aMargins) const;
	IMPORT_C TBool operator!=(const TMargins& aMargins) const;
public:
	/** Left margin, (in twips or pixels). */
	TInt iLeft;
	/** Right margin, (in twips or pixels). */
	TInt iRight;
	/** Top margin, (in twips or pixels). */
    TInt iTop;
	/** Bottom margin, (in twips or pixels). */
	TInt iBottom;
	};

/** Picture capabilities.

These include the types of scaling that can be applied to a picture, and whether 
or not it is croppable. 
@publishedAll
@released
*/
class TPictureCapability
	{
public:
	/** Scaling types. */
	enum TScalingType
		{
		/** The picture is not scalable. */
		ENotScaleable,
		/** The picture is fully scalable. */
		EFullyScaleable,
		/** The picture is scalable to any size, as long as its aspect ratio 
		(the ratio of its height to its width) remains constant. */
		EScaleableMaintainingAspectRatio
		};
public:
	inline TPictureCapability(TScalingType aScalingType,TBool aCroppable);
public:
	/** Whether or not the picture is croppable. */
	TScalingType iScalingType;
	/** Scaling type. */
	TBool iIsCroppable;
	};

/** Abstract base class for: drawing a picture to a graphics context, storing 
and restoring the picture. 

The class defines the protocol for a number of concrete picture classes. Its 
main role is to support glass doors used in object embedding.

The class provides a protocol for the provision of scaling and cropping 
functions by derived classes, together with default implementations.

Its main function is Draw(), which draws the picture onto the graphics context 
at a particular point. It also has two important pure virtual functions: 
ExternalizeL() and GetOriginalSizeInTwips().

A picture has both an original representation and an on-screen representation. 
The original representation has a size in twips, and can somehow be drawn. 
The on-screen representation is assumed to be drawn under the control of an 
application which may wish to re-size or scale the original in some way, to 
crop it at the edges, and/or to ensure it fits within a particular defined 
area on the screen.

The class provides several functions that allow an application to set scaling 
and cropping before invoking the Draw() function to draw the picture on-screen. 
It is up to the internal workings of the function to determine the order of 
application cropping and scaling. 
@publishedAll
@released
*/
class CPicture : public CBase
    {
public:
	/** Options for detaching pictures from stores. Used by DetachFromStoreL(). */
	enum TDetach
		{
		/** Internalise all data that is required to fully store the picture 
		later; null any references to containing stores. */
		EDetachFull,
		/** Internalise enough information to draw the picture (and no more); 
		null any references to containing stores. */
		EDetachDraw
		};
public:
	IMPORT_C virtual ~CPicture();
	/** Draws a picture.
	
	@param aGc The graphics context. 
	@param aTopLeft The co-ordinates where the top left corner pixel of the 
	picture should be placed. Note that whether this is actually drawn depends 
	on the clipping area defined.
	@param aClipRect A clipping rectangle that defines the area to which the 
	function should draw. An implementation should never draw outside this 
	rectangle. Note that the graphics context may also have a clipping rectangle 
	set on it. 
	@param aMap The device map for the graphics device. The implementation 
	should use this to find the scaling to apply to the picture. */
	virtual void Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,MGraphicsDeviceMap* aMap) const=0;
	IMPORT_C virtual TStreamId StoreL(CStreamStore& aStore) const; // assumes no sub streams
	virtual void DetachFromStoreL(TDetach /*aDegree*/=EDetachFull) {}

	/** Externalises the picture to a stream.
	
	The presence of this function means that the standard templated stream 
	operator<<() is available to externalise objects of this class.
	
	A derived class must supply an implementation of this function.
	
	@param aStream The write stream. */
	virtual void ExternalizeL(RWriteStream& aStream) const =0;

	/** Gets the pictur's original size, in twips.
	
	@param aSize The size of the picture, in twips */
	virtual void GetOriginalSizeInTwips(TSize& aSize) const =0;
	IMPORT_C virtual void SetScaleFactor(TInt aScaleFactorWidth,TInt aScaleFactorHeight); // does nothing
	IMPORT_C virtual void SetCropInTwips(const TMargins& aMargins); // does nothing
	IMPORT_C virtual TPictureCapability Capability() const; // no scale, no crop
	IMPORT_C virtual void GetCropInTwips(TMargins& aMargins) const; // returns no crop
	IMPORT_C virtual TInt ScaleFactorWidth() const; // returns no scaling
	IMPORT_C virtual TInt ScaleFactorHeight() const; // returns no scaling
	IMPORT_C virtual TBool LineBreakPossible(TUint aClass,TBool aBeforePicture,TBool aHaveSpaces) const;
	IMPORT_C virtual TBool NativePixelSize(TSize& aPixelSize);

	IMPORT_C void GetSizeInPixels(MGraphicsDeviceMap* aMap, TSize& aSize) const;
	IMPORT_C void SetSizeInPixels(MGraphicsDeviceMap* aMap, const TSize& aSize);
	IMPORT_C void AddCropInPixels(MGraphicsDeviceMap* aMap, const TMargins& aMargins);
	IMPORT_C void GetSizeInTwips(TSize& aSize) const;
	IMPORT_C void SetSizeInTwips(const TSize& aSize);
	IMPORT_C void ResetToOriginal();
protected:
	IMPORT_C CPicture();
	};


/** Picture header providing an interface to a stored picture. The header holds:

the picture's type, encoded as a UID, which ensures that it will be restored 
to the correct picture type

the picture's size, which facilitates deferred loading

the stream ID of the picture body, which is replaced by a pointer to the picture 
object when it has been restored. 
@publishedAll
@released
*/
class TPictureHeader
	{
public:
	IMPORT_C TPictureHeader();
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void DeletePicture();
public:
	/** A swizzle storing either the ID of the stream in which the picture is 
	stored, or a pointer to the internalised picture. */
	TSwizzle<CPicture> iPicture;
	/** A UID specifying the type of the picture. */
	TUid iPictureType;
	/** The original size of the picture. */
	TSize iSize;
	};


/** Abstract interface for instantiating and restoring new CPicture derived 
objects.

A concrete derived class creates pictures of one or more specific types. The 
class has no member data and just one function, NewPictureL, that needs to 
be provided by derived classes. 
@publishedAll
@released
*/
class MPictureFactory
	{
public:
	/** Interface to the picture restoration process that ensures that a 
	picture of the correct type is restored.
	
	The class of the object to be restored is determined by the UID of the 
	stored picture, contained in the specified picture header.
	
	The function can allow the restoration of:
	
	just one CPicture-derived class, returning an error if the UID indicates 
	that the stored picture is not of the required type
	
	many different CPicture-derived classes, using the UID to choose which of 
	the possible CPicture-derived classes should be instantiated
	
	@param aHeader The picture header that should be restored. 
	@param aDeferredPictureStore The store in which both the header and picture 
	reside. */
	virtual void NewPictureL(TPictureHeader& aHeader,const CStreamStore& aDeferredPictureStore)const=0;
	};

// printing classes

/**
The maximum length of a printer model name.
@see TPrinterModelName 
@publishedAll
@released
*/
const TInt KMaxPrinterModelNameLength=0x20;

/**
Defines a modifiable buffer descriptor that can contain the name of a
printer model. The maximum length of the buffer is 32.
@since 5.0 
@publishedAll
@released
*/
typedef TBuf<KMaxPrinterModelNameLength> TPrinterModelName;

/** Page specification for a print operation.

  The page specification consists of the page orientation and the page
size in twips or pixels. By default, the page orientation is portrait.
When using landscape orientation, the left hand side of the page
becomes the top. 
"gdi.lib"
@since 5.0
@publishedAll
@released
*/	
class TPageSpec
	{
public:
	/**The available page orientations. */
	enum TPageOrientation
		{
		/** Portrait page orientation */
		EPortrait,
		/** Landscape page orientation */
		ELandscape
		};
public:
	IMPORT_C TPageSpec();
	IMPORT_C TPageSpec(TPageOrientation aOrientation,const TSize& aSize);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TSize OrientedPageSize() const;
	IMPORT_C TBool operator==(const TPageSpec& aPageSpec) const;
	IMPORT_C TBool operator!=(const TPageSpec& aPageSpec) const;
public:
	/** The width and height of the page in portrait orientation in twips or 
	pixels.
	
	Note that OrientedPageSize() returns the width and height in reverse order 
	for a landscape portrait. */
	TSize iPortraitPageSize;
	/** The page orientation. */
	TPageOrientation iOrientation;
	};


/**  Controls the attributes of the band to be printed.

An object of this type is passed as a parameter to functions.
MPageRegionPrinter::PrintBandL() and CPrinterControl::QueueGetBand().
@see MPageRegionPrinter::PrintBandL()
@see CPrinterControl::QueueGetBand() 
@since 5.0
@publishedAll
@released
*/	
class TBandAttributes
    {
public:
	/** The width and height of the band in twips or pixels. */
	TRect iRect;
	/** ETrue to draw no text to the band; EFalse to draw text. */
	TBool iTextIsIgnored; // any text drawing to this band is ignored
	/** ETrue to draw no graphics to the band; EFalse to draw graphics. */
	TBool iGraphicsIsIgnored; // any graphics drawing to this band is ignored
	/** ETrue if band is the first band on the page; EFalse if not. */
	TBool iFirstBandOnPage;
	};

/**  Printer port interface.

This interface is used as the base class for the various types of
printer port. New printer port classes should be derived from this
class.After allocation and construction, a pointer to a concrete
printer port should be passed to
CPrintSetup::StartPrintL().
@see CPrintSetup::StartPrintL() 
@since 5.0
@publishedAll
@released
*/
class CPrinterPort : public CBase
	{
public:
	/** Writes data asynchronously to the printer port.
	
	@param aBuf Data to be written to the port 
	@param aRequestStatus A reference to the request status object. If the 
	request is cancelled, this should be set to KErrCancel; if the request 
	completes normally, this should be set to KErrNone. */
	virtual void WriteRequest(const TDesC8& aBuf,TRequestStatus& aRequestStatus)=0;
 
	/** Cancels an outstanding WriteRequest() operation. */
	virtual void Cancel()=0;
	};


/** Detailed information about a printer model. 
"gdi.lib" 
@since 5.0
@publishedAll
@released
*/
class TPrinterModelEntry
  	{
public:
	/** The name of the printer model. */
	TPrinterModelName iModelName;
	/** If ETrue, then a printer port is required. */
	TBool iRequiresPrinterPort;
	/** The UID associated with this printer model. */
	TUid iUid;
public:
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	};


/** Information about a printer model.

An instance of this class consists of a TPrinterModelEntry and a stream ID, 
and is passed to CPrinterDevice::SetModel().

@see CPrinterDevice 
@publishedAll
@released
*/
class TPrinterModelHeader
    {
public:
	/** The printer model. */
	TPrinterModelEntry iEntry;
	/** The stream ID for model data. */
	TStreamId iModelDataStreamId;
public:
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	};


/** Printer control interface.

This abstract base class acts as the interface between a CPrinterDevice object 
and the printer. It creates the context for, and controls the progress and 
termination of the print job.

Only those who need to add new printer drivers should write derived classes. 
After instantiation of such a class, using CPrinterDevice::CreateControlL(), 
the object can be accessed via the iControl member of CPrinterDevice.

@see CPrinterDevice 
@publishedAll
@released
*/
class CPrinterControl : public CBase
    {
public:
	/** Flags indicating whether there is more on the page to print. */
	enum TMoreOnPage
		{
		/** Indicates there is more to print on the current page. */
		EMoreOnPage,
		/** Indicates there is no more to print on the current page. */
		ENoMoreOnPage
		};
public:
	IMPORT_C ~CPrinterControl();

	/** Gets the number of bands per page.
 
 	@return The number of bands on each page. */
	virtual TInt BandsPerPage()=0;

	/**  Prints the next band on the page.
 
 	This is an asynchronous function.
 	@param  aStatus Request status object. On successful completion
    contains KErrNone, otherwise another of the system-wide 
    error codes.
 	@param aBand  On return, indicates the attributes of the band, including 
 	its height and width and whether or not the device will ignore one or 
 	other of graphics or text output. This information may be used by 
 	applications to draw more efficiently, or may be ignored
 	@return EMoreOnPage, if any unprinted bands remain on the 
 	current page. ENoMoreOnPage, if the current band is last on 
 	page. */
	virtual TMoreOnPage QueueGetBand(TRequestStatus& aStatus, TBandAttributes& aBand)=0; // returns affirmative if more to print on current page

	/** Terminates the print process.

	This is an asynchronous function and is called when no more bands in
	the document remain to be printed.Note that all bands have been
	printed when no more pages or copies remain to be printed, and
	QueueGetBand() returns ENoMoreToPrint.

	@param aStatus Request status object. On successful completion contains 
	KErrNone, otherwise another of the system-wide error codes. */
	virtual void QueueEndPrint(TRequestStatus& aStatus)=0; 

	/** Aborts the print process before it has completed.
	
	This function should be called if QueueGetBand() reports an error. */
	virtual void AbortPrint()=0; // tidy up synchronously in a short time, return success code
protected:
	IMPORT_C CPrinterControl(CPrinterPort* aPrinterPort);
protected:
	/** State flags. */
	enum TState 
		{
		/** Not printing. */
		ENotPrinting, 
		/** Printing. */
		EPrinting
		};
	/** Printing state. */
	TState iState;
	/** Printer port. */
	CPrinterPort* iPrinterPort;
	};

class CDictionaryStore;
class RFs;

/** Printer graphics device interface.

This abstract class represents a physical graphics device that is used for 
printing.

This class is used to:

set and get the page specification

map between the co-ordinates of the printed page (in twips) and the co-ordinates 
of the image device (in pixels)

get and set the printer model entry

create and delete a printer control.

A printer driver is defined in terms of a printer device and a printer control. 
A printer device can own either a single or no printer control. The control 
determines the progress and termination of the print job and is responsible 
for producing output.

@see CPrinterControl 
@publishedAll
@released
*/
class CPrinterDevice : public CGraphicsDevice
	{
public:
	IMPORT_C ~CPrinterDevice();

	/** Gets the current page specification in twips.
	
	@return The current page specification, in twips. */
	inline TPageSpec CurrentPageSpecInTwips() const {return(iCurrentPageSpecInTwips);}

	/** Sets the page specification in twips.
	
	@param aPageSpec The page specification in twips. */
	IMPORT_C virtual void SelectPageSpecInTwips(const TPageSpec& aPageSpec);
	IMPORT_C virtual TRect PrintablePageInPixels() const;

	/** Gets the printer model entry.
	
	@return The printer model entry. */
	virtual TPrinterModelEntry Model()const =0;

	/** Sets the printer model header and the store.
	
	@param aModel The printer model header.
	@param aStore  The store.
	@return KErrNone if successful; otherwise, another of the system-wide 
	error codes.*/
	virtual TInt SetModel(const TPrinterModelHeader& aModel,CStreamStore& aStore)=0;

	/** Creates a printer control.
	
	The printer control is an instance of a CPrinterControl derived class; it 
	is assigned to this printer device's iControl member.
	
	Note that this function is called by CPrintSetup::StartPrintL().
	
	@param aPrinterPort Pointer to an instance of a printer port. */
	virtual void CreateControlL(CPrinterPort* aPrinterPort)=0;
	IMPORT_C virtual void DeleteControl();

	/**  Externalizes printer properties to the store.

	The default implementation is empty.

	@param  aStream  The read stream. */
	virtual void InternalizePropertiesL(RReadStream& /*aStream*/) {}

	/**  Externalizes printer properties to the store.
	
	The default implementation is empty.
	
	@param  aStream  The write stream. */
	virtual void ExternalizePropertiesL(RWriteStream& /*aStream*/) const {}
	IMPORT_C void RestorePropertiesL();
	IMPORT_C void StorePropertiesL() const;
protected:
	IMPORT_C CPrinterDevice();
public:
	/** The printer control.
	
	This may be NULL. If implemented, it provides control over the print 
	operation. */
	CPrinterControl* iControl;
protected:
	/** Current page specification in twips. */
	TPageSpec iCurrentPageSpecInTwips;
	};


/** Printer model list interface.

Functions provided by this abstract base class can be used to extract printer 
model information from the list of printer models. This class would typically 
be used to implement printer selection in a dialog box.

@see CPdrModelList 
@publishedAll
@released
*/
class CPrinterModelList : public CBase
     {
public:
	/** Gets the number of printer models in the printer model list.
 
 	@return The number of printer models. */
    virtual TInt ModelCount() const=0;

	/**Gets printer model name.
 
	This is the name of the printer model at the specified index within
	the list of printer models.
 
	@param  anIndex The index of the printer model within the array of
    printer models. Note that this number must be between zero and 
    ModelCount().
    @return Name of printer model, up to 32 characters long */
    virtual const TPrinterModelEntry operator[](TInt anIndex)=0;

	/** Gets a printer models index within the model list from its UID.

	@param aModelUid The UID of the printer model.
	@return  The index of the printer model within the array of printer models.*/
	virtual TInt UidToNum(TUid aModelUid) const=0;
    };


/** Interface for printing in bands.

This class provides a single PrintBandL() function that prints 
the body of each page band by band. Classes derived from this interface must 
provide an implementation of the PrintBandL() function. 
@since 5.0 
@publishedAll
@released
*/
class MPageRegionPrinter
	{
public:
	/** Prints a band.
	
	Implementations should set up a graphics context to which all drawing should 
	take place using CGraphicsDevice::CreateContext(). All co-ordinate 
	calculations should be done in twips, and converted into pixels before 
	starting the print job.
	
	@param aDevice Pointer to the graphics device representing the printer.
	@param aPageNo The number of the page containing the band to be printed.
	@param aBandInPixels Attributes of the band to be printed. */
	virtual void PrintBandL(CGraphicsDevice* aDevice,TInt aPageNo,const TBandAttributes& aBandInPixels)=0;
	};

/**
The UID value of a printer specification data store. 
@publishedAll
@released
*/
const TInt KPdrStoreFileUidVal=268435514;

const TInt KPdlUidVal=268450588;
const TInt KUdlUidVal=268450589;


/** Printer specific user interface.

The class is used to implement a printer specific setup dialog. A concrete 
implementation of the class is supplied in a UDL (i.e. a UI DLL).

CPrinterDriver::CreatePrinterDriverUIL() is used to construct a CPrinterDriverUI 
object for a specific printer. (This function calls the ordinal 1 exported 
function of the UDL that performs the construction of the CPrinterDriverUI 
object).

@see CPrinterDriver::CreatePrinterDriverUIL() 
@publishedAll
@released
*/
class CPrinterDriverUI : public CBase
    {
protected:
	IMPORT_C CPrinterDriverUI();
public:
	IMPORT_C virtual TBool BeforePrintL();
	IMPORT_C virtual void AfterPrintL();
	IMPORT_C virtual void SetPropertiesL();
	IMPORT_C virtual TBool CanSetProperties();

	/** Sets the printer device.
	
	@param aPrinterDevice The printer device. */
	virtual TInt SetPrinterDevice(CPrinterDevice* aPrinterDevice)=0;
	};
								   
class CFileStore;


/** Provides access to a store containing printer specification data.

Printer specification data is held as a direct file store, and usually has 
a .pdr file type.

A printer driver is implemented as two files: a PDR file containing the printer 
specification data and a PDL file containing the code to link the printer 
with the graphics printing system.

The printer specification data is generated by compiling a text file (a .pd 
file type) using the pdrtran tool. Printer specification data defines:

the name of the associated PDL

a list of one or more models supported by this driver, identified by name 
and UID

device information such as the size of the device, typeface information for 
each model, including which typefaces are supported, the heights available 
in each typeface and the width of each character in a font.

@see CPrinterDriverUI 
@publishedAll
@released
*/
class CPrinterDriver : public CBase
     {
public:
	IMPORT_C static CPrinterDriver* NewL();
	IMPORT_C ~CPrinterDriver();
	IMPORT_C void OpenPdrL(const TDesC &aName);
	IMPORT_C void Close();
	IMPORT_C TInt NumModels() const;
	IMPORT_C TPrinterModelEntry Model(TInt aNum) const;

	/** Gets the associated graphics printer device.
	
	@return The physical graphics device used for printing. */
	inline CPrinterDevice* PrinterDevice() {return iPrinterDevice;}
	IMPORT_C CPrinterDevice* CreatePrinterDeviceL(TUid aModelUid);
	IMPORT_C CPrinterDriverUI* CreatePrinterDriverUIL();
private:
	CPrinterDriver();
	void DeletePrinterDevice();
	void DoOpenPdrL(const TDesC &aName);
	void DoCreatePrinterDeviceL(TUid aModelUid);
	void LoadLibraryL(RLibrary& aLibrary,const TDesC& aExt,TUid aUid2);
private:
	RFs iFs;
	CFileStore *iPdrStore;
	TInt iNumModels;
	TPrinterModelHeader* iModelList;
	TFileName iPdlName;
	TUid iPdlUid;
	RLibrary iPdlLibrary;
	CPrinterDevice* iPrinterDevice;
	RLibrary iUdlLibrary;
	};

class RResourceFile;


/** Implements a printer model list interface for a collection of PDR files. 
@publishedAll
@released
*/
class CPdrModelList : public CPrinterModelList
    {
public:
	IMPORT_C static CPdrModelList* NewL();
	IMPORT_C virtual ~CPdrModelList();
    IMPORT_C TInt ModelCount() const;
    IMPORT_C const TPrinterModelEntry operator [] (TInt anIndex);
	IMPORT_C TInt UidToNum(TUid aModelUid) const;  // returns KErrNotFound the model uid is not matched
	IMPORT_C void AddDirectoryL(const TDesC& aDir);
	IMPORT_C CPrinterModelList* ScanForModelsL();
	IMPORT_C CPrinterDriver* CreatePrinterDriverL(TInt anIndex);
private:
	CPdrModelList();
	void ConstructL();
private:
	class TFileEntry
		{
	public:
		TFileName iFileName;
		TDesC* iDirectory;
		};
	class TModelEntry
		{
	public:
		TPrinterModelEntry iEntry;
		TFileEntry* iFile; 
		};
private:
	void ScanDirectoryL(TInt aDirIndex);
	void ListModelsL(TInt aFileIndex, TParse& aParser, TFileName& aNameOfLoadedResourceFile, TFileName& aTempFileName, RResourceFile& aResourceFile, HBufC8*& aResource);
	HBufC* NewPathBufL(const TFileEntry& aFileEntry);
private:
	CArrayFixSeg<TModelEntry>* iModelArray;
	CArrayFixFlat<TFileEntry>* iFileArray;
	CArrayFixFlat<HBufC*>* iDirectoryArray;
	RFs iFileServer;
	};

 
/** The interface for mapping between twips and device-specific units enriched 
with facilities to allow zooming. 

The class is recursive, because a TZoomFactor object can have a 
MGraphicsDeviceMap (which could itself be a TZoomFactor) in its member data. 
This allows a zoom factor object to contain another zoom factor object, and 
is used to allow objects with different zoom factors to be embedded in each 
other to an arbitrary depth by the application architecture. 
@publishedAll
@released
*/
class TZoomFactor : public MGraphicsDeviceMap
	{
public:
	IMPORT_C TZoomFactor();
	IMPORT_C ~TZoomFactor();
	inline TZoomFactor(const MGraphicsDeviceMap* aDevice);
	inline TZoomFactor(const TZoomFactor* aDevice);
	IMPORT_C TInt ZoomFactor() const;
	IMPORT_C void SetZoomFactor(TInt aZoomFactor);
	inline void SetGraphicsDeviceMap(const MGraphicsDeviceMap* aDevice);
	inline const MGraphicsDeviceMap* GraphicsDeviceMap() const;
	IMPORT_C void SetTwipToPixelMapping(const TSize& aSizeInPixels,const TSize& aSizeInTwips);
	IMPORT_C TInt HorizontalTwipsToPixels(TInt aTwipWidth) const;
	IMPORT_C TInt VerticalTwipsToPixels(TInt aTwipHeight) const;
	IMPORT_C TInt HorizontalPixelsToTwips(TInt aPixelWidth) const;
	IMPORT_C TInt VerticalPixelsToTwips(TInt aPixelHeight) const;
	IMPORT_C TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	IMPORT_C void ReleaseFont(CFont* aFont);
public:

	/**  One to one zoom factor.  */
	enum {EZoomOneToOne=1000};
private:
	TInt iZoomFactor;
	const MGraphicsDeviceMap* iDevice;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS	
#include <graphics/gdi/gdistructs.h>
#include <graphics/gdi/gdiinline.inl>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS	

#include <gdi.inl>

#endif // __GDI_H__
