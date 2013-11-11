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

#ifndef __SYSMON_PATCH_H__
#define __SYSMON_PATCH_H__

/**
This patchable constant contains the throttle time - time to wait before restarting a process/application.
@SYMPatchable
@publishedPartner
@released
*/
IMPORT_C extern const TInt KWaitTime;

/**
This patchable constant contains the Rate of Failure (i.e. re-launch) of a monitored component.
If, at a re-launch attempt, a count(number of re-launch attempts) exceeds the maximum
(no. of retries specified for the component) allowed, the component will not be re-launched and 
the failure policy specified when this component was monitored will be enacted.
Default value for this constant is - 30mins.  Interval is taken in seconds

@SYMPatchable
@publishedPartner
@released
*/
IMPORT_C extern const TInt KIntervalForReLaunchRateOfFailure;

#endif