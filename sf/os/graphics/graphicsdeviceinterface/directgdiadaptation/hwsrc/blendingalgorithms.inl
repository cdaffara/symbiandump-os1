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

const TUint16 NormalisationTable[256] = {
    0,65280,32640,21760,16320,13056,10880,9326,8160,7254,6528,
    5935,5440,5022,4663,4352,4080,3840,3627,3436,3264,
    3109,2968,2839,2720,2612,2511,2418,2332,2252,2176,
    2106,2040,1979,1920,1866,1814,1765,1718,1674,1632,
    1593,1555,1519,1484,1451,1420,1389,1360,1333,1306,
    1280,1256,1232,1209,1187,1166,1146,1126,1107,1088,
    1071,1053,1037,1020,1005,990,975,960,947,933,
    920,907,895,883,871,859,848,837,827,816,
    806,797,787,778,768,760,751,742,734,726,
    718,710,702,695,688,680,673,667,660,653,
    647,640,634,628,622,616,611,605,599,594,
    589,583,578,573,568,563,558,554,549,544,
    540,536,531,527,523,519,515,510,507,503,
    499,495,491,488,484,480,477,474,470,467,
    463,460,457,454,451,448,445,442,439,436,
    433,430,427,424,422,419,416,414,411,408,
    406,403,401,399,396,394,391,389,387,384,
    382,380,378,376,374,371,369,367,365,363,
    361,359,357,355,353,351,350,348,346,344,
    342,340,339,337,335,334,332,330,329,327,
    325,324,322,320,319,317,316,314,313,311,
    310,308,307,306,304,303,301,300,299,297,
    296,295,293,292,291,289,288,287,286,284,
    283,282,281,279,278,277,276,275,274,272,
    271,270,269,268,267,266,265,264,263,262,
    261,260,259,258,256,
};

//Algoriths for Premultiplied alpha screenmode/pixel format.

/**
Blend A over B. From Porter & Duff:
C = CaFa+CbFb, where Ca = aSrcPixel, Cb = aDest_io, Fa = 1, Fb=(1-Alpha(a))
C = Ca+Cb(1-Alpha(a))

@param aDestPixel			Destination pixel.
@param aSrcPixel		Source pixel.
@param aMaskingFactor	Alpha channel of aSrcPixel.
*/
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

inline TUint32 PMA2NonPMAPixel(TUint32 aPixel)
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
	TUint16 norm = NormalisationTable[alpha];
	TUint32 norm_rb = (((aPixel & KRBMask) * norm) >> 8) & KRBMask;
	TUint32 norm_g =  (((aPixel & KGMask ) * norm) >> 8) & KGMask;
	return ((aPixel & 0xff000000) | norm_rb | norm_g);
	}

