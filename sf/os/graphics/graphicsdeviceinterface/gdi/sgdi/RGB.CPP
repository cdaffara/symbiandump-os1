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
//

#include <graphics/lookuptable.h>
#include <graphics/blendingalgorithms.h>
#include <gdi.h>
#include <palette.h>

// Class TRgb Definition

/** Sets the red component.

@param aRed Red component (0 - 255). */
EXPORT_C void TRgb::SetRed(TInt aRed)
	{
	iValue&=0xff00ffff;
	iValue|=(aRed&0xff)<<16;
	}

/** Sets the green component.

@param aGreen Green component (0 - 255). */
EXPORT_C void TRgb::SetGreen(TInt aGreen)
	{
	iValue&=0xffff00ff;
	iValue|=(aGreen&0xff)<<8;
	}

/** Sets the blue component.

@param aBlue Blue component (0 - 255). */
EXPORT_C void TRgb::SetBlue(TInt aBlue)
	{
	iValue&=0xffffff00;
	iValue|=(aBlue&0xff);
	}

/** Gets TRgb from 2 level grayscale.

The function takes a grayscale argument and return a TRgb whose red, green
and blue values are set to an appropriate level.

@param aGray2 Grayscale value to be converted.
@return Equivalent 24 bit colour. Gray2 has only 2 levels (black and white),
the function returns r=g=b=0 or r=g=b=255. */
EXPORT_C TRgb TRgb::Gray2(TInt aGray2)
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
EXPORT_C TRgb TRgb::Gray4(TInt aGray4)
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
EXPORT_C TRgb TRgb::Gray16(TInt aGray16)
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
EXPORT_C TRgb TRgb::Gray256(TInt aGray256)
	{
    aGray256&=0xff;
    return(TRgb(aGray256,aGray256,aGray256));
	}

/** Gets TRgb from 4 bit colour index.

The function takes a 4 bit index into a colour palette and returns a TRgb
whose red, green and blue values are set to an appropriate level.

@param aColor16 4 bit index into a colour palette
@return Equivalent 24 bit colour. */
EXPORT_C TRgb TRgb::Color16(TInt aColor16)
	{
	return(TRgb(DynamicPalette::Color16array()[aColor16&0xf]));
	}

/** Gets TRgb from 8 bit colour index.

The function takes an 8 bit index into a colour palette and returns a TRgb
whose red, green and blue values are set to an appropriate level.

@param aColor256 8 bit index into a colour palette.
@return Equivalent 24 bit colour. */
EXPORT_C TRgb TRgb::Color256(TInt aColor256)
	{
	return(TRgb(DynamicPalette::DefaultColor256Util()->iColorTable[aColor256&0xff]));
	}

/** Gets TRgb from 4K colour index.

The function takes a 12 bit index into a colour palette and returns a TRgb
whose red, green and blue values are set to an appropriate level.

@param aColor4K 12 bit index into a colour palette
@return Equivalent 24 bit colour. */
EXPORT_C TRgb TRgb::Color4K(TInt aColor4K)
	{
	return _Color4K(aColor4K);
	}

/** Gets TRgb from 64K colour index.

The function takes a 16 bit index into a colour palette and returns a TRgb
whose red, green and blue values are set to an appropriate level.

@param aColor64K 16 bit index into a colour palette
@return Equivalent 24 bit colour. */
EXPORT_C TRgb TRgb::Color64K(TInt aColor64K)
	{
	return _Color64K(aColor64K);
	}

