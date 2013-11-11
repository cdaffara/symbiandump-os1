/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include "BTASYStubCmdHandlerBase.h"
#include "BTAsyStubMainServiceBase.h"
#include "siftrace.h"
#include "TfAccessoryTestControlTypes.h"
#include "bttypes.h"

//
// ----------------------------------------------------------------------------------
// CBTASYStubCmdHandlerBase::CBTASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
//  
CBTASYStubCmdHandlerBase::CBTASYStubCmdHandlerBase()
	{
	
	COMPONENT_TRACE( ( _L( "BTASYStub - CBTASYStubCmdHandlerBase::CBTASYStubCmdHandlerBase()" ) ) );
	}

//
// ----------------------------------------------------------------------------------
// CBTASYStubCmdHandlerBase::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CBTASYStubCmdHandlerBase* CBTASYStubCmdHandlerBase::NewL()
	{
	return new ( ELeave ) CBTASYStubCmdHandlerBase();
	}


//
// ----------------------------------------------------------------------------------
// CBTASYStubCmdHandlerBase::~CBTASYStubCmdHandlerBase()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CBTASYStubCmdHandlerBase::~CBTASYStubCmdHandlerBase()
	{
	}

//
// ----------------------------------------------------------------------------------
// CBTASYStubCmdHandlerBase::ProcessCommandL()
// ----------------------------------------------------------------------------------
// 
void CBTASYStubCmdHandlerBase::ProcessCommandL( const TProcessCmdId aCommand, const TASYCmdParams& aCmdParams )
    {
    COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTBTASYStubCmdHandlerBase::ProcessCommandL()" ) ) );

    TASYBTAccInfo btInfo;
    btInfo.iReserved = 0;
    TASYCommandParamRecord cmdParams ( aCmdParams() );//For debugging

    switch ( aCommand )
        {
        case ECmdGetSupportedBTProfiles: 
            {
            TBTDevAddr btAddr( aCmdParams().iCmdValue );
          	switch( aCmdParams().iCmdValue )	          
          	    {
          	    case KBTAddrCarkit:
          	        {
          	        btInfo.iDeviceInfo = 0x191;
                    btInfo.iDeviceType = KDTCarKit;
                    btInfo.iProfiles = KAccInfoHFP;
                    btInfo.iSupportedFeatures.At(0) = 1;   
                                
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;
          	    
          	    case KBTAddrHeadsetHSPAndHFP:
          	        {
          	        btInfo.iDeviceInfo = 0x192;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP | KAccInfoHFP;
                    btInfo.iSupportedFeatures.At(0) = 1;   
                    btInfo.iSupportedFeatures.At(1) = 2;   
                	
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;
          	    
          	    case KBTAddrHeadsetHSP:
          	        {
          	        btInfo.iDeviceInfo = 0x193;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP; 
                    btInfo.iSupportedFeatures.At(0) = 1;   
                	
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;
          	    
          	    case KBTAddrHeadsetHFP:
          	        {
          	        btInfo.iDeviceInfo = 0x194;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHFP; 
                    btInfo.iSupportedFeatures.At(1) = 1;   
                    btInfo.iReserved |= KAccInfoLatency;
                	
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;

                case KBTAddrHeadsetAVRCP:
          	        {
          	        btInfo.iDeviceInfo = 0x195;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHFP; 
                    btInfo.iSupportedFeatures.At(1) = 1;   
                	
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;
                 
                case KBTAddrHeadseA2DP:
          	        {
          	        btInfo.iDeviceInfo = 0x196;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHFP; 
                    btInfo.iSupportedFeatures.At(1) = 1;   
                    btInfo.iReserved |= KAccInfoDRM;
                	
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;          	                   	        

                case KBTAddrHeadsetEmptyPR:
          	        {
          	        btInfo.iDeviceInfo = 0x197;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = 0; 
                	
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;

                case KBTAddrHeadsetNokiaSPId:
          	        {
          	        btInfo.iDeviceInfo = 0x181;
                    btInfo.iDeviceType = KDTHeadset;
                	btInfo.iProfiles = KAccInfoHSP; 
                    btInfo.iSupportedFeatures.At(0) = 1;   
                    
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;
          	        
          	    case KBTAddrNoResponse:
          	        {
          	        // No response
          	        }
          	        break;    

                case KBTAddrErrCode:
          	        {
          	        btInfo.iDeviceInfo = 0x198;
                    btInfo.iDeviceType = KDTHeadset;
                	btInfo.iProfiles = KAccInfoHSP; 
                    btInfo.iSupportedFeatures.At(0) = 1;   
                    
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrGeneral );
          	        }
          	        break;

                case KBTAllBitsOn: // All supported bits on
          	        {
          	        btInfo.iDeviceInfo = 0x198;
                    btInfo.iDeviceType = KDTHeadset;
                	btInfo.iProfiles = KAccInfoHSP | KAccInfoHFP | KAccInfoAVRCP | KAccInfoA2DP; 
                    btInfo.iSupportedFeatures.At(0) = 0xFFFF;   
                    btInfo.iSupportedFeatures.At(1) = 0xFFFF;   
                    btInfo.iSupportedFeatures.At(2) = 0xFFFF;   
                    btInfo.iSupportedFeatures.At(3) = 0xFFFF;   

                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );

          	        }
          	        break;

          	    default:
          	        {                	     			
                    btInfo.iDeviceInfo = 0x181;
                    btInfo.iDeviceType = KDTHeadset;
                    btInfo.iProfiles = KAccInfoHSP | KAccInfoHFP;
                    btInfo.iSupportedFeatures.At(0) = 1;   
                    btInfo.iSupportedFeatures.At(1) = 2;  
             
                	TASYBTAccInfoPckgC btInfoPckgC( btInfo );              					       		
        		  	ProcessResponseL( btInfoPckgC, KErrNone );
          	        }
          	        break;
          	    }
            }
            break;
            
        case ECmdProcessCommandInit:
            {
            COMPONENT_TRACE( ( _L( "BTASYSTUB - CASYStubCmdHandlerBase::ProcessCommandL() - ProcessCommandInit" ) ) );
            TAccValueTypeTBool boolInitResponse;            
            boolInitResponse.iValue = ETrue;
            ProcessResponseL( boolInitResponse );            
            }
            break;
            
        default:
            {
            COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTBTASYStubCmdHandlerBase::ProcessCommandL() - ERROR: unhandled command" ) ) );  
            TRACE_ASSERT_ALWAYS;
            User::Panic(_L("BT-ASY Stub"), KErrGeneral );//PANIC_ALWAYS(KErrGeneral);
            }
            break;
        }       
		
    }
    
