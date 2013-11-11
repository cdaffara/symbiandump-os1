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


// INCLUDE FILES
#include    "CPhCltEmergencyTelNumber.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPhCltEmergencyTelNumber::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltEmergencyTelNumber* CPhCltEmergencyTelNumber::NewLC()
    {
    CPhCltEmergencyTelNumber* self = new( ELeave ) CPhCltEmergencyTelNumber;
    CleanupStack::PushL( self );
    return self;
    }


// Destructor
EXPORT_C CPhCltEmergencyTelNumber::~CPhCltEmergencyTelNumber()
    {
    delete iTelNumber;
    }


// -----------------------------------------------------------------------------
// CPhCltEmergencyTelNumber::InternalizeL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltEmergencyTelNumber::InternalizeL( 
    RReadStream& aStream )
    {
    HBufC* number = HBufC::NewL( aStream, KPhCltTelephoneNumberLength );
    delete iTelNumber;
    iTelNumber = number;
    //
    iFieldMCC = aStream.ReadUint16L();
    iFieldMNC = aStream.ReadUint16L();
    }


// -----------------------------------------------------------------------------
// CPhCltEmergencyTelNumber::ExternalizeL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltEmergencyTelNumber::ExternalizeL(
    RWriteStream& aStream ) const
    {
    aStream << *iTelNumber;
    aStream.WriteUint16L( iFieldMCC );
    aStream.WriteUint16L( iFieldMNC );
    }


// -----------------------------------------------------------------------------
// CPhCltEmergencyTelNumber::TelNumber
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPhCltEmergencyTelNumber::TelNumber() const
    {
    if( !iTelNumber )
        {
        return KNullDesC;
        }
    return *iTelNumber;
    }


// -----------------------------------------------------------------------------
// CPhCltEmergencyTelNumber::MNC
//
// -----------------------------------------------------------------------------
//
EXPORT_C TUint16 CPhCltEmergencyTelNumber::MNC() const
    {
    return iFieldMNC;
    }


// -----------------------------------------------------------------------------
// CPhCltEmergencyTelNumber::MCC
//
// -----------------------------------------------------------------------------
//
EXPORT_C TUint16 CPhCltEmergencyTelNumber::MCC() const
    {
    return iFieldMCC;
    }


// -----------------------------------------------------------------------------
// CPhCltEmergencyTelNumber::NewLC
// 
// Creates a subsession, as part of the given session.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltEmergencyTelNumber::SetL( 
    const TPhCltTelephoneNumber& aNumber, TUint16 aMNC, TUint16 aMCC )
    {
    HBufC* number = aNumber.AllocL();
    delete iTelNumber;
    iTelNumber = number;
    //
    iFieldMNC = aMNC;
    iFieldMCC = aMCC;
    }


//  End of File  
