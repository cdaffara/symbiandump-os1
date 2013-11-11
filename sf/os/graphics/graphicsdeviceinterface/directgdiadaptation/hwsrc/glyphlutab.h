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

#ifndef GLYPHLUTAB_H_
#define GLYPHLUTAB_H_

/**
@file
@internalComponent
*/

/**
Outline colour index used to get percentage of outline colour. This is used to get the final colour from the lookup table.
*/
const TInt KOutlineColorIndex = 0;

/**
Shadow colour index used to get percentage of shadow colour. This is used to get the final colour from the lookup table.
*/
const TInt KShadowColorIndex = 1;

/**
Fill colour index used to get percentage of fill colour. This is used to get the final colour from the lookup table.
*/
const TInt KFillColorIndex = 2;

/**
Background colour index used to get percentage of background colour. This is used to get the final colour from the lookup table.
*/
const TInt KBackgroundColorIndex = 3;

/**
Lookup table used for outline and shadow fonts.
*/
GLREF_D const TInt FourColorBlendLookup[256][4];

/**
The structure of a compound font.
*/
struct TCompoundFont
	{
	TInt iOutline;
	TInt iShadow;
	TInt iFill;
	TInt iBackground;
	};

#endif
