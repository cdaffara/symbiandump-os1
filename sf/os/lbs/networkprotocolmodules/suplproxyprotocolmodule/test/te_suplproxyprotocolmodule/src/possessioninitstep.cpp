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
 @file possessioninitstep.cpp
*/

#include "possessioninitstep.h"
#include "te_suplproxypmsuitedefs.h"
#include "asyncreqwaiter.h"

#include <lbs/epos_comasuplpossessionbase.h>    // For testing the baseclass methods
#include "epos_tomasuplallowedcapabilities.h"
#include "epos_comasuplsetcapabilities.h"
#include "epos_eomasuplposerrors.h"
//Uncomment if you need any of these suplinfo parameter classes
#include "epos_comasuplinforequestlist.h"
#include "epos_comasuplreqasstdata.h"
#include "epos_comasuplposition.h"
#include "epos_comasuplpospayload.h"
#include "epos_comasuplvelocity.h"


/**
 * Constructor
 */
CPosSessionInitStep::CPosSessionInitStep() 
    {
    SetTestStepName(KPosSessionIntStep);
    }

/**
 * Destructor
 */
CPosSessionInitStep::~CPosSessionInitStep()
    {
    if (iReqWaiter)
        {
        delete iReqWaiter;
        }
    }


TVerdict CPosSessionInitStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    // Call base class method for pre test actions  
    CTe_SuplProxyPMSuiteStepBase::doTestStepPreambleL();
    
    // The Property is used the modify the behaviour of the test stub plugins for different
    // test cases
    TInt err;
    static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);    
    
    err = RProperty::Define(KSuplProxyPMSuite,KLbsPosSessionInitStepKey,RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
    if (err != KErrNone && err != KErrAlreadyExists)
        {
        User::Leave(err);
        }
    
    SetTestStepResult(EPass);
    return TestStepResult();
    }


/** Perform CPosSessionInitStep test step.
 * This test verifies various functions of the pos message api relating to a session
 *
 * @return TVerdict test result code
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
*/
TVerdict CPosSessionInitStep::doTestStepL()

    {
    _LIT(KTestMethod, "CPosSessionInitStep::doTestStepL()");
    INFO_PRINTF1(KTestMethod);
    
    TBool runStatus=EFalse;
    GetBoolFromConfig(ConfigSection(),KTe_SuiteBool,runStatus);
    
    if (!runStatus)
        {
        _LIT(KIniWarning, "Test run disabled in the ini file");
        ERR_PRINTF1(KIniWarning);
        SetTestStepResult(EFail);
        }
    else
        {
        if (TestStepResult()==EPass) 
            {
            TInt testIndex = KErrNotFound;
            if(!GetIntFromConfig(ConfigSection(),KTe_SuiteTestId,testIndex))
                {
                User::Leave(KErrNotFound);
                }
            else
                {
                _LIT(KTestName, "Call test by index");
                INFO_PRINTF1(KTestName);
                TInt err = KErrNone;
                TRAP(err, TestByIndexL(testIndex));
                if (err!= KErrNone)
                    {
                    _LIT(KTestLeft, "Test left with error %d");
                    ERR_PRINTF2(KTestLeft, err);
                    }
                }
            }
        }
    return TestStepResult();
    }


void CPosSessionInitStep::TestByIndexL(TInt aIndex)
{
    // set result of test to fail, then can reset if all's well
    SetTestStepResult(EFail);
    
    switch (aIndex)
        {
        case 11:
            _LIT(KTestName11, "POS Session init Test");
            INFO_PRINTF1(KTestName11);
            TestPosSessionInitL(aIndex);
            break;
        case 12:
            _LIT(KTestName13, "POS Session multi init Test");
            INFO_PRINTF1(KTestName13);
            TestMultiPosSessionInitL(aIndex);
            break;
        case 14:
            _LIT(KTestName15, "POS Session init cancel Test");
            INFO_PRINTF1(KTestName15);
            TestPosSessionInitCancelL(aIndex);
            break;
        case 16:            
            _LIT(KTestName16, "POS Session reinit Test");
            INFO_PRINTF1(KTestName16);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 17:
            _LIT(KTestName17, "Pos Session End with Requests Outstanding");
            INFO_PRINTF1(KTestName17);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 20:
            _LIT(KTestName20, "GetSuplInfoL invalid request");
            INFO_PRINTF1(KTestName20);        
            TestFollowsInitSuccessL(aIndex);
            break;            
        case 21:
            _LIT(KTestName21, "GetSuplInfoL without init");
            INFO_PRINTF1(KTestName21);        
            TestGetSuplInfoWithoutInitL(aIndex);
            break;
        case 22:
            _LIT(KTestName22, "GetSuplInfoL without reinit");
            INFO_PRINTF1(KTestName22);        
            TestFollowsInitSuccessL(aIndex);
            break;
        case 24:
            _LIT(KTestName24, "CancelSuplInfoRequest-Pending");
            INFO_PRINTF1(KTestName24);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 25:
            _LIT(KTestName25, "Cancel non-existant SuplInfoRequest");
            INFO_PRINTF1(KTestName25);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 28:
            _LIT(KTestName28, "CancelGetPosition-Pending");
            INFO_PRINTF1(KTestName28);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 29:
            _LIT(KTestName29, "Get position without Init");
            INFO_PRINTF1(KTestName29);
            TestGetPositionWithoutInitL(aIndex);
            break;
        case 30:    
            _LIT(KTestName30, "Get position without ReInit");
            INFO_PRINTF1(KTestName30);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 36:
            _LIT(KTestName36, "Cancel non-existant Get Position");
            INFO_PRINTF1(KTestName36);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 37:
            _LIT(KTestName37, "GetSuplInfoL - SET Capabilities (POS protocol version)");
            INFO_PRINTF1(KTestName37);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 38:
            _LIT(KTestName38, "Testing the COMASuplPosSessionBase::GetSuplState & SetSuplState methods");
            INFO_PRINTF1(KTestName38);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 39:
            _LIT(KTestName39, "Testing the COMASuplPosSessionBase::SuplObserver and MOMASuplObserver::x methods");
            INFO_PRINTF1(KTestName39);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 40:
            _LIT(KTestName40, "Testing the different classes used as arguments in GetSuplInfoL()");
            INFO_PRINTF1(KTestName40);
            TestSuplInfoAssDataArgClassesL();
            break;
        case 41:
            _LIT(KTestName41, "Testing the different classes related to assistance data that are used as arguments in GetSuplInfoL()");
            INFO_PRINTF1(KTestName41);
            TestSuplInfoGenericArgClassesL();
            break;
        case 42:
            _LIT(KTestName42, "Negative test - invalid values for the pos protocol version");
            INFO_PRINTF1(KTestName42);
            TestInvalidPosProtocolVersionL();
            break;
        case 43:
            _LIT(KTestName43, "Test to validate all possible pos methods and allowed capabilities");
            INFO_PRINTF1(KTestName43);
            TestFollowsInitSuccessL(aIndex);
            break;
        case 44:
            _LIT(KTestName44, "Testing the different classes related to position that are used as arguments in GetSuplInfoL()");
            INFO_PRINTF1(KTestName44);
            TestSuplInfoPositionArgClassesL();
            break;
        case 45:
            _LIT(KTestName45, "Testing the different classes related to Pos Payload that are used as arguments in GetSuplInfoL()");
            INFO_PRINTF1(KTestName45);
            TestSuplInfoPosPayloadArgClassesL();
            break;
        case 46:
            _LIT(KTestName46, "Negative test to investigate erroneous values returned from the Ref Loc plugin to the SPPM");
            INFO_PRINTF1(KTestName46);
            TestFollowsInitSuccessL(aIndex);
        default:
            _LIT(KIniFileErr, "Invalid TestId from the ini file");
            ERR_PRINTF1(KIniFileErr);
            break;            
        }
    }

