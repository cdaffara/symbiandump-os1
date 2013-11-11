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
 @file NoServerStep.cpp
 @internalTechnology
*/
#include "noserverstep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbssatellite.h>
#include <e32math.h>
#include <s32mem.h>
#include <bautils.h>

CNoServerStep::~CNoServerStep()
/**
 * Destructor
 */
	{
	}

CNoServerStep::CNoServerStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNoServerStep);
	}

TVerdict CNoServerStep::doTestStepPreambleL()
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


TVerdict CNoServerStep::doTestStepL()
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
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsApiSuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	  return TestStepResult();
	}



TVerdict CNoServerStep::doTestStepPostambleL()
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


void CNoServerStep::StartL (TInt aIndex)
	{
	switch (aIndex)
		{
	case 0:
		RPositionServer_NoConnectionL ();
		break;
	case 1:
		RPositionServer_NoL ();
		break;
	case 2:
		RPositioner_NoL ();
		break;
	default:
		break;
		}
	}

void CNoServerStep::CheckThereIsNoServerL()
    {
    DECLARE_ERROR_LOGGING;
    
    RPositionServer server;
    CleanupClosePushL(server);
    FAIL(server.Connect() == KErrNone, 
        "Location Server must be deleted to run this test case");
        
    CleanupStack::PopAndDestroy(&server);
    }


// 1.1.2 Connect but server doesn't start
// The test must be started when there is no server
// See LBSAPIACCTESTNOSVR.BAT for details
void CNoServerStep::RPositionServer_NoConnectionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    CheckThereIsNoServerL();
    
    RPositionServer server;

    __UHEAP_MARK;
    CHECK( 0 == server.Handle() );
    CHECK( KErrNone != server.Connect() );
    CHECK( server.Handle() == 0 );
    __UHEAP_MARKEND;

    StandardCleanup();
	}

// x.x.x Connect and try to execute RPositionServer methods
// This should fail with panic

TInt No_Server_GetDefaultModuleId(TAny* /*aPtr*/)
    {
    RPositionServer server;
    TPositionModuleId moduleId = KPositionNullModuleId;
    server.GetDefaultModuleId(moduleId);
    return KErrGeneral;
    }


TInt No_Server_GetNumModules(TAny* /*aPtr*/)
    {
    RPositionServer server;
    TUint numModules;
    server.GetNumModules(numModules);
    return KErrGeneral;
    }


TInt No_Server_GetModuleInfoByIndex(TAny* /*aPtr*/)
    {
    RPositionServer server;
    TUint index = 0;
    TPositionModuleInfo info;
    server.GetModuleInfoByIndex(index, info);
    return KErrGeneral;
    }


TInt No_Server_GetModuleInfoById(TAny* /*aPtr*/)
    {
    RPositionServer server;
    TPositionModuleId moduleId = KPositionNullModuleId;
    TPositionModuleInfo info;
    server.GetModuleInfoById(moduleId, info);
    return KErrGeneral;
    }


TInt No_Server_GetModuleStatus(TAny* /*aPtr*/)
    {
    RPositionServer server;
    TPositionModuleStatus moduleStatus;
    TPositionModuleId moduleId = KPositionNullModuleId;
    server.GetModuleStatus(moduleStatus, moduleId);
    return KErrGeneral;
    }


TInt No_Server_NotifyModuleStatusEvent(TAny* /*aPtr*/)
    {
    RPositionServer server;
    TPositionModuleStatusEvent event;
    TPositionModuleId moduleId = KPositionNullModuleId;
    TRequestStatus request;
    server.NotifyModuleStatusEvent(event, request, moduleId);
    return KErrGeneral;
    }


TInt No_Server_CancelRequest(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    return KErrGeneral;
    }


void CNoServerStep::RPositionServer_NoL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    CheckThereIsNoServerL();
    
    RPositionServer server;

    CHECK( KErrNone != server.Connect() );
    FAIL( server.Handle() != 0 , "Server found, while should not be");
        
    DO_PANIC_TEST_L(
        No_Server_GetDefaultModuleId,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);
       
    DO_PANIC_TEST_L(
        No_Server_GetNumModules,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Server_GetModuleInfoByIndex,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Server_GetModuleInfoById,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Server_GetModuleStatus,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Server_NotifyModuleStatusEvent,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Server_CancelRequest,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    StandardCleanup();
    }


// 9.1.3 Connect and try to execute RPositioner methods
// This should fail with panic

TInt No_Positioner_CancelRequest(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    RPositioner positioner;
    positioner.CancelRequest(EPositionerGetLastKnownPosition);
    return KErrGeneral;
    }


TInt No_Positioner_Open(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    RPositioner positioner;
    positioner.Open(server);
    return KErrGeneral;
    }


TInt No_Positioner_Open_ModuleId(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    TPositionModuleId moduleId = {0x012345678};

    RPositioner positioner;
    positioner.Open(server, moduleId);
    return KErrGeneral;
    }


TInt No_Positioner_Open_Criteria(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    TPositionCriteria crit;
    
    RPositioner positioner;
    positioner.Open(server, crit);
    return KErrGeneral;
    }
    

// Close method should not fail    
TInt No_Positioner_Close(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    RPositioner positioner;
    positioner.Close();
    return KErrNone;
    }


TInt No_Positioner_SetRequestor(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    RPositioner positioner;
    positioner.SetRequestor(0, 0, KNullDesC);
    return KErrGeneral;
    }


TInt No_Positioner_SetRequestor_Stack(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    RRequestorStack stack;
    
    RPositioner positioner;
    positioner.SetRequestor(stack);
    return KErrGeneral;
    }


TInt No_Positioner_SetUpdateOptions(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    TPositionUpdateOptions options;

    RPositioner positioner;
    positioner.SetUpdateOptions(options);
    return KErrGeneral;
    }


TInt No_Positioner_GetUpdateOptions(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    TPositionUpdateOptions options;

    RPositioner positioner;
    positioner.GetUpdateOptions(options);
    return KErrGeneral;
    }


TInt No_Positioner_GetLastKnownPosition(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    TPositionInfo info;
    TRequestStatus status;

    RPositioner positioner;
    positioner.GetLastKnownPosition(info, status);
    return KErrGeneral;
    }


TInt No_Positioner_NotifyPositionUpdate(TAny* /*aPtr*/)
    {
    RPositionServer server;
    server.Connect();
    
    TPositionInfo info;
    TRequestStatus status;

    RPositioner positioner;
    positioner.NotifyPositionUpdate(info, status);
    return KErrGeneral;
    }


void CNoServerStep::RPositioner_NoL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    CheckThereIsNoServerL();
    
    RPositionServer server;

    CHECK( KErrNone != server.Connect() );
    FAIL( server.Handle() != 0, "Server found, while should not be" );
        
    DO_PANIC_TEST_L(
        No_Positioner_CancelRequest,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);
       
    DO_PANIC_TEST_L(
        No_Positioner_Open,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_Open_ModuleId,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_Open_Criteria,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_Close,
        KNoClientFault,
        KErrNone,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_SetRequestor,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_SetRequestor_Stack,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_SetUpdateOptions,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_GetUpdateOptions,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_GetLastKnownPosition,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    DO_PANIC_TEST_L(
        No_Positioner_NotifyPositionUpdate,
        KPosClientFault,
        EPositionServerBadHandle,
        KDefaultTestTimeout);

    StandardCleanup();
	}
