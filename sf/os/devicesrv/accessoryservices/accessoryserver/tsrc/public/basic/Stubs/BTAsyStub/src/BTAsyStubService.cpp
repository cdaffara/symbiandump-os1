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
#include <TfAccessoryTestControlTypes.h>
#include "siftrace.h"
#include "BTASYStubObserver.h" 

// ----------------------------------------------------------------------------------
// CBTASYStubService::CBTASYStubService() 
// ----------------------------------------------------------------------------------	
CBTASYStubService::CBTASYStubService() 
	{						
		
	
	}
	
// ----------------------------------------------------------------------------------
// CBTASYStubService::ConstructL() 
// ----------------------------------------------------------------------------------	
void CBTASYStubService::ConstructL(void) 
	{						

	TInt err(KErrNone);
	
	err = iAccessoryServer.Connect();                
    
	if ( err != KErrNone )
        {
        COMPONENT_TRACE( _L("BTASYSTUB - CBTASYStubService::ConstructL() - failed to connect AccServer") );
        User::Leave( err );
        }
    
    COMPONENT_TRACE( ( _L( "BTASYSTUB - CASYParameterResolver::ConstructL() - return" ) ) );
	}	
//
// ----------------------------------------------------------------------------------
// CBTASYStubService::~CBTASYStubService()
// ----------------------------------------------------------------------------------	
CBTASYStubService::~CBTASYStubService()
	{				
		COMPONENT_TRACE( ( _L( "BTASYSTUB - CTFTestControlObserver::~CTFTestControlObserver - Enter" ) ) );    
        iAccessoryServer.Close();
	    COMPONENT_TRACE( ( _L( "BTASYSTUB - CTFTestControlObserver::~CTFTestControlObserver - succesful" ) ) );    
	}
//
// ----------------------------------------------------------------------------------
// CBTASYStubService::Service()
// ----------------------------------------------------------------------------------
void CBTASYStubService::Service( 
    TTFASYReference aMethod,
    TInt64 aParam, 
    TInt aTestCaseID,
    TAccPolGenericID aGenericID,
    TUint32& aParam2 )
    {
    
        COMPONENT_TRACE( ( _L( "BTASYSTUB - CTFTestControlObserver::Service()" ) ) );    
        TInt err(KErrNone);
        
        if(aMethod == ETFBTALinkClosedNo || aMethod == ETFBTALinkOpenedNo)
        {
        
            RAccessoryBTControl aAccessoryBTControl;
            
            err = aAccessoryBTControl.CreateSubSession( iAccessoryServer ); 
            if ( err != KErrNone )
            {
            COMPONENT_TRACE( _L("BTASYSTUB - CBTASYStubService::ConstructL() - failed to connect RAccessoryBTControl") );
            User::Leave( err );
            }
            
        	iTestCaseID = aTestCaseID;
        	
    		switch ( aMethod )
           	    {
            	case ETFBTALinkClosedNo:
           		    {
                    aAccessoryBTControl.BluetoothAudioLinkClosedNotifyL(aParam, (TAccAudioType)aParam2 );								   	
           	    	}
            	break;
           	
            	case ETFBTALinkOpenedNo:
            		{
                    aAccessoryBTControl.BluetoothAudioLinkOpenedNotifyL(aParam, (TAccAudioType)aParam2 );								   	 
           	    	}
                break;
           	
           	    
           	
    	        default:
    	            {
                    COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubService::Service - Subscribe destination is WIRED ASY" ) ) );    
    	            }
                    break;			
       	        }
       	    
       	    aAccessoryBTControl.CloseSubSession();
            }
        else
            {
            switch ( aMethod )
           	{
                case ETFAsySetBoolValue:
           	        {           	        
                    TAccPolNameRecord nameRecord;
			        nameRecord.SetNameL( aParam2 );
                    TAccValueTypeTBool aValue = { EFalse };
                    aValue.iValue = aParam;
                    RAccessoryBTControl accessoryBTControl;   
                             
                    err = accessoryBTControl.CreateSubSession( iAccessoryServer ); 
                      
           	        accessoryBTControl.SetValueL( aGenericID, nameRecord, aValue );  
           	        
           	        accessoryBTControl.CloseSubSession();         	        
           	        }
           	        break;
           	        
           	    case ETFAsyIntNotify:
           	        {           	        
           	        TBTDevAddr tempAddr( 0 );
                    TAccPolNameRecord nameRecord;
			        nameRecord.SetNameL( aParam2 );
                    TAccValueTypeTInt value;
                    value.iValue = aParam;
                    RAccessoryBTControl accessoryBTControl;   
                             
                    err = accessoryBTControl.CreateSubSession( iAccessoryServer ); 
                      
           	        accessoryBTControl.AccessoryValueChangedNotifyL( tempAddr, nameRecord, value );  
           	        
           	        accessoryBTControl.CloseSubSession();         	        
           	        }
           	        break;
           	    
           	    default:
    	            {
                    COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubService::Service - Subscribe destination is unknown" ) ) );    
    	            }
                    break;			
       	        }
            }
   	    
   	    COMPONENT_TRACE( ( _L( "BTASYSTUB - CTFTestControlObserver::Service() - return" ) ) );               
    }    
//
// ----------------------------------------------------------------------------------
// CBTASYStubService::GetTestCaseID()
// ----------------------------------------------------------------------------------
TInt CBTASYStubService::GetTestCaseID()
{
	COMPONENT_TRACE( ( _L( "BTASYSTUB - CBTASYStubService::GetTestCaseID()" ) ) );    
	return iTestCaseID;
}


