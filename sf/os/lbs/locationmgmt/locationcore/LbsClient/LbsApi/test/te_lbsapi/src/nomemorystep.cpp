// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file NoMemoryStep.cpp
 @internalTechnology
*/
#include "nomemorystep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <s32mem.h>

CNoMemoryStep::~CNoMemoryStep()
/**
 * Destructor
 */
	{
	}

CNoMemoryStep::CNoMemoryStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNoMemoryStep);
	}

TVerdict CNoMemoryStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	 CTe_LbsApiSuiteStepBase::doTestStepPreambleL();
	 if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CNoMemoryStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		TInt test;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsApiSuiteInt,test))
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	return TestStepResult();
	}



TVerdict CNoMemoryStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_LbsApiSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

void CNoMemoryStep::StartL (TInt aIndex)
	{
	switch (aIndex)
		{
	case 0:
		OOM_RPositionServer_Open_NoMemoryL ();
		break;
	case 1:
		OOM_RPositioner_OpenL ();
		break;
	case 2:
		OOM_RPositioner_SetRequestorL ();
		break;
	case 3:
		OOM_CRequestor_ConstructionL ();
		break;
	case 4:
		OOM_CRequestor_SetRequestorL ();
		break;
	case 5:
		OOM_CRequestor_StreamingL ();
		break;
	case 6:
		OOM_RRequestorStack_StreamingL ();
		break;
	case 7:
		OOM_HPositionGenericInfo_ConstructionL ();
		break;
	default:
		break;
		}
	}


#define DO_OOM_CUSTOMISED_LOOP(aType, aCreationCode, aDeletionCode, aLoopStartCode, aLoopEndCode)  \
{                                                                            \
    aType;                                                                   \
    TInt failureRate;                                                        \
    TBool doContinue = ETrue;                                                \
    for (failureRate = 1; doContinue; failureRate++)                         \
        {                                                                    \
        aLoopStartCode;                                                      \
        __UHEAP_SETFAIL(RHeap::EFailNext, failureRate);                      \
        __UHEAP_MARK;                                                        \
                                                                             \
        TInt err;                                                            \
        TRAP(err, aCreationCode);                                            \
        if (err != KErrNoMemory)                                             \
            {                                                                \
            aDeletionCode;                                                   \
            doContinue = EFalse;                                             \
            }                                                                \
                                                                             \
        __UHEAP_MARKEND;                                                     \
        __UHEAP_RESET;                                                       \
        aLoopEndCode;                                                        \
        }                                                                    \
    CHECK( failureRate > 2 );                                                \
    message.Format(_L("Checked %d allocations in \"%s\""), failureRate-2, L## #aCreationCode); \
    LOG_DES(message); \
}


#define DO_OOM(aType, aCreationCode, aDeletionCode) \
    DO_OOM_CUSTOMISED_LOOP(aType, aCreationCode, aDeletionCode, (void) 0, (void) 0)


//
//
// RPositionServer
//
//

//1.1.6 Connect with No Memory
void CNoMemoryStep::OOM_RPositionServer_Open_NoMemoryL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    RPositionServer server;

    // connect
    __UHEAP_SETFAIL(RHeap::EFailNext, 1);
    __UHEAP_MARK;
    CHECK_EQUAL_(KErrNoMemory, server.Connect());
    __UHEAP_MARKEND;
    __UHEAP_RESET;

    StandardCleanup();
	}


//
//
// RPositioner
//
//

//1.10.7 Open with No Memory
void CNoMemoryStep::OOM_RPositioner_OpenL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;

    // open by default
    __UHEAP_SETFAIL(RHeap::EFailNext, 1);
    __UHEAP_MARK;
    CHECK_EQUAL_(KErrNoMemory, positioner.Open(server));
    __UHEAP_MARKEND;
    __UHEAP_RESET;

    // by module id
    TPositionModuleId moduleId;
    server.GetDefaultModuleId(moduleId);
    __UHEAP_SETFAIL(RHeap::EFailNext, 1);
    __UHEAP_MARK;
    CHECK_EQUAL_(KErrNoMemory, positioner.Open(server, moduleId));
    __UHEAP_MARKEND;
    __UHEAP_RESET;

    // by criteria
    TPositionCriteria criteria;
    criteria.ResetCriteria(); // defaults
    __UHEAP_SETFAIL(RHeap::EFailNext, 1);
    __UHEAP_MARK;
    CHECK_EQUAL_(KErrNoMemory, positioner.Open(server, criteria));
    __UHEAP_MARKEND;
    __UHEAP_RESET;

    CleanupStack::PopAndDestroy();

    StandardCleanup();
	}


