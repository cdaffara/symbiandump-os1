/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef LCF_COMMON_PROCEDURES_H
#define LCF_COMMON_PROCEDURES_H

//  INCLUDES
#include <lbs.h>

NONSHARABLE_CLASS(CLcfCommonProcedures) : public CBase
    {
    public:
        static CLcfCommonProcedures* NewL();
        ~CLcfCommonProcedures();
        
        void FlushEventsL();

        void AsyncWait(TRequestStatus& aStatus);
        void FlushEvents();

        TInt GetHeapUsageL(TInt& aHeapSize);
        
    private:
        // Constructor
        CLcfCommonProcedures();
        
        virtual void ConstructL();
        
        static TInt TimerCallback(TAny* aAny);
        
    private:
        
        static TInt IdleCallback(TAny *aThisObj);
        void StopWaiting();
        
        CActiveSchedulerWait iWait;
        
        CIdle*  iIdle;
        
        CPeriodic* iTimer;
    };		
#endif // LCF_COMMON_PROCEDURES_H
