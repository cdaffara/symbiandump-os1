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

#ifndef __FONTARABIC_H__
#define __FONTARABIC_H__


#include <e32std.h>
#include "GlyphSel.h"


// 
//
// GlyphSelector_Arabic Class declaration
//
//


class GlyphSelector_Arabic
/**
 This is the default glyph selector class for Arabic characters in the range
 0x0600..0x06FF. It is able to process Arabic base characters and combining
 marks to construct the necessary output glyph cluster.
@internalComponent
*/
	{
public:

	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&) ;
	
	static TBool CharactersJoin(TInt aLeftCharacter, TInt aRightCharacter);

	};



#endif // __FONTARABIC_H__
