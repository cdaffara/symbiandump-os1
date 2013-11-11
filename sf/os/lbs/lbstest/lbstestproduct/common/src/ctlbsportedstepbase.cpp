// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "ctlbsportedstepbase.h"
#include "ctlbsportedsuitedefs.h"

/*
* ============================================================================
* ============================================================================
*/

//  INCLUDES
#include <test/testexecutestepbase.h>
#include <epos_cposmodules.h>
#include <epos_cposmoduleupdate.h>
#include <epos_cposmoduleidlist.h>
#include <simulationpsyinternalcrkeys.h>
#include "ctlbsclientlog.h"

// Device driver constants

TVerdict CT_LbsPortedStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CT_LbsPortedStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	// SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPortedStepBase::CT_LbsPortedStepBase(CT_LbsServer& aParent): CT_LbsStep(aParent)
    {
    iUidExternalGps.iUid 		= KUidExternalGps;
    iUidManualInput.iUid 		= KUidManualInput;
    iUidDefault.iUid 			= KUidDefault;
    iUidStrategy.iUid 			= KUidStrategy;
    iUidStubPositioner.iUid 	= KUidStubPositioner;
    iUidSimulationPsy.iUid 		= KUidSimulationPsy;
    iUidAGPSPsy.iUid			= KUidAGPSPsy;
    iUidNETPsy.iUid				= KUidNETPsy;
    iUidDynDbTestPsy.iUid 		= KUidDynDbTestPsy;      
    iUidInstallTestPsy.iUid 	= KUidInstallTestPsy;
    iUidTestPsySimulateIsa.iUid = KUidTestPsySimulateIsa;
    iUidTestPsy1.iUid       	= KUidTestPsy1;         
    iUidTestPsy2.iUid       	= KUidTestPsy2;        
    iUidTestPsy3.iUid       	= KUidTestPsy3;
    iUidTestPsy4.iUid       	= KUidTestPsy4;        
    iUidTestPsy5.iUid       	= KUidTestPsy5;        
    iUidTestPsy6.iUid       	= KUidTestPsy6;         
    iUidTestPsy7.iUid       	= KUidTestPsy7;         
    iUidTestPsy8.iUid       	= KUidTestPsy8;         
    iUidTestPsy9.iUid       	= KUidTestPsy9;         
    iUidTestPsy10.iUid      	= KUidTestPsy10;         
    iUidTestPsy11.iUid      	= KUidTestPsy11;         
    iUidTestPsy12.iUid      	= KUidTestPsy12;
    iUidTestPsy13.iUid      	= KUidTestPsy13;
    iUidTestPsyTp176.iUid   	= KUidTestPsyTp176;
    iUidMultiPsy.iUid       	= KUidMultiPsy;
    iUidTestPsy256.iUid     	= KUidTestPsy256;        
    iUidExamplePsy.iUid     	= KUidExamplePsy;
    iUidTestStatusPsy.iUid  	= KUidTestStatusPsy;
    iUidTestTimerPsy.iUid   	= KUidTestTimerPsy;
    iUidPanicPsy.iUid          	= KUidPanicPsy;
    iUidPsySwitching.iUid      	= KUidPsySwitching;
    iUidTestPsyPartialUpdate.iUid  = KUidTestPsyPartialUpdate;
    iUidTestPsyMaxAge.iUid     	= KUidTestPsyMaxAge;
    iUidTestRangePsy.iUid      	= KUidTestRangePsy;
    iUidTestSingPsy.iUid       	= KUidTestSingPsy;
    iUidTestTrackingPsy.iUid   	= KUidTestTrackingPsy;
    iUidTestProxyPsy1.iUid     	= KTestProxyPsy1;
    iUidTestProxyPsy2.iUid     	= KTestProxyPsy2;
    iUidTestProxyPsy3.iUid     	= KTestProxyPsy3;
    iUidTestProxyPsy4.iUid     	= KTestProxyPsy4;
	iUidTestProxyPsy5.iUid     	= KTestProxyPsy5;
    iUidInstallPsyTp273.iUid   	= KInstallPsyTp273;
    iUidPartialUpdate2.iUid    	= KUidTestPsyPartialUpdate2; 
    iUidSatInfoPsy.iUid        	= KUidSatInfoPsy;
    iUidBtgPsy.iUid            	= KUidBtgPsy;
    iUidIgPsy.iUid             	= KUidIgPsy;
	iUidInstallableTrackingPsy.iUid = KUidInstallableTrackingPsy;
    iUidDelayedResponsePsy.iUid = KUidDelayedResponsePsy;
    }

