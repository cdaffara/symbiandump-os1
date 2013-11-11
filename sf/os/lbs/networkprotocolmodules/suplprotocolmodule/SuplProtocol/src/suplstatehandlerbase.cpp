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
// This file provides the implementation of the base class for
// protocol states used in the SUPL Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include "suplstatehandlerbase.h"
#include "supldevloggermacros.h"
#include "suplend.h"
#include "suplgatewayinterface.h"

/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CSuplStateHandlerBase::CSuplStateHandlerBase(CSuplFsmSessionBase& aMachine)
: iMachine(aMachine)
	{
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::CSuplStateHandlerBase() Begin\n");
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::CSuplStateHandlerBase() End\n");
	}


/** Standard destructor.
*/  
CSuplStateHandlerBase::~CSuplStateHandlerBase()
	{
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::~CSuplStateHandlerBase() Begin\n");
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::~CSuplStateHandlerBase() End\n");
	}


/**
Utility method to retrieve the location Id in from the store
@param Pointer to the location Id. Caller owns the heap object created.
*/
TBool CSuplStateHandlerBase::BuildLocationIdL(CSuplLocationId*& aLocationId)
{
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::BuildLocationIdL() Begin\n");
	TBool ret = EFalse;
	CSuplLocationId* ptr = NULL;

	//  Location ID is constructed with network information
	// stored by the state machine.

	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
	RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;

	if(iMachine.IsNetworkInfoAvailable())
		{
		iMachine.RetrieveStoredNetworkInfo(networkInfo, locationArea);
		
		switch(networkInfo.iMode)
			{
			case RMobilePhone::ENetworkModeGsm:
				{
				RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
				ptr = CSuplLocationId::NewL(ESuplLocationTypeGsm);
				aLocationId = ptr;
				TLex(networkInfo.iCountryCode).Val(aLocationId->iGsmCellInfo->iRefMCC);
				TLex(networkInfo.iNetworkId).Val(aLocationId->iGsmCellInfo->iRefMNC);
				aLocationId->iGsmCellInfo->iRefLAC = locationArea.iAreaKnown?locationArea.iLocationAreaCode:0;
				aLocationId->iGsmCellInfo->iRefCI  = locationArea.iCellId;
				if(iMachine.RetrieveStoredCellInfo(cellInfo))
					{
					aLocationId->iGsmCellInfo->iTA = cellInfo.iTimingAdvance;			
					}
				ret = ETrue;
				}
			break;

			case RMobilePhone::ENetworkModeWcdma:
				ptr = CSuplLocationId::NewL(ESuplLocationTypeWcdma);
				aLocationId = ptr;
				TLex(networkInfo.iCountryCode).Val(aLocationId->iWcdmaCellInfo->iRefMCC);
				TLex(networkInfo.iNetworkId).Val(aLocationId->iWcdmaCellInfo->iRefMNC);
				aLocationId->iWcdmaCellInfo->iRefUC  = locationArea.iCellId;
				ret = ETrue;
			break;
			
			default: // only GSM and WCDMA networks are supported
				ret = EFalse;
			 break;
			}

		}
		
	if(ptr != NULL)
		{
		RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
		if(iMachine.RetrieveStoredCellInfo(cellInfo))
			{
			switch(cellInfo.iStatus)
				{
				case RMobilePhone::ECellInfoStatusNotCurrent:
					ptr->iStatus = ESuplLocStatusStale;
					break;
				case RMobilePhone::ECellInfoStatusCurrent:
					ptr->iStatus = ESuplLocStatusCurrent;				
					break;				
				default:
				case RMobilePhone::ECellInfoStatusUnknown:
					ptr->iStatus = ESuplLocStatusUnknown;
					break;	
				}
			}
		else
			{
			ptr->iStatus = ESuplLocStatusCurrent;
			}
		}

	SUPLLOG(ELogP1, "CSuplStateHandlerBase::BuildLocationIdL() End\n");
	return ret;	
}

