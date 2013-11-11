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


#ifndef CSENSRVDATAHANDLER_H
#define CSENSRVDATAHANDLER_H


#include <e32base.h>

#include "sensrvclient.h"


class MSensrvDataHandlerObserver;

/**
 *  Data handler class
 *
 *  @lib SensorClient.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CSensrvDataHandler ) : public CActive
    {
    public:


        static CSensrvDataHandler* NewL( RSensrvClient& aSensrvClient, const TSensrvChannelInfo& aChannelInfo );

        static CSensrvDataHandler* NewLC( RSensrvClient& aSensrvClient, const TSensrvChannelInfo& aChannelInfo );

        virtual ~CSensrvDataHandler();


    public:
        /**
        * Start data listening. Send request to server.
        *
        * @since S60 5.0
        * @param aDataListener Data listener for channel.
        * @param aListeningParameters Listening parameters
        * @pararm aMaxBufferSize Maximum allowed buffer size
        */
        void StartListeningL( MSensrvDataHandlerObserver* aDataListener,
                             TSensrvStartListeningParameters aListeningParameters,
                             TInt aMaxBufferSize );

        /**
        * Get next data item.
        *
        * @since S60 5.0
        * @param aDataPtr Reference to pointer data to be written
        * @return TInt KErrNotFound if data buffer is empty.
        */
        TInt GetData( TDes8& aDataPtr );


    protected: // From CActive

          void DoCancel();
          void RunL();
          TInt RunError(TInt aError);


    private:

        CSensrvDataHandler( RSensrvClient& aSensrvClient,  const TSensrvChannelInfo& aChannelInfo  );

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
         * Data listener
         * Not own.
         */
        MSensrvDataHandlerObserver* iDataObserver;

        /**
         * Data listening parameters
         */
        TSensrvStartListeningParameters iListeningParameters;

        /**
         * Channel info
         */
        TSensrvChannelInfo iChannelInfo;

        /**
         *  Server write data to here
         * Own
         */
        HBufC8* iWriteBuffer;

        /**
         * Pointer to iWriteBuffer data
         * Own
         */
        TPtr8*  iWriteBufferPtr;

        /**
         * Client read data from here
         * Own
         */
        HBufC8* iReadBuffer;

        /**
         * Read buffer count
         */
        TInt iReadBufferCount;

        /**
         * Counter for data getting
         */
        TInt iGetDataCount;

        /**
         * Data count package
         */
        TSensrvAsyncChannelDataCountsRetvalPckgBuf iDataCountsPckgBuf;

        /**
         * Error count
         */
        TInt iErrorCount;

    };



#endif // CSENSRVDATAHANDLER_H
