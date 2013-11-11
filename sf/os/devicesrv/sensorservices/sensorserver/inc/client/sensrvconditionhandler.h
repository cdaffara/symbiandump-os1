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
* Description:  Data handler
*
*/


#ifndef CSENSRVCONDITIONHANDLER_H
#define CSENSRVCONDITIONHANDLER_H


#include <e32base.h>

#include "sensrvclient.h"


class MSensrvConditionHandlerObserver;

/**
 *  Condition handler class
 *
 *  @lib SensorClient.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CSensrvConditionHandler ) : public CActive
    {
    public:


        static CSensrvConditionHandler* NewL( RSensrvClient& aSensrvClient, 
                                              const TSensrvChannelInfo& aChannelInfo );

        static CSensrvConditionHandler* NewLC( RSensrvClient& aSensrvClient, 
                                               const TSensrvChannelInfo& aChannelInfo );

        virtual ~CSensrvConditionHandler();


    public:
        /*
        * Start condition listening. Send request to server.
        *
        * @since S60 5.0
        * @param aConditionObserver Condition listener for channel.
        * @param aListeningParameters Listening parameters
        */
        void StartListening( MSensrvConditionHandlerObserver* aConditionObserver,
                             TSensrvStartListeningParameters aListeningParameters );


        /**
        * Blocks sending async message.
        *
        * @since S60 5.0
        */
        inline void BlockAsyncMessage() { iListening = EFalse; };


    protected: // From CActive

          void DoCancel();
          void RunL();
          TInt RunError(TInt aError);


    private:

        CSensrvConditionHandler( RSensrvClient& aSensrvClient,  const TSensrvChannelInfo& aChannelInfo  );

        void ConstructL();

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
        * Condition listener
        * Not own.
        */
        MSensrvConditionHandlerObserver* iConditionObserver;

        /**
        * Channel info
        */
        TSensrvChannelInfo iChannelInfo;

        /**
        * Error count
        */
        TInt iErrorCount;
        
        /**
        * Listening parameters
        */
        TSensrvStartListeningParameters iListeningParameters;

        /**
        * Condition set id that was met is returned in this buffer.
        */
        TSensrvTIntPckgBuf iMetSetIdBuf;

        /**
        * Server writes the data value that was met here
        * Own.
        */
        HBufC8* iWriteBuffer;

        /**
        * Pointer to iWriteBuffer data
        * Own
        */
        TPtr8* iWriteBufferPtr;
        
        /**
        * Indicates logical listening status
        */
        TBool iListening;
    };



#endif // CSENSRVCONDITIONHANDLER_H
