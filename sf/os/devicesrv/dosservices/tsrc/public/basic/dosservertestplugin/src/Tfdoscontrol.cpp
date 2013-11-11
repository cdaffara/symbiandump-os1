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
* Description:
*
*/


// INCLUDE FILES
#include <ctffactory.h>
#include <tftypes.h>
#include <psvariables.h>
#include "siftrace.h"
#include "dosenums.h"
#include "ctfdoscontrol.h"
#include "ctfdoscontroltestcase.h"
#include "ctfeventlistener.h"
#include "CTFDosSharedDataTC.h"

const TUid KUidDosTestCategory = {0x10200C6A};

MTFDosControl* GetDosServerControl( void )
    {
    CTFStub* stub = CTFFactory::Stub( KTFStubTypeDosServerControl );
    return STATIC_CAST( CTFDosControl*, stub );
    }

MTFDosControl::MTFDosControl( void )
    {
    }
    
    
MTFDosControl::~MTFDosControl( void )
    {
    }
    

CTFDosControl::CTFDosControl( void )
: CTFStub( KTFStubTypeDosServerControl )
    {
    }
    
//lint -e1740 Pointer not directly deleted by destructor
CTFDosControl::~CTFDosControl( void )
    {
    ResetDosServices();
    COMPONENT_TRACE( ( _L( "DOSSERVERTESTPLUGIN - CTFDosServerControl::~CTFDosServerControl() - DosServer connection closed" ) ) );
    }
//lint +e1740    
    
void CTFDosControl::InitializeL( void )
    {
    COMPONENT_TRACE( ( _L( "DOSSERVERTESTPLUGIN - CTFDosServerControl::InitializeL()" ) ) );
    User::LeaveIfError( iDosServer.Connect() );            
    iInitialized = ETrue;
    COMPONENT_TRACE( ( _L( "DOSSERVERTESTPLUGIN - CTFDosServerControl::InitializeL - return" ) ) );
    }

void CTFDosControl::ResetDosServices( void )
    {
    COMPONENT_TRACE( ( _L( "CTFDosServerStub::ResetDosServices()" ) ) );
    if ( iEventListener != NULL )
        {
        iEventListener->Stop();
        delete iEventListener;
        iEventListener = NULL;
        }  
  
    if ( iDosSharedData != NULL )
        {
        iDosSharedData->Close();
        delete iDosSharedData;
        iDosSharedData = NULL;
        }      
      if ( iDosExtension != NULL )
        {
        iDosExtension->Close();
        delete iDosExtension;
        iDosExtension = NULL;
        }
          
    iDosServer.Close();
    iInitialized = EFalse;
    COMPONENT_TRACE( ( _L( "CTFDosServerStub::ResetDosServices() - Success" ) ) );
    }

