// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <ctsy/pluginapi/mtelephonyaudiocontrol.h>
#include "cmockltsyfactory.h"
#include <ctsy/pluginapi/mtelephonyaudiocontrolcallback.h>
#include "cmmtelephonyaudiocontrol.h"
#include "e32property.h"

TCallInfo::TCallInfo( const TName& aCallName, 
                      MTelephonyAudioControl::TTelephonyAudioControlCallType aCallType, 
                      TBool aEmergency, 
                      RMobileCall::TMobileCallDirection aCallDirection ) :
    iCallName(aCallName), 
    iCallType(aCallType), 
    iEmergency(aEmergency), 
    iCallDirection(aCallDirection),
    iCallStatus( RMobileCall::EStatusUnknown )
    {
        
    }

void TCallInfo::SetCallStatus( RMobileCall::TMobileCallStatus aCallStatus )
    {
    iCallStatus = aCallStatus;
    }

TName& TCallInfo::CallName()
    {
    return iCallName;
    }
    
RMobileCall::TMobileCallStatus TCallInfo::CallStatus()
    {
    return iCallStatus;
    }

TBool TCallInfo::Emergency()
    {
    return iEmergency;
    }
    
RMobileCall::TMobileCallDirection TCallInfo::CallDirection()
    {
    return iCallDirection;
    }

