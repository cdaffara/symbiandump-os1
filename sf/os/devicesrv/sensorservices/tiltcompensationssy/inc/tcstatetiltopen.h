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
* Description:  TTCStateTiltOpen class declaration.
*
*/



#ifndef TCSTATETILTOPEN_H
#define TCSTATETILTOPEN_H

// INCLUDES
#include "tcstatetilt.h"

/**
*  Open state for tilt channel.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCStateTiltOpen ) : public CTCStateTilt
    {
    public:
    
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltOpen
        */
        static CTCStateTiltOpen* NewL( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
        
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltOpen
        */
        static CTCStateTiltOpen* NewLC( MTCStatePool& aStatePool,
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
        CTCStateTiltOpen( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
    };

#endif // TCSTATETILTOPEN_H

// End of File
