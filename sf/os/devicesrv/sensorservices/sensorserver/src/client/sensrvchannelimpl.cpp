/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSensrvChannelImpl implementation
*
*/


#include "sensrvpropertylistener.h"
#include "sensrvchannelimpl.h"
#include "sensrvdatahandler.h"
#include "sensrvpropertyhandler.h"
#include "sensrvtrace.h"
#include "sensrvtypes.h"
#include "sensrvdatalistener.h"
#include "sensrvchannelconditionlistener.h"
#include "sensrvconditionhandler.h"
#include "sensrvchannelconditionimpl.h"
#include "sensrvchannelconditionsetimpl.h"

const TInt KDefaultPropertyCount = 40;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelImpl::CSensrvChannelImpl( const TSensrvChannelInfo& aChannelInfo )
    : iChannelInfo( aChannelInfo ),
      iConditionSetList(_FOFF(CSensrvChannelConditionSetImpl,iLink)),
      iConditionSetIter(iConditionSetList)
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::CSensrvChannelImpl()" ) ) );
    }


// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ConstructL() - Start" ) ) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ConstructL() - Return" ) ) );
    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelImpl::~CSensrvChannelImpl()
    {
    delete iDataHandler;
    delete iChannelPropertyHandler;
    delete iConditionHandler;
    iSensrvClient.Close();
    iConditionSetList.Reset();
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::OpenChannelL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::OpenChannelL()
    {
    // Open Channel and reset listeners
    DoOpenChannelL( ETrue );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::DoOpenChannelL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::DoOpenChannelL( TBool aResetListeners )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DoOpenChannelL()" ) ) );

    if( iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DoOpenChannelL() -  ERROR: Channel is already open" ) ) );
        User::Leave( KErrAlreadyExists );
        }

    User::LeaveIfError( iSensrvClient.Connect() );
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DoOpenChannelL() - Client connected" ) ) );

    TInt maxBufferCount(0);
    TSensrvTIntPckgBuf intBuf( maxBufferCount );
    TInt dataItemSize(0);
    TSensrvTIntPckgBuf intBuf2( dataItemSize );

    User::LeaveIfError( iSensrvClient.SendReceiveSync( ESensrvSrvReqOpenChannel,
                                        TIpcArgs( iChannelInfo.iChannelId, &intBuf, &intBuf2 ) ) );

    iMaxBufferCount = intBuf();
    iChannelInfo.iDataItemSize = intBuf2(); // Store the actual data item size, known after SSY channel registration

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DoOpenChannelL -  iMaxBufferSize: %d" ), iMaxBufferCount ) );

    iChannelOpen = ETrue;

    if( aResetListeners )
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DoOpenChannelL() - Reset listeners" ) ) );
        iDataListener = NULL;
        iChannelPropertyListener = NULL;
        iConditionListener = NULL;

        // Also reset conditions
        iConditionSetList.Reset();
        }
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DoOpenChannelL() - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::CloseChannel
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelImpl::CloseChannel()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::CloseChannel() - Start" ) ) );
    if( iChannelOpen )
        {
        // Cancel handlers handler
        if ( iDataHandler )
            {
            iDataHandler->Cancel();
            }

        if ( iChannelPropertyHandler )
            {
            iChannelPropertyHandler->Cancel();
            }

        if ( iConditionHandler )
            {
            iConditionHandler->Cancel();
            }

        iSensrvClient.SendReceiveSync( ESensrvSrvReqCloseChannel, TIpcArgs( iChannelInfo.iChannelId ) );
        iChannelOpen = EFalse;

        iSensrvClient.Close();
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::CloseChannel() - Client closed" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::CloseChannel() - Return" ) ) );

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CSensrvChannelImpl::GetChannelInfo
// ---------------------------------------------------------------------------
//
const TSensrvChannelInfo& CSensrvChannelImpl::GetChannelInfo() const
    {
    return iChannelInfo;
    }


