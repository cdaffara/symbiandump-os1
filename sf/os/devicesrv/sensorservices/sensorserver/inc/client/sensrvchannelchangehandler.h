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
* Description:  Channel change handler
*
*/


#ifndef CSENSRVCHANNELCHANGEHANDLER_H
#define CSENSRVCHANNELCHANGEHANDLER_H


#include <e32base.h>
#include "sensrvclient.h"
#include "sensrvchannelinfo.h"

class MSensrvChannelChangeHandlerObserver;

/**
 *  Channel Change handler class
 *
 *  @lib sensrvclient.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CSensrvChannelChangeHandler ) : public CActive
    {
    public:

        static CSensrvChannelChangeHandler* NewL( RSensrvClient& aSensrvClient );

        static CSensrvChannelChangeHandler* NewLC( RSensrvClient& aSensrvClient );

        /**
        * Destructor
        */
        virtual ~CSensrvChannelChangeHandler();        
    
    public:
    
        /**
        * Start channel change listening. Send request to server.
        * 
        * @since S60 5.0
        * @param aChannelChangeObserver Channel change observer
        * @param aSearchParameters Search parameters.
        */
        void StartListening( MSensrvChannelChangeHandlerObserver* aChannelChangeObserver,
                             const TSensrvChannelInfo& aSearchParameters );
        
    protected: // From CActive

          void DoCancel();
          void RunL();
          TInt RunError(TInt aError);

    private:

        /**
        * Constructor
        */
        CSensrvChannelChangeHandler( RSensrvClient& aSensrvClient );

        /**
        * Create and send request to server
        */
        void CreateAndSendRequest();


    private: // data
        
        /**
        * Client
        * Not own.
        */
        RSensrvClient& iSensrvClient;
        
        /**
        * Observer
        * Not own.         
        */
        MSensrvChannelChangeHandlerObserver* iObserver;       
       
        /**
        * Server writes channel info of new or removed channel here.
        */
        TSensrvChannelInfoPckgBuf iChannelInfoPckgBuf;
        
        /**
        * Server writes type of change here.
        */
        TSensrvTSensrvChannelChangeTypePckgBuf iChangeTypePckgBuf;
        
        /**
        * Error count
        */
        TInt iErrorCount;
        
        /**
        * Stores the current search parameters.
        */
        TSensrvChannelInfoPckgBuf iSearchParameterBuf;
    };
    
#endif // CSENSRVCHANNELCHANGEHANDLER_H
