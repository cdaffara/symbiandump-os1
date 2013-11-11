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
* cell information in a WCDMA PLMN. This class may also optionally contain
* neighbouring cell information
*
*/


#include <e32std.h>
#include <lbs/lbslocationwcdmainfo.h>


// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsWcdmaCellInfo* CLbsWcdmaCellInfo::NewL()
    {
    CLbsWcdmaCellInfo* self = new( ELeave ) CLbsWcdmaCellInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }
  
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsWcdmaCellInfo* CLbsWcdmaCellInfo::NewL( const TInt aCountryCode,
                                               const TInt aNetworkCode,
                                               const TInt aUniqueCellId )
    {
    CLbsWcdmaCellInfo* self = new( ELeave ) CLbsWcdmaCellInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aCountryCode, aNetworkCode, (-1), aUniqueCellId );
    CleanupStack::Pop();
    return self;    
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsWcdmaCellInfo* CLbsWcdmaCellInfo::NewL( const TInt aCountryCode,
                                               const TInt aNetworkCode,
											   const TInt aLocalAreaCode,
                                               const TInt aUniqueCellId )
    {
    CLbsWcdmaCellInfo* self = new( ELeave ) CLbsWcdmaCellInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aCountryCode, aNetworkCode, aLocalAreaCode, aUniqueCellId );
    CleanupStack::Pop();
    return self;    
    }



