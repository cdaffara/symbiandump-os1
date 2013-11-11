// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Shells for window server bitmap class
// Note: Note: A pointer to CWsBitmap must have the same pointer value as a pointer
// to the associated CFbsBitmap, otherwise code in BitGdi component will be
// Broken.
// 
//

#include <e32std.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"


EXPORT_C CWsBitmap::CWsBitmap()
/** Default constructor. Developers should use the other constructor overload. */
	{
	}

EXPORT_C CWsBitmap::CWsBitmap(RWsSession &aWs) : MWsClientClass(aWs.iBuffer)
/** Constructor which takes a window server session as an argument.

@param aWs Handle to window server session. */
	{
	}

EXPORT_C CWsBitmap::~CWsBitmap()
/** Destructor. */
	{
	Reset();
	}

EXPORT_C void CWsBitmap::Reset()
/** Releases the bitmap's handle from the font and bitmap server.

The function also decrements the bitmap's access count in the font and bitmap 
server. The server-side bitmap is deleted only if the access count for the 
bitmap decrements to zero. */
	{
	if (iBuffer && iWsHandle)
		Write(EWsBitmapOpFree);
	iWsHandle=NULL;
	CFbsBitmap::Reset();
	}

TInt CWsBitmap::createWsBitmap(TInt aErr)
	{
	if (aErr==KErrNone)
		{
		TWsClCmdCreateBitmap createBitmap;
		createBitmap.handle=Handle();
		TInt ret;
		if ((ret=iBuffer->WriteReplyWs(&createBitmap,sizeof(createBitmap),EWsClOpCreateBitmap))<0)
			{
			CFbsBitmap::Reset();
			aErr=ret;
			}
		else
			iWsHandle=ret;
		}
	return(aErr);
	}

EXPORT_C TInt CWsBitmap::Create(const TSize& aSizeInPixels,TDisplayMode aDispMode)
/** Creates a bitmap, specifying the size and display mode.

This function, if successful, always causes a flush of the window server buffer.

@param aSizeInPixels The size of the bitmap to be created. 
@param aDispMode The display-mode of the bitmap to be created. 
@return KErrNone if successful, KErrCouldNotConnect if no connection to the 
font and bitmap server could be made, KErrArgument if aSizeInPixels is illegal.
@see CFbsBitmap::Create() */
	{
	Reset();
	return(createWsBitmap(CFbsBitmap::Create(aSizeInPixels,aDispMode)));
	}

EXPORT_C TInt CWsBitmap::Duplicate(TInt aHandle)
/** Makes this bitmap a duplicate of the specified bitmap.

This function, if successful, always causes a flush of the window server buffer.

@param aHandle The handle of the bitmap to be duplicated. 
@return KErrNone if successful, KErrCouldNotConnect if no connection to the 
font and bitmap server could be made, or KErrUnknown if no bitmap could be 
found whose handle is aHandle. 
@see CFbsBitmap::Duplicate() */
	{
	Reset();
	return(createWsBitmap(CFbsBitmap::Duplicate(aHandle)));
	}

EXPORT_C TInt CWsBitmap::Load(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded)
/** Loads a bitmap from a file.

If aShareIfLoaded is ETrue the bitmap will be allowed to be shared by a number 
of font and bitmap server clients.

This function, if successful, always causes a flush of the window server buffer.

@param aFileName The filename of the multibitmap (.mbm) file containing the 
bitmap to be loaded. 
@param aId The identifier of the bitmap in the .mbm file which should be loaded.
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be made 
available for sharing between font and bitmap server clients. 
@return KErrNone if successful, otherwise KErrCouldNotConnect, KErrEof, or 
an error specific to the font and bitmap server.
@see CFbsBitmap::Load() */
	{
	Reset();
	return(createWsBitmap(CFbsBitmap::Load(aFileName,aId,aShareIfLoaded)));
	}

EXPORT_C void CWsBitmap::InternalizeL(RReadStream& aStream)
/** Internalises a CWsBitmap from the read stream.

The presence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

This function always causes a flush of the window server buffer.

@param aStream The stream from which to internalise the bitmap. */
	{
	Reset();
	CFbsBitmap::InternalizeL(aStream);
	User::LeaveIfError(createWsBitmap(KErrNone));
	}

