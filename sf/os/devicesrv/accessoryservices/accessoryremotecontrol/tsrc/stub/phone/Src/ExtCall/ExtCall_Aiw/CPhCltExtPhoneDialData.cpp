/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "CPhCltExtPhoneDialData.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPhCltExtPhoneDialData::CPhCltExtPhoneDialData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltExtPhoneDialData::CPhCltExtPhoneDialData():
    iTelNumber( KNullDesC ),
    iCallType( EPhCltVoice ),
    iName( KNullDesC ), 
    iWindowGroup( KPhCltGoToIdle ), 
    iRedial( KPhCltRedialDefault ), 
    iSubAddress( KNullDesC ),
    iShowNumber( ETrue ), 
    iAllowMatch( KPhCltDefaultAllowMatch ),
    iSATCall( EFalse ),
    iEndOtherCalls( EFalse ),
    iBearer( KNullDesC8 ),
    iSendKey( EFalse ),
    iReturnToNumberEntryOnFailure( KPhCltDefaultNumberEntry ), 
    iInitCall ( EFalse ), 
    iFlags( EPhCltCallFlagsDefault )   
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)
    ,    
    iServiceId( NULL )
#endif // __VOIP) && RD_VOIP_REL_2_2  
    { 
    iRedialMaximumDuration = 0;      
    }


// ---------------------------------------------------------------------------
// 
// Symbian OS constructor
// 
// ---------------------------------------------------------------------------
//
void CPhCltExtPhoneDialData::ConstructL()
    {    
    iContactLink = KNullDesC8().AllocL();
    iUUI = KNullDesC().AllocL();
    }

// ---------------------------------------------------------------------------
// 
// Symbian OS constructor  
// Initialises the member data from package.
//
// ---------------------------------------------------------------------------
//
void CPhCltExtPhoneDialData::ConstructL(
    const TDesC8& aPackage ) 
    {
    RDesReadStream stream( aPackage );
    CleanupClosePushL( stream );
    
    stream >> iTelNumber;
    iCallType = static_cast<TPhCltCallType>( stream.ReadInt32L() );
    stream >> iName;
    iWindowGroup = stream.ReadInt32L();
    iRedial = stream.ReadInt32L();
    iRedialMaximumDuration = stream.ReadInt32L();
    ReadDescriptorFromStreamL( stream, iContactLink );
    stream >> iSubAddress;
    iShowNumber = stream.ReadInt32L();
    iAllowMatch = stream.ReadInt32L();
    iSATCall = stream.ReadInt32L(); 
    iEndOtherCalls = stream.ReadInt32L();
    stream >> iBearer;
    iSendKey = stream.ReadInt32L();
    iReturnToNumberEntryOnFailure = stream.ReadInt32L();
    iInitCall = stream.ReadInt32L();
    iFlags = stream.ReadUint32L();
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)    
    iServiceId = stream.ReadUint32L();
#endif // __VOIP) && RD_VOIP_REL_2_2  
    ReadDescriptorFromStreamL( stream, iUUI );
    CleanupStack::PopAndDestroy( 1 ); // stream;
    }
