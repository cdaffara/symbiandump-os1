// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file poshandlerinitstep.cpp
*/

#include "poshandlerinitstep.h"
#include "te_suplproxypmsuitedefs.h"

/**
 * Constructor
 */
CPosHandlerInitStep::CPosHandlerInitStep()
	{
	SetTestStepName(KPosHandlerIntStep);
	}

/**
 * Destructor
 */
CPosHandlerInitStep::~CPosHandlerInitStep()
	{
	if (iSuplPosSession)
	    {
	    delete iSuplPosSession;
		iSuplPosSession = NULL;
	    }
	
	}


TVerdict CPosHandlerInitStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Call base class method for pre test actions	
	CTe_SuplProxyPMSuiteStepBase::doTestStepPreambleL();
			
	// Connect to the Channel
	//iGatewayChannel = CGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	
	}


/** Perform CPosHandlerInitStep test step.
 *
 *
 * @return TVerdict test result code
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
*/
TVerdict CPosHandlerInitStep::doTestStepL()

	{
	_LIT(KTestMethod, "CPosSessionInitStep::doTestStepL()");
	INFO_PRINTF1(KTestMethod);
	
	TBool runStatus=EFalse;
	GetBoolFromConfig(ConfigSection(),KTe_SuiteBool,runStatus);
	
	if (!runStatus)
	    {
	    _LIT(KIniWarning, "Test run disabled in the ini file");
	    WARN_PRINTF1(KIniWarning);
	    SetTestStepResult(EFail);
	    }
	else
	    {
            if (TestStepResult()==EPass)
                {
                TInt testIndex = KErrNotFound;
                if(!GetIntFromConfig(ConfigSection(),KTe_SuiteTestId,testIndex))
                    {
                    _LIT(KIndexNotFound, "Failed to find test index");
                    INFO_PRINTF1(KIndexNotFound);
                    User::Leave(KErrNotFound);
                    }
                else
                    {
                    _LIT(KTestName, "Calling test by index");
                    INFO_PRINTF1(KTestName);
                    TestByIndexL(testIndex);
                    }
                }
	    }
	
	return TestStepResult();
	
	}


void CPosHandlerInitStep::TestByIndexL(TInt aIndex)
{
	// Set test result to fail initially but will be reset later if no leaves and all ok
	SetTestStepResult(EFail);

	// The following are the known tests: Since possession test cases start at 11, we have effectively
	// reserved up to 10 poshandler cases.
    switch (aIndex)
        {
        case 1:
            _LIT(KTestName1, "POS Handler init Test");
            INFO_PRINTF1(KTestName1);
            TestPosHandlerInitL(aIndex);
            break;
        
        case 2:
            _LIT(KTestName2, "POS Handler init - Cancel init Test");
            INFO_PRINTF1(KTestName2);
            TestPosHandlerCancelInitL(aIndex);
            break;
        
        case 3:
            _LIT(KTestName3, "POS Handler init - Create POS session Test");
            INFO_PRINTF1(KTestName3);
            TestCreatePosSessionL(aIndex);
            break;
        
        case 4:
            _LIT(KTestName4, "POS Handler init - Create POS sesion Negative Test");
            INFO_PRINTF1(KTestName4);
            #ifdef _DEBUG
                {
                TestCreatePosSessionHeapFailL(aIndex);
                }
            #else
                {
                _LIT(KNote, "NOTE - TEST NOT RUN as this test is valid only for DEBUG runs");
                INFO_PRINTF1(KNote);
                SetTestStepResult(EPass);
                }
            #endif
            break;
        case 5:
            _LIT(KTestName5, "Verify the Version of COMASuplPosHandlerBase implementation");
            INFO_PRINTF1(KTestName5);
            TestCheckPluginVersionL(aIndex);
            break;
        case 6:
            _LIT(KTestName6, "Implementation UID of COMASuplPosHandlerBase implementation");
            INFO_PRINTF1(KTestName6);
            TestCheckPluginUidL(aIndex);
            break;
        default:
            _LIT(KIniFileErr, "Invalid TestId from the ini file");
            ERR_PRINTF1(KIniFileErr);
            break;
        }
}

/**
 * 
 */
void CPosHandlerInitStep::TestPosHandlerInitL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosHandlerInitStep::TestPosHandlerInitL");
    INFO_PRINTF1(KTestName);
            
    TRequestStatus aStatus;
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(aStatus);
    User::WaitForRequest(aStatus);
    
    if (aStatus.Int() == KErrNone)
        {
        SetTestStepResult(EPass);
        }
    else
        {
        _LIT(KInitErr, "Expected KErrNone from InitializeL but got %d");
        INFO_PRINTF2(KInitErr, aStatus.Int());			
        }            
    }

/**
 * 
 */
