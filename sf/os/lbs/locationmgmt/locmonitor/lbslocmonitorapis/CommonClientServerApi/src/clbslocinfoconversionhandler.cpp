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
* Contributors:
*
* Description: Implementation of CLbsLocInfoConversionHandler class.
*
*/

#include <s32strm.h>
#include <s32mem.h>
#include <lbs/lbslocationinfo.h>
#include "rlbslocinfoconverter.h"
#include "clbslocinfoconversionhandler.h"


//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::NewL
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocInfoConversionHandler* CLbsLocInfoConversionHandler::NewL( 
                            RLbsLocInfoConverter& aLocInfoConverter )
    {
    CLbsLocInfoConversionHandler* self = new ( ELeave ) 
                                CLbsLocInfoConversionHandler( aLocInfoConverter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::CLbsLocInfoConversionHandler
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocInfoConversionHandler::CLbsLocInfoConversionHandler( 
                            RLbsLocInfoConverter& aLocInfoConverter ):
                            CActive( EPriorityStandard ),
                            iLocInfoConverter( aLocInfoConverter )
    {
    CActiveScheduler::Add( this );
    }
                            
//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::SetBuffer
// (other items were commented in a header).
//----------------------------------------------------------------     
void CLbsLocInfoConversionHandler::SetBuffer( CBufFlat* aBuffer )
    {
    if( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }
    iBuffer = aBuffer;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::SetStatus
// (other items were commented in a header).
//----------------------------------------------------------------     
void CLbsLocInfoConversionHandler:: SetStatus( TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::SetClientLocationInfoArray
// (other items were commented in a header).
//----------------------------------------------------------------     
void CLbsLocInfoConversionHandler::SetClientLocationInfoArray( 
                                  RLbsLocationInfoBaseArray*
                                  aLocationInfoArray )
    {
    iClientLocationInfoList = aLocationInfoArray;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::SetClientLocationInfo
// (other items were commented in a header).
//----------------------------------------------------------------     
void CLbsLocInfoConversionHandler::SetClientLocationInfo(
                              CLbsLocationInfoBase& aLocationInfo )
    {
    iClientLocationInfo = &aLocationInfo;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::SetClientLocationInfo
// (other items were commented in a header).
//----------------------------------------------------------------     
void CLbsLocInfoConversionHandler::SetClientRequestedInfo( 
                            TLbsConversionOutputInfoMask aRequestedInfo )
    {
    iRequestedInfo = aRequestedInfo;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::ReleaseAllResources
// (other items were commented in a header).
//----------------------------------------------------------------     
void CLbsLocInfoConversionHandler::ReleaseAllResources()
    {
    if( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }
    iClientLocationInfoList = NULL;
    iClientLocationInfo = NULL;
    iClientStatus = NULL;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::~CLbsLocInfoConversionHandler
// (other items were commented in a header).
//----------------------------------------------------------------
CLbsLocInfoConversionHandler::~CLbsLocInfoConversionHandler()
    {
    if( IsActive() )
        {
        Cancel();
        }
    delete iBuffer;
    iBuffer = NULL;
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::RunL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocInfoConversionHandler::RunL()
    {
    TInt error = iStatus.Int();
    if( error == KErrNone )
        {
        switch( iRequestType )
            {
            case EGetBuffersizeForSingleArea:
                {
                iLocInfoConverter.HandleSingleConversionBufferSizeL();
                break;
                }
            case EGetSingleConvertedArea:
                {
                HandleSingleConversionL();
                break;
                }
            case EGetBuffersizeForMultipleArea:
                {
                iLocInfoConverter.HandleMultipleConversionBufferSizeL();
                break;
                }
            case EGetMultipleConvertedArea:
                {
                HandleMultipleConversionL();
                break;
                }
            }
        }
    // Incase of error, report it to the client through observer.
    else
        {
        User::RequestComplete( iClientStatus,error );
        }
    }


//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::HandleSingleConversionL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocInfoConversionHandler::HandleSingleConversionL()
    {
    RBufReadStream readStream( *iBuffer );
    CleanupClosePushL( readStream );
    
    CLbsLocationInfo* clientLocInfo = static_cast<CLbsLocationInfo*> 
                                                       ( iClientLocationInfo );
    
    // Get the client requested area info from location info object.
    switch( iRequestedInfo )
        {
        case ELbsConversionOutputPosition:
            GetPositionInfoArrayL( readStream,*clientLocInfo );
            break;
            
        case ELbsConversionOutputGsm:
            GetGsmCellInfoArrayL( readStream,*clientLocInfo );
            break;
            
        case ELbsConversionOutputWcdma:
            GetWcdmaCellInfoArrayL( readStream,*clientLocInfo );
            break;
            
        case ELbsConversionOutputWlan:
            GetWlanInfoArrayL( readStream,*clientLocInfo );
            break;
        
        }
    
    // Complete client status
    CleanupStack::PopAndDestroy( &readStream ); 
    User::RequestComplete( iClientStatus,KErrNone );
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::HandleMultipleConversionL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocInfoConversionHandler::HandleMultipleConversionL()
    {
    RBufReadStream readStream( *iBuffer );
    CleanupClosePushL( readStream );
    
    // Extract the converted info array from the stream.
    TInt locInfoArrayCount = readStream.ReadInt8L();
    
    // Get the client requested area info from the each object of location 
    // info in the array
    for( TInt i=0;i<locInfoArrayCount;i++ )
        {
        // Each element in the client's array are extracted to fill the corresponding 
        // converted area info.
        CLbsLocationInfo* clientLocInfo = static_cast<CLbsLocationInfo*> 
                                                           ( (*iClientLocationInfoList)[i] );
        
        switch( iRequestedInfo )
            {
            case ELbsConversionOutputPosition:
                GetPositionInfoArrayL( readStream,*clientLocInfo );
                break;
                
            case ELbsConversionOutputGsm:
                GetGsmCellInfoArrayL( readStream,*clientLocInfo );
                break;
                
            case ELbsConversionOutputWcdma:
                GetWcdmaCellInfoArrayL( readStream,*clientLocInfo );
                break;
                
            case ELbsConversionOutputWlan:
                GetWlanInfoArrayL( readStream,*clientLocInfo );
                break;
            }     
        }
    
    // Complete client status
    CleanupStack::PopAndDestroy( &readStream ); 
    User::RequestComplete( iClientStatus,KErrNone );
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::DoCancel
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocInfoConversionHandler::DoCancel()
    {
    User::RequestComplete(iClientStatus, iStatus.Int());
    }

//----------------------------------------------------------------
// CLbsLocInfoConversionHandler::ConstructL
// (other items were commented in a header).
//----------------------------------------------------------------
void CLbsLocInfoConversionHandler::ConstructL()
    {
    // Nothing to be done here.
    }

// ---------------------------------------------------------
// CLbsLocInfoConversionHandler::Start
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CLbsLocInfoConversionHandler::Start( TAyncRequestType aType )
    {
    iRequestType = aType;
    SetActive();
    }

// ---------------------------------------------------------
// CLbsLocInfoConversionHandler::GetPositionInfoArrayL
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CLbsLocInfoConversionHandler::GetPositionInfoArrayL(
                                              RBufReadStream& readStream, 
                                              CLbsLocationInfo& aDesLocationInfo  )
    {
    // Read the count of converted info.
    TInt convertedInfoCount = readStream.ReadInt8L();
    
    for( TInt i=0;i<convertedInfoCount;i++ )
        {
        CLbsPositionInfo* positionInfo = CLbsPositionInfo::NewL();
        CleanupStack::PushL( positionInfo );
        positionInfo->InternaliseL( readStream );
        aDesLocationInfo.AddAreaInfoL( positionInfo );
        CleanupStack::Pop( positionInfo );
        }
    }


// ---------------------------------------------------------
// CLbsLocInfoConversionHandler::GetGsmCellInfoArrayL
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CLbsLocInfoConversionHandler::GetGsmCellInfoArrayL(
                                              RBufReadStream& readStream, 
                                              CLbsLocationInfo& aDesLocationInfo )
    {
    // Read the count of converted info.
    TInt convertedInfoCount = readStream.ReadInt8L();
    
    for( TInt i=0;i<convertedInfoCount;i++ )
        {
        CLbsGsmCellInfo* gsmCellInfo = CLbsGsmCellInfo::NewL();
        CleanupStack::PushL( gsmCellInfo );
        gsmCellInfo->InternaliseL( readStream );
        aDesLocationInfo.AddAreaInfoL( gsmCellInfo );
        CleanupStack::Pop( gsmCellInfo );
        }
    }

// ---------------------------------------------------------
// CLbsLocInfoConversionHandler::GetWcdmaCellInfoArrayL
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CLbsLocInfoConversionHandler::GetWcdmaCellInfoArrayL(
                                               RBufReadStream& readStream, 
                                               CLbsLocationInfo& aDesLocationInfo )
    {
    // Read the count of converted info.
    TInt convertedInfoCount = readStream.ReadInt8L();
    
    for( TInt i=0;i<convertedInfoCount;i++ )
        {
        CLbsWcdmaCellInfo* wcdmaCellInfo = CLbsWcdmaCellInfo::NewL();
        CleanupStack::PushL( wcdmaCellInfo );
        wcdmaCellInfo->InternaliseL( readStream );
        aDesLocationInfo.AddAreaInfoL( wcdmaCellInfo );
        CleanupStack::Pop( wcdmaCellInfo );
        }
    }

// ---------------------------------------------------------
// CLbsLocInfoConversionHandler::GetWlanInfoArrayL
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CLbsLocInfoConversionHandler::GetWlanInfoArrayL(
                                              RBufReadStream& readStream, 
                                              CLbsLocationInfo& aDesLocationInfo )
    {
    // Read the count of converted info.
    TInt convertedInfoCount = readStream.ReadInt8L();
    
    for( TInt i=0;i<convertedInfoCount;i++ )
        {
        CLbsWlanInfo* wlanInfo = CLbsWlanInfo::NewL();
        CleanupStack::PushL( wlanInfo );
        wlanInfo->InternaliseL( readStream );
        aDesLocationInfo.AddAreaInfoL( wlanInfo );
        CleanupStack::Pop( wlanInfo );
        }
    }

// End of file.
