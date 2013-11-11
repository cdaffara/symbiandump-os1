/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CTFFactory class 
*	             member functions.
*
*/


// INCLUDE FILES
#include "siftrace.h"
#include "remconcontrol.h"
#include "remconcontroltestcase.h"
#include "ctffactory.h"
#include "tftypes.h"
#include "accremconpstypes.h"

DECLARE_TRACE_ASSERT


_LIT_SECURITY_POLICY_PASS( KWritePolicy );
_LIT_SECURITY_POLICY_PASS( KReadPolicy );

static TTFRemConkeyEvent MapRemConToTfKey( TRemConCoreApiOperationId aOperationId )
    {
    if ( aOperationId == ERemConCoreApiVolumeUp ) return ETFRemConEvent_VolumeUp;
    if ( aOperationId == ERemConCoreApiVolumeDown ) return ETFRemConEvent_VolumeDown;
    if ( aOperationId == ERemConCoreApiStop ) return ETFRemConEvent_CodeStop;
    if ( aOperationId == ERemConCoreApiFastForward ) return ETFRemConEvent_CodeForward;
    if ( aOperationId == ERemConCoreApiRewind ) return ETFRemConEvent_CodeBack;
    if ( aOperationId == ERemConCoreApiForward ) return ETFRemConEvent_CodeForward;
    if ( aOperationId == ERemConCoreApiBackward ) return ETFRemConEvent_CodeBack;
    if ( aOperationId == ERemConCoreApiPausePlayFunction ) return  ETFRemConEvent_CoreApiPlaybackSpeedX1; //event not used in real implementation
    if ( aOperationId == ERemConCoreApiMute )return ETFRemConEvent_Mute;

    return ETFRemConEvent_None;
    }


MRemConControl* GetRemConControl( void )
    {
    CTFStub* stub = CTFFactory::Stub( KTFStubTypeRemConControl );
    return STATIC_CAST( CRemConControl*, stub );
    }

MRemConControl::MRemConControl( void )
    {
    }
    
    
MRemConControl::~MRemConControl( void )
    {
    }


CRemConControl::CRemConControl( void )
: CTFRemoteStub( KTFStubTypeRemConControl )
    {
    
    iKeyEvent  = ETFRemConEvent_None;
    iParameter = 0; 
    iError     = 0;   
    
    iTargetInterfaceSelector = NULL; 
    iPocTarget               = NULL;
    iCoreTarget              = NULL;
    iSyncTarget              = NULL;    
    iCallTarget = NULL;
    		
    }
    
//lint -e1740 Pointer not directly deleted by destructor
CRemConControl::~CRemConControl( void )
    {
    Finalize();
    COMPONENT_TRACE( ( _L( "CRemConControl::~CRemConControl()" ) ) );
    }
//lint +e1740    


void CRemConControl::InitializeL( void )
    {
    
    TInt retval = KErrNone;
    retval = RProperty::Define(KTFRemConTestProperty, KTFRemConTriggerCmdKey, RProperty::EByteArray, KReadPolicy, KWritePolicy);    
	if(retval != KErrAlreadyExists)
    	User::LeaveIfError(retval);           
	
    retval = RProperty::Define(KTFRemConTestProperty, KTFRemConTriggerNotifyCmdKey, RProperty::EByteArray, KReadPolicy, KWritePolicy);    
	if(retval != KErrAlreadyExists)
    	User::LeaveIfError(retval);       	
	
    User::LeaveIfError(iSubscribe.Attach(KTFRemConTestProperty, KTFRemConTriggerCmdKey));       
    	
	TInt retval1 = KErrNone;
    retval1 = RProperty::Define(KPSUidCtsyCallInformation, KCTsyCallState, RProperty::EInt, KReadPolicy, KWritePolicy);    
	if(retval1 != KErrAlreadyExists)
		User::LeaveIfError(retval);           
    
    if ( iCmdReceiverFromStub == NULL)
        {
        iCmdReceiverFromStub = CRemConPSPCHObserver::NewL();
	    iCmdReceiverFromStub->Start();    
        }

    retval = RProperty::Define(KTFRemConTestProperty, KTFRemConClientKey, RProperty::EByteArray, KReadPolicy, KWritePolicy);    
    
    if( retval != KErrAlreadyExists )
        {
        User::LeaveIfError(retval);
        }
    
    COMPONENT_TRACE( ( _L( "CRemConControl::InitializeL - return" ) ) );
    }