void CPosHandlerInitStep::TestPosHandlerCancelInitL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosHandlerInitStep::TestPosHandlerCancelInitL");
    INFO_PRINTF1(KTestName);
        
    TRequestStatus aStatus;
    
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(aStatus);
    
    // Cancel initialize does nothing - so initializeL should complete with KErrNone
    iSuplPosHandler->CancelInitialize();
    
    User::WaitForRequest(aStatus);
    
    if (aStatus.Int() == KErrNone)
        {
		SetTestStepResult(EPass);
        }
	else
		{
		_LIT(KCancelInitErr, "Expected KErrNone from CancelInitialize but got %d");
		INFO_PRINTF2(KCancelInitErr, aStatus.Int());			
		}
    }

/**
 * 
 */
void CPosHandlerInitStep::TestCreatePosSessionL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosHandlerInitStep::TestCreatePosSessionL");
    INFO_PRINTF1(KTestName);
    
    TRequestStatus aStatus;
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(aStatus);
    User::WaitForRequest(aStatus);
    
    if (aStatus.Int() == KErrNone)
        {
        iSuplPosSession = iSuplPosHandler->CreateNewSessionL(this);
        if ( iSuplPosSession )
			{
            SetTestStepResult(EPass);
			}
		else
			{
			_LIT(KSessionCreateErr, "CreateNewSession() returned NULL");
			INFO_PRINTF1(KSessionCreateErr);			
			}				
        }
    else
        {
        _LIT(KInitErr, "Expected KErrNone from InitializeL but got %d");
        INFO_PRINTF2(KInitErr, aStatus.Int());          
        }  
    }

/**
 * 
 */
void CPosHandlerInitStep::TestCreatePosSessionHeapFailL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosHandlerInitStep::TestCreatePosSessionL");
    INFO_PRINTF1(KTestName);
    
    TRequestStatus aStatus;
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(aStatus);
    User::WaitForRequest(aStatus);
    
    if (aStatus.Int() == KErrNone)
        {
        __UHEAP_FAILNEXT(0);
        TRAPD(allocErr,iSuplPosSession = iSuplPosHandler->CreateNewSessionL(this));
           
        if ( allocErr == KErrNoMemory )
            {
            SetTestStepResult(EPass);
            }
         else
            {
            _LIT(KExpectedNoMemory, "Expected KErrNoMemeory from CreateNewSession but got %d");
            INFO_PRINTF2(KExpectedNoMemory, aStatus.Int());                       
            }
        }
    else
        {
        _LIT(KInitErr, "Expected KErrNone from InitializeL but got %d");
        INFO_PRINTF2(KInitErr, aStatus.Int());          
        }  
    }


/** Test case to verify the Version of COMASuplPosHandlerBase implementation.
 * 
 */
void CPosHandlerInitStep::TestCheckPluginVersionL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosHandlerInitStep::TestCheckPluginVersion");
    INFO_PRINTF1(KTestName);
     
    TRequestStatus aStatus;
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(aStatus);
    User::WaitForRequest(aStatus);
      
    if (aStatus.Int() == KErrNone)
        {
        TInt version;
        TRAPD(err, version = iSuplPosHandler->VersionL());
        if ( err == KErrNone )
            {
            if(version == 1)
                {
                SetTestStepResult(EPass);                  
                }
            else
                {
                _LIT(KInitErr, "Expected version 1 but got %d");
                ERR_PRINTF2(KInitErr, version); 
                }
            }
        else
            {
            _LIT(KVersionLleaveErr, "COMASuplPosHandlerBase::VersionL() call left with error - %d");
            ERR_PRINTF2(KVersionLleaveErr,err);            
            }               
        }
    else
        {
        _LIT(KInitErr, "Expected KErrNone from InitializeL but got %d");
        ERR_PRINTF2(KInitErr, aStatus.Int());          
        }  
    }

/** Test case to verify the implementation UID of COMASuplPosHandlerBase implementation.
 * 
 */
void CPosHandlerInitStep::TestCheckPluginUidL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosHandlerInitStep::TestCheckPluginVersion");
     INFO_PRINTF1(KTestName);
      
     TRequestStatus aStatus;
     // Creates a session with the SUPL Proxy protocol module
     iSuplPosHandler->InitializeL(aStatus);
     User::WaitForRequest(aStatus);
     if (aStatus.Int() == KErrNone)
         {
         TUid posHandlerImplUid = iSuplPosHandler->ImplementationUID();
         TUid expposHandlerImplUid = {0x102871EF};
         if (posHandlerImplUid == expposHandlerImplUid)
             {
             SetTestStepResult(EPass); 
             }
         else
             {
             _LIT(KInitErr, "Expected the implementation UID %d but got %d");
             ERR_PRINTF3(KInitErr,expposHandlerImplUid,posHandlerImplUid); 
             }
         }
     else
         {
         _LIT(KInitErr, "Expected KErrNone from InitializeL but got %d");
         ERR_PRINTF2(KInitErr, aStatus.Int());          
         } 
    }

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CPosHandlerInitStep::doTestStepPostambleL()
	{

	// Call base class method for post test actions	
	CTe_SuplProxyPMSuiteStepBase::doTestStepPostambleL();
		
	return TestStepResult();
	}


