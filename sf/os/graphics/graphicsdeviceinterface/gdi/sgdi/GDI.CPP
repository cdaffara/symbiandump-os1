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
// This file has been added to solve a dependency issue raised in INC102308. 
// LookupTable.h and BlendingAlgorithms.h are two Internal technologies that were 
// implicitly included in GDI.h. However, when building third-party releases of the OS
// these files are removed from the graphics directory, making them unreachable by 
//

// #include in GDI.h which causes the dependency failure.
// The solution here is to make the functions calling the inline functions in 
// LookupTable.h and BlendingAlgorithms.h non-inline as they were. Rather we will 
// export them within this file. Now the functions are safe to be called from other 
// components and they are not dependent on the headers.  
//

#include <graphics/lookuptable.h>
#include <graphics/blendingalgorithms.h>
#include <gdi.h>

/**Converts the TRgb to EColor16MAP (with color channels pre-multiplied with the alpha channel.
@return    The index which represents the TRgb values after multiplication with Alpha value. */
EXPORT_C TUint TRgb::_Color16MAP() const
	{
	return NonPMA2PMAPixel(iValue);
	}

/** Gets TRgb from 16MAP colour index.
The function takes a 32 bit colour value with eight bits for each
component, blue in the low byte, and returns the TRgb
whose red, green, and blue vales are divided by the alpha value.
@param     aARGB The pre-multiplied (EColor16MAP) color value.
@return    The TRgb which represents the color channel and alpha information. */
/* static */
EXPORT_C TRgb TRgb::_Color16MAP(TUint aARGB)
	{
	const TUint16* ArTable = PtrTo16BitNormalisationTable();
	TRgb retColor; 
	retColor.SetInternal(PMA2NonPMAPixel(aARGB, ArTable));
	return retColor;
	}
