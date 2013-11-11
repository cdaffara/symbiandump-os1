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
// This file provides the implementation of the class for
// the SUPL MO-LR procedure state handler
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include "suplmolrfsmsession.h"
#include "suplmolrstatehandler.h"
#include "suplgatewayinterface.h"
#include "suplmessagebase.h"
#include "suplstart.h"
#include "suplpos.h"
#include "suplposinit.h"
#include "suplresponse.h"
#include "suplend.h"
#include <etelmm.h>
#include "supldevloggermacros.h"


/** Static constructor.
@param aMachine A reference to the parent state machine.
@return A new instance of the CSuplMoLrStateHandler class
*/
CSuplMoLrStateHandler* CSuplMoLrStateHandler::NewL(CSuplFsmSessionBase& aMachine)
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::NewL()\n");
	CSuplMoLrStateHandler* self = new (ELeave) CSuplMoLrStateHandler(aMachine);
	return self;
	}

/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CSuplMoLrStateHandler::CSuplMoLrStateHandler(CSuplFsmSessionBase& aMachine)
: CSuplStateHandlerBase(aMachine)
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::CSuplMoLrStateHandler() Begin\n");
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::CSuplMoLrStateHandler() End\n");
	}


/** Standard destructor.
*/  
CSuplMoLrStateHandler::~CSuplMoLrStateHandler()
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::~CSuplMoLrStateHandler() Begin\n");
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::~CSuplMoLrStateHandler() End\n");
	}
	
/** Perform entry actions.
This is called from the state machine to perform any actions
associated with entering the current state following an external
event or an autonomous state transition.

For some states, there is only one possible course of action.
For other states, the event store has to be checked to see what
events have occurred and what actions have to be taken to handle
such events.

If an event conductive to cancelling the state machine had occurred
(e.g, an explicit client cancel, an error, or a timeout), then the
state machine would have been set in an state that terminates the 
session before this method is called (either EStateSuplSessionEnded
or EStatePosSessionEnded).

@return TBool ETrue if any actions have been taken
*/  
TBool CSuplMoLrStateHandler::EntryActionsL()
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsL() Begin\n");
	TBool actionsTaken = ETrue;
	// Retrieve current state and act accordingly
	switch(iMachine.CurrentState())
		{
		case CSuplFsmSessionBase::EStateProcedureNull:
			// No action
			break;
		
		case CSuplFsmSessionBase::EStateNetConnectionStarted:
			actionsTaken = EntryActionsFromNetConnectionStarted();
			break;

		case CSuplFsmSessionBase::EStateStartSent:
			// Build and send a SUPL START message
			actionsTaken = EntryActionsFromStartSentStateL();
			break;
			
		case CSuplFsmSessionBase::EStateResponseReceived:
			// Handle a received SUPL RESPONSE message
			actionsTaken = EntryActionsFromResponseReceivedStateL();
			break;
			
		case CSuplFsmSessionBase::EStatePosInitSent:
			// Build and send a SUPL POS INIT
			actionsTaken = EntryActionsFromPosInitSentStateL();
			break;

		case CSuplFsmSessionBase::EStatePositioningInProgress:
			actionsTaken = EntryActionsFromPositioningInProgressStateL();
			break;

		case CSuplFsmSessionBase::EStatePositionReceived:
			actionsTaken = EntryActionsFromPositionReceivedStateL();
			break;
			
		case CSuplFsmSessionBase::EStateSuplSessionEnded:
			// Send SUPL END
			actionsTaken = EntryActionsFromSuplSessionEndedStateL();
			break;

		case CSuplFsmSessionBase::EStatePosSessionEnded:
			// Send to Positioning FSM an instruction to terminate silently
			//
			iMachine.PositioningFsm()->CancelMachine(CSuplPositioningProtocolFsm::ESuplFsmCancel, CSuplFsmSessionBase::EReasonNone);
			break;
			
		case CSuplFsmSessionBase::EStateLbsSessionEnded:
			// Send session complete indication to LBS
			actionsTaken = EntryActionsFromLbsSessionEndedStateL();
			break;
			
		case CSuplFsmSessionBase::EStateNetConnectionClosed:
			// Close the network connection
			iMachine.CompleteProcedure();
			break;

		default:
			ASSERT(EFalse);
			break;
		}
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsL() End\n");	
	return actionsTaken;
	}
