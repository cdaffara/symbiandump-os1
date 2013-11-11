/*
* Copyright (c) 2007,2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel class of Orientation SSY
*
*/


#include <ssycallback.h>
#include <sensrvorientationsensor.h>
#include "SsyChannel.h"
#include "SsyOrientation.h"
#include "SsyProperty.h"
#include "SsyConfiguration.h"
#include "SsyTrace.h"


#ifdef _DEBUG

// ----------------------------------------------------------------------------------
// ConvertChannelStateIntoString()
// Helper function for printing this channel into a "string"
// ----------------------------------------------------------------------------------
// 
const TDesC* ConvertChannelStateIntoString( TChannelState aChannelState )
    {
    _LIT(KChannelIdle,              "EChannelIdle");
    _LIT(KChannelOpening,           "EChannelOpening");
    _LIT(KChannelOpen,              "EChannelOpen");
    _LIT(KChannelClosing,           "EChannelClosing");
    _LIT(KChannelListening,         "EChannelListening");
    _LIT(KChannelDataReceived,      "EChannelDataReceived");
    _LIT(KChannelStopListening,     "EChannelStopListening");
    _LIT(KChannelForceBufferFilled, "EChannelForceBufferFilled");
    _LIT(KChannelBufferFilled,      "EChannelBufferFilled");
    _LIT(KDefault,                  "EDefault");

    switch ( aChannelState )
        {
        case EChannelIdle:
            return &KChannelIdle;
        case EChannelOpening:
            return &KChannelOpening;
        case EChannelOpen:
            return &KChannelOpen;
        case EChannelClosing:
            return &KChannelClosing;
        case EChannelListening:
            return &KChannelListening;
        case EChannelDataReceived:
            return &KChannelDataReceived;
        case EChannelStopListening:
            return &KChannelStopListening;
        case EChannelForceBufferFilled:
            return &KChannelForceBufferFilled;
        case EChannelBufferFilled:
            return &KChannelBufferFilled;
        default:
            return &KDefault;
        }
    }

#endif

