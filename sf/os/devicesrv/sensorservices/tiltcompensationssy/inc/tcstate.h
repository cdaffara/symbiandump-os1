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
* Description:  TTCState class declaration.
*
*/



#ifndef TCSTATE_H
#define TCSTATE_H

// INCLUDES
#include <e32base.h>

#include "tcevent.h"
#include "tctransaction.h"

// FORWARD DECLARATIONS
class MTCStatePool;
class MTCTransactionHandler;

// CLASS DECLARATION

/**
*  State base class.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCState ): public CBase
    {
    public:
    
        /**
        * Tilt compensation SSY unique state type.
        * Invidual channels can have own states. Use this ID
        * to sepserate states from each others.
        */
        enum TCStateType
            {
            // UID for tilt channel states
            ETCStateTypeTilt = 0x01
            };
    
        /**
        * Tilt compensation SSY states.
        */
        enum TCStateId
            {
            ETCStateIdle,
            ETCStateOpen,
            ETCStateListenData,
            ETCStateBufferFull,
            ETCStateClosing,
            ETCStateLast
            };
            
    public:
    
        /**
        * Return state ID.
        * 
        * @return TTCStateId.
        */
        TCStateId Id() const;
        
        /**
        * The type of the state.
        * 
        * @return TCStateType
        */
        TCStateType Type() const;

    public:
    
        /**
        * State is requested to handle an event.
        * Event ID is mandatory, event data can be NULL.
        * 
        * @param aId event id
        * @param aEvent
        */
        virtual void HandleEventL( TTCEventId aId,
            TTCEvent* aEvent = NULL ) = 0;
    
        /**
        * Completed transactions are notified through here.
        * 
        * @parama TransactionId the transaction id
        * @param aError Symbian error codes
        * @return
        */
        virtual void HandleTransactionCompletedL(
            TTCTransactionId aTransactionId,
            TInt aError ) = 0;
    
        /**
        * Handle state entry.
        * This method is called when the state is entered.
        */
        virtual void HandleStateEntryL() = 0;

        /**
        * Handle state exit.
        * This method is called when the state is exit.
        */
        virtual void HandleStateExitL() = 0;

    protected: // New methods
    
        /**
        * Set a bit flag on.
        *
        * @param aFlag Bit flag to set.
        */
        void Set( TUint32 aFlag );
    
        /**
        * Unset a bit flag.
        *
        * @param aFlag Bit flag to unset.
        */
        void Unset( TUint32 aFlag );

        /**
        * Checks if specified bit flag is on.
        *
        * @param aFlag Bit flag to check.
        * @return ETrue if set.
        */
        TBool IsSet( TUint32 aFlag );
    
        /**
        * Clears all the bit flags.
        */
        void Clear();

    protected:
    
        /**
        * Default C++ constructor.
        *
        * @param aType
        * @param aId
        * @param aStatePool
        * @param aTransactionHandler
        */
        CTCState( TCStateType aType,
            TCStateId aId,
            MTCStatePool& aStatePool,
            MTCTransactionHandler& aTransactionHandler );
        
    protected: // Data
    
        /** State channel UID */
        TCStateType iType;
    
        /** State ID */
        TCStateId iId;
        
        /** Reference to state pool */
        MTCStatePool& iStatePool;
        
        /** Reference to transaction handler */
        MTCTransactionHandler& iTransactionHandler;
        
        /** Bit flag for internal state handling */
        TUint32 iFlags;
    };
    
/** State array */
typedef RPointerArray<CTCState> RStateArray;

#endif // TCSTATE_H

// End of File
