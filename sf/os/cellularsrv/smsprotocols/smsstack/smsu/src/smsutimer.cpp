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
// Implements CSmsuTimeout
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsutimerTraces.h"
#endif

#include "SmsuTimer.h"

#include <e32std.h>

#include "smsumain.h"
#include "smsstacklog.h"

/**
 *  2 phase contructor
 *  
 *  @param aActive Reference to an active observer
 */
CSmsuTimeout* CSmsuTimeout::NewL(CSmsuActiveBase& aActive)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUTIMEOUT_NEWL_1, "CSmsuTimeout::NewL()");
    
    CSmsuTimeout* self = new(ELeave) CSmsuTimeout(aActive);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 *  D'tor
 */
CSmsuTimeout::~CSmsuTimeout()
	{
	Cancel();
	}


/**
 *  Start a timeout specified in aTimeIntervalMicroSeconds32
 */
void CSmsuTimeout::Start(const TTimeIntervalMicroSeconds32& aTimeIntervalMicroSeconds32)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUTIMEOUT_START_1, "CSmsuTimeout::Start()");

	iTimedOut = EFalse;
	After(aTimeIntervalMicroSeconds32);
	} // CSmsuTimeout::Start


/**
 *  C'tor
 */
CSmsuTimeout::CSmsuTimeout(CSmsuActiveBase& aActive)
  : CTimer(aActive.Priority()),
	iActive(aActive),
	iTimedOut(EFalse),
	iCompleteMyselfRequested(EFalse)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUTIMEOUT_CTOR_1, "CSmsuTimeout::CSmsuTimeout()");

	CActiveScheduler::Add(this);
	} // CSmsuTimeout::CSmsuTimeout


/**
 *  Timer completed - cancel the observer
 */
void CSmsuTimeout::RunL()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSUTIMEOUT_RUNL_1, "CSmsuTimeout::RunL [iStatus=%d]", iStatus.Int() );
	iTimedOut = ETrue;
	iActive.Cancel();

	//
	// It would be nice to call a DoTimeout() function here,
	// and give the client the option of deciding what to do
	// next, but that would be a BC break.
	//
	// So instead, we can do the next best thing. If the client
	// calls CSmsuActiveBase::CompleteMyselfAfterTimeout() after
	// a timeout (but within the DoCancel() function) then
	// we now call the complete method - thereby restarting
	// the active object outside of the Cancel().
	//
	if (iCompleteMyselfRequested)
		{
		iActive.CompleteMyself(iCompleteMyselfStatus);
		iCompleteMyselfRequested = EFalse;
		}
	} // CSmsuTimeout::RunL


void CSmsuTimeout::RequestCompleteMyselfAfterCancel(TInt aStatus)
    {
    // Ignore in code coverage - not used within the SMS stack.
    BULLSEYE_OFF
    __ASSERT_DEBUG(iCompleteMyselfRequested == EFalse, SmsuPanic(ESmsuCompleteMyselfAlreadyRequested));
    
    iCompleteMyselfRequested = ETrue;
    iCompleteMyselfStatus    = aStatus;
    BULLSEYE_RESTORE
    }