/** Gets TRgb from 16M colour index.

The function takes a 24 bit index into a colour palette and returns the TRgb
whose red, green and blue values represent it exactly.

@param aColor16M 24 bit index into a colour palette
@return The TRgb which represents the index exactly. */
EXPORT_C TRgb TRgb::Color16M(TInt aColor16M)
	{
	return _Color16M(aColor16M);
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return    The index (0 - 1) representing the nearest TRgb. */
EXPORT_C TInt TRgb::Gray2() const
	{
	return _Gray2();
	}

/**Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return     The index (0 - 3) representing the nearest TRgb. */
EXPORT_C TInt TRgb::Gray4() const
	{
	return _Gray4();
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return     The index (0 - 15) representing the nearest TRgb.*/
EXPORT_C TInt TRgb::Gray16() const
	{
	return _Gray16();
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return     The index (0 - 255) representing the nearest TRgb.*/
EXPORT_C TInt TRgb::Gray256() const
	{
	return _Gray256();
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return  The index (0 - 15) representing the nearest TRgb. */
EXPORT_C TInt TRgb::Color16() const
	{
	TInt index = (iValue & 0x000000e0) << 1;
	index |= (iValue & 0x0000e000) >> 10;
	index |= (iValue & 0x00e00000) >> 21;
	return DynamicPalette::Color16inverse()[index];
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return    The index (0 - 255) representing the nearest TRgb. */
EXPORT_C TInt TRgb::Color256() const
	{
	TInt index = (iValue & 0x000000f0) << 4;
	index |= (iValue & 0x0000f000) >> 8;
	index |= (iValue & 0x00f00000) >> 20;
	return DynamicPalette::DefaultColor256Util()->iInverseColorTable[index];
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return The index (0 - 4095) representing the nearest TRgb. */
EXPORT_C TInt TRgb::Color4K() const
	{
	return _Color4K();
	}

/**  Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return The index (0 - 65535) representing the nearest TRgb.*/
EXPORT_C TInt TRgb::Color64K() const
	{
	return _Color64K();
	}

/** Gets the index of the closest TRgb value to this,
based on the matching display mode.

@return The index (0 - 16777215) representing the nearest TRgb.*/
EXPORT_C TInt TRgb::Color16M() const
	{
	return _Color16M();
	}

/** Gets the difference between two TRgbs.

This difference is defined as the sum of the absolute values of the difference
in the red, green and blue components.

@param aColor The TRgb to be compared.
@return The sum of the absolute value of the differences between the red, green
and blue components. */
EXPORT_C TInt TRgb::Difference(const TRgb& aColor) const
	{
	return(Abs((TInt)(aColor.Internal()&0xFF)-(TInt)(Internal()&0xFF))+
		(Abs((TInt)(aColor.Internal()&0xFF00)-(TInt)(Internal()&0xFF00))>>8)+
		(Abs((TInt)(aColor.Internal()&0xFF0000)-(TInt)(Internal()&0xFF0000))>>16));
	}

/** Internalises a TRgb object from a stream.

The presence of this function means that the standard templated stream operator>>()
is available to internalise objects of this class.

@param aStream Stream from which the object is internalised.
@see operator>>() */
EXPORT_C void TRgb::InternalizeL(RReadStream& aStream)
	{
	TInt red=aStream.ReadUint8L();
	TInt green=aStream.ReadUint8L();
	TInt blue=aStream.ReadUint8L();
	*this=TRgb(red,green,blue);
	}

/** Externalises a TRgb object to a stream.

The presence of this function means that the standard templated stream operator<<()
is available to externalise objects of this class.

@param aStream Stream to which the object is externalised. */
EXPORT_C void TRgb::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(Red());
	aStream.WriteUint8L(Green());
	aStream.WriteUint8L(Blue());
	}

/** Gets TRgb from 16MU colour index.
The function takes a 24 bit colour value with eight bits for each
component, blue in the low byte, and returns the TRgb
whose red, green and blue values represent it exactly.
@param     a0RGB The color - 0, R, G, B bytes. / BGR0 - little endian format /
@return    The TRgb which represents the index exactly. */
EXPORT_C TRgb TRgb::Color16MU(TInt a0RGB)
	{
	//                  R                           G                     B
	return _Color16MU(a0RGB);
	}

/** Gets the index of the closest TRgb value to this, based on the matching display mode.
@return   The index (0 - 16777215) representing the nearest TRgb. */
EXPORT_C TInt TRgb::Color16MU() const
	{
	//              R                            G                      B
	return _Color16MU();
	}


/** Sets the alpha component.

@param aAlpha Alpha component (0 - 255). */
EXPORT_C void TRgb::SetAlpha(TInt aAlpha)
	{
	iValue&=0x00ffffff;
	iValue|=(aAlpha << 24);
	}


/** Gets TRgb from 16MA colour index.
The function takes a 32 bit colour value with eight bits for each
component, blue in the low byte, and returns the TRgb
whose red, green, blue and alpha values represent it exactly.
@param     aARGB The color - A, R, G, B bytes. / BGRA - little endian format /
@return    The TRgb which represents the index exactly. */
EXPORT_C TRgb TRgb::Color16MA(TUint aARGB)
	{
	return _Color16MA(aARGB);
	}

/** Gets the index of the closest TRgb value to this, based on the matching display mode.
@return   The index (0 - 16777215) representing the nearest TRgb. */
EXPORT_C TUint TRgb::Color16MA() const
	{
	return _Color16MA();
	}

/** Gets TRgb from 16MAP colour index.
The function takes a 32 bit colour value with eight bits for each
component, blue in the low byte, and returns the TRgb
whose red, green, and blue vales are divided by the alpha value.
@param     aARGB The pre-multiplied (EColor16MAP) color value.
@return    The TRgb which represents the color channel and alpha information. */
/* static */
EXPORT_C TRgb TRgb::Color16MAP(TUint aARGB)
	{
	const TUint16* ArTable = PtrTo16BitNormalisationTable();
	TRgb retColor;
	retColor.SetInternal(PMA2NonPMAPixel(aARGB, ArTable));
	return retColor;
	}

/** Gets the index of the closest TRgb value to this, based on the matching display mode.
pre-multiplies the alpha channels with the color channel.
@return   The pre-multiplied color value */	
EXPORT_C TUint TRgb::Color16MAP() const
	{
	return NonPMA2PMAPixel(iValue);
	}
	

/** Tests whether the display mode specified is colour or greyscale.

@param aDispMode The display mode.
@return ETrue if colour; EFalse if greyscale or monochrome. */
EXPORT_C TBool TDisplayModeUtils::IsDisplayModeColor(TDisplayMode aDispMode)
	{
	return (aDispMode >= EColor16);
	}
	
/** Tests whether the display mode specified is one of the valid values.

@param aDispMode The display mode to be tested.
@return ETrue if aDispMode is valid; EFalse if not valid. */
EXPORT_C TBool TDisplayModeUtils::IsDisplayModeValid(TDisplayMode aDispMode)
	{
	return aDispMode >= ENone && aDispMode < EColorLast;
	}

/** Gets the number of colours or shades of grey supported by the specified
display mode.

For instance, a display mode of EGray4 returns 4, EColor4K returns 4096.

@param aDispMode The display mode.
@return The number of colours/grey shades supported by the display mode. */
EXPORT_C TInt TDisplayModeUtils::NumDisplayModeColors(TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		return 2;
	case EGray4:
		return 4;
	case EGray16:
	case EColor16:
		return 16;
	case EGray256:
	case EColor256:
		return 256;
	case EColor4K:
		return 4096;
	case EColor64K:
		return 65536;
	case EColor16M:
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		return 16777216;
	default:
		return 0;
		};
	}

/** Gets the number of bits required by each pixel when displayed in the
specified display mode.

@param aDispMode The display mode.
@return The number of bits required by each pixel. */
EXPORT_C TInt TDisplayModeUtils::NumDisplayModeBitsPerPixel(TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		return 1;
	case EGray4:
		return 2;
	case EGray16:
	case EColor16:
		return 4;
	case EGray256:
	case EColor256:
		return 8;
	case EColor4K:
		return 12;
	case EColor64K:
		return 16;
	case EColor16M:
		return 24;
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		return 32;
	default:
		return 0;
		};
	}

//
// TColor256Util
//
/** Initialises the two lookup tables using the specified palette.

@param aPalette The palette of colours used to initialise the colour lookup
tables. */
EXPORT_C void TColor256Util::Construct(const CPalette& aPalette)
	{
	TInt n = aPalette.Entries();
	if(n>256)
		n = 256;

	TInt i;
	for(i=0; i<n; i++)
		iColorTable[i] = aPalette.GetEntry(i).Value();
	for(; i<256; i++)
		iColorTable[i] = 0;

	i = 0;
	for(TInt b=0; b<0x100; b+=0x11)
		for(TInt g=0; g<0x100; g+=0x11)
			for(TInt r=0; r<0x100; r+=0x11)
				iInverseColorTable[i++] = (TUint8)aPalette.NearestIndex(TRgb(r,g,b));
	}

/** Gets the entry from the inverse colour lookup table for the colour that most
closely matches the specified TRgb value.

Entries in the inverse colour lookup table are indices into the palette that
the object was created with. Essentially, this function matches aRgb to the
index of the nearest colour in the palette.

@param aRgb The conversion colour.
@return The index of the nearest colour to aRgb in the palette. */
EXPORT_C TInt TColor256Util::Color256(TRgb aRgb) const
	{
	TInt index = (aRgb.Value() & 0x000000f0) >> 4;
	index |= (aRgb.Value() & 0x0000f000) >> 8;
	index |= (aRgb.Value() & 0x00f00000) >> 12;
	return iInverseColorTable[index];
	}

/** Gets the entries from the inverse colour lookup table for the colours that
most closely match the specified TRgb values.

@param aDestination On return, a pointer to a buffer containing the entries
from the inverse colour lookup table.
@param aSource Pointer to the first TRgb value to match.
@param aNumPixels The number of TRgb values to match. */
EXPORT_C void TColor256Util::Color256(TUint8* aDestination,const TRgb* aSource,TInt aNumPixels) const
	{
	TUint8* limit = aDestination+aNumPixels;
	while(aDestination<limit)
		{
		TInt value = (*aSource++).Value();
		TInt index = (value & 0x000000f0) >> 4;
		index |= (value & 0x0000f000) >> 8;
		index |= (value & 0x00f00000) >> 12;
		*(aDestination++) = iInverseColorTable[index];
		}
	}

/** Returns a pointer to the system default 256 colour palette.

@return Pointer to the system default 256 colour palette. */
EXPORT_C const TColor256Util* TColor256Util::Default()
	{
	return DynamicPalette::DefaultColor256Util();
	}

