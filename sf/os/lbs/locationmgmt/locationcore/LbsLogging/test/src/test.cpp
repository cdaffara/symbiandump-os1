// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// test.cpp for logger
// 
//

#include "CommonFramework.h"
#include "LOGVIEW.H"

#include "lbspositioninfo.h"

#include "lbslogger.h"

// derived lbs logging event classes
#include <lbs/lbsselflocatelogevent.h>
#include <lbs/lbstransmitlocationlogevent.h>
#include <lbs/lbsnetworklocatelogevent.h>
#include <lbs/lbsexternallocatelogevent.h>
#include <lbs/lbsassistancedatalogevent.h>

#define APGS_MODINFO_MODULE_ID      			271064388
#define NETWORK_MODINFO_MODULE_ID   			271064387 

// constants definition
_LIT(KLbsLogFile, "C:\\LbsLog_TEST.TXT");
_LIT(KLbsLogResultFile, "C:\\LbsLogResult_TEST.TXT");
_LIT(KLbsLogX3PDestination, "X3P Desination");
_LIT(KLbsLogExternalOriginator, "External Originator");

// The overall test method used here consists of four steps, these four
// steps will be proceeded by default for each of the test cases described
// below:
//	--> Record logging details in a txt file (Create new file by default)
// 	--> Log into log engine
//	--> Read from log engine and save the details in another txt file (Create new file by default)
//	--> Compare two txt files line by line to decide fail or success of the test
//	This approach enables the automation run of the test on build machine
//  To read and navigate log entries from Symbain Log Engine via CLogViewEvent class

// ref: http://lon-xref.intra/lxr/source/common/generic/syslibs/logeng/ongoing/test/src/t_logbench.cpp
// http://lon-xref.intra/lxr/source/common/generic/comms-infras/nifman/src/eventlogger.cpp#029
// http://lon-xref.intra/lxr/source/common/generic/syslibs/logeng/ongoing/test/src/t_loghicaphelper.cpp#191
// http://lon-xref.intra/lxr/source/common/generic/syslibs/logeng/ongoing/logcli/src/logcli.cpp#118
// http://lon-xref.intra/lxr/source/common/generic/syslibs/logeng/ongoing/logwrap/src/logevent.cpp#021

// 0x80000001 = KRequestPending
// 1) Test AddEvent on five types of events
//		--> Globally open RLbsLogger
// 		--> Create event objects
//		--> Filling dummy values
//		--> Send AddEventL to RLbsLogger and check the returned error code
//		--> Globally close RLbsLogger
//		--> Create CLogViewEvent

// 2) Test UpdateEvent on five types of events
//		--> Globally open RLbsLogger
// 		--> Create event objects
//		--> Filling dummy values
//		--> Send AddEventL to RLbsLogger and check the returned error code
//		--> Update the previous event contents
//		--> Send UpdateEventL to RLbsLogger and check the returned error code
//		--> Globally close RLbsLogger

// 3) Test ChangeEvent on five types of events
//		--> Globally open RLbsLogger
//		--> Execute Test Step 1) and record the Event ID given by SLE
//		--> Obtain a CLogEvent from SLE via GetEventL()
//		--> Internalize CLogEvent into CLbsLogEvent (and derived event classes)
//		--> Change the details of event based on EventType
//		--> Call ChangeEventL() on RLbsLogger
//		--> Globally close RLbsLogger

// 4) Test DeleteEvent on five types of events
//		--> Globally open RLbsLogger
//		--> Execute Test Step 1) and record the Event ID given by SLE
//		--> Obtain a CLogEvent from SLE via GetEventL()
//		--> Call DeleteEventL() on CLogWrapper
//		--> Globally close RLbsLogger
//		(Only search on the ID to check existence or not.)

// 5) Performance test
//		a) using timer to add large group of events in the RLbsLogger > MaxBufferSize, 
//		test the response of Logging, repeating test step 1)
//		b) switch on/off the logging to compare the timing delay caused
//		by logging - To do so, need to put all logging scripts within 
//		logging Macro