// ---------------------------------------------------------------------------
// CSensrvChannelImpl::StartDataListeningL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::StartDataListeningL( MSensrvDataListener* aDataListener,
                                      const TInt aDesiredCount,
                                      const TInt aMaximumCount,
                                      const TInt aBufferingPeriod )
    {
    COMPONENT_TRACE( (
    _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() - Start: aDataListener=%x, aDesiredCount=%d, aMaximumCount =%d, aBufferingPeriod=%d" ),
     aDataListener, aDesiredCount, aMaximumCount, aBufferingPeriod ) );

    if( aDesiredCount < 0 || aMaximumCount < 0 || aBufferingPeriod < 0 )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() -  ERROR: Negative argument: leaving KErrArgument" ) ) );
        User::Leave( KErrArgument );
        }

    if ( !aDataListener )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() -  ERROR: NULL listener: leaving KErrArgument" ) ) );
        User::Leave( KErrArgument );
        }

    if( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() -  ERROR: Channel is not open: leaving KErrNotFound" ) ) );
        User::Leave( KErrNotFound );
        }

    TSensrvStartListeningParameters listeningParams;

    listeningParams.iDesiredBufferingCount = aDesiredCount;
    listeningParams.iMaximumBufferingCount = aMaximumCount;
    listeningParams.iBufferingPeriod = aBufferingPeriod;

    // Data and condition listening use same parameters for desired count and buffering period,
    // so use lowest value specified, if both are active at the same time
    if ( iConditionListener )
        {
        if (iDataListeningParameters.iDesiredBufferingCount < aDesiredCount)
            {
            listeningParams.iDesiredBufferingCount = iDataListeningParameters.iDesiredBufferingCount;
            }

        if ( iDataListeningParameters.iBufferingPeriod < aBufferingPeriod )
            {
            listeningParams.iBufferingPeriod = iDataListeningParameters.iBufferingPeriod;
            }
        }

    TSensrvStartListeningParametersPckgBuf startListeningPckg( listeningParams );

    TInt startErr = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartListening,
        TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );

    if ( startErr == KErrAlreadyExists )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() -  ERROR: Already listening: return KErrAlreadyExists" ) ) );
        User::Leave( startErr );
        }

    if ( startErr )
        {
        // Try start
        startErr = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartListening,
                                TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );
        if ( startErr )
            {
            ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() -  ERROR: restart failed %d" ), startErr ) );

            // Open channel
            TRAPD( err, ReopenChannelForListeningL(EStartDataListening) );

            if ( !err )
                {
                // try startListening again
                startErr = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartListening,
                                TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );
                }
            else
                {
                // Note: Channel can be left closed if reopen fails, depending at which
                //       point it fails.
                ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() -  ERROR: Reopening channel failed %d" ), err ) );
                }
            }
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() -  error code: %d" ), startErr ) );

    User::LeaveIfError( startErr );

    // If first time, create datahandler
    if (!iDataHandler)
        {
        TRAP( startErr, iDataHandler = CSensrvDataHandler::NewL( iSensrvClient, iChannelInfo ) );
        }

    if ( startErr == KErrNone )
        {
        TRAP( startErr, iDataHandler->StartListeningL( this, listeningParams, iMaxBufferCount ) );
        }

    if ( startErr )
        {
        // Stop listening
        iSensrvClient.SendReceiveSync( ESensrvSrvReqStopListening, TIpcArgs( iChannelInfo.iChannelId ) );
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() - ERROR: Starting listening failed, leave: %d" ), startErr ) );
        User::Leave( startErr );
        }

    // Now that we have successfully started, store parameters
    iDataListener = aDataListener;

    iDataListeningParameters.iDesiredBufferingCount = listeningParams.iDesiredBufferingCount;
    iDataListeningParameters.iMaximumBufferingCount = listeningParams.iMaximumBufferingCount;
    iDataListeningParameters.iBufferingPeriod = listeningParams.iBufferingPeriod;

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartDataListening() - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::StopDataListening
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelImpl::StopDataListening()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StopDataListening() - Start" ) ) );
    
    TInt err( KErrNone );
    if ( iChannelOpen )
        {
        if ( iDataHandler && iDataHandler->IsActive() )
            {
            iDataHandler->Cancel();
            }
        else
            {
            err = iSensrvClient.SendReceiveSync( ESensrvSrvReqStopListening, TIpcArgs( iChannelInfo.iChannelId ) );
            }
        }

    iDataListener = NULL;

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StopDataListening() - Return" ) ) );

    return err;
    }


