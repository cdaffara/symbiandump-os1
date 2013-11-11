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
// TRgb
// 
//


inline TRgb::TRgb():
	iValue(0xffffffff)
	/**  Constructs a TRgb initialised to KRgbWhite.*/
	{}


inline TRgb::TRgb(TUint32 aValue):
	iValue(((aValue & 0xff0000) >> 16) | (aValue & 0x00ff00) | ((aValue & 0x0000ff) << 16) | (0xff000000 - (aValue & 0xff000000)))
/** Constructs a TRgb directly from a single 32-bit integer.

The integer is of the form 0xaabbggrr, where bb is the hex number for blue,
gg is the hex number for green, rr is the hex number for red, and aa is the hex number for
"transparency" alpha (0 means opaque, 255 means transparent).

This constructor is deprecated. The byte order of Red ,Green and Blue
does not match other constructors and methods in this class,
and the meaning of alpha is reversed compared to current convention.

For example, TRgb(0x00080402) using this constructor
can be replaced with the 3 colour constructor TRgb(2,4,8).
The equivalent explicit alpha constructors are TRgb(0x020408,0xff) and TRgb(2,4,8,255).
The equivalent call to SetInternal is SetInternal(0xff020408).

This constructor is deprecated. Use other constructors or SetInternal() instead.

@param aValue Integer representing colour value. Takes form 0x00bbggrr.
@deprecated  */
	{}

inline TRgb::TRgb(TUint32 aInternalValue, TInt aAlpha) :
	iValue((aInternalValue & 0x00ffffff) | (aAlpha << 24))
/** Constructs a TRgb from a 32-bit integer (which corresponds to a colour) and from an alpha value.

The first parameter is of the form 0x00rrggbb, where rr is the hex number for red,
gg is the hex number for green, and bb is the hex number for blue.

The second parameter corresponds to an alpha channel (0 means transparent, 255 means opaque).

For example, TRgb(2,4,8,255) using the 3 colour+alpha constructor is equal to TRgb(0x00020408, 255) using
this constructor.

This constructor, which implements alpha in the conventional way,
replaces TRgb( TUint32 aValue ) which is deprecated.

@param aInternalValue	Integer representing a colour value, which takes the form 0x00rrggbb.
@param aAlpha		Alpha component of the colour (0 - 255).*/
	{
	}

inline TRgb::TRgb(TInt aRed,TInt aGreen,TInt aBlue):
	iValue(aRed<<16|aGreen<<8|aBlue|0xff000000)
/** Constructs a TRgb from its three component colours.

Each colour has a value between 0 and 255.
The alpha component is always set to opaque (255)

@param aRed Red component of the colour (0 - 255).
@param aGreen Green component of the colour (0 -  255).
@param aBlue Blue component of the colour (0 - 255). */
	{}

/** Constructs a TRgb from its three colour components and alpha component.

Each component has a value between 0 and 255.
The fourth parameter corresponds to an alpha channel (0 means transparent, 255 means opaque).

This constructor, which implements alpha in the conventional way,
replaces TRgb( TUint32 aValue ) which is deprecated.

@param aRed Red component of the colour (0 - 255). 
@param aGreen Green component of the colour (0 -  255). 
@param aBlue Blue component of the colour (0 - 255).
@param aAlpha Alpha component of the colour (0 - 255).*/
inline TRgb::TRgb(TInt aRed,TInt aGreen,TInt aBlue, TInt aAlpha):
	iValue(aRed<<16|aGreen<<8|aBlue|aAlpha<<24)
	{}


inline TInt TRgb::Red() const
/** Gets the red component.

@return The red component (0 - 255). */
	{return((iValue&0xff0000)>>16);}


inline TInt TRgb::Green() const
/** Gets the green component.

@return The green component (0 - 255). */
	{return((iValue&0xff00)>>8);}


inline TInt TRgb::Blue() const
/** Gets the blue component.

@return The blue component (0 - 255). */
	{return(iValue&0xff);}

 