EXPORT_C  CLbsWcdmaCellInfo* CLbsWcdmaCellInfo::NewL( const CLbsWcdmaCellInfo& aPositionInfo )
	{
	
	CLbsWcdmaCellInfo* self = CLbsWcdmaCellInfo::NewL( aPositionInfo.iMCC,aPositionInfo.iMNC, aPositionInfo.iLAC, aPositionInfo.iUCid);
	self->iSCode = aPositionInfo.iSCode;
	
	RLbsWcdmaNeighbourCellInfoArray  neighbourCellInfoArray;
	aPositionInfo.GetNeighbouringCellInfoL(neighbourCellInfoArray);
		
	for (TInt i=0; i < neighbourCellInfoArray.Count(); i++)
		{
		TLbsWcdmaNeighbourCellInfo neighbourCellInfo = neighbourCellInfoArray[i];
		self->AddNeighbouringCellInfoL(&neighbourCellInfo);
		}
		
	self->iDataValidationMask = aPositionInfo.iDataValidationMask;
		
	neighbourCellInfoArray.Close();

	return self;  
	}
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::~CLbsWcdmaCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsWcdmaCellInfo::~CLbsWcdmaCellInfo()
    {
    ResetNeighbouringCellInfo();
    }
    
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SetMobileCountryCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::SetMobileCountryCode( const TInt aCountryCode )
    {
    iMCC = aCountryCode;
    iDataValidationMask |= EWcdmaMCC;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::MobileCountryCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsWcdmaCellInfo::MobileCountryCode() const
    {
    return iMCC;
    }
    
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SetMobileNetworkCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::SetMobileNetworkCode( const TInt aNetworkCode )
    {
    iMNC = aNetworkCode;
    iDataValidationMask |= EWcdmaMNC;
    }


    
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::MobileNetworkCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsWcdmaCellInfo::MobileNetworkCode() const
    {
    return iMNC;
    }


// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SetLocalAreaCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::SetLocalAreaCode( const TInt aLocalAreaCode )
    {
    iLAC = aLocalAreaCode;
    iDataValidationMask |= EWcdmaLAC;
    }


    
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::LocalAreaCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsWcdmaCellInfo::LocalAreaCode(void) const
    {
    return iLAC;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SetUniqueCellId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::SetUniqueCellId( const TInt aUniqueCellId )
    {
    iUCid = aUniqueCellId;
    iDataValidationMask |= EWcdmaUCid;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::UniqueCellId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsWcdmaCellInfo::UniqueCellId() const
    {
    return iUCid;
    }
    
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SetScramblingCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::SetScramblingCode( const TInt aScramblingCode )
    {
    iSCode = aScramblingCode;
    iDataValidationMask |= EWcdmaSC;
    }
    
// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::ScramblingCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsWcdmaCellInfo::ScramblingCode() const
    {
    return iSCode;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SetSignalStrength()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::SetSignalStrength( const TInt aRssi )
    {
    iRssi = aRssi;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SignalStrength()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLbsWcdmaCellInfo::SignalStrength() const 
    {
    return iRssi;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::AddNeighbouringCellInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::AddNeighbouringCellInfoL(
                                                TLbsWcdmaNeighbourCellInfo* 
                                                aNeighbourCellInfo )
    {
    if( !aNeighbourCellInfo )
        {
        User::Leave( KErrArgument ); 
        }
    TLbsWcdmaNeighbourCellInfo wcdmaNeighbourCellInfo;
    wcdmaNeighbourCellInfo.SetUniqueCellId( aNeighbourCellInfo->UniqueCellId());
    wcdmaNeighbourCellInfo.SetScramblingCode( aNeighbourCellInfo->ScramblingCode() );
    wcdmaNeighbourCellInfo.SetSignalStrength( aNeighbourCellInfo->SignalStrength() );
    wcdmaNeighbourCellInfo.SetPathloss( aNeighbourCellInfo->Pathloss() );
    iNeighbourCellInfo.AppendL( wcdmaNeighbourCellInfo );
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::SetNeighbouringCellInfoList()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::GetNeighbouringCellInfoL( 
                                       RLbsWcdmaNeighbourCellInfoArray& 
                                       aNeighbourCellInfoList ) const
    {
    for( TInt i = 0; i < iNeighbourCellInfo.Count(); i++ )
        {
        aNeighbourCellInfoList.AppendL( iNeighbourCellInfo[i] );
        }    
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::ResetNeighbouringCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsWcdmaCellInfo::ResetNeighbouringCellInfo()
    {
    iNeighbourCellInfo.Reset();
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::Type()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
TLbsAreaInfoClassType CLbsWcdmaCellInfo::Type()
    {
    return ELbsAreaWcmdaCellInfoClass;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::DoInternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWcdmaCellInfo::DoInternaliseL( RReadStream& aStream )
    {
    iMCC = aStream.ReadInt16L();
    iMNC = aStream.ReadInt16L();
    iUCid = aStream.ReadInt32L();
    iSCode = aStream.ReadInt16L();
    iRssi  = aStream.ReadInt16L();
    
    TInt count = aStream.ReadInt16L();
    for( TInt i = 0; i < count; i++ )
        {
        TLbsWcdmaNeighbourCellInfo cellInfo;
        cellInfo.SetUniqueCellId( aStream.ReadInt32L() );
        cellInfo.SetScramblingCode( aStream.ReadInt16L() );
        cellInfo.SetPathloss( aStream.ReadInt16L() );
        cellInfo.SetSignalStrength( aStream.ReadInt16L() );
        iNeighbourCellInfo.Append( cellInfo );
        }
    
	iLAC = aStream.ReadInt32L();
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::DoExternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWcdmaCellInfo::DoExternaliseL( RWriteStream& aStream )const
    {
    aStream.WriteInt16L( iMCC );
    aStream.WriteInt16L( iMNC );
    aStream.WriteInt32L( iUCid );
    aStream.WriteInt16L( iSCode );
    aStream.WriteInt16L( iRssi );
    
    TInt count = iNeighbourCellInfo.Count();
    aStream.WriteInt16L( count );
    for( TInt i = 0; i < count; i++ )
        {
        aStream.WriteInt32L( iNeighbourCellInfo[i].UniqueCellId() );
        aStream.WriteInt16L( iNeighbourCellInfo[i].ScramblingCode() );
        aStream.WriteInt16L( iNeighbourCellInfo[i].Pathloss() );
        aStream.WriteInt16L( iNeighbourCellInfo[i].SignalStrength() );
        }

    aStream.WriteInt32L( iLAC );

    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::ValidateAreaInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWcdmaCellInfo::ValidateDataL() const
    {
    // The following condition is to check the hierarchy of data values specified 
    if( ( iMCC < 0 || iMCC > 999 ) || 
        ( iMNC < 0 && iUCid > -1 ) || 
        ( iUCid < 0 && iSCode > -1 ) || 
        ( iUCid < 0 && iRssi > -1 ) ||
        ( iUCid < 0 && iNeighbourCellInfo.Count() ) )
        User::Leave( KErrArgument );
    
    // Check for range values if they are set specifically by clients
    if( iDataValidationMask & EWcdmaMNC )
        {
        if( iMNC < 0 || iMNC > 999 )
            User::Leave( KErrArgument );
        }
    if( iDataValidationMask & EWcdmaUCid )
        {
        if( iUCid < 0 || iUCid > 268435455 )
            User::Leave( KErrArgument );
        }
    if( iDataValidationMask & EWcdmaSC )
        {
        if( iSCode < 0 || iSCode > 511 )
            User::Leave( KErrArgument );
        }
    if ( iDataValidationMask & EWcdmaRSSI )
        {
        if( iRssi < 0 || iRssi > 127 )
            User::Leave( KErrArgument );
        }
    
    for( TInt i = 0; i < iNeighbourCellInfo.Count(); i++ )
        {
        if( iNeighbourCellInfo[i].UniqueCellId() < 0 || 
                iNeighbourCellInfo[i].UniqueCellId() > 268435455 ||
                iNeighbourCellInfo[i].ScramblingCode() < 0  || 
                iNeighbourCellInfo[i].ScramblingCode() > 511 ||
                iNeighbourCellInfo[i].Pathloss() < 46 || 
                iNeighbourCellInfo[i].Pathloss() > 173 ||
                iNeighbourCellInfo[i].SignalStrength() < 0 || 
                iNeighbourCellInfo[i].SignalStrength() > 127 )
            {
            User::Leave( KErrArgument );
            }
        }

	if ( iDataValidationMask & EWcdmaLAC)
		{
		if (iLAC > 0xffff)
			{
            User::Leave( KErrArgument );
			}
		}
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::CLbsWcdmaCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
CLbsWcdmaCellInfo::CLbsWcdmaCellInfo()
    {
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::ConstructL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsWcdmaCellInfo::ConstructL()
    {
    iMCC = -1;
    iMNC = -1;
    iUCid = -1;
    iSCode = -1;
    iRssi  = -1;
	iLAC = -1;
    iDataValidationMask = EWcdmaDataNull;
    }

// ---------------------------------------------------------------------------
// CLbsWcdmaCellInfo::ConstructL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//

void CLbsWcdmaCellInfo::ConstructL( const TInt aCountryCode, 
                                    const TInt aNetworkCode,
									const TInt aLocalAreaCode,
                                    const TInt aUniqueCellId )
    {
    SetMobileCountryCode( aCountryCode );
    SetMobileNetworkCode( aNetworkCode );
    SetUniqueCellId( aUniqueCellId );
	if (aLocalAreaCode == -1)
		{
		iLAC = -1;
		}
	else
		{
		SetLocalAreaCode(aLocalAreaCode);
		}
    iSCode = -1;
    iRssi  = -1;
    }

