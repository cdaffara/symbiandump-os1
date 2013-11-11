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
* Description: Container class representing geographical area information as a
* collection of GSM/WCDMA cell information and WLAN MAC IDs.
*
*/


#include <e32std.h>
#include <lbs/lbslocationclasstypes.h>
#include <lbs/lbslocationinfo.h>


// ---------------------------------------------------------------------------
// CLbsLocationInfo::NewL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsLocationInfo* CLbsLocationInfo::NewL()
    {
    CLbsLocationInfo* self = new( ELeave ) CLbsLocationInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::NewLC()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsLocationInfo* CLbsLocationInfo::NewLC()
    {
    CLbsLocationInfo* self = new( ELeave ) CLbsLocationInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::~CLbsLocationInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C CLbsLocationInfo::~CLbsLocationInfo()
    {
    iAreaInfoArray.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::AddAreaInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsLocationInfo::AddAreaInfoL( CLbsAreaInfoBase* 
                                              aAreaInfo )
    {
    if( !aAreaInfo )
        {
        User::Leave( KErrArgument );
        }
    iAreaInfoArray.AppendL( aAreaInfo );
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::GetAreaInfoL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsLocationInfo::GetAreaInfoL( RLbsAreaInfoBaseArray& aAreaInfoArray, 
                                             TAreaInfoMask aAreaInfoMask )
    {
	
    for( TInt i = 0; i < iAreaInfoArray.Count(); i++ )
        {
        switch( iAreaInfoArray[i]->Type() )
            {
            case ELbsAreaPositionInfoClass:
                if( aAreaInfoMask & ELbsPosInfo )
                    {
					// an object that is a copy of the CLbsPositionInfo
					// object that is owned by the CLbsAreaInfoBase object
					// is created. Note, ownership of this is passed to
					// the caller of GetAreaInfo so the caller needs to do
					// a ResetAndDestroy on the array to clean up.
		                
  					CLbsPositionInfo* posInfo = static_cast<CLbsPositionInfo*>( iAreaInfoArray[i] );
			           
					CLbsPositionInfo* copyOfPosInfo = CLbsPositionInfo::NewL(*posInfo);
				    CleanupStack::PushL(copyOfPosInfo );
					aAreaInfoArray.AppendL(copyOfPosInfo);
					CleanupStack::Pop(copyOfPosInfo);    // ownership transferred to instance of CLbsLocationInfo
					  
	                }
                break;
                
            case ELbsAreaGsmCellInfoClass:
                if( aAreaInfoMask & ELbsGsmInfo )
                    {
					CLbsGsmCellInfo* cellInfo = static_cast<CLbsGsmCellInfo*>( iAreaInfoArray[i]);
					CLbsGsmCellInfo* copyOfGsmCellInfo = CLbsGsmCellInfo::NewL(*cellInfo);
				    CleanupStack::PushL(copyOfGsmCellInfo );
					aAreaInfoArray.AppendL( copyOfGsmCellInfo );
					CleanupStack::Pop(copyOfGsmCellInfo);    // ownership transferred to instance of CLbsLocationInfo

                    }
                break;
                
            case ELbsAreaWcmdaCellInfoClass:
                if( aAreaInfoMask & ELbsWcdmaInfo )
                    {
					CLbsWcdmaCellInfo* wcdaCellInfo = static_cast<CLbsWcdmaCellInfo*>( iAreaInfoArray[i]);
					CLbsWcdmaCellInfo* copyOfWcdaCellInfo = CLbsWcdmaCellInfo::NewL(*wcdaCellInfo);
				    CleanupStack::PushL(copyOfWcdaCellInfo );
					aAreaInfoArray.AppendL(copyOfWcdaCellInfo);
				    CleanupStack::Pop(copyOfWcdaCellInfo ); // ownership transferred to instance of CLbsLocationInfo
                    }
                break;
            
            case ELbsAreaWlanInfoClass:
                if( aAreaInfoMask & ELbsWlanInfo )
                    {
					CLbsWlanInfo* wlanInfo = static_cast<CLbsWlanInfo*>( iAreaInfoArray[i]);
					CLbsWlanInfo* copyOfWlanInfo = CLbsWlanInfo::NewL(*wlanInfo);
				    CleanupStack::PushL(copyOfWlanInfo );
					aAreaInfoArray.AppendL(copyOfWlanInfo);
				    CleanupStack::Pop(copyOfWlanInfo ); // ownership transferred to instance of CLbsLocationInfo
                    }
                break;
                
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::ResetAreaInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void CLbsLocationInfo::ResetAreaInfo( TAreaInfoMask aAreaInfoMask  )
    {
    // If the mask reflects reset all,then reset and destroy is called on 
    // areaInfoArray directly
    if( aAreaInfoMask == ELbsAreaInfoAll )
        {
        iAreaInfoArray.ResetAndDestroy();
        return;
        }
    
    for( TInt i = 0; i < iAreaInfoArray.Count(); i++ )
        {
        switch( iAreaInfoArray[i]->Type() )
            {
            case ELbsAreaPositionInfoClass:
                if( aAreaInfoMask & ELbsPosInfo )
                    {
                    CLbsAreaInfoBase* areaInfo = iAreaInfoArray[i];
                    iAreaInfoArray.Remove( i );
                    delete areaInfo;
                    }
                break;
                
            case ELbsAreaGsmCellInfoClass:
                if( aAreaInfoMask & ELbsGsmInfo )
                    {
                    CLbsAreaInfoBase* areaInfo = iAreaInfoArray[i];
                    iAreaInfoArray.Remove( i );
                    delete areaInfo;
                    }
                break;
                
            case ELbsAreaWcmdaCellInfoClass:
                if( aAreaInfoMask & ELbsWcdmaInfo )
                    {
                    CLbsAreaInfoBase* areaInfo = iAreaInfoArray[i];
                    iAreaInfoArray.Remove( i );
                    delete areaInfo;
                    }
                break;
            
            case ELbsAreaWlanInfoClass:
                if( aAreaInfoMask & ELbsWlanInfo )
                    {
                    CLbsAreaInfoBase* areaInfo = iAreaInfoArray[i];
                    iAreaInfoArray.Remove( i );
                    delete areaInfo;
                    }
                break;
                
            default:
                break;
            }
        }
    }


// ---------------------------------------------------------------------------
// CLbsLocationInfo::Type()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsLocationInfoClassType CLbsLocationInfo::Type()
    {
    return ELbsLocationInfoClass;
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::DoInternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsLocationInfo::DoInternaliseL( RReadStream& aStream )
    {
    TInt count = aStream.ReadInt8L();
    for( TInt i = 0; i < count; i++ )
        {
        TLbsAreaInfoClassType type = aStream.ReadUint16L();
        CLbsAreaInfoBase* area = NULL;
        if( ELbsAreaPositionInfoClass == type )
            {
            area = CLbsPositionInfo::NewL();
            }
        if( ELbsAreaGsmCellInfoClass == type )
            {
            area = CLbsGsmCellInfo::NewL();
            }
        if( ELbsAreaWcmdaCellInfoClass == type )
            {
            area = CLbsWcdmaCellInfo::NewL();
            }
        if( ELbsAreaWlanInfoClass  == type )
            {
            area = CLbsWlanInfo::NewL();
            }
        if( area )
            {
            area->InternaliseL( aStream );
            iAreaInfoArray.AppendL( area );
            }
        }
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::DoExternaliseL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsLocationInfo::DoExternaliseL( RWriteStream& aStream ) const
    {
    TInt count = iAreaInfoArray.Count();
    aStream.WriteInt8L( count );
    for( TInt i = 0; i < count; i++ )
        {
        TLbsAreaInfoClassType type = iAreaInfoArray[i]->Type();
        aStream.WriteUint16L( type );
        iAreaInfoArray[i]->ExternaliseL( aStream );
        }
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::CLbsLocationInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
CLbsLocationInfo::CLbsLocationInfo()
    {
    }

// ---------------------------------------------------------------------------
// CLbsLocationInfo::ConstructL()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
void CLbsLocationInfo::ConstructL()
    {
    }
