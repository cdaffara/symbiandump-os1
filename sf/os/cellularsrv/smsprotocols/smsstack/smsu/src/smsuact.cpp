// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the CSmsuActive class
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsuactTraces.h"
#endif

#include "smsuact.h"
#include "SmsuTimer.h"
#include "smsstacklog.h"
#include "smsumain.h"

/**
 *  Constructor.
 *  
 *  @param aPriority Active object priority
 *  @capability None
 */
EXPORT_C CSmsuActiveBase::CSmsuActiveBase(TInt aPriority)
: CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	} // CSmsuActiveBase::CSmsuActiveBase


/**
 *  Destructor.
 *  @capability None
 */
EXPORT_C CSmsuActiveBase::~CSmsuActiveBase()
	{
	Cancel();
	delete iSmsuTimeout;
	} // CSmsuActiveBase::~CSmsuActiveBase


/**
 *  Constructs a timer.
 *  
 *  This is required before use of TimedSetActive().
 *  @capability None
 */
EXPORT_C void CSmsuActiveBase::ConstructTimeoutL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_CONSTRUCTTIMEOUTL_1, "CSmsuActiveBase::ConstructTimeoutL()");

	iSmsuTimeout = CSmsuTimeout::NewL(*this);
	} // CSmsuActiveBase::ConstructTimeoutL


/**
 *  Activates the object, and sets it to be completed after a specified time.
 *  
 *  @param aTimeIntervalMicroSeconds32 Time after which to complete the object
 *  @capability None
 */
EXPORT_C void CSmsuActiveBase::TimedSetActive(const TTimeIntervalMicroSeconds32& aTimeIntervalMicroSeconds32)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_TIMEDSETACTIVE_1, "CSmsuActiveBase::TimedSetActive(): aTimeIntervalMicroSeconds32=%d",aTimeIntervalMicroSeconds32.Int());

	__ASSERT_DEBUG(iSmsuTimeout != NULL, SmsuPanic(ESmsuTimeoutNull));
	
	if (iSmsuTimeout != NULL)
		{
		iSmsuTimeout->Start(aTimeIntervalMicroSeconds32);
		}

	SetActive();
	} // CSmsuActiveBase::TimedSetActive


/**
 *  Cancels a TimedSetActive() request.
 *
 *  @see TimedSetActive
 */
EXPORT_C void CSmsuActiveBase::TimedSetActiveCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_TIMEDSETACTIVECANCEL_1, "CSmsuActiveBase::TimedSetActiveCancel()");

	if (iSmsuTimeout != NULL)
		{
		iSmsuTimeout->Cancel();
		}
	} // CSmsuActiveBase::TimedSetActiveCancel


/**
 *  Determines if the object has timed out.
 *
 *  @see TimedSetActive
 */
EXPORT_C TBool CSmsuActiveBase::TimedOut() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_TIMEDOUT_1, "CSmsuActiveBase::TimedOut()");

	if (iSmsuTimeout != NULL)
		{
		return iSmsuTimeout->TimedOut();
		}
	
	return EFalse;
	} // CSmsuActiveBase::TimedOut


/**
 *  Sets an asynchronous observer to which to report completion of this object.
 *  
 *  @param aStatus Asynchronous status word of the observer
 *  @capability None
 */
EXPORT_C void CSmsuActiveBase::Queue(TRequestStatus& aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_QUEUE_1, "CSmsuActiveBase::Queue()");

	__ASSERT_DEBUG(iReport==NULL, SmsuPanic(ESmsuAlreadyActive));

	aStatus=KRequestPending;
	iReport=&aStatus;
	} // CSmsuActiveBase::Queue


/**
 *  Handles the object's request completion event.
 *  
 *  If a timer has been set with TimedSetActive(), this is cancelled. DoRunL()
 *  is then called. If, after this, the object is not active, Complete() is called.
 *  @capability None
 */
EXPORT_C void CSmsuActiveBase::RunL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_RUNL_1, "CSmsuActiveBase::RunL()");

	if (iSmsuTimeout != NULL)
		{
		iSmsuTimeout->Cancel();
		}

	DoRunL();

    if (!IsActive())
    	{
        Complete(iStatus.Int());
        }
	} // CSmsuActiveBase::RunL


/**
 *  Object use complete.
 *  
 *  This is called at the end of the RunL(), if the RunL() has not reset the object
 *  to be active.
 *  
 *  If an observer has been set (see Queue()), DoComplete() is called, and that
 *  observer is signalled with the object status.
 *  
 *  @param aStatus Active object status word
 *  @capability None
 */
EXPORT_C void CSmsuActiveBase::Complete(TInt aStatus)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_COMPLETE_1, "CSmsuActiveBase::Complete(): aStatus=%d", aStatus);

	if (iReport)
		{
		DoComplete(aStatus);
		User::RequestComplete(iReport, aStatus);
		iReport = NULL;
		}
	} // CSmsuActiveBase::Complete


/**
 *  Handles leaves occurring in the RunL() function.
 *  
 *  It calls Complete().
 *  
 *  @param aError RunL() leave code
 *  @return KErrNone
 *  @capability None
 */
EXPORT_C TInt CSmsuActiveBase::RunError(TInt aError)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_RUNERROR_1, "CSmsuActiveBase::RunError(): aError=%d", aError);

	__ASSERT_DEBUG(!IsActive(), User::Invariant());

	Complete(aError);

	return KErrNone;
	} // CSmsuActiveBase::RunError


/**
 *  Signals this object.
 *  
 *  @param aStatus Status code with which to signal the object
 *  @param aSetActive True to set the object to be active
 *  @capability None
 */
EXPORT_C void CSmsuActiveBase::CompleteMyself(TInt aStatus, TBool aSetActive /* = ETrue */)
    {
    // Ignore in code coverage - not used within the SMS stack.
    BULLSEYE_OFF
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_COMPLETEMYSELF_1, "CSmsuActiveBase::CompleteMyself(): aStatus=%d, aSetActive=%d",aStatus, aSetActive);
    
    // Initialise iStatus with a pending request...
    iStatus = KRequestPending;
    
    // Set active if required...
    if (aSetActive)
        {
        SetActive();
        }

    // Complete the request...
    TRequestStatus* status = &iStatus;
    
    User::RequestComplete(status, aStatus);
    BULLSEYE_RESTORE
    }

/**
 *  This function is used to request that the active object is completed
 *  after the current timeout is handled. When a timeout occurs, the active
 *  object is cancelled which requires DoCancel() to be called. However
 *  the object is active until after the DoCancel() meaning it is not
 *  possible to make a new request. This function requests a second
 *  call to Complete() (via RunL etc.) which is done without the object
 *  being active.
 *
 *  This function can only be called when a timeout occurs, otherwise
 *  it would not get actioned.
 *  
 *  @param aStatus Status code with which to signal the object
 *
 *  @capability None
 */
EXPORT_C void CSmsuActiveBase::CompleteMyselfAfterTimeout(TInt aStatus)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSUACTIVEBASE_COMPLETEMYSELFAFTERTIMEOUT_1, "CSmsuActiveBase::CompleteMyselfAfterTimeout(): aStatus=%d", aStatus);

	__ASSERT_DEBUG(iSmsuTimeout != NULL, SmsuPanic(ESmsuTimeoutNull));
	
	if (iSmsuTimeout != NULL  &&  iSmsuTimeout->TimedOut())
		{
		iSmsuTimeout->RequestCompleteMyselfAfterCancel(aStatus);
		}
	} // CSmsuActiveBase::CompleteMyselfAfterTimeout
