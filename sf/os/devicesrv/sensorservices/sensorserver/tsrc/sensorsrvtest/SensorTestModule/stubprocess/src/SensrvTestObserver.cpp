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
* Description:
*
*/


// INCLUDE FILES
#include <sensrvtypes.h>
#include <sensrvchannelfinder.h>
#include <sensrvchannel.h>
#include "sensrvtrace.h"
#include "SensrvTestObserver.h" 
#include "SensrvTestTypes.h"
#include "SensrvInternalPSKeys.h"


// ----------------------------------------------------------------------------------
// CSensrvTestObserver* CSensrvTestObserver::NewL()
// ----------------------------------------------------------------------------------	

CSensrvTestObserver* CSensrvTestObserver::NewL( TInt aProcessID )
{	
	CSensrvTestObserver* self = CSensrvTestObserver::NewLC(aProcessID);
    CleanupStack::Pop( self );
    return self;	
}

CSensrvTestObserver* CSensrvTestObserver::NewLC( TInt aProcessID )
    {
    CSensrvTestObserver* self = new( ELeave ) CSensrvTestObserver(aProcessID);
    CleanupStack::PushL( self );
    //self->ConstructL();
    return self;
    }
    
// ----------------------------------------------------------------------------------
// CSensrvTestObserver::CSensrvTestObserver() 
// ----------------------------------------------------------------------------------	
CSensrvTestObserver::CSensrvTestObserver(TInt aProcessID) :
  CActive( CActive::EPriorityStandard ),
  iMyProcessID(aProcessID)
	{						
	TInt retval(KErrNone);
	
    retval =  iProperty.Attach( KPSUidSensrvTest, KSensrvTestProcessCommand );
    User::LeaveIfError(retval);           
    
	CActiveScheduler::Add( this );
	
	IssueRequest();
	}
//
// ----------------------------------------------------------------------------------
// CSensrvTestObserver::~CSensrvTestObserver()
// ----------------------------------------------------------------------------------	
CSensrvTestObserver::~CSensrvTestObserver()
	{				
    if( iSensorChannel )
        {
        delete iSensorChannel;
        }
	}
//
// ----------------------------------------------------------------------------------
// CSensrvTestObserver::RunL()
// ----------------------------------------------------------------------------------
void CSensrvTestObserver::RunL()
    {                      
	COMPONENT_TRACE( ( _L( "CSensrvTestObserver::RunL()" ) ) );
	
	TInt retval(KErrNone);	
    TPckgBuf<TSensrvProcessData> buf;
	retval = iProperty.Get( KPSUidSensrvTest, KSensrvTestProcessCommand, buf );	
	User::LeaveIfError(retval);			
	
	//if( iMyProcessID & buf().iProcessID )
	if( iMyProcessID == buf().iProcessID )
	    {
	    //Command is for this process
        switch( buf().iCommand )
            {
            case KProcessCommandOpenChannel:
                {
                DoOpenChannelL( buf().iExpectedResult );
                }
                break;
            
            case KProcessCommandCloseChannel:
                {
                DoCloseChannelL( buf().iExpectedResult );
                }
                break;
            
            case KProcessCommandSetProperty:
                {
                DoSetPropertyL( buf().iExpectedResult, buf().iProperty );
                }
                break;

            case KProcessCommandFindChannel:
                {
                DoFindChannelL();
                }
                break;                
            
            default:
                {
                COMPONENT_TRACE( ( _L( "CSensrvTestObserver::RunL - ERROR: unknown command" ) ) );
                }
                break;
            }
        RProperty::Set( KPSUidSensrvTest, KSensrvTestComplete1, KErrNone );//only KSensrvTestComplete1 supported at the momont
	    }
	
	IssueRequest();
    }        
//
// ----------------------------------------------------------------------------------
// CSensrvTestObserver::DoCancel()
// ----------------------------------------------------------------------------------
void CSensrvTestObserver::DoCancel()
    {
    }    

//
// ----------------------------------------------------------------------------------
// CSensrvTestObserver::RunError()
// ----------------------------------------------------------------------------------    
TInt CSensrvTestObserver::RunError( TInt aError )
    {
    COMPONENT_TRACE( ( _L( "CSensrvTestObserver::RunError(%d)" ), aError ) );	
	
    RProperty::Set( KPSUidSensrvTest, KSensrvTestComplete1, aError );//only KSensrvTestComplete1 supported at the momont
    iStatus = KRequestPending;
    SetActive();
    return KErrNone;
    }

