// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "HWRMLightTestFappui.h"
#include "HWRMLightTestFappview.h"
#include "HWRMLightTestF.pan"
#include "HWRMLightTestF.hrh"
#include "testcasecontroller.h"

_LIT(KDebugPrefix,"HWRMLightTestF # ");
	
void CHWRMLightTestFAppUi::ConstructL()
    {
    BaseConstructL();
		
    iAppView = CHWRMLightTestFAppView::NewL(ClientRect());
    AddToStackL(iAppView);
    
    iTestCaseController = CHwrmTestCaseController::NewL(*this);
    }

CHWRMLightTestFAppUi::CHWRMLightTestFAppUi()                              
    {
		
	_LIT(KLogFile,"C:\\HWRMLightTest.log");
		
	TRAPD( err, iLog.OpenL(KLogFile, EFalse, EFalse, EFalse, Log::ZERO ) );
	if( err == KErrNone )
		{
		iLog.Printf(_L("Start HWRM Light tests..."));
		iLog.Printf(_L("   "));
		}
			
    }

CHWRMLightTestFAppUi::~CHWRMLightTestFAppUi()
    {
    delete iTestCaseController;
    
    if (iAppView)
        {
        iEikonEnv->RemoveFromStack(iAppView);
        delete iAppView;
        }         
    }

void CHWRMLightTestFAppUi::InfoMessage(const TDesC& aMessage)
	{
	iEikonEnv->InfoMsg(aMessage);
	}

// FUTURE TASK: Refactor duplicate code in LogMessage and DebugMessage
// ALSO GET RESULT (PASS/FAIL) AS PARAMETER (OPTIONAL, IF NOT PRESENT ASSUME ONLY A COMMENT)
void CHWRMLightTestFAppUi::LogMessage(const TDesC& aMessage)
	{
	HBufC* dbgMsg(NULL);
	TRAPD(err,	dbgMsg = HBufC::NewL(KDebugPrefix().Length() + aMessage.Length()));
	if (err != KErrNone)
		return;
	
	CleanupStack::PushL(dbgMsg);
	
	dbgMsg->Des().Append(KDebugPrefix);
	dbgMsg->Des().Append(aMessage);
	
	iLog.Printf(*dbgMsg);
	
	CleanupStack::PopAndDestroy(dbgMsg);
	}
	
void CHWRMLightTestFAppUi::DebugMessage(const TDesC& aMessage)
	{

	HBufC* dbgMsg(NULL);
	TRAPD(err,	dbgMsg = HBufC::NewL(KDebugPrefix().Length() + aMessage.Length()));
	if (err != KErrNone)
		return;
	
	CleanupStack::PushL(dbgMsg);
	
	dbgMsg->Des().Append(KDebugPrefix);
	dbgMsg->Des().Append(aMessage);
	
	RDebug::Print(*dbgMsg);
	
	CleanupStack::PopAndDestroy(dbgMsg);
	}
	

void CHWRMLightTestFAppUi::HandleCommandL(TInt aCommand)
    {
    switch( aCommand )
        {
        case EEikCmdExit: // pass through
             Exit();
             break;

		// determine wether to use Extended or SDK API for running Basic cases
        case EHWRMLightTestFCmdBasicXAPI: // fall through
        case EHWRMLightTestFCmdBasicSAPI:
            {         
            _LIT( message, "Basic test cases" );
            iEikonEnv->InfoMsg(message); 		
      		
      		THWRMLightTestFAPIType apiType(EHWRMApiTypeNoAPI);
      		if (aCommand == EHWRMLightTestFCmdBasicXAPI)
      			{
      			apiType = EHWRMApiTypeXAPI;
      			}
      		else
      			{
      			apiType = EHWRMApiTypeSAPI;
      			}
      		
      		iTestCaseController->RunBasicTestCases(apiType);
      		
      		iLog.Printf( _L( "All basic test cases executed.\n") );
            }
            break;

        case EHWRMLightTestFCmdMultiClient:
            {
            _LIT( message,"Multiclient cases" );
            iEikonEnv->InfoMsg(message);
			iTestCaseController->RunMulticlientTestCases();
            iLog.Printf( _L( "All multiclient test cases executed.\n") );
            }
            break;		
            
            
        case EHWRMLightTestFCmdRepeatInvalidParams:
            {
            _LIT( message,"Repeating invalid parameters" );
            iEikonEnv->InfoMsg(message);
      		iTestCaseController->RunTestCases( ERepeatInvalidParamsTest );    
      		iLog.Printf( _L( "Repeating invalid parameters - Done\n") );
            }
            break;		
       
            
        case EHWRMLightTestFCmdVibraTest:
            {
            _LIT( message, "Vibra test (30 min)" );
            iEikonEnv->InfoMsg(message);
      		iTestCaseController->RunTestCases( EMaliciousVibraTest );    
      		iLog.Printf( _L( "Vibra test (30 min) - Done\n" ) );
            }
            break;		
       
            
        case EHWRMLightTestFCmdWaitChargingNotify:
            {
            _LIT( message,"Wait charging notify" );
            iEikonEnv->InfoMsg(message);
      		iTestCaseController->RunTestCases( EWaitForChargingNotify );    
      		iLog.Printf( _L( "Wait charging notify - Done\n" ) );
            }
            break;		
       
                                                
        default:
        	        		
            Panic( EHWRMLightTestFUi );
            break;
        }
    
    _LIT(messageEnd,"All Tests executed, Report created\n");
    iEikonEnv->InfoMsg(messageEnd);
    }
