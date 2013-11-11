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

#include <f32file.h>
#include <s32file.h>
#include <fbs.h>
#include <bitmap.h>
#include "UTILS.H"
#include <graphics/blendingalgorithms.h>
#include <graphics/lookuptable.h>
//#include "12to16.h"	// lookup table for 12->16 bpp conversion

#ifdef __ARMCC__
#pragma arm
#pragma O3
#pragma Otime
#endif

GLREF_C void Panic(TFbsPanic aPanic);

#define COLOR_VALUE(ScanLinePtr, XPos) (*((ScanLinePtr) + ((XPos) >> 5)) & ( 1 << ((XPos) & 0x1F)))


void CBitwiseBitmap::GetScanLineGray2(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TBool aDither,const TPoint& aDitherOffset,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,(TInt)((aBuf.MaxLength()) << 3));
	aBuf.SetLength((aLength + 7) >> 3);

	TUint8* ptr = (TUint8*)aBuf.Ptr();

	TUint8 mask = 1;
	TInt x = aPixel.iX;
	*ptr=0;

	if (aDither)
		{
		TBool oddx = aDitherOffset.iX & 1;
		TBool oddy = aDitherOffset.iY & 1;

		for(TInt count = 0;count < aLength;count++)
			{
			if (!mask)
				{
				mask = 1;
				ptr++;
				*ptr = 0;
				}
			if (HashTo1bpp(GetGrayPixelEx(x,aScanlinePtr),oddx,oddy))
				*ptr |= mask;
			mask <<= 1;
			oddx ^= 1;
			x++;
			}
		}
	else
		{
		for(TInt count = 0;count < aLength;count++)
			{
			if (!mask)
				{
				mask = 1;
				ptr++;
				*ptr = 0;
				}
			if (GetGrayPixelEx(x,aScanlinePtr) > 127)
				*ptr |= mask;
			mask <<= 1;
			x++;
			}
		}
	}

void CBitwiseBitmap::GetScanLineGray4(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TBool aDither,const TPoint& aDitherOffset,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,(TInt)((aBuf.MaxLength())<<2));
	aBuf.SetLength((aLength + 3) >> 2);

	TUint8* ptr=(TUint8*)aBuf.Ptr();

	TInt x = aPixel.iX;
	if (iSettings.CurrentDisplayMode() == EGray16 && aDither)
		{
		*ptr=0;
		TInt shift = 0;
		TUint8 col = 0;
		const TInt hasharray[4]={0,3,2,1};
		TInt index = (aDitherOffset.iX&1)+((aDitherOffset.iY&1)<<1);
		for(TInt count=0;count<aLength;count++,shift+=2)
			{
			if (shift==8)
				{
				shift = 0;
				ptr++;
				*ptr = 0;
				}
			col = TUint8(GetGrayPixelEx(x+count,aScanlinePtr) >> 4);
			TInt value=col/5;
			col%=5;
			if (col>2) col--;
			if (hasharray[index]<TInt(col))
				value++;
			value<<=shift;
			*ptr|=value;
			index^=1;
			}
		}
	else
		{
		TUint8* ptrLimit = ptr + ((aLength + 3) >> 2);
		while (ptr < ptrLimit)
			{
			TUint8 pixelGrayShade = TUint8(GetGrayPixelEx(x++,aScanlinePtr) >> 6);
			pixelGrayShade |= TUint8((GetGrayPixelEx(x++,aScanlinePtr) >> 4) & 0x0c);
			pixelGrayShade |= TUint8((GetGrayPixelEx(x++,aScanlinePtr) >> 2) & 0x30);
			pixelGrayShade |= TUint8(GetGrayPixelEx(x++,aScanlinePtr) & 0xc0);
			*ptr++ = pixelGrayShade;
			}
		}
	}

void CBitwiseBitmap::GetScanLineGray16(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength=Min(aLength,(TInt)(aBuf.MaxLength()<<1));
	aBuf.SetLength((aLength + 1) >> 1);

	TUint8* ptr = (TUint8*)aBuf.Ptr();
	TUint8* ptrLimit = ptr + aBuf.Length();
	TInt x = aPixel.iX;

	if(iHeader.iBitsPerPixel == 1)
		{
		while (ptr < ptrLimit)
			{
			TUint8 pixelGrayShade = TUint8(COLOR_VALUE(aScanlinePtr, x) ? 0x0F : 0);
			x++;
			pixelGrayShade |= TUint8(COLOR_VALUE(aScanlinePtr, x) ? 0xF0 : 0);
			x++;
			*ptr++ = pixelGrayShade;
			}
		}
	else
		{
		while (ptr < ptrLimit)
			{
			TUint8 pixelGrayShade = TUint8(GetGrayPixelEx(x++,aScanlinePtr) >> 4);
			pixelGrayShade |= GetGrayPixelEx(x++,aScanlinePtr) & 0xf0;
			*ptr++ = pixelGrayShade;
			}
		}
	}

void CBitwiseBitmap::GetScanLineGray256(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength());
	aBuf.SetLength(aLength);

	TUint8* ptr = (TUint8*)aBuf.Ptr();
	TUint8* ptrLimit = ptr + aLength;
	TInt xCoord = aPixel.iX;

	if(iHeader.iBitsPerPixel == 1)
		{
		while (ptr < ptrLimit)
			{
			*ptr++ = TUint8(COLOR_VALUE(aScanlinePtr, xCoord) ? 0xFF : 0);
			xCoord++;
			}
		}
	else
		{
		while (ptr < ptrLimit)
			*ptr++ = GetGrayPixelEx(xCoord++,aScanlinePtr);
		}
	}