// -----------------------------------------------------------------------------
// CPhCltExtPhoneDialData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltExtPhoneDialData* CPhCltExtPhoneDialData::NewL()
    {
    CPhCltExtPhoneDialData* self = CPhCltExtPhoneDialData::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPhCltExtPhoneDialData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltExtPhoneDialData* CPhCltExtPhoneDialData::NewLC(  )
    {
    CPhCltExtPhoneDialData* self = new( ELeave ) CPhCltExtPhoneDialData;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// First phase constructor
// Initialises the member data from package.
// 
// --------------------------------------------------------------------------- 
//    
EXPORT_C CPhCltExtPhoneDialData* CPhCltExtPhoneDialData::NewL( 
    const TDesC8& aPackage )
    {
    CPhCltExtPhoneDialData* self = CPhCltExtPhoneDialData::NewLC( aPackage );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// First phase constructor
// Initialises the member data from package.
// 
// --------------------------------------------------------------------------- 
//
EXPORT_C CPhCltExtPhoneDialData* CPhCltExtPhoneDialData::NewLC(
    const TDesC8& aPackage )
    {
    CPhCltExtPhoneDialData* self = new( ELeave ) CPhCltExtPhoneDialData;
    CleanupStack::PushL( self );
    self->ConstructL( aPackage );
    return self;
    }

// Destructor
EXPORT_C CPhCltExtPhoneDialData::~CPhCltExtPhoneDialData()
    {  
    delete iContactLink;
    iContactLink = NULL;
    delete iUUI; 
    iUUI = NULL;
    }
   

 // Access - Setters  
 
// ---------------------------------------------------------------------------
// Sets allow match.
// ---------------------------------------------------------------------------
//   
EXPORT_C void CPhCltExtPhoneDialData::SetAllowMatch( TBool aAllowMatch ) 
    {
    iAllowMatch = aAllowMatch;
    }

// ---------------------------------------------------------------------------
// Sets bearer.
// ---------------------------------------------------------------------------
EXPORT_C void CPhCltExtPhoneDialData::SetBearerL ( 
const TPhCltBearerCapability& aBearer )
    {
    iBearer = aBearer;
    }
    
// ---------------------------------------------------------------------------
// Sets call type.
// ---------------------------------------------------------------------------
//   
EXPORT_C void CPhCltExtPhoneDialData::SetCallType( TPhCltCallType aCallType ) 
    {
    iCallType = aCallType;
    }
 
// ---------------------------------------------------------------------------
// Sets used contact link.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPhCltExtPhoneDialData::SetContactLinkL( const TDesC8& aContactLink ) 
    {
    CopyDescriptorToMemberDescriptorL( aContactLink, iContactLink );
    }

// ---------------------------------------------------------------------------
// Sets end other calls.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPhCltExtPhoneDialData::SetEndOtherCalls( TBool aEndCalls ) 
    {
    iEndOtherCalls = aEndCalls ;
    }
 
 // ---------------------------------------------------------------------------
// Set initiate call.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CPhCltExtPhoneDialData::SetInitiateCall( TBool aInitCall )
    {
    iInitCall = aInitCall;
    } 
// ---------------------------------------------------------------------------
// Set name.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CPhCltExtPhoneDialData::SetNameL( const TPhCltNameBuffer& aName ) 
    {
    iName = aName;
    }
    
// ---------------------------------------------------------------------------
// Sets phone number.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPhCltExtPhoneDialData::SetTelephoneNumber(
    const TPhCltTelephoneNumber& aNumber ) 
    {
    iTelNumber = aNumber;
    }
  
// ---------------------------------------------------------------------------
// Sets redial value.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CPhCltExtPhoneDialData::SetRedial( TInt aRedial ) 
    {
    iRedial = aRedial;
    }
    
// ---------------------------------------------------------------------------
// Sets redial max duration time.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPhCltExtPhoneDialData::SetRedialMaximumDuration( 
    TTimeIntervalSeconds aMaximumRedialDuration ) 
    {
    iRedialMaximumDuration = aMaximumRedialDuration;
    }
  
// ---------------------------------------------------------------------------
// Sets SAT call.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPhCltExtPhoneDialData::SetSATCall( TBool aIsSATCall ) 
    {
    iSATCall = aIsSATCall;
    }

// -----------------------------------------------------------------------------
// SetSendKeyPressed
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltExtPhoneDialData::SetSendKeyPressed( TBool aSendKey )
    {
    iSendKey = aSendKey;
    }

// ---------------------------------------------------------------------------
// Set show number.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CPhCltExtPhoneDialData::SetShowNumber( TBool aShowNumber ) 
    {
    iShowNumber = aShowNumber;
    } 
    
// ---------------------------------------------------------------------------
// Sets SubAddress for SAT call.
// ---------------------------------------------------------------------------
EXPORT_C void CPhCltExtPhoneDialData::SetSubAddressL( 
    const TPhCltSubAddress& aSubAddress )
    {
    iSubAddress = aSubAddress;
    }
    
// ---------------------------------------------------------------------------
// Sets window group.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CPhCltExtPhoneDialData::SetWindowGroup( TInt aWindowGroup ) 
    {
    iWindowGroup = aWindowGroup;
    }
  
// ---------------------------------------------------------------------------
// Sets window group.
// ---------------------------------------------------------------------------
//
EXPORT_C void CPhCltExtPhoneDialData::SetUUIL( const TDesC& aUUI )
    {
    CopyDescriptorToMemberDescriptorL( aUUI, iUUI );
    }

// Access - Getters 

// ---------------------------------------------------------------------------
// Return allow match.
// ---------------------------------------------------------------------------
//   
EXPORT_C TBool CPhCltExtPhoneDialData::AllowMatch() const 
    {
    return iAllowMatch;
    }
    
// ---------------------------------------------------------------------------
// Returns bearer.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPhCltBearerCapability& CPhCltExtPhoneDialData::Bearer() const
    {
    return iBearer;
    }
    
// ---------------------------------------------------------------------------
// Returns call type.
// ---------------------------------------------------------------------------
//    
EXPORT_C TPhCltCallType  CPhCltExtPhoneDialData::CallType() const 
    {
    return iCallType;
    }

// ---------------------------------------------------------------------------
// Return contact link.
// ---------------------------------------------------------------------------
//    
EXPORT_C const TDesC8& CPhCltExtPhoneDialData::ContactLink() const 
    {
    return *iContactLink;
    } 
     
// ---------------------------------------------------------------------------
// Returns end other calls value.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CPhCltExtPhoneDialData::EndOtherCalls() const
    {
    return iEndOtherCalls;
    }
    
// ---------------------------------------------------------------------------
// Returns initiate call.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CPhCltExtPhoneDialData::InitiateCall() const 
    {
    return iInitCall;
    } 
    
// ---------------------------------------------------------------------------
// Returns name.
// ---------------------------------------------------------------------------
//
EXPORT_C const TPhCltNameBuffer& CPhCltExtPhoneDialData::Name() const 
    {
    return iName;
    }

// ---------------------------------------------------------------------------
// Returns phone number.
// ---------------------------------------------------------------------------
//    
EXPORT_C const TPhCltTelephoneNumber& CPhCltExtPhoneDialData::TelephoneNumber() const 
    {
    return iTelNumber;    
    }

// ---------------------------------------------------------------------------
// Return redial value.
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CPhCltExtPhoneDialData::Redial() const 
    {
    return iRedial;
    }
// ---------------------------------------------------------------------------
// Returns redial max duration.
// ---------------------------------------------------------------------------
//
EXPORT_C const TTimeIntervalSeconds CPhCltExtPhoneDialData::RedialMaximumDuration() const
    {
    return iRedialMaximumDuration;
    }   

// ---------------------------------------------------------------------------
// Returns SAT call value.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CPhCltExtPhoneDialData::SATCall() const
    {
    return iSATCall;    
    }
    
 // -----------------------------------------------------------------------------
// SendKeyPressed
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPhCltExtPhoneDialData::SendKeyPressed() const
    {
    return iSendKey;
    }
        
// ---------------------------------------------------------------------------
// Returns show number.
// ---------------------------------------------------------------------------
//   
EXPORT_C TBool CPhCltExtPhoneDialData::ShowNumber() const 
    {
    return iShowNumber;
    }
    
// ---------------------------------------------------------------------------
// Returns subaddress.
// ---------------------------------------------------------------------------
//   
EXPORT_C const TPhCltSubAddress& CPhCltExtPhoneDialData::SubAddress() const
    {
    return iSubAddress;
    }
    
// ---------------------------------------------------------------------------
// Return window group.
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CPhCltExtPhoneDialData::WindowGroup() const
    {
    return iWindowGroup;
    }
    
// ---------------------------------------------------------------------------
// Return user to user information
// ---------------------------------------------------------------------------
//      
EXPORT_C const TDesC& CPhCltExtPhoneDialData::UUI() const 
    {
    return *iUUI;
    }
 
 // Other member functions   

// ---------------------------------------------------------------------------
// Calculates the size of descriptor in package. Note that to retreave 
// a descriptor from stream in ConstructL we need to know the size of a
// descriptor in order to allocate right size descriptor. This is the reason
// for adding sizeof( int ) to descriptor length.
// ---------------------------------------------------------------------------
// 
TInt CPhCltExtPhoneDialData::CalculateDescriptorSize( const TDesC& aDescriptor ) const 
    {
    // sizeof( TInt ) is for the length of a descriptor.
    return sizeof( TInt ) + aDescriptor.Length() * 2; // unicode takes 2 bytes.
    }
    
// ---------------------------------------------------------------------------
// Calculates the size of descriptor in package. Note that to retreave 
// a descriptor from stream in ConstructL we need to know the size of a
// descriptor in order to allocate right size descriptor. This is the reason
// for adding sizeof( int ) to descriptor length.
// ---------------------------------------------------------------------------
// 
TInt CPhCltExtPhoneDialData::CalculateDescriptorSize( const TDesC8& aDescriptor ) const 
    {
    // sizeof( TInt ) is for the length of a descriptor in stream
    return sizeof( TInt ) + aDescriptor.Length();
    }   
    
// ---------------------------------------------------------------------------
// Calculates the size of all member data.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CPhCltExtPhoneDialData::CalculatePackageSize() const
    {
    TInt packageSize = 0;
    packageSize += CalculateDescriptorSize( iTelNumber );
    packageSize += sizeof( iCallType );
    packageSize += CalculateDescriptorSize( iName );
    packageSize += sizeof( iWindowGroup );
    packageSize += sizeof( iRedial );
    packageSize+=sizeof( iRedialMaximumDuration.Int() );
    packageSize += CalculateDescriptorSize( *iContactLink );
    packageSize += CalculateDescriptorSize( iSubAddress );
    packageSize += sizeof( iShowNumber);
    packageSize += sizeof( iAllowMatch );
    packageSize += sizeof( iSATCall );
    packageSize += sizeof( iEndOtherCalls );
    packageSize += CalculateDescriptorSize( iBearer );
    packageSize += sizeof (iSendKey );
    packageSize += sizeof( iReturnToNumberEntryOnFailure);
    packageSize += sizeof (iInitCall );
    packageSize += sizeof( iFlags );
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2) 
    packageSize += sizeof( iServiceId );
#endif // __VOIP) && RD_VOIP_REL_2_2   
    packageSize += CalculateDescriptorSize( *iUUI );
    return packageSize;
    }
    
