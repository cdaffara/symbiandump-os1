// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements inline methdos of RDirectGdiDrawableSource.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef DIRECTGDIDRAWABLESOURCE_INL
#define DIRECTGDIDRAWABLESOURCE_INL

/**
Accessor for reference to adaptation object.

@see RDirectGdiImageTarget::SetHandle()

@return	A handle to the adaptation object.
*/
inline TInt RDirectGdiDrawableSource::Handle() const
	{
	return iHandle;
	}

#endif // DIRECTGDIDRAWABLESOURCE_INL