// Destructor
CT_LbsPortedStepBase::~CT_LbsPortedStepBase()
    {
    iPositioner.Close();
    iPosServer.Close();
    delete iScheduler;
    iFileServer.Close();   
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::StartL(TInt aIndex)
    {
    if (aIndex == 0)
        {
        
        iLog = CT_LbsClientLog::NewL(Logger());
        
	// Give extra time for epos server to shut down after previous tests:
	User::After(2000000);
        TRAPD(err, InitTestL());
        if (err == KErrNone)
            {
            TRAP(err, StartL());
            }
        CloseTest();
    	
        delete iLog;
        
        if (err != KErrNone)
            {
            User::Leave(err); // Not using LeaveIfError to allow positive error codes
            }
        }
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::SetupPsyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::SetupPsyL(const TUid aPsyUid,
                           TPosSetupPsyDirective /*aDirective*/)
    {
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    moduleUpdate->SetUpdateAvailability(EFalse);
    CPosModuleIdList* prioList = db->ModuleIdListLC();

    // Disable all PSY:s except aPsyUid
    for (TInt i = 0 ; i < prioList->Count(); i++)
        {
        if ((*prioList)[i] != aPsyUid)
            {
            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
            }
        }

    // Enable the PSY that came as an in parameter
	INFO_PRINTF2(_L("Enable psy 0x%x\n"), aPsyUid);
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(aPsyUid, *moduleUpdate);

    CleanupStack::PopAndDestroy(prioList);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    }
// ---------------------------------------------------------
// CT_LbsPortedStepBase::InitTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::InitTestL()
    {
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::CloseTest()
    {
    }

// ---------------------------------------------------------
// CTe_LbsPortedStepBase::ConnectL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::ConnectL()
	{
	TInt err = iPosServer.Connect();
	if (err != KErrNone)
		{
		_LIT(KOpenError,
			"TP%d, part %d failed. Unable to open connection to Epos, err = %d");
		TBuf<100> error;
		error.Format(KOpenError, iModuleId, iNrOfRequests, err);
		LogErrorAndLeaveL(error);
		}
	}

// ---------------------------------------------------------
// CT_LbsPortedStepBase::OpenPositionerByName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsPortedStepBase::OpenPositionerByName(
                                  const TUid aPsyUid)
	{
	TInt err = iPositioner.Open(iPosServer, aPsyUid);
	return err;
	}

// ---------------------------------------------------------
// CT_LbsPortedStepBase::OpenPositionerL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsPortedStepBase::OpenPositioner()
	{
	TInt err = iPositioner.Open(iPosServer);
	return err;
	}

// ---------------------------------------------------------
// CT_LbsPortedStepBase::ClosePositioner
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::ClosePositioner()
    {
    iPositioner.Close();
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::Disconnect
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::Disconnect()
    {
    iPosServer.Close();
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::PerformRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::PerformRequestL(const TDesC& aRequestor,
            CRequestor::_TRequestorType aRequestorType,
          CRequestor::_TRequestorFormat aRequestorFormat)
    {
    iNrOfRequests++;

    iModuleInfo = TPositionInfo();
    TInt err = iPositioner.SetRequestor(aRequestorType,
                             aRequestorFormat, aRequestor);
    if (err != KErrNone)
        {
        _LIT(KErrRequestor, "Error when setting requestor in PerformRequestL");
        LogErrorAndLeaveL(KErrRequestor, err);
        }

    iPositioner.NotifyPositionUpdate(iModuleInfo, iStatus);
    }

// ---------------------------------------------------------
// CTe_LbsPortedStepBase::PerformRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::PerformRequestL(
    const TDesC& aService, const TDesC& aContact,
    CRequestor::_TRequestorType aServiceIdType,
    CRequestor::_TRequestorType aContactIdType,
    CRequestor::_TRequestorFormat aServiceFormat,
    CRequestor::_TRequestorFormat aContactFormat)
	{
	CRequestor* contactId = 
        CRequestor::New(aContactIdType, aContactFormat, 
                    aContact);

	CRequestor* servId = 
    	CRequestor::New(aServiceIdType, aServiceFormat, 
                    aService);
    
	RRequestorStack stack = RRequestorStack();
  	stack.Append(contactId);
  	stack.Append(servId);

  	CleanupStack::PushL(contactId);
  	CleanupStack::PushL(servId);

  	iNrOfRequests++;

  	TInt err = iPositioner.SetRequestor(stack);
  	stack.Close();
  	if (err != KErrNone)
        {
        _LIT(KErrRequestor, "Error when setting requestor in PerformRequestL");
        LogErrorAndLeaveL(KErrRequestor, err);
        }
  
  	CleanupStack::PopAndDestroy(servId);
  	CleanupStack::PopAndDestroy(contactId);
  	iModuleInfo = TPositionInfo();
  	iPositioner.NotifyPositionUpdate(iModuleInfo, iStatus);
	}

// ---------------------------------------------------------
// CT_LbsPortedStepBase::PerformSyncRequestL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsPortedStepBase::PerformSyncRequest(
                            const TDesC& aServiceName,
                            TPositionInfoBase* aModuleInfo)
  {
  iNrOfRequests++;
  iPositioner.SetRequestor(CRequestor::ERequestorService,
                           CRequestor::EFormatApplication, aServiceName);

  iPositioner.NotifyPositionUpdate(*aModuleInfo, iStatus);
  
  User::WaitForRequest(iStatus);
  TInt err = iStatus.Int();
  return err;
  }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::CheckRequestResultL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::CheckRequestResultL(TInt aExpectedErrorCode, TBool aLogFlag)
    {
    User::WaitForRequest(iStatus);

    if (iStatus != aExpectedErrorCode)
        {
        _LIT(KError, "TP%d, request %d failed. iStatus = %d");
        TBuf<100> error;
        error.Format(KError, iModuleId, iNrOfRequests, iStatus.Int());
        LogErrorAndLeaveL(error);
        }
    else {
        if (aLogFlag)
            {
            _LIT(KMsg, "TP%d, part %d completed.");
            TBuf<30> msg;
            msg.Format(KMsg, iModuleId, iNrOfRequests);
            INFO_PRINTF1(msg);
            }
        }    
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::AssertTrueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::AssertTrueL(TBool aCondition, 
	const TDesC& aErrorMsg, TInt aErrorCode)
	{
	if (!aCondition)
        {
        TBuf<100> buf;
        buf.Format(aErrorMsg, aErrorCode);
        LogErrorAndLeaveL(buf);
        }
	}

// ---------------------------------------------------------
// CT_LbsPortedStepBase::AssertTrueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::AssertTrueL(TBool aCondition, 
	const TDesC& aErrorMsg)
	{
	if (!aCondition)
        {
        LogErrorAndLeaveL(aErrorMsg);
        }
	}

// ---------------------------------------------------------
// CTe_LbsPortedStepBase::AllocPositionInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPositionInfo CT_LbsPortedStepBase::AllocPositionInfoL()
    {
    TPositionInfo moduleInfo = TPositionInfo();
    iPositioner.GetLastKnownPosition(moduleInfo, iStatus);
    return moduleInfo;
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::VerifyPsyLoadedL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::VerifyPsyLoadedL(TPositionModuleId aUid)                           
    {
    _LIT(KVerPSYLoaded, "Verifies if PSY: %x, is loaded by getting its status");   
    TBuf<100> buf;
    buf.Format(KVerPSYLoaded, aUid.iUid);
    INFO_PRINTF1(buf);   
  
    _LIT(KDevStatusErr, "The PSY has not reported expected Device Status."); 
    TPositionModuleStatus moduleStatus;
    iPosServer.GetModuleStatus (moduleStatus, aUid); 
    AssertTrueL(TPositionModuleStatus::EDeviceReady == moduleStatus.DeviceStatus(), 
    		KDevStatusErr, KErrGeneral);
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::VerifyPsyUnloadedL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::VerifyPsyUnloadedL(TPositionModuleId aUid)                           
    {
    _LIT(KVerPSYUnloaded, "Verifies if PSY: %x, is unloaded by getting its status");   
    TBuf<100> buf;
    buf.Format(KVerPSYUnloaded, aUid.iUid);
    INFO_PRINTF1(buf);   
    
    _LIT(KDevStatusErr, "The PSY has not reported expected Device Status."); 
    TPositionModuleStatus moduleStatus;
    iPosServer.GetModuleStatus(moduleStatus, aUid); 
    AssertTrueL(TPositionModuleStatus::EDeviceInactive == moduleStatus.DeviceStatus(), 
    		KDevStatusErr, KErrGeneral);
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::RequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::RequestL(TPositionInfo& aPosInfo, 
                                    const TInt& aRequestIndex, 
                                    const TInt& aExpectedErrorCode)
    {
    TBuf<100> info;
    _LIT(KInfo, "Making request: %d");
    _LIT(KInfoCompCode, "Making request: %d, expecting completion code: %d.");
    if (aExpectedErrorCode != 999)
    	{
        info.Format(KInfoCompCode, aRequestIndex, aExpectedErrorCode);
    	}
    else 
    	{
        info.Format(KInfo, aRequestIndex);    
    	}    
    INFO_PRINTF1(info);

    aPosInfo.SetUpdateType(aRequestIndex); // Used here as request index

    TTime starttime, stoptime;
    starttime.UniversalTime();    

    TRequestStatus status;
    iPositioner.NotifyPositionUpdate(aPosInfo, status);
    User::WaitForRequest(status);
    
    stoptime.UniversalTime();
    iRequestTime = stoptime.Int64() - starttime.Int64();

    if (aExpectedErrorCode != 999)
    	{
        _LIT(KUnexpectedErrCode, "Unexpected error code %d, expected %d.");
        TBuf<200> buf;
        buf.Format(KUnexpectedErrCode, status.Int(), aExpectedErrorCode);
        AssertTrueL(status.Int() == aExpectedErrorCode, buf);
    	}
    }

// ---------------------------------------------------------
// CT_LbsPortedStepBase::LogErrorAndLeave
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CT_LbsPortedStepBase::LogErrorAndLeaveL(const TDesC& aErrorInfo,
                                              const TInt   aReason)
    {
    ERR_PRINTF2(aErrorInfo, aReason);
    User::Leave(aReason);
    }

TVerdict CT_LbsPortedStepBase::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    if (TestStepResult() == EPass)
        {
        CActiveScheduler* sched = new (ELeave) CActiveScheduler;
        CleanupStack::PushL(sched);
        if(!CActiveScheduler::Current() )
        	{
	        sched->Install(sched);
        	}
        StartL(0);
        CleanupStack::PopAndDestroy(sched);        
        SetTestStepResult(EPass);
        }
    return TestStepResult();
	}

// ---------------------------------------------------------
// CPosSimulationPSYTestProcedureBase::SetSimDataFile()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::SetSimDataFileL(const TDesC& aFileName)
    {
    CRepository* repository =
    CRepository::NewLC(KCRUidSimulationPSY);
    TInt err = repository->Set(KCRKeySimPSYSimulationFile,
                        aFileName);
    if (err != KErrNone)
    	{
    	_LIT(KErr, "CRepository::Set(KCRKeySimPSYSimulationFile...) returned error");
    	INFO_PRINTF1(KErr);
    	LogErrorAndLeaveL(KErr);
    	}
    	          
    CleanupStack::PopAndDestroy(repository); 
    }

// ---------------------------------------------------------
// CPosSimulationPSYTestProcedureBase::ResetDefaultPSYLastWorkingExtGPSPSYL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::ResetDefaultPSYLastWorkingExtGPSPSYL()
    {
    TPositionModuleId modId = KPositionNullModuleId;
    
    CRepository* repos = CRepository::NewL( KCRUidDefaultProxyConfiguration );    
    TBuf<40> idBuf;
    idBuf.AppendNumFixedWidth( modId.iUid, EHex, 8);    
    TInt err = repos->Set(KDefaultProxyLastWorkingGpsPsy, idBuf);
    delete repos;
    User::LeaveIfError(err);
    
	_LIT(KInfo, "ResetDefaultPSYLastWorkingExtGPSPSYL successfully executed.");
	INFO_PRINTF1(KInfo);
    }
	
// ---------------------------------------------------------
// CPosSimulationPSYTestProcedureBase::VerifyPositionFromL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::VerifyPositionFromL(TPositionInfo& aPosInfo, 
	TPositionModuleId& aModuleId)
    {
    if (aModuleId.iUid != aPosInfo.ModuleId().iUid)
	   {		
	   _LIT(KIncorrPsyErr,
	   		"Position from wrong PSY received, got position from: %x, expected: %x.");
	   TBuf<200> buf;
	   buf.Format(KIncorrPsyErr, aPosInfo.ModuleId().iUid, aModuleId.iUid);
	   LogErrorAndLeaveL(buf);
	   }    
    }

// ---------------------------------------------------------
// CPosSimulationPSYTestProcedureBase::VerifyPositionL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::VerifyPositionL(TPositionInfo& aPosInfo, 
	TReal64 aLat, TReal64 aLong, TReal64 aAlt)
	{
	_LIT(KWrongFix, "Wrong Fix");
	TPosition pos;
	aPosInfo.GetPosition(pos);
	AssertTrueL( aLat == pos.Latitude(), KWrongFix);
	AssertTrueL( aLong == pos.Longitude(), KWrongFix);
	AssertTrueL( aAlt == pos.Altitude(), KWrongFix);
	}

// ---------------------------------------------------------
// CPosSimulationPSYTestProcedureBase::VerifyRequestTimeLessThanL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPortedStepBase::VerifyRequestTimeLessThanL(TInt aExpectedReqTime)
	{
    if (iRequestTime > aExpectedReqTime)
	    {
	    _LIT(KTime, "Request time for last request was more than: %d");
	    TBuf<100> buf;
	    buf.Format(KTime, aExpectedReqTime);
	    INFO_PRINTF1(buf);
	    }
	}

void CT_LbsPortedStepBase::ConstructL()
	{
	iScheduler = new ( ELeave ) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
    User::LeaveIfError(iFileServer.Connect());
	}

TInt CT_LbsPortedStepBase::SetUpdateOptions(TPositionUpdateOptions aUpdateOptions)
	{
	//Set the update options
	return iPositioner.SetUpdateOptions(aUpdateOptions);
	}

// End of File
