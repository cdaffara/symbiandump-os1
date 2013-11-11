// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

const TUint32 KRBMask = 0x00ff00ff;
const TUint32 KAGMask = 0xff00ff00;
const TUint32 KGMask  = 0x0000ff00;

//Algoriths for Premultiplied alpha screenmode/pixel format.

inline TUint32 PMAPixelBlend(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMask)
	{
	if(aMask)
		{
		if(aMask == 0xff) // opaque, so unchanged
			{
			return aSrcPixel;
			}
		else
			{
			return PMABlend_noChecks(aDestPixel, aSrcPixel, aMask);
			}
		}
	 else // completely transparent
		{
		return aDestPixel;
 		}
	}

inline TUint32 PMAPixelBlend(TUint32 aDestPixel, TUint32 aSrcPixel)
	{
	TUint8 mask = (TUint8)(aSrcPixel >> 24);
	return PMAPixelBlend(aDestPixel, aSrcPixel, mask);
	}

inline TUint32 PMABlend_noChecks(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMaskingFactor)
	{
	TUint32 src_c = aSrcPixel & KRBMask;
	TUint32 dst_c = aDestPixel  & KRBMask;
	const TUint32 mask = 0x0100 - aMaskingFactor;
	dst_c = (src_c +  ((mask * dst_c)>>8)) & KRBMask;
	src_c = (aSrcPixel & KAGMask)>>8;
	TUint32 dst_ag = (aDestPixel & KAGMask)>>8;
	dst_c |= ((src_c +  ((mask * dst_ag)>>8)) & KRBMask)<<8;
	return dst_c;
	}

inline void PMABlend_noChecksInplace(TUint32& aDest_io, const TUint32& aSrcPixel, TUint8 aMaskingFactor)
	{
	TUint32 src_c = aSrcPixel & KRBMask;
	TUint32 dst_ag = (aDest_io & KAGMask) >> 8;
	aDest_io = aDest_io  & KRBMask;
	const TUint32 mask = 0x0100 - aMaskingFactor;
	aDest_io = (src_c +  ((mask * aDest_io) >> 8)) & KRBMask;
	src_c = (aSrcPixel & KAGMask) >> 8;
	aDest_io |= ((src_c +  ((mask * dst_ag) >> 8)) & KRBMask)<<8;
	}

inline void PMAInplaceBlend(TUint32& aDest_io, TUint32& aSrc_in)
	{
	TUint8 mask = (TUint8)(aSrc_in >> 24);
	if(mask)
		{
		if(mask == 0xff) // opaque, so dst = src.
			{
			aDest_io = aSrc_in;
			}
		else
			{
			PMABlend_noChecksInplace(aDest_io, aSrc_in, mask);
			}
		}
	//else src completely transparent, so dst unchanged.
	}

inline TUint32 NonPMA2PMAPixel(TUint32 aPixel)
	{
	TUint8 tA = (TUint8)(aPixel >> 24);
	if (tA==0)
		{ 
		return 0;
		}
	if (tA==0xff) 
		{
		return aPixel;
		}

	// Use a bias value of 128 rather than 255, but also add 1/256 of the numerator 
	// before dividing the sum by 256.

	TUint32 tap1=tA+1;
	TUint32 scaledRB = (aPixel & KRBMask) * tap1;
	TUint32 scaledG = (aPixel & KGMask ) * tap1;
	return (aPixel & 0xff000000) | ((scaledRB>>8) & KRBMask) | ((scaledG>>8)& KGMask);
	}

inline TUint32 PMA2NonPMAPixel(TUint32 aPixel, const TUint16* aNormTable)
	{
	TUint8 alpha = (TUint8)(aPixel >> 24);
	if (alpha==0)
		{ 
		return 0;
		}
	if (alpha==0xff) 
		{
		return aPixel;
		}
	TUint16 norm = aNormTable[alpha];
	TUint32 norm_rb = (((aPixel & KRBMask) * norm) >> 8) & KRBMask;
	TUint32 norm_g =  (((aPixel & KGMask ) * norm) >> 8) & KGMask;
	return ((aPixel & 0xff000000) | norm_rb | norm_g);
	}

inline void Convert2PMA(TUint32& aInOutValue)
	{
	aInOutValue = NonPMA2PMAPixel(aInOutValue);
	}

inline void Convert2NonPMA(TUint32& aInOutValue, const TUint16* aNormTable)
	{
	aInOutValue = PMA2NonPMAPixel(aInOutValue, aNormTable);
	}