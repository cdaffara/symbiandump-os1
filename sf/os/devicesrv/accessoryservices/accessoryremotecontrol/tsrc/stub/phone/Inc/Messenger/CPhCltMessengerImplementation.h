/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the PhCltExtension Messenger interface.
*
*/



#ifndef CPhCltMessengerIMPLEMENTATION_H
#define CPhCltMessengerIMPLEMENTATION_H


//  INCLUDES
#include    <CPhCltMessenger.h>
#include    "RPhCltMessenger.h"
#include    "RPhCltServer.h"

/**
*  Implementation of the CPhCltMessenger interface.
*
*
*  @lib PhoneClient.lib
*  @since 3.2
*/
NONSHARABLE_CLASS( CPhCltMessengerImplementation )
:   public CPhCltMessenger
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPhCltMessengerImplementation* NewL( const TUid& aCategoryUid,
            const TInt& aDefaultMessageSize = KPhCltDefMesSize );
        

        /**
        * Destructor.
        */
        ~CPhCltMessengerImplementation();


    public: // Functions from base classes

        /**
        * @see CPhCltMessenger::Receive().
        */
        void Receive(
            TRequestStatus& aStatus,
            TInt& aMessageSize,
            TDes8& aMessagePayload );


        /**
        * @see CPhCltMessenger::Send().
        */
        void Send(
            TRequestStatus& aStatus,
            const TDesC8& aMessagePayload );


        /**
        * @see CPhCltMessenger::CancelRequest().
        */
        void CancelRequest( const TPhCltMessengerRequest& aRequest );


        /**
        * @see CPhCltMessenger::Skip().
        */
        void Skip();


    private:

        /**
        * C++ constructor.
        */
        CPhCltMessengerImplementation();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TUid& aCategoryUid,
            const TInt& aDefaultMessageSize );

    private:    // Data

        // The messenger to perform requests.
        RPhCltMessenger    iMessenger;
        
        //RPhCltServer class instance creates phonesession
        RPhCltServer iServer;
    };


#endif      // CPhCltMessengerIMPLEMENTATION_H

  
// End of File