void CPosSessionInitStep::TestPosSessionInitL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosSessionInitStep::TestPosSessionInitL");
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
             TRequestStatus reqStatus;
             iSuplPosSession->InitializeL(1,reqStatus);
             TInt posSessionErr = reqStatus.Int();
             if (posSessionErr==KErrNone)
                 {
                 // Verify if an existing subsession is being reused by just resetting the value
                 // of the request id.
                 iSuplPosSession->InitializeL(99,reqStatus);
                 TInt posSessionErr2 = reqStatus.Int();
                 if (posSessionErr2==KErrNone)
                     {
                     SetTestStepResult(EPass);
                     }
                }
            }
        }
    }

void CPosSessionInitStep::TestMultiPosSessionInitL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosSessionInitStep::TestMultiPosSessionInitL");
    INFO_PRINTF1(KTestName);
        
    TRequestStatus aStatus;
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(aStatus);
    User::WaitForRequest(aStatus);
 
    if (aStatus.Int() == KErrNone)
        {
        COMASuplPosSessionBase* suplPosSession1 = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(suplPosSession1);
        COMASuplPosSessionBase* suplPosSession2 = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(suplPosSession2);
        COMASuplPosSessionBase* suplPosSession3 = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(suplPosSession3);
        COMASuplPosSessionBase* suplPosSession4 = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(suplPosSession4);
        COMASuplPosSessionBase* suplPosSession5 = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(suplPosSession5);

        if ( suplPosSession1 && suplPosSession2 && suplPosSession3 &&
                suplPosSession4 && suplPosSession5 )
            {
            TRequestStatus reqStatus1,reqStatus2,reqStatus3,reqStatus4,reqStatus5;
            suplPosSession1->InitializeL(1,reqStatus1);
            suplPosSession2->InitializeL(2,reqStatus2);
            suplPosSession3->InitializeL(3,reqStatus3);
            suplPosSession4->InitializeL(4,reqStatus4);
            suplPosSession5->InitializeL(5,reqStatus5);
                        
            TInt posSessionErr1 = reqStatus1.Int();
            TInt posSessionErr2 = reqStatus2.Int();
            TInt posSessionErr3 = reqStatus3.Int();
            TInt posSessionErr4 = reqStatus4.Int();
            TInt posSessionErr5 = reqStatus5.Int();
                        
            if ((posSessionErr1==KErrNone) && (posSessionErr2==KErrNone) &&
                                (posSessionErr3==KErrNone) && (posSessionErr4==KErrNone) &&
                                (posSessionErr5==KErrNone))
               {
               _LIT(KInitStatus, "First five sessions initialized successfully");
               INFO_PRINTF1(KInitStatus); 
               
               COMASuplPosSessionBase* suplPosSession6 = iSuplPosHandler->CreateNewSessionL(this);
               CleanupStack::PushL(suplPosSession6);
               TRequestStatus reqStatus6;
               suplPosSession6->InitializeL(6,reqStatus6);
                if (reqStatus6.Int() == KErrGeneral)
                    {
                    _LIT(KSixthSessionInitStatus, "Sixth session initialization returned correct error code(KErrGeneral)");
                    INFO_PRINTF1(KSixthSessionInitStatus); 
                    SetTestStepResult(EPass);
                    }
                else
                    {
                    _LIT(KFailedInitialize, "Initialization of a sixth session returned (%d) against expected (KErrGeneral)");
                    ERR_PRINTF2(KFailedInitialize,reqStatus6.Int()); 
                    }
                CleanupStack::PopAndDestroy(suplPosSession6);  
                }
            else
                {
                _LIT(KFailedInitialize, "Could not Initialise up to five sessions");
                ERR_PRINTF1(KFailedInitialize);                            
                }
            }
        else
            {
            _LIT(KFailedSession, "Could not create up to five sessions with CreateNewSessionL");
            ERR_PRINTF1(KFailedSession);            
            }
        
        // Pop suplPosSession2-5, check we have suplPosSession1 next (then pop this also).
        CleanupStack::PopAndDestroy(4);          
        CleanupStack::Check(static_cast<TAny*>(suplPosSession1));
        CleanupStack::PopAndDestroy();
        }  
    }


void CPosSessionInitStep::TestPosSessionInitCancelL(TInt /*aIndex*/)
    {
    SetTestStepResult(EFail);
       
    _LIT(KTestName, "CPosSessionInitStep::TestPosSessionInitCancelL");
    INFO_PRINTF1(KTestName);
    
    TRequestStatus status;
    
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(status);
          
    User::WaitForRequest(status);
       
    TInt err = status.Int();
         
    if (err == KErrNone)
        {     
        iSuplPosSession = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(iSuplPosSession);          
        
        if (iSuplPosSession)
            {            
            TRequestStatus reqStatus;
            iSuplPosSession->InitializeL(1,reqStatus);
             
            iSuplPosSession->CancelInitialize();
            // Do it again for good measure!
            iSuplPosSession->CancelInitialize();

            TInt err = reqStatus.Int();
            if (err != KErrNone)
                {
                _LIT(KCancelInitError, "After cancel initialize expected status of KErrNone, but got %d");
                ERR_PRINTF2(KCancelInitError, err);
                }
            
            // Now try to initialize the session again with same id and check
            // that all goes through ok.
            iSuplPosSession->InitializeL(1, reqStatus);
             
            User::WaitForRequest(reqStatus);
                         
            err = reqStatus.Int();
            if (err == KErrNone)
                {
                // All ok, set test to pass
                SetTestStepResult(EPass);                
                }
            else
                {
                _LIT(KInitError, "Attempting Initialize after cancel returns %d");
                ERR_PRINTF2(KInitError, err);
                }
            }
        CleanupStack::PopAndDestroy();
        }   
    else
        {
            _LIT(KPosHandler, "Pos Handler Initialise returned %d");
            ERR_PRINTF2(KPosHandler, err);
        }
    }

