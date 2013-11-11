// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: An CTimer derived active object to simulate the delay in the response 
// to a reference location request from the SuplProxyProtocolModule.

#ifndef DELAYTIMER_H
#define DELAYTIMER_H

#include <e32base.h>

/*
 * For callback from CTe_DelayTimer to CTe_LbsRefLocationSource.
 */
class MTimerCallback
    {
public:
    virtual void CompleteCallback()=0;
    };

/*
 * Timer to delay the update to the SPPM.
 */
class CTe_DelayTimer: public CTimer
    {
    
public:
    static CTe_DelayTimer* NewL(MTimerCallback& aObserver);
    ~CTe_DelayTimer();
    void Start();
    void SetDelay(const TTimeIntervalMicroSeconds32 aDelay);
protected: 
    // from CActive
    void RunL();
    TInt RunError(TInt aError);
private:
    CTe_DelayTimer(MTimerCallback& aGatewayCallback);
    void ConstructL();
private:
    TTimeIntervalMicroSeconds32 iDelay;
    MTimerCallback& iObserver;
    };


#endif //DELAYTIMER_H