/** Actions on entering state EStatePositioningInProgress
Events conductive to (re)entering this state are a SUPL POS
message received from the SLP or a request from the positioning
state machine for sending a SUPL POS with a positining protocol
payload.
*/
TBool CSuplStateHandlerBase::EntryActionsFromPositioningInProgressStateL()
	{
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromPositioningInProgressStateL() Begin\n");
	TBool actionsTaken = EFalse;

	// Check first if a positioning payload is available in the event
	// store for sending out
	const CSuplPosPayload* constPosPayload = iMachine.RetrievePosPayload();
	CSuplPosPayload* posPayload = const_cast <CSuplPosPayload*> (constPosPayload);
	if (posPayload)
		{
		ASSERT(iMachine.IsSessionConnected());

		// Set payload in a SUPL POS 
		CSuplPos* suplPos = CSuplPos::NewLC(ETrue);
		suplPos->SetPosPayload(posPayload);

		CSuplSessionId* msgId = iMachine.MessageSessionId();
		
		suplPos->SetSessionId(*msgId);
		
		CleanupStack::Pop(suplPos);
		// Ask the Connection Manager to send the SUPL POS message
		iMachine.Observer().ConnectionManager().SendMessage(suplPos, iMachine.SessionId().SessionNum());
		
		// If this was the last of the SUPL POS messages (positioning state machine
		// has terminated), start the SUPL END timer.
		if (iMachine.RetrievePositioningSessionEnded())
			{
			iMachine.StartSuplEndTimer();
			}
		
		actionsTaken = ETrue;
		}
	else 
		{
		// There must be a SUPL POS in the event store
		CSuplMessageBase* suplMessage = const_cast <CSuplMessageBase*> (iMachine.RetrieveSuplMessage());
		ASSERT(suplMessage != NULL);
		ASSERT(CSuplMessageBase::ESuplPos == suplMessage->MessageType());

		if ((suplMessage != NULL) && (CSuplMessageBase::ESuplPos == suplMessage->MessageType()))
			{
			CSuplPos* suplPosMessage = static_cast <CSuplPos*>(suplMessage);

			// make sure the session id matches the one received in earlier messages
			CSuplSessionId *sessId = CSuplSessionId::NewLC();
			suplPosMessage->GetSessionId(*sessId);
			if (! ((*(iMachine.MessageSessionId())) == (*sessId)))
				{
				iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonInvalidSessId);
				// Store received session Id for sending later in SUPL END as per SUPL specification
				CleanupStack::Pop(sessId);
				iMachine.SetMessageSessionId(sessId); // ownership transferred
				delete suplPosMessage;
				SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromPositioningInProgressStateL() End\n");
				return actionsTaken;
				}
			else
				{
				CleanupStack::PopAndDestroy(sessId);
				}
			// Extract the payload
			posPayload = suplPosMessage->PosPayload();
			ASSERT(posPayload != NULL);
			if (posPayload)
				{
				// send the payload to the positioning fsm
				iMachine.PositioningFsm()->ProcessPositioningMessage(posPayload);
				}

			// Delete the SUPL POS message
			delete suplPosMessage;
			actionsTaken = ETrue;			
			}
		}
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromPositioningInProgressStateL() End\n");
	return actionsTaken;
	}

