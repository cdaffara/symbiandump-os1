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

#include "directgdifont.h"

/**
Construct calls the constructor for the base class CFbsFont and sets default
value of iCopy as EFalse.
*/
EXPORT_C CDirectGdiFont::CDirectGdiFont():
	CFbsFont(),
	iCopy(EFalse)
	{}

/**
Default destructor. 
*/
EXPORT_C CDirectGdiFont::~CDirectGdiFont()
	{}

/**
On return contains iAddress pointer for the CBitmapFont.
@return	A pointer to the font.
@see	CFbsFont::Address();
*/
EXPORT_C CBitmapFont* CDirectGdiFont::Address() const
	{
	return CFbsFont::Address();
	}

/**
Calls reset on the object (sets iHandle to zero) and carries out related 
messenger housekeeping and then duplicates it using the base class method.
@param	aHandle The integer handler for the font.
@return	KErrNone if successful, otherwise a system-wide error code.
@see	CFbsFont::Duplicate(TInt)
*/
EXPORT_C TInt CDirectGdiFont::Duplicate(TInt aHandle)
	{
	Reset();

	return CFbsFont::Duplicate(aHandle);
	}

/**
Resets the font to its default settings.
*/
EXPORT_C void CDirectGdiFont::Reset()
	{
	if (!iCopy)
		{
		CFbsFont::Reset();
		}
	else
		{
		iAddressPointer = NULL;
		iHandle = 0;
		iServerHandle = 0;
		iCopy = EFalse;
		}
	}

/**
Defines the meaning of the equals operator when acting on a CDirectGdiFont
object. It allows these objects to be set as being equal to one another.
@param	aFont The font to be copied.
*/
EXPORT_C void CDirectGdiFont::operator=(const CDirectGdiFont& aFont)
	{
	if (this != &aFont)
		{
		Reset();
		iAddressPointer = aFont.iAddressPointer;
		iHandle = aFont.iHandle;
		iServerHandle = aFont.iServerHandle;
		iCopy = ETrue;
		}
	}

