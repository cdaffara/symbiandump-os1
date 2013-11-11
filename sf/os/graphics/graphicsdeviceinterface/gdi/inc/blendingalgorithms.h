// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BLENDINGALGORITHMS_H__
#define __BLENDINGALGORITHMS_H__

#include <e32def.h>
/**
@file
@internalTechnology
*/

/** Premultiplied Alpha (PMA) Blending algorithm.
Assumes src and target pixels are in PMA format (i.e., the color channels are already multiplied
with the alpha channel.
Blending Equations:
Cd' = Cs + ((255 - As) * Cd)/255
Ad' = As + ((255 - As) * Ad)/255
where Cd' and Ad' are destination values for color and alpha channels respectively, after blending,
Cd and Ad are destination values before blending, and Cs and As are source values (color and
alpha values respectively).
Optimisations:
Use of 256 instead of 255 converts the division to shift (faster).
Red and Blue channels are calculated together in one 32 bit operation.
Alpha and Green channels are calculated in the same way. However, due the
position of these channels within the pixel format, an extra shift is required.
The equations used are:
Cd' = Cs + (((0x100 - As) * Cd)>>8)
Ad' = As + (((0x100 - As) * Ad)>>8)
For the results to not overflow, it is required that the value of any of the color channels
is no greater than the alpha channel, which is what is exepected of pma format.
It is also assumed that the aMask is equal to source alpha.
@param 	aDestPixel	The destination pixel value.
@param	aSrcPixel	The source pixel value.
@param	aMask		The source alpha. It is assumed to be same as source alpha.
@return the alpha-blended pixel (within the PMA space).
@internalTechnology
@released
*/
inline TUint32 PMAPixelBlend(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMask);

/** Same as inline TUint32 PMAPixelBlend(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMask)
Calls inline TUint32 PMAPixelBlend(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMask), with aMask
extracted from the source pixel.
@see PMAPixelBlend(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMask)
@internalTechnology
@released
*/
inline TUint32 PMAPixelBlend(TUint32 aDestPixel, TUint32 aSrcPixel);

/**
Does the premultiplied alpha blend, but does not check for any pre/post conditions.
Please DO NOT Add any optimisations to check for these conditions in this code!
@internalTechnology
@released
*/
inline TUint32 PMABlend_noChecks(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMaskingFactor);

/**
The version of PMABlend_noChecks which does the blending in place.
@internalTechnology
@released
*/
inline void PMABlend_noChecksInplace(TUint32& aDest_io, const TUint32& aSrcPixel, TUint8 aMaskingFactor);

/**
The in-place version of the PMAPixelBlend algorithm
@internalTechnology
@released
*/
inline void PMAInplaceBlend(TUint32& aDest_io, TUint32& aSrc_in);
/**
Premultiplies the color channel values with the Alpha channel value.
Alpha value remains unchanged. An approximation is used in the operation where the division
by 255 is approximated by a shift-by-8-bits operation (i.e. division by 256).
@param	aPixel	The 32 bit pixel value to be pre-multiplied.
@return	The PMA value.
@internalTechnology
@released
*/
inline TUint32 NonPMA2PMAPixel(TUint32 aPixel);

/**
Divives the PMA pixel color channels with the Alpha value, to convert them to non-PMA format.
Alpha value remains unchanged.
@param	aPixel	the premultiplied 32 bit pixel value.
@param	aNormTable	The lookup table used to do the normalisation (the table converts the division
					to multiplication operation).
					The table is usually obtainable by a call to the method:
					PtrTo16BitNormalisationTable, which is defined in lookuptable.dll(.lib).
					The lookup table for normalised alpha is compluted using this equation: 
					Table[index] = (255*256) / index (where index is an 8 bit value).
@return The NON-PMA 32 bit pixel value.
@internalTechnology
@released
*/
inline TUint32 PMA2NonPMAPixel(TUint32 aPixel, const TUint16* aNormTable);

/**
In-place version of NonPMA2PMAPixel.
@see NonPMA2PMAPixel
@internalTechnology
@released
*/
inline void Convert2PMA(TUint32& aInOutValue);

/**
In-place version of PMA2NonPMAPixel
@see PMA2NonPMAPixel
@internalTechnology
@released
*/
inline void Convert2NonPMA(TUint32& aInOutValue, const TUint16* aNormTable);

#include <graphics/blendingalgorithms.inl>

#endif //__BLENDINGALGORITHMS_H__
