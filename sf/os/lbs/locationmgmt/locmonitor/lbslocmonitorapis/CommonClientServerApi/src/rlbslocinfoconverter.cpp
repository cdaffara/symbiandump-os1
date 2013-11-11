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
* Description: Implementation of RLbsLocInfoConverter class.
*
*/

#include <s32mem.h>
#include <lbs/lbslocerrors.h>
#include "rlbslocinfoconverter.h"
#include "clbslocinfoconversionhandler.h"
#include "lbslocmonitorclientconsts.h"
#include "clbslocmonitorptrholder.h"
#include "lbslocmonitorserverdata.h"
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationgsminfo.h>
#include <lbs/lbslocationwcdmainfo.h>
#include <lbs/lbslocationwlaninfo.h>

void CleanUpAreaInfoArray(TAny* aArray)
    {
	 	static_cast<RLbsAreaInfoBaseArray*>(aArray)->ResetAndDestroy();
    }


//----------------------------------------------------------------
// RLbsLocInfoConverter::RLbsLocInfoConverter
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C RLbsLocInfoConverter::RLbsLocInfoConverter():
                                iConversionHandler( NULL ),
                                iPtrHolder( NULL )
    {
    //iConversionHandler = NULL;
    //iPtrHolder = NULL;
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::~RLbsLocInfoConverter
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C RLbsLocInfoConverter::~RLbsLocInfoConverter()
    {
    delete iConversionHandler;
    iConversionHandler = NULL;
    
    delete iPtrHolder;
    iPtrHolder = NULL;
    
    delete iClientPrefs;
    iClientPrefs = NULL;
    }


//----------------------------------------------------------------
// RLbsLocInfoConverter::OpenL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void RLbsLocInfoConverter::OpenL( RLbsLocMonitorSession& 
                                           aLocMonSession )
    {
    __ASSERT_ALWAYS(aLocMonSession.Handle() != 0, User::Invariant()); // trying to open twice
    TInt err(KErrNone);
    ConstructL();
    
    // Call Server Framework 'Open'
    TLbsLocMonitorConversionPositionerData emptyData;
    TLbsLocMonitorSubSessionType type(ELocMonitorSubSessionTypeConversionPositiner);
    err = RSecureSubSessionBase::Open<TLbsLocMonitorSubSessionType, 
                                      TLbsLocMonitorConversionPositionerData>
                                      (aLocMonSession, type, emptyData);
    User::LeaveIfError(err);
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::Close
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void RLbsLocInfoConverter::Close()
    {
    // Call Server Framework 'Close'
    RSecureSubSessionBase::Close();
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::ConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void RLbsLocInfoConverter::ConvertLocationInfoL( 
                                    CLbsLocationInfoBase& aLocationInfo,
                                    TLbsConversionPrefs aConversionPrefs, 
                                    const TLbsConversionOutputInfoMask& aRequestedInfo,
                                    const TUid aConverterModuleId,
                                    TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());
    
    // If there is any outstanding request,panic the client with error code 
    // ELocMonitorDuplicateRequest
    if( iConversionHandler->IsActive() )
        {
        User::Panic( KLocationMonitorFault,ELocMonitorDuplicateRequest );
        }
    
    aStatus = KRequestPending;
    
    // Validate area information
    ValidateLocationInfoL( &aLocationInfo );
    
    // Release the resources used by the earlier request.
    iConversionHandler->ReleaseAllResources();
    
    // Store the handle to client's location info. This is required to 
    // update it with the converted info.
    iConversionHandler->SetClientLocationInfo( aLocationInfo );
    iConversionHandler->SetClientRequestedInfo( aRequestedInfo );
    
    iPtrHolder->Ptr(0).Set( reinterpret_cast<TUint8*>(&iBufferSize),
                            sizeof(iBufferSize),sizeof(iBufferSize) );

    CBufFlat* buffer = CBufFlat::NewL( 512 );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream;
    writeStream.Open( *buffer );
    CleanupClosePushL( writeStream );
    
    // Write location info content to the stream.
    aLocationInfo.ExternaliseL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::Pop( buffer );
    
    // Store the buffer handle in the handler object.
    iConversionHandler->SetBuffer( buffer );
    
    // Store handle to client's status in handler object.Handler 
    // object will complete client's status on conversion completion.
    iConversionHandler->SetStatus( aStatus );
    
    TPtr8 ptr = buffer->Ptr(0);
    iPtrHolder->Ptr(1).Set( ptr );
    
    // Client preferences are packed into a structure in order to send it in 
    // a single IPC slot.
    iClientPrefs->iConversionPrefs = aConversionPrefs;
    iClientPrefs->iRequestedInfo = aRequestedInfo;
    iClientPrefs->iConverterModuleId = aConverterModuleId;
    TPckg<TLbsClientPrefs> clientPrefsPckg( *iClientPrefs );
    
    TIpcArgs ipcArgs;
    ipcArgs.Set( KParamBufferSize,&iPtrHolder->Ptr(0) );
    ipcArgs.Set( KParamLocationInfo,&iPtrHolder->Ptr(1) );
    ipcArgs.Set( KParamClientPrefs,&clientPrefsPckg );
    
    SendReceive( EConvertSingleLocationInfoBufferSize,ipcArgs,iConversionHandler->iStatus );
    iConversionHandler->Start( CLbsLocInfoConversionHandler::EGetBuffersizeForSingleArea );
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::ConvertLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void RLbsLocInfoConverter::ConvertLocationInfoL( 
                                   RLbsLocationInfoBaseArray& aLocationInfoArray,
                                   TLbsConversionPrefs aConversionPrefs,
                                   TLbsConversionOutputInfoMask aRequestedInfo,
                                   const TUid aConverterModuleId,
                                   TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());
    
    // If there is any outstanding request,panic the client with error code 
    // ELocMonitorDuplicateRequest
    if( iConversionHandler->IsActive() )
        {
        User::Panic( KLocationMonitorFault,ELocMonitorDuplicateRequest );
        }
    
    aStatus = KRequestPending;
    
    TInt count = aLocationInfoArray.Count();
    // Leave with error code if array count is zero or count exceeds KPositionMaxBatchSize
    if( !count || count > KPositionMaxBatchSize  )
        {
        User::Leave( KErrArgument );
        }
    
    // Validate each location info object in the array.
    for( TInt i=0;i<count;i++ )
        {
        ValidateLocationInfoL( aLocationInfoArray[i] );
        }
    
    // Release the resources used by the earlier request.
    iConversionHandler->ReleaseAllResources();
    
    // Store the handle to client's location info array. This is required to 
    // update it with the converted info.
    iConversionHandler->SetClientLocationInfoArray( &aLocationInfoArray );
    iConversionHandler->SetClientRequestedInfo( aRequestedInfo );
    
    iPtrHolder->Ptr(0).Set( reinterpret_cast<TUint8*>(&iBufferSize),
                            sizeof(iBufferSize),sizeof(iBufferSize) );

    CBufFlat* buffer = CBufFlat::NewL( 512 );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream;
    writeStream.Open( *buffer );
    CleanupClosePushL( writeStream );
    
    TInt arrayCount = aLocationInfoArray.Count();
    // Write the array count to the stream.     
    writeStream.WriteInt8L( arrayCount );
    
    // Write location info array contents to the stream.
    for( TInt i=0;i<arrayCount;i++ )
        {
        aLocationInfoArray[i]->ExternaliseL( writeStream );
        }
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::Pop( buffer );
        
    // Store the buffer handle in the handler object.
    iConversionHandler->SetBuffer( buffer );
    
    // Store handle to client's status in handler object.Handler 
    // object will complete client's status on conversion completion.
    iConversionHandler->SetStatus( aStatus );

    TPtr8 ptr = buffer->Ptr(0);
    iPtrHolder->Ptr(1).Set( ptr );
    
    // Client preferences are packed into a structure in order to send it in 
    // a single IPC slot.
    iClientPrefs->iConversionPrefs = aConversionPrefs;
    iClientPrefs->iRequestedInfo = aRequestedInfo;
    iClientPrefs->iConverterModuleId = aConverterModuleId;
    TPckg<TLbsClientPrefs> clientPrefsPckg( *iClientPrefs );
    
    TIpcArgs ipcArgs;
    ipcArgs.Set( KParamBufferSize,&iPtrHolder->Ptr(0) );
    ipcArgs.Set( KParamLocationInfo,&iPtrHolder->Ptr(1) );
    ipcArgs.Set( KParamClientPrefs,&clientPrefsPckg );
    
    SendReceive( EConvertMultipleLocationInfoBufferSize,ipcArgs,iConversionHandler->iStatus );
    iConversionHandler->Start( CLbsLocInfoConversionHandler::EGetBuffersizeForMultipleArea );
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::CancelConvertLocationInfo
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void RLbsLocInfoConverter::CancelConvertLocationInfo()
    {
    __ASSERT_ALWAYS(SubSessionHandle(), User::Invariant());
    SendReceive( ECancelConvertLocationInfo );
    if( iConversionHandler->iStatus.Int() != KRequestPending )
        iConversionHandler->Cancel();
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::HandleSingleConversionBufferSizeL
// (other items were commented in a header).
//----------------------------------------------------------------
void RLbsLocInfoConverter::HandleSingleConversionBufferSizeL()
    {
    CBufFlat* buffer = CBufFlat::NewL(256);
    CleanupStack::PushL( buffer );
    buffer->ResizeL( iBufferSize );
    CleanupStack::Pop( buffer );
    iConversionHandler->SetBuffer( buffer );
    
    TPtr8 bufPtr = buffer->Ptr(0);
    iPtrHolder->Ptr(0).Set( bufPtr );
    TIpcArgs ipcArgs;
    ipcArgs.Set( KParamConvertedInfo,&iPtrHolder->Ptr(0) );
    
    SendReceive( EGetSingleLocationInfo,ipcArgs,iConversionHandler->iStatus );
    iConversionHandler->Start( CLbsLocInfoConversionHandler::EGetSingleConvertedArea );
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::HandleMultipleConversionBufferSizeL
// (other items were commented in a header).
//----------------------------------------------------------------
void RLbsLocInfoConverter::HandleMultipleConversionBufferSizeL()
    {
    CBufFlat* buffer = CBufFlat::NewL(256);
    CleanupStack::PushL( buffer );
    buffer->ResizeL( iBufferSize );
    CleanupStack::Pop( buffer );
    iConversionHandler->SetBuffer( buffer );
    
    TPtr8 bufPtr = buffer->Ptr(0);
    iPtrHolder->Ptr(0).Set( bufPtr );
    TIpcArgs ipcArgs;
    ipcArgs.Set( KParamConvertedInfo,&iPtrHolder->Ptr(0) );
    
    SendReceive( EGetMultipleLocationInfo,ipcArgs,iConversionHandler->iStatus );
    iConversionHandler->Start( CLbsLocInfoConversionHandler::EGetMultipleConvertedArea );
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::ConstructL
// (other items were commented in a header).
//----------------------------------------------------------------
EXPORT_C void RLbsLocInfoConverter::ConstructL()
    {
    iConversionHandler = CLbsLocInfoConversionHandler::NewL( *this );
    iPtrHolder = CLbsLocMonitorPtrHolder::NewL(CLbsLocMonitorPtrHolder::ELastParam, 0);
    iClientPrefs = new( ELeave ) TLbsClientPrefs;
    }

//----------------------------------------------------------------
// RLbsLocInfoConverter::ValidateLocationInfoL
// (other items were commented in a header).
//----------------------------------------------------------------
void RLbsLocInfoConverter::ValidateLocationInfoL( CLbsLocationInfoBase*
                                                  aLocationInfo )
    {
    CLbsLocationInfo* locationInfo = static_cast<CLbsLocationInfo*>
                                                     ( aLocationInfo );
    
    RLbsAreaInfoBaseArray areaInfoArray;    
 
 	CleanupStack::PushL(TCleanupItem(*CleanUpAreaInfoArray, &areaInfoArray ));

    locationInfo->GetAreaInfoL( areaInfoArray );                                                     
    
    // Leave with error if array count is zero.
    TInt count = areaInfoArray.Count();
    if( !count )
        {
        User::Leave( KErrArgument );
        }
    
    // Validate each and every area info object.
    for( TInt i=0;i<count;i++ )
        {
        areaInfoArray[i]->ValidateDataL();
        }
    CleanupStack::PopAndDestroy( &areaInfoArray );
    }

// End of file
