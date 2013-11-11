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

#ifndef CSWDIRECTGDIBITMAP_H_
#define CSWDIRECTGDIBITMAP_H_

/**
@file
@internalComponent
*/

#include <e32std.h>
#include <gdi.h>
#include <fbs.h>

/**
A bitmap managed by the font and bitmap server.
*/
class CSwDirectGdiBitmap : public CFbsBitmap
{
public:
	inline CBitwiseBitmap* Address() const;
};

/**
Gets the bitmap address.
@return The bitmap address.
*/
inline CBitwiseBitmap* CSwDirectGdiBitmap::Address() const
	{
	return CFbsBitmap::CleanAddress();
	}

#endif /*CSWDIRECTGDIBITMAP_H_*/
