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

#include "activitymanager.h"
#include "activitymanagerdebug.h"

/**
Factory function used to allocate and construct a new CUserActivityManager
object. 
 
@param  aPriority    Priority for the CTimer active object.
@leave  KErrNoMemory When memory allocations fail
@leave  ...          Any other one of the other system wide error codes
@return CUserActivityManager*   Pointer to a new User Activity Manager object
*/
EXPORT_C CUserActivityManager* CUserActivityManager::NewL(TInt aPriority)
	{
	CUserActivityManager* self = new (ELeave) CUserActivityManager(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Default constructor.

@param  aPriority Priority for the CTimer.
@internalComponent
*/
CUserActivityManager::CUserActivityManager(TInt aPriority)
	: CTimer(aPriority), iInactiveCallback(0,0), iActiveCallback(0,0), iState(EStNotActive)
	{
	}

/**
Destructor. 
*/
EXPORT_C CUserActivityManager::~CUserActivityManager()
	{
	}

/**
Starts the user activity monitoring using the callbacks supplied with the 
specify inactivity interval in seconds. If aInterval is not positive then 
in release builds it assumes an interval of 1 second but in debug builds it 
will panic. The function pointers in the callback arguments may be NULL if the
client is not interested in one or other events.

When called following construction or while inactivity is being monitored the 
manager will enter/stay in inactivity monitoring mode when aInterval is larger
than the current elapsed inactivity time at the time of the call and 
aInactiveCallback will be called first. Otherwise it will invoke 
aInactiveCallback immediately and enter activity monitoring mode and 
aActiveCallback will be called next.

When called while user activity is being monitored the manager will stay in 
activity monitoring mode and aActiveCallback will be called first.

@param aInterval         Inactivity interval in seconds.
@param aInactiveCallback This callback is used when there has not been user 
            activity for aInterval seconds.
@param aActiveCallback   This callback is used when user activity is detected 
            after an inactivity period longer or equal to aInterval seconds.
@panic "CUserActivityManager" KErrNotSupported raised when when aInterval is 
            not positive in DEBUG builds
@see   CTimer::Inactivity()
@see   RTimer::Inactivity()
*/
EXPORT_C void CUserActivityManager::Start(TTimeIntervalSeconds aInterval,
		TCallBack aInactiveCallback, TCallBack aActiveCallback) 
	{  	
	iInterval = aInterval;
	iInactiveCallback = aInactiveCallback;
	iActiveCallback = aActiveCallback;
			
	if	(iInterval.Int() <= 0)
		{
		// Panic if interval is not greater than zero. 
		__ACTIVITYMANAGER_TRACE1("Activity Mgr: Negative or zero interval supplied to Start(): %d", iInterval.Int());			
		__ASSERT_DEBUG(EFalse, UAM_PANIC(KErrNotSupported));
		// In release mode interval will be set to one second
		iInterval = TTimeIntervalSeconds(1);
		}
		
	// Cancel outstanding timer, if any
	CTimer::Cancel();
	
	if (iState == EStNotActive || iState == EStMonForInactivity) 
		{
		// Begin or contine monitoring for user inactivity
		MonitorInactivity();
		}
	else 
		{
		// Continue monitoring for user activity
		__ACTIVITYMANAGER_TRACE1("Activity Mgr: Starting activity monitoring. Inactive for %d", User::InactivityTime().Int());
		Inactivity(0); 
		}
	}

/**
Re-starts activity monitoring with a new inactivity interval. See Start() for 
behavioural description. 

Requires Start() to have been called previously otherwise the method will 
panic in debug builds if Start() has not be called before hand to
set up at least one callback. This prevents wasting CPU cycles/power on 
monitoring when no action will take place.

@see   CUserActivityManager::Start()
@param aInterval New inactivity interval in seconds.
@panic "CUserActivityManager" KErrNotSupported Raised when aInterval is 
            not positive in debug builds
@panic "CUserActivityManager" KErrNotReady Raised when Start() has not yet 
            been called in debug builds.
*/	     
EXPORT_C void CUserActivityManager::SetInactivityTimeout(TTimeIntervalSeconds aInterval)
	{
	if (iState <= EStNotActive)
    	{
    	__ACTIVITYMANAGER_TRACE("Activity Mgr: SetInactivityTimeout() called before Start()");
    	__ASSERT_DEBUG(EFalse, UAM_PANIC(KErrNotReady));
    	}
    	
	__ACTIVITYMANAGER_TRACE1("Activity Mgr: SetInactivityTimeout: %d", aInterval.Int());
	Start(aInterval, iInactiveCallback, iActiveCallback); 
	}

/**
This method attempts to enter inactivity monitoring mode. For this to occur the
current ellapsed inactivity must be less than the iInterval otherwise it will
invoke the inactivity callback and enter activity monitoring mode.

@see CTimer::Inactivity()
@see RTimer::Inactivity()
@internalComponent
*/
void CUserActivityManager::MonitorInactivity()
	{
	if (User::InactivityTime() >= iInterval) 
		{
		// Already inactive enough, monitor for activity
		iState = EStMonForActivity;
		// Monitor for activity, set timer before callback to avoid missing any
		// user activity should callback take some time.
		Inactivity(0); 
		// Inform client that the inactivity interval has ended.
		(void) iInactiveCallback.CallBack();
		__ACTIVITYMANAGER_TRACE1("Activity Mgr::MonitorInactivity - Starting activity monitoring. Inactive for %d", User::InactivityTime().Int());
		}
	else
		{
		// Monitor for Inactivity
		iState = EStMonForInactivity;
		Inactivity(iInterval);
		__ACTIVITYMANAGER_TRACE1("Activity Mgr::MonitorInactivity - Starting inactivity monitoring. Inactive for %d", User::InactivityTime().Int());
		}
	}

/**
This is called from CActiveScheduler when the timer registered using the 
CTimer::Inactivity() call expires. This could indicate user inactivity or
activity, iStatus is checked to determine which condition has been detected
by CTimer.

If the manager is in inactivity monitoring mode the iInactiveCallback will be
invoked and the manager will switch to monitoring for activity.
If the manager is in activity monitoring mode the iActiveCallback will be 
invoked and the manager will attempt to switch to monitoring for inactivity. 
This attempt may fail and it will remain in activity monitoring mode if an
iInterval amount of inactivity has passed since RunL() was invoked.

When CActive::iStatus indicates and error a panic is raised in debug builds. In
release builds it returns to the active scheduler to handle.

@see CUserActivityManager::MonitorInactivity()
@see CTimer::Inactivity()
@see RTimer::Inactivity()
@internalComponent
*/	
void CUserActivityManager::RunL()
	{
	if (iStatus == KErrNone)
		{
		if (iState == EStMonForInactivity) 
			{
			// Looking for Inactivity, found it
			__ACTIVITYMANAGER_TRACE("Activity Mgr: User inactivity detected");
	   	    // Monitor for activity, set timer before callback to avoid missing 
		    // any user activity should callback take some time.
       		iState = EStMonForActivity;
			Inactivity(0); 
			// Inform client that the inactivity interval has ended.
 			(void) iInactiveCallback.CallBack();
			}
		else if (iState == EStMonForActivity)
			{
			// Looking for activity, found it
			__ACTIVITYMANAGER_TRACE("Activity Mgr: User activity detected");
			(void) iActiveCallback.CallBack();
			// Monitor for activity unless MonitorInactivity overrides it
    		iState = EStMonForInactivity; 
			MonitorInactivity();
			}	
		else
            { // Must never happen, indicates internal state of object incorrect.
            __ASSERT_ALWAYS(EFalse, UAM_PANIC(KErrGeneral));
            }
		}
	else
		{
		// KErrCancel, KErrArgument, KErrOverflow,...
		__ACTIVITYMANAGER_TRACE1("Activity Mgr: RunL error: %d",iStatus.Int());			
		// Irrecoverable error, no way to notify client of failure (callbacks
		// have no parameters to pass back error codes).
		__ASSERT_ALWAYS(EFalse, UAM_PANIC(iStatus.Int()));
		}
	}