void CBitwiseBitmap::GetScanLineColor16(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength=Min(aLength,(TInt)(aBuf.MaxLength()<<1));
	aBuf.SetLength((aLength + 1) >> 1);

	TUint8* ptr = (TUint8*)aBuf.Ptr();
	TUint8* ptrLimit = ptr + aBuf.Length();
	TInt x = aPixel.iX;

	if(iHeader.iBitsPerPixel == 1)
		{
		while (ptr < ptrLimit)
			{
			TUint8 pixelGrayShade = TUint8(COLOR_VALUE(aScanlinePtr, x) ? 0x0F : 0);
			x++;
			pixelGrayShade |= TUint8(COLOR_VALUE(aScanlinePtr, x) ? 0xF0 : 0);
			x++;
			*ptr++ = pixelGrayShade;
			}
		}
	else
		{
		while (ptr < ptrLimit)
			{
			TUint8 pixelGrayShade = TUint8(GetRgbPixelEx(x++,aScanlinePtr).Color16());
			pixelGrayShade |= GetRgbPixelEx(x++,aScanlinePtr).Color16() << 4;
			*ptr++ = pixelGrayShade;
			}
		}
	}

void CBitwiseBitmap::GetScanLineColor256(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength());
	aBuf.SetLength(aLength);

	TUint8* ptr = (TUint8*)aBuf.Ptr();
	TUint8* ptrLimit = ptr + aLength;
	TInt xCoord = aPixel.iX;

	if(iHeader.iBitsPerPixel == 1)
		{
		while (ptr < ptrLimit)
			{
			*ptr++ = TUint8(COLOR_VALUE(aScanlinePtr, xCoord) ? 0xFF : 0);
			xCoord++;
			}
		}
	else
		{
		while (ptr < ptrLimit)
			*ptr++ = TUint8(GetRgbPixelEx(xCoord++,aScanlinePtr).Color256());
		}
	}

void CBitwiseBitmap::GetScanLineColor4K(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength() >> 1);
	aBuf.SetLength(aLength << 1);

	TUint16* ptr = (TUint16*)aBuf.Ptr();
	const TUint16* ptrLimit = ptr + aLength;
	TInt x = aPixel.iX;

	if(iHeader.iBitsPerPixel == 1)
		{
		while (ptr < ptrLimit)
			{
			*ptr++ = TUint16(COLOR_VALUE(aScanlinePtr, x) ? 0x0FFF : 0);
			x++;
			}
		}
	else
		{
		while (ptr < ptrLimit)
			*ptr++ = TUint16(GetRgbPixelEx(x++,aScanlinePtr)._Color4K());
		}
	}

void CBitwiseBitmap::GetScanLineColor64K(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength() >> 1);
	aBuf.SetLength(aLength << 1);

	TUint16* ptr = (TUint16*)aBuf.Ptr();
	TUint16* ptrLimit = ptr + aLength;
	TInt x = aPixel.iX;

	if(iHeader.iBitsPerPixel == 1)
		{
		while (ptr < ptrLimit)
			{
			*ptr++ = TUint16(COLOR_VALUE(aScanlinePtr, x) ? 0xFFFF : 0);
			x++;
			}
		}
	else if(iHeader.iBitsPerPixel == 12)
		{
/*
		// use lookup table for 12->16 conversion
		TUint16* pixel4K = ((TUint16*) aScanlinePtr) + x;
		while(ptr < ptrLimit)
			{
			// this takes the 12 bit value, this is a number between 0 and 4095,
			// and looks up its corrosponding 16 bit value in the lookup table.
			// the colour should be identical, and but just in a different format
			// see below for an explaination of 12 & 16 bit colour values
			*ptr++ = K12to16LUT[*pixel4K++];
			}
*/
/*		This code uses logic rather than a lookup table
		to convert from 12->16 bpp and can be used instead of the above code
		if the 8k the lookup table uses becomes an issue

		12 bit colour
		-------------
		The 12 bit colour format uses 4 bits for the red, green and blue values.
		The colour is stored as a word with the most significant 4 bits having a
		value of zero.
		i.e. 0000RRRR GGGGBBBB where R,G & B represent single bits in the word.

		The code below labeled 'conversion of 4k colour...' changes the colour from
		16 bit to 32 bit where each colour nibble in the 16 bit version is changed
		to a byte in the 32 bit version e.g.
		0000RRRR GGGGBBBB -> 00000000 RRRRRRRR GGGGGGGG BBBBBBBB


		16 bit colour
		-------------
		The 16 bit colour format uses all 16 bits to represent the required colour.
		There are two possible 16 bit formats 5-5-5 and 5-6-5.
		Symbian uses the 5-6-5 format, with this all 16 bits are used to make the colour
		giving a possible value between 0..65535.  The RGB components are divided up
		as follows RRRR RGGG GGGB BBBB i.e. 5 bits for red and blue, and 6 for green.

		The code below labeled 'conversion to 64k' converts the colour from a 16 bit
		0000 RRRR GGGG BBBB -> RRRR RGGG GGGB BBBB format.
*/
		register TUint16* pixel4K = ((TUint16*) aScanlinePtr) + x;
		while (ptr < ptrLimit)
			{
			// conversion of 4k colour from 16 to 32 bits
			// this changes from a 16 to 32 bit value while keeping colour unchanged
			register TUint16 pixelVal = *pixel4K++;
			register TUint32 value32 = ((pixelVal & 0x0f00) >> 8) |
									   ((pixelVal & 0x00f0) << 4) |
									   ((pixelVal & 0x000f) << 16);
			value32 |= (value32 << 4);
			// conversion to 64k (RRRR RGGG GGGB BBBB)
			// this will make the change from (16 bit) 4-4-4 bpp to 5-6-5 bpp format
			register TUint32 color64K = ((value32 & 0x000000f8) << 8) |
										((value32 & 0x0000fc00) >> 5) |
										((value32 & 0x00f80000) >> 19);
			// store new colour value
			*ptr++ = static_cast <TUint16> (color64K);
			}
		}
	else
		{
		while (ptr < ptrLimit)
			*ptr++ = TUint16(GetRgbPixelEx(x++,aScanlinePtr)._Color64K());
		}
	}