// ----------------------------------------------------------------------------------
// CSensrvTestObserver::IssueRequest
// ----------------------------------------------------------------------------------  
void CSensrvTestObserver::IssueRequest() 			  
{
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iProperty.Subscribe( iStatus );		
		SetActive();		
		}
}

// ----------------------------------------------------------------------------------
// CSensrvTestObserver::DoSetProperty
// ----------------------------------------------------------------------------------  
void CSensrvTestObserver::DoSetPropertyL( TInt aExpectedResult, TSensrvProperty& aProperty )
    {
   	COMPONENT_TRACE( ( _L( "CSensrvTestObserver::DoSetProperty" ) ) );	
   	
   	if( !iSensorChannel )
        {
        User::Leave( KErrNotFound );
        }
        
    TInt ret = iSensorChannel->SetProperty( aProperty );
    
    if( ret != aExpectedResult )
        {
        User::Leave( ret );
        }
           	
    }
    
// ----------------------------------------------------------------------------------
// CSensrvTestObserver::DoCloseChannelL
// ----------------------------------------------------------------------------------  
void CSensrvTestObserver::DoCloseChannelL( TInt /*aExpectedResult*/ )
    {
   	COMPONENT_TRACE( ( _L( "CSensrvTestObserver::DoCloseChannelL" ) ) );
   	
   	if( iSensorChannel )
   	    {
   	    iSensorChannel->CloseChannel();
   	    delete iSensorChannel;
   	    iSensorChannel = NULL;
   	    }
   	else
   	    {
   	    User::Leave( KErrNotFound );
   	    }
    }
    
// ----------------------------------------------------------------------------------
// CSensrvTestObserver::DoOpenChannel
// ----------------------------------------------------------------------------------  
void CSensrvTestObserver::DoOpenChannelL( TInt aExpectedResult )
    {
    COMPONENT_TRACE( ( _L( "CSensrvTestObserver::DoOpenChannelL()" ) ) );
   
    TInt                   lastOpenedChannel( 0 );
    TInt                   channelIndex( 0 );
    TSensrvChannelInfo     channelInfo;
    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL( channelInfoList );
    CSensrvChannelFinder*  channelFinder;
    channelFinder = CSensrvChannelFinder::NewLC();
       
   
    RProperty::Get( KPSUidSensrvTest, KSensrvLatestOpenedChannel, lastOpenedChannel );
    
    channelFinder->FindChannelsL( channelInfoList, channelInfo );
    
    for( TInt i=0 ; i<channelInfoList.Count() ; i++ )
        {
        if( channelInfoList[i].iChannelId == lastOpenedChannel )
            {
            channelIndex = 1;
            }
        }
    
    iSensorChannel = CSensrvChannel::NewL( channelInfoList[ channelIndex ] );
        
    TBuf<16> location;
    location.Copy( iSensorChannel->GetChannelInfo().iLocation );
    COMPONENT_TRACE( ( _L( "CSensrvTestObserver::DoOpenChannelL - Channel %S"), &location ));
    
    TRAPD( err, iSensorChannel->OpenChannelL() );
    if( err != aExpectedResult )
        {
        User::Leave( err );
        }
    
    
    CleanupStack::PopAndDestroy( channelFinder );
    CleanupStack::PopAndDestroy( &channelInfoList ); // <--- results in Close() being called on "channelInfoList".
    
    }

// ----------------------------------------------------------------------------------
// CSensrvTestObserver::DoFindChannelL
// ----------------------------------------------------------------------------------  
void CSensrvTestObserver::DoFindChannelL()
    {
    COMPONENT_TRACE( ( _L( "CSensrvTestObserver::DoFindChannelL" ) ) );

    CSensrvChannelFinder*  channelFinder = CSensrvChannelFinder::NewLC();
    TSensrvChannelInfo     channelInfo;
    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL( channelInfoList );
    
    TRAPD( err, channelFinder->FindChannelsL( channelInfoList, channelInfo ));

    if( err )
        {
        COMPONENT_TRACE( ( _L("CSensrvTestCases::FindChannelsL: FindChanels error: %d"), err ));
        User::Leave( err );
        }
    err = CheckFindResult( channelInfo, channelInfoList );
    if( err )
        {
        COMPONENT_TRACE( ( _L("CSensrvTestCases::FindChannelsL: CheckFindResult error: %d"), err ));
        User::Leave( err );
        }    
    CleanupStack::PopAndDestroy( &channelInfoList );
    CleanupStack::PopAndDestroy( channelFinder );
    }

