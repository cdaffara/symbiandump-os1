/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Ext Phone Dial Data.
*
*/



// INCLUDE FILES
#include "TPhCltExtPhoneDialData.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::TPhCltExtPhoneDialData
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltExtPhoneDialData::TPhCltExtPhoneDialData()
    : iWindowGroup( KPhCltGoToIdle ), 
    iRedial( KPhCltRedialDefault ), 
    iNumberType( KPhCltDefaultNumberType ), 
    iShowNumber( ETrue ), 
    iReturnToNumberEntryOnFailure( KPhCltDefaultNumberEntry ), 
    iAllowMatch( KPhCltDefaultAllowMatch ), 
    iName( KNullDesC ), 
    iContactId( KPhCltNoContact), 
    iTelNumber( KNullDesC ),
    iEndOtherCalls( EFalse ),
    iEmergencyCall( EFalse ),
    iSubAddress( KNullDesC ),
    iSATCall( EFalse ),
    iBearer( KNullDesC8 ),
    iCallType( EPhCltVoice ),
    iSendKey( EFalse )
    {
    iRedialMaximumDuration = 0;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::TPhCltExtPhoneDialData
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltExtPhoneDialData::TPhCltExtPhoneDialData( 
    const TPhCltTelephoneNumber& aNumber, 
    TPhCltContactId aContactId )
    : iWindowGroup( KPhCltGoToIdle), 
    iRedial( KPhCltRedialDefault ),
    iNumberType( KPhCltDefaultNumberType ), 
    iShowNumber( ETrue ), 
    iReturnToNumberEntryOnFailure( KPhCltDefaultNumberEntry ), 
    iAllowMatch( KPhCltDefaultAllowMatch ), 
    iName( KNullDesC ), 
    iContactId( aContactId ), 
    iTelNumber( aNumber ),
    iEndOtherCalls( EFalse ),
    iEmergencyCall( EFalse ),
    iSubAddress( KNullDesC ),
    iSATCall( EFalse ),
    iBearer( KNullDesC8 ),
    iCallType( EPhCltVoice ),
    iSendKey( EFalse )
    {
    iRedialMaximumDuration = 0;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::TPhCltExtPhoneDialData
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltExtPhoneDialData::TPhCltExtPhoneDialData( 
    const TPhCltTelephoneNumber& aNumber, 
    TInt aWindowGroupId )
    : iWindowGroup( aWindowGroupId ), 
    iRedial( KPhCltRedialDefault ),
    iNumberType( KPhCltDefaultNumberType ), 
    iShowNumber( ETrue ),
    iReturnToNumberEntryOnFailure( KPhCltDefaultNumberEntry ),
    iAllowMatch( KPhCltDefaultAllowMatch ), 
    iName( KNullDesC ),
    iContactId( KPhCltNoContact ), 
    iTelNumber( aNumber ),
    iEndOtherCalls( EFalse ),
    iEmergencyCall( EFalse ),
    iSubAddress( KNullDesC ),
    iSATCall( EFalse ),
    iBearer( KNullDesC8 ),
    iCallType( EPhCltVoice ),
    iSendKey( EFalse )
    {
    iRedialMaximumDuration = 0;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::TPhCltExtPhoneDialData
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltExtPhoneDialData::TPhCltExtPhoneDialData( 
    const TPhCltTelephoneNumber& aNumber, 
    TPhCltContactId aContactId, 
    TInt aWindowGroupId )
    : iWindowGroup( aWindowGroupId ), 
    iRedial( KPhCltRedialDefault ),
    iNumberType( KPhCltDefaultNumberType ), 
    iShowNumber( ETrue ), 
    iReturnToNumberEntryOnFailure( KPhCltDefaultNumberEntry ),
    iAllowMatch( KPhCltDefaultAllowMatch ), 
    iName (KNullDesC ),
    iContactId( aContactId ), 
    iTelNumber( aNumber ),
    iEndOtherCalls( EFalse ),
    iEmergencyCall( EFalse ),
    iSubAddress( KNullDesC ),
    iSATCall( EFalse ),
    iBearer( KNullDesC8 ),
    iCallType( EPhCltVoice ),
    iSendKey( EFalse )
    {
    iRedialMaximumDuration = 0;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::TPhCltExtPhoneDialData
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltExtPhoneDialData::TPhCltExtPhoneDialData( 
    const TPhCltNameBuffer& aName, 
    const TPhCltTelephoneNumber& aNumber, 
    TPhCltContactId aContactId, 
    TInt aWindowGroupId )
    : iWindowGroup( aWindowGroupId ), 
    iRedial( KPhCltRedialDefault ),
    iNumberType( KPhCltDefaultNumberType ), 
    iShowNumber( ETrue ),
    iReturnToNumberEntryOnFailure( KPhCltDefaultNumberEntry ), 
    iAllowMatch( KPhCltDefaultAllowMatch ), 
    iName( aName ), 
    iContactId( aContactId ), 
    iTelNumber( aNumber ),
    iEndOtherCalls( EFalse ),
    iEmergencyCall( EFalse ),
    iSubAddress( KNullDesC ),
    iSATCall( EFalse ),
    iBearer( KNullDesC8 ),
    iCallType( EPhCltVoice ),
    iSendKey( EFalse )
    {
    iRedialMaximumDuration = 0;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetWindowGroup
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetWindowGroup( TInt aWindowGroupId )
    {
    iWindowGroup = aWindowGroupId;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetShowNumber
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetShowNumber( TBool aShowNumber )
    {
    iShowNumber = aShowNumber;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetRedial
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetRedial( TInt aRedial )
    {
    iRedial = aRedial;
    }

// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::Set redial maximum duration
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetRedialMaximumDuration( TTimeIntervalSeconds aMaximumRedialDuration )
    {
    iRedialMaximumDuration = aMaximumRedialDuration;
    }
    
// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetName
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetName( const TPhCltNameBuffer& aName )
    {
    iName = aName;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetContactId
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetContactId( 
    TPhCltContactId aContactId )
    {
    iContactId = aContactId;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetTelephoneNumber
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetTelephoneNumber( 
    const TPhCltTelephoneNumber& aNumber )
    {
    iTelNumber = aNumber;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetReturnToNumberEntryOnFailure
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetReturnToNumberEntryOnFailure( 
    TBool aReturn )
    {
    iReturnToNumberEntryOnFailure = aReturn;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetAllowMatch
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetAllowMatch( TBool aAllowMatch )
    {
    iAllowMatch = aAllowMatch;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetNumberType
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetNumberType( TInt aNumberType )
    {
    iNumberType = aNumberType;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetEndOtherCalls
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetEndOtherCalls( TBool aEndCalls )
    {
    iEndOtherCalls = aEndCalls;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetEmergency
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetEmergency( TBool aEmergency )
    {
    iEmergencyCall = aEmergency;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetSubAddress
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetSubAddress( 
    const TPhCltSubAddress& aSubAddress )
    {
    iSubAddress = aSubAddress;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetSATCall
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetSATCall( TBool aSATCall )
    {
    iSATCall = aSATCall;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetBearer
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetBearer( 
    const TPhCltBearerCapability& aBearer )
    {
    iBearer = aBearer;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetCallType
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetCallType( TPhCltCallType aCallType )
    {
    iCallType = aCallType;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SetSendKeyPressed
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::SetSendKeyPressed( TBool aSendKey )
    {
    iSendKey = aSendKey;
    }

// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::WindowGroupId
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TPhCltExtPhoneDialData::WindowGroupId() const
    {
    return iWindowGroup;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::Redial
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TPhCltExtPhoneDialData::Redial() const
    {
    return iRedial;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData:: Redial maximum duration
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TTimeIntervalSeconds TPhCltExtPhoneDialData::RedialMaximumDuration( ) const
    {
    return iRedialMaximumDuration;
    }  
    
// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::NumberType
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TPhCltExtPhoneDialData::NumberType() const
    {
    return iNumberType;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::ReturnToNumberEntryOnFailure
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TPhCltExtPhoneDialData::ReturnToNumberEntryOnFailure() const
    {
    return iReturnToNumberEntryOnFailure;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::AllowMatch
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TPhCltExtPhoneDialData::AllowMatch() const
    {
    return iAllowMatch;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::ShowNumber
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TPhCltExtPhoneDialData::ShowNumber() const
    {
    return iShowNumber;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::Name
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C const TPhCltNameBuffer& TPhCltExtPhoneDialData::Name() const
    {
    return iName;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::ContactId
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltContactId TPhCltExtPhoneDialData::ContactId() const
    {
    return iContactId;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::TelephoneNumber
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C const TPhCltTelephoneNumber& 
    TPhCltExtPhoneDialData::TelephoneNumber() const
    {
    return iTelNumber;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::EndOtherCalls
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TPhCltExtPhoneDialData::EndOtherCalls() const
    {
    return iEndOtherCalls;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::Emergency
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TPhCltExtPhoneDialData::Emergency() const
    {
    return iEmergencyCall;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SubAddress
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C const TPhCltSubAddress& TPhCltExtPhoneDialData::SubAddress() const
    {
    return iSubAddress;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SATCall
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TPhCltExtPhoneDialData::SATCall() const
    {
    return iSATCall;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::Bearer
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C const TPhCltBearerCapability& TPhCltExtPhoneDialData::Bearer() const
    {
    return iBearer;
    }


// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::CallType
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltCallType TPhCltExtPhoneDialData::CallType() const
    {
    return iCallType;
    }

// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::SendKeyPressed
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TPhCltExtPhoneDialData::SendKeyPressed() const
    {
    return iSendKey;
    }

// -----------------------------------------------------------------------------
// TPhCltExtPhoneDialData::AsIPCStruct
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void TPhCltExtPhoneDialData::AsIPCStruct( TPhCltCallArgs& aReturn ) 
    const
    {
    aReturn.iContactId = iContactId;
    aReturn.iName = iName;
    aReturn.iRedial = iRedial;
    aReturn.iTelNumber = iTelNumber;
    aReturn.iWindowGroup = iWindowGroup;
    aReturn.iNumberType = iNumberType;
    aReturn.iSubAddress = iSubAddress;
    aReturn.iBearer = iBearer;
	aReturn.iRedialMaximumDuration = iRedialMaximumDuration;
    // Flags
    aReturn.iFlags = TPhCltCallArgs::EPhCltCallFlagsDefault;

    if ( iShowNumber )
        {
        aReturn.iFlags |= TPhCltCallArgs::EPhCltCallFlagsAllowNumberShowing;
        }

    if ( iReturnToNumberEntryOnFailure )
        {
        aReturn.iFlags |= 
            TPhCltCallArgs::EPhCltCallFlagsReturnToNumberEntryOnFailure;
        }
    if ( iAllowMatch )
        {
        aReturn.iFlags |= 
            TPhCltCallArgs::EPhCltCallFlagsAllowPersonalDirectoryMatching;
        }
    if ( iEndOtherCalls )
        {
        aReturn.iFlags |= TPhCltCallArgs::EPhCltCallFlagsEndBeforeDial;
        }

    if ( iEmergencyCall )
        {
        aReturn.iFlags |= TPhCltCallArgs::EPhCltCallFlagsEmergencyCall;
        }

    if ( iSATCall )
        {
        aReturn.iFlags |= TPhCltCallArgs::EPhCltCallFlagsSAT;
        }

    if ( ( iCallType == EPhCltVideo ) || ( iCallType == EPhCltForcedVideo ) )
        {
        aReturn.iFlags |= TPhCltCallArgs::EPhCltCallFlagsVideoCall;
        }

    if ( iCallType == EPhCltCallVoIP )
        {
        aReturn.iFlags |= TPhCltCallArgs::EPhCltCallFlagsVoIPCall;
        }
        
    if ( iSendKey )
        {
        aReturn.iFlags |= TPhCltCallArgs::EPhCltCallFlagsSendKey;
        }
    }


// End of File
