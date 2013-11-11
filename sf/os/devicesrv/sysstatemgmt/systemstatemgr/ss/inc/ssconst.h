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

#ifndef __SSCONST_H__
#define __SSCONST_H__

/**
@internalTechnology
@released
*/
const TInt KSsmStartSafeNoTimeout = 0;
const TInt KSsmStartSafeMilliToMicro = 1000;
const TUint64 KSsmStartSafeNullThreadId = KNullThreadId;

#define __IN_RANGE( x, y ) ( (x >= 0) && (x < y) )

#endif // __SSCONST_H__
