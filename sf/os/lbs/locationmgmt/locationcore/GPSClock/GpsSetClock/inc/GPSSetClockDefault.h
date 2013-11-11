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

#ifndef GPSSETCLOCKDEFAULT_H
#define GPSSETCLOCKDEFAULT_H

// System
#include <e32base.h>

// Component
#include <lbs/gpssetclockbase.h>

/** Default implementation of the SetUTCTime() used to sync the system clock with the GPS clock.
@see CGPSSetClockBase
@internalComponent
@released
 */
NONSHARABLE_CLASS(CGPSSetClockDefault) : public CGPSSetClockBase
	{
public:
	static CGPSSetClockDefault* NewL();
	virtual ~CGPSSetClockDefault();
	
	virtual TInt SetUTCTime(const TTime &aUTCTime);

private:
	CGPSSetClockDefault();
	};

#endif //GPSSETCLOCKDEFAULT_H