void CBitwiseBitmap::GetScanLineColor16M(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength() / 3);
	aBuf.SetLength(aLength * 3);

	TUint8* ptr = (TUint8*)aBuf.Ptr();
	TUint8* ptrLimit = ptr + (aLength * 3);
	TInt x = aPixel.iX;

	if(iHeader.iBitsPerPixel == 1)
		{
		while (ptr < ptrLimit)
			{
			const TUint8 color = TUint8(COLOR_VALUE(aScanlinePtr, x) ? 0xFF : 0);
			*ptr++ = color;
			*ptr++ = color;
			*ptr++ = color;
			x++;
			}
		}
	else
		{
		GetRgbPixelExMany16M(aPixel.iX,aScanlinePtr,ptr,aLength);
		}
	}

void CBitwiseBitmap::GetScanLineColor16MU(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength() >> 2);
	aBuf.SetLength(aLength << 2);

	TUint32* ptr = (TUint32*)aBuf.Ptr();

	GetRgbPixelExMany(aPixel.iX,aScanlinePtr,ptr,aLength);
	}

void CBitwiseBitmap::GetScanLineColor16MA(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength() >> 2);
	aBuf.SetLength(aLength << 2);

	TUint32* ptr = (TUint32*)aBuf.Ptr();
	GetRgbPixelExMany(aPixel.iX,aScanlinePtr,ptr,aLength);
	}

/**
Get the scanline data into the destination buffer in the EColor16MAP format.
@param	aDestBuf - destination buffer
@param	aPixel - the start position of the scanline.
@param	aLength - scanline length, as word length
@param	aScanlinePtr - scanline pointer
*/
void CBitwiseBitmap::GetScanLineColor16MAP(TDes8& aDestBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength, aDestBuf.MaxLength() >> 2);
	aDestBuf.SetLength(aLength << 2);
	TUint32* ptr = (TUint32*)aDestBuf.Ptr();
	GetRgbPixelExMany16MAP(aPixel.iX,aScanlinePtr,ptr,aLength);
	}


void CBitwiseBitmap::GetScanLineColorRgb(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TUint32* aScanlinePtr) const
	{
	aLength = Min(aLength,aBuf.MaxLength() / sizeof(TRgb));
	aBuf.SetLength(aLength * sizeof(TRgb));

	TUint32* ptr = (TUint32*)aBuf.Ptr();
	GetRgbPixelExMany(aPixel.iX,aScanlinePtr,ptr,aLength);
	}

void CBitwiseBitmap::GetScanLineExBits(TDes8& aBuf,TInt aX,TInt aLength,TUint32* aScanlinePtr) const
	{
	TInt bitshift = 1;
	TInt pixelsPerWord = 8;
	TInt roundingmask = ~0x7;
	TInt logbpp = 2;
	TInt roundupfactor = 1;
	const TDisplayMode displayMode = iSettings.CurrentDisplayMode();

	switch(displayMode)
		{
		case EGray16:
		case EColor16:
			break; // set by default
		case EGray4:
			{
			bitshift = 2;
			pixelsPerWord = 16;
			roundingmask = ~0xf;
			logbpp = 1;
			roundupfactor = 3;
			break;
			}
		case EGray2:
			{
			bitshift = 3;
			pixelsPerWord = 32;
			roundingmask = ~0x1f;
			logbpp = 0;
			roundupfactor = 7;
			break;
			}
		default:
			Panic(EFbsBitmapInvalidMode);
		}

	aLength = Min(aLength,aBuf.MaxLength() << bitshift);
	aBuf.SetLength((aLength + roundupfactor) >> bitshift);

	TUint32* ptr = (TUint32*)aBuf.Ptr(); // guaranteed to be word aligned by the calling function
	TInt startlong = aX & roundingmask;
	TInt finishlong = (aX + aLength + pixelsPerWord - 1) & roundingmask;
	bitshift += 2; // Convert pixels per byte shift to pixels per word shift
	TUint32* wordptr = aScanlinePtr + (startlong >> bitshift);
	TInt wordLength = (finishlong - startlong) >> bitshift;
	TUint32* wordptrLimit = wordptr + wordLength;

	const TInt destinationWords = Min(aBuf.MaxLength() >> 2,wordLength);
	TUint32* ptrlimit = ptr + destinationWords;

	TInt offset = (aX - startlong) << logbpp;

	if (offset)
		{
		TInt offsetextra = 32-offset;
		TUint32 data = *wordptr++;
		data >>= offset;

		while (ptr < ptrlimit - 1)
			{
			TUint32 tmp = *wordptr++;
			data |= tmp << offsetextra;
			*ptr++ = data;
			data = tmp >> offset;
			}

		if (wordptr < wordptrLimit)
			*ptr = data | (*wordptr << offsetextra);
		else
			*ptr = data;
		}
	else
		{
		while (ptr < ptrlimit)
			*ptr++ = *wordptr++;

		// if the buffer isn't a whole number of words long,
		// we need to copy the remaining bytes
		const TInt bytesRemaining = aBuf.Length() - (destinationWords * sizeof(TUint32));
		if (bytesRemaining > 0)
			Mem::Copy(ptr,wordptr,bytesRemaining);

		}
	}

