/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Encapsulates an active object used in notify services.
*
*/


#ifndef CPHCLTUSSDREQUESTHANDLER_H
#define CPHCLTUSSDREQUESTHANDLER_H

// INCLUDES
#include <e32base.h>
#include "RPhCltUssd.h"
#include "RPhCltServer.h"

// FORWARD DECLARATIONS
class MPhCltUssdRequestHandler;

// CLASS DECLARATION

/**
*  Encapsulates an active object used in notify services.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
NONSHARABLE_CLASS( CPhCltUssdRequestHandler ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aHandler Handler for the USSD messages.
        * @param aPriority Priority to be used.
        * @return Pointer to created CPhCltUssdRequestHandler instance.
        */        
        static CPhCltUssdRequestHandler* NewL( 
            MPhCltUssdRequestHandler& aHandler,
            TInt aPriority );
        
        /**
        * Destructor.
        */
        ~CPhCltUssdRequestHandler();


    public:

        /**
        * Send string.
        *
        * @param aMsgData 7-bit data.
        * @param aMsgAttribute Definitions.
        */
        void SendUssd( 
            const TDesC8& aMsgData, 
            const TDesC8& aMsgAttribute );

        /**
        * Cancel send.
        */
        void SendUssdCancel();
        
        /**
        * USSD Client
        *                 
        * @return Reference to RPhCltUssd instance.
        */  
        RPhCltUssd& UssdClient();
        

    private: // From base classes
        
        /*
        * From CActive. Called when send is done.
        */
        void RunL();

        /**
        * From CActive. Called when send is cancelled.
        */
        void DoCancel();


    private:

        /**
        * C++ constructor.
        */
        CPhCltUssdRequestHandler( 
            MPhCltUssdRequestHandler& aHandler,
            TInt aPriority );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        

    private:    // Data

        // The handler for requests.
        MPhCltUssdRequestHandler& iHandler;

        // Server connection to be used to perform functionality.
        RPhCltServer iPhClient;

        // USSD subsession to server side.
        RPhCltUssd iUssdClient;
    };

#endif // CPHCLTUSSDREQUESTHANDLER_H

// End of File