// ---------------------------------------------------------------------------
// Checks that descriptorLength is not longer than maximum allowed descriptor
// length.
// ---------------------------------------------------------------------------
//   
void CPhCltExtPhoneDialData::CheckDescriptorMaxLengthL( 
    TInt aDescriptorLength, TInt aMaxAllowedDescriptorLength ) const
    {
    if( aDescriptorLength > aMaxAllowedDescriptorLength )
        {
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// Copies aSource descriptor to aMemberDescriptor. This quarantees that member
// descriptor will be valid even if the allocation fails.
// ---------------------------------------------------------------------------
//   
void CPhCltExtPhoneDialData::CopyDescriptorToMemberDescriptorL( 
    const TDesC& aSource, 
    HBufC*& aMemberDescriptor )
    {
    HBufC* temp = aSource.AllocL();
    delete aMemberDescriptor;
    aMemberDescriptor = temp;
    }

// ---------------------------------------------------------------------------
// Copies aSource descriptor to aMemberDescriptor. This quarantees that member
// descriptor will be valid even if the allocation fails.
// ---------------------------------------------------------------------------
//       
void CPhCltExtPhoneDialData::CopyDescriptorToMemberDescriptorL( 
    const TDesC8& aSource, 
    HBufC8*& aMemberDescriptor )
    {
    HBufC8* temp = aSource.AllocL();
    delete aMemberDescriptor;
    aMemberDescriptor = temp;
    }
              
  
// ---------------------------------------------------------------------------
// Reads descriptor from stream.
// ---------------------------------------------------------------------------
//  
void CPhCltExtPhoneDialData::ReadDescriptorFromStreamL( 
    RDesReadStream& aStream,
    HBufC8*& aDescriptor 
    )
    {
    const TInt descLength( aStream.ReadInt32L() );
    if ( aDescriptor )
        {
        delete aDescriptor;
        }
    // Not to cleanupstack, aDescriptor is member variable
    aDescriptor = HBufC8::NewL( descLength ); 
    TPtr8 ptr = aDescriptor->Des();
    aStream.ReadL( ptr, descLength );
    }    
    
// ---------------------------------------------------------------------------
// Reads descriptor from stream.
// ---------------------------------------------------------------------------
//  
void CPhCltExtPhoneDialData::ReadDescriptorFromStreamL( 
    RDesReadStream& aStream, 
    HBufC*& aDescriptor )
    {
   const TInt descLength( aStream.ReadInt32L() );
    if ( aDescriptor )
        {
        delete aDescriptor;
        }
    // Not to cleanupstack, aDescriptor will be member variable
    aDescriptor = HBufC::NewL( descLength ); 
    TPtr16 ptr = aDescriptor->Des();
    aStream.ReadL( ptr, descLength );
    }

// ---------------------------------------------------------------------------
// Writes descriptor to stream.
// ---------------------------------------------------------------------------
//  
void CPhCltExtPhoneDialData::WriteDescriptorToStreamL( 
    RDesWriteStream& aStream, 
    const TDesC8& aDescriptor ) 
    {
    aStream.WriteInt32L( aDescriptor.Length() );
    aStream.WriteL( aDescriptor );
    }
    
// ---------------------------------------------------------------------------
// Writes descriptor to stream.
// ---------------------------------------------------------------------------
//  
void CPhCltExtPhoneDialData::WriteDescriptorToStreamL( 
    RDesWriteStream& aStream, 
    const TDesC& aDescriptor ) 
    {
    aStream.WriteInt32L( aDescriptor.Length() );
    aStream.WriteL( aDescriptor );
    }    

// ---------------------------------------------------------------------------
// Packs the member data to descriptor. Ownershipd of returned descriptor
// is given for caller.
// ---------------------------------------------------------------------------
//  
EXPORT_C HBufC8* CPhCltExtPhoneDialData::PackLC() 
    {
    HBufC8* package = HBufC8::NewLC( CalculatePackageSize() );
    
    TPtr8 des = package->Des();
    RDesWriteStream stream( des );
    
    CleanupClosePushL( stream );

    stream << iTelNumber;     
    stream.WriteInt32L( iCallType );       
    stream << iName;
    stream.WriteInt32L( iWindowGroup );
    stream.WriteInt32L( iRedial );
    stream.WriteInt32L( iRedialMaximumDuration.Int() );
    WriteDescriptorToStreamL( stream, *iContactLink );
    stream << iSubAddress;
    stream.WriteInt32L( iShowNumber );
    stream.WriteInt32L( iAllowMatch );
    stream.WriteInt32L( iSATCall);
    stream.WriteInt32L( iEndOtherCalls );
    stream << iBearer;
    stream.WriteInt32L( iSendKey );
    stream.WriteInt32L( iReturnToNumberEntryOnFailure );
    stream.WriteInt32L( iInitCall );
    
    
    iFlags = EPhCltCallFlagsDefault;

    if ( iShowNumber )
        {
        iFlags |= EPhCltCallFlagsAllowNumberShowing;
        }

    if ( iAllowMatch )
        {
        iFlags |= 
            EPhCltCallFlagsAllowPersonalDirectoryMatching;
        }
    if ( iEndOtherCalls )
        {
           iFlags |= EPhCltCallFlagsEndBeforeDial;
        }

    if ( iSATCall )
        {
        iFlags |= EPhCltCallFlagsSAT;
        }

    if ( ( iCallType == EPhCltVideo ) || ( iCallType == EPhCltForcedVideo  ) )
        {
        iFlags |= EPhCltCallFlagsVideoCall;
        }

    if ( iCallType == EPhCltCallVoIP   )
        {
        iFlags |= EPhCltCallFlagsVoIPCall;
        }
      
      if ( iSendKey )
        {
        iFlags |= EPhCltCallFlagsSendKey;
        }

    stream.WriteUint32L( iFlags );   
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2) 
    stream.WriteUint32L( iServiceId );
#endif // __VOIP) && RD_VOIP_REL_2_2   
    WriteDescriptorToStreamL( stream, *iUUI );
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( 1 ); // stream
    
    return package;   
    
      }    

// -----------------------------------------------------------------------------
// CPhCltExtPhoneDialData::SetServiceId
// 
// 
// -----------------------------------------------------------------------------
//
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)  
EXPORT_C void CPhCltExtPhoneDialData::SetServiceId( TUint32 aServiceId )
    {
    iServiceId = aServiceId;
    } 
