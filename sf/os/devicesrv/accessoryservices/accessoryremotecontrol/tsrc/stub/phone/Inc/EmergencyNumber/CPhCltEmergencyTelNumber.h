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
* Description:  Emergency Telephony Number
*
*/


#ifndef CPHCLTEMERGENCYTELNUMBER_H
#define CPHCLTEMERGENCYTELNUMBER_H

//  INCLUDES
#include <e32std.h>
#include "PhCltTypes.h"
#include "RPhCltServer.h"

// CLASS DECLARATION

/**
*  Emergency Telephony Number.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltEmergencyTelNumber : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        *
        * @return Pointer to created CPhCltEmergencyTelNumber instance.
        */
        IMPORT_C static CPhCltEmergencyTelNumber* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C ~CPhCltEmergencyTelNumber();


    public: // [In|Ex]ternalize

        /**
        * Read an emergency number from a stream.
        *
        * @param aStream The stream from which the emergency number is read.
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );

        /**
        * Write an emergency number to a stream.
        *
        * @param aStream The stream where the emergency number is written.
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;


    public: // Access

        /**
        * Return the phone number.
        *
        * @return The telephony number field of the emergency number.
        */
        IMPORT_C const TDesC& TelNumber() const;

        /**
        * Return the MNC field.
        *
        * @return The MNC field of the emergency number.
        */
        IMPORT_C TUint16 MNC() const;

        /**
        * Return the MCC field.
        *
        * @return The MCC field of the emergency number.
        */
        IMPORT_C TUint16 MCC() const;

        /**
        * Set details of the emergency number.
        *
        * @param aNumber Telephony number field of the emergency number.
        * @param aMNC MNC field of the emergency number.
        * @param aMCC MCC field of the emergency number.
        */
        IMPORT_C void SetL( 
            const TPhCltTelephoneNumber& aNumber, 
            TUint16 aMNC, 
            TUint16 aMCC );


    private:

        // MNC field of the emergency number.
        TUint16 iFieldMNC;

        // MCC field of the emergency number.
        TUint16 iFieldMCC;

        // Telephony number field of the emergency number.
        HBufC* iTelNumber;

    };


#endif // CPHCLTEMERGENCYTELNUMBER_H

// End of File