void CPosSessionInitStep::PosSessionReinitL(void)
    {
    _LIT(KTestName, "CPosSessionInitStep::PosSessionReinitL");
    INFO_PRINTF1(KTestName);    
        
    iSuplPosSession->SessionEnd();
    TRequestStatus status;
    iSuplPosSession->InitializeL(1, status);             
    User::WaitForRequest(status);
    
    TInt err = status.Int();
    if (err == KErrNone)
        {
        SetTestStepResult(EPass);
        }
    else
        {
        _LIT(KInitError, "Attempting Initialize after prior session end returned error %d");
        ERR_PRINTF2(KInitError, err);
        }
    }
    

void CPosSessionInitStep::PosSessionEndwithRequestsL(void)
    {
    _LIT(KTestName, "CPosSessionInitStep::PosSessionEndwithRequestsL");
    INFO_PRINTF1(KTestName);
    
    //Create the active object to handle the GetSuplInfoL request
    iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
    iReqWaiter->StartTest(CAsyncReqWaiter::EGetSuplInfoSessionEnd);
    CActiveScheduler::Start();

    TInt err1 = (iReqWaiter->iStatus).Int();
    if (err1!=KErrCancel)
        {
        _LIT(KInitError, "Original GetSuplInfoL doesn't get cancelled when SessionEnd is rxd:expected KErrCancel / obtained %d");
        ERR_PRINTF2(KInitError, err1);
        }
    else
        {
        TRequestStatus status;
        // Now re-initialise and it goes through ok
        iSuplPosSession->InitializeL(1, status);
        User::WaitForRequest(status);

        TInt err2 = status.Int();
        if (err2 == KErrNone)
            {
            SetTestStepResult(EPass);
            }
        else
            {
            _LIT(KInitError, "Attempting Initialize after premature session end returned error %d");
            ERR_PRINTF2(KInitError, err2);
            }
        }
    
    delete iReqWaiter;
    iReqWaiter = NULL;
    }


void CPosSessionInitStep::GetSuplInfoWithoutReInitL(void)
    {
    _LIT(KTestName, "CPosSessionInitStep::GetSuplInfoWithoutReInitL");
    INFO_PRINTF1(KTestName);
            
    // End the previous session and don't re-init
    iSuplPosSession->SessionEnd();

    //Create the active object to handle the GetSuplInfoL request
    iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
    iReqWaiter->StartTest(CAsyncReqWaiter::EGetSuplInfo);
    CActiveScheduler::Start();
    
    // check that we get the expected error
    TInt err = (iReqWaiter->iStatus).Int();
    if (err == KErrOMASuplPosInActive)
        {
        SetTestStepResult(EPass);
        }
    else
        {
        _LIT(KExpectedOmaError, "ExpectedOMAError but got %d");                
        ERR_PRINTF2(KExpectedOmaError, err);
        }

    delete iReqWaiter;
    iReqWaiter = NULL;
    }

void CPosSessionInitStep::TestGetPositionWithoutInitL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosSessionInitStep::TestGetPositionWithoutInitL");
    INFO_PRINTF1(KTestName);
    
    TRequestStatus status;

    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(status);

    User::WaitForRequest(status);
       
    TInt err = status.Int();
         
    if (err == KErrNone)
        {     
        iSuplPosSession = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(iSuplPosSession);
                  
        if (iSuplPosSession )
            {            
            //Create the active object to handle the GetPositionL request
            iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
            iReqWaiter->StartTest(CAsyncReqWaiter::EGetPosition);
            CActiveScheduler::Start();
            
            err = (iReqWaiter->iStatus).Int();
            if (err != KErrOMASuplPosInActive)
                {
                _LIT(KExpectedOmaError, "ExpectedOMAError but got %d");                
                ERR_PRINTF2(KExpectedOmaError, err);
                }
            else
                {
                SetTestStepResult(EPass);
                }
            
            delete iReqWaiter;
            iReqWaiter = NULL;
            }
        CleanupStack::PopAndDestroy(); // removing iSuplPosSession
        }
    else
        {
        _LIT(KPosHandler, "Pos Handler Initialise returned %d");
        ERR_PRINTF2(KPosHandler, err);
        }    
    }


void CPosSessionInitStep::TestGetSuplInfoWithoutInitL(TInt /*aIndex*/)
    {
    _LIT(KTestName, "CPosSessionInitStep::TestGetSuplInfoWithoutInitL");
    INFO_PRINTF1(KTestName);
    
    TRequestStatus status;
    
    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(status);

    User::WaitForRequest(status);
       
    TInt err = status.Int();         
    if (err == KErrNone)
        {     
        iSuplPosSession = iSuplPosHandler->CreateNewSessionL(this);        
                  
        if (iSuplPosSession )
            {
            //Create the active object to handle the GetSuplInfoL request
            iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
            iReqWaiter->StartTest(CAsyncReqWaiter::EGetSuplInfo);
            CActiveScheduler::Start();
            
            err = (iReqWaiter->iStatus).Int();
            if (err != KErrOMASuplPosInActive)
                {
                _LIT(KExpectedOmaError, "ExpectedOMAError but got %d");                
                ERR_PRINTF2(KExpectedOmaError, err);
                }
            else
                {
                SetTestStepResult(EPass);
                }
            delete iReqWaiter;
            iReqWaiter = NULL;
            }
        }
    else
        {
        _LIT(KPosHandler, "Pos Handler Initialise returned %d");
        ERR_PRINTF2(KPosHandler, err);
        }
    }

