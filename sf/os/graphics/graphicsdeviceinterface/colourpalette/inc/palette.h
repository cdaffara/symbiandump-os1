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

#ifndef __PALETTE_H__
#define __PALETTE_H__

#include <e32base.h>

class TColor256Util;


class DynamicPalette
/** Hooks to allow dynamic palette switching in 256 colour mode.

TRgb::Color256() transparently invokes hooks in this class. The behaviour 
of the system can be customized by replacing the implementation in palette.dll 
with device-specific functionality to switch the colour palette at run-time. 
@publishedAll
@released
*/
	{
public:
	/** Stores the index values of the palettes defined. This enum may be as 
	short or as long as desired - there is no requirement that the size of the 
	enum in any given implementation be limited to the five sample values given 
	in this example implementation. */
	enum TIndex
		{
		/** A palette index number. */
		EIndex0,
		/** A palette index number. */
		EIndex1,
		/** A palette index number. */
		EIndex2,
		/** A palette index number. */
		EIndex4,
		/** A palette index number. */
		EIndex5,
	};

	IMPORT_C static const TColor256Util* DefaultColor256Util();
	IMPORT_C static void SetColor256Util(TIndex aIndex);

	IMPORT_C static const TUint32* Color16array();
	IMPORT_C static const TUint8* Color16inverse();
	};

#endif