// ----------------------------------------------------------------------------------
// CSSYChannel::CSSYChannel()
// ----------------------------------------------------------------------------------
// 
CSSYChannel::CSSYChannel( CSSYProperty* aProperty,
                          MSsyCallback* const aCallBack,                                  
                          const TSensrvChannelInfo& aChannelInfo ) :
    CActive( EPriorityStandard ),
    iOrientationHandler( NULL ),
    iChannelProperties( NULL ),
    iSensorProperties( aProperty ),
    iCallback( aCallBack ),
    iChannelState( EChannelIdle ),
    iChannelInfo( aChannelInfo ),
    iBuffer( NULL ),
    iCount( 0 ),
    iCountReceived( 0 ),
    iWritePointer( NULL )
    {
    SSY_TRACE_IN();

    //ASSERT_DEBUG_SSY( aCallBack );

    // Add our active object to the process active scheduler
    CActiveScheduler::Add( this );

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::NewL()
// ----------------------------------------------------------------------------------
// 
CSSYChannel* CSSYChannel::NewL( CSSYProperty* aProperty,
                                MSsyCallback* const aCallBack,                                  
                                const TSensrvChannelInfo& aChannelInfo )
    {
    SSY_TRACE_IN();

    ASSERT_DEBUG_SSY( aCallBack );

    if ( !aCallBack )
        {
        User::Leave( KErrArgument );
        }

    CSSYChannel* self = new( ELeave ) CSSYChannel( aProperty, aCallBack, aChannelInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    SSY_TRACE_OUT();

    return self;
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::ConstructL()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::ConstructL()
    {
    SSY_TRACE_IN();
    
    _LIT( KThreadName, "OrientationThread" );
    RThread thread;
    thread.RenameMe( KThreadName );
    
    // Create channel properties
    if ( iChannelInfo.iChannelType == KSensrvChannelTypeIdOrientationData )
        {        
        iChannelProperties = new (ELeave) CSSYProperty( 
                                            0, 
                                            KSSyChannelOrientationProperties, 
                                            ARRAY_LENGTH( 
                                                KSSyChannelOrientationProperties) );
        }
    else if ( iChannelInfo.iChannelType == KSensrvChannelTypeIdRotationData )
        {        
        iChannelProperties = new (ELeave) CSSYProperty( 
                                            0, 
                                            KSSyChannelRotationProperties, 
                                            ARRAY_LENGTH( 
                                                KSSyChannelRotationProperties) );
        }    

    // Create orientation handler
    iOrientationHandler = new (ELeave) CSSYOrientation( iChannelInfo.iChannelType );
    iOrientationHandler->ConstructL();

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::~CSSYChannel()
// ----------------------------------------------------------------------------------
// 
CSSYChannel::~CSSYChannel()
    {
    SSY_TRACE_IN();

    Cancel();

    delete iChannelProperties;
    iChannelProperties = NULL;

    delete iOrientationHandler;
    iOrientationHandler = NULL;

    // These pointers are not owned
    iSensorProperties = NULL; // Pointer assigned in cosntructor
    iCallback = NULL;         // Pointer assigned in cosntructor
    iBuffer = NULL;           // Assigned in StartChannelData
    iWritePointer = NULL;     // Assigned in AppendData
    
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::DoCancel()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::DoCancel()
    {
    SSY_TRACE_IN();

    // Try to stop the channel, ignore errors
    TRAP_IGNORE( StopChannelDataL( iChannelInfo.iChannelId ) );

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::RunError()
// ----------------------------------------------------------------------------------
// 
#ifdef _DEBUG
TInt CSSYChannel::RunError( TInt aError )
#else
// not used in UREL builds
TInt CSSYChannel::RunError( TInt /*aError*/ )
#endif
    {
    SSY_TRACE( EMust, "ORIENTATIONSSY:RunError %d", aError );
    
    return KErrNone;
    }


// ----------------------------------------------------------------------------------
// CSSYChannel::SetChannelState()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::SetChannelState( const TChannelState aChannelState )
    {
#ifdef _DEBUG
    SSY_TRACE( EMust, "ORIENTATIONSSY:State change from %S to %S with channelId %d",
               ConvertChannelStateIntoString(iChannelState), 
               ConvertChannelStateIntoString(aChannelState), 
               iChannelInfo.iChannelId );
#endif

    iChannelState = aChannelState;
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::AppendData()
// ----------------------------------------------------------------------------------
// 
TBool CSSYChannel::AppendData( 
    const TSensrvOrientationData& aOrientationData, 
    const TSensrvRotationData& aRotationData )
    {
    TBool ret( ETrue );

    if ( iChannelState == EChannelDataReceived )
        {
        if( iCountReceived == 0 )
            {
            SSY_TRACE( EExtended, "ORIENTATIONSSY:First data append to buffer" );
            iWritePointer = iBuffer;
            }

        ASSERT_DEBUG_SSY( iWritePointer );

        if ( !iWritePointer )
            {
            return EFalse;
            }

        if ( iChannelInfo.iChannelType == KSensrvChannelTypeIdOrientationData )
            {
            TSensrvOrientationData* p = reinterpret_cast<TSensrvOrientationData*>(iWritePointer);
            p->iTimeStamp = aOrientationData.iTimeStamp;
            p->iDeviceOrientation = aOrientationData.iDeviceOrientation;
            iWritePointer += sizeof( TSensrvOrientationData );
            }
        else if ( iChannelInfo.iChannelType == KSensrvChannelTypeIdRotationData )
            {
            TSensrvRotationData* p = reinterpret_cast<TSensrvRotationData*>(iWritePointer);
            p->iDeviceRotationAboutZAxis = aRotationData.iDeviceRotationAboutZAxis;
            p->iDeviceRotationAboutXAxis = aRotationData.iDeviceRotationAboutXAxis;
            p->iDeviceRotationAboutYAxis = aRotationData.iDeviceRotationAboutYAxis;
            p->iTimeStamp = aRotationData.iTimeStamp;
            iWritePointer += sizeof( TSensrvRotationData );
            }
        else
            {
            ret = EFalse;
            }
        }
    else
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Trying to append while not listening ", ChannelId() );
        ret = EFalse;
        }

    return ret;
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::RunL()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::RunL()
    {
    SSY_TRACE_IN();

    SSY_TRACE( EMust, "ORIENTATIONSSY:RunL in state %S", ConvertChannelStateIntoString(iChannelState) );

    switch ( iChannelState )
        {
        case EChannelIdle:
            break;
        case EChannelOpening:
            {
            SetChannelState( EChannelOpen );
            iCallback->ChannelOpened( iChannelInfo.iChannelId, iStatus.Int(), this, this );
            break;
            }
        case EChannelOpen:
            break;
        case EChannelClosing:
            SetChannelState( EChannelIdle );
            iCallback->ChannelClosed( iChannelInfo.iChannelId );
            break;
        case EChannelListening:
            SetChannelState( EChannelDataReceived );
            iOrientationHandler->GetDataFromHardware( this );
            break;
        case EChannelDataReceived:

            // Channel excepted data, increase the receive counter
            iCountReceived++;

            if( iCountReceived == iCount )
                {
                SetChannelState( EChannelBufferFilled );
                }
            else
                {
                SetChannelState( EChannelListening );
                }

            IssueRequest();
            break;
        case EChannelStopListening:
            SetChannelState( EChannelOpen );
            iBuffer        = NULL;
            iCount         = 0;
            iCountReceived = 0;
            break;
        case EChannelForceBufferFilled:
            // Intentional flowthrough
        case EChannelBufferFilled:

            iBuffer = NULL;
            iCount  = 0;

            iCallback->BufferFilled( iChannelInfo.iChannelId, iCountReceived, iBuffer, iCount );

            // Check that client did not change the state in the callback function
            if ( iChannelState == EChannelForceBufferFilled ||
                 iChannelState == EChannelBufferFilled )
                {
                iCountReceived = 0;

                if ( iBuffer && iCount )
                    {
                    SetChannelState( EChannelListening );
                    }
                else
                    {
                    SSY_TRACE( EError, "ORIENTATIONSSY:ERROR:New buffer and/or count values are wrong!" );
                    iOrientationHandler->StopListeningL();
                    SetChannelState( EChannelStopListening );
                    }

                IssueRequest();
                }
            break;
        default:
            ASSERT_DEBUG_SSY( 0 );
            break;
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::IssueRequest()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::IssueRequest( TInt aError )
	{
    SSY_TRACE_IN();

	if ( !IsActive() )
        {
        iStatus = KRequestPending;
	    TRequestStatus *status = &iStatus;
	    User::RequestComplete( status, aError );
        SSY_TRACE( EExtended, "ORIENTATIONSSY:Setactive for channelId %d", iChannelInfo.iChannelId );
	    SetActive();
        }

    SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSSYChannel::StartChannelDataL()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::StartChannelDataL( const TSensrvChannelId aChannelId, 
                                     TUint8* aBuffer, 
                                     TInt aCount )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId ); 

    if ( ChannelId() != aChannelId )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: StartChannelDataL wrong channelId!" );
        User::Leave( KErrArgument );
        }

    ASSERT_DEBUG_SSY( aBuffer );
    ASSERT_DEBUG_SSY( aCount );

    if ( !aBuffer || !aCount )
        {
        User::Leave( KErrArgument );
        }

    iBuffer = aBuffer;
    iCount  = aCount;

    SSY_TRACE( EExtended, "ORIENTATIONSSY:New data buffer: 0x%x", iBuffer );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:New data count : %d", iCount );

    if ( iChannelState & KChannelCanBeListened )
        {
        iOrientationHandler->StartListeningL();
        SetChannelState( EChannelListening );
        IssueRequest();
        }
    else
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Trying to listen a channel that is not open! channelId: %d, state: %S", 
                   aChannelId,
                   ConvertChannelStateIntoString( iChannelState ) );

        User::Leave( KErrGeneral );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::StopChannelDataL()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::StopChannelDataL( const TSensrvChannelId aChannelId )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY::ChannelId %d", iChannelInfo.iChannelId ); 

    if ( ChannelId() != aChannelId )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY::ERROR: StartChannelDataL wrong channelId!" );
        User::Leave( KErrArgument );
        }

    if ( iChannelState & KChannelCanBeStopped )
        {
        iOrientationHandler->StopListeningL();
        SetChannelState( EChannelStopListening );
        IssueRequest();
        }
    else
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Trying to stop listening a channel that is not listened! channelId: %d, state: %S",
                   aChannelId,
                   ConvertChannelStateIntoString( iChannelState ) );

        User::Leave( KErrGeneral );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::ForceBufferFilledL()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::ForceBufferFilledL( const TSensrvChannelId aChannelId )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId ); 

    if ( ChannelId() != aChannelId )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: StartChannelDataL wrong channelId!" );
        User::Leave( KErrArgument );
        }

    if( iChannelState & KChannelCanBeFilled )
        {
        SetChannelState( EChannelForceBufferFilled );
        IssueRequest();
        }
    else
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: ForceBufferFilledL called in incorrected state! channelId: %d, state: %S",
                   aChannelId,
                   ConvertChannelStateIntoString( iChannelState ) );

        User::Leave( KErrGeneral );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::ChannelId()
// ----------------------------------------------------------------------------------
// 
TSensrvChannelId CSSYChannel::ChannelId()
    {
    SSY_TRACE_IN();
    SSY_TRACE_OUT();
    return iChannelInfo.iChannelId;
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::SetChannelId()
// ----------------------------------------------------------------------------------
// 
void CSSYChannel::SetChannelId( const TSensrvChannelId aChannelId )
    {
    SSY_TRACE_IN();
    // Set the channelId
    iChannelInfo.iChannelId = aChannelId;

    // Add us as an affected party to the sensor properties, ignore errors
    iSensorProperties->RegisterChannel( iChannelInfo.iChannelId );

    // Add us as an affected party to the channel properties, ignore errors
    iChannelProperties->RegisterChannel( iChannelInfo.iChannelId );

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::ChannelInfo()
// ----------------------------------------------------------------------------------
// 
TSensrvChannelInfo& CSSYChannel::ChannelInfo()
    {
    SSY_TRACE_IN();
    SSY_TRACE_OUT();
    return iChannelInfo;
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::OpenChannel()
// ----------------------------------------------------------------------------------
// 
TInt CSSYChannel::OpenChannel()
	{
	SSY_TRACE_IN();
	SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId ); 

	TInt err( KErrNone );

	if ( iChannelState & KChannelCanBeOpened )
	    {
        TRAP( err, iOrientationHandler->OpenChannelL() );
        if ( err == KErrNone )
            {
            SetChannelState( EChannelOpening );
            IssueRequest();
            }
	    }
	else
	    {	    
        SSY_TRACE( EExtended, "ORIENTATIONSSY:ERROR: Trying to open a channel in a wrong state! channelId: %d, state: %S",
                   iChannelInfo.iChannelId,
                   ConvertChannelStateIntoString( iChannelState ) );

	    err = KErrGeneral;
	    }

	SSY_TRACE_OUT();
	return err;
	}

// ----------------------------------------------------------------------------------
// CSSYChannel::CloseChannel()
// ----------------------------------------------------------------------------------
// 
TInt CSSYChannel::CloseChannel()
	{
	SSY_TRACE_IN();
	SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId ); 

	TInt err( KErrNone );

	if( iChannelState & KChannelCanBeClosed )
	    {
        TRAP( err, iOrientationHandler->CloseChannelL() ); 
        if ( err == KErrNone )
            {
            iBuffer        = NULL; // Reassigned in data listening
            iCount         = 0;
            iCountReceived = 0;
            SetChannelState( EChannelClosing );
            IssueRequest();
            }
        else
            {
            SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Trying to close a channel! err: %d, channelId: %d, state: %S",
                       err,
                       iChannelInfo.iChannelId,
                       ConvertChannelStateIntoString( iChannelState ) );
            }
	    }
	else
	    {
	    SSY_TRACE( EExtended, "ORIENTATIONSSY:ERROR: Trying to close a channel in a wrong state! channelId: %d, state: %S",
                   iChannelInfo.iChannelId,
                   ConvertChannelStateIntoString( iChannelState ) );

	    err = KErrGeneral;
	    }

	SSY_TRACE_OUT();
	return err;
	}

// ----------------------------------------------------------------------------------
// CSSYChannel::CheckPropertyDependenciesL()
// ----------------------------------------------------------------------------------
//    
void CSSYChannel::CheckPropertyDependenciesL( const TSensrvChannelId aChannelId, 
                                              const TSensrvProperty& aProperty,
                                              RSensrvChannelList& aAffectedChannels  )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId ); 

    if ( ChannelId() != aChannelId )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: CheckPropertyDependenciesL wrong channelId!" );
        User::Leave( KErrArgument );
        }

    CSSYProperty* propertyPtr = iSensorProperties;
    TSensrvProperty property = aProperty;

    // try first common sensor properties
    TInt ret = iSensorProperties->GetProperty( property );

    if ( ret != KErrNone )
        {
        propertyPtr = iChannelProperties;

        // then try the channel properties
        ret = iChannelProperties->GetProperty( property );        
        }    

    if ( ret != KErrNone )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Property is not supported: 0x%x ", aProperty.GetPropertyId() );
        User::Leave( KErrNotFound );
        }

    propertyPtr->GetAffectedChannels( aAffectedChannels );

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::SetPropertyL()
// ----------------------------------------------------------------------------------
//    
void CSSYChannel::SetPropertyL( const TSensrvChannelId aChannelId,
                                    const TSensrvProperty& aProperty )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId ); 

    if ( ChannelId() != aChannelId )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: SetPropertyL wrong channelId!" );
        User::Leave( KErrArgument );
        }

    TBool valueChanged( ETrue );
    RSensrvChannelList affectedChannels;
    TInt valueInt( 0 );

    // try first common sensor properties
    TInt ret = iSensorProperties->SetProperty( aProperty, affectedChannels );

    if ( ret == KErrNotFound )
        {
        // then try the channel properties
        ret = iChannelProperties->SetProperty( aProperty, affectedChannels );        
        }

    if ( ret != KErrNone && ret != KErrAlreadyExists )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Property is not supported or it's readonly: 0x%x ", aProperty.GetPropertyId() );
        User::Leave( KErrNotSupported );
        }

    if ( ret == KErrAlreadyExists )
        {
        // client tried to set the same value that the property already has, lets just return without doing anything
        // but informing the "change" via the callback
        valueChanged = EFalse;
        }

    // do something if the value was really changed
    if ( valueChanged )
        {
        aProperty.GetValue( valueInt );

        // These are ReadOnly values, cannot change these
        if ( ( aProperty.GetPropertyId() == KSensrvPropIdDataRate ) ||  
             ( aProperty.GetPropertyId() == KSensrvPropIdAvailability ) ||   
             ( aProperty.GetPropertyId() == KSensrvPropIdMeasureRange ) || 
             ( aProperty.GetPropertyId() == KSensrvPropIdChannelDataFormat ) || 
             ( aProperty.GetPropertyId() == KSensrvPropIdChannelAccuracy ) )
            {
            SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Setting the property is not supported: 0x%x ", aProperty.GetPropertyId() );
            User::Leave( KErrNotSupported );
            }
        }

    // we call the callback function to inform that property was changed, even if it was not actually changed
    iCallback->PropertyChanged( iChannelInfo.iChannelId, affectedChannels, aProperty );

    affectedChannels.Reset();

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::GetPropertyL()
// ----------------------------------------------------------------------------------
//  
void CSSYChannel::GetPropertyL( const TSensrvChannelId aChannelId,
                                    TSensrvProperty& aProperty )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId ); 

    if ( ChannelId() != aChannelId )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: GetPropertyL wrong channelId!" );
        User::Leave( KErrArgument );
        }
    
    // try first common sensor properties
    TInt ret = iSensorProperties->GetProperty( aProperty );

    if ( ret != KErrNone )
        {
        // then try the channel properties
        ret = iChannelProperties->GetProperty( aProperty );
        }

    if ( ret != KErrNone )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Property is not supported: 0x%x ", aProperty.GetPropertyId() );
        User::Leave( KErrNotSupported );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::GetAllPropertiesL()
