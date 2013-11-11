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
// the SUPL MT-LR procedure state handler
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include "suplmtlrfsmsession.h"
#include "suplmtlrstatehandler.h"
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
@return A new instance of the CSuplMtLrStateHandler class
*/
CSuplMtLrStateHandler* CSuplMtLrStateHandler::NewL(CSuplFsmSessionBase& aMachine)
	{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::NewL() \n");
	CSuplMtLrStateHandler* self = new (ELeave) CSuplMtLrStateHandler(aMachine);
	return self;
	}

/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CSuplMtLrStateHandler::CSuplMtLrStateHandler(CSuplFsmSessionBase& aMachine)
: CSuplStateHandlerBase(aMachine)
	{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::CSuplMtLrStateHandler() Begin\n");
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::CSuplMtLrStateHandler() End\n");
	}


/** Standard destructor.
*/  
CSuplMtLrStateHandler::~CSuplMtLrStateHandler()
	{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::~CSuplMtLrStateHandler() Begin\n");
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::~CSuplMtLrStateHandler() End\n");
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
TBool CSuplMtLrStateHandler::EntryActionsL()
	{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsL() Begin\n");
	TBool actionsTaken = ETrue;
	// Retrieve current state and act accordingly
	switch(iMachine.CurrentState())
		{
		case CSuplFsmSessionBase::EStateProcedureNull:
			// No action required
			break;
		
		case CSuplFsmSessionBase::EStateSuplInitReceived:
			actionsTaken = EntryActionsFromSuplInitReceived();
			break;
			
		case CSuplFsmSessionBase::EStateNetConnectionStarted:
			actionsTaken = EntryActionsFromNetConnectionStarted();
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
	
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsL() End\n");
	return actionsTaken;
	}
//---------------------------------------------------------------------------------------------------------------------
// -----------------------     METHODS FOR ACTIONS AFTER ENTERING STATES  ---------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

/** Actions on entering state EStateSuplInitReceived
This method sends a LocationRequest and a PrivacyRequest
to LBS.
*/
TBool CSuplMtLrStateHandler::EntryActionsFromSuplInitReceived()
	{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsFromSuplInitReceived() Begin\n");
	TLbsNetPosRequestPrivacy privacyAdvice;
	TLbsExternalRequestInfo requestor;
	TLbsNetPosRequestMethod posMethods;
	TLbsNetPosMethod selectedMethod; // only one method is sent to LBS
	TLbsNetPosRequestQuality quality;

	// Send a PrivacyRequest followed by a Location Request to LBS
	iMachine.RetrievePrivacyAdvice(privacyAdvice);
	iMachine.RetrieveRequestorInfo(requestor);
	iMachine.Observer().PrivacyReq(iMachine.LbsSessionId(), privacyAdvice, requestor);


	// Get the pos methods received in a SUPL INIT
	// (kept in event store).
	iMachine.RetrieveInitPosMethod(posMethods);
	TLbsNetPosMethod netPrefMethod;
	posMethods.GetPosMethod(0, netPrefMethod);
	if (KLbsPositioningMeansNone != netPrefMethod.PosMeans()) // not a "noPosition" INIT
		{
		// Check that there are supported positioning methods different 
		// from Cell Based (Cell-Based can't be requested from LBS).
		TLbsNetPosCapabilities capabilities;
		iMachine.Observer().Gateway().GetCapabilities(capabilities);
		if (iMachine.PosMethodSupported(posMethods, selectedMethod,capabilities) &&
			KLbsPositioningMeansCell != selectedMethod.PosMeans())
			{
			// Method to be presented to LBS
			posMethods.SetPosMethods(&selectedMethod, 1);
			}
		else
			{
			// No mutually supported positioning methods.
			// Send to LBS the positioning method that is preferred
			// by LBS (the first in the capabilities)
			capabilities.GetPosMethod(0, selectedMethod);
			posMethods.SetPosMethods(&selectedMethod, 1);
			}

		iMachine.RetrieveInitQuality(quality);
		iMachine.Observer().LocationReq(iMachine.LbsSessionId(),	iMachine.LocReqType(),
										   quality, posMethods);	
		}

	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsFromSuplInitReceived() End\n");
	return ETrue;
	}
	

/** Actions on entering state EStateNetConnectionStarted

This method obtains an SLP Id from the Host Settings store
and asks the Connection Manager to provide a connection with it.
*/
TBool CSuplMtLrStateHandler::EntryActionsFromNetConnectionStarted()
	{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsFromNetConnectionStarted() Begin\n");
	TBool ret = ETrue;
	TLbsHostSettingsId hostId = TUid::Uid(0);
	TLbsNetSessionId::TSessionNum sessIdNum = 0;
	// Request a connection if one has not been set up already
	if (!iMachine.IsSessionConnected())
		{
		TInt err = iMachine.GetHostId(hostId); 
			
		if (KErrNone == err)
			{
			// Request a connection from the Connection Manager
			//
			sessIdNum = iMachine.SessionId().SessionNum();
			iMachine.Observer().ConnectionManager().Connect(sessIdNum, hostId, CSuplSessionRecord::EServiceMtlr, iMachine);
			// start the timer that oversees connection creation
			iMachine.StartConnectionTimer();		
			}
		else
			{
			// Cancel session as a SUPL server could not be determined
			iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt, CSuplFsmSessionBase::EReasonSlpSettingsMissing);
			}
		}
	else
		{
		 // This will happen if the connection has been created but the state
		 // machine remains in EStateNetConnectionStarted due to being waiting
		 // for an event from LBS (either a position notification or an assistance
		 // data request). Nothing to be done.
		 ret = EFalse;
		}
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsFromNetConnectionStarted() End\n");
	return ret;
	}
	


/** Actions on entering state EStatePosInitSent
This method builds a SUPL POS INIT message and hands it over to 
the Connection Manager for sending to the SUPL server.
*/
TBool CSuplMtLrStateHandler::EntryActionsFromPosInitSentStateL()
	{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsFromPosInitSentStateL() Begin\n");
	TBool err = EFalse;
	TLbsNetPosRequestQuality quality;
	TLbsNetPosCapabilities capabilities;
	TBool actionsTaken = EFalse;
	
	// There should be either an assistance data request 
	// or a position from LBS in the event store
	if (iMachine.IsAssistanceDataRequestStored() || iMachine.IsPositionStored())
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

		    // Set either a supplied position or assistance data request in the message
		 	if (iMachine.IsPositionStored())
			 	{
				TPositionInfoBase* posPtr = iMachine.RetrievePositionL();
				User::LeaveIfError(suplPosInit->SetPosition(*posPtr));
			 	}
			else if (iMachine.IsAssistanceDataRequestStored())
				{
				TLbsAsistanceDataGroup assitDataReqMask;
				TBool assitDataReqPresent = iMachine.RetrieveAssistanceDataRequest(assitDataReqMask);
				if (assitDataReqMask != 0)
					{
					User::LeaveIfError(suplPosInit->SetRequestedAssistanceData(assitDataReqMask));
					}
				else
					{
					// This was an empty assistance data request. No need to set it
					// in the POS INIT message (intentionally no action take here)
					}
				}
			
			// Set in message header the stored session id
			// after adding to it the SET session ID
			CSuplSessionId* sessionId = iMachine.MessageSessionId();
			TUint32 ipAddr = iMachine.RetrieveLocalIpAddress().Address();
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
			
			//sessionId->iSetSessionId->iSetId->iSetIdType = ESuplSetIdTypeIPAddress;
			//sessionId->iSetSessionId->iSetId->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
			//sessionId->iSetSessionId->iSetId->iIpAddress->iIpAddress.Copy(reinterpret_cast<unsigned char*>(&ipAddr), 4);
			
			User::LeaveIfError(suplPosInit->SetSessionId(*sessionId));
			
			// Set in message the Ver field
			TBuf8<8> ver;
			iMachine.RetrieveVer(ver);
			User::LeaveIfError(suplPosInit->SetVer(ver));
			
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
		err = ETrue;
		}
		
	if (err)
		{
		iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt, CSuplFsmSessionBase::EReasonNone);			
		}
	
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::EntryActionsFromPosInitSentStateL() End\n");
	return actionsTaken;
	}