//1.13.5 SetRequestor Stack with No Memory
void CNoMemoryStep::OOM_RPositioner_SetRequestorL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    RPositionServer server;
    User::LeaveIfError(server.Connect());
    CleanupClosePushL(server);

    RPositioner positioner;
    CHECK_EQUAL_(KErrNone, positioner.Open(server));
    CleanupClosePushL(positioner);

    RRequestorStack stack;
    CRequestor* requestor = NULL;

    // All Accepted
    requestor = CRequestor::NewLC(
            CRequestor::ERequestorContact,
            DUMMY_REQUESTOR_FORMAT_ACCEPTED2,
            DUMMY_REQUESTOR_NAME_ACCEPTED2);
    stack.Append(requestor);
    requestor = CRequestor::NewLC(
            CRequestor::ERequestorService,
            DUMMY_REQUESTOR_FORMAT_ACCEPTED1,
            DUMMY_REQUESTOR_NAME_ACCEPTED1);
    stack.Append(requestor);

    // set requestor stack
    __UHEAP_SETFAIL(RHeap::EFailNext, 1);
    __UHEAP_MARK;

    CHECK_EQUAL_(KErrNoMemory, positioner.SetRequestor(stack));
    CHECK_EQUAL_(KErrNone, positioner.SetRequestor(stack));

    __UHEAP_MARKEND;
    __UHEAP_RESET;

    CleanupStack::Pop(2); // requestors
    CleanupStack::PopAndDestroy(2); // server & positioner
    stack.ResetAndDestroy();

    StandardCleanup();
    }


//
//
// CRequestor
//
//

// 8.2.4. Constructors with No Memory
void CNoMemoryStep::OOM_CRequestor_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    _LIT(KRequestorData, "+441223719800");

    // New
    CRequestor* requestor = NULL;
    TBool doContinue = ETrue;
    TInt failureRate = 0;

    for (failureRate = 1; doContinue; failureRate++)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failureRate);
        __UHEAP_MARK;

        TInt err;
        TRAP(err, 
            User::LeaveIfNull(requestor = 
                CRequestor::New(CRequestor::ERequestorContact, 
                                CRequestor::EFormatTelephone, 
                                KRequestorData));
        );
        
        if (err != KErrNoMemory)
            {
            delete requestor;
            doContinue = EFalse;
            }

        __UHEAP_MARKEND;
        __UHEAP_RESET;
        }
    CHECK( failureRate > 2 );
    message.Format(_L("Checked %d allocations in \"New\""), failureRate-2);
    LOG_DES(message);

    // NewL
    doContinue = ETrue;
    for (failureRate = 1; doContinue; failureRate++)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failureRate);
        __UHEAP_MARK;

        TInt err;
        TRAP(err, 
            requestor = CRequestor::NewL(CRequestor::ERequestorContact, 
                                          CRequestor::EFormatTelephone, 
                                          KRequestorData);
        	);
        
        if (err != KErrNoMemory)
            {
            delete requestor;
            doContinue = EFalse;
            }

        __UHEAP_MARKEND;
        __UHEAP_RESET;
        }
    CHECK( failureRate > 2 );
    message.Format(_L("Checked %d allocations in \"NewL\""), failureRate-2);
    LOG_DES(message);

    // NewLC
    doContinue = ETrue;
    for (failureRate = 1; doContinue; failureRate++)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failureRate);
        __UHEAP_MARK;

        TInt err;
        TRAP(err, 
            requestor = CRequestor::NewLC(CRequestor::ERequestorContact, 
                                          CRequestor::EFormatTelephone, 
                                          KRequestorData);
            CleanupStack::Pop(requestor);                                          
        	);
        
        if (err != KErrNoMemory)
            {
            delete requestor;
            doContinue = EFalse;
            }

        __UHEAP_MARKEND;
        __UHEAP_RESET;
        }
    CHECK( failureRate > 2 );
    message.Format(_L("Checked %d allocations in \"NewLC\""), failureRate-2);
    LOG_DES(message);

    StandardCleanup();   
	}


