// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for TDisplayMode.
//

#ifndef DISPLAYMODE_H
#define DISPLAYMODE_H

/** Display modes. 
@publishedAll
@released
*/
enum TDisplayMode
	{
	/** No display mode */
	ENone,
	/** Monochrome display mode (1 bpp) */
	EGray2,
	/** Four grayscales display mode (2 bpp) */
	EGray4,
	/** 16 grayscales display mode (4 bpp) */
	EGray16,
	/** 256 grayscales display mode (8 bpp) */
	EGray256,
	/** Low colour EGA 16 colour display mode (4 bpp) */
	EColor16,
	/** 256 colour display mode (8 bpp) */
	EColor256,
	/** 64,000 colour display mode (16 bpp) */
	EColor64K,
	/** True colour display mode (24 bpp) */
	EColor16M,
	/** (Not an actual display mode used for moving buffers containing bitmaps) */
	ERgb,
	/** 4096 colour display (12 bpp). */
	EColor4K,
	/** True colour display mode (32 bpp, but top byte is unused and unspecified) */
	EColor16MU,
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	EColor16MA,
	/** Pre-multiplied Alpha display mode (24bpp color multiplied with the alpha channel value, plus 8bpp alpha) */
	EColor16MAP,
	//Any new display mode should be insterted here!
	//There might be up to 255 display modes, so value of the last
	//inserted EColorXXX enum item should be less than 256 -
	//BC reasons!
	EColorLast
	};

#endif /* DISPLAYMODE_H */