// --------------------------------------------------------------------------
/**
  To log in an SelfLocate type of event
*/
TInt TEST_SelfLocate_Logging()
	{
	RLbsLogger logger;
	logger.OpenL(10);
	CLbsLogEvent* event = CLbsSelfLocateLogEvent::NewL(TUid::Uid(0), NULL);
	TUint cost = 1;
	event->SetCostInformation(cost); // set cost information if there are any;
	event->SetRequestOutcome(ELbsRequestOutcomeSuccess); // by passing the location request outcome;
	RProcess process;
	static_cast<CLbsSelfLocateLogEvent*>(event)->SetLocalApp(process.SecureId()); // by passing in NewL() the UID of the local application;

	// set position information
	TPositionInfoBase* posInfo = new TPositionInfo();
	posInfo->SetModuleId(TUid::Uid(APGS_MODINFO_MODULE_ID));
	posInfo->SetUpdateType(EPositionUpdateGeneral);
	TPositionModuleInfo::TTechnologyType techType = TPositionModuleInfo::ETechnologyTerminal;
	TPositionModeReason techTypeReason = EPositionModeReasonNone;
	posInfo->SetPositionMode(techType);
	posInfo->SetPositionModeReason(techTypeReason);
	TPosition position;
	position.SetCurrentTime();
	position.SetAccuracy(1.0,1.0);
	position.SetCoordinate(100.0,100.0);
	static_cast<TPositionInfo*>(posInfo)->SetPosition(position);

	event->SetPositionInfo(posInfo); // by passing optional TpositionInfoBase*;
	TInt err = logger.AddEventL(*event);
	delete event;
	logger.Close();
	return KErrNone;
	}
	
// --------------------------------------------------------------------------
/**
  To log in an Network Locate type of event
*/
TInt TEST_NetworkLocate_Logging()
	{
	RLbsLogger logger;
	logger.OpenL(10);
	CLbsLogEvent* event = CLbsNetworkLocateLogEvent::NewL(TUid::Uid(0), NULL);
	TUint cost = 1;
	event->SetCostInformation(cost); // set cost information if there are any;
	event->SetRequestOutcome(ELbsRequestOutcomeSuccess); // by passing the location request outcome;
	RProcess process;
	static_cast<CLbsNetworkLocateLogEvent*>(event)->SetLocalApp(process.SecureId()); // by passing in NewL() the UID of the local application;

	// set position information
	TPositionInfoBase* posInfo = new TPositionInfo();
	posInfo->SetModuleId(TUid::Uid(NETWORK_MODINFO_MODULE_ID));
	posInfo->SetUpdateType(EPositionUpdateGeneral);
	TPositionModuleInfo::TTechnologyType techType = TPositionModuleInfo::ETechnologyNetwork;
	TPositionModeReason techTypeReason = EPositionModeReasonNone;
	posInfo->SetPositionMode(techType);
	posInfo->SetPositionModeReason(techTypeReason);
	TPosition position;
	position.SetCurrentTime();
	position.SetAccuracy(1.0,1.0);
	position.SetCoordinate(100.0,100.0);
	static_cast<TPositionInfo*>(posInfo)->SetPosition(position);

	event->SetPositionInfo(posInfo); // by passing optional TpositionInfoBase*;
	TInt err = logger.AddEventL(*event);
	delete event;
	logger.Close();
	return KErrNone;
	}

// --------------------------------------------------------------------------
/**

*/
TInt TEST_Transmit_Logging()
	{
	RLbsLogger logger;
	logger.OpenL(10);
	CLbsLogEvent* event = CLbsTransmitLocationLogEvent::NewL(TUid::Uid(0), NULL, NULL);
	TUint cost = 1;
	event->SetCostInformation(cost); // set cost information if there are any;
	event->SetRequestOutcome(ELbsRequestOutcomeSuccess); // by passing the location request outcome;
	RProcess process;
	static_cast<CLbsTransmitLocationLogEvent*>(event)->SetX3PApp(process.SecureId()); // set the destination description

	// set position information
	TPositionInfoBase* posInfo = new TPositionInfo();
	posInfo->SetModuleId(TUid::Uid(NETWORK_MODINFO_MODULE_ID));
	posInfo->SetUpdateType(EPositionUpdateGeneral);
	TPositionModuleInfo::TTechnologyType techType = TPositionModuleInfo::ETechnologyAssisted;
	TPositionModeReason techTypeReason = EPositionModeReasonNone;
	posInfo->SetPositionMode(techType);
	posInfo->SetPositionModeReason(techTypeReason);
	TPosition position;
	position.SetCurrentTime();
	position.SetAccuracy(1.0,1.0);
	position.SetCoordinate(100.0,100.0);
	static_cast<TPositionInfo*>(posInfo)->SetPosition(position);

	event->SetPositionInfo(posInfo); // by passing optional TpositionInfoBase*;
	
	// set X3P destination
	HBufC* dest = HBufC::NewLC(20);
	*dest = KLbsLogX3PDestination;
	static_cast<CLbsTransmitLocationLogEvent*>(event)->SetDestination(dest);

	logger.AddEventL(*event);
	delete event;
	logger.Close();
	return KErrNone;
	}