/** Actions on entering state EStatePositionReceived
This method extracts a position from a SUPL END message and
sends it to LBS.
*/
TBool CSuplStateHandlerBase::EntryActionsFromPositionReceivedStateL()
	{
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromPositionReceivedStateL() Begin\n");
	TBool actionsTaken = EFalse;

	// Extract the position from the SUPL END in the event store and send to
	// LBS in a ProcessLocationUpdate method
	TPosition position;

	// There must be a SUPL END in the store
	CSuplMessageBase* suplMessage = const_cast <CSuplMessageBase*> (iMachine.RetrieveSuplMessage());
	
	ASSERT(suplMessage != NULL);
	ASSERT(CSuplMessageBase::ESuplEnd == suplMessage->MessageType());

	if ((suplMessage != NULL) && (CSuplMessageBase::ESuplEnd == suplMessage->MessageType()))
		{
		
 		CSuplEnd* suplEndPtr = static_cast <CSuplEnd*>(suplMessage);
 		
 		// make sure the session id matches the one received in earlier messages
		CSuplSessionId *sessId = CSuplSessionId::NewLC();
		suplEndPtr->GetSessionId(*sessId);
		
		// Compare only the SET session ID as this may be the first message we get from
		// the SLP and the SLP session ID is not yet known.
		CSuplSetSessionId* receivedSetSessId = (*(iMachine.MessageSessionId())).iSetSessionId;
		CSuplSetSessionId* storedSetSessId = (*sessId).iSetSessionId;
		if (! ((*receivedSetSessId)== (*storedSetSessId)))
			{
			iMachine.CancelMachine(CSuplFsmSessionBase::ECancelSuplProt,CSuplFsmSessionBase::EReasonInvalidSessId);
			// Store received session Id for sending later in SUPL END as per SUPL specification
			CleanupStack::Pop(sessId);
			iMachine.SetMessageSessionId(sessId); // ownership transferred
			delete suplEndPtr;
			SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromPositionReceivedStateL() End\n");
			return actionsTaken;
			}
		else
			{
			CleanupStack::PopAndDestroy(sessId);
			}
 
		// There must be a position in the SUPL END
		ASSERT(suplEndPtr->PositionPresent());
		if (suplEndPtr->PositionPresent())
			{
 			suplEndPtr->GetPosition(position);
 			
			// Use position to build a TPositionInfo object
			// and send it to LBS
			TPositionInfo pos;
			pos.SetPosition(position);
			const TPositionModuleId id = {KSuplv1UidValue};
			pos.SetModuleId(id);
			pos.SetUpdateType(EPositionUpdateGeneral);
			pos.SetPositionModeReason(EPositionModeReasonNone);
			
			// If the position has been received and no positioning session
			// has taken place, then the position is the reference location
			// calculated based on cell-id.
			if (!iMachine.PosSessionConducted())
				{
				pos.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);	
				}
			else
				{
				// Position calculated in Terminal Assisted mode unless the 
				// SUPL END has an error code, in which case the position is
				// the reference location.
				if(suplEndPtr->StatusCodePresent())
					{
					pos.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);	
					}					
				else
					{
					pos.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
					}
				}

			iMachine.Observer().Gateway().NetworkLocationInd(iMachine.LbsSessionId(), pos);
 
			actionsTaken = ETrue;
			}
			
		// SUPL POS message no longer needed
		delete suplEndPtr;
		}
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromPositionReceivedStateL() End\n");
	return actionsTaken;
	}


