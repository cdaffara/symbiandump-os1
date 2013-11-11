/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TTCStateTiltIdle class declaration.
*
*/



#ifndef TCSTATETILTIDLE_H
#define TCSTATETILTIDLE_H

// INCLUDES
#include "tcstatetilt.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Idle state for tilt channel.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCStateTiltIdle ): public CTCStateTilt
    {
    public:
    
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltIdle*
        */
        static CTCStateTiltIdle* NewL( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
        
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltIdle*
        */
        static CTCStateTiltIdle* NewLC( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
    
    private: // From base classes
    
        // @see CTCStateBase
        void HandleEventL( TTCEventId aId, TTCEvent* aEvent );
    
        // @see CTCStateBase
        void HandleTransactionCompletedL( TTCTransactionId aTransactionId, TInt aError );
        
        // @see CTCStateBase
        void HandleStateEntryL();

        // @see CTCStateBase
        void HandleStateExitL();

    private:
        
        /**
        * C++ constructor
        *
        * @param aStatePool
        * @param aTransactionHandler
        */
        CTCStateTiltIdle( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
    };

#endif // TCSTATETILTIDLE_H

// End of File