// --------------------------------------------------------------------------
/**

*/
TInt TEST_ExternalLocate_Logging()
	{
	RLbsLogger logger;
	logger.OpenL(10);
	CLbsLogEvent* event = CLbsExternalLocateLogEvent::NewL(NULL, NULL);
	
	TUint cost = 1;
	event->SetCostInformation(cost); // set cost information if there are any;
	event->SetRequestOutcome(ELbsRequestOutcomeSuccess); // by passing the location request outcome;
	RProcess process;
	
	// set privacy request parameters
	TPrivacyRequestParams reqParams; 
	TLbsNetSessionId sessionId(process.SecureId(), 1);
	reqParams.iSessionId = sessionId;
	TLbsNetPosRequestPrivacy reqPri;
	reqPri.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
	reqPri.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	reqParams.iRequestPrivacy = reqPri;
	static_cast<CLbsExternalLocateLogEvent*>(event)->SetPrivacyRequestParams(reqParams); // set privacy request parameters from request originator
	
	TPrivacyResponseParams respParams;
	respParams.iSessionId = sessionId;
	respParams.iResult = CLbsNetworkProtocolBase::EPrivacyResponseAccepted;
	static_cast<CLbsExternalLocateLogEvent*>(event)->SetPrivacyResponseParams(respParams); // set privacy response parameters from privacy controller

	HBufC* dest = HBufC::NewLC(20);
	*dest = KLbsLogExternalOriginator;
	static_cast<CLbsExternalLocateLogEvent*>(event)->SetRequestOriginator(dest); // set the originator of the external location requestor originator

	// set position information
	TPositionInfoBase* posInfo = new TPositionInfo();
	posInfo->SetModuleId(TUid::Uid(NETWORK_MODINFO_MODULE_ID));
	posInfo->SetUpdateType(EPositionUpdateGeneral);
	TPositionModuleInfo::TTechnologyType techType = TPositionModuleInfo::ETechnologyAssisted;
	TPositionModeReason techTypeReason = EPositionModeReasonNone;
	posInfo->SetPositionMode(techType);
	posInfo->SetPositionModeReason(techTypeReason);
	TPosition position;
	position.SetCurrentTime();
	position.SetAccuracy(1.0,1.0);
	position.SetCoordinate(100.0,100.0);
	static_cast<TPositionInfo*>(posInfo)->SetPosition(position);

	static_cast<CLbsExternalLocateLogEvent*>(event)->SetReferenceLocation(posInfo); // set the reference location returned from external originator
	logger.AddEventL(*event);
	delete event;
	logger.Close();
	return KErrNone;
	}

// --------------------------------------------------------------------------
/**

*/
TInt TEST_AssistanceData_Logging()
	{
	RLbsLogger logger;
	logger.OpenL(10);
	CLbsLogEvent* event = CLbsAssistanceDataLogEvent::NewL(0, 0);
	TUint32 requestMask = 0x000F;
	TUint32 receiveMask = 0x0007;
	static_cast<CLbsAssistanceDataLogEvent*>(event)->SetRequestAssistanceDataType(requestMask); // requested assistance data type
	static_cast<CLbsAssistanceDataLogEvent*>(event)->SetReceiveAssistanceDataType(receiveMask); // received assistance data type
	static_cast<CLbsAssistanceDataLogEvent*>(event)->SetRequestOutcome(ELbsRequestOutcomeInconclusive); // by passing the location request outcome;
	logger.AddEventL(*event);
	delete event;
	logger.Close();
	return KErrNone;
	}

// --------------------------------------------------------------------------
/**

*/
TInt TEST_Performance_Logging()
	{
	
	return KErrNone;
	}


// --------------------------------------------------------------------------
// Test entry
LOCAL_C void doTestL()
    {
	TInt error = KErrNone;
	// instantialise a CLogView object for browsing log database
	
	error = TEST_SelfLocate_Logging();
	if(error != KErrNone)
		return;
	
	error = TEST_NetworkLocate_Logging();
	
	error = TEST_Transmit_Logging();
	
	error = TEST_ExternalLocate_Logging();
	
	error = TEST_AssistanceData_Logging();
	
	error = TEST_Performance_Logging();
	}