// ----------------------------------------------------------------------------------
//  
void CSSYChannel::GetAllPropertiesL( const TSensrvChannelId aChannelId, 
                                         RSensrvPropertyList& aChannelPropertyList )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY:ChannelId %d", iChannelInfo.iChannelId );

    if ( ChannelId() != aChannelId )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: GetAllPropertiesL wrong channelId!" );
        User::Leave( KErrArgument );
        }

    aChannelPropertyList.Reset();
    
    RSensrvPropertyList channelProperties;

    iSensorProperties->GetAllProperties( aChannelPropertyList );
    iChannelProperties->GetAllProperties( channelProperties );

    for ( TInt index = 0; index < channelProperties.Count(); index++ )
        {
        aChannelPropertyList.Append( channelProperties[index] );
        }
    
    channelProperties.Close();
    
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::GetPropertyProviderInterfaceL()
// ----------------------------------------------------------------------------------
//    
void CSSYChannel::GetPropertyProviderInterfaceL(
    TUid aInterfaceUid, 
    TAny*& aInterface )
    {
    aInterface = NULL;
    
	  if ( aInterfaceUid.iUid == KSsyPropertyProviderInterface1.iUid )
		    {
		    aInterface = reinterpret_cast<TAny*>(
			    static_cast<MSsyPropertyProvider*>( this ) );
		    }
    }

// ----------------------------------------------------------------------------------
// CSSYChannel::GetChannelDataProviderInterfaceL()
// ----------------------------------------------------------------------------------
//    
void CSSYChannel::GetChannelDataProviderInterfaceL(
    TUid aInterfaceUid, 
    TAny*& aInterface )
    {
    aInterface = NULL;
    
	  if ( aInterfaceUid.iUid == KSsyChannelDataProviderInterface1.iUid )
		    {
		    aInterface = reinterpret_cast<TAny*>(
			    static_cast<MSsyChannelDataProvider*>( this ) );
		    }
    }

// End of File
