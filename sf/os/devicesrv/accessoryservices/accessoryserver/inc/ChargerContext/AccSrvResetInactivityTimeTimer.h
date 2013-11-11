/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer for calling User::ResetInactivityTimer() function frequently.
*
*/



#ifndef C_ACCSRVRESETINACTIVITYTIMETIMER_H
#define C_ACCSRVRESETINACTIVITYTIMETIMER_H



#include <e32base.h>


#include "ResetInactivityTimeTimerObserver.h"


/**  Timer value must be lower than keylock timers minimun value, i.e. < 5 seconds */
const TInt KResetInactivityTimerValueDefault    = 4800000;


/**
 *  Timer class for calling User::ResetInactivityTime() frequently.
 *  
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS(  CAccSrvResetInactivityTimeTimer ): public CTimer
    {

public:


    /**
    * Two-phased constructor.
    * @param aResetInactivityTimeTimerObserver Observer callback interface.
    */
    static CAccSrvResetInactivityTimeTimer* NewL(
            MResetActivityTimeTimerObserver* aResetInactivityTimeTimerObserver );

    /**
    * Destructor.
    */
    virtual ~CAccSrvResetInactivityTimeTimer();

    /**
     * Starts the timer.
     * @since S60 5.1
     * @param aTime Time out value.
     * @return void
     */
     void StartTimer( TInt aTime = KResetInactivityTimerValueDefault );


protected:

    /**
     * From CActive.
     * Handles an active object's request completion event.
     *
     * @since S60 5.1
     * @param void
     */
   void RunL();
    
   /**
    * From CActive.
    * Handles RunL's leave cases
    *
    * @since S60 5.1
    * @param aError Error code.
    */
   TInt RunError( TInt aError );


private:

    /**
    * C++ default constructor.
    * @param aResetInactivityTimeTimerObserver Observer callback interface.
    */
    CAccSrvResetInactivityTimeTimer(
    MResetActivityTimeTimerObserver* aResetInactivityTimeTimerObserver );

    /**
     * By default Symbian OS constructor is private.
     * @param none.
     */
    void ConstructL();


private: // data


    /**
     * TimeOut time
     */
    TInt iTime; // 

    /**
     * Pointer to ResetInactivityTimeTimerObserver object.
     * Not own.
     */
    MResetActivityTimeTimerObserver* iResetInactivityTimeTimerObserver;

    };


#endif // C_ACCSRVRESETINACTIVITYTIMETIMER_H
