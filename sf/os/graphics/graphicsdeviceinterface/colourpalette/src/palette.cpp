// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "palette.h"
#include "rgbarray.h"


EXPORT_C const TUint32* DynamicPalette::Color16array()
	{
	return color16array;
	}

EXPORT_C const TUint8* DynamicPalette::Color16inverse()
	{
	return color16inverse;
	}

EXPORT_C const TColor256Util* DynamicPalette::DefaultColor256Util()
/** Returns the current system palette. Depending on the
implementation this value depends on the current color
scheme set using SetColorScheme.

@return   Current system palette. */
	{
	return &defaulTColor256Util;
	}

EXPORT_C void DynamicPalette::SetColor256Util(DynamicPalette::TIndex /*aScheme*/)
/** Switches the device to use the specified colour palette. 
	
The reference version provided does nothing, since only one palette is used 
in the reference implementation.
	
@param aIndex The index number of the colour palette to use. */
	{
	}



