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
// Maximum length of a text field from the Comms database
// in a file alone so it can be used in a resource file.
// 
//

/**
 @file
 @publishedAll
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef CDBLEN_H
#define CDBLEN_H

#include <e32def.h>

/**
Maximum text field length (in characters).
@publishedAll
@released
*/
const TInt KCommsDbSvrMaxFieldLength=50;

/**
@publishedAll
@released
*/
const TInt KCommsDbSvrDefaultTextFieldLength=50;
const TInt KCommsDbSvrMaxUserIdPassLength=255;
const TInt KCommsDbSvrRealMaxFieldLength=KCommsDbSvrMaxUserIdPassLength;

/** Maximum length of table and column names. 
@publishedAll
@released */
const TInt KCommsDbSvrMaxColumnNameLength=50;

#endif