void CBitwiseBitmap::GetScanLineExBytes(TDes8& aBuf,TInt aX,TInt aLength,TUint32* aScanlinePtr) const
	{
	TInt numberOfBytesToCopy = 0;
	TUint8* ptr = (TUint8*)aScanlinePtr;
	TDisplayMode displayMode = iSettings.CurrentDisplayMode();
	switch(displayMode)
		{
		case EGray256:
		case EColor256:
			{
			aLength = Min(aLength,aBuf.MaxLength());
			numberOfBytesToCopy = aLength;
			ptr += aX;
			break;
			}
		case EColor4K:
		case EColor64K:
			{
			aLength = Min(aLength,aBuf.MaxLength() / 2);
			numberOfBytesToCopy = aLength * 2;
			ptr += (aX << 1);
			break;
			}
		case EColor16M:
			{
			aLength = Min(aLength,aBuf.MaxLength() / 3);
			numberOfBytesToCopy = aLength * 3;
			ptr += (aX * 3);
			break;
			}
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:
			{
			aLength = Min(aLength,aBuf.MaxLength() / 4);
			numberOfBytesToCopy = aLength * 4;
			ptr += (aX * 4);
			break;
			}
		default:
			Panic(EFbsBitmapInvalidMode);
		}

	aBuf.SetLength(numberOfBytesToCopy);

	Mem::Copy((TAny*)aBuf.Ptr(),ptr,numberOfBytesToCopy);
	}

