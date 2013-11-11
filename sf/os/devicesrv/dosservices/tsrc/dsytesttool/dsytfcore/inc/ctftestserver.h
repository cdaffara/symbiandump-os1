/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of test server classes
*
*/


#ifndef __CTFTESTSERVER_H__
#define __CTFTESTSERVER_H__

#include <e32base.h>

class CTFFactory;


_LIT( KTFRemoteTestServerName, "TESTFRAMEWORK_REMOTESERVER" );
_LIT( KTFLocalTestServerName,  "TESTFRAMEWORK_LOCALSERVER" );

NONSHARABLE_CLASS( RTFTestServer ): public RSessionBase
    {
    public:
        /**
        * Connects to the test server
        * @param aLocal If ETrue, connects to local server. Otherwise to remote server
        * @return TInt 
        */
        TInt Connect( TBool aLocal );

        /**
        * Posts a RMessage asynchronously to test server with the given parameters
        */
        void PostMessage( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4, TRequestStatus& aStatus ) const;

        /**
        * Posts a RMessage synchronously to test server with the given parameters
        */
        TInt PostMessage( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 ) const;

        /**
        * Posts a completion message to the test server
        */
        void PostCompletionMessage( TInt aStubType, TInt aResult, TRequestStatus& aStatus ) const;

        /**
        * Posts a shutdown message to the test server
        */
        void PostShutdownMessage() const;
    };


NONSHARABLE_CLASS(  CTFTestServerWrapper ): public CActive
    {
    public:
        /**
        * Constructor
        */
        CTFTestServerWrapper( void );

        /**
        * Destructor
        */
        ~CTFTestServerWrapper( void );

        /**
        * Posts a remote message to the test server and completes the status when response is received.
        * This object is deleted when the server responds.
        */
        void PostMessageLD( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4, TRequestStatus& aStatus );

        /**
        * Posts a local message to the test server and completes the status when response is received.
        * This object is deleted when the server responds.
        */
        void PostLocalMessageL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 );

    protected:
        /**
        * Called when test server completes the message
        */
        void RunL( void );

        /**
        * Called to cancel the message passed to test server
        */
        void DoCancel( void );

    private:
        TRequestStatus* iCallerStatus;
        TInt iStubType;
        RTFTestServer iClient;
        TInt iState;
    };

    
NONSHARABLE_CLASS( CTFTestSession ): public CSession2
    {
    public:
        /**
        * Constructor
        */
        CTFTestSession( CTFFactory* aFactory );

        /**
        * Called by server to process the given message
        */
        void ServiceL( const RMessage2& aMessage );

    private:
        CTFFactory* iFactory;
    };


NONSHARABLE_CLASS( CTFTestServer ): public CServer2
    {
    public:
        /**
        * Constructor
        */
        CTFTestServer( CTFFactory* aFactory );

        /**
        * Creates a new server session
        */
        CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

    private:
        CTFFactory* iFactory;
    };


#endif