/** Actions on entering state EStateSuplSessionEnded

Send a SUPL END message to the SUPL server. This state is reached
after the state machine was cancelled for some reason.
*/
TBool CSuplStateHandlerBase::EntryActionsFromSuplSessionEndedStateL()
	{
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromSuplSessionEndedStateL() Begin\n");
	TBool verRequired = EFalse;
	TBool statusCodeRequired = ETrue;
	// Send a SUPL END to the SLP. Most likely reason to be here is 
	// that the machine is cancelling due to explicit client cancel, a
	// client privacy rejection, an error or a timeout. If so, the
	// cancel source and reason can be taken from the event store.
	//
	// The only other use case where we end up in here is for MTLRs
	// where the SUPL INIT has specified "no Position".
	
	
	// Take cancel source and reason from the event store
	CSuplFsmSessionBase::TCancelSource source;
	CSuplFsmSessionBase::TCancelReason reason;
	iMachine.RetrieveCancelInfo(source,reason);
		
	// work out the reason code to set in a SUPL END
	TSuplStatusCode statusCode = ESuplStatusUnspecified;
	switch (source)
		{
		case CSuplFsmSessionBase::ECancelNone:
			// Not here after a cancel...assuming
			// there has been a SUPL INIT with "no position"
			// and we are here to send a SUPL END with privacy 
			// acceptance.

			verRequired = ETrue;
			statusCodeRequired = ETrue;
			statusCode = ESuplStatusConsentGrantedByUser;

			break;
		case CSuplFsmSessionBase::ECancelNetwork:
			
			switch (reason)
				{
				case CSuplFsmSessionBase::EReasonInsuficcientSecurity:
					// no matching supl status code
					statusCode = ESuplStatusUnspecified;
					break;
				// Since the connection manager performs the decoding
				// of received SUPL messages, all of the errors in decoding
				// show as having ECancelNetwork source, hence:
				case CSuplFsmSessionBase::EReasonParsingError:
					statusCode = ESuplStatusProtocolError;
					break;
				case CSuplFsmSessionBase::EReasonDataMissing:
					statusCode = ESuplStatusDataMissing;
					break;
				case CSuplFsmSessionBase::EReasonUnexpectedDataValue:
					statusCode = ESuplStatusUnexpectedDataValue;
					break;
				default:
					statusCode = ESuplStatusUnspecified;
					break;
				}
			break;

		case CSuplFsmSessionBase::ECancelPosProt:
			statusCode = ESuplStatusPosMethodFailure;
			break;

		case CSuplFsmSessionBase::ECancelSuplProt:
			switch (reason)
				{
				case CSuplFsmSessionBase::EReasonMethodMismatch:
					statusCode = ESuplStatusPosMethodMismatch;
					break;			
				case CSuplFsmSessionBase::EReasonInvalidSessId:
					statusCode = ESuplStatusInvalidSessionId;
					break;
				case CSuplFsmSessionBase::EReasonUnexpectedMessage:
					statusCode = ESuplStatusUnexpectedMessage;
					break;
				default:
					statusCode = ESuplStatusUnspecified;
					break;
				}
			break;		

		case CSuplFsmSessionBase::ECancelClient:
			// no matching status code in SUPL
			if (CSuplFsmSessionBase:: EReasonPrivacyRejected == reason)
				{
				statusCode = ESuplStatusConsentDeniedByUser;
				verRequired = ETrue;					
				}
			else if (CSuplFsmSessionBase::EReasonFutilePosCalc == reason)
				{
				statusCode = ESuplStatusPosMethodFailure;
				}
			else 
				{
				statusCode = ESuplStatusUnspecified;	
				}
			break;

		case CSuplFsmSessionBase::ECancelClosing:
		default:
			//No other source of cancel should result in this method being called
			ASSERT(EFalse);
			break;
		}
	
	// Build and send a SUPL END 
	CSuplEnd* suplEndPtr = CSuplEnd::NewLC(ETrue);
 
 	if (statusCodeRequired)
	 	{
		suplEndPtr->SetStatusCode(statusCode);	 		
	 	}
	
	if (verRequired)
		{
		// Set the Ver field in SUPL END
		TBuf8<8> ver;
		iMachine.RetrieveVer(ver);
		User::LeaveIfError(suplEndPtr->SetVer(ver));
		}
	
	// Set the session Id
	CSuplSessionId* msgId = iMachine.MessageSessionId();
	suplEndPtr->SetSessionId(*msgId);	
	
	CleanupStack::Pop(suplEndPtr);
	iMachine.Observer().ConnectionManager().SendMessage(suplEndPtr, iMachine.SessionId().SessionNum());
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromSuplSessionEndedStateL() End\n");
	return ETrue;
	}


