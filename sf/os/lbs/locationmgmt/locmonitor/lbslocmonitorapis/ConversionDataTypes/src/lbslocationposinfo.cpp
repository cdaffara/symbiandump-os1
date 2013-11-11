/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Data class representing a geographical area in terms of 
* latitude,longitude,altitude,horizontal accuracy and vertical accuracy.
*
*/


#include <e32std.h>
#include <e32math.h>
#include <lbs/lbslocationposinfo.h>


// ---------------------------------------------------------------------------
// CLbsPositionInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsPositionInfo* CLbsPositionInfo::NewL()
    {
    CLbsPositionInfo* self = new( ELeave ) CLbsPositionInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;  
    }

EXPORT_C CLbsPositionInfo* CLbsPositionInfo::NewL( const CLbsPositionInfo& aPositionInfo )
	{
	TLocality locality;
	
	aPositionInfo.GetLocality(locality);
    CLbsPositionInfo* self = CLbsPositionInfo::NewL( locality );
    return self;  

	}

// ---------------------------------------------------------------------------
// CLbsPositionInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsPositionInfo* CLbsPositionInfo::NewL( const TLocality& aLocality )
    {
    CLbsPositionInfo* self = new( ELeave ) CLbsPositionInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aLocality );
    CleanupStack::Pop();
    return self;  
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::~CLbsPositionInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsPositionInfo::~CLbsPositionInfo()
    {
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::SetLocality()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsPositionInfo::SetLocality( 
                            const TLocality& aLocality )
    {
    iLocality = aLocality;
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::Locality()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsPositionInfo::GetLocality( TLocality& aLocality ) const
    {
    aLocality = iLocality;
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::Type()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
TLbsAreaInfoClassType CLbsPositionInfo::Type()
    {
    return ELbsAreaPositionInfoClass;
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::ValidateAreaInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsPositionInfo::ValidateDataL() const
    {
    if( Math::IsNaN( iLocality.Latitude() ) || 
        Math::IsNaN( iLocality.Longitude() ) ||
        iLocality.HorizontalAccuracy() < 0.0 ) 
        {
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::DoInternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsPositionInfo::DoInternaliseL( RReadStream& aStream )
    {
    TReal64 latitude = aStream.ReadReal64L();
    TReal64 longitude = aStream.ReadReal64L();
    TReal32  altitude = aStream.ReadReal32L();
    iLocality.SetCoordinate( latitude, longitude, altitude );
    TInt datum = aStream.ReadInt32L();
    iLocality.SetDatum( TUid::Uid( datum ) );
    TReal32 hAccuracy = aStream.ReadReal32L();
    TReal32 vAccuracy = aStream.ReadReal32L();
    iLocality.SetAccuracy( hAccuracy, vAccuracy );    
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::DoExternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsPositionInfo::DoExternaliseL( RWriteStream& aStream ) const
    {
    aStream.WriteReal64L( iLocality.Latitude() );
    aStream.WriteReal64L( iLocality.Longitude() );
    aStream.WriteReal32L( iLocality.Altitude() );
    aStream.WriteInt32L( iLocality.Datum().iUid );
    aStream.WriteReal32L( iLocality.HorizontalAccuracy() );
    aStream.WriteReal32L( iLocality.VerticalAccuracy() );
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::CLbsPositionInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
CLbsPositionInfo::CLbsPositionInfo()
    {
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::ConstructL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsPositionInfo::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CLbsPositionInfo::ConstructL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsPositionInfo::ConstructL( const TLocality& aLocality )
    {
    SetLocality( aLocality );
    }
