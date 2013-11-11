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
* Description:  Stub imlementation
*
*/


// INCLUDE FILES
#include <SsyCallback.h>
#include <e32property.h>
#include "ssypropertyprovider.h"
#include "SSYChannelBase.h"
#include "siftrace.h"
#include "SensrvInternalPSKeys.h"
#include "stubtimer.h"




//
// ----------------------------------------------------------------------------------
// CSSYChannelBase::CSSYChannelBase()
// ----------------------------------------------------------------------------------
//
CSSYChannelBase::CSSYChannelBase( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider )
    : CActive( EPriorityStandard ), iSsyCallback( aSsyCallback ), iPropertyProvider( aPropertyProvider )
    {
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CSSYChannelBase() - Enter" ) ) );

	iChannelState = EChannelIdle;

	iPeriodic = CStubTimer::NewL();//CPeriodic::NewL( EPriorityStandard );

	// add to active scheduler
    CActiveScheduler::Add( this );

	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CSSYChannelBase() - Return" ) ) );
	}


//
// ----------------------------------------------------------------------------------
// CSSYChannelBase::~CSSYChannelBase()
// ----------------------------------------------------------------------------------
//
// Destructor.
CSSYChannelBase::~CSSYChannelBase()
	{
	Cancel();
	if( iPeriodic )
	    {
	    iPeriodic->Cancel();
	    }
	delete iPeriodic;


	}

void CSSYChannelBase::IssueRequest( TInt aError )
	{
	if ( !IsActive() )
        {
	    TRequestStatus *s = &iStatus;
	    User::RequestComplete( s, aError );
	    SetActive();
        }
	}
//
// ----------------------------------------------------------------------------------
// CSSYChannelBase::~CSSYChannelBase()
// ----------------------------------------------------------------------------------
//
void CSSYChannelBase::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::DoCancel() - Enter" ) ) );


    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::DoCancel() - Return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CSSYChannelBase::RunL()