// ---------------------------------------------------------------------------
// CSensrvChannelImpl::GetData
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelImpl::GetData( TDes8& aDataPtr )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetData() - Start" ) ) );
    TInt err( KErrNone );
    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetData() -  ERROR: Channel is not open: return KErrNotFound" ) ) );
        err = KErrNotFound;
        }
    else
        {
        if ( iDataHandler )
            {
            err = iDataHandler->GetData( aDataPtr );
            }
        else
            {
            err = KErrNotFound;
            }
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetData() - Return %d" ), err ) );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::SetProperty
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelImpl::SetProperty( const TSensrvProperty& aProperty )
    {
#ifdef COMPONENT_TRACE_DEBUG
    if ( ESensrvIntProperty == aProperty.PropertyType() )
        {
        TInt value( 0 );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetProperty(PropertyId:0x%x value:%d)" ), aProperty.GetPropertyId(), value ) );
        }
    else if ( ESensrvRealProperty == aProperty.PropertyType() )
        {
        TReal value( 0 );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetProperty(PropertyId:0x%x value:%f)" ), aProperty.GetPropertyId(), value ) );
        }
    else if ( ESensrvBufferProperty == aProperty.PropertyType() )
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetProperty(PropertyId:0x%x value:%S)" ) , aProperty.GetPropertyId(), &const_cast<TSensrvProperty*>(&aProperty)->GetValueRef() ) );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetProperty(PropertyId:0x%x)" ), aProperty.GetPropertyId() ) );
        }
#endif//COMPONENT_TRACE_DEBUG

    TInt err( KErrNone );
    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetProperty() -  ERROR: Channel is not open: return KErrNotFound" ) ) );
        err = KErrNotFound;
        }
    else
        {
        TPropertyPckg propertyPckg( aProperty );

        err = iSensrvClient.SendReceiveSync( ESensrvSrvReqSetProperty, TIpcArgs( iChannelInfo.iChannelId, &propertyPckg ) );
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetProperty - return %d" ), err ) );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::GetPropertyL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::GetPropertyL( const TSensrvPropertyId aPropertyId,
                                       const TInt aItemIndex,
                                       TSensrvProperty& aProperty )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL(PropertyId:0x%x ItemIndex:%d)" ),aPropertyId, aItemIndex ) );

    if (aItemIndex < 0 && aItemIndex != KSensrvItemIndexNone)
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL() -  ERROR: Negative argument: leaving KErrArgument" ) ) );
        User::Leave( KErrArgument );
        }

    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL() -  ERROR: Channel is not open: return KErrNotFound" ) ) );
        User::Leave( KErrNotFound );
        }

    TPropertyPckg propertyPckg( aProperty );

    //Set aPropertyId and aItemIndex to aProperty for server use
    aProperty.SetPropertyId(aPropertyId);
    aProperty.SetItemIndex(aItemIndex);

    User::LeaveIfError( iSensrvClient.SendReceiveSync(
        ESensrvSrvReqGetProperty, TIpcArgs( iChannelInfo.iChannelId, &propertyPckg ) ) );

#ifdef COMPONENT_TRACE_DEBUG
    if ( ESensrvIntProperty == aProperty.PropertyType() )
        {
        TInt value( 0 );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL() - Return property value=%d" ), value ) );
        }
    else if ( ESensrvRealProperty == aProperty.PropertyType() )
        {
        TReal value( 0 );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL() - Return property value=%f" ), value ) );
        }
    else if ( ESensrvBufferProperty == aProperty.PropertyType() )
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL() - Return property value=%S" ), &aProperty.GetValueRef() ) );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL() - Return" ) ) );
        }
