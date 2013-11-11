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
* Description:  Client side access to emergency number information
*
*/



#ifndef RPHCLTEMERGENCYNUMBER_H
#define RPHCLTEMERGENCYNUMBER_H

//  INCLUDES
#include "PhCltTypes.h"
#include "RPhCltServer.h"
#include <e32std.h>

// FORWARD DECLARATIONS

/**
* Emergency number query mode
*/
enum TPhCltEmergencyNumberQueryMode
    {
    EPhCltEmergencyNumberQueryModeNormal = 0,  // Normal mode
    EPhCltEmergencyNumberQueryModeAdvanced     // advanced mode
    };

// CLASS DECLARATION

/**
*  Client side access to emergency number information
*
*  @lib phoneclient.lib
*  @since Series60_1.0
*/
class RPhCltEmergencyNumber : public RSubSessionBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C RPhCltEmergencyNumber();
      

    public:  // New functions

        /**
        * Opens the subsession.
        *
        * @param aServer It is the session of which subsession this is.
        * @return Returns error code.
        */       
        IMPORT_C TInt Open( RPhCltServer& aServer );

        /**
         * Closes the subsession.
         */       
        IMPORT_C void Close();


    public: // Emergency number API

        /**
        * Returns information whether the number specified would initiate 
        * an emergency call.
        * 
        * @param aNumber Number to be queried.
        * @param aIsEmergencyNumber If ETrue, then the aNumber parameter 
        *                           was emergency number. 
        * @param aMode Emergency number query mode, default: normal. 
        * @return Returns error code of the success of the operation.
        */
        IMPORT_C TInt IsEmergenyPhoneNumber(
            const TPhCltTelephoneNumber& aNumber, 
            TBool& aIsEmergencyNumber,
            TPhCltEmergencyNumberQueryMode aMode = 
                EPhCltEmergencyNumberQueryModeNormal );

        /**
        * Returns information whether the number specified would initiate 
        * an emergency call.
        * 
        * @param aNumber Number to be queried, matched emergency number is 
        *                 returned if a correct emergency number is found at
        *                 the last of the number.
        * @param aIsEmergencyNumber If ETrue, then the aNumber parameter 
        *                           was emergency number. 
        * @param aMode Emergency number query mode, default: normal. 
        * @return Returns error code of the success of the operation.
        */
        IMPORT_C TInt IsEmergencyPhoneNumber(
            TPhCltTelephoneNumber& aNumber, 
            TBool& aIsEmergencyNumber,
            TPhCltEmergencyNumberQueryMode aMode = 
                EPhCltEmergencyNumberQueryModeNormal );



    private: 

        // Internal cleanup support
        static void CleanupEmergencyList( TAny* aList );

    };
        
#endif // RPHCLTEMERGENCYNUMBER_H

// End of File