// -----------------------------------------------------------------------------
// CSensrvTestCases::CheckFindResult
//
// -----------------------------------------------------------------------------
//
TInt CSensrvTestObserver::CheckFindResult( TSensrvChannelInfo& aSearchConditions, RSensrvChannelInfoList aChannelInfoList )
    {    
    COMPONENT_TRACE( _L("CSensrvTestObserver::CheckFindResult") );
    COMPONENT_TRACE( ( _L("### conditions: ChannelId=%d, ContextType=%d, Quantity=%d, ChannelType=%d, Location=%S, VendorId=%S, DataItemSize=%d"),
                     aSearchConditions.iChannelId, aSearchConditions.iContextType, aSearchConditions.iQuantity, aSearchConditions.iChannelType,
                     &aSearchConditions.iLocation, &aSearchConditions.iVendorId, aSearchConditions.iDataItemSize ));

    // Find all channels
    TBufC8<1> nullBuf( KNullDesC8 );
    TSensrvChannelInfo channelInfo( 0, ESensrvContextTypeNotDefined, ESensrvQuantityNotdefined, 0, nullBuf, nullBuf, 0, 0 );
    RSensrvChannelInfoList allChannelList(10);
    CleanupClosePushL( allChannelList );
    CSensrvChannelFinder*  channelFinder = CSensrvChannelFinder::NewLC();
    TRAPD( err, channelFinder->FindChannelsL( allChannelList, channelInfo ));
    if( err )
        {
        COMPONENT_TRACE( ( _L("CSensrvTestCases::CheckFindResult: Find all channels Leave: %d"), err ));
        return err;
        }

    //
    RSensrvChannelInfoList acceptedChannels(10);
    for( TInt i = 0; i < allChannelList.Count(); i++ )
        {
        if( allChannelList[i].IsMatch( aSearchConditions ) )
            {
            acceptedChannels.Append( allChannelList[i] );
            }
        }

    if( acceptedChannels.Count() != aChannelInfoList.Count() )
        {
        // Test fails.
        COMPONENT_TRACE( ( _L("CSensrvTestCases::CheckFindResult: Test fails") ));
        COMPONENT_TRACE( ( _L("   required count %d"), acceptedChannels.Count() ));
        COMPONENT_TRACE( ( _L("   found channels count %d"), aChannelInfoList.Count() ));
        return KErrGeneral;
        }

    TBool testFail( EFalse );
    // Check all channels infos
    for( TInt i = 0; i < aChannelInfoList.Count(); i++ )
        {
        COMPONENT_TRACE( ( _L("     ###channel info [%d]:"), i ));
        COMPONENT_TRACE( ( _L("     ### ChannelId=%d, ContextType=%d, Quantity=%d, ChannelType=%d, Location=%S, VendorId=%S, DataItemSize=%d"),
                   aChannelInfoList[i].iChannelId, aChannelInfoList[i].iContextType, aChannelInfoList[i].iQuantity, aChannelInfoList[i].iChannelType,
                   &aChannelInfoList[i].iLocation, &aChannelInfoList[i].iVendorId, aChannelInfoList[i].iDataItemSize ));

        TBool found( EFalse );
        for( TInt x = 0; x < acceptedChannels.Count(); x++ )
            {
            if( acceptedChannels[x].iChannelId == aChannelInfoList[i].iChannelId )
                {
                found = ETrue;
                break;
                }

            }
        if( found )
            {
            COMPONENT_TRACE( _L("     -OK-") );
            }
        else
            {
            COMPONENT_TRACE( _L("     -FAIL-") );
            testFail = ETrue;
            }

        }
    
    CleanupStack::PopAndDestroy( channelFinder );
    CleanupStack::PopAndDestroy( &allChannelList );    
    
    if( testFail )
        {
        return KErrGeneral;
        }
    return KErrNone;
    }    


