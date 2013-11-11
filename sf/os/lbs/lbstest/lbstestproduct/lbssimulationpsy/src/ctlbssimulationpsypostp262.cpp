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



//  INCLUDES
#include "ctlbssimulationpsypostp262.h"

// CONSTANTS

#ifdef __WINS__    
    _LIT(KSimFile, "z:\\system\\test\\TestData\\simulationData.sps");
    _LIT(KSimFileBadSyntax, "z:\\system\\test\\TestData\\simulationBadSyntax1.sps");
    //simulationBadSyntax1.sps
    _LIT(KIncorrectNmeaFile, "z:\\system\\test\\TestData\\IncorrectChecksum.nme");
#else
    _LIT(KSimFile, "c:\\system\\test\\TestData\\simulationData.sps");
    _LIT(KIncorrectNmeaFile, "c:\\system\\test\\TestData\\IncorrectChecksum.nme");
    _LIT(KSimFileBadSyntax, "c:\\system\\test\\TestData\\simulationBadSyntax1.sps");
#endif

_LIT(KServiceName, "TP262");


// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp262::CT_LbsSimulationPSYPosTp262(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP262 - Status Events");
	SetTestStepName(KTestName);
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp262::~CT_LbsSimulationPSYPosTp262()
	{
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp262::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp262::StartL()
    {
    _LIT(KConnectErr, "Error when connecing to Position Server, %d");
    
    TInt err = iPosServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err); 
	
    //SIMULATION mode starts here
    //normal case in simulation mode
	_LIT(KInfDatafileCorrect, "***Correct sim datafile***");
    INFO_PRINTF1(KInfDatafileCorrect);
    SetSimDataFileL(KSimFile);

    iModuleEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventAll);
    
    OpenL(KErrNone);

    VerifyStatusRequestL(
                    TPositionModuleStatus::EDeviceReady, 
                    TPositionModuleStatus::EDataQualityNormal);

    iPositioner.SetRequestor(
                    CRequestor::ERequestorService,
					CRequestor::EFormatApplication, KServiceName);

    MakePosRequestL(KErrNone);
    CloseAndVerifyStatusL();
   
    //Missing sim data file
    _LIT(KInfDatafileMissing, "***Missing sim datafile***");
    INFO_PRINTF1(KInfDatafileMissing);
    _LIT(KPath, "c:\\dfsfsdf.sps");
    SetSimDataFileL(KPath); //maybe also test a file that does not exist and
    OpenL(KErrNotFound);

    VerifyStatusRequestL(
                    TPositionModuleStatus::EDeviceError, 
                    TPositionModuleStatus::EDataQualityUnknown);

    CloseAndVerifyStatusL();

    //bad syntax sim datafile
    _LIT(KInfDatafileBadSintax, "***Bad syntax sim datafile***");
    INFO_PRINTF1(KInfDatafileBadSintax);
    SetSimDataFileL(KSimFileBadSyntax);
    OpenL(KErrCorrupt);

    VerifyStatusRequestL(
                    TPositionModuleStatus::EDeviceError, 
                    TPositionModuleStatus::EDataQualityUnknown);

    CloseAndVerifyStatusL();

    //NMEA test starts here
    //bad checksum, but this should not affect anything should
    //be treated as the normal case just a little bit more tricky
    _LIT(KInfDatafileCorrectNmea, "***Correct nmea datafile***");
    INFO_PRINTF1(KInfDatafileCorrectNmea);
    SetSimDataFileL(KIncorrectNmeaFile);
    
    OpenL(KErrNone); 
    VerifyStatusRequestL(
                    TPositionModuleStatus::EDeviceReady, 
                    TPositionModuleStatus::EDataQualityNormal);
    
    iPositioner.SetRequestor(
                    CRequestor::ERequestorService,
					CRequestor::EFormatApplication, KServiceName);   

    MakePosRequestL(KErrNone);

    CloseAndVerifyStatusL();    

    //missing nme datafile
    _LIT(KInfDatafileMissingNmea, "***Missing nme datafile***");
    INFO_PRINTF1(KInfDatafileMissingNmea);
    _LIT(KFileName, "dfsfsdf.nme");
    SetSimDataFileL(KFileName);
    // For some reason if no drive is specified in filename, KErrPathNotFound is returned
    OpenL(KErrPathNotFound);

    VerifyStatusRequestL(
                    TPositionModuleStatus::EDeviceError, 
                    TPositionModuleStatus::EDataQualityUnknown);

    CloseAndVerifyStatusL();

    //general test cases starts here
    _LIT(KInfSimNotNmeOrSps, "***sim file that is neither nme or sps file***");
    INFO_PRINTF1(KInfSimNotNmeOrSps);
    _LIT(Kdfsfsdf, "dfsfsdf");
    SetSimDataFileL(Kdfsfsdf);
    OpenL(KErrNotSupported);

    VerifyStatusRequestL(
                    TPositionModuleStatus::EDeviceError, 
                    TPositionModuleStatus::EDataQualityUnknown);

    CloseAndVerifyStatusL();

	//Test with empty string as simulation data file in registry
    _LIT(KEmptyStringFile, "***empty string as file***");
    INFO_PRINTF1(KEmptyStringFile);
    _LIT(KEmptyString, "");
    SetSimDataFileL(KEmptyString);

  }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp262::OpenL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp262::OpenL(const TInt aExpectedError)
    {
    _LIT(KFunctionName, "Opening");
    INFO_PRINTF1(KFunctionName);
    _LIT(KOpenErr, "Error or unexpected error when opening positioner,  %d");
    
    IssueStatusRequest();
    TInt err = iPositioner.Open(iPosServer, iUidSimulationPsy);
  
    User::WaitForRequest(iEventStatus);  
    AssertTrueL(err == aExpectedError, KOpenErr, err);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp262::OpenAndVerifyStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp262::CloseAndVerifyStatusL()
    {
    _LIT(KFunctionName, "Closing and verifying status");
    INFO_PRINTF1(KFunctionName);
   
    IssueStatusRequest();
    iPositioner.Close();
    User::WaitForRequest(iEventStatus); 
   
    VerifyStatusRequestL(
                    TPositionModuleStatus::EDeviceInactive, 
                    TPositionModuleStatus::EDataQualityUnknown);

    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp262::OpenAndVerifyStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp262::MakePosRequestL(const TInt aExpectedStatus)
    {
    _LIT(KInfoFunction, "Requests a positioning and verifies completed status");
    INFO_PRINTF1(KInfoFunction);
    _LIT(KStatusErr, "Unexpected status when request completed , %d");
    
    TRequestStatus status;
    TPositionInfo posInfo;
    iPositioner.NotifyPositionUpdate(posInfo, status);
    User::WaitForRequest(status);

    AssertTrueL(status.Int() == aExpectedStatus, KStatusErr, status.Int());
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp262::IssueStatusRequest()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp262::IssueStatusRequest()
    {
    _LIT(KFunctionName, "We are inside IssueStatusRequest()");
    INFO_PRINTF1(KFunctionName);
    iPosServer.NotifyModuleStatusEvent(iModuleEvent, iEventStatus, iUidSimulationPsy);
    }
    
// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp262::VerifyStatusRequest()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp262::VerifyStatusRequestL(
                                const TPositionModuleStatus::TDeviceStatus& aStatus,
                                const TPositionModuleStatus::TDataQualityStatus& aQuality)
    {
    _LIT(KInfFunction, "Verifies reported device status and quality status");
    INFO_PRINTF1(KInfFunction);

    _LIT(KStatusErr, "NotifyModuleStatusEvent is not returning status KErrNone but %d");
    _LIT(KDeviceStatusErr, "The PSY has not reported expected Device Status"); 
    _LIT(KQualityStatusErr, "The PSY has not reported expected Quality Status");
    
    AssertTrueL(iEventStatus.Int() == KErrNone, KStatusErr, iEventStatus.Int());

    TPositionModuleStatus moduleStatus;
    iModuleEvent.GetModuleStatus(moduleStatus);
    
    TInt deviceStatus = moduleStatus.DeviceStatus();  
    TInt qualityStatus = moduleStatus.DataQualityStatus(); 
    AssertTrueL(deviceStatus == aStatus, KDeviceStatusErr, KErrGeneral);
    AssertTrueL(qualityStatus == aQuality, KQualityStatusErr, KErrGeneral);
    
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp262::CloseTest()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp262::CloseTest()
	{
    iPosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
  
    iPositioner.Close();
    iPosServer.Close();
    }

//  End of File
