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
* Description:  TTCStateTiltClosing class declaration.
*
*/



#ifndef TCSTATETILTCLOSING_H
#define TCSTATETILTCLOSING_H

// INCLUDES
#include "tcstatetilt.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Closing state for tilt channel.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCStateTiltClosing ): public CTCStateTilt
    {
    public:
    
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltClosing
        */
        static CTCStateTiltClosing* NewL( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
        
        /**
        * Two phased constructor.
        * 
        * @param aStatePool State pool interface.
        * @param aTransactionHandler Transaction handler interface.
        * @return CTCStateTiltClosing
        */
        static CTCStateTiltClosing* NewLC( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
    
    private: // From base classes
    
        // @see TTCStateBase
        void HandleEventL( TTCEventId aId, TTCEvent* aEvent );
    
        // @see TTCStateBase
        void HandleTransactionCompletedL( TTCTransactionId aTransactionId, TInt aError );

        // @see TTCStateBase
        void HandleStateEntryL();

        // @see TTCStateBase
        void HandleStateExitL();

    private:
    
        /**
        * C++ constructor
        *
        * @param aStatePool
        * @param aTransactionHandler
        */
        CTCStateTiltClosing( MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
    };

#endif // TCSTATETILTCLOSING_H

// End of File