void CPosSessionInitStep::TestFollowsInitSuccessL(TInt aIndex)
    {    

    TRequestStatus status;

    // Creates a session with the SUPL Proxy protocol module
    iSuplPosHandler->InitializeL(status);
    User::WaitForRequest(status);

    __UHEAP_MARK;
    // check if error
    TInt err = status.Int();
    if (err == KErrNone)
        {     
        iSuplPosSession = iSuplPosHandler->CreateNewSessionL(this);
        CleanupStack::PushL(iSuplPosSession);
                  
        if (iSuplPosSession )
            {            
            iSuplPosSession->InitializeL(1 ,status);
            User::WaitForRequest(status);
            err = status.Int();
            
            if (err == KErrNone)
                {
                if (aIndex == 16)
                    {
                    PosSessionReinitL();
                    }                
                else if (aIndex == 17)
                    {
                    PosSessionEndwithRequestsL();
                    }
                else if (aIndex == 20)
                    {
                    TestGetSuplInfoInvalidRequestL();
                    }
                else if (aIndex == 22)
                    {
                    GetSuplInfoWithoutReInitL();
                    }
                else if (aIndex == 24)
                    {
                    TestCancelGetSuplInfoL();
                    }
                else if (aIndex == 25)
                    {
                    TestCancelNonExistRequest();
                    }
                else if (aIndex == 28)
                    {
                    TestCancelGetPositionL();
                    }
                else if (aIndex == 30)
                    {
                    GetPositionWithoutReInitL();
                    }
                else if (aIndex == 36)
                    {
                    TestCancelNonExistGetPos();                    
                    }
                else if (aIndex == 37)
                    {
                    TestSuplInfoPosProtocolVersionL();
                    }
                else if (aIndex == 38)
                    {
                    TestSuplStateMethods();
                    }
                else if (aIndex == 39)
                    {
                    TestSuplObserverHandle();
                    }
                else if (aIndex == 43)
                    {
                    TestSetPosMethodAndAllowedCapabilities();
                    }
                else if (aIndex == 46)
                    {
                    TestRefLocPluginPositionError(aIndex);
                    }
                }
            else
                {
                _LIT(KPosSessionError, "Initialise of session returned error %d");
                ERR_PRINTF2(KPosSessionError, err);                
                }
            }
        else
            {
            _LIT(KNoPosSession, "No session created %d");
            ERR_PRINTF1(KNoPosSession);                
            }
        CleanupStack::PopAndDestroy();   // iSuplPosSession
        }
    else
        {
        _LIT(KPosHandler, "Pos Handler Initialise returned %d");
        ERR_PRINTF2(KPosHandler, err);
        }    
    __UHEAP_MARKEND;
    }

/*
 * Negative test to validate the behaviour of the SPPM in the case of incorrect arguments
 * passed in a GetSuplInfoL request.
 */
void CPosSessionInitStep::TestGetSuplInfoInvalidRequestL()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestGetSuplInfoInvalidRequestL");
    INFO_PRINTF1(KTestName);

    COMASuplInfoRequestList* reqList = COMASuplInfoRequestList::NewL();
    CleanupStack::PushL(reqList);

    COMASuplSETCapabilities* setCaps = NULL;
    reqList->Append(setCaps);

    TRequestStatus status;
    TRAPD(leaveErr,iSuplPosSession->GetSuplInfoL(reqList,status));
    if (leaveErr == KErrArgument)
        {
        SetTestStepResult(EPass);
        }
    else
        {
        _LIT(KSuplInfoError, "GetSuplInfoL() with an incorrect argument [null COMASuplSETCapabilities ptr]: Expected KErrArgument but got %d");
        ERR_PRINTF2(KSuplInfoError, leaveErr);
        }

    CleanupStack::PopAndDestroy(reqList);
    
    }


/*
 * Test to validate the processing of CancelSuplInfoRequest(). The original GetSuplInfoL()
 * request is expected to be cancelled retuning KErrCancel.
 */
 void CPosSessionInitStep::TestCancelGetSuplInfoL(void)
    {    
    //Create the active object to handle the GetSuplInfoL request
    iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
    iReqWaiter->StartTest(CAsyncReqWaiter::ECancelGetSuplInfo);
    CActiveScheduler::Start();
    
    //Send the cancel immediately
    TInt err = (iReqWaiter->iStatus).Int();
    //check if the original GetSuplInfoL request gets completed with KErrCancel
    if ( err == KErrCancel)
        {
        SetTestStepResult(EPass);
        }
    else
        {
        _LIT(KCancelSuplInfoError, "While cancelling GetSuplInfoL: Expected KErrCancel but got %d");
        ERR_PRINTF2(KCancelSuplInfoError, err);
        }

    delete iReqWaiter;
    iReqWaiter = NULL;
    }
    
 /*
  * Test to validate the processing of CancelGetPosition(). The original GetPositionL()
  * request is expected to be cancelled retuning KErrCancel.
  */
 void CPosSessionInitStep::TestCancelGetPositionL(void)
     {
     //Create the active object to handle the GetPositionL request
     iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
     iReqWaiter->StartTest(CAsyncReqWaiter::ECancelGetPosition);
     CActiveScheduler::Start();
     
     TInt err = (iReqWaiter->iStatus).Int();
     //check if the original GetSuplInfoL request gets completed with KErrCancel
     
     if (err == KErrCancel)
         {
         SetTestStepResult(EPass);         
         }
     else
         {
         _LIT(KNotCancel, "Expected KErrCancel but got %d");                
         ERR_PRINTF2(KNotCancel, err);
         }

     delete iReqWaiter;
     iReqWaiter = NULL;
     }
 
void CPosSessionInitStep::TestCancelNonExistRequest(void)
     {
     iSuplPosSession->CancelSuplInfoRequest();
     
     SetTestStepResult(EPass);
     }

void CPosSessionInitStep::TestCancelNonExistGetPos(void)
     {
     iSuplPosSession->CancelGetPosition();
     
     SetTestStepResult(EPass);
     }

void CPosSessionInitStep::GetPositionWithoutReInitL(void)
    {    
    // End the session without following Re-init.
    iSuplPosSession->SessionEnd();
    
    //Create the active object to handle the GetPositionL request
    iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
    iReqWaiter->StartTest(CAsyncReqWaiter::EGetPosition);
    CActiveScheduler::Start();
    
    // Check that we got the expected error.
    TInt err = (iReqWaiter->iStatus).Int();             
    if (err == KErrOMASuplPosInActive)
        {
        SetTestStepResult(EPass);
        }
    else
        {        
        _LIT(KOmaSuplPosError, "Expected OMA supl pos Error but got %d");                
        ERR_PRINTF2(KOmaSuplPosError, err);
        }
    
    delete iReqWaiter;
    iReqWaiter = NULL;

    }