//---------------------------------------------------------------------------------------------------------------------
// -----------------------     METHODS FOR ACTIONS AFTER ENTERING STATES  ---------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

/** Actions on entering state EStateNetConnectionStarted

This method obtains an SLP Id from the Host Settings store
and asks the Connection Manager to provide a connection with it.
*/
TBool CSuplMoLrStateHandler::EntryActionsFromNetConnectionStarted()
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromNetConnectionStarted() Begin\n");
	TLbsHostSettingsId hostId = TUid::Uid(0);
	TLbsNetSessionId::TSessionNum sessIdNum = 0;
	TInt err = iMachine.GetHostId(hostId); 
		
	if (KErrNone == err)
		{
		// Request a connection from the Connection Manager
		//
		sessIdNum = iMachine.SessionId().SessionNum();
		iMachine.Observer().ConnectionManager().Connect(sessIdNum, hostId, CSuplSessionRecord::EServiceMolr, iMachine);
		// start the timer that oversees connection creation
		iMachine.StartConnectionTimer();		
		}
	else
		{
		// Cancel session as a SUPL server could not be determined
		iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt, CSuplFsmSessionBase::EReasonSlpSettingsMissing);
		}
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromNetConnectionStarted() End\n");
	return ETrue;
	}
	
/** Actions on entering state EStateStartSent

This method builds a SUPL START and passes it on to
the Connection Manager for sending to the SUPL server

*/
TBool CSuplMoLrStateHandler::EntryActionsFromStartSentStateL()
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromStartSentStateL() Begin\n");
	
	TLbsNetPosRequestQuality quality;
	TLbsNetPosCapabilities capabilities;

    ASSERT(iMachine.IsSessionConnected()); // should not have transitioned into this state without a connection

	// A SUPL START message must contain a Location ID with
	// network information (cell ID, etc). If such data has
	// not been made available to the state machine then
	// cancel ongoing session
	CSuplLocationId* locationId ;
	if (BuildLocationIdL(locationId))
		{
		CleanupStack::PushL(locationId);
		}
	else
		{
		iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt, CSuplFsmSessionBase::EReasonNone);
		return EFalse;
		}

	// Create a SUPL message common part.
	// Set the SET session ID only in CSuplSessionId.
	CSuplSessionId* sessionId = CSuplSessionId::NewL();
	CleanupStack::PushL(sessionId);
	sessionId->iSetSessionIdPresent = ETrue;
	sessionId->iSetSessionId->iSessionId = iMachine.SessionId().SessionNum();
	sessionId->iSetSessionId->iSetId->iSetIdType = ESuplSetIdTypeMsisdn; // TSuplSetIdType
	
	const TUint8 KMaxMsisdnLength = 8;
	TDes8& des = sessionId->iSetSessionId->iSetId->iSetId;
	if(iMachine.Msisdn())
		{
		ConvertToBcd(des);
		}
	else
		{
		//Fill the descriptor with all bits set to 0, then append a 0xFF to the end to indicate the end of the MSISDN
		des.SetLength(KMaxMsisdnLength);
		des.Fill(0x00, (KMaxMsisdnLength -1));	//7 0's
		des.Append(0xFF);	//Final 1 FF
		}
	
	//The commented code can be used when the set id type selection from the Host Settings API is implemented
	//TUint32 ipAddr = iMachine.RetrieveLocalIpAddress().Address();
	//sessionId->iSetSessionId->iSetId->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
	//sessionId->iSetSessionId->iSetId->iIpAddress->iIpAddress.Copy(reinterpret_cast<unsigned char*>(&ipAddr), 4);

	// Keep session Id for future reference
	iMachine.SetMessageSessionId(sessionId); // ownership transferred
	CleanupStack::Pop(sessionId);
	
	// positioning capabilities and request quality to be set in SUPL START
	// are built using the stored Request Options
	if (CSuplMolrFsmSession::ESuplMolrCellBased == (reinterpret_cast <CSuplMolrFsmSession&> (iMachine)).MachineType())
		{
		TLbsNetPosRequestOptions locReqOptions;
		iMachine.RetrieveLocationRequestOptions(locReqOptions);
		
		TLbsNetPosMethod posMethods[1];
		posMethods[0].SetPosMethod(KLbsPositioningMeansCell, TPositionModuleInfo::ETechnologyNetwork);
		capabilities.SetPosMethods(posMethods, 1);
		
		locReqOptions.GetRequestQuality(quality);
		}
	else // Not Cell-Based only
		{
		// Send all of LBS' capabilities in the SUPL START
		BuildCapabilitiesL(capabilities,quality);
		}

	// Create the SUPL START Message
	CSuplStart* suplStart = CSuplStart::NewLC();
	TInt err = suplStart->SetCapabilities(capabilities);
	err = suplStart->SetLocationId(*locationId);
	err= suplStart->SetSessionId(*sessionId);
	err = suplStart->SetQoP(quality);
	
	CleanupStack::Pop(suplStart);
	CleanupStack::PopAndDestroy(locationId);
	
 	// Ask the Connection Manager to send the SUPL START message to the server
	iMachine.Observer().ConnectionManager().SendMessage(suplStart, iMachine.SessionId().SessionNum());
	
	iMachine.SetSessionInProgress(ETrue);
	  
	// start the timer that oversees the arrival of SUPL RESPONSE
	(reinterpret_cast <CSuplMolrFsmSession&> (iMachine)).StartSuplResponseTimer();

	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromStartSentStateL() End\n");
	return ETrue;
	}

