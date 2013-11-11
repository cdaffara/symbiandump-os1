/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Property handler
*
*/


#ifndef CSENSRVPROPERTYHANDLER_H
#define CSENSRVPROPERTYHANDLER_H


#include <e32base.h>
#include "sensrvclient.h"
#include "sensrvchannelinfo.h"

class MSensrvPropertyHandlerObserver;

/**
 *  Property handler class
 *
 *  @lib SensorClient.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CSensrvPropertyHandler ) : public CActive
    {
    public:

        static CSensrvPropertyHandler* NewL( RSensrvClient& aSensrvClient, const TSensrvChannelInfo& aChannelInfo );

        static CSensrvPropertyHandler* NewLC( RSensrvClient& aSensrvClient, const TSensrvChannelInfo& aChannelInfo );

        /**
        * Destructor
        */
        virtual ~CSensrvPropertyHandler();        
    
    public:
    
        /**
        * Start property listening. Send request to server.
        * 
        * @since S60 5.0
        * @param aPropertyListener Property listener for channel.
        */
        void StartListening( MSensrvPropertyHandlerObserver* aPropertyObserver );
        
    protected: // From CActive

          void DoCancel();
          void RunL();
          TInt RunError(TInt aError);

    private:

        /**
        * Constructor
        */
        CSensrvPropertyHandler( RSensrvClient& aSensrvClient,  const TSensrvChannelInfo& aChannelInfo  );

        /**
        * Create and send request to server
        */
        void CreateAndSendRequest();


    private: // data
        
        /**
        * Client
        */
        RSensrvClient& iSensrvClient;
        
        /**
        * The channel information
        */
        TSensrvChannelInfo iChannelInfo;
        
         /**
         * Property listener
         * Not own.         
         */
        MSensrvPropertyHandlerObserver* iPropertyObserver;       
       
        /**
        * Server writes changed property data here.
        */
        TPropertyPckgBuf iPropertyPckgBuf;
        
        /**
        * Error count
        */
        TInt iErrorCount;
    };
    
#endif // CSENSRVPROPERTYHANDLER_H
