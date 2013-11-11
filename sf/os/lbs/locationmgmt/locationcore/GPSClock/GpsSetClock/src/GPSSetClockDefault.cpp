// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Lbs
#include "lbsdevloggermacros.h"

// Component
#include "GPSSetClockDefault.h"

/** Static constructor.
@see CGPSSetClockBase
@internalComponent
@released
*/
CGPSSetClockDefault* CGPSSetClockDefault::NewL()
	{
	LBSLOG(ELogP1, "CGPSSetClockDefault::NewL()\n");
	CGPSSetClockDefault* self = new(ELeave)CGPSSetClockDefault;
	return self;
	}

/** Default destructor.
@see CGPSSetClockBase
@internalComponent
@released
*/
CGPSSetClockDefault::~CGPSSetClockDefault()
	{
	LBSLOG(ELogP1, "CGPSSetClockDefault::~CGPSSetClockDefault()\n");
	}

/** Default constructor.
@see CGPSSetClockBase
@internalComponent
@released
*/
CGPSSetClockDefault::CGPSSetClockDefault()
	{
	LBSLOG(ELogP1, "CGPSSetClockDefault::CGPSSetClockDefault()\n");
	}

/** The default implementation calls User::SetUTCTime()
@see CGPSSetClockBase
@internalComponent
@released
*/
TInt CGPSSetClockDefault::SetUTCTime(const TTime& aUTCTime)
	{
	LBSLOG(ELogP1, "CGPSSetClockDefault::SetUTCTime()\n");
	return User::SetUTCTime(aUTCTime);
	}

