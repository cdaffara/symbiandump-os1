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
// Declaration of the CDirectGdiFont class
// which is very similar to the CFbsBitGcFont
// 
//

#ifndef DIRECTGDIFONT_H
#define DIRECTGDIFONT_H

/**
@file
@internalTechnology
*/

#include <e32std.h>
#include <fbs.h>

class CBitmapFont;

/**
Font class for use with DirectGDI
*/
NONSHARABLE_CLASS(CDirectGdiFont) : public CFbsFont
	{
public:
	IMPORT_C CDirectGdiFont();
	IMPORT_C virtual ~CDirectGdiFont();
	IMPORT_C CBitmapFont* Address() const;
	IMPORT_C TInt Duplicate(TInt aHandle);
	IMPORT_C void Reset();
	IMPORT_C void operator=(const CDirectGdiFont& aFont);
protected:
	TBool iCopy;	/**< ETrue if this font is copied from another font.*/
	};

#endif /*DIRECTGDIFONT_H*/