// ----------------------------------------------------------------------------------
//
void CSSYChannelBase::RunL()
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - %S" ), &location ) );

    switch( iChannelState )
        {
        case EChannelOpening:
            {
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - EChannelOpening" ) ) );
            TInt jam( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvOpenChannelAsyncJam, jam );
            if( jam )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - OpenChannel JAM" ) ) );
                TRequestStatus status;
                User::WaitForRequest( status );
                return;
                }

            TInt panic( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvOpenChannelAsyncPanic, panic );
            if( panic )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - OpenChannel PANIC" ) ) );
                User::Panic( _L("TESTPANIC"), 9999 );
                }

            TInt error( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvChannelOpenedError, error );
            if( error )
                {
                if ( error == KErrNotSupported )
                	{
					COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - OpenChannel - KErrNotSupported - Property set to 0" ) ) );

					TSensrvProperty property( /*KSensrvPropIdAvailability*/ 4, KSensrvItemIndexNone, 0 );
					TInt err (0);
					/*TRAP(err,*/iPropertyProvider->SetPropertyL( iChannelInfo.iChannelId, property /*)*/);
	                iChannelState = EChannelOpen;
	                iSsyCallback->ChannelOpened( iChannelInfo.iChannelId, KErrNotSupported/*iStatus.Int()*/, this, iPropertyProvider );
	                return;
					}
                else
                	{
	                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - SSY DOES NOT CALL ChannelOpened" ) ) );
	                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelIdle" ) ) );
	                iChannelState = EChannelIdle;
	                return;
					}
                }
            else
            	{

				COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - OpenChannel - Property set to 1" ) ) );

				TSensrvProperty property( /*KSensrvPropIdAvailability*/ 4, KSensrvItemIndexNone, 1 );
				TInt err (0);
				/*TRAP(err,*/iPropertyProvider->SetPropertyL( iChannelInfo.iChannelId, property /*)*/);
				}

            if( iStatus.Int() == KErrNone )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelOpen" ) ) );
                iChannelState = EChannelOpen;
                }
            else
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelIdle" ) ) );
                iChannelState = EChannelIdle;
                }
            iSsyCallback->ChannelOpened( iChannelInfo.iChannelId, iStatus.Int(), this, iPropertyProvider );
            break;
            }
        case EChannelClosing:
            {
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - EChannelClosing" ) ) );
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelIdle" ) ) );

            TInt jam( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvCloseChannelAsyncJam, jam );
            if( jam )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - CloseChannel JAM" ) ) );
                TRequestStatus status;
                User::WaitForRequest( status );
                }

            TInt panic( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvCloseChannelAsyncPanic, panic );
            if( panic )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - CloseChannel PANIC" ) ) );
                User::Panic( _L("TESTPANIC"), 9999 );
                }

            TInt error( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvChannelClosedError, error );
            if( error )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - SSY DOES NOT CALL ChannelClosed" ) ) );
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelIdle" ) ) );
                iChannelState = EChannelIdle;
                return;
                }
            iChannelState = EChannelIdle;
            iSsyCallback->ChannelClosed( iChannelInfo.iChannelId );
            break;
            }
        case EChannelOpen:
            {
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - EChannelOpen" ) ) );
            break;
            }
        case EChannelListening:
            {
            TInt intervalInMillisecond( 5 );
            RProperty::Get( KPSUidSensrvTest, KSensrvSamplingInterval, intervalInMillisecond );
            iPeriodic->Start( TTimeIntervalMicroSeconds32( intervalInMillisecond *1000 ),
                                /*TTimeIntervalMicroSeconds32( intervalInMillisecond *1000 ),*/
                                TCallBack( PeriodicTick, this ) );

            break;
            }
        case EChannelStopListening:
            {
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - EChannelStopListening" ) ) );

            TInt panic( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvStopListeningAsyncPanic, panic );
            if( panic )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - StopListening PANIC" ) ) );
                User::Panic( _L("TESTPANIC"), 9999 );
                }


            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelOpen" ) ) );
            iChannelState = EChannelOpen;
            break;
            }
        case EChannelBufferFilled:
        case EChannelForceBufferFilled:
            {
            TInt jam( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvDataListeningJam, jam );
            if( jam )
                {
                iPeriodic->Cancel();
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - BufferFilled JAM" ) ) );
                TRequestStatus status;
                User::WaitForRequest( status );
                return;
                }

            TInt panic( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvDataListeningPanic, panic );
            if( panic )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - BufferFilled PANIC" ) ) );
                User::Panic( _L("TESTPANIC"), 9999 );
                }

            TInt error( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvDataListeningBufferFilledError, error );
            if( error )
                {
                iPeriodic->Cancel();
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - SSY DOES NOT CALL BufferFilled" ) ) );
                iCounter = 0;
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelListening" ) ) );
                iChannelState = EChannelListening;
                return;
                }

            TInt negativeCount( KErrNone );
            RProperty::Get( KPSUidSensrvTest, KSensrvBufferFilledNegativeCount, negativeCount );
            if( negativeCount )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - BufferFilled: Return NEGATIVE COUNT -1" ) ) );
                iCounter = -1;
                }

            RProperty::Get( KPSUidSensrvTest, KSensrvSingleBufferFilledNegativeCount, negativeCount );
            if( negativeCount )
                {
                COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - BufferFilled: Return NEGATIVE COUNT -1" ) ) );
                iCounter = -1;
                RProperty::Set( KPSUidSensrvTest, KSensrvSingleBufferFilledNegativeCount, 0 );
                }

            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() - EChannelForceBufferFilled" ) ) );
            iSsyCallback->BufferFilled( iChannelInfo.iChannelId, iCounter, iDataBuffer, iMaxDataCount );
            iCounter = 0;
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() iChannelState = EChannelListening" ) ) );
            iChannelState = EChannelListening;
            IssueRequest();
            break;
            }
        default:
            {
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::RunL() unknown channel state" ) ) );
            }
        }


    }

// ---------------------------------------------------------------------------
// CSSYChannelBase::StartChannelDataL()
// ?implementation_description
// ---------------------------------------------------------------------------
//
#ifdef COMPONENT_TRACE_DEBUG
void CSSYChannelBase::StartChannelDataL( const TSensrvChannelId aChannelId,
                                         TUint8* aBuffer, TInt aCount)
#else
void CSSYChannelBase::StartChannelDataL( const TSensrvChannelId /*aChannelId*/,
                                         TUint8* aBuffer, TInt aCount)
