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

#ifndef __LOOKUPTABLE_H__
#define __LOOKUPTABLE_H__

#include <e32std.h>

/**
@internalTechnology
*/
 IMPORT_C const TUint16* Convert16to32bppLow();
 IMPORT_C const TUint32* Convert16to32bppHigh();

/**
Returns the pointer to the table of 16-bit normalisation table.
The table is currently used for performing un-multiplying Premultiplied Alpha color channels.
Use: Let x be c * a/255, and y = c, then y = (x/a)*255, where a is in 0-255 range.
		To use this table, use y = (x * Table[a])>>8. i.e, the inaccurate division is converted 
		to more accurate multiplication and shift.
		
@return pointer to the normalisation table.
@internalTechnology

*/
IMPORT_C const TUint16* PtrTo16BitNormalisationTable();



#endif
