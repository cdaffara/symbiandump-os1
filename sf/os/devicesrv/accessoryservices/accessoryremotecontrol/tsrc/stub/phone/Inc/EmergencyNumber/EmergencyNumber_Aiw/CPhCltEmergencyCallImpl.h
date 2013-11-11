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


#ifndef CPHCLTEMERGENCYCALLIMPL_H
#define CPHCLTEMERGENCYCALLIMPL_H

//  INCLUDES
#include <e32std.h>
#include "RPhCltServer.h"
#include "CPhCltEmergencyCall.h"
#include "RPhCltEmergencyCall.h"


// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Client side access to emergency call
*
*  @lib phoneclient.lib
*  @since Series60_1.0
*/
NONSHARABLE_CLASS( CPhCltEmergencyCallImpl ) : public CPhCltEmergencyCall
    {
    public: // Constructors and destructors.

        static CPhCltEmergencyCallImpl* NewL( 
            MPhCltEmergencyCallObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CPhCltEmergencyCallImpl();

    public: // Emergency number API

        /**
        * Returns information whether the number specified was an emergency number.
        * 
        * @param aNumber Number to be queried.
        *                The aNumber parameter should be a buffer and the 
        *                clients are provided with a typedef called 
        *                TPhCltEmergencyNumber. 
        * @param aIsEmergencyNumber If ETrue, then the aNumber 
        *                           parameter included emergency number.
        * @return Returns error code of the success of the operation.
        */
        virtual TInt IsEmergencyPhoneNumber(
            const TDesC& aNumber, 
            TBool& aIsEmergencyNumber );

        /**
        * Returns information whether the number specified would initiate 
        * an emergency call.
        * 
        * @param aNumber Number to be queried, matched emergency number 
        *                is returned if a correct emergency number is 
        *                found in the end of the number.
                         The aNumber parameter should be a buffer and the 
        *                clients are provided with a typedef called 
        *                TPhCltTelephoneNumber.         
        * @param aIsEmergencyNumber If ETrue, then the aNumber parameter 
        *                           was emergency number. 
        * @return Returns error code of the success of the operation.
        */
        virtual TInt FindEmergencyPhoneNumber(
            TDes& aNumber, 
            TBool& aIsEmergencyNumber );

        /**
        * Initiates an emergency call.
        *
        * @param aNumber Emergency number to be dialed (optional).
        *                The aNumber parameter should be a buffer and the 
        *                clients are provided with a typedef called 
        *                TPhCltEmergencyNumber. 
        *
        *                If the aNumber parameter supplies an "empty" buffer 
        *                the call is created either dialing the default emergency 
        *                phone number or use the SET UP EMERGENCY request  
        *                that does not contain a  dialling number.
        */
        virtual void DialEmergencyCallL( const TDesC& aNumber );

    private:
    
        /**
        * C++ constructor.
        */
        CPhCltEmergencyCallImpl( MPhCltEmergencyCallObserver* aObserver );
        
        /**
        * Symbian OS constructor.
        */
        void ConstructL();
        
        /**
        * From CActive, it is called when object is active and 
        * request has been processed.
        */
        void RunL();

        /**
        * From CActive, calcels pending request.
        */
        void DoCancel();
        
        /**
        * Check that active object is not active and subsession
        * before attempting a dial
        */
        void DialEmergencyPreconditionCheckL();
        
    private:
        
        // RClass instance
        RPhCltEmergencyCall iEmergencyCall;
        
        // wrapper for emergencynumber
        TPtrC8 iEmergencyCallNumberPtr;
        
        //RPhCltServer
        RPhCltServer iServer;
        
       //EmergencyCallNumber
       TPhCltEmergencyNumber iEmergencyCallNumber;
    };

#endif // CPHCLTEMERGENCYCALLIMPL_H

// End of File
