/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ASY Proxy Handler
*
*/



#ifndef CACCSRVASYPROXYHANDLER_H
#define CACCSRVASYPROXYHANDLER_H

// INCLUDE FILES
#include "AccSrvHandlerBase.h"
#include "AccSrvNotificationQueue.h"
#include <AsyCmdTypes.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvConnectionController;
class CAccPolAccessoryPolicy;

// CLASS DECLARATION

/**
*  ASY transaction related tasks are handled here.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvASYProxyHandler )
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvASYProxyHandler* NewL( CAccSrvConnectionController* aConnectionController );

        /**
        * Destructor.
        */
        virtual ~CAccSrvASYProxyHandler();

    public: // New functions

        /**
        * Starts command handling towards ASY.
        * @since S60 3.1
        * @param aCmdId      Defines the ASY command in question.
        * @param aASYCommand Data for ASY command.
        * @param aPtrBuf Pointer to CBufFlat buffer where CAccPolObjectCon object
        *                is externalized to.
        * @return Transaction ID assigned to this operation.
        */
        TInt HandleASYCommsL( TProcessCmdId aCmdId,
                              TASYCommandParamRecord& aASYCommand,
                              TPtr8* aPtrBuf=NULL );

        /**
        * Removes data related to transaction towards ASY.
        * Called when response to ASY request is received.
        *
        * @since S60 3.1
        * @param aTransactionID Transaction in question.
        * @return void
        * @see StoreTransactionID()
        */
        void RemoveTransactionID( TInt aTransactionID );

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * Constructor with notify handling.
        */
        CAccSrvASYProxyHandler( CAccSrvConnectionController* aConnectionController );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        struct TAccSrvTrIDMatchingRecord
            {
            TInt              iUniqueID;//Unique DBID of the Generic ID
            TAccPolNameRecord iNameRecord;
            TInt64            iCommand;
            TInt              iTransactionID;
            };

        /**
        * Stores data related to transaction towards ASY.
        * Used internally to filter ASY commands.
        *
        * @since S60 3.1
        * @param aCmdId      Defines the ASY command in question.
        * @param aASYCommand Data for ASY command.
        * @param aPtrBuf Pointer to CBufFlat buffer where CAccPolObjectCon object
        *                is externalized to.
        * @return Transaction ID assigned to this operation.
        * @see RemoveTransactionID
        */
        TInt StoreTransactionIDL( TProcessCmdId aCmdId,
                                 TASYCommandParamRecord& aASYCommand,
                                 TPtr8* aPtrBuf );

        /**
        * Generate a new unique transaction id for ASY transaction handling.
        *
        * @since S60 3.1
        * @return New transaction id.
        */
        TInt GenerateNewTransactionID();


    public:     // Data

    protected:  // Data

    private: //Data

        //Handle to Connection Controller
        CAccSrvConnectionController* iConnectionController; //not owned

        //Ongoing transactions towards ASY are stored here to be able
        //to use existing Transaction ID for filtered ASY commands
        RArray<TAccSrvTrIDMatchingRecord> iOngoingTransactions;

        //Transaction id for ASY communication
        TInt iTransactionId;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // CACCSRVASYPROXYHANDLER_H

// End of File