inline TBool TRgb::operator==(const TRgb& aColor) const
/** Compares this colour with the specified colour for equality.

For two colours to be equal, their red, green and blue components must all 
be equal.

@param aColor Colour to be compared. 
@return ETrue. if the colours are equal; EFalse, otherwise. */
 
	{return(iValue==aColor.iValue);}

 
inline TBool TRgb::operator!=(const TRgb& aColor) const
/** Compares this colour with the specified colour for inequality.

Two colours are unequal if one at least one of their red, green and blue components 
are unequal.

@param aColor Colour to be compared. 
@return ETrue, if the colours are not equal; EFalse, otherwise.
 */
	{return(!(*this==aColor));}
 
inline TRgb& TRgb::operator&=(const TRgb& aColor)
/** Logical AND assignment operator.

The operator ANDs the first operand with the second and then assigns the result 
back to the first operand.

Note:

This operates in the TRgb domain. Graphics defines logical operations for 
drawing primitives which operate in the device colour domain.

@param aColor Colour to be compared. 
@return First operand-contains result of logical AND. */
	{iValue&=aColor.iValue;return(*this);}

inline TRgb& TRgb::operator|=(const TRgb& aColor)
/** Logical OR assignment operator.

The operator ORs the first operand with the second and then assigns the result 
back to the first operand.

Note:

This operates in the TRgb domain. Graphics defines logical operations for 
drawing primitives which operate in the device colour domain.

@param aColor Colour to be compared. 
@return First operand- contains result of logical OR. */
	{iValue|=aColor.iValue;return(*this);}

 
inline TRgb& TRgb::operator^=(const TRgb& aColor)
/** Logical EXCLUSIVE OR assignment operator.

The operator Exclusive ORs the first operand with the second and then assigns 
the result back to the first operand.

Note:

This operates in the TRgb domain. Graphics defines logical operations for 
drawing primitives which operate in the device colour domain.

@param aColor Colour to be compared. 
@return First operand  contains result of logical Exclusive OR. */
	{iValue^=aColor.iValue;iValue^=0xff000000; return(*this);}


inline TUint32 TRgb::Value() const
/** Gets the 32-bit value of the TRgb as an integer.
This function is deprecated. Use Internal() instead.
Note: the order of Red, Green and Blue components returned by this method
is reversed compared to all other methods. The alpha value is also reversed in meaning
compared to current convention, such that 0 represents opaque and 0xff represents transparent.

@return The 32 bit value of the TRgb. Has the form 0xaabbggrr, where bb is the hex number for blue,
gg is the hex number for green, rr is the hex number for red, and aa is the hex number for
"transparency" alpha (0 means opaque, 255 means transparent).
@deprecated */
		//					rr							gg						bb								aa
		{return (((iValue & 0xff0000) >> 16) | (iValue & 0x00ff00) | ((iValue & 0x0000ff) << 16) | (0xff000000 - (iValue & 0xff000000)));}

inline TUint32 TRgb::Internal() const
/** Gets the 32-bit value of the TRgb as an integer.

@return The 32 bit value of the TRgb. Has the form 0xaarrggbb. */
		{return (iValue);}

inline void TRgb::SetInternal(TUint32 aInternal)
/** Sets the 32-bit value of the TRgb as a 32-bit integer.
@param aInternal Colour internal representation. Has the form 0xaarrggbb.
*/
		{iValue = aInternal;}

inline TRgb TRgb::operator~() const
/** Bitwise logical inversion operator.

@return Contains results of logical inversion. */
	{TRgb rgb; rgb.SetInternal(iValue^0x00ffffff); return rgb;}

 
inline TRgb TRgb::operator&(const TRgb& aColor)
/** Bitwise logical AND operator.

@param aColor Colour to be compared. 
@return Contains results of logical AND. */
	{TRgb rgb; rgb.SetInternal(iValue&aColor.iValue); return rgb;}

 
inline TRgb TRgb::operator|(const TRgb& aColor)
/** Bitwise logical OR operator.

@param aColor Colour to be compared. 
@return Contains results of logical OR. */
	{TRgb rgb; rgb.SetInternal(iValue|aColor.iValue); return rgb;}

 
