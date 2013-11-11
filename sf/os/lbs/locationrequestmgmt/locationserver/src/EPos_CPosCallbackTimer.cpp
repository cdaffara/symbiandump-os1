// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include  "EPos_CPosCallbackTimer.h"

// ================= MEMBER FUNCTIONS =======================

/**
 * C++ default constructor.
 * @param aCallBack 
 */
CPosCallbackTimer::CPosCallbackTimer(TCallBack& aCallBack)
:   CTimer(EPriorityStandard),
    iTimeoutCallBack(aCallBack)    
    {
    CActiveScheduler::Add(this);
    }
/**
 * Symbian constructor.
 */
void CPosCallbackTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

/**
 * Two-phased constructor.
 * @param aCallBack 
 */
CPosCallbackTimer* CPosCallbackTimer::NewL(TCallBack& aCallBack)
    {
    CPosCallbackTimer* self = new (ELeave) CPosCallbackTimer(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Destructor.
 */
CPosCallbackTimer::~CPosCallbackTimer()
    {
    }

/**
 * Start the callback timer. If there is already a timer running, it
 * will be restarted.
 *
 * @param aTimeout The timeout.
 */
void CPosCallbackTimer::StartTimer(const TTimeIntervalMicroSeconds& aTimeOut)
    {    
    Cancel();    

    // MaxInt64 = MaxInt32 * MaxUint32
    // up to MaxUint32 internal rounds by MaxInt32 are possible before final 
    // timeout of up to MaxInt32

    iNumInternalRounds = static_cast<TUint32>(aTimeOut.Int64() / KMaxTInt32);
    iTimeOut = static_cast<TInt32>(aTimeOut.Int64() % KMaxTInt32);
    // iTimeOut can be also zero. It is catered for in NextRound

    NextRound();
    }

/**
 *  Activates timer for next period: full round (KMaxTInt32) or 
 *  small delay stored in iTimeOut
 */
TBool CPosCallbackTimer::NextRound()
    {
    if (iNumInternalRounds > 0)
        {
        After(KMaxTInt32);
        iNumInternalRounds--;
        return ETrue;           // timer not yet completed
        }
    else if (iTimeOut.Int() > 0)
        {
        After(iTimeOut);
        iTimeOut = 0;           // timer will complete on next round
        return ETrue;           // timer not yet completed
        }
    else
        {
        return EFalse;          // timer completed
        }
    }

/**
 * From CActive
 */
void CPosCallbackTimer::RunL()
    {    
    if (!NextRound())           // timer completed?
        {
        if (iStatus.Int() == KErrNone)
            {
            iStatus = KErrTimedOut;
            }
        iTimeoutCallBack.CallBack();
        }    
    }

/**
 * From CActive
 */
TInt CPosCallbackTimer::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

/**
 * Returns the request status of this callback timer.
 * 
 * @return Returns KErrTimedOut if timer elapsed or KErrCancel
 *         if timer was cancelled. Otherwise any of the system wide
 *         error codes.
 */
TInt CPosCallbackTimer::Status()
    {
    return iStatus.Int();
    }

void CPosCallbackTimer::ExtendTimeout(const TTimeIntervalMicroSeconds& aAdditionalTime)
	{
	if(!IsActive())
		{
		return;
		}
	//We do not need to cancel here. Just correct the iNumInternalRounds and iTimeOut
	
	TInt64 timeout = iNumInternalRounds * KMaxTInt32 + iTimeOut.Int() + aAdditionalTime.Int64();
	iNumInternalRounds = static_cast<TUint32>(timeout / KMaxTInt32);
	iTimeOut = static_cast<TInt32>(timeout % KMaxTInt32);
	}

//  End of File  
