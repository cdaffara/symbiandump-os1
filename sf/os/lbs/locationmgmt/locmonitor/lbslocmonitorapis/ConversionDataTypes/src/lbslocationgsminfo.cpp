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
* cell information in a GSM PLMN. This class may also optionally contain
* neighbouring cell information
*
*/


#include <e32std.h>
#include <lbs/lbslocationgsminfo.h>


// ---------------------------------------------------------------------------
// CPosGsmCellInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsGsmCellInfo* CLbsGsmCellInfo::NewL()
    {
    CLbsGsmCellInfo* self = new( ELeave ) CLbsGsmCellInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsGsmCellInfo* CLbsGsmCellInfo::NewL( const TInt aCountryCode, 
                                                 const TInt aNetworkCode,
                                                 const TInt aLocationAreaCode, 
                                                 const TInt aCellId )
    {
    CLbsGsmCellInfo* self = new( ELeave ) CLbsGsmCellInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aCountryCode, aNetworkCode, aLocationAreaCode, aCellId );
    CleanupStack::Pop();
    return self;
    }

EXPORT_C  CLbsGsmCellInfo* CLbsGsmCellInfo::NewL( const CLbsGsmCellInfo& aPositionInfo )
	{
	CLbsGsmCellInfo* self = CLbsGsmCellInfo::NewL(aPositionInfo.iMCC,aPositionInfo.iMNC,aPositionInfo.iLAC,aPositionInfo.iCid);
	
	self->iRxLev 				=  aPositionInfo.iRxLev;
	self->iTA  					= aPositionInfo.iTA;
	self->iDataValidationMask	= aPositionInfo.iDataValidationMask;

    RLbsGsmNeighbourCellInfoArray neighbourCellInfoArray;
    aPositionInfo.GetNeighbouringCellInfoL(neighbourCellInfoArray);
    
	for (TInt i=0; i < neighbourCellInfoArray.Count(); i++)
		{
		TLbsGsmNeighbourCellInfo neighbourCellInfo = neighbourCellInfoArray[i];
		self->AddNeighbouringCellInfoL(&neighbourCellInfo);
		}
	neighbourCellInfoArray.Close();

	return self;  
    
	}
    

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::~CLbsGsmCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsGsmCellInfo::~CLbsGsmCellInfo()
    {
    ResetNeighbouringCellInfo();
    }
    
// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::SetMobileCountryCodeL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::SetMobileCountryCode( const TInt aCountryCode )
    {
    iMCC = aCountryCode;
    iDataValidationMask |= EGsmMCC;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::MobileCountryCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsGsmCellInfo::MobileCountryCode() const
    {
    return iMCC;
    }
    
// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::SetMobileNetworkCodeL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::SetMobileNetworkCode( const TInt aNetworkCode )
    {
    iMNC = aNetworkCode;
    iDataValidationMask |= EGsmMNC;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::MobileNetworkCodeL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsGsmCellInfo::MobileNetworkCode() const
    {
    return iMNC;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::SetLocationAreaCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::SetLocationAreaCode( const TInt aLocationAreaCode )
    {
    iLAC = aLocationAreaCode;
    iDataValidationMask |= EGsmLAC;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::LocationAreaCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsGsmCellInfo::LocationAreaCode() const
    {
    return iLAC;
    }
        
// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::SetCellId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::SetCellId( const TInt aCellId )
    {
    iCid = aCellId;
    iDataValidationMask |= EGsmCI;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::CellId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsGsmCellInfo::CellId() const
    {
    return iCid;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::SetSetRxLevel()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::SetRxLevel( const TInt aRxLev )
    {
    iRxLev = aRxLev;
    iDataValidationMask |= EGsmRx;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::RxLevel()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsGsmCellInfo::RxLevel() const
    {
    return iRxLev;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::SetTimingAdvance()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::SetTimingAdvance( const TInt aTimingAdvance )
    {
    iTA = aTimingAdvance;
    iDataValidationMask |= EGsmTA;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::TimingAdvance()
// other items were commented in the header
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CLbsGsmCellInfo::TimingAdvance() const
    {
    return iTA;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::SetNeighbouringCellInfoList()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::AddNeighbouringCellInfoL( 
                                                 TLbsGsmNeighbourCellInfo*
                                                 aNeighbourCellInfo )
    {
    if( !aNeighbourCellInfo  )
        {
        User::Leave( KErrArgument );
        }
    
    TLbsGsmNeighbourCellInfo neighbourCellInfo;
    neighbourCellInfo.SetArfcn( aNeighbourCellInfo->Arfcn() );
    neighbourCellInfo.SetBsic( aNeighbourCellInfo->Bsic() );
    neighbourCellInfo.SetRxLevel( aNeighbourCellInfo->RxLevel() );
    
    iNeighbourCellInfo.Append( neighbourCellInfo );
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::GetNeighbouringCellInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::GetNeighbouringCellInfoL( 
                                        RLbsGsmNeighbourCellInfoArray&
                                        aNeighbourCellInfo ) const
    {
    for( TInt i = 0; i < iNeighbourCellInfo.Count(); i++ )
        {
        aNeighbourCellInfo.AppendL( iNeighbourCellInfo[i] );
        }
    }


// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::ResetNeighbouringCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsGsmCellInfo::ResetNeighbouringCellInfo()
    {
    iNeighbourCellInfo.Reset();
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::Type()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
TLbsAreaInfoClassType CLbsGsmCellInfo::Type()
    {
    return ELbsAreaGsmCellInfoClass;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::DoInternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsGsmCellInfo::DoInternaliseL( RReadStream& aStream )
    {
    iMCC = aStream.ReadInt16L();
    iMNC = aStream.ReadInt16L();
    iLAC = aStream.ReadInt32L();
    iCid = aStream.ReadInt32L();
    iRxLev = aStream.ReadInt8L();
    iTA = aStream.ReadInt16L();
    TInt count = aStream.ReadInt8L();
    for( TInt i = 0;i < count;i++ )
        {
        TLbsGsmNeighbourCellInfo cellInfo;
        cellInfo.SetArfcn( aStream.ReadInt16L() );
        cellInfo.SetBsic( aStream.ReadInt8L() );
        cellInfo.SetRxLevel( aStream.ReadInt8L() );
        iNeighbourCellInfo.AppendL( cellInfo );
        }   
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::DoExternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsGsmCellInfo::DoExternaliseL( RWriteStream& aStream ) const
    {
    aStream.WriteInt16L( iMCC );
    aStream.WriteInt16L( iMNC );
    aStream.WriteInt32L( iLAC );
    aStream.WriteInt32L( iCid );
    aStream.WriteInt8L( iRxLev );
    aStream.WriteInt16L( iTA );
    TInt count = iNeighbourCellInfo.Count();
    aStream.WriteInt8L( count );
    for( TInt i = 0;i < count;i++ )
        {
        aStream.WriteInt16L( iNeighbourCellInfo[i].Arfcn() );
        aStream.WriteInt8L( iNeighbourCellInfo[i].Bsic() );
        aStream.WriteInt8L( iNeighbourCellInfo[i].RxLevel() );
        }   
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::ValidateAreaInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsGsmCellInfo::ValidateDataL() const
    {
    // The following condition is to check the hierarchy of data values specified 
    if( ( iMCC < 0 || iMCC > 999 ) || ( iMNC < 0 && iLAC > -1 ) || 
        ( iLAC < 0 && iCid > -1 ) || ( iCid < 0 && iTA > -1 ) || 
        ( iCid < 0 && iRxLev > -1 ) ||
        ( iCid < 0 && iNeighbourCellInfo.Count() ) )
        User::Leave( KErrArgument );

    // Check for range values if they are set specifically by clients
    if( iDataValidationMask & EGsmMNC )
        {
        if( iMNC < 0 || iMNC > 999 )
            User::Leave( KErrArgument );
        }
    if( iDataValidationMask & EGsmLAC )
        {
        if( iLAC < 0 || iLAC > 65535 )
            User::Leave( KErrArgument );
        }
    if( iDataValidationMask & EGsmCI )
        {
        if( iCid < 0 || iCid > 65535 )
            User::Leave( KErrArgument );
        }
    if( iDataValidationMask & EGsmRx )
        {
        if( iRxLev < 0 || iRxLev > 63 )
            User::Leave( KErrArgument );
        }
    if( iDataValidationMask & EGsmTA )
        {
        if( iTA < 0 || iTA > 255 )
            User::Leave( KErrArgument );
        }
    
    // Check neighbouring cell information
    for( TInt i = 0; i < iNeighbourCellInfo.Count(); i++ )
        {
        if( iNeighbourCellInfo[i].Arfcn() < 0 || iNeighbourCellInfo[i].Arfcn() > 1023 ||
            iNeighbourCellInfo[i].Bsic() < 0  || iNeighbourCellInfo[i].Bsic() > 63 ||
            iNeighbourCellInfo[i].RxLevel() < 0 || iNeighbourCellInfo[i].RxLevel() > 63 )
            {
            User::Leave( KErrArgument );
            }
        }
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::CLbsGsmCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
CLbsGsmCellInfo::CLbsGsmCellInfo()
    {
    }
    
// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::CLbsGsmCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsGsmCellInfo::ConstructL()
    {
    iMCC = -1;
    iMNC = -1;
    iLAC = -1;
    iCid = -1;
    iRxLev = -1;
    iTA = -1;
    iDataValidationMask = EGsmDataNull;
    }

// ---------------------------------------------------------------------------
// CLbsGsmCellInfo::CLbsGsmCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsGsmCellInfo::ConstructL( const TInt aCountryCode, const TInt aNetworkCode,
                                  const TInt aLocationAreaCode, const TInt aCellId )
    {
    SetMobileCountryCode( aCountryCode );
    SetMobileNetworkCode( aNetworkCode );
    SetLocationAreaCode( aLocationAreaCode );
    SetCellId( aCellId );
    iRxLev = -1;
    iTA = -1;
    }