/** Actions on entering state EStateResponseReceived

This method extracts the positioning method from SUPL RESPONSE and 
sends a new location request to LBS.
*/
TBool CSuplMoLrStateHandler::EntryActionsFromResponseReceivedStateL()
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromResponseReceivedStateL() Begin\n");
	TBool actionsTaken = EFalse;
	
	// A SUPL RESPONSE must be the oldest SUPL message in the 
	// event store at this point.

	CSuplMessageBase* suplMessage = const_cast <CSuplMessageBase*> (iMachine.RetrieveSuplMessage());
	
	ASSERT(suplMessage != NULL);
	ASSERT(CSuplMessageBase::ESuplResponse == suplMessage->MessageType());
	
	if ((suplMessage != NULL) && (CSuplMessageBase::ESuplResponse == suplMessage->MessageType()))
		{
		CSuplResponse* responsePtr = static_cast <CSuplResponse*>(suplMessage);

		// Check that the SetId is the same that was sent in the SUPL START
		CSuplSessionId *sessId = CSuplSessionId::NewLC();
		responsePtr->GetSessionId(*sessId);
		if (! ((*(iMachine.MessageSessionId()->iSetSessionId)) == (*(sessId->iSetSessionId))))
			{
			iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonInvalidSessId);
			// Store received session Id for sending later in SUPL END as per SUPL specification
			CleanupStack::Pop(sessId);
			iMachine.SetMessageSessionId(sessId); // ownership transferred
			delete responsePtr;
			SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromResponseReceivedStateL() End\n");
			return actionsTaken;
			}
		else
			{
			CleanupStack::Pop(sessId);
			// Store received session Id for later use
			iMachine.SetMessageSessionId(sessId); // ownership transferred
			}

		// Extract the Positioning Method that SHALL (as per SUPL specification)
		// be used for the SUPL POS session
		TLbsNetPosRequestMethod posMethod;
		TLbsNetPosMethod selectedMethod; // only one to send to LBS
		TInt err = responsePtr->GetPosMethod(posMethod);
		TLbsNetPosCapabilities capabilities;
		iMachine.Observer().Gateway().GetCapabilities(capabilities);
		if ((KErrNone != err) || !iMachine.PosMethodSupported(posMethod, selectedMethod,capabilities))
			{
			//Cancel with error code posMethodMistmach
			iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonMethodMismatch);
			SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromResponseReceivedStateL() End\n");
			delete responsePtr;
			return actionsTaken;
			}

		// send request to LBS
		if (CSuplMolrFsmSession::ESuplMolrCellBased != (reinterpret_cast <CSuplMolrFsmSession&> (iMachine)).MachineType())
			{
			// Methods to be requested from LBS
			posMethod.SetPosMethods(&selectedMethod, 1);
			
			// Quality to be requested from LBS. The SUPL RESPONSE constains no quality requirements.
			//  Use the quality originally requested by LBS
			TLbsNetPosRequestOptionsAssistance locReqAssistOptions;
			TLbsNetPosRequestQuality quality;
			iMachine.RetrieveLocationRequestOptions(locReqAssistOptions);
			locReqAssistOptions.GetRequestQuality(quality);
			
			iMachine.Observer().LocationReq(iMachine.LbsSessionId(),	iMachine.LocReqType(),
									quality, posMethod);
			}

		delete responsePtr;
		actionsTaken = ETrue;
		}
	
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromResponseReceivedStateL() End\n");
	return actionsTaken;
	}


