/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer for implementing mediator service command timeout.
*
*/

#ifndef MEDIATORCOMMANDTIMER_H
#define MEDIATORCOMMANDTIMER_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  An observer interface for receiving command timeout.
*
*  @since S60 3.1
*/
class MMediatorTimerCallback
    {
    public: // Timer callback

        /**
        * Timer has completed
        *
        * @since S60 3.1        
        * @param aDomain    domain of the expired command
        * @param aCategory  category of the expired command
        * @param aCommandId id of the expired command
        * @return None.
        */
        virtual void TimerCallBack( TUid aDomain, 
                                    TUid aCategory, 
                                    TInt aCommandId ) = 0;
    };


/**
*  Timer class for command handling timeouts.
*
*  @lib MediatorServer
*  @since S60 3.1
*/
class CMediatorCommandTimer : public CTimer
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMediatorCommandTimer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMediatorCommandTimer();

    public: // New functions
        
        /**
        * Starts timer with specified interval
        * @since S60 3.1
        * @param aCallBack  observer for command expiration
        * @param aDomain    domain of the command
        * @param aCategory  category of the command
        * @param aCommandId id of the command
        * @param aInterval  specifies timer interval. 
        *                   Milliseconds, so multiplied by 1000
        * @return TInt indicating the start status
        */
        TInt StartTimer( MMediatorTimerCallback* aCallBack,
                         TUid aDomain, 
                         TUid aCategory, 
                         TInt aCommandId, 
                         TInt aInterval );

    private: // From CTimer (CActive)

        /**
        * From CActive RunL.
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void RunL();
        
    private:

        /**
        * C++ default constructor.
        */
        CMediatorCommandTimer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        
        // Callback pointer for timer event
        MMediatorTimerCallback* iCallBack; 
        
        // Domain
        TUid    iDomain;
        // Category
        TUid    iCategory;
        // Command
        TInt    iCommandId;
    };

#endif      // MEDIATORCOMMANDTIMER_H   
            
// End of File