void CTFDosControl::CallDosFunctionL( TTFDosServerControlTestCaseState& aParameter )
    {
    COMPONENT_TRACE( ( _L( "DOSSERVERTESTPLUGIN - CTFDosServerControl::CallDosFunctionL(%d, %d, %d)" ), aParameter.iDosFunction, aParameter.iArg1, aParameter.iArg2 ) );
    if ( !iInitialized )
        {
        InitializeL();
        }
    TInt retval = KTFErrDosServiceMissing;
    switch ( aParameter.iDosFunction )
        {      
         // SharedData TC
        case ETFDosSharedData_Open:
        	if ( iDosSharedData == NULL )
        		{
        		iDosSharedData = new ( ELeave ) RDosSharedData();
        		retval = iDosSharedData->Open ( iDosServer );
        		}
        	else
        		{
        		retval = KErrNone;
        		}
        	break;
        case ETFDosSharedData_Close:
            TRACE_ASSERT( iDosSharedData != NULL );
            if ( iDosSharedData != NULL )
                {
                iDosSharedData->Close();
                delete iDosSharedData;
                iDosSharedData = NULL;
                }
            retval = KErrNone;
            break;
        case ETFDosSharedData_FillDisk:
        	if ( iSdDisk == NULL )
        	    {
        		iSdDisk = CSdDisk::NewL();
        	    }
        	
       		iSdDisk->FillDisk(iFs, aParameter.iArg1);
       		delete iSdDisk;
       		iSdDisk = NULL;
				
			retval = KErrNone;
			break;
        case ETFDosSharedData_ReleaseDisk:
        	if ( iSdDisk == NULL )
        	    {
        		iSdDisk = CSdDisk::NewL();
        	    }
        	
       		iSdDisk->ReleaseDisk(iFs);
       		delete iSdDisk;
       		iSdDisk = NULL;
				
			retval = KErrNone;
    	    break;
        case ETFDosSharedData_CheckDiskSpace:
        	if ( iSdDisk == NULL )
        	    {
        		iSdDisk = CSdDisk::NewL();
        	    }

      		iFreeDiskSpace = iSdDisk->CheckDiskSpace( iFs );
     
   			RDebug::Print(_L("Shareddatatest ## CheckDiskSpace: %f Bytes"), iFreeDiskSpace);
       		        		      		
       		if ( aParameter.iArg1 != 0 )
       		    {       		    
       		    if ( aParameter.iArg1 == ( TInt(iFreeDiskSpace) / 1024 ) || 
       		         iSdDisk->CheckReserveFileSize( iFs ) == 0 )
       		        {
       			    retval = KErrNone;
       		        }
       		    else 
       		        {
       			    retval = KErrGeneral;
				    iSdDisk->ReleaseDisk(iFs);
	        	
	        	    if ( iDosSharedData != NULL )
	        	        {
					    iDosSharedData->Close();
               		    delete iDosSharedData;
               		    iDosSharedData = NULL;
       			        }
        		
        		    delete iSdDisk;
        		    iSdDisk = NULL;
       		        }
        	    }

   		    else if ( aParameter.iArg2 == 1 )
   		        {
   		        if ( iSdDisk->CheckReserveFileSize( iFs ) == 256*1024 )
   		            {
   		            retval = KErrNone;
   		            }
   		        else
   		            {
   		            retval = KErrGeneral;
   		            }
   		        }
        	    
        	else
        	    {
        	    retval = KErrNone;    
        	    }
    	    break;    	   
        case ETFDosSharedData_RequestFreeDiskSpace:
        	if ( iDosSharedData != NULL )
        	    {
        		RDebug::Print(_L("Shareddatatest ## RequestfreeDiskSpace: %d kB"), aParameter.iArg1 * 1024);
        		iDosSharedData->RequestFreeDiskSpace(aParameter.iArg1 * 1024);
        		retval = KErrNone;
        	    }
	        break;
        case ETFDosSharedData_RequestFreeDiskSpaceCancel:
        	if ( iDosSharedData != NULL )
        	    {
        		RDebug::Print(_L("Shareddatatest ## RequestfreeDiskSpaceCancel"));
        		iDosSharedData->RequestFreeDiskSpaceCancel();
        		retval = KErrNone;
        	    }    	    
	        break;        
	        
	      case ETFDosExtension_Open:
            if ( iDosExtension == NULL )
                {
                iDosExtension = new ( ELeave ) RTFDosExtension();
                retval = iDosExtension->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosExtension_CallFunction:
            TRACE_ASSERT( iDosExtension != NULL );
            if ( iDosExtension != NULL )
                {
                retval = iDosExtension->CallSyncDosExtFunction( aParameter.iArg1, ( TAny* )&aParameter.iArg2, sizeof ( aParameter.iArg2 ), ETrue );
                }
            break;
        case ETFDosExtension_CallFunctionAndComplete:
            TRACE_ASSERT( iDosExtension != NULL );
            if ( iDosExtension != NULL )
                {
                TRequestStatus status;
                retval = iDosExtension->CallAsyncDosExtFunction( status, aParameter.iArg1, ( TAny* )&aParameter.iArg2, sizeof ( TInt ), ETrue );                
                }
            break;
        
        case ETFDosExtension_GeneralEvent:
            TRACE_ASSERT( iDosExtension != NULL );
            
            iEventListener = new ( ELeave ) CTFEventListener( iDosServer );
            iEventListener->StartListeningL( KLDSYDummyEvent, sizeof(TInt), EQueue );
    
            if ( iDosExtension != NULL )
                {
                TRequestStatus status;
                iDosExtension->CallAsyncDosExtFunction( status, aParameter.iArg1, ( TAny* )&aParameter.iArg2, sizeof ( TInt ), EFalse );                
                retval = KErrNone;
                }
            break;    
            
        case ETFDosExtension_GeneralEventArray:
            TRACE_ASSERT( iDosExtension != NULL );
            
            iEventListener = new ( ELeave ) CTFEventListener( iDosServer );
            
            TRegisterEvent eventList[] = 
            {
               { KLDSYDummyEvent, sizeof ( TInt ), EQueue }    
            };
            
            iEventListener->StartListeningL( eventList, sizeof ( eventList ) / sizeof ( TRegisterEvent ) );
    
            if ( iDosExtension != NULL )
                {
                TRequestStatus status;
                iDosExtension->CallAsyncDosExtFunction( status, aParameter.iArg1, ( TAny* )&aParameter.iArg2, sizeof ( TInt ), EFalse );                
                retval = KErrNone;
                }
            break;    
                        
        case ETFDosExtension_Close:
            TRACE_ASSERT( iDosExtension != NULL );
            if ( iDosExtension != NULL )
                {
                iDosExtension->Close();
                delete iDosExtension;
                iDosExtension = NULL;
                }
                
            if ( iEventListener != NULL )
                {
                iEventListener->Stop();
                delete iEventListener;
                iEventListener = NULL;
                }
                    
            retval = KErrNone;
            break;
        
        case ETFRFs_Connect:
					{
					retval = iFs.Connect();	
					}
	        break;

				case ETFRFs_Close:
					{
					iFs.Close();	
					retval = KErrNone;
					}
	      break;
	              	        
        default:
            TRACE_ASSERT_ALWAYS;
        break;
        }
    User::LeaveIfError( retval );
    }

void CTFDosControl::NotifyDosEvent( TInt aEvent, TInt aParameter )
    {
    COMPONENT_TRACE( ( _L( "DOSSERVERTESTPLUGIN - CTFDosServerControl::NotifyDosEvent(%d, %d)" ), aEvent, aParameter ) );
    CTFDosControlTestCase* testCase = STATIC_CAST( CTFDosControlTestCase*, CurrentTestCase() );
    TRACE_ASSERT( testCase != NULL );
    if ( testCase != NULL )
        {
        testCase->NotifyDosEvent( aEvent, aParameter );
        }
    COMPONENT_TRACE( ( _L( "DOSSERVERTESTPLUGIN - CTFDosServerControl::NotifyDosEvent - return" ) ) );
    }
    
TInt RTFDosExtension::CallSyncDosExtFunction( const TInt& aFunc, TAny* aParam = NULL, TInt aParLength = 0, TBool aAutoComplete = ETrue )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallSyncDosExtFunction(0x%x, 0x%x, 0x%x, 0x%x)" ), aFunc, aParam, aParLength, aAutoComplete ) );
    TExtensionParPckg package;
    package().iFunc = aFunc;
    package().iParLength = aParLength;
    package().iAutoComplete = aAutoComplete;
    TPtr8 ptr( ( TUint8* )aParam, aParLength, aParLength );
    TInt result = CallFunction( package, ptr );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallSyncDosExtFunction - return 0x%x" ), result ) );
    return result;
    }


TInt RTFDosExtension::CallAsyncDosExtFunction( TRequestStatus& aStatus, TInt aFunc, TAny* aParam = NULL, TInt aParLength = 0, TBool aAutoComplete = EFalse )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallAsyncDosExtFunction(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), &aStatus, aFunc, aParam, aParLength, aAutoComplete ) );
    TExtensionParPckg package;
    package().iFunc = aFunc;
    package().iParLength = aParLength;
    package().iAutoComplete = aAutoComplete;
    TPtr8 ptr( ( TUint8* )aParam, aParLength, aParLength );
    CallFunction( aStatus, package, ptr );
    User::WaitForRequest( aStatus );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallAsyncDosExtFunction - return 0x%x" ), aStatus.Int() ) );
    return aStatus.Int();
    }