inline TRgb TRgb::operator^(const TRgb& aColor)
/** Bitwise EXCLUSIVE OR operator

@param aColor Colour to be compared. 
@return Contains results of logical EXCLUSIVE OR. */
	{TRgb rgb; rgb.SetInternal(iValue^aColor.iValue); return rgb;}


/** Gets TRgb from 2 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level. 

@param aGray2 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray2 has only 2 levels (black and white), 
the function returns r=g=b=0 or r=g=b=255. */
inline TRgb TRgb::_Gray2(TInt aGray2)
	{
	if(aGray2) return(TRgb(0xffffff, 0xff));
	return(TRgb(0, 0xff));
	}
 
/** Gets TRgb from 4 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level.

@param aGray4 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray4 has 4 levels-  the function returns 
r=g=b=85*c, where c=0,1,2, or 3. */
inline TRgb TRgb::_Gray4(TInt aGray4)
	{
	aGray4&=3;
	aGray4|=aGray4<<2;
	aGray4|=aGray4<<4;
	return(TRgb(aGray4,aGray4,aGray4));
	}
 
/** Gets TRgb from 16 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level.

@param aGray16 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray16 has 16 levels - the function returns 
r=g=b=17*c, where c=0, 1, ... 15. */
inline TRgb TRgb::_Gray16(TInt aGray16)
	{
	aGray16&=0xf;
	aGray16|=aGray16<<4;
	return(TRgb(aGray16,aGray16,aGray16));
	}

/** Gets TRgb from 256 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level.

@param aGray256 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray256 has 256 levels- the function 
returns r=g=b=c, where c=0, 1, ... 255. */
inline TRgb TRgb::_Gray256(TInt aGray256)
	{
	aGray256&=0xff;
	return(TRgb(aGray256,aGray256,aGray256));
	}

/** Gets TRgb from 4K colour index.

The function takes a 12 bit index into a colour palette and returns a TRgb 
whose red, green and blue values are set to an appropriate level.

@param aColor4K 12 bit index into a colour palette 
@return Equivalent 24 bit colour. */
inline TRgb TRgb::_Color4K(TInt aColor4K)
	{
	TUint32 value = (aColor4K & 0xf00) << 8;
	value |= (aColor4K & 0x0f0) << 4;
	value |= (aColor4K & 0x00f);
	return TRgb(value | (value << 4), 0xff);
	}

/** Gets TRgb from 64K colour index.

The function takes a 16 bit index into a colour palette and returns a TRgb 
whose red, green and blue values are set to an appropriate level.

@param aColor64K 16 bit index into a colour palette 
@return Equivalent 24 bit colour. */
inline TRgb TRgb::_Color64K(TInt aColor64K)
	{
	TInt red = (aColor64K&0xF800)>>8;
	red += red>>5;
	TInt green = (aColor64K&0x07E0)>>3;
	green += green>>6;
	TInt blue = (aColor64K&0x001F)<<3;
	blue += blue>>5;
	return TRgb(red,green,blue);
	}

/** Gets TRgb from 16M colour index.

The function takes a 24 bit index into a colour palette and returns the TRgb 
whose red, green and blue values represent it exactly.

@param a0RGB 24 bit index into a colour palette 
@return The TRgb which represents the index exactly. */
inline TRgb TRgb::_Color16M(TInt a0RGB)
	{
	return TRgb(a0RGB, 0xff);
	}

