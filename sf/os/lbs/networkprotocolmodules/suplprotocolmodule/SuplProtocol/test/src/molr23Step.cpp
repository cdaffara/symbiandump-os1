// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file molr23Step.cpp
 @internalTechnology
*/
#include "molr23Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 Destructor
 */
Cmolr23Step::~Cmolr23Step()

	{
	}

/**
 Constructor
 */
Cmolr23Step::Cmolr23Step()
	{
	SetTestStepName(Kmolr23Step);
	}

/**
 @return - TVerdict code
 Override of base class virtual
 */
TVerdict Cmolr23Step::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();
	 
	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	 
	return TestStepResult();
	}


/** Perform CMoLrStep23 test step.
This test verifies that the SUPL Protocol Module correctly handles 
a request for additional assistance data sent from LBS after a change
from Terminal Based to Terminal Assisted during the SUPL POS RRLP
session.

This test also checks some aspects of assistance data handling:

1.- Assistance data types already made available to LBS but requested
    again by LBS will not be requested from the network.
2.- Assistance data types not supported by LBS (e.g. DgpsCorrections)
	are not requested from the network
3.- Assistance data types requested from the network but not reported
	by the network result in an assistance data error notification being
	sent to LBS.

@return TVerdict test result code
*/
TVerdict Cmolr23Step::doTestStepL()
	{
 	
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR basic procedure - Additional RRLP Assistance Data Requested"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));
	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654323), 0x1111);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataBadSatList;
	TLbsAsistanceDataGroup dataRequestMask2 = EAssistanceDataReferenceLocation;
	// Request both sets of assitance data. They'll be delivered in different
	// SUPL POS messages later on in the test.
	TLbsAsistanceDataGroup dataRequestMask = dataRequestMask1|dataRequestMask2;
	options1.SetDataRequestMask(dataRequestMask); 
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation"));
	iModule->RequestSelfLocation(sessionId1, options1);

	// Check Connection Manager receives a request for connecting
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EConnectReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t ConnectionRequest -> NET"));


	// Simulate the connection is up (inject that event)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Connected <- NET"));
    iNetworkObserver->InjectConnectedIndication(iNetworkObserver->SessionId());
	
	// Check Connection Manager receives a request to send a SUPL START
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESuplStartSendReq))
	{
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL START -> NET"));
	
	// Inject a SUPL RESPONSE
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL RESPONSE <- NET"));
	CSuplMessageBase* resp = BuildSuplResponseL(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), resp);
	
	// Check gateway receives Location Request
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		
	// LBS Requests assistance data
	INFO_PRINTF1(_L("\tLBS -> RequestAssistanceData ()"));
	TLbsNetSessionIdArray dummyIdArray;
	iModule->RequestAssistanceData(dataRequestMask, dummyIdArray);
	
	// Check Connection Manager receives a request to send a SUPL POS INIT
	// with the assistance data mask requested by the gateway
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosInitSendReq))
	{
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS INIT -> NET"));

	// Inject a SUPL POS (RRLP ASSIST DATA) with some of the Assistance data requested
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assitance Data <- NET"));
	CSuplMessageBase* suplPos = BuildSuplPosAssitDataL(dataRequestMask2, ETrue); // ETrue = more assistance data underway
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	

	// Check the Connection Manager receives a request to send a SUPL POS (ack to assistance data)
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assistance Data Ack -> NET"));

	// Inject a SUPL POS (RRLP MEASURE POSITION REQUEST) wich carries
	// assistance data.
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Measure Position Request <- NET"));
	suplPos = BuildSuplPosRequestL(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted, dataRequestMask1);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	


	// Gateway receives reference location and then assistance data 
	// (it happens in the same callback; errors checked separetely below)
	if (EFail == CheckGatewayCallbackL(CSuplGatewayObserver::EProcessAssistanceData) ||
		(dataRequestMask & iGatewayObserver->AssistanceDataSetMask() != dataRequestMask))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		
	// Ref Loc has been requested. Make sure it's been notified	(the callback mechanism does
	// not work well because EProcessAssistanceData overwrites EProcessLocationUpdate as they
	// happen in the same RunL of the SUPL Protocol). Instead, a flag has to be checked after
	// having received EProcessAssistanceData above
	if (!iGatewayObserver->IsRefLocNotified())
		{
		SetTestStepResult(EFail);
		return TestStepResult();			
		}
	
	// Reference Location is reported before Assistance Data...if we got this far,
	// both ref loc and assist data have been notified:
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate(Reference Location)"));
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData()"));
	
	// Check gateway receives a location request (due to the RRLP Measure Position Request)
	//
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// ****************************************************************************************
	// Fake LBS requires additional assistance data after receiving the location request above.
	// ****************************************************************************************

	// In addition to AquisitionAssistance and ReferenceTime, LBS requests again the same assistance data
	// it just got (datamask1 and datamask2) and also the unsupported DgpsCorrections. Only 
	// AcquisitionAssistance and EAssistanceDataReferenceTime should end up being requested from the
	// network in the RRLP Measure Position response.
	INFO_PRINTF1(_L("\tLBS -> RequestAssistanceData ()"));
	TLbsAssistanceDataGroup newDataMask = EAssistanceDataAquisitionAssistance | EAssistanceDataReferenceTime | EAssistanceDataDgpsCorrections;
	dataRequestMask = dataRequestMask1|dataRequestMask2|newDataMask;
	iModule->RequestAssistanceData(dataRequestMask, dummyIdArray);


	// Check that the Gateway Observer has been notified synchronously of an error 
	// due to assistance data type EAssistanceDataDgpsCorrections not being supported
	//
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData(EAssistanceDataDgpsCorrections, KErrArgument)"));		
	if (EFail == CheckGatewayCallbackL(CSuplGatewayObserver::EProcessAssistanceData) ||
		(KErrArgument != iGatewayObserver->AssistanceDataError()) ||
		(EAssistanceDataDgpsCorrections & iGatewayObserver->AssistanceDataErrorMask() != EAssistanceDataDgpsCorrections) )
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// Check the Connection Manager receives a request to send a SUPL POS (RRLP MEASURE POSITION RESPONSE)
	// The RRLP Measure Position Response Message should request assistance data types
	// EAssistanceDataAquisitionAssistance and EAssistanceDataReferenceTime only.
	//
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosSendReq) ||
				 (CSuplPosPayload::ERrlpMeasurePositionResp != iNetworkObserver->RrlpMessageType()) ||
				 ((EAssistanceDataAquisitionAssistance | EAssistanceDataReferenceTime) != iNetworkObserver->AssistDataReq()))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Measure Position Response -> NET"));


	// Inject a SUPL POS (RRLP MEASURE POSITION REQUEST) wich carries
	// assistance data. It only carries one of the assistance data types
	// previously requested (EAssistanceDataAquisitionAssistance)
	//
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Measure Position Request <- NET"));
	suplPos = BuildSuplPosRequestL(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted, EAssistanceDataAquisitionAssistance);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	


	// Check gateway receives the assistance data of type EAssistanceDataAquisitionAssistance
	// Check that gateway receives an assistance data error reporting missing assistance data
	// type EAssistanceDataReferenceTime (both are sent almost simultaneously so they have to
	// be checked in the same callback)
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData(EAssistanceDataReferenceTime, KErrNotFound)"));	
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData(EAssistanceDataAquisitionAssistance, KErrNone)"));	
	if (EFail == CheckGatewayCallbackL(CSuplGatewayObserver::EProcessAssistanceData) ||
		(EAssistanceDataAquisitionAssistance & iGatewayObserver->AssistanceDataSetMask() != EAssistanceDataAquisitionAssistance) ||
		(EAssistanceDataReferenceTime & iGatewayObserver->AssistanceDataErrorMask() != EAssistanceDataReferenceTime) ||
		(KErrNotFound != iGatewayObserver->AssistanceDataError()) )
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Check gateway receives a location request (due to the RRLP Measure Position Request)
	//
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Fake LBS sending results (position) to SPM
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	TReal64 latitude = 10;
	TReal64 longitude = 0;
	TReal32 HorAccuracy = 10;
	TPositionInfo mobilePosInfo;
	TTime utcTime;
	utcTime.UniversalTime();
	TCoordinate coor(latitude,longitude);
	TPosition mobilePos(TLocality(coor,HorAccuracy),utcTime);
	mobilePosInfo.SetPosition(mobilePos);
	iModule->RespondLocationRequest(sessionId1,KErrNone,mobilePosInfo);

	
	// Check the Connection Manager receives a request to send a SUPL POS (RRLP MEASURE POSITION RESPONSE)
	//
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Measure Position Response -> NET"));

	// Inject a SUPL END (without position)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END <- NET"));
	CSuplMessageBase* suplEnd = BuildSuplEndL(EFalse); // EFalse means no position
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplEnd);
	
	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Check Connection Manager receives a disconnection request
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EDisconnectReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t DisconnectRequest -> NET"));
	
	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
	iNetworkObserver->IsMoreObserverActivity())
	{
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	INFO_PRINTF1(_L("- END -"));
	
	SetTestStepResult(EPass);
	return TestStepResult();	

}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr23Step::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
