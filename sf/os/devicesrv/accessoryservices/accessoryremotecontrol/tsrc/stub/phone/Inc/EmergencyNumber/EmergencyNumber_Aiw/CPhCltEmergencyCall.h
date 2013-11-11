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
* Description:  
*
*/


#ifndef CPHCLTEMERGENCYCALL_H
#define CPHCLTEMERGENCYCALL_H

//  INCLUDES
#include <e32base.h>
#include "PhCltTypes.h"		//For TPhCltTelephoneNumber
// FORWARD DECLARATIONS
class MPhCltEmergencyCallObserver;

// CLASS DECLARATION

// CONSTANTS
const TInt KPhCltEmergencyNumberSize = 6;

// DATA TYPES
typedef TBuf< KPhCltEmergencyNumberSize > TPhCltEmergencyNumber;

// CLASS DECLARATION
class CPhCltEmergencyCall : public CActive
    {
    public:  // Constructors and destructor
 
        /**
        * Create an instance that implements the interface.
        *
        * @param aObserver It is the observer for the instance.
        * @return Returns the created instance.
        */
        IMPORT_C static CPhCltEmergencyCall* NewL( 
            MPhCltEmergencyCallObserver* aObserver );

    public: // New functions    
        
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
        IMPORT_C virtual TInt IsEmergencyPhoneNumber(
            const TDesC& aNumber, 
            TBool& aIsEmergencyNumber ) = 0;

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
        IMPORT_C virtual TInt FindEmergencyPhoneNumber(
            TDes& aNumber, 
            TBool& aIsEmergencyNumber ) = 0;

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
        IMPORT_C virtual void DialEmergencyCallL( const TDesC& aNumber ) = 0;

    protected:

        /**
        * C++ constructor.
        */
        CPhCltEmergencyCall( MPhCltEmergencyCallObserver* aObserver );


    protected: // Data
        
        // Observer to handle events.
        MPhCltEmergencyCallObserver* iObserver;

    };

#endif      // CPHCLTEMERGENCYCALL_H
            
// End of File
