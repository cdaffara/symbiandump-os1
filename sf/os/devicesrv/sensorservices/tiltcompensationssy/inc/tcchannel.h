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
* Description:  CTCChannel class declaration.
*
*/



#ifndef TCCHANNEL_H
#define TCCHANNEL_H

// INCLUDES
#include <e32base.h>
#include <sensrvchannelinfo.h>
#include <sensrvdatalistener.h>
#include <ssypropertyprovider.h>
#include <ssychanneldataprovider.h>
#include <sensrvtypes.h>

#include "tcstatepool.h"
#include "tctransactionhandler.h"
#include "tcstate.h"

// FORWARD DECLARATIONS
class MSsyCallback;

// CLASS DECLARATION

/**
*  Base class for a channel handler. Base class has the basic
*  implementation support for the state machine.
*
*  Channel supports following states:
*  - Idle
*  - Open
*  - Listen data
*  - Buffer full
*  - Closing
*
*  @lib None.
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCChannel ): public CActive,
    public MSsyPropertyProvider,
    public MSsyChannelDataProvider,
    public MTCStatePool,
    public MTCTransactionHandler
    {
    public:
    
        // Default C++ destructor
        ~CTCChannel();
    
    public: // New methods
    
        /**
        * Updates correct channel ID received from sensor server.
        * 
        * @param aId Channel ID.
        * @return None.
        */
        virtual void SetChannelId( TSensrvChannelId aId );
        
        /**
        * Returns channel ID.
        * 
        * @return TSensrvChannelId.
        */
        virtual TSensrvChannelId ChannelId() const;
        
        /**
        * Checks if the channel is supported.
        * 
        * @param aInfo Sensor channel info.
        * @return ETrue if supported.
        */
        virtual TBool ChannelSupported( const TSensrvChannelInfo& aInfo );

        /**
        * Registers a channel supported by this channel instance.
        *
        * @param aChannels Channel list.
        * @return None.
        */
        virtual void RegisterChannelL( RSensrvChannelInfoList& aChannels ) = 0;
        
        /**
        * Open the channel.
        */
        virtual void OpenChannelL() = 0;

        /**
        * Close the channel.
        */
        virtual void CloseChannelL() = 0;

    protected: // Methods from base classes
    
        // @see CActive
        void DoCancel();
        
        // @see CActive
        TInt RunError( TInt aError );
        
        // @see MTCStatePool
        CTCState* CurrentState() const;
        
        // @see MTCStatePool
        //CTCState* PreviousState() const;

        // @see MTCStatePool
        void ChangeStateL( TInt aStateId );
        
        // @see MTCChannelTransaction
        //TBool ReadyForTransaction() const;
        
        // @see MTCChannelTransaction
        TUint8* DataBuffer() const;
        
        // @see MTCChannelTransaction
        TInt DataCount() const;

    protected: // New methods
    
        /**
        * Constructor
        */
        void BaseConstructL();
        
        /**
        * Finds a sensor channel.
        *
        * @param aChannelTypeId Channel type ID.
        * @param aLocation Location of the sensor.
        * @param aVendorId Sensor vendor ID.
        * @param aChannelInfo Found channel info.
        * @return ETrue if channel found.
        */
        static TBool FindChannelL( const TSensrvChannelTypeId& aChannelTypeId,
            const TDesC8& aLocation,
            const TDesC8& aVendorId,
            TSensrvChannelInfo& aChannelInfo );

        /**
        * Register a state.
        * Only the state with aStateId can be registered.
        * 
        * @param aStateId State to register.
        * @param aStateArray Array where to add the state.
        */
        virtual void RegisterStateL( CTCState::TCStateId aStateId,
            RStateArray& aStateArray ) = 0;
            
        /**
        * Forces the current transaction to be completed.
        * 
        * @param aCode Completion code of the transaction.
        */
        virtual void CompleteTransaction( TInt aCode );

    protected:

        /**
        * Default C++ constructor.
        */
        CTCChannel( MSsyCallback& aCallback );
        
    private: // New methods
    
        // Completes pending request
        void CompleteSelf( TInt aCode );
        
    protected: // Data for inherited channels
    
        /** Channel ID */
        TSensrvChannelInfo iChannelInfo;
        
        /** SSY callback interface */
        MSsyCallback& iCallback;
        
        /** Current transaction ID */
        TTCTransactionId iCurrentTransactionId;
        
        /** Current transaction, not owned */
        TTCTransaction* iCurrentTransaction;
        
        /** Data buffer, not owned */
        TUint8* iData;
        
        /** Data buffer count */
        TInt iCount;
        
    private: // Private data for base channel
    
        /** State pool */
        RStateArray iStatePool;
        
        /** Current state pointer, not owned */
        CTCState* iCurrentState;
        
        /** Previous state pointer, not owned */
        CTCState* iPreviousState;
    };

/** Channel array */
typedef RPointerArray<CTCChannel> RChannelArray;

#endif // TCCHANNEL_H

// End of File
