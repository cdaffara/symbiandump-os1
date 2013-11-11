// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bitdev.h>
#include "BITPANIC.H"

/**
Construct calls the constructor for the base class CFbsFont and sets default
value of iCopy as EFalse.
*/
EXPORT_C CFbsBitGcFont::CFbsBitGcFont():
	CFbsFont(),
	iCopy(EFalse)
	{}

/**
Default destructor. 
*/
EXPORT_C CFbsBitGcFont::~CFbsBitGcFont()
	{}

/**
On return contains iAddress pointer for the CBitmapFont.
@return CBitmapFont* A pointer to the font
@see CFbsFont::Address();
*/
EXPORT_C CBitmapFont* CFbsBitGcFont::Address() const
	{
	return CFbsFont::Address();
	}

/**
Calls reset on the object (sets iHandle to zero and carries out related 
messenger housekeeping and then duplicates it using the base class method.
@param TInt aHandle Is the integer handler for the font.
@return TInt KErrNone If successful else a system wide error code.
@see CFbsFont::Duplicate(TInt)
*/
EXPORT_C TInt CFbsBitGcFont::Duplicate(TInt aHandle)
	{
	Reset();

	return CFbsFont::Duplicate(aHandle);
	}

/**
Resets the graphics context to its default settings.

The function provides a concrete implementation of the pure virtual
function <code>CGraphicsContext::Reset()</code>. The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGcFont::Reset()
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
Defines the meaning of the equals operator when acting on a CFbsBitGcFont
object. i.e. allows these objects to be set as being equal to one another.
@param CFbsBitGcFont& aFont the font to be copied
*/
EXPORT_C void CFbsBitGcFont::operator=(const CFbsBitGcFont& aFont)
	{
	Reset();
	iAddressPointer = aFont.iAddressPointer;
	iHandle = aFont.iHandle;
	iServerHandle = aFont.iServerHandle;
	iCopy = ETrue;
	}

/**
Gets the bitmap address.
@return The bitmap address.
*/
EXPORT_C CBitwiseBitmap* CFbsBitGcBitmap::Address() const
	{
	return CFbsBitmap::CleanAddress();
	}

/**
Locks the global bitmap heap 
*/
EXPORT_C void CFbsBitGcBitmap::LockHeap() const
	{
	CFbsBitmap::LockHeap();
	}

/**
Unlock the global bitmap heap 
*/
EXPORT_C void CFbsBitGcBitmap::UnlockHeap() const
	{
	CFbsBitmap::UnlockHeap();
	}

