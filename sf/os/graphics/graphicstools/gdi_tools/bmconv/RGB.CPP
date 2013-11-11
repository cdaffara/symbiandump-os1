// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BMCONV.H"
#include "RGB.H"

extern TRgb* color256Palette;
extern char* color256InversePalette;

TRgb* color256Palette = NULL;
char* color256InversePalette = NULL;

TRgb::TRgb()
	: iRed(255),iGreen(255),iBlue(255),iSpare(0)
/** Constructs default TRgb with all 3 colour components set to 255. */
	{}

TRgb::TRgb(long unsigned int val)
	: iRed((unsigned char)(val&0xff)),iGreen((unsigned char)((val>>8)&0xff)),iBlue((unsigned char)((val>>16)&0xff)),iSpare(0)
	{}

TRgb::TRgb(int r,int g,int b)
	: iRed((unsigned char)r),iGreen((unsigned char)g),iBlue((unsigned char)b),iSpare(0)
/** Constructs a TRgb from its three component colours.
	
Each colour has a value between 0 and 255.
	
@param aRed Red component of the colour (0 - 255). 
@param aGreen Green component of the colour (0 - 255). 
@param aBlue Blue component of the colour (0 - 255). */
	{}

TRgb &TRgb::operator=(const TRgb &col)
	{
	iRed=col.iRed;
	iGreen=col.iGreen;
	iBlue=col.iBlue;
	return(*this);
	}

int TRgb::operator==(const TRgb &col)
	{
	return(iRed==col.iRed && iGreen==col.iGreen && iBlue==col.iBlue);
	}

int TRgb::Difference(const TRgb& col) const
	{
	return abs(iRed-col.iRed) + abs(iGreen-col.iGreen) + abs(iBlue-col.iBlue);
	}

int TRgb::Gray2() const
/** Gets a 2 level grayscale value from this TRgb. 

@return Equivalent 2 level grayscale value. The greyscale value is 0 or 1, 
and is calculated using c=(2*r+5*g+b)/1024. Note that the return value is 
rounded down to the nearest integer. */
	{
	return Gray256() / 128;
	}

int TRgb::Gray4() const
/** Gets a 4 level grayscale value from this TRgb.

@return Equivalent 4 level grayscale value. The greyscale value is calculated 
using c=(2*r+5*g+b)/512. Note that the return value is rounded down to the 
nearest integer. */
	{
	return Gray256() / 64;
	}

int TRgb::Gray16() const
/** Gets a 16 level grayscale value from this TRgb.

@return The greyscale value is calculated using c=(2*r+5*g+b)/128. Note that 
the return value is rounded down to the nearest integer. */
	{
	return Gray256() / 16;
	}

int TRgb::Gray256() const
/** Gets a 256 level grayscale value from this TRgb.

@return The greyscale value is calculated using c=(2*r+5*g+b)/8. Note that 
the return value is rounded down to the nearest integer. */
	{
	return((2*iRed+5*iGreen+iBlue)/8);
	}

int TRgb::Color16() const
/** Gets a 4 bit index into a colour palette from this TRgb.

@return The EGA low colour constant closest to the TRgb. */
	{
	int index = (iRed >> 5) & 0x007;
	index |= (iGreen  >> 2) & 0x038;
	index |= (iBlue << 1) & 0x1c0;
	return color16inverse[index];
	}

int TRgb::Color256() const
/** Gets an 8 bit index into a colour palette from this TRgb.

@return An 8 bit index. */
	{
	int index = (iRed >> 4) & 0x00f;
	index |= iGreen & 0x0f0;
	index |= (iBlue << 4) & 0xf00;

	if (color256InversePalette)
		return color256InversePalette[index];
	else
		return color256inverse[index];
	}

int TRgb::Color4K() const
/** Gets a 12 bit index into a colour palette from this TRgb.

@return A 12 bit index. */
	{
	return(((iRed&0xf0)<<4)|(iGreen&0xf0)|((iBlue&0xf0)>>4));
	}

