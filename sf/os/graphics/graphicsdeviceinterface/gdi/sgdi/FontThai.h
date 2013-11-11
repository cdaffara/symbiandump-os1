// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __FONTTHAI_H__
#define __FONTTHAI_H__


#include "GlyphSel.h"


// 
//
// GlyphSelector_Thai Class declaration
//
//


class GlyphSelector_Thai
/**
 This is the default glyph selector class for Thai characters in the range
 0x0E00..0x0E7F. It is able to process Thai base characters and combining
 marks to construct the necessary output glyph cluster.
@internalComponent
*/
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};


// 
//
// GlyphSelector_ThaiSaraAm Class declaration
//
//


class GlyphSelector_ThaiSaraAm
/**
 This is the glyph selector class for the decomposable Thai character 
 SARA AM (0x0E33). It is special because depending on the surrounding 
 text it may be rendered using 1, 2, or 3 (dotted circle) glyphs.
@internalComponent
*/
	{
public:
	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};


#endif // __FONTTHAI_H__