/*Test to check the version of the underlying control plane positioning protocol.
 *
 *
 */
void CPosSessionInitStep::TestSuplInfoPosProtocolVersionL()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSuplInfoPosProtocolVersionL");
    INFO_PRINTF1(KTestName);

    //Create the active object to handle the GetPositionL request
    iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
    iReqWaiter->StartTest(CAsyncReqWaiter::ESuplInfoPosProtocolVersion);
    CActiveScheduler::Start();
    
    if (iReqWaiter->iStatus.Int() == KErrNone)
        {
        COMASuplSETCapabilities* rxdSETcaps = iReqWaiter->ReadReceivedSETCaps();
        //To validate the CloneL() method - Note: CloneL doesn't clone the protocolVersion.
        COMASuplSETCapabilities* localCopy;
        TRAPD(err,localCopy = static_cast<COMASuplSETCapabilities*>(rxdSETcaps->CloneL()));
        if (err != KErrNone)
            {
            _LIT(KCloneLError, "COMASuplSETCapabilities::CloneL() left with error - %d");                
            ERR_PRINTF2(KCloneLError, err);
            return;
            }
        delete localCopy;
        localCopy = NULL;
        //To verify the protocolVersion
        TOMASuplPosTechnology posTech;
        COMASuplSETCapabilities::TOMASuplPrefMethod  prefMethod;
        TOMASuplPosProtocol   posProtocol;
        TOMASuplPosProtocolVersion protocolVersion;
        rxdSETcaps->GetSETCapabilities(posTech,prefMethod,posProtocol,protocolVersion);
        TUint8 expMajorVersion=5,expTechVersion=12,expMinorVersion=0; 
        TUint8 rxdMajorVersion,rxdTechVersion,rxdMinorVersion;
        protocolVersion.GetPosProtocolVersion(rxdMajorVersion,rxdTechVersion,rxdMinorVersion);
        if ((expMajorVersion==rxdMajorVersion)&&
                (expTechVersion==rxdTechVersion)&&
                (expMinorVersion==rxdMinorVersion))
            {
            SetTestStepResult(EPass);            
            }
        else
            {
            _LIT(KPosProtocolVersionError, "Expected Protocol Version [5.12.0] but got [%d.%d.%d]");                
            ERR_PRINTF4(KPosProtocolVersionError, rxdMajorVersion,rxdTechVersion,rxdMinorVersion);
            }
        }
    delete iReqWaiter;
    iReqWaiter = NULL;
    }

/** Test case for testing the following methods of COMASuplPosSessionBase class
 * -GetSuplState(TOMASuplMsg& aSuplMsg,TOMASuplMsgState& aSuplMsgState) 
 * -SetSuplState(TOMASuplMsg aSuplMsg,TOMASuplMsgState aSuplMsgState)
 */ 
void CPosSessionInitStep::TestSuplStateMethods()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSuplStateMethods");
    INFO_PRINTF1(KTestName);

    // Validate the Set* and Get* SUPL STATE methods
    // For example, when SUPL Framework sends SUPL START to SLP, SUPL Message type
    // is set to EOMASuplStart and Message state is set to EOMASuplSent.
    COMASuplPosSessionBase::TOMASuplMsg suplMsg;
    COMASuplPosSessionBase::TOMASuplMsgState suplMsgState;
    iSuplPosSession->GetSuplState(suplMsg,suplMsgState);
    if ((suplMsg!=COMASuplPosSessionBase::EOMASuplStart)||
            (suplMsgState!=COMASuplPosSessionBase::EOMASuplGeneric))
        {
        _LIT(KSuplMsgError,"Initial SuplMsg state info in the POS message plugin is incorrect: Expected[SuplMsg %d,SuplMsgState %d] Received[SuplMsg %d,SuplMsgState %d]");
        ERR_PRINTF5(KSuplMsgError,COMASuplPosSessionBase::EOMASuplStart,COMASuplPosSessionBase::EOMASuplGeneric,suplMsg,suplMsgState);
        }
    iSuplPosSession->SetSuplState(COMASuplPosSessionBase::EOMASuplStart,COMASuplPosSessionBase::EOMASuplSent);
    COMASuplPosSessionBase::TOMASuplMsg rxdSuplMsg;
    COMASuplPosSessionBase::TOMASuplMsgState rxdSuplMsgState;
    iSuplPosSession->GetSuplState(rxdSuplMsg,rxdSuplMsgState);
    if ((COMASuplPosSessionBase::EOMASuplStart==rxdSuplMsg)&&(COMASuplPosSessionBase::EOMASuplSent==rxdSuplMsgState))
        {
        SetTestStepResult(EPass);       
        }
    else
        {
        _LIT(KSuplMsgError,"SuplMsg state info retained in the POS message plugin is incorrect: Expected[SuplMsg %d,SuplMsgState %d] Received[SuplMsg %d,SuplMsgState %d]");
        ERR_PRINTF5(KSuplMsgError,COMASuplPosSessionBase::EOMASuplStart,COMASuplPosSessionBase::EOMASuplSent,rxdSuplMsg,rxdSuplMsgState);
        }
    }

/** Test case for testing the following methods of COMASuplPosSessionBase class
 * -MOMASuplObserver* SuplObserver()
 */
void CPosSessionInitStep::TestSuplObserverHandle()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSuplObserverHandle");
    INFO_PRINTF1(KTestName);

    //Check The observer for the COMASuplPosHandlerBase implementation
    //This is used to retrieve SUPL Version and POS Message plugin can terminate SUPL Session
    //in SUPL Framework using this observer.
    MOMASuplObserver* suplFwHandle = iSuplPosSession->SuplObserver();
    if (suplFwHandle)
        {
        TReal version = suplFwHandle->SuplVersion();
        TReal expVersion = 1.0;
        if (version != expVersion)
            {
            _LIT(KSuplVersionError, "Supl Version is incorrect: Expected %f Rxd %f");
            ERR_PRINTF3(KSuplVersionError,expVersion,version);
            }
        else
            {
            SetTestStepResult(EPass);
            }
        }
    else
        {
        _LIT(KObserverValueError, "observer value set through COMASuplPosHandler::CreateNewSessionL(MOMASuplObserver* aSuplObserver) is NULL");
        ERR_PRINTF1(KObserverValueError);
        }
    }


/**Positive and Negative test case to validate the expected behaviour of Classes
 * related to [COMASuplReqAsstData] GetSuplInfoL argument.
 */