void CRemConControl::SwapToNormalHandlerL( void )
    {
		
	 if(iTargetInterfaceSelector == NULL)
         {
	     iTargetInterfaceSelector  = CRemConInterfaceSelector::NewL();
         
         if(iPocTarget == NULL)
             {
             iPocTarget = CRemConPoc::NewL(iTargetInterfaceSelector);
             }
        				
         if(iSyncTarget == NULL)
             {
        	 iSyncTarget = CRemConSync::NewL(iTargetInterfaceSelector);
        	 }
         
         if(iCallTarget == NULL)
             {
             iCallTarget = CRemConCall::NewL(iTargetInterfaceSelector);
             }

         if(iCoreTarget == NULL)
            {
            iCoreTarget = CRemConCore::NewL(iTargetInterfaceSelector);
            }
        
         if( !iTargetInterfaceSelector->ControllerOpened() )
            {
            iTargetInterfaceSelector->OpenControllerL();
            }
            if( !iTargetInterfaceSelector->TargetOpened() )
            {
            iTargetInterfaceSelector->OpenTargetL();		 
            }
        }
    }

void CRemConControl::Finalize( void )
    {
    COMPONENT_TRACE( ( _L( "CRemConControl::Finalize()" ) ) );
    
       if ( iPocTarget != NULL )
    		{
    		delete iPocTarget;
    		iPocTarget = NULL;
    		}
    		
		if ( iCoreTarget != NULL )
    		{
    		delete iCoreTarget;
    		iCoreTarget = NULL;
    		}
		
		if ( iSyncTarget != NULL )
    		{
    		delete iSyncTarget;
    		iSyncTarget = NULL;
    		}
		
		if ( iCallTarget != NULL )
		    {
		    delete iCallTarget;
		    iCallTarget = NULL;
		    }
	
        if ( iCmdReceiverFromStub != NULL)
    	    {
    	    iCmdReceiverFromStub->Cancel();
    		delete iCmdReceiverFromStub;
    		iCmdReceiverFromStub = NULL;
    		}    	    			    

        if ( iTargetInterfaceSelector != NULL )
	    	{
	    	delete iTargetInterfaceSelector;
	    	iTargetInterfaceSelector = NULL;
	    	}
        
        if ( iAudioClientProc.Handle() )
            {
            iAudioClientProc.Kill( KErrNone );
            }
        
        //Just increase test coverage!
	    User::After(1000000);
	    
	    COMPONENT_TRACE( ( _L( "CRemConControl::Finalize() - Success" ) ) );
	    }

void CRemConControl::DeleteInstances( void )
    {
    COMPONENT_TRACE( ( _L( "CRemConControl::Finalize()" ) ) );
		
		if ( iPocTarget != NULL )
    		{
    		delete iPocTarget;
    		iPocTarget = NULL;
    		}
    		
		if ( iCoreTarget != NULL )
    		{
    		delete iCoreTarget;
    		iCoreTarget = NULL;
    		}
		
		if ( iSyncTarget != NULL )
    		{
    		delete iSyncTarget;
    		iSyncTarget = NULL;
    		}
		
		if( iCallTarget != NULL )
		    {
		    delete iCallTarget;
		    iCallTarget = NULL;
		    }
	    }