/** Actions on entering state EStatePosInitSent
This method builds a SUPL POS INIT message and hands it over to 
the Connection Manager for sending to the SUPL server.
*/
TBool CSuplMoLrStateHandler::EntryActionsFromPosInitSentStateL()
	{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromPosInitSentStateL() Begin\n");
	TBool err = EFalse;
	TLbsNetPosRequestQuality quality;
	TLbsNetPosCapabilities capabilities;
	TBool actionsTaken = EFalse;
	
	// There should be an assistance data request from LBS in the event store
	//
	if (iMachine.IsAssistanceDataRequestStored())
		{
		ASSERT(iMachine.IsSessionConnected()); // should not have transitioned into this state without a connection

		// Create the SUPL POS INIT Message
		CSuplPosInit* suplPosInit = CSuplPosInit::NewL();
	 	CleanupStack::PushL(suplPosInit);
	 	
	 	// Set the Location Id in the message
		CSuplLocationId* locationId;
		if (BuildLocationIdL(locationId))
			{
			CleanupStack::PushL(locationId);
	 	
	 		suplPosInit->SetLocationId(*locationId);
			CleanupStack::PopAndDestroy(locationId);
	 	
			// Set Capabilities in the message
			BuildCapabilitiesL(capabilities,quality);
			User::LeaveIfError(suplPosInit->SetCapabilities(capabilities));
			
			// Set assistance data request in the message if any data 
			// is actually required
			TLbsAsistanceDataGroup assitDataReqMask;
			TBool assitDataReqPresent = iMachine.RetrieveAssistanceDataRequest(assitDataReqMask);
			if (assitDataReqMask != 0)
				{
				User::LeaveIfError(suplPosInit->SetRequestedAssistanceData(assitDataReqMask));
				}
			else
				{
				// No need to set assistance data request parameter
				// in SUPL POS INIT when request is empty.
				// (intentionally no action taken here)
				}

			// Set in message header the stored session id
			CSuplSessionId* msgId = iMachine.MessageSessionId();
			User::LeaveIfError(suplPosInit->SetSessionId(*msgId));
	
			CleanupStack::Pop(suplPosInit);

			// Ask the Connection Manager to send the SUPL POS INIT message
			iMachine.Observer().ConnectionManager().SendMessage(suplPosInit, iMachine.SessionId().SessionNum());
			
			// Start the timer that oversees the arrival of SUPL POS
			iMachine.StartSuplPosTimer();
			
			actionsTaken = ETrue;
			}
		else
			{
			// Failed to build Location Id
			err = ETrue;
			CleanupStack::PopAndDestroy(suplPosInit);
			}
		}
	else
		{
		// An unexpected event must have happened (e.g. SUPL Message received)
		// that triggered the transition to this state.
		err = ETrue;
		}
		
	if (err)
	{
	iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt, CSuplFsmSessionBase::EReasonNone);			
	}
	
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::EntryActionsFromPosInitSentStateL() End\n");
	return actionsTaken;
	}