CMockTelephonyAudioControl* CMockTelephonyAudioControl::NewL( MTelephonyAudioControlCallback* aTelephonyAudioControlCallback )
    {
    CMockTelephonyAudioControl* self = new(ELeave) CMockTelephonyAudioControl( aTelephonyAudioControlCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

void CMockTelephonyAudioControl::ConstructL()
    {
    
    }

CMockTelephonyAudioControl::CMockTelephonyAudioControl(  MTelephonyAudioControlCallback* aTelephonyAudioControlCallback  ) :
    iTelephonyAudioControlCallback( aTelephonyAudioControlCallback )
    {
    }

CMockTelephonyAudioControl::~CMockTelephonyAudioControl()
    {
    ASSERT( 0 == iCallInfoArray.Count() );
    iCallInfoArray.Close();
    }

void CMockTelephonyAudioControl::SetupTelephonyAudio( const TName& aCallName, 
                                              MTelephonyAudioControl::TTelephonyAudioControlCallType aCallType, 
                                              TBool aEmergency, 
                                              RMobileCall::TMobileCallDirection aCallDirection )
    {
    TInt returnCode( KErrNone );
    
    if ( KErrNone != RProperty::Get( KMockLtsyFactoryProperty, EReturnErrorCode, returnCode ) )
        {
        //Ongoing test case has not defined this property. Return and do not continue further.
        return;
        }

    TBool reset( EFalse );
    
    if ( KErrNone != RProperty::Get( KMockLtsyFactoryProperty, EReset, reset ) )
        {
        //Ongoing test case has not defined this property. Return and do not continue further.
        return;
        }
    
    if ( reset )
        {
        iCallInfoArray.Close();
        }
    
    TInt count( iCallInfoArray.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        if ( 0 == iCallInfoArray[ i ].CallName().Compare( aCallName ) )
            {
            iTelephonyAudioControlCallback->NotifyTelephonyAudioControlError( aCallName, 
                                                                              KErrAlreadyExists );
            }
        }
        
    TInt error = iCallInfoArray.Append( TCallInfo( aCallName, aCallType, aEmergency, aCallDirection ) );
    
    ASSERT( KErrNone == error );
   
    // since this is call setup the call status will be EStatusUnknown
    error = RProperty::Set( KMockLtsyFactoryProperty, ECallStatus, RMobileCall::EStatusUnknown );
    
    ASSERT( KErrNone == error );
           
    error = RProperty::Set( KMockLtsyFactoryProperty, ENumberOfCalls, iCallInfoArray.Count() );
    
    ASSERT( KErrNone == error );
    
    error = RProperty::Set( KMockLtsyFactoryProperty, ECallName, aCallName );
    
    ASSERT( KErrNone == error );
    
    error = RProperty::Set( KMockLtsyFactoryProperty, EEmergencyCall, aEmergency );
    
    ASSERT( KErrNone == error );
    
    error = RProperty::Set( KMockLtsyFactoryProperty, ECallDirection, aCallDirection );
    
    ASSERT( KErrNone == error );
    
    error = RProperty::Set( KMockLtsyFactoryProperty, ECallType, aCallType );
    
    ASSERT( KErrNone == error ); 

    if ( KErrNone != returnCode )
        {
        iTelephonyAudioControlCallback->NotifyTelephonyAudioControlError( aCallName, 
                                                                          returnCode );
        }
    }

void CMockTelephonyAudioControl::TeardownTelephonyAudio( const TName& aCallName, TInt aError )
    {
    TInt returnCode( KErrNone );
    
    if ( KErrNone != RProperty::Get( KMockLtsyFactoryProperty, EReturnErrorCode, returnCode ) )
        {
        //Ongoing test case has not defined this property. Return and do not continue further.
        return;
        }
    
    TInt count( iCallInfoArray.Count() );
    
    ASSERT( 0 != count );  
               
    for ( TInt i = 0; i < count; i++ )
        {
        if ( 0 == iCallInfoArray[ i ].CallName().Compare( aCallName ) )
            {
            TInt error = RProperty::Set( KMockLtsyFactoryProperty, ECallName, aCallName );
            ASSERT( KErrNone == error );
            
            error = RProperty::Set( KMockLtsyFactoryProperty, EHangUpReason, aError );
            ASSERT( KErrNone == error );
            
            // since this is call tear down the call status will be EStatusIdle
            error = RProperty::Set( KMockLtsyFactoryProperty, ECallStatus, RMobileCall::EStatusIdle );
            ASSERT( KErrNone == error );
             
            iCallInfoArray.Remove( i );  
            
            error = RProperty::Set( KMockLtsyFactoryProperty, ENumberOfCalls, iCallInfoArray.Count() );
            
            ASSERT( KErrNone == error ); 
    
            if ( KErrNone != returnCode )
                {
                iTelephonyAudioControlCallback->NotifyTelephonyAudioControlError( aCallName, 
                                                                                  returnCode );
                }
            
            return;
            }
        }
    iTelephonyAudioControlCallback->NotifyTelephonyAudioControlError( aCallName, 
                                                                      KErrNotFound );
    }

void CMockTelephonyAudioControl::CallStateChange( const TName& aCallName, 
		                                          RMobileCall::TMobileCallStatus aCallStatus )
    {
    TInt returnCode( KErrNone );
    
    if ( KErrNone != RProperty::Get( KMockLtsyFactoryProperty, EReturnErrorCode, returnCode ) )
        {
        //Ongoing test case has not defined this property. Return and do not continue further.
        return;
        }

    TInt count( iCallInfoArray.Count() );
    
    ASSERT( 0 != count );  
           
    for ( TInt i = 0; i < count; i++ )
        {
        if ( 0 == iCallInfoArray[ i ].CallName().Compare( aCallName ) )
            {
            iCallInfoArray[ i ].SetCallStatus( aCallStatus );  
            
            TInt error = RProperty::Set( KMockLtsyFactoryProperty, ECallName, aCallName );
            ASSERT( KErrNone == error );      
            
            error = RProperty::Set( KMockLtsyFactoryProperty, ECallStatus, aCallStatus );
            ASSERT( KErrNone == error );   

            if ( KErrNone != returnCode )
                {
                iTelephonyAudioControlCallback->NotifyTelephonyAudioControlError( aCallName, 
                                                                                  returnCode );
                }
            
            return;
            }
        }

    iTelephonyAudioControlCallback->NotifyTelephonyAudioControlError( aCallName, 
                                                                      KErrNotFound );
    }


