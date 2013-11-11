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
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef SGERROR_H
#define SGERROR_H

#include <e32def.h>


/**
Graphics Resource error code: an attempt to allocate specialised memory has failed.
*/
const TInt KErrNoSpecializedMemory = -481;

/**
Graphics Resource error code: a resource cannot be initialised due to lack of data.
*/
const TInt KErrNoInitializationData = -482;


#endif // SGERROR_H