#endif
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StartChannelDataL() - %S: channelId = %d" ), &location, aChannelId ) );
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StartChannelDataL() - aBuffer = %x, aCount = %d" ), aBuffer, aCount ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStartListeningLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StartChannelDataL() - StartChannelData Leave %d" ), leave ) );
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStartListeningJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StartChannelDataL() - StartChannelData JAM" ) ) );
        TRequestStatus status;
        User::WaitForRequest( status );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStartListeningMethodPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StartChannelDataL() - StartChannelData PANIC" ) ) );
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    if( iChannelState == EChannelOpen )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StartChannelDataL() iChannelState = EChannelListening" ) ) );
        iChannelState = EChannelListening;
        iDataBuffer = aBuffer;
        iMaxDataCount = aCount;
        iCounter = 0;

        IssueRequest();

        RProperty::Set( KPSUidSensrvTest, KSensrvLatestStartListeningChannel, iChannelInfo.iChannelId );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StartChannelDataL() Incorrect channel state - Leave" ) ) );
        User::Leave( KErrGeneral );
        }

    }

// ---------------------------------------------------------------------------
// CSSYChannelBase::StopChannelDataL()
// ?implementation_description
// ---------------------------------------------------------------------------
//
#ifdef COMPONENT_TRACE_DEBUG
void CSSYChannelBase::StopChannelDataL( const TSensrvChannelId aChannelId )
#else
void CSSYChannelBase::StopChannelDataL( const TSensrvChannelId /*aChannelId*/ )
#endif
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StopChannelDataL() - %S: channelId = %d" ), &location, aChannelId ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStopListeningLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StopChannelDataL() - StopChannelDataL Leave %d" ), leave ) );
        iPeriodic->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStopListeningJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StopChannelDataL() - StopChannelData JAM" ) ) );
        iPeriodic->Cancel();
        TRequestStatus status;
        User::WaitForRequest( status );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStopListeningMethodPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StopChannelDataL() - StopChannelData PANIC" ) ) );
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    if( iChannelState == EChannelListening )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StopChannelDataL() iChannelState = EChannelStopListening" ) ) );
        iChannelState = EChannelStopListening;
        // Stop data buffering
        iPeriodic->Cancel();
        IssueRequest();

        RProperty::Set( KPSUidSensrvTest, KSensrvLatestStopListeningChannel, iChannelInfo.iChannelId );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::StopChannelDataL() Incorrect channel state - Leave" ) ) );
        User::Leave( KErrGeneral );
        }

    }

// ---------------------------------------------------------------------------
// CSSYChannelBase::ForceBufferFilledL()
// ?implementation_description
// ---------------------------------------------------------------------------
//
#ifdef COMPONENT_TRACE_DEBUG
void CSSYChannelBase::ForceBufferFilledL( const TSensrvChannelId aChannelId )
#else
void CSSYChannelBase::ForceBufferFilledL( const TSensrvChannelId /*aChannelId*/ )
#endif
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ForceBufferFilledL() - %S: channelId = %d" ), &location, aChannelId ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvForceBufferFilledLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ForceBufferFilledL() - ForceBufferFilledL Leave %d" ), leave ) );
        iPeriodic->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvForceBufferFilledJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ForceBufferFilledL() - ForceBufferFilledL JAM" ) ) );
        iPeriodic->Cancel();
        TRequestStatus status;
        User::WaitForRequest( status );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvForceBufferFilledPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ForceBufferFilledL() - ForceBufferFilledL PANIC" ) ) );
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    if( iChannelState == EChannelListening ||
        iChannelState == EChannelBufferFilled ||
        iChannelState == EChannelForceBufferFilled )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ForceBufferFilledL() iChannelState = EChannelForceBufferFilled" ) ) );
        iChannelState = EChannelForceBufferFilled;
        IssueRequest();
        }
    else
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ForceBufferFilledL() Incorrect channel state: %d - Leave" ), iChannelState ) );
        User::Leave( KErrGeneral );
        }

    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::GetChannelDataProviderInterfaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSSYChannelBase::GetChannelDataProviderInterfaceL( TUid aInterfaceUid,
	                                        TAny*& aInterface )
    {
    aInterface = NULL;

	if ( aInterfaceUid.iUid == KSsyChannelDataProviderInterface1.iUid )
		{
		aInterface = reinterpret_cast<TAny*>(
			static_cast<MSsyChannelDataProvider*>( this ) );
		}
    }

