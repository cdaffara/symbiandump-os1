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
* WLAN MAC ID. 
*
*/


#include <e32std.h>
#include <lbs/lbslocationwlaninfo.h>


// ---------------------------------------------------------------------------
// CLbsWlanInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsWlanInfo* CLbsWlanInfo::NewL()
    {
    CLbsWlanInfo* self = new( ELeave ) CLbsWlanInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::~CLbsWlanInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C  CLbsWlanInfo* CLbsWlanInfo::NewL( const CLbsWlanInfo& aPositionInfo)
	{

	TLbsWlanMacAddress macAddress;
	aPositionInfo.GetMacAddress(macAddress);
	TInt16 powRx = aPositionInfo.SignalStrength();

	CLbsWlanInfo* self = CLbsWlanInfo::NewL( macAddress, powRx);

	return self;  
	}
// ---------------------------------------------------------------------------
// CLbsWlanInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsWlanInfo* CLbsWlanInfo::NewL( const TLbsWlanMacAddress& MacAddress,
                                           const TInt aPowRx )
    {
    CLbsWlanInfo* self = new( ELeave ) CLbsWlanInfo;
    CleanupStack::PushL( self );
    self->ConstructL( MacAddress, aPowRx );
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------------------------
// CLbsWlanInfo::~CLbsWlanInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsWlanInfo::~CLbsWlanInfo()
    {
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::SetMacId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWlanInfo::SetMacAddress( const TDesC8& aMacAddress  )
    {
    iMacAddress = aMacAddress;
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::MacId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWlanInfo::GetMacAddress(TDes8& aMacAddress ) const
    {
    aMacAddress = iMacAddress;
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::SetSignalStrength()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWlanInfo::SetSignalStrength( const TInt aPowRx )
    {
    iPowRx = aPowRx;
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::SignalStrength()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsWlanInfo::SignalStrength() const
    {
    return iPowRx;
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::Type()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
TLbsAreaInfoClassType CLbsWlanInfo::Type()
    {
    return ELbsAreaWlanInfoClass;
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::DoInternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWlanInfo::DoInternaliseL( RReadStream& aStream )
    {
    TInt len = aStream.ReadInt32L();
    aStream.ReadL( iMacAddress, len );
    iPowRx = aStream.ReadInt16L();
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::DoExternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWlanInfo::DoExternaliseL( RWriteStream& aStream )const
    {
    TInt len = iMacAddress.Length();
    aStream.WriteInt32L( len );
    aStream.WriteL( iMacAddress,len );
    aStream.WriteInt16L( iPowRx );
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::ValidateAreaInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWlanInfo::ValidateDataL() const
    {
    if( !iMacAddress.Length() )
        User::Leave( KErrArgument );
        
    if( iPowRx < 0 || iPowRx > 255 ) 
        User::Leave( KErrArgument );
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::CLbsWlanInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
CLbsWlanInfo::CLbsWlanInfo()
    {
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::ConstructL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWlanInfo::ConstructL()
    {
    iMacAddress.FillZ();
    iPowRx = -1;
    }

// ---------------------------------------------------------------------------
// CLbsWlanInfo::ConstructL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWlanInfo::ConstructL( const TLbsWlanMacAddress& MacAddress,
                               const TInt aPowRx )
    {
    SetMacAddress( MacAddress );
    SetSignalStrength( aPowRx );
    }