// 8.2.5. SetRequestor with No Memory
void CNoMemoryStep::OOM_CRequestor_SetRequestorL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    _LIT(KRequestorData, "+441223719800");
    _LIT(KRequestorData2, "+44122371980101");

    // NOTE, this is a special case where the CRequestor is created at the start, then
    // its member data is replaced (which is tested under OOM conditions).
    DO_OOM(
          CRequestor* requestor = CRequestor::NewLC(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData),
          requestor->SetRequestorL(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData2),
          CleanupStack::PopAndDestroy(requestor)
          );

    StandardCleanup();    
	}


// 8.3.3. Streaming with NoMemory
void CNoMemoryStep::OOM_CRequestor_StreamingL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    _LIT(KRequestorData, "+441223719800");
    CRequestor* requestor = NULL;

    requestor = CRequestor::NewLC(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData);
    TBuf8<100> buffer;
    RDesWriteStream writeStrm(buffer);
    CleanupClosePushL(writeStrm);
    TInt err;
    TRAP(err, writeStrm << *requestor);
    CleanupStack::PopAndDestroy(&writeStrm);

    // Internalize
    DO_OOM_CUSTOMISED_LOOP
        (
        (void) 0,
        strm >> *requestor,
        delete requestor,
        RDesReadStream strm(buffer); CleanupClosePushL(strm),
        CleanupStack::PopAndDestroy(&strm)
        );

    CleanupStack::Pop(requestor);

    // Construction
    DO_OOM_CUSTOMISED_LOOP
        (
        (void) 0,
        requestor = CRequestor::NewL(strm),
        delete requestor,
        RDesReadStream strm(buffer); CleanupClosePushL(strm),
        CleanupStack::PopAndDestroy(&strm)
        );

    StandardCleanup();
    }


//
//
// RRequestorStack
//
//

void CleanupResetAndDestroy(TAny* aArray)
    {
    reinterpret_cast<RRequestorStack*>(aArray)->ResetAndDestroy();
    }


// 8.1.3. Streaming with No Memory
void CNoMemoryStep::OOM_RRequestorStack_StreamingL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    _LIT(KRequestorData, "+441223719800");
    const TInt KNumberOfRequestors = 3;
    RRequestorStack stack;
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &stack));
    TInt i;
    for (i = 0; i < KNumberOfRequestors; i++)
            User::LeaveIfError(stack.Append(NULL));
    for (i = 0; i < KNumberOfRequestors; i++)
            stack[i] = CRequestor::NewL(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, KRequestorData);

    TBuf8<100> buffer;
    RDesWriteStream writeStrm(buffer);
    CleanupClosePushL(writeStrm);
    TInt err;
    TRAP(err, writeStrm << stack);
    CleanupStack::PopAndDestroy(&writeStrm);

    CleanupStack::PopAndDestroy(&stack);
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &stack));

    DO_OOM_CUSTOMISED_LOOP
        (
        (void) 0,
        strm >> stack,
        stack.ResetAndDestroy(),
        RDesReadStream strm(buffer); CleanupClosePushL(strm),
        CleanupStack::PopAndDestroy(&strm)
        );

    CleanupStack::Pop(&stack);

    StandardCleanup();
    }


//
//
// HPositionGenericInfo
//
//

// 3.2.2. NoMemory Construction
void CNoMemoryStep::OOM_HPositionGenericInfo_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    // New
    DO_OOM
            (
            HPositionGenericInfo* genInfo = NULL,
            User::LeaveIfNull(genInfo = HPositionGenericInfo::New(10)),
            delete genInfo
            );
    // NewL
    DO_OOM
            (
            HPositionGenericInfo* genInfo = NULL,
            genInfo = HPositionGenericInfo::NewL(10),
            delete genInfo
            );
    // NewLC
    DO_OOM
            (
            HPositionGenericInfo* genInfo = NULL,
            genInfo = HPositionGenericInfo::NewLC(10);
            CleanupStack::Pop(genInfo),
            delete genInfo
            );

    StandardCleanup();
    }