// ---------------------------------------------------------------------------
// CSSYChannelBase::ChannelId()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TSensrvChannelId CSSYChannelBase::ChannelId()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ChannelId() - Return %d" ), iChannelInfo.iChannelId ) );
    return iChannelInfo.iChannelId;
    }

// ---------------------------------------------------------------------------
// CSSYChannelBase::SetChannelId()
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CSSYChannelBase::SetChannelId( const TSensrvChannelId aChannelId )
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::SetChannelId() - aChannelId=%d" ), aChannelId ) );
    iChannelInfo.iChannelId = aChannelId;
    }
// ---------------------------------------------------------------------------
// CSSYChannelBase::ChannelInfo()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TSensrvChannelInfo& CSSYChannelBase::ChannelInfo()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::ChannelInfo()" ) ) );
    return iChannelInfo;
    }

// ---------------------------------------------------------------------------
// CSSYChannelBase::OpenChannel()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CSSYChannelBase::OpenChannel()
	{
	TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::OpenChannel() - %S" ), &location ) );

	TInt error( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvOpenChannelError, error );
    if( error )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::OpenChannel() - OpenChannel return error %d" ), error ) );
        return error;
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvOpenChannelMethodJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::OpenChannel() - OpenChannel JAM" ) ) );
        iPeriodic->Cancel();
        TRequestStatus status;
        User::WaitForRequest( status );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvOpenChannelMethodPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::OpenChannel() - OpenChannel PANIC" ) ) );
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    TInt completeError( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvChannelOpenedWithError, completeError );

	TInt err( KErrNone );

	if( iChannelState == EChannelIdle )
	    {
	    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::OpenChannel() iChannelState = EChannelOpening" ) ) );
	    iChannelState = EChannelOpening;
	    IssueRequest( completeError );

	    RProperty::Set( KPSUidSensrvTest, KSensrvLatestOpenedChannel, iChannelInfo.iChannelId );
	    }
	else
	    {
	    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::OpenChannel() Incorrect channel state" ) ) );
	    err = KErrGeneral;
	    }


	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::OpenChannel() - Return %d" ), err ) );
	return err;
	}



// ---------------------------------------------------------------------------
// CSSYChannelBase::CloseChannel()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CSSYChannelBase::CloseChannel()
	{
	TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CloseChannel() - %S" ), &location ) );

    TInt error( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCloseChannelError, error );
    if( error )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CloseChannel() - CloseChannel return error %d" ), error ) );
        iPeriodic->Cancel();
        return error;
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCloseChannelMethodJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CloseChannel() - CloseChannel JAM" ) ) );
        iPeriodic->Cancel();
        TRequestStatus status;
        User::WaitForRequest( status );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCloseChannelMethodPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CloseChannel() - CloseChannel PANIC" ) ) );
        User::Panic( _L("TESTPANIC"), 9999 );
        }

	TInt err( KErrNone );

	if( iChannelState != EChannelIdle )
	    {
	    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CloseChannel() iChannelState = EChannelClosing" ) ) );
	    iChannelState = EChannelClosing;
	    iPeriodic->Cancel();
	    IssueRequest();
	    RProperty::Set( KPSUidSensrvTest, KSensrvLatestClosedChannel, iChannelInfo.iChannelId );
	    }
	else
	    {
	    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CloseChannel() Incorrect channel state" ) ) );
	    err = KErrGeneral;
	    }

    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::CloseChannel() - Return %d " ),err ) );
	return err;
	}

// ---------------------------------------------------------------------------
// CSSYChannelBase::PeriodicTick()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CSSYChannelBase::PeriodicTick( TAny* aObject )
    {
    return static_cast<CSSYChannelBase*>( aObject )->DoPeriodicTick();
    }

// ---------------------------------------------------------------------------
// CSSYChannelBase::DoPeriodicTick()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CSSYChannelBase::DoPeriodicTick()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelBase::DoPeriodicTick() " ) ) );
    TInt ret( EFalse );
    if( iCounter < iMaxDataCount )
        {
        DoAppendData();
        iCounter++;
        ret = ETrue;

        if( iCounter == iMaxDataCount )
            {
            iChannelState = EChannelBufferFilled;
            IssueRequest();
            ret = EFalse;
            }
        }

    return ret;

    }

