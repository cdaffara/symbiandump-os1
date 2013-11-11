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
//

#ifndef ENDSTATETIMER_H
#define ENDSTATETIMER_H

#include <e32base.h>

class MTimerExpired
	{
public:
	virtual void EndStateTimerExpired(TInt aError) = 0;
	};

class CEndStateTimer : public CTimer
    {
public:
    CEndStateTimer(MTimerExpired& aObserver);
    ~CEndStateTimer();
    static CEndStateTimer* NewL(MTimerExpired& aObserver);
    void Start(TReal aTimeOutSecs);
     
public: //from CActive
    void RunL();
    void DoCancel();
        
protected:                    
    void ConstructL();   
private:
    MTimerExpired& iTimerObserver;    
    };

#endif
