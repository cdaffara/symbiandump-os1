/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client side access to emergency call
*
*/


#ifndef RPhCltEmergencyCall_H
#define RPhCltEmergencyCall_H

//  INCLUDES
#include <e32std.h>
#include "PhCltTypes.h"
#include "RPhCltServer.h"

/**
* Emergency number query mode
*/
enum TPhCltEmergencyNumberQueryMode
    {
    EPhCltEmergencyNumberQueryModeNormal = 0,  // Normal mode
    EPhCltEmergencyNumberQueryModeAdvanced     // advanced mode
    };

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Client side access to emergency call
*
*  @lib phoneclient.lib
*  @since Series60_1.0
*/
class RPhCltEmergencyCall : public RSubSessionBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        RPhCltEmergencyCall();


    public:  // New functions

        /**
        * Opens the subsession.
        *
        * @param aServer It is the session of which subsession this is.
        * @return Returns error code.
        */
        TInt Open( RPhCltServer& aServer );

        /**
         * Closes the subsession.
         */
        void Close();


    public:

        /**
        * Returns information whether the number specified would initiate 
        * an emergency call.
        * 
        * @param aNumber Number to be queried.
        *        The aNumber parameter should be a buffer and the 
        *        clients are provided with a typedef called 
        *        TPhCltEmergencyNumber which is equivalent to a 
        *        TBuf< KPhCltEmergencyNumberSize > where KPhCltEmergencyNumberSize 
        *        is defined as 6.
        * @param aIsEmergencyNumber If ETrue, then the aNumber parameter 
        *                           was emergency number. 
        * @return Returns error code of the success of the operation.
        */
        TInt IsEmergencyPhoneNumber(
            const TDesC& aNumber, 
            TBool& aIsEmergencyNumber );

        /**
        * Returns information whether the number specified would initiate 
        * an emergency call.
        * 
        * @param aNumber Number to be queried, matched emergency number 
        *                is returned if a correct emergency number is 
        *                found in the end of the number.
        *                The aNumber parameter should be a buffer and the 
        *                clients are provided with a typedef called 
        *                TPhCltTelephoneNumber which is equivalent to a 
        *                TBuf< KPhCltTelephoneNumberLength > where KPhCltTelephoneNumberLength
        *                is defined as 100.        
        * @param aIsEmergencyNumber If ETrue, then the aNumber parameter 
        *                           was emergency number. 
        * @return Returns error code of the success of the operation.
        */
        TInt FindEmergencyPhoneNumber(
            TDes& aNumber, 
            TBool& aIsEmergencyNumber );

        /**
        * Initiates an emergency call.
        *
        * @param aReqStatus       On return, KErrNone if the emergency call successfully reaches 
        *                         the connected state. If the call set-up fails, the function 
        *                         member will complete with an error.
        * @param aNumber          Emergency number to be dialed (optional).
        *                         The aNumber parameter should be a buffer and the 
        *                         clients are provided with a typedef called 
        *                         TPhCltEmergencyNumber which is equivalent to a 
        *                         TBuf<KPhCltEmergencyNumberSize> where KPhCltEmergencyNumberSize 
        *                         is defined as 6.
        *
        *                         If the aNumber parameter supplies an "empty" buffer 
        *                         the call is created either dialing the default emergency 
        *                         phone number or use the SET UP EMERGENCY request  
        *                         that does not contain a  dialling number.
        */
        void DialEmergencyCall( TRequestStatus& aReqStatus, TDesC8& aNumber );
        
        /**
        * Cancels an emergency call.
        *
        */
        void CancelDialEmergencyCall();

    };

#endif // RPhCltEmergencyCall_H

// End of File