// This method calls API of module under test.
void CRemConControl::CallApiFunctionL( TRemConControlTestCaseState& aParameter )
    {
    //COMPONENT_TRACE( ( _L( "CRemConControl::CallApiFunctionL(%d, %d, %d)" ), aParameter.iDosFunction, aParameter.iArg1, aParameter.iArg2 ) );
    
    TInt retval = KTFErrRemConEventMissing;
    switch ( aParameter.iFunction )
        {

       	case ETFRemConEvent_NormalHandler:
            	{
            	    SwapToNormalHandlerL();
    				retval = KErrNone;
    			break;
            	}
            	                           
    	case ETFRemConEvent_CmdHandler:
            	{            	 
    				retval = KErrNone;
    			break;
            	}
        
        case ETFRemCon_Subscribe:
        	{
        	
		    	TPckgBuf<TRemConStubPSData> keyDatabuf;		
		    	TRemConStubPSData psData;
				psData.iKeyEvent     = aParameter.iKeyEvents;
				psData.iOperationId  = aParameter.iArg1;
				psData.iData.Copy( aParameter.iArg2 );
				psData.iUseConverter = aParameter.iUseConverter;
				keyDatabuf = psData;
	    		retval = iSubscribe.Set(KTFRemConTestProperty, KTFRemConTriggerCmdKey,keyDatabuf);
	    		retval = KErrNone;
    
	  		break;
        	}         
        	
        case ETFRemCon_NotifyCommandSubscribe:
        	{
		    	TPckgBuf<TRemConStubPSData> keyDatabuf;		
		    	TRemConStubPSData psData;
				psData.iKeyEvent     = aParameter.iKeyEvents;
				psData.iOperationId  = aParameter.iArg1;
				psData.iData.Copy( aParameter.iArg2 );
				psData.iUseConverter = aParameter.iUseConverter;
				keyDatabuf = psData;
	    		retval = RProperty::Set(KTFRemConTestProperty, KTFRemConTriggerNotifyCmdKey,keyDatabuf);
	    		retval = KErrNone;
    
	  		break;
        	}             
		case ETFRemCon_PocButtonDownResponse:
        	{
				iPocTarget->APOCButtonDownResponse();
				retval = KErrNone;
			break;
        	}

		case ETFRemCon_PocButtonUpResponse:
        	{
				iPocTarget->APOCButtonUpResponse();
				retval = KErrNone;
			break;
        	}
		case ETFRemCon_SyncButtonResponse:
        	{
        		iSyncTarget->SynchronizeResponse();				
        		retval = KErrNone;				
			break;
        	}
       	
       	case ETFRemCon_AddressOutgoingCommand:
        	{
        		iCoreTarget->Play();				
        		retval = KErrNone;				
			break;
        	}

       	case ETFRemCon_PermitOutgoingCommand:
        	{        	
        		iCoreTarget->Stop();				
        		retval = KErrNone;				
			break;
        	}
       	
       	case ETFRemCon_CancelOutgoingCommand:
        	{        	
        		iCoreTarget->Cancel();				        		
        		retval = KErrNone;				
			break;
        	}

       	case ETFRemConEvent_CheckEvent:
        	{   
        	    retval = KErrNone;
        	           	
        		if( aParameter.iExpectedKeyEvent != iKeyEvent )
        		    {
        		    retval = KErrNotFound;
        		    }
                
                if( aParameter.iArg1 != iParameter )
                    {
                    retval = KErrArgument;
                    }
                
                ResetEventResult();                    
                
			break;
        	}

        case ETFRemConEvent_CheckResponse:
        	{        	
        		retval = KErrNone;
        	           	
        		if( aParameter.iExpectedKeyEvent != iKeyEvent )
        		    {
        		    retval = KErrNotFound;
        		    }
                
                if( aParameter.iArg1 != iParameter )
                    {
                    retval = KErrArgument;
                    }
                
                if( iError != KErrNone )    
                    {
                    retval = iError;
                    }

                ResetEventResult();                    
                
			break;
        	}
        case ETFRemConEvent_CheckOutgoingCommandResponse:
        	{        	
        		retval = KErrNone;
        	           	
        		if( aParameter.iExpectedKeyEvent != iKeyEvent && iKeyEvent != ETFRemConEvent_CoreApiPlaybackSpeedX1 )
        		    {
        		    retval = KErrNotFound;
        		    }
                
                if( aParameter.iArg1 != iParameter && iParameter != ERemConCoreApiButtonClick )
                    {
                    retval = KErrArgument;
                    }
                
                if( iError != KErrNone )    
                    {
                    retval = iError;
                    }

                ResetEventResult();                    
                
			break;
        	}
        	
        case ETFRemConEvent_CheckCmdSource:
            {
            TRemConExtCmdSource source;
            retval = KErrNone;
            
            iPocTarget->GetCommandSourceInfo( source );
            
            switch( source )
                {
                case ERemConExtCmdSourceBluetooth:
                    {
                    COMPONENT_TRACE( ( _L( "CRemConControl::CallApiFunctionL() ETFRemConEvent_CheckCmdSource - Bluetooth" ) ) );
                    break;
                    }
                case ERemConExtCmdSourceWired:
                    {
                    COMPONENT_TRACE( ( _L( "CRemConControl::CallApiFunctionL() ETFRemConEvent_CheckCmdSource - Wired" ) ) );
                    break;
                    }
                default:
                    {
                    COMPONENT_TRACE( ( _L( "CRemConControl::CallApiFunctionL() ETFRemConEvent_CheckCmdSource - UnKnown" ) ) );
                    break;
                    }
                }
            if( aParameter.iArg1 != source )
                {
                retval = KErrGeneral;
                }
                
            // For coverage
            if( retval == KErrNone )
            {
            	iCallTarget->GetCommandSourceInfo( source );
	            if( source != ERemConExtCmdSourceUnKnown )
	              {
	              retval = KErrGeneral;
	              }
            }
              
            if( retval == KErrNone )
            {
	            iSyncTarget->GetCommandSourceInfo( source );
	            if( source != ERemConExtCmdSourceUnKnown )
	              {
	              retval = KErrGeneral;
	              }
            }
            
            break;
            }
            
        	case ETFRemConEvent_SetCallState:
        	{        	
        		retval = iSubscribe.Set(KPSUidCtsyCallInformation, KCTsyCallState, aParameter.iArg1);
        		retval = KErrNone;				
			break;
        	}
        	
        case ETFRemConEvent_LoadPCH_NoAccFW:
        	{
        	if( !iTargetInterfaceSelector )
        		{
        		iTargetInterfaceSelector = CRemConInterfaceSelector::NewL();
        		}        	        		
        	retval = KErrNone;
        	}
        	break;                

        case ETFRemCon_LaunchAudioClient:
            {
            // reset return channel
            TRemConClientKeyDataBuf keyDatabuf;     
            TRemConClientKeyData keyData;
            keyData.iOperationId = ERemConCoreApiSelect; // if not changed by audio client maps to ETRemConEvent_None 
            keyData.iButtonAct = ERemConCoreApiButtonPress; 
            keyDatabuf = keyData;
            
            retval = RProperty::Set( KTFRemConTestProperty, KTFRemConClientKey, keyDatabuf );
            
            if ( retval == KErrNone )
                {
                retval = iAudioClientProc.Create( _L("audioclientstub.exe"), KNullDesC );
                
                // wait for start
                if ( retval == KErrNone )
                    {
                    TRequestStatus status;
                    iAudioClientProc.Rendezvous( status );
                    iAudioClientProc.Resume();
                    User::WaitForRequest( status );
                    retval = status.Int();
                    }
                }
            }
            break;
            
        case ETFRemCon_CloseAudioClient:
            if ( iAudioClientProc.Handle() )
                {
                iAudioClientProc.Kill( KErrNone );
                retval = KErrNone;
                }
            break;

        case ETFRemCon_CheckClientKey:
            {
            TRemConClientKeyDataBuf keyDataBuf;
            retval = RProperty::Get( KTFRemConTestProperty, KTFRemConClientKey, keyDataBuf );
            if ( retval == KErrNone )
                {
                if ( aParameter.iExpectedKeyEvent != MapRemConToTfKey( keyDataBuf().iOperationId ) )
                    {
                    retval = KErrNotFound;
                    }
                
                if( aParameter.iArg1 != keyDataBuf().iButtonAct )
                    {
                    retval = KErrArgument;
                    }
                
                }
            }
            break;
            
        default:
	        //TRACE_ASSERT_FALSE();
            break;
        }
    User::LeaveIfError( retval );
    }