void CPosSessionInitStep::TestSuplInfoAssDataArgClassesL()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSuplInfoAssDataArgClassesL");
    INFO_PRINTF1(KTestName);
    
    COMASuplInfoRequestList* reqList = COMASuplInfoRequestList::NewL();
    CleanupStack::PushL(reqList);
    //NULL ptr returned when requesting an invalid entry
    COMASuplInfoRequest* suplInfoReq = reqList->GetElement(-1);
    if(!suplInfoReq)
        {
        COMASuplInfoRequest* suplInfoReq = reqList->GetElement(2);
        if(!suplInfoReq)
            {
            COMASuplReqAsstData* reqAssData = COMASuplReqAsstData::NewL();
            reqAssData->SetReqAsstData(EFalse, EFalse, EFalse, EFalse, EFalse,
                    ETrue/*aRefTime*/, EFalse, EFalse);
            TOMASuplNavigationModel expNaviModel,naviModel;
            //Range of values for the paramters
            //aGpsWeek  0 to 1023
            //aGpsToe   0 to 167
            //aNSAT     0 to 31
            //aToeLimit 0 to 10
            //Negative testing values outside the range
            TInt err1 = naviModel.SetNavigationModel(1024,168,32,11);
            TInt err2 = naviModel.SetNavigationModel(-1,-1,-1,-1);
            if((err1 == KErrArgument)&&((err2 == KErrArgument)))
                {
                const TInt Kgpsweek = 0,KgpsToe=1,KnSat=8,KtoeLimit=2;
                //Set dummy values to validate the behaviour of the class
                TInt setError = naviModel.SetNavigationModel(Kgpsweek,KgpsToe,KnSat,KtoeLimit);
                if (setError == KErrNone)
                    {
                    //Range of values for the paramters
                    //Satellite ID  0 to 63
                    //IODE          0 to 255
                    TOMASuplSatelliteInfoElement satInfoElement;
                    TInt err3 = satInfoElement.SetSatInfoElement(-1,-1);
                    TInt err4 = satInfoElement.SetSatInfoElement(64,256);
                    if((err3 == KErrArgument)&&((err4 == KErrArgument)))
                        {
                        const TInt KsatId = 1,Kiode=2;;
                        
                        TInt err5 = satInfoElement.SetSatInfoElement(KsatId,Kiode);
                        if (err5==KErrNone)
                            {
                            TInt satInfoSetError = naviModel.SetSatInfoElement(satInfoElement,0);
                            if (satInfoSetError==KErrNone)
                                {
                                reqAssData->SetNavigationData(naviModel);
                                reqList->Append(reqAssData);   
                                
                                COMASuplReqAsstData* assDataCopy = static_cast<COMASuplReqAsstData*>(reqAssData->CloneL());
                                TInt gpsWeekCopy,gpsToeCopy,nSatCopy,toeLimitCopy;
                                TOMASuplNavigationModel naviModelCopy;
                                assDataCopy->GetNavigationData(naviModelCopy);
                                naviModelCopy.GetNavigationModel(gpsWeekCopy,gpsToeCopy,nSatCopy,toeLimitCopy);
                                if ((gpsWeekCopy==Kgpsweek)&&(gpsToeCopy==KgpsToe)&&
                                        (nSatCopy==KnSat)&&(toeLimitCopy==KtoeLimit))
                                    {
                                    TOMASuplSatelliteInfoElement satInfoElementCopy,satInfoElementExtraCopy;
                                    naviModelCopy.GetSatInfoElement(satInfoElementCopy,0);
                                    TInt satId,iode,satIdExtra,iodeExtra;
                                    satInfoElementCopy.GetSatInfoElement(satId,iode);
                                    RArray<TOMASuplSatelliteInfoElement> satInfoArray;
                                    naviModelCopy.GetSatInfoElement(satInfoArray);
                                    satInfoElementExtraCopy = satInfoArray[0];
                                    satInfoElementExtraCopy.GetSatInfoElement(satIdExtra,iodeExtra);
                                    satInfoArray.Close();
                                    
                                    if((satId==KsatId)&&(iode==Kiode)&&(satIdExtra==KsatId)&&(iodeExtra==Kiode))
                                        {
                                        SetTestStepResult(EPass);
                                        }
                                    else
                                        {
                                        _LIT(KSatInfoError, "SatID and IODE [TOMASuplSatelliteInfoElement] not retained on a copy obtained using CloneL() ");
                                        ERR_PRINTF1(KSatInfoError);
                                        }
                                    }
                                else
                                    {
                                    _LIT(KNaviModelError, "Navigational model values set through SetNavigationData not retained on a copy obtained using CloneL() ");
                                    ERR_PRINTF1(KNaviModelError);
                                    }//if ((gpsWeekCopy==0)&&(g...
                                delete assDataCopy;
                                }//(satInfoSetError==KErrNone)
                            else
                                {
                                _LIT(KErrorSetSatInfo, "Setting TOMASuplSatelliteInfoElement in TOMASuplNavigationModel returned the error %d");
                                ERR_PRINTF2(KErrorSetSatInfo,satInfoSetError);
                                }
                            }//if (err5==KErrNone)
                        else
                            {
                            _LIT(KErrorSetSatInfo, "Setting SatID and IODE [TOMASuplSatelliteInfoElement] returned the error %d");
                            ERR_PRINTF2(KErrorSetSatInfo,err5);
                            }
                        }// if((err3 == KErrArgument)&&((err4 == KErrArgument)))
                    else
                        {
                        _LIT(KErrorSatInfoOutsideRange, "Negative test - Setting invalid SatInfoElement parameters didn't return the expected error: KErrArgument");
                        ERR_PRINTF1(KErrorSatInfoOutsideRange);
                        }
                    }//if (setError == KErrNone)
                else
                    {
                    _LIT(KErrorSetNaviModel, "Setting NavigationModel parameters returned the error %d");
                    ERR_PRINTF2(KErrorSetNaviModel,setError);
                    }
                }//if((err1 == KErrArgument)&&((err2 == KErrArgument)))
            else
                {
                _LIT(KErrorOutsideRange, "Negative test - Setting invalid NavigationModel parameters didn't return the expected error: KErrArgument");
                ERR_PRINTF1(KErrorOutsideRange);
                }
            }// if(!suplInfoReq)
        else
            {
            _LIT(KErrorInvalidElement1, "Negative test -accessing an invalid COMASuplInfoRequestList returned a non-zero ptr");
            ERR_PRINTF1(KErrorInvalidElement1);
            }
        }// if(!suplInfoReq)
    else
        {
        _LIT(KErrorInvalidElement2, "Negative test -accessing an invalid COMASuplInfoRequestList returned a non-zero ptr");
        ERR_PRINTF1(KErrorInvalidElement2);
        }
    CleanupStack::PopAndDestroy(reqList);
    }