#endif//COMPONENT_TRACE_DEBUG
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::GetAllPropertiesL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::GetAllPropertiesL( RSensrvPropertyList& aChannelPropertyList )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL()" ) ) );

    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL() -  ERROR: Channel is not open: return KErrNotFound" ) ) );
        User::Leave( KErrNotFound );
        }
    TSensrvTIntPckgBuf  countPckgBuf;
    TInt                err( KErrNone );
    RSensrvPropertyList channelPropertyList( KDefaultPropertyCount );

    CleanupClosePushL( channelPropertyList );

    for( TInt i=0; i<KDefaultPropertyCount; i++ )
        {
        channelPropertyList.AppendL( TSensrvProperty() );
        }

    TInt  arraySize( ( sizeof( TSensrvProperty ) ) * KDefaultPropertyCount );
    TPtr8 arrayPtr( (TUint8*)&channelPropertyList[0], arraySize, arraySize );

    err = iSensrvClient.SendReceiveSync( ESensrvSrvReqGetAllProperties,
                                         TIpcArgs( iChannelInfo.iChannelId,
                                                   &arrayPtr,
                                                   &countPckgBuf ) );

    if ( err == KErrOverflow ) // buffer overflow. Allocate new buffer
        {
        for( TInt i=KDefaultPropertyCount; i<countPckgBuf(); i++ )
            {
            channelPropertyList.AppendL( TSensrvProperty() );
            }
        TInt  arraySize( ( sizeof( TSensrvProperty ) ) * countPckgBuf() );
        TPtr8 arrayPtr( (TUint8*)&channelPropertyList[0], arraySize, arraySize );
        err = iSensrvClient.SendReceiveSync( ESensrvSrvReqGetAllProperties,
                                             TIpcArgs( iChannelInfo.iChannelId,
                                                       &arrayPtr,
                                                       &countPckgBuf ) );
        }

    if ( err == KErrNone )
        {
        for( TInt i = 0; i < countPckgBuf(); ++i )
            {
            aChannelPropertyList.AppendL( channelPropertyList[i] );
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL() -  ERROR: Failed to get properties" ) ) );
        User::Leave( err );
        }

    // Close the array and free all memory allocated to the array.
    CleanupStack::PopAndDestroy( &channelPropertyList );

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL return void" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::GetPropertyL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::GetPropertyL( const TSensrvPropertyId aPropertyId,
                                       const TInt aItemIndex,
                                       const TInt aArrayIndex,
                                       TSensrvProperty& aProperty )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL(item:%d array:%d)"), aItemIndex, aArrayIndex )  );

    if ((aItemIndex < 0 && aItemIndex != KSensrvItemIndexNone) ||
       (aArrayIndex < 0 && aArrayIndex != ESensrvSingleProperty && aArrayIndex != ESensrvArrayPropertyInfo))
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetPropertyL() -  ERROR: Negative argument: leaving KErrArgument" ) ) );
        User::Leave( KErrArgument );
        }

    aProperty.SetArrayIndex( aArrayIndex );
    GetPropertyL( aPropertyId, aItemIndex, aProperty );

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL -index- return void" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::GetAllPropertiesL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::GetAllPropertiesL( const TSensrvPropertyId aPropertyId,
                                            RSensrvPropertyList& aChannelPropertyList )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL(id:%d)"),aPropertyId )  );
    RSensrvPropertyList propertyList;
    CleanupClosePushL( propertyList );
    TBool found( EFalse );

    GetAllPropertiesL( propertyList );

    for( TInt i = 0; i < propertyList.Count(); ++i )
        {
        if ( aPropertyId == propertyList[i].GetPropertyId() )
            {
            COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL - match found" ) ) );
            aChannelPropertyList.AppendL( propertyList[i] );
            found = ETrue;
            }
        }

     // Close the array and free all memory allocated to the array.
    CleanupStack::PopAndDestroy( &propertyList );

    if ( !found )
        {
        User::Leave( KErrNotFound );//According to the specification
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::GetAllPropertiesL -id- return void" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::SetPropertyListenerL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::SetPropertyListenerL( MSensrvPropertyListener* aChannelPropertyListener )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetPropertyListenerL()" ) ) );

    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetPropertyListenerL() -  ERROR: Channel is not open: return KErrNotFound" ) ) );
        User::Leave( KErrNotFound );
        }

    if ( iChannelPropertyListener && aChannelPropertyListener )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetPropertyListenerL() -  ERROR: Already listening: return KErrAlreadyExists" ) ) );
        User::Leave( KErrAlreadyExists );
        }

    iChannelPropertyListener = aChannelPropertyListener;

    if ( iChannelPropertyListener )
        {
        if ( !iChannelPropertyHandler )
            {
            iChannelPropertyHandler = CSensrvPropertyHandler::NewL( iSensrvClient, iChannelInfo );
            }

        iChannelPropertyHandler->StartListening( this );
        }
    else
        {
        if ( iChannelPropertyHandler && iChannelPropertyHandler->IsActive() )
            {
            iChannelPropertyHandler->Cancel();
            }
        else
            {
            TInt ignore = iSensrvClient.SendReceiveSync( ESensrvSrvReqStopPropertyListening,
                                                           TIpcArgs( iChannelInfo.iChannelId ) );
            }
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetPropertyListenerL() - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::AddConditionL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::AddConditionL( CSensrvChannelConditionSet& aChannelConditionSet )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::AddConditionL() - Start" ) ) );

    // Check that channel is open
    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::AddConditionL() -  ERROR: Channel is not open: return KErrNotFound" ) ) );
        User::Leave( KErrNotFound );
        }

    CSensrvChannelConditionSetImpl* setImpl =
        static_cast<CSensrvChannelConditionSetImpl*>(&aChannelConditionSet);

    // Check that condition set is complete (i.e. no unpaired range conditions)
    if ( !setImpl->IsComplete() )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::AddConditionL() -  ERROR: Condition set is incomplete" ) ) );
        User::Leave( KErrArgument );
        }

    TBool duplicateId(ETrue);

    // Set the set id after checking for conflicts
    while ( duplicateId )
        {
        duplicateId = EFalse;
        iConditionSetIdCounter++;

        // Find set from local list
        CSensrvChannelConditionSetImpl* ptr = NULL;
        iConditionSetIter.SetToFirst();
        while ( (ptr = iConditionSetIter++) != NULL )
            {
            if ( ptr == setImpl )
                {
                // Do not allow same set to be added twice
                User::Leave( KErrAlreadyExists );
                }

            if ( ptr->ConditionSetId() == iConditionSetIdCounter )
                {
                duplicateId = ETrue;
                }
            };
        }

    setImpl->SetConditionSetId(iConditionSetIdCounter);

    // Extract data from set
    HBufC8* serializedSet = setImpl->SerializeLC();

    // Add set to local list
    iConditionSetList.AddLast(*setImpl);

    // Add set to server
    TInt err(iSensrvClient.SendReceiveSync( ESensrvSrvReqAddConditionSet,
                                            TIpcArgs( iChannelInfo.iChannelId,
                                                      serializedSet ) ) );

    // If set adding to server failed, remove set from local list also
    if (err != KErrNone)
        {
        iConditionSetList.Remove( *setImpl );
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::AddConditionL() -  ERROR: Adding set to server failed" ) ) );
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy(serializedSet);

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::AddConditionL() - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::RemoveConditionL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::RemoveConditionL( CSensrvChannelConditionSet& aChannelConditionSet )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::RemoveConditionL() - Start" ) ) );

    // Check that channel is open
    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::RemoveConditionL() -  ERROR: Channel is not open: return KErrNotFound" ) ) );
        User::Leave( KErrNotFound );
        }

    CSensrvChannelConditionSetImpl* setImpl = static_cast<CSensrvChannelConditionSetImpl*>(&aChannelConditionSet);

    // Remove set from server
    User::LeaveIfError(iSensrvClient.SendReceiveSync( ESensrvSrvReqRemoveConditionSet,
                                                      TIpcArgs( iChannelInfo.iChannelId,
                                                                setImpl->ConditionSetId())));
    // Remove set from local list if it is there
    iConditionSetIter.SetToFirst();
    TBool found(EFalse);
    CSensrvChannelConditionSetImpl* ptr = NULL;
    while (!found && (ptr = iConditionSetIter++) != NULL)
        {
        if (ptr == setImpl)
            {
            found = ETrue;
            iConditionSetList.Remove( *setImpl );
            COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::RemoveConditionL() - Removed set from client side list" ) ) );
            }
        };

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::RemoveConditionL() - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::StartConditionListeningL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::StartConditionListeningL(
                        MSensrvChannelConditionListener* aChannelConditionListener,
                        const TInt aObjectCount,
                        const TInt aBufferingPeriod )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL(aChannelConditionListener: 0x%x, aObjectCount: %d, aBufferingPeriod: %d) " ),
        aChannelConditionListener, aObjectCount, aBufferingPeriod ) );

    if (aObjectCount < 0 || aBufferingPeriod < 0)
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() -  ERROR: Negative argument: leaving KErrArgument" ) ) );
        User::Leave( KErrArgument );
        }

    if (!aChannelConditionListener)
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() -  ERROR: NULL listener: leaving KErrArgument" ) ) );
        User::Leave( KErrArgument );
        }

    if ( !iChannelOpen )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() - ERROR: Channel is not open: return KErrNotFound" ) ) );
        User::Leave( KErrNotFound );
        }

    TSensrvStartListeningParameters listeningParams;

    listeningParams.iDesiredBufferingCount = aObjectCount;
    listeningParams.iMaximumBufferingCount = iDataListeningParameters.iMaximumBufferingCount;
    listeningParams.iBufferingPeriod = aBufferingPeriod;

    // Data and condition listening use same parameters for desired count and buffering period,
    // so use lowest value specified, if both are active at the same time.
    if (iDataListener)
        {
        if (iDataListeningParameters.iDesiredBufferingCount < aObjectCount)
            {
            listeningParams.iDesiredBufferingCount = iDataListeningParameters.iDesiredBufferingCount;
            }

        if (iDataListeningParameters.iBufferingPeriod < aBufferingPeriod)
            {
            listeningParams.iBufferingPeriod = iDataListeningParameters.iBufferingPeriod;
            }
        }

    TSensrvStartListeningParametersPckgBuf startListeningPckg( listeningParams );

    TInt startErr = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartConditionListening,
        TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );

    if ( startErr == KErrAlreadyExists )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() -  ERROR: Already listening: return KErrAlreadyExists" ) ) );
        User::Leave( startErr );
        }

    if ( startErr )
        {
        // Try start again
        startErr = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartConditionListening,
                                TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );
        if ( startErr )
            {
            ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() -  ERROR: restart failed %d" ), startErr ) );

            // Open channel
            TRAPD( err, ReopenChannelForListeningL(EStartConditionListening) );

            if ( !err )
                {
                // try startListening again
                startErr = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartConditionListening,
                                TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );
                }
            else
                {
                // Note: Channel can be left closed if reopen fails, depending at which
                //       point it fails.
                ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() -  ERROR: Reopening channel failed %d" ), startErr ) );
                }
            }
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() -  error code: %d" ), startErr ) );

    User::LeaveIfError( startErr );

    if ( !iConditionHandler )
        {
        TRAP( startErr, iConditionHandler = CSensrvConditionHandler::NewL( iSensrvClient, iChannelInfo ));
        }

    if ( startErr == KErrNone )
        {
        TRAP( startErr, iConditionHandler->StartListening( this, listeningParams ) );
        }

    if ( startErr )
        {
        // Stop listening
        iSensrvClient.SendReceiveSync( ESensrvSrvReqStopConditionListening, TIpcArgs( iChannelInfo.iChannelId ) );
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() - ERROR: Starting listening failed, leave: %d" ), startErr ) );
        User::Leave( startErr );
        }

    // Now that we have successfully started, store parameters
    iConditionListener = aChannelConditionListener;

    iDataListeningParameters.iDesiredBufferingCount = listeningParams.iDesiredBufferingCount;
    iDataListeningParameters.iBufferingPeriod = listeningParams.iBufferingPeriod;

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StartConditionListeningL() - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::StopConditionListening
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelImpl::StopConditionListening()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StopConditionListening() - Start" ) ) );

    TInt err( KErrNone );
    
    // Check that channel is open
    if ( iChannelOpen )
        {
        if ( iConditionHandler && iConditionHandler->IsActive() )
            {
            iConditionHandler->Cancel();
            }
        else
            {
            // Block further async condition messages,
            // in case stop is called from ConditionMet, when handler is not active.
            if ( iConditionHandler )
                {
                iConditionHandler->BlockAsyncMessage();
                }

            err = iSensrvClient.SendReceiveSync( ESensrvSrvReqStopConditionListening, TIpcArgs( iChannelInfo.iChannelId ) );
            }
        }

    iConditionListener = NULL;

    // Remove all added conditions
    iConditionSetList.Reset();

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::StopConditionListening() - Return " ) ) );

    return err;
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::DataReceived
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::DataReceived( TInt aCount, TInt aDataLost )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DataReceived()" ) ) );
    if ( iDataListener )
        {
        iDataListener->DataReceived( *this, aCount, aDataLost );
        }
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::DataError
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::DataError( TSensrvErrorSeverity aError )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::DataError( %d )" ), aError ) );

    if ( aError == ESensrvErrorSeverityFatal )
        {
        CloseChannel();
        }

    if ( iDataListener )
        {
        iDataListener->DataError( *this, aError );
        }
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::ReopenChannelForDataListeningL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::ReopenChannelForDataListeningL( TInt& aMaxBufferCount )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ReopenChannelForDataListeningL" ) ) );

    ReopenChannelForListeningL(EDataHandler);

    aMaxBufferCount = iMaxBufferCount;
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::PropertyChanged
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::PropertyChanged( const TSensrvProperty& aChangedProperty )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::PropertyChanged()" ) ) );

    if ( iChannelPropertyListener )
        {
        iChannelPropertyListener->PropertyChanged( *this, aChangedProperty );
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::PropertyChanged - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::PropertyError
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::PropertyError( TSensrvErrorSeverity aError )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::PropertyError()" ) ) );

    if ( aError == ESensrvErrorSeverityFatal )
        {
        CloseChannel();
        }

    if ( iChannelPropertyListener )
        {
        iChannelPropertyListener->PropertyError( *this, aError );
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::PropertyError - return" ) ) );
    }


// ---------------------------------------------------------------------------
// CSensrvChannelImpl::ReopenChannelForPropertyListeningL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::ReopenChannelForPropertyListeningL()
    {
    ReopenChannelForListeningL(EPropertyHandler);
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::SetPropertySuccessIndicationChanged
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::SetPropertySuccessIndicationChanged( TSetPropertySuccessIndicator aIndication )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetPropertySuccessIndicationChanged()" ) ) );

    if ( iChannelPropertyListener )
        {
        iChannelPropertyListener->SetPropertySuccessIndicationChanged( aIndication );
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::SetPropertySuccessIndicationChanged - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::ConditionMet
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::ConditionMet( TInt aSetId,
                                       TDesC8& aValue )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ConditionMet()" ) ) );

    // Find set from local list
    TBool found(EFalse);
    CSensrvChannelConditionSetImpl* ptr = NULL;
    iConditionSetIter.SetToFirst();
    while (!found && (ptr = iConditionSetIter++) != NULL)
        {
        if (ptr->ConditionSetId() == aSetId)
            {
            found = ETrue;
            }
        };

    // Remove met set from list as it is no longer an added set
    iConditionSetList.Remove(*ptr);

    // Notify listener
    if ( iConditionListener )
        {
        iConditionListener->ConditionMet( *this,
                                          *(static_cast<CSensrvChannelConditionSet*>(ptr)),
                                          aValue );
        }

    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ConditionMet - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::ConditionError
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::ConditionError( TSensrvErrorSeverity aError )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ConditionError( %d )" ), aError ) );

    if ( aError == ESensrvErrorSeverityFatal )
        {
        CloseChannel();
        }

    if ( iConditionListener )
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ConditionError - Notifying listener" ) ) );
        iConditionListener->ConditionError( *this, aError );
        }
    }

// ---------------------------------------------------------------------------
// CSensrvChannelImpl::ReopenChannelForConditionListeningL
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::ReopenChannelForConditionListeningL()
    {
    ReopenChannelForListeningL(EConditionHandler);
    }

// ---------------------------------------------------------------------------
// Reopens channel and restarts listenings, except for the initiator.
//
// Note: Initiator is excluded for (at least) the following reasons:
// - In start listening initiated cases, the member variables required cannot
//   be updated yet when this method is potentially needed.
// - In data handler case also the maximum buffering count
//   can theoretically change (if it is ever based on something else than
//   maximum data rate, which should never change).
// - In property and condition handling it might be possible to just always
//   do same thing regardless of the initator, but for consistency we do
//   it the same way for every initiator.
//
// With some redesign, it would probably be possible
// to make reopening without almost duplicate code in handlers
// and starting listening.
// ---------------------------------------------------------------------------
//
void CSensrvChannelImpl::ReopenChannelForListeningL( THandlerId aInitiator  )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelImpl::ReopenChannelForListeningL(aInitiator: %d)" ), aInitiator ) );

    // Close channel without resetting listeners or clearing conditions
    CloseChannel();

    // open channel without resetting listeners or clearing conditions
    DoOpenChannelL( EFalse );

    // Re-add conditions
    CSensrvChannelConditionSetImpl* ptr = NULL;
    iConditionSetIter.SetToFirst();
    while ((ptr = iConditionSetIter++) != NULL)
        {
        // Extract data from set
        HBufC8* serializedSet = ptr->SerializeLC();

        // Add set to server
        User::LeaveIfError(iSensrvClient.SendReceiveSync(
            ESensrvSrvReqAddConditionSet,
            TIpcArgs( iChannelInfo.iChannelId,
                      serializedSet ) ) );

        CleanupStack::PopAndDestroy(serializedSet);
        };

    // Start data listening
    if (    aInitiator != EDataHandler
         && aInitiator != EStartDataListening
         && iDataListener
         && iDataHandler )
        {
        TSensrvStartListeningParametersPckgBuf startListeningPckg( iDataListeningParameters );

        User::LeaveIfError( iSensrvClient.SendReceiveSync(
                ESensrvSrvReqStartListening,
                TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) ) );

        iDataHandler->StartListeningL( this, iDataListeningParameters, iMaxBufferCount );
        }

    // Start property listening
    if (    aInitiator != EPropertyHandler
         && iChannelPropertyListener
         && iChannelPropertyHandler )
        {
        iChannelPropertyHandler->StartListening( this );
        }

    // Start condition listening
    if (    aInitiator != EConditionHandler
         && aInitiator != EStartConditionListening
         && iConditionListener
         && iConditionHandler )
        {
        TSensrvStartListeningParametersPckgBuf startListeningPckg( iDataListeningParameters );

        User::LeaveIfError( iSensrvClient.SendReceiveSync(
                ESensrvSrvReqStartConditionListening,
                TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) ) );

        iConditionHandler->StartListening( this, iDataListeningParameters);
        }
    }