void CBitwiseBitmap::DoStretchScanLine(TDes8& aBuf,TInt x,TInt y,TInt aClipStrchX,
	TInt aClipStrchLen,TInt aStretchLength,TInt aOrgX,TInt aOrgLen,
	const TPoint& aDitherOffset,TDisplayMode aDispMode,TUint32* aBase,
	TLineScanningPosition& aLineScanningPosition) const
	{
	TInt lastValue = 0;
	TUint32* bufptr=(TUint32*)((TInt)(&aBuf[0]+3)&~3);
	TUint32* buflimit=bufptr;
	TUint32* slptr=NULL;
	TPoint pixel(aOrgX,y);
	const TDisplayMode displayMode = iSettings.CurrentDisplayMode();
	GetScanLinePtr(slptr, aOrgLen, pixel,aBase, aLineScanningPosition);
	if (!slptr)
		{
		WhiteFill((TUint8*)aBuf.Ptr(),aBuf.MaxLength(),displayMode);
		return;
		}

	TInt lastcoord=-1;
	TLinearDDA stretchmap;
	TPoint coordmap(aOrgX,0);
	stretchmap.Construct(coordmap,coordmap+TPoint(aOrgLen,aStretchLength),TLinearDDA::ELeft);
	coordmap.iY=aClipStrchX;
	if (aClipStrchX>0) stretchmap.JumpToYCoord(coordmap.iX,coordmap.iY);
	else stretchmap.SingleStep(coordmap);
	switch(aDispMode)
		{
		case EGray4:
			{
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength()>>2)<<4));
			aBuf.SetLength((aClipStrchLen+3)>>2);
			buflimit+=(aBuf.Length()+3)>>2;
			if (displayMode == EGray16)
				{
				TInt index=((aDitherOffset.iY&1)<<1)+((aDitherOffset.iX+x)&1);
				while(bufptr<buflimit)
					{
					TInt shift=0;
					*bufptr=0;
					while(shift<32)
						{
						if (coordmap.iX>lastcoord)
							{
							lastValue=HashTo2bpp(GetGrayPixelEx(coordmap.iX,slptr),index);
							lastcoord=coordmap.iX;
							}
						*bufptr|=(lastValue<<shift);
						index^=1;
						shift+=2;
						if (stretchmap.SingleStep(coordmap)) break;
						}
					bufptr++;
					}
				}
			else
				{
				while (bufptr < buflimit)
					{
					TInt shift = 0;
					*bufptr = 0;
					while (shift < 32)
						{
						if (coordmap.iX>lastcoord)
							{
							lastValue = GetGrayPixelEx(coordmap.iX,slptr) >> 6;
							lastcoord = coordmap.iX;
							}
						*bufptr |= (lastValue << shift);
						shift += 2;
						if (stretchmap.SingleStep(coordmap)) break;
						}
					bufptr++;
					}
				}
			break;
			}
		case EGray16:
			{
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength()>>2)<<3));
			aBuf.SetLength((aClipStrchLen+1)>>1);
			buflimit+=(aBuf.Length()+3)>>2;
			while(bufptr<buflimit)
				{
				TInt shift=0;
				*bufptr=0;
				while(shift<32)
					{
					if (coordmap.iX>lastcoord)
						{
						lastValue = GetGrayPixelEx(coordmap.iX,slptr) >> 4;
						lastcoord=coordmap.iX;
						}
					*bufptr |= lastValue << shift;
					shift+=4;
					if (stretchmap.SingleStep(coordmap)) break;
					}
				bufptr++;
				}
			break;
			}
		case EColor16:
			{
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength()>>2)<<3));
			aBuf.SetLength((aClipStrchLen+1)>>1);
			buflimit+=(aBuf.Length()+3)>>2;
			while(bufptr<buflimit)
				{
				TInt shift=0;
				*bufptr=0;
				while(shift<32)
					{
					if (coordmap.iX>lastcoord)
						{
						lastValue = GetRgbPixelEx(coordmap.iX,slptr).Color16();
						lastcoord = coordmap.iX;
						}
					*bufptr |= lastValue << shift;
					shift+=4;
					if (stretchmap.SingleStep(coordmap)) break;
					}
				bufptr++;
				}
			break;
			}
		case EGray256:
			{
			aClipStrchLen = Min(aClipStrchLen,(TInt)(aBuf.MaxLength() & ~3));
			aBuf.SetLength(aClipStrchLen);
			buflimit += (aBuf.Length() + 3) >> 2;

			while (bufptr < buflimit)
				{
				TInt shift=0;
				*bufptr=0;
				while(shift<32)
					{
					if (coordmap.iX>lastcoord)
						{
						lastValue = GetGrayPixelEx(coordmap.iX,slptr);
						lastcoord=coordmap.iX;
						}
					*bufptr |= lastValue << shift;
					shift += 8;
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				bufptr++;
				}
			break;
			}
		case EColor256:
			{
			aClipStrchLen = Min(aClipStrchLen,(TInt)(aBuf.MaxLength() & ~3));
			aBuf.SetLength(aClipStrchLen);
			buflimit += (aBuf.Length() + 3) >> 2;

			while (bufptr < buflimit)
				{
				TInt shift=0;
				*bufptr=0;
				while(shift<32)
					{
					if (coordmap.iX>lastcoord)
						{
						lastValue = TUint8(GetRgbPixelEx(coordmap.iX,slptr).Color256());
						lastcoord = coordmap.iX;
						}
					*bufptr |= lastValue << shift;
					shift += 8;
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				bufptr++;
				}
			break;
			}
		case EColor4K:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 1);
			aBuf.SetLength(aClipStrchLen << 1);
			buflimit += (aBuf.Length() + 3) >> 2;

			while (bufptr < buflimit)
				{
				if (coordmap.iX>lastcoord)
					{
					lastValue = GetRgbPixelEx(coordmap.iX,slptr)._Color4K();
					lastcoord=coordmap.iX;
					}
				*bufptr = lastValue;
				if (stretchmap.SingleStep(coordmap))
					break;
				if (coordmap.iX>lastcoord)
					{
					lastValue = GetRgbPixelEx(coordmap.iX,slptr)._Color4K();
					lastcoord=coordmap.iX;
					}
				*bufptr |= lastValue << 16;
				if (stretchmap.SingleStep(coordmap))
					break;
				bufptr++;
				}
			break;
			}
		case EColor64K:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 1);
			aBuf.SetLength(aClipStrchLen << 1);
			buflimit += (aBuf.Length() + 3) >> 2;

			while (bufptr < buflimit)
				{
				if (coordmap.iX>lastcoord)
					{
					lastValue = GetRgbPixelEx(coordmap.iX,slptr)._Color64K();
					lastcoord=coordmap.iX;
					}
				*bufptr = lastValue;
				if (stretchmap.SingleStep(coordmap))
					break;
				if (coordmap.iX>lastcoord)
					{
					lastValue = GetRgbPixelEx(coordmap.iX,slptr)._Color64K();
					lastcoord=coordmap.iX;
					}
				*bufptr |= lastValue << 16;
				if (stretchmap.SingleStep(coordmap))
					break;
				bufptr++;
				}
			break;
			}
		case EColor16M: // Destination Mode
			{
			// Optimisation: Both of conditions on 32bpp and 24bpp were added to avoid to use
			// GetRgbPixelEx() for each pixel
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() / 3);
			aBuf.SetLength(aClipStrchLen * 3);
			TUint8* ptr = (TUint8*)bufptr;
			TUint8* ptrLimit = ptr + aBuf.Length();

			if (iHeader.iBitsPerPixel == 32) // 32bpp source image => color
				{
				TInt lastColor = 0;
				if(displayMode == EColor16MAP)
					{
					const TUint16* normTable = PtrTo16BitNormalisationTable();
					while (ptr < ptrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = PMA2NonPMAPixel(*(slptr + coordmap.iX), normTable);
							lastcoord = coordmap.iX;
							}
						*ptr++ = TUint8(lastColor);
						*ptr++ = TUint8(lastColor >> 8);
						*ptr++ = TUint8(lastColor >> 16);
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				else{
					while (ptr < ptrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = *(slptr + coordmap.iX);
							lastcoord = coordmap.iX;
							}
						*ptr++ = TUint8(lastColor);
						*ptr++ = TUint8(lastColor >> 8);
						*ptr++ = TUint8(lastColor >> 16);
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				}
			else if (iHeader.iBitsPerPixel == 24) //24bpp source image => color
				{
				TInt lastColor = 0;
				while (ptr < ptrLimit)
					{
					if (coordmap.iX > lastcoord)
						{
						TUint8* scanLineBytePointer = (TUint8*)slptr + coordmap.iX*3;
						lastColor = TUint8(*scanLineBytePointer);
						scanLineBytePointer++;
						lastColor |= TUint8(*scanLineBytePointer) << 8;
						scanLineBytePointer++;
						lastColor |= TUint8(*scanLineBytePointer) << 16;
						lastcoord = coordmap.iX;
						}
					*ptr++ = TUint8(lastColor);
					*ptr++ = TUint8(lastColor >> 8);
					*ptr++ = TUint8(lastColor >> 16);
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			else
				{
				TRgb lastColor;
				while (ptr < ptrLimit)
					{
					if (coordmap.iX>lastcoord)
						{
						lastColor = GetRgbPixelEx(coordmap.iX,slptr);
						lastcoord=coordmap.iX;
						}
					TInt color16M = lastColor._Color16M();
					*ptr++ = TUint8(color16M);
					*ptr++ = TUint8(color16M >> 8);
					*ptr++ = TUint8(color16M >> 16);
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			break;
			}
		case ERgb:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() / sizeof(TRgb));
			aBuf.SetLength(aClipStrchLen * sizeof(TRgb));
			TRgb* pixelPtr = (TRgb*)bufptr;
			TRgb* pixelPtrLimit = pixelPtr + aClipStrchLen;
			TRgb lastColor;

			while (pixelPtr < pixelPtrLimit)
				{
				if (coordmap.iX > lastcoord)
					{
					lastColor = GetRgbPixelEx(coordmap.iX,slptr);
					lastcoord = coordmap.iX;
					}
				*pixelPtr++ = lastColor;
				if (stretchmap.SingleStep(coordmap))
					break;
				}
			break;
			}
		case EColor16MU: // Destination Mode
			{
			// Optimisation: The condition 32bpp was added to avoid to use
			//GetRgbPixelEx() for each pixel (construction of a TRgb object each time)
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 2);
			aBuf.SetLength(aClipStrchLen << 2);
			TInt32* pixelPtr = (TInt32*)bufptr;
			TInt32* pixelPtrLimit = pixelPtr + aClipStrchLen;

			if (iHeader.iBitsPerPixel == 32) //32bpp source image => color
				{
				TInt lastColor = 0;
				if(displayMode == EColor16MAP)
					{
					const TUint16* normTable = PtrTo16BitNormalisationTable();
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = PMA2NonPMAPixel(*(slptr + coordmap.iX), normTable);
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = (lastColor | 0xff000000);//BGR0 (Blue/Green/Red) as little endian byte order
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				else{
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = *(slptr + coordmap.iX);
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = (lastColor | 0xff000000);//BGR0 (Blue/Green/Red) as little endian byte order
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				}
				
			else if (iHeader.iBitsPerPixel == 24) //24bpp source image => color
				{
				TInt lastColor = 0;
				while (pixelPtr < pixelPtrLimit)
					{
					if (coordmap.iX > lastcoord)
						{
						TUint8* scanLineBytePointer = (TUint8*)slptr + coordmap.iX*3;
						lastColor = TUint8(*scanLineBytePointer);
						scanLineBytePointer++;
						lastColor |= TUint8(*scanLineBytePointer) << 8;
						scanLineBytePointer++;
						lastColor |= TUint8(*scanLineBytePointer) << 16;
						lastcoord = coordmap.iX;
						}
					*pixelPtr++ = (lastColor | 0xff000000);
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			else if (iHeader.iBitsPerPixel == 16) //16bpp source image => color
				{
				TInt lastColor = 0;
				while (pixelPtr < pixelPtrLimit)
					{
					if (coordmap.iX > lastcoord)
						{
						TUint rawColor = *(((TUint16*)slptr) + coordmap.iX);
						TInt red = (rawColor   & 0xF800)>>8;
						red += red>>5;
						TInt green = (rawColor & 0x07E0)>>3;
						green += green>>6;
						TInt blue = (rawColor  & 0x001F)<<3;
						blue += blue>>5;
						lastColor = 0xff000000 | (red << 16) | (green << 8) | blue;
						lastcoord = coordmap.iX;
						}
					*pixelPtr++ = lastColor;
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			else
				{
				TRgb lastColor;
				while (pixelPtr < pixelPtrLimit)
					{
					if (coordmap.iX > lastcoord)
						{
						lastColor = GetRgbPixelEx(coordmap.iX,slptr);
						lastcoord = coordmap.iX;
						}
					*pixelPtr++ = (lastColor._Color16MU() | 0xff000000);//BGR0 (Blue/Green/Red) as little endian byte order
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			break;
			}
		case EColor16MA:
			{
			// Optimisation: The condition 32bpp was added to avoid to use
			// GetRgbPixelEx() for each pixel (construction of a TRgb object each time)
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 2);
			aBuf.SetLength(aClipStrchLen << 2);
			TInt32* pixelPtr = (TInt32*)bufptr;
			TInt32* pixelPtrLimit = pixelPtr + aClipStrchLen;
			const TUint16* normTable = PtrTo16BitNormalisationTable();
			if (iHeader.iBitsPerPixel == 32) //32bpp source image => color
				{
				TInt lastColor = 0;
				if(displayMode == EColor16MAP)
					{
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = PMA2NonPMAPixel(*(slptr + coordmap.iX), normTable);
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = lastColor;//BGRA (Blue/Green/Red) as little endian byte order
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				else
					{
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = *(slptr + coordmap.iX);
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = lastColor;
						if (stretchmap.SingleStep(coordmap))
								break;
						}
					}
				}
			else if (iHeader.iBitsPerPixel == 24) //24bpp source image => color
				{
				TInt lastColor = 0;
				while (pixelPtr < pixelPtrLimit)
					{
					if (coordmap.iX > lastcoord)
						{
						TUint8* scanLineBytePointer = (TUint8*)slptr + coordmap.iX*3;
						lastColor = TUint8(*scanLineBytePointer);
						scanLineBytePointer++;
						lastColor |= TUint8(*scanLineBytePointer) << 8;
						scanLineBytePointer++;
						lastColor |= TUint8(*scanLineBytePointer) << 16;
						lastcoord = coordmap.iX;
						}
					*pixelPtr++ = (lastColor | 0xff000000);
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			else if (iHeader.iBitsPerPixel == 16) //16bpp source image => color
				{
				TInt lastColor = 0;
				while (pixelPtr < pixelPtrLimit)
					{
					if (coordmap.iX > lastcoord)
						{
						TUint rawColor = *(((TUint16*)slptr) + coordmap.iX);
						TInt red = (rawColor   & 0xF800)>>8;
						red += red>>5;
						TInt green = (rawColor & 0x07E0)>>3;
						green += green>>6;
						TInt blue = (rawColor  & 0x001F)<<3;
						blue += blue>>5;
						lastColor = 0xff000000 | (red << 16) | (green << 8) | blue;
						lastcoord = coordmap.iX;
						}
					*pixelPtr++ = lastColor;
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			else
				{
				TRgb lastColor;

				while (pixelPtr < pixelPtrLimit)
					{
					if (coordmap.iX > lastcoord)
						{
						lastColor = GetRgbPixelEx(coordmap.iX,slptr);
						lastcoord = coordmap.iX;
						}
					*pixelPtr++ = lastColor._Color16MA();//BGRA (Blue/Green/Red) as little endian byte order
					if (stretchmap.SingleStep(coordmap))
						break;
					}
				}
			break;
			}
		case EColor16MAP:
			{ //if alpha is not available, assign 255 as alpha (opaque).
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 2);
			aBuf.SetLength(aClipStrchLen << 2);
			TInt32* pixelPtr = (TInt32*)bufptr;
			TInt32* pixelPtrLimit = pixelPtr + aClipStrchLen;
			if (iHeader.iBitsPerPixel == 32) //32bpp source image => color
				{
				TInt lastColor = 0;
				//pre-multiply if alpha IS available.
				if(displayMode == EColor16MA)
					{
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = NonPMA2PMAPixel(*(slptr + coordmap.iX));
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = lastColor;//BGRA (Blue/Green/Red) as little endian byte order
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				else if(displayMode == EColor16MU)
					{
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							//do not want to convert to non pma, since keep transparency
							//e.g. alpha 0.5, red 0.5.  Want to keep red as 0.5 since it
							//is not fully red.  For 16MA convert to 1, and keep 0.5 alpha
							lastColor = (*(slptr + coordmap.iX))|0xff000000;
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = lastColor;//BGRA (Blue/Green/Red) as little endian byte order
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				else	
					{
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = *(slptr + coordmap.iX);
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = lastColor;
						if (stretchmap.SingleStep(coordmap))
								break;
						}
					}
				}
				else if (iHeader.iBitsPerPixel == 24) //24bpp source image => color
					{
					TInt lastColor = 0;
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							TUint8* scanLineBytePointer = (TUint8*)slptr + coordmap.iX*3;
							lastColor = TUint8(*scanLineBytePointer);
							scanLineBytePointer++;
							lastColor |= TUint8(*scanLineBytePointer) << 8;
							scanLineBytePointer++;
							lastColor |= TUint8(*scanLineBytePointer) << 16;
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = (lastColor | 0xff000000);
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				else if (iHeader.iBitsPerPixel == 16) //16bpp source image => color
					{
					TInt lastColor = 0;
					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							TUint rawColor = *(((TUint16*)slptr) + coordmap.iX);
							TInt red = (rawColor   & 0xF800)>>8;
							red += red>>5;
							TInt green = (rawColor & 0x07E0)>>3;
							green += green>>6;
							TInt blue = (rawColor  & 0x001F)<<3;
							blue += blue>>5;
							lastColor = 0xff000000 | (red << 16) | (green << 8) | blue;
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = lastColor;
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				else
					{
					TRgb lastColor;

					while (pixelPtr < pixelPtrLimit)
						{
						if (coordmap.iX > lastcoord)
							{
							lastColor = GetRgbPixelEx(coordmap.iX,slptr);
							lastcoord = coordmap.iX;
							}
						*pixelPtr++ = lastColor._Color16MA();//BGRA (Blue/Green/Red) as little endian byte order
						if (stretchmap.SingleStep(coordmap))
							break;
						}
					}
				break;
			}
		case EGray2:
			{
			TBool oddx=(aDitherOffset.iX&1);
			TBool oddy=(aDitherOffset.iY&1);
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength()>>2)<<5));
			aBuf.SetLength((aClipStrchLen+7)>>3);
			buflimit+=(aBuf.Length()+3)>>2;
			while(bufptr<buflimit)
				{
				TUint32 mask=1;
				*bufptr=0;
				while(mask)
					{
					if (coordmap.iX > lastcoord)
						{
						lastValue = GetGrayPixelEx(coordmap.iX,slptr);
						lastcoord = coordmap.iX;
						}
					if (HashTo1bpp(lastValue,oddx,oddy))
						*bufptr|=mask;
					mask<<=1;
					oddx^=1;
					if (stretchmap.SingleStep(coordmap)) break;
					}
				bufptr++;
				}
			break;
			}
		default:
			Panic(EFbsBitmapInvalidMode);
		}
	}

void CBitwiseBitmap::DoCompressScanLine(TDes8& aBuf,TInt x,TInt y,TInt aClipStrchX,TInt aClipStrchLen,TInt aStretchLength,TInt aOrgX,TInt aOrgLen,const TPoint& aDitherOffset,TDisplayMode aDispMode,TUint32* aBase,TLineScanningPosition& aLineScanningPosition) const
	{
	TInt first=0,second=0,third=0,fourth=0,fifth=0,sixth=0,seventh=0,eighth=0;
	TUint8* bufptr=&aBuf[0];
	TUint8* buflimit=bufptr;
	TUint32* slptr=NULL;
	TPoint pixel(aOrgX,y);
	TDisplayMode displayMode = iSettings.CurrentDisplayMode();
	GetScanLinePtr(slptr,aOrgLen,pixel,aBase, aLineScanningPosition);
	if (!slptr)
		{
		WhiteFill((TUint8*)aBuf.Ptr(),aBuf.MaxLength(),displayMode);
		return;
		}
	TLinearDDA stretchmap;
	TPoint coordmap(aOrgX,0);
	stretchmap.Construct(coordmap,coordmap+TPoint(aOrgLen,aStretchLength),TLinearDDA::ELeft);
	coordmap.iY=aClipStrchX;
	if (aClipStrchX>0) stretchmap.JumpToYCoord(coordmap.iX,coordmap.iY);
	else stretchmap.NextStep(coordmap);
	switch(aDispMode)
		{
		case EGray4:
			{
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength())<<2));
			aBuf.SetLength((aClipStrchLen+3)>>2);
			TInt index=((aDitherOffset.iY&1)<<1)+((aDitherOffset.iX+x)&1);
			buflimit+=aBuf.Length();
			if (displayMode==EGray16)
				{
				while(bufptr<buflimit)
					{
					first=HashTo2bpp(GetGrayPixelEx(coordmap.iX,slptr),index);
					index^=1;
					stretchmap.NextStep(coordmap);
					second=HashTo2bpp(GetGrayPixelEx(coordmap.iX,slptr),index);
					index^=1;
					stretchmap.NextStep(coordmap);
					third=HashTo2bpp(GetGrayPixelEx(coordmap.iX,slptr),index);
					index^=1;
					stretchmap.NextStep(coordmap);
					fourth=HashTo2bpp(GetGrayPixelEx(coordmap.iX,slptr),index);
					index^=1;
					*bufptr++=TUint8(first|(second<<2)|(third<<4)|(fourth<<6));
					stretchmap.NextStep(coordmap);
					}
				}
			else
				{
				while(bufptr<buflimit)
					{
					first=GetGrayPixelEx(coordmap.iX,slptr)>>6;
					stretchmap.NextStep(coordmap);
					second=GetGrayPixelEx(coordmap.iX,slptr)>>6;
					stretchmap.NextStep(coordmap);
					third=GetGrayPixelEx(coordmap.iX,slptr)>>6;
					stretchmap.NextStep(coordmap);
					fourth=GetGrayPixelEx(coordmap.iX,slptr)>>6;
					*bufptr++=TUint8(first|(second<<2)|(third<<4)|(fourth<<6));
					stretchmap.NextStep(coordmap);
					}
				}
			break;
			}
		case EGray16:
			{
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength())<<1));
			aBuf.SetLength((aClipStrchLen+1)>>1);
			buflimit+=aBuf.Length();
			while(bufptr<buflimit)
				{
				first = GetGrayPixelEx(coordmap.iX,slptr) >> 4;
				stretchmap.NextStep(coordmap);
				first |= GetGrayPixelEx(coordmap.iX,slptr) & 0xf0;
				*bufptr++ = TUint8(first);
				stretchmap.NextStep(coordmap);
				}
			break;
			}
		case EColor16:
			{
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength())<<1));
			aBuf.SetLength((aClipStrchLen+1)>>1);
			buflimit+=aBuf.Length();
			while(bufptr<buflimit)
				{
				first = GetRgbPixelEx(coordmap.iX,slptr).Color16();
				stretchmap.NextStep(coordmap);
				first |= GetRgbPixelEx(coordmap.iX,slptr).Color16() << 4;
				*bufptr++ = TUint8(first);
				stretchmap.NextStep(coordmap);
				}
			break;
			}
		case EGray256:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength());
			aBuf.SetLength(aClipStrchLen);
			buflimit += aBuf.Length();

			while(bufptr < buflimit)
				{
				*bufptr++ = GetGrayPixelEx(coordmap.iX,slptr);
				stretchmap.NextStep(coordmap);
				}
			break;
			}
		case EColor256:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength());
			aBuf.SetLength(aClipStrchLen);
			buflimit += aBuf.Length();

			while(bufptr < buflimit)
				{
				*bufptr++ = TUint8(GetRgbPixelEx(coordmap.iX,slptr).Color256());
				stretchmap.NextStep(coordmap);
				}
			break;
			}
		case EColor4K:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 1);
			aBuf.SetLength(aClipStrchLen << 1);
			buflimit += aBuf.Length();

			while(bufptr < buflimit)
				{
				*((TUint16*)bufptr) = TUint16(GetRgbPixelEx(coordmap.iX,slptr)._Color4K());
				bufptr += 2;
				stretchmap.NextStep(coordmap);
				}
			break;
			}
		case EColor64K:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 1);
			aBuf.SetLength(aClipStrchLen << 1);
			buflimit += aBuf.Length();

			while(bufptr < buflimit)
				{
				*((TUint16*)bufptr) = TUint16(GetRgbPixelEx(coordmap.iX,slptr)._Color64K());
				bufptr += 2;
				stretchmap.NextStep(coordmap);
				}
			break;
			}
		case EColor16M:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() / 3);
			aBuf.SetLength(aClipStrchLen * 3);
			buflimit += aBuf.Length();

			while(bufptr<buflimit)
				{
				TInt color16M = GetRgbPixelEx(coordmap.iX,slptr)._Color16M();
				*bufptr++ = TUint8(color16M);
				*bufptr++ = TUint8(color16M >> 8);
				*bufptr++ = TUint8(color16M >> 16);
				stretchmap.NextStep(coordmap);
				}
			break;
			}
		case ERgb:
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:
			{
			aClipStrchLen = Min(aClipStrchLen,aBuf.MaxLength() >> 2);
			aBuf.SetLength(aClipStrchLen << 2);
			TUint32* pixelPtr = (TUint32*)bufptr;
			TUint32* pixelPtrLimit = pixelPtr + aClipStrchLen;
			if (aDispMode == EColor16MAP && displayMode == EColor16MA)
			while(pixelPtr < pixelPtrLimit)
				{
					*pixelPtr++ = NonPMA2PMAPixel(*(slptr + coordmap.iX));
				stretchmap.NextStep(coordmap);
				}
			else if (aDispMode == EColor16MAP && displayMode == EColor16MAP)
				while(pixelPtr < pixelPtrLimit)
					{
					*pixelPtr++ = *(slptr + coordmap.iX);
					stretchmap.NextStep(coordmap);
					}
			else if (aDispMode == EColor16MU)
				while(pixelPtr < pixelPtrLimit)
					{
					*pixelPtr++ = GetRgbPixelEx(coordmap.iX, slptr).Internal();
					stretchmap.NextStep(coordmap);
					}
			else
				while(pixelPtr < pixelPtrLimit)
					{
					*pixelPtr++ = GetRgbPixelEx(coordmap.iX, slptr).Internal();
					stretchmap.NextStep(coordmap);
				}
			break;
			}
		case EGray2:
			{
			TBool oddx=(aDitherOffset.iX&1);
			TBool oddy=(aDitherOffset.iY&1);
			aClipStrchLen=Min(aClipStrchLen,(TInt)((aBuf.MaxLength())<<3));
			aBuf.SetLength((aClipStrchLen+7)>>3);
			buflimit+=aBuf.Length();
			while(bufptr<buflimit)
				{
				first=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				stretchmap.NextStep(coordmap);
				second=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				stretchmap.NextStep(coordmap);
				third=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				stretchmap.NextStep(coordmap);
				fourth=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				stretchmap.NextStep(coordmap);
				fifth=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				stretchmap.NextStep(coordmap);
				sixth=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				stretchmap.NextStep(coordmap);
				seventh=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				stretchmap.NextStep(coordmap);
				eighth=HashTo1bpp(GetGrayPixelEx(coordmap.iX,slptr),oddx,oddy);
				*bufptr++=TUint8(first|(second<<1)|(third<<2)|(fourth<<3)|
					(fifth<<4)|(sixth<<5)|(seventh<<6)|(eighth<<7));
				stretchmap.NextStep(coordmap);
				oddx^=1;
				}
			break;
			}
		default:
			Panic(EFbsBitmapInvalidMode);
		}
	}
