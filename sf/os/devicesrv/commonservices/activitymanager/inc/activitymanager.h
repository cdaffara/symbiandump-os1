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
// CUserActivityManager class
// 
//

#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <e32base.h>

/**
The CUserActivityManager API allows clients to easily monitor for
user inactivity and activity. It is a key part in implementing functionality 
such as power management and screen savers.

Clients construct the object using NewL() and call Start() to register 
callbacks to begin [in]activity monitoring. Monitoring continues until the 
object is destroyed. Clients can call SetInactivityTimeout() to adjust the
inactivity interval once started.

Callbacks supplied by clients should:
 - complete quickly with minimal blocking. 
 - never call CUserActivityManager APIs as reentrancy is not supported.
 - never leave otherwise it can lead to panics.

This API uses "CUserActivityManager" category panics for its error strategy as 
it has few error return code paths. Specific panics raised by APIs through 
improper use can be seen in the API documentation. KErrGeneral is raised
when the internal state is irrecoverable.

@see CTimer
@see TCallback

@publishedPartner
@released
*/
NONSHARABLE_CLASS(CUserActivityManager) : public CTimer
	{
public:
 	IMPORT_C static CUserActivityManager* NewL(TInt aPriority);
	IMPORT_C ~CUserActivityManager();
	IMPORT_C void Start(TTimeIntervalSeconds aInterval,
	                    TCallBack aInactiveCallback,
	                    TCallBack aActiveCallback);
	IMPORT_C void SetInactivityTimeout(TTimeIntervalSeconds aInterval);
private:
	void RunL();
	CUserActivityManager(TInt aPriority);
	void MonitorInactivity();
private:

    /** @internalComponent */
    enum TState {
        EStUndefined = 0,    //< Never used, reserved
        EStNotActive,        //< Constructed but not started
        EStMonForInactivity, //< Monitoring user inactivity mode
        EStMonForActivity    //< Monitoring user activity mode
    };

    /** Inactivity interval */
    TTimeIntervalSeconds iInterval;
    
    /** Callback to be used in case that user inactivity is detected */
	TCallBack            iInactiveCallback;
	
    /** Callback to be used in case of user activity after inactivity period */
	TCallBack            iActiveCallback;
	
    /** State of object, what form of monitoring is active. */
	TState                iState;
	};

#endif // ACTIVITYMANAGER_H