/**
Utility method that obtains the capabilities in the stored location request options
and adds to them the preferred positioning method also stored in the location request
option
*/
TBool CSuplMoLrStateHandler::BuildCapabilitiesL(TLbsNetPosCapabilities& aCapabilities, TLbsNetPosRequestQuality& aQuality)
{
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::BuildCapabilitiesL() Begin\n");
	TLbsNetPosRequestOptionsAssistance locReqAssistOptions;
	TLbsNetPosMethod prefMethod;
	TInt numMethods;
	TInt index;
	TBool ecidSupported = EFalse;
	TInt additionalMethods = 0;

	// The positioning method preferred by LBS was sent in the call to 
	// RequestSelfLocation and was kept in the event store.
	iMachine.Observer().Gateway().GetCapabilities(aCapabilities);
	numMethods = aCapabilities.NumPosMethods();
	iMachine.RetrieveLocationRequestOptions(locReqAssistOptions);
	prefMethod.SetPosMethod(KLbsPositioningMeansGps, locReqAssistOptions.PosMode());
	additionalMethods ++;
	
	// Check if E-CID is possible
	// (currently this mean Timing Advance only)
	if(iMachine.IsNetworkInfoAvailable())
		{
		RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
		RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
		iMachine.RetrieveStoredNetworkInfo(networkInfo, locationArea);	
		if (RMobilePhone::ENetworkModeGsm == networkInfo.iMode)
			{
			RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
			if(iMachine.RetrieveStoredCellInfo(cellInfo))
				{
				// Timing Advance not supported if negative
				ecidSupported = (cellInfo.iTimingAdvance >= 0);
				additionalMethods++;			
				}
			}
		}

	// The preferred method has to be added at the end of the array of methods in "capabilities"
	// because that is where the ASN1 encoder expects it to be.
	// If E-CID is supported and there is room for it in the array, it will also be added to the 
	// array.
	TLbsNetPosMethod* methods;
	if  (numMethods + additionalMethods < KLbsMaxNetPosMethods)
		{
		TLbsNetPosMethod ecidMethod;
		ecidMethod.SetPosMethod(KLbsPositioningMeansCell, TPositionModuleInfo::ETechnologyNetwork);
		methods = new (ELeave) TLbsNetPosMethod[numMethods + additionalMethods];
		
		for (index =0; index < numMethods; index++)
			{
			aCapabilities.GetPosMethod(index, methods[index]);
			}
			
		if (ecidSupported)
			{
			methods[numMethods] = ecidMethod;	
			}
			
		// Add preferred method at the end
		methods[numMethods + additionalMethods - 1] = prefMethod;
		aCapabilities.SetPosMethods(methods, numMethods + additionalMethods);
		}
	else
		{
		// There is only additional room for 1 or 0 methods.
		methods = new (ELeave) TLbsNetPosMethod[KLbsMaxNetPosMethods];

		for (index =0; index < KLbsMaxNetPosMethods; index++)
			{
			aCapabilities.GetPosMethod(index, methods[index]);
			}

		// Set the preferred method at the end (even if it 
		// overwrites current entry)
		methods[KLbsMaxNetPosMethods-1] = prefMethod;
		aCapabilities.SetPosMethods(methods, KLbsMaxNetPosMethods);	
		}

	delete[] methods;
	locReqAssistOptions.GetRequestQuality(aQuality);
	
	SUPLLOG(ELogP1, "CSuplMoLrStateHandler::BuildCapabilitiesL() End\n");
	return ETrue;
}