/** Gets TRgb from 16MU colour index.
The function takes a 24 bit colour value with eight bits for each
component, blue in the low byte, and returns the TRgb
whose red, green and blue values represent it exactly.
@param     a0RGB The color - 0, R, G, B bytes. / BGR0 - little endian format /
@return    The TRgb which represents the index exactly. */
inline TRgb TRgb::_Color16MU(TInt a0RGB)
	{
	return TRgb(a0RGB, 0xff);
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return    The index (0 - 1) representing the nearest TRgb. */	
inline TInt TRgb::_Gray2() const
	{
	return(Gray256()>>7);
	}

/**Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return     The index (0 - 3) representing the nearest TRgb. */	
inline TInt TRgb::_Gray4() const
	{
	return(Gray256()>>6);
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return     The index (0 - 15) representing the nearest TRgb.*/	
inline TInt TRgb::_Gray16() const
	{
	return(Gray256()>>4);
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return     The index (0 - 255) representing the nearest TRgb.*/	
inline TInt TRgb::_Gray256() const
	{
	return(((Red()<<1)+Green()+(Green()<<2)+Blue())>>3);
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return The index (0 - 4095) representing the nearest TRgb. */	
inline TInt TRgb::_Color4K() const
	{
	TInt color4K = (iValue & 0x0000f0) >> 4;
	color4K |= (iValue & 0x00f000) >> 8;
	color4K |= (iValue & 0xf00000) >> 12;
	return color4K;
	} //0RGB

/**  Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return The index (0 - 65535) representing the nearest TRgb.*/	
inline TInt TRgb::_Color64K() const
	{
	TInt color64K = (iValue & 0x0000f8) >> 3;
	color64K |= (iValue & 0x00fc00) >> 5;
	color64K |= (iValue & 0xf80000) >> 8;
	return color64K;
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return The index (0 - 16777215) representing the nearest TRgb.*/	
inline TInt TRgb::_Color16M() const
	{
	return (iValue & 0xffffff);
	// 0RGB
	}

/** Gets the index of the closest TRgb value to this, based on the matching display mode.
@return   The index (0 - 16777215) representing the nearest TRgb. */
inline TInt TRgb::_Color16MU() const
	{
	return (iValue & 0xffffff);
	// 0RGB
	}


/** Gets the alpha component.
@return The alpha component (0 - 255). */
inline TInt TRgb::Alpha() const
	{return (iValue >> 24);}


/** Gets TRgb from 16MA colour index.
The function takes a 32 bit colour value with eight bits for each
component, blue in the low byte, and returns the TRgb
whose red, green, blue and alpha values represent it exactly.
@param     aARGB The color - A, R, G, B bytes. / BGR0 - little endian format /
@return    The TRgb which represents the index exactly. */
inline TRgb TRgb::_Color16MA(TUint aARGB)
	{
	TRgb col; col.SetInternal(aARGB); 
	return col;
	}

/** Gets the index of the closest TRgb value to this, based on the matching display mode.
@return   The index (0 - 16777215) representing the nearest TRgb. */
inline TUint TRgb::_Color16MA() const
	{
	return (iValue);
	// ARGB
	}
//
// CPalette
//

 
inline TInt CPalette::Entries() const
/** Gets the number of entries in the palette

@return The number of entries in the palette. */
	{return(iNumEntries);}

//
// TColor256Utils
//

 
inline TRgb TColor256Util::Color256(TInt aColor256) const
/** Gets the TRgb value of the entry at the specified index in the colour lookup 
table.

@param aColor256 The index into the colour lookup table.
@return The TRgb value of the entry at the specified index. */
	{ return TRgb(iColorTable[aColor256]); }

//
// TFontStyle
//
inline TGlyphBitmapType TFontStyle::BitmapType() const
/** Gets the anti-aliasing setting for the font, as set by SetBitmapType().

@return Indicates whether or not this font should be drawn using anti-aliasing. */
	{
	return (TGlyphBitmapType)(iFlags >> 16);
	}

 
inline void TFontStyle::SetBitmapType(TGlyphBitmapType aBitmapType)
/** Sets whether the font should be drawn using anti-aliasing. If set, this value 
overrides the default setting (set by CFbsTypefaceStore::SetDefaultBitmapType()) 
for this font.

Anti-aliasing can only be used for scalable fonts. There is currently no anti-aliasing 
support for bitmapped fonts.

@param aBitmapType Indicates whether or not this font should be drawn using 
anti-aliasing. */
	{
	iFlags &= 0xFFFF;
	iFlags |= (aBitmapType << 16);
	}

//
// CBitmapDevice
//

 
inline TInt CBitmapDevice::CreateBitmapContext(CBitmapContext*& aGC)
/** Creates a bitmap context for this bitmap device.

@param aGC On return, holds a pointer to the created bitmap context. 
@return KErrNone, if successful; otherwise, another of the system-wide error 
codes. */
	{return(CreateContext((CGraphicsContext*&)aGC));} // relies on CBitmapContext deriving _only_ from CGraphicsContext

//
// TPictureCapability
//

inline TPictureCapability::TPictureCapability(TScalingType aScalingType,TBool aCroppable):
	iScalingType(aScalingType),iIsCroppable(aCroppable)
/** Constructs the object setting the scaling-type and croppability properties.

@param aScalingType Whether or not the picture is scalable. 
@param aCroppable Whether or not the picture is croppable. */
	{}

//
// TZoomFactor
//

 
inline TZoomFactor::TZoomFactor(const MGraphicsDeviceMap* aDevice):
	iZoomFactor(TZoomFactor::EZoomOneToOne),
	iDevice(aDevice)
/** Constructs a zoom factor object for a specific graphics device map.

The graphics map is either directly associated with a particular graphics 
device itself, or is associated with a hierarchy of device maps whose root 
map is associated with a particular graphics device.

@param aDevice The graphics device map with which the zoom factor is associated. */
	{}

inline TZoomFactor::TZoomFactor(const TZoomFactor* aDevice):
	iDevice(aDevice)
	{ 
	iZoomFactor=aDevice->iZoomFactor; 
	}

 
inline void TZoomFactor::SetGraphicsDeviceMap(const MGraphicsDeviceMap* aDevice)
/** Sets the graphics device map for this zoom factor object.

@param aDevice The graphics device map for this TZoomFactor. */
	{iDevice=aDevice;}

 
inline const MGraphicsDeviceMap* TZoomFactor::GraphicsDeviceMap() const
/** Gets the graphics device map of this zoom factor object.

@return The graphics device map of the TZoomFactor. */
	{return(iDevice);}



/** Gets the ascent of an ANSI capital letter in the font whether or not
there are any ANSI capitals in the font.
@return The positive distance from the font baseline to the top of a
standard ANSI capital letter
@publishedAll
@released
*/
inline TInt CFont::FontCapitalAscent() const
	{
	return ExtendedFunction(KFontCapitalAscent);
	}

/** Gets the max ascent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max ascent of composite glyphs that have to be
created by a layout engine. This is also the recommended distance between
the top of a text box and the baseline of the first line of text. 
@return The positive distance from the font baseline to the top of the
highest pre-composed glyph (including accents) above the baseline
@publishedAll
@released
*/
inline TInt CFont::FontMaxAscent() const
	{
	return ExtendedFunction(KFontMaxAscent);
	}

/** Gets the descent of an ANSI descending character in the font.
Whether or not there are any ANSI descenders in the font.
@return The positive distance from the font baseline to the bottom of the
lowest ANSI descender
@publishedAll
@released
*/
inline TInt CFont::FontStandardDescent() const
	{
	return ExtendedFunction(KFontStandardDescent);
	}

/** Gets the max descent of any pre-composed glyph in the font. This will
include accents or diacritics that form part of pre-composed glyphs. It is
not guaranteed to cover the max descent of composite glyphs that have to be
created by a layout engine.
@return The positive distance from the font baseline to the bottom of the
lowest pre-composed glyph (including accents) below the baseline
@publishedAll
@released
*/
inline TInt CFont::FontMaxDescent() const
	{
	return ExtendedFunction(KFontMaxDescent);
	}

/** Gets the suggested line gap for the font. This is the recommended
baseline to baseline distance between successive lines of text in the font.
@return The positive recommended gap between successive lines
@publishedAll
@released
*/
inline TInt CFont::FontLineGap() const
	{
	return ExtendedFunction(KFontLineGap);
	}

/**
Gets the (positive) maximum height in pixels of the font.
This may differ from the design height.

@return The maximum height of the font.
@publishedAll
@released
*/
inline TInt CFont::FontMaxHeight() const
	{
	return FontMaxAscent() + FontMaxDescent();
	}