int TRgb::Color64K() const
/** Gets a 24 bit index into a colour palette from this TRgb.

@return A 24 bit index. */
	{
	return(((iRed&0xf8)<<8)|((iGreen&0xfc)<<3)|((iBlue&0xf8)>>3));
	}

long int TRgb::Color16M() const
/** Gets a 16 bit index into a colour palette from this TRgb.

@return A 16 bit index. */
	{
	return((iRed<<16)|(iGreen<<8)|iBlue);
	}

TRgb TRgb::Gray2(int aGray2)
/** Gets TRgb from 2 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level. 

@param aGray2 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray2 has only 2 levels (black and white), -
the function returns r=g=b=0 or r=g=b=255. */
	{
	aGray2 *= 255;
	return TRgb(aGray2,aGray2,aGray2);
	}

TRgb TRgb::Gray4(int aGray4)
/** Gets TRgb from 4 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level.

@param aGray4 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray4 has 4 levels- the function returns 
r=g=b=85*c, where c=0,1,2, or 3. */
	{
	aGray4 *= 85;
	return TRgb(aGray4,aGray4,aGray4);
	}

TRgb TRgb::Gray16(int aGray16)
/** Gets TRgb from 16 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level.

@param aGray16 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray16 has 16 levels- the function returns 
r=g=b=17*c, where c=0, 1, ... 15. */
	{
	aGray16 *= 17;
	return TRgb(aGray16,aGray16,aGray16);
	}

TRgb TRgb::Gray256(int aGray256)
/** Gets TRgb from 256 level grayscale. 

The function takes a grayscale argument and return a TRgb whose red, green 
and blue values are set to an appropriate level.

@param aGray256 Grayscale value to be converted. 
@return Equivalent 24 bit colour. Gray256 has 256 levels- the function 
returns r=g=b=c, where c=0, 1, ... 255. */
	{
	return TRgb(aGray256,aGray256,aGray256);
	}

TRgb TRgb::Color16(int aColor16)
/** Gets TRgb from 4 bit colour index.

The function takes a 4 bit index into a colour palette and returns a TRgb 
whose red, green and blue values are set to an appropriate level.

@param aColor16 4 bit index into a colour palette 
@return Equivalent 24 bit colour. */
	{
	return TRgb(color16array[aColor16&0xf]);
	}

TRgb TRgb::Color256(int aColor256)
/** Gets TRgb from 8 bit colour index.

The function takes an 8 bit index into a colour palette and returns a TRgb 
whose red, green and blue values are set to an appropriate level.

@param aColor256 8 bit index into a colour palette. 
@return Equivalent 24 bit colour. */
	{
	if (color256Palette)
		return color256Palette[aColor256&0xff];
	else
		return TRgb(color256array[aColor256&0xff]);
	}

TRgb TRgb::Color4K(int aColor4K)
	{
	return TRgb(((aColor4K>>8)&0xf)*17,((aColor4K>>4)&0xf)*17,(aColor4K&0xf)*17);
	}

TRgb TRgb::Color64K(int aColor64K)
/** Gets TRgb from 64K colour index.

The function takes a 16 bit index into a colour palette and returns a TRgb 
whose red, green and blue values are set to an appropriate level.

@param aColor64K 16 bit index into a colour palette 
@return Equivalent 24 bit colour. */
	{
	return TRgb(((aColor64K>>11)&0x1f)*255/31,((aColor64K>>5)&0x3f)*255/63,(aColor64K&0x1f)*255/31);
	}

TRgb TRgb::Color16M(long int aColor16M)
/** Gets TRgb from 16M colour index.

The function takes a 24 bit index into a colour palette and returns the TRgb 
whose red, green and blue values represent it exactly.

@param aColor16M 24 bit index into a colour palette 
@return The TRgb which represents the index exactly. */
	{
	return TRgb(((aColor16M>>16)&0xff),(aColor16M>>8)&0xff,aColor16M&0xff);
	}