/** Actions on entering state EStateLbsSessionEnded

This method informs LBS that the session has ended.
*/
TBool CSuplStateHandlerBase::EntryActionsFromLbsSessionEndedStateL()
	{
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromLbsSessionEndedStateL() Begin\n");
	TBool actionsTaken = EFalse;
	
	TInt sessionCompletionReason = KErrNone; // reason sent to LBS

	// Check the event store to see in the completion reason
	// has to be different from KErrNone
	// Take cancel source and reason from the event store.
	//
	CSuplFsmSessionBase::TCancelSource cancelSource;
	CSuplFsmSessionBase::TCancelReason cancelReason;
	iMachine.RetrieveCancelInfo(cancelSource,cancelReason);
	
	if (cancelSource != CSuplFsmSessionBase::ECancelNone)
		{
		// Session was cancelled.  Error code is KErrCancel
		// except if network connection is down (KErrDisconnected).
		if ((cancelSource == CSuplFsmSessionBase::ECancelNetwork)
			&& (cancelReason == CSuplFsmSessionBase::EReasonDisconnected))
			{
			sessionCompletionReason = KErrDisconnected;
			}
		else
			{
			sessionCompletionReason = KErrCancel;
			}
		actionsTaken = ETrue;
		}
		
	// Check if the session was terminated after an unexpected 
	// SUPL END or a SUPL END with reason code indicating an error
	else if (iMachine.IsUnexpectedSuplEndStored())
		{
		// Complete with KErrPositionProtocolErr
		sessionCompletionReason = KErrPositionProtocolErr;
		actionsTaken = ETrue;
		}
	else
		{
		// If the session has not been cancelled, there could be a
		// SUPL END message in the store (there won't be one if the
		// SUPL END carried position information as it is dealt with
		// in a previous state).
		const CSuplMessageBase* suplEnd = iMachine.RetrieveSuplMessage();

		if ((suplEnd != NULL) && (CSuplMessageBase::ESuplEnd == (const_cast <CSuplMessageBase*>(suplEnd))->MessageType()))
			{
			delete suplEnd;
			}
			
		sessionCompletionReason = KErrNone;
		actionsTaken = ETrue;
		}

	if (actionsTaken)
		{
		iMachine.Observer().Gateway().SessionCompleteInd(iMachine.LbsSessionId(), sessionCompletionReason);
		}
	iMachine.SetSessionInProgress(EFalse);
	SUPLLOG(ELogP1, "CSuplStateHandlerBase::EntryActionsFromLbsSessionEndedStateL() End\n");
	return actionsTaken;
	}


/** Utility method to convert a numeric string to Binary Coded Decimal string

Taken from section 8.3 of the OMA SUPL document UserPlane Location Protocol OMA-TS-ULP-V1-0-20070615-A
The BCD string is stored in an 8 octet buffer with 2 digits per octet.
Each digit is encoded 0000 to 1001 ( 0 to 9).
Bits 8765 of octet n encoding digit 2n
Bits 4321 of octet n encoding digit 2(n-1)+1
Unused digits in the string are filled with 0xFF

*/
void CSuplStateHandlerBase::ConvertToBcd(TDes8& aBcdString)
	{
	
	const TUint8 KMaxBcdStringLength = 8;
	const TUint8 fillBuffer[KMaxBcdStringLength] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	
	TBool LeastSignificantNibble(ETrue);
	TInt OctetCount = 0;
	TChar nextChar;
	TUint8 nextOctet = 0;
	TLex lexString(*iMachine.Msisdn());
	
	aBcdString.Zero();
	while (!lexString.Eos())
		{
		nextChar = lexString.Get();
		if (nextChar.IsDigit())  // ignore any non-numeric characters
			{
			if (LeastSignificantNibble)
				{
				nextOctet = nextChar.GetNumericValue(); //store digit in L.S. nibble
				LeastSignificantNibble = EFalse;
				}
			else
				{
				nextOctet |= (nextChar.GetNumericValue()<<4); //store digit in M.S. nibble
				aBcdString.Append(nextOctet);
				OctetCount++;
				LeastSignificantNibble = ETrue;
				}
			}
		}
	
	// All of the decimal string has been converted - deal with an half filled octet
	if (!LeastSignificantNibble)
		{
		nextOctet |= 0xF0;
		aBcdString.Append(nextOctet);
		OctetCount++;
		}
	
	// Fill remainder of string with 0xFF values
	aBcdString.Append(fillBuffer, (KMaxBcdStringLength-OctetCount));
	
	}