/**Positive and Negative test case to validate the expected behaviour of Classes
 * related to [COMASuplInfoRequest] GetSuplInfoL argument. 
 */
void CPosSessionInitStep::TestSuplInfoGenericArgClassesL()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSuplInfoGenericArgClassesL");
    INFO_PRINTF1(KTestName);
    COMASuplInfoRequestList* reqList = COMASuplInfoRequestList::NewL();
    CleanupStack::PushL(reqList);
    COMASuplInfoRequest* suplInfoReq = COMASuplInfoRequest::NewL();
    reqList->Append(suplInfoReq);
    suplInfoReq->SetStatus(KErrNone);
    suplInfoReq->SetType(COMASuplInfoRequest::EOMASuplSETCapabilities);
    COMASuplInfoRequest* suplInfoReqCopy = suplInfoReq->CloneL();
    reqList->Append(suplInfoReqCopy);
    if((suplInfoReqCopy->Status()==KErrNone)&&
            (suplInfoReqCopy->Type()==COMASuplInfoRequest::EOMASuplSETCapabilities))
        {
        SetTestStepResult(EPass);        
        }
    else
        {
        _LIT(KCloneError, "CloneL didn't retain the type and status info");
        ERR_PRINTF1(KCloneError);
        }
    CleanupStack::PopAndDestroy(reqList);
    }

/**
 * Negative test - invalid values for the pos protocol version
 */
void CPosSessionInitStep::TestInvalidPosProtocolVersionL()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestInvalidPosProtocolVersionL");
    INFO_PRINTF1(KTestName);
    TOMASuplPosProtocolVersion protocolVersion;
      //Set all the values to NULL
        TInt setVersionErr2 = protocolVersion.SetPosProtocolVersion(0,0,0);
        if (setVersionErr2==KErrNone)
            {
            TUint8 majorVer,techVer,minVer;
            TInt getVersionErr = protocolVersion.GetPosProtocolVersion(majorVer,techVer,minVer);
            if (getVersionErr==KErrNotFound)
                {
                SetTestStepResult(EPass);
                }
            else
                {
                _LIT(KErrorNullVer, "Didn't return the expected error code[KErrNotFound] when all the version numbers are null");
                ERR_PRINTF1(KErrorNullVer);
                }
            }
    }


/**
 * Test case to validate the Get/SetAllowedCapabilities and SetPosMethodAndAllowedCapabilities[preferred methods]
 */
void CPosSessionInitStep::TestSetPosMethodAndAllowedCapabilities()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSetPosMethodAndAllowedCapabilities");
    INFO_PRINTF1(KTestName);
    TOMASuplAllowedCapabilities allowedCaps;//ignored in the pos message plugin
    allowedCaps.SetAllowedCapabilities(ETrue,EFalse,EFalse,EFalse,EFalse,EFalse,EFalse,EFalse);
    TBool  aGpsSETAssisted,aGpsSETBased,aAutonomousGps,aFLT,aeOTD,aoTDOA,aeCID,aCID;
    allowedCaps.GetAllowedCapabilities(aGpsSETAssisted,aGpsSETBased,aAutonomousGps,aFLT,aeOTD,aoTDOA,aeCID,aCID);
    if (aGpsSETAssisted&&(!aGpsSETBased)&&(!aAutonomousGps)&&(!aFLT)&&(!aeOTD)&&(!aoTDOA)&&(!aeCID)&&(!aCID))
        {
        //Try to set the different possible pos methods  - 
        //Note: [this tests some pos methods that are not valid wrt to SUPL FW] and hence are not covered by integ tests
        iSuplPosSession->SetPosMethodAndAllowedCapabilities(allowedCaps,COMASuplPosSessionBase::EGpsSETBasedPreferred);
        iSuplPosSession->SetPosMethodAndAllowedCapabilities(allowedCaps,COMASuplPosSessionBase::EGpsSETAssistedPreferred);
        SetTestStepResult(EPass);
        }
    }

/**Positive and Negative test case to validate the expected behaviour of Classes
 * related to [COMASuplPosition] GetSuplInfoL argument.
 */