void CRemConControl::ResetEventResult()
    {
    //Reset values   
    iKeyEvent  = ETFRemConEvent_None;
    iParameter = 0; 
    iError     = 0;   
    }
// This method is used by DsyTest to ask the current test case id. Call from 
// DsyTest to this method is delivered by SAA test framework's client server.
void CRemConControl::ProcessMessageAndCompleteL( const RMessage2& aMessage )
    {    
    aMessage.Complete( KErrNone );
    }

// This method can be called by SAA test framework when the method above is 
// completed. 
void CRemConControl::MessageCompleteL( TInt /*aResult*/ )
    {
    }

void CRemConControl::NotifyKeyEvent( TInt aEvent, TInt aParameter ) 
{
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL -  CRemConControl::NotifyKeyEvent( %d, %d )" ), aEvent, aParameter ) );
	
	iKeyEvent      = aEvent; 
	iParameter     = aParameter;

    COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConControl::NotifyKeyEvent - return" ) ) );   	
}

void CRemConControl::CompleteKeyEventResponse( TInt aEvent, TInt aParam, TInt aError ) 
{
	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL -  CRemConControl::CompleteKeyEventResponse( %d, %d, %d )" ), aEvent, aParam, aError ) );
	
    iKeyEvent      = aEvent; 
	iParameter     = aParam;
	iError         = aError;

    COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL - CRemConControl::NotifyKeyEvent - return" ) ) );   	
}