#else
EXPORT_C void CPhCltExtPhoneDialData::SetServiceId( TUint32 /*aServiceId*/ )
    {  
    }     
#endif // __VOIP && RD_VOIP_REL_2_2 
// -----------------------------------------------------------------------------
// CPhCltExtPhoneDialData::ServiceId
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CPhCltExtPhoneDialData::ServiceId() const
    {
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)    
    return iServiceId;
#else
    return NULL;
#endif // __VOIP && RD_VOIP_REL_2_2         
    }
    
// ---------------------------------------------------------------------------
// Sets dial data values using given package
//
// ---------------------------------------------------------------------------
//
void CPhCltExtPhoneDialData::SetParamsL(
    const TDesC8& aPackage ) 
    {
    RDesReadStream stream( aPackage );
    CleanupClosePushL( stream );
    
    stream >> iTelNumber;
    iCallType = static_cast<TPhCltCallType>( stream.ReadInt32L() );
    stream >> iName;
    iWindowGroup = stream.ReadInt32L();
    iRedial = stream.ReadInt32L();
    iRedialMaximumDuration = stream.ReadInt32L();
    ReadDescriptorFromStreamL( stream, iContactLink );
    stream >> iSubAddress;
    iShowNumber = stream.ReadInt32L();
    iAllowMatch = stream.ReadInt32L();
    iSATCall = stream.ReadInt32L(); 
    iEndOtherCalls = stream.ReadInt32L();
    stream >> iBearer;
    iSendKey = stream.ReadInt32L();
    iReturnToNumberEntryOnFailure = stream.ReadInt32L();
    iInitCall = stream.ReadInt32L();
    iFlags = stream.ReadUint32L();
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)    
    iServiceId = stream.ReadUint32L();
#endif // __VOIP) && RD_VOIP_REL_2_2  
    ReadDescriptorFromStreamL( stream, iUUI );

    CleanupStack::PopAndDestroy( 1 ); // stream;
    }
//  End of File  