void CPosSessionInitStep::TestSuplInfoPositionArgClassesL()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSuplInfoPositionArgClassesL");
    INFO_PRINTF1(KTestName);
    
    COMASuplInfoRequestList* reqList = COMASuplInfoRequestList::NewL();
    CleanupStack::PushL(reqList);
    COMASuplPosition* position = COMASuplPosition::NewL();
    CleanupStack::PushL(position);
    
    TOMASuplUtcTime utcTime;
    TDateTime dateTime;
    const TInt Kyear = 2002,Kday = 1,Khour = 1,Kmin = 1,Ksec = 1,Kmsec = 0;
    const TMonth Kmonth(ENovember);
    TInt dateSetErr = dateTime.Set(Kyear,Kmonth,Kday,Khour,Kmin,Ksec,Kmsec);
    if (dateSetErr == KErrNone)
        {
        //valid values for zone code - 0,1
        TInt err1 = utcTime.SetUtcTime(dateTime,3,0);  //beyond range limt
        TInt err2 = utcTime.SetUtcTime(dateTime,-1,0); //below range limit
        if ((err1==KErrArgument)&&((err1==KErrArgument)))
            {
            //set a valid entry
            const TInt Kzonecode = 0;
            const TInt Kzone = 0;
            TInt timeSetErr = utcTime.SetUtcTime(dateTime,Kzonecode,Kzone);
            if (timeSetErr==KErrNone)
                {
                TOMASuplPositionEstimate posEstimate;
                //Confidence - valid range 0 to 100 (percentage)
                TInt err3 = posEstimate.SetConfidence(101);
                TInt err4 = posEstimate.SetConfidence(-1);
                if ((err3 == KErrArgument)&&(err4 == KErrArgument))
                    {
                    const TInt KConfidence = 30; 
                    TInt setConfidenceErr = posEstimate.SetConfidence(KConfidence);
                    if (setConfidenceErr==KErrNone)
                        {
                        position->SetPosition(utcTime,posEstimate);
                        COMASuplVelocity* suplVelocity = COMASuplVelocity::NewL();
                        position->SetSuplVelocity(suplVelocity);
                        CleanupStack::Pop(position);
                        reqList->Append(position); //reqList takes ownership
                            
                        COMASuplPosition* positionCopy = static_cast<COMASuplPosition*>(position->CloneL());
                        TInt confidenceCopy,zoneCodeCopy,zoneCopy;
                        TOMASuplPositionEstimate posEstimateCopy;
                        TOMASuplUtcTime utcTimeCopy;
                        TDateTime dateTimeCopy;
                        positionCopy->GetPosition(utcTimeCopy,posEstimateCopy);
                        delete positionCopy;
                            
                        TInt readConfidenceCopyErr = posEstimateCopy.GetConfidence(confidenceCopy);
                        utcTimeCopy.GetUtcTime(dateTimeCopy,zoneCodeCopy,zoneCopy);
                        TBool verifyTime = ((dateTimeCopy.Year()==Kyear)&&
                                    (dateTimeCopy.Month()==Kmonth)&&(dateTimeCopy.Day()==Kday)&&(dateTimeCopy.Hour()==Khour)&&(dateTimeCopy.Minute()==Kmin)&&
                                    (dateTimeCopy.Second()==Ksec)&&(dateTimeCopy.MicroSecond()==Kmsec));
                        if((confidenceCopy==KConfidence)&&(verifyTime)&&(zoneCodeCopy==Kzonecode)&&(zoneCopy==Kzone))
                            {
                            SetTestStepResult(EPass);
                            }
                        else
                            {
                             _LIT(KCloneError, "TOMASuplPositionEstimate/TOMASuplUtcTime info not retained on a copy obtained using CloneL() ");
                             ERR_PRINTF1(KCloneError);
                            }
                       }
                   else
                       {
                       _LIT(KSetConfidenceErr, "SetConfidence returned error code %d ");
                       ERR_PRINTF2(KSetConfidenceErr,setConfidenceErr);
                       }
                    }
                 else
                    {
                     _LIT(KIncorrectConfSetError, "Setting incorrect confidence value didn't return the expected error  code - KErrArgument");
                     ERR_PRINTF1(KIncorrectConfSetError);
                    }
                  }
            else
                {
                _LIT(KTimeSetError, "Setting UTC time returned the error  code - %d");
                ERR_PRINTF2(KTimeSetError,timeSetErr);
                }
            }
        else
            {
            _LIT(KIncorrectTimeSetError, "Setting incorrect utc time didn't return the expected error  code - KErrArgument");
            ERR_PRINTF1(KIncorrectTimeSetError);
            }
        }
    else
        {
        _LIT(KIncorrectDateTimeSetError, "Setting date and time returned the error  code - %d");
        ERR_PRINTF2(KIncorrectDateTimeSetError,dateSetErr);
        }
    CleanupStack::PopAndDestroy(reqList);    
    }


/**Test case to validate the expected behaviour of Classes
 * related to PosPayload argument passed in a GetSuplInfoL.
 */
void CPosSessionInitStep::TestSuplInfoPosPayloadArgClassesL()
    {
    _LIT(KTestName, "CPosSessionInitStep::TestSuplInfoPosPayloadArgClassesL");
    INFO_PRINTF1(KTestName);
    COMASuplPosPayload* posPayload = COMASuplPosPayload::NewL();
    CleanupStack::PushL(posPayload);
    posPayload->SetSuplPayloadType(COMASuplPosPayload::ERRLP);
    COMASuplPosPayload* posPayloadCopy = static_cast<COMASuplPosPayload*>(posPayload->CloneL());
    CleanupStack::PushL(posPayloadCopy);
    if (posPayloadCopy)
        {
        if (posPayloadCopy->SuplPayloadType() == COMASuplPosPayload::ERRLP)
            {
            SetTestStepResult(EPass);
            }
        else
            {
            _LIT(KTypeInfoError, "The PosPayload type info not retained after CloneL() ");
            ERR_PRINTF1(KTypeInfoError);
            }
        }
    else
        {
        _LIT(KCloneError, "The PosPayload CloneL() funtion didn't return a valid pointer");
        ERR_PRINTF1(KCloneError);
        }
    CleanupStack::PopAndDestroy(2,posPayload);
    }


/*
 * Negative test to validate the processing of error in the case when the ref loc plugin
 * returns just an error without a position to the SPPM.
 */
void CPosSessionInitStep::TestRefLocPluginPositionError(TInt aIndex)
    {
    //Using RProperty to inform the ref loc plugin to modify its behaviour for 
    //different test cases
    SetTestCasePropertyL(KSuplProxyPMSuite,KLbsPosSessionInitStepKey,aIndex);
    
    //Create the active object to handle the GetPositionL request
    iReqWaiter = CAsyncReqWaiter::NewL(iSuplPosSession);
    iReqWaiter->StartTest(CAsyncReqWaiter::ERefLocPluginPositionError);
    CActiveScheduler::Start();
    
    TInt err = (iReqWaiter->iStatus).Int();
    //check if the original GetSuplInfoL request gets completed with KErrCancel
    
    if (err == KErrNone)
        {
        COMASuplInfoRequestList* rxdReqList = iReqWaiter->ReadReceivedSuplInfoReqList();//ownership held in the AO.
        TInt count = rxdReqList->GetCount();
        TInt index;

        for ( index=0;index<count;index++)
            {
            COMASuplInfoRequest::TOMASuplInfoType infoReqType = (rxdReqList->GetElement(index))->Type();
            if (infoReqType==COMASuplInfoRequest::EOMASuplPosition)
                {
                TInt rxdPosErr = (rxdReqList->GetElement(index))->Status();
                if (rxdPosErr==KErrOMASuplPosInfo)
                    {
                    SetTestStepResult(EPass);
                    }
                else
                    {
                    _LIT(KCompletionErr, "This negative test case expected error KErrOMASuplPosInfo but got %d");                
                    ERR_PRINTF2(KCompletionErr, rxdPosErr);
                    }
                break;
                }
            }
        }
    else
        {
        _LIT(KCompletionErr, "AO handling the GetSuplInfoL was completed with error - %d");                
        ERR_PRINTF2(KCompletionErr, err);
        }

    delete iReqWaiter;
    iReqWaiter = NULL;
    }

TVerdict CPosSessionInitStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    // Delete the keys property used to configure the test location monitor
    // to respond to different test cases
    RProperty property;
    TInt err = property.Delete(KSuplProxyPMSuite, KLbsPosSessionInitStepKey);
    if (err != KErrNone)
        {
        User::Leave(err);
        }
    
    // Call base class method for post test actions 
    CTe_SuplProxyPMSuiteStepBase::doTestStepPostambleL();
    
    return TestStepResult();
    }


