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
//


#include <e32def.h>
#include "CS_STD.H"
#include "C32LOG.H"
#include "cs_glob.h"
/** @file
 *
 * implements the static utility class CommTimer
 */

EXPORT_C void CommTimer::Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds, TDeltaTimerEntry& aHandle)
/**
 Queue a timer on the global timer.
 
 This class is only suitable for CSYs since it currently has no direct mechanism through which to release
 the Thread Local Storage and heap resources it allocates.
 In the case of CSYs these resources are managed separately by C32.
 
 
 Note:
 
 1. The Thread Local Storage of the calling thread must be available for use.
 
 2. The first time this is called for a thread, a small amount of heap memory is required. If
    no memory is available, a "c32-fault" panic of type 7 is raised. Subsequent calls are not affected since
    they reuse this memory.
 
 @param aTimeInMicroSeconds the timeout value in micro seconds
 @param aHandle handle to the delta timer entry
 */
 // extra note: C32 does not leak this memory when a CSY uses the CommTimer since they will fetch the 
 // tls of the C32.DLL for their particular player thread. This same tls is then used by the Player
 // during shutdown to find and release the CommTimer that the CSYs in the Player have been using.
 // However, when a client calls this API, they will in affect be getting CommTimer to allocate
 // the CDeltaTimer and CTLSRedirector but the client has no means to interact with the unpublished
 // CTLSRedirector class in order to delete the CDeltaTimer class that it refers to.
	{
	C32_STATIC_LOG(KC32Detail,_L8("CommTimer::Queue()"));
	aTimeInMicroSeconds = aTimeInMicroSeconds.Int() + (KCommTimerGranularity>>2);

	if(aTimeInMicroSeconds.Int() < KDeltaTimerInterval)
		{
		aTimeInMicroSeconds = aTimeInMicroSeconds.Int() + KCommTimerGranularity;
		}

	CDeltaTimer* timer = GetTimer();
	if (timer) // can't do much if the allocation of the CDeltaTimer failed.
		{      // There is no return value
		timer->Queue(aTimeInMicroSeconds, aHandle);
		}
	}


EXPORT_C void CommTimer::Remove(TDeltaTimerEntry& aTimer)
/**
 * Call cancel on the global timer
 *
 * Note: This does not free the Thread Local Storage for the calling thread.
 *
 * @param aTimer the timer to cancel
 */
	{
	C32_STATIC_LOG(KC32Detail,_L8("CommTimer::Remove()"));
	CDeltaTimer* timer = GetTimer();
 	if (timer)
		{
 		timer->Remove(aTimer);
		}
	}


CDeltaTimer* CommTimer::GetTimer()
/**
  Get the pointer to the global timer. If it does not
  exist, create a new timer and store the pointer in the TLS.
 
 Only clients within CSYs should use this API, since this function has a legacy behaviour
 of panicking in the case of no memory. This is due to the API not containing a means
  to communicate this result back to the caller.
 In the case where GetTimer is used by a CSY this is not a problem since the CSY will be running in a thread
 that C32 has already allocated the CommTimer memory for.
 
  @return pointer to the timer
 
  @note This function is using TLS (Thread Local Storage) and may
        reduce performance.
 */
	{
	// Previously it was planned to move away from using the TLS,
	// but CommTimer is published now so no current means by which we can change this.
	TAny* d = Dll::Tls();
	CTLSRedirector* tls = NULL;
	if (d == NULL)
		{
		C32_STATIC_LOG(KC32Detail,_L8("CommTimer::GetTimer() Client TLS is NULL, initializing"));
		TRAPD(ret,tls = CTLSRedirector::NewL());
		
#ifdef	__FLOG_ACTIVE
		if (ret != KErrNone)
			{
			C32_STATIC_LOG2(KC32Detail,_L8("CommTimer::GetTimer()-creation of redirector failed with %d. Will panic"),ret);
			}
#endif
		__ASSERT_ALWAYS(ret==KErrNone, Fault(EDTimerAllocFailure));
		Dll::SetTls(tls);
		}
	else
		{
		tls = static_cast<CTLSRedirector*>(d);
		}
		
		
	if (tls->DeltaTimer() == NULL)
		{
		C32_STATIC_LOG(KC32Detail,_L8("CommTimer::GetTimer() CDeltaTimer is NULL, initializing"));
		
		CDeltaTimer* timer = NULL;
		TRAPD(ret, timer = CDeltaTimer::NewL(CActive::EPriorityHigh, KCommTimerGranularity));
#ifdef	__FLOG_ACTIVE
		if (ret != KErrNone)
			{
			C32_STATIC_LOG2(KC32Detail,_L8("CommTimer::GetTimer()-creation of CDeltaTimer failed with %d. Will panic"),ret);
			}
#endif
		__ASSERT_ALWAYS(ret==KErrNone, Fault(EDTimerAllocFailure));
		tls->SetDeltaTimer(timer); //transfer ownership
		
		}
	else
		{
		C32_STATIC_LOG(KC32Detail,_L8("CommTimer::GetTimer() returning already initialized client TLS"));
		}
		
	return tls->DeltaTimer();
	}

// EOF - CS_TIME.CPP
