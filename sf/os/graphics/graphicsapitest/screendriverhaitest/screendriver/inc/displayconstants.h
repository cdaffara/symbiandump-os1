/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef	DISPLAY_CONSTANTS_H
#define	DISPLAY_CONSTANTS_H

#include <gdi.h>

const TInt KNumDispModes = 12;
const TInt KNumTestBackgrounds = 6;
const TInt KNumTestColors = 12;
const TInt KNumDrawModes = 6;
const TInt KMaskFill =3;
const TInt KNumShadowModes = 4;
const TInt KRectWidth = 32;

const TDisplayMode TestDisplayMode[KNumDispModes] = 
	{
	EGray2,
	EGray4,
	EGray16,
	EGray256,
	EColor16,
	EColor256,
	EColor4K,
	EColor64K,
	EColor16M,
	ERgb,
	EColor16MU,
	EColor16MA,
	};

const TRgb TestBackground[KNumTestBackgrounds] = 
	{
	KRgbWhite,
	KRgbBlack,
	TRgb(20,250,160),
	TRgb(0xff27D99f),
	TRgb(0x00E73121),
	TRgb(0x771FE3B7),
	};

/**
*	Defines Color Values
*/
const TRgb TestColor[KNumTestColors] = 
	{
	KRgbBlack,
	KRgbDarkGray,
	KRgbGray,
	KRgbWhite,
	KRgbRed,
	KRgbCyan,
	TRgb(230,180,70),
	TRgb(150,240,30),
	TRgb(20,250,160),
	TRgb((TUint32)0xff2665A3),
	TRgb((TUint32)0x002766A4),
	TRgb((TUint32)0x57339A4E),
	};

const TUint32 MaskFill[KMaskFill] =
	{
	0x00,
	0xff,
	0x3A,
	};

enum TPostShadowMode	
	{
	ENoPostShadow,
	EPostShadow,
	EPostFade,
	EPostShadowFade,	
	};

#endif // DISPLAY_CONSTANTS_H