/**
Utility method that builds the capabilities to send in a SUPL POS INIT using LBS's supported
capabilities and network preferred positioning method.
*/
TBool CSuplMtLrStateHandler::BuildCapabilitiesL(TLbsNetPosCapabilities& aCapabilities, TLbsNetPosRequestQuality& aQuality)
{
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::BuildCapabilitiesL() Begin\n");
	TLbsNetPosRequestMethod locReqMethods;
	TLbsNetPosMethod prefMethod;
	TInt numMethods;
	TInt index;
	TBool ecidSupported = EFalse;
	TInt additionalMethods = 1; // preferred method

	iMachine.Observer().Gateway().GetCapabilities(aCapabilities);
	numMethods = aCapabilities.NumPosMethods();
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

	// Determine what method has been used for this session (prefMethod).
	// The positioning method preferred by the SLP had been received in the 
	// SUPL INIT message and is kept in the event store.
	iMachine.RetrieveInitPosMethod(locReqMethods);
	TLbsNetPosCapabilities capabilities;
	iMachine.Observer().Gateway().GetCapabilities(capabilities);
	if (!iMachine.PosMethodSupported(locReqMethods, prefMethod, capabilities) &&
			KLbsPositioningMeansCell != prefMethod.PosMeans())
		{
		// No pos method received from the SLP was used for the session as
		// no supported method was found. The first method in 
		// the array of capabilities (LBS's preferred) was used instead. Get it.
		aCapabilities.GetPosMethod(0, prefMethod);
		}

	// The prefMethod has to be added at the end of the array of methods in aCapabilities
	// because that is where the ASN1 encoder expects it to be.
	// If E-CID is supported and there is room for it in the array, it will also be added to the 
	// array.
	TLbsNetPosMethod* methods = NULL;
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

	iMachine.RetrieveInitQuality(aQuality);
	SUPLLOG(ELogP1, "CSuplMtLrStateHandler::BuildCapabilitiesL() End\n");
	return ETrue;
}

