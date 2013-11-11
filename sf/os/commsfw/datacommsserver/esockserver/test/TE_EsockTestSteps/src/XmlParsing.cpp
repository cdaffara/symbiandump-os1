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
//

/**
 @file XmlParsing.cpp
*/

#include "XmlParsing.h"
#include <comms-infras/ss_msgintercept.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestXmlPrsn, "ESockTestXmlPrsn");
#endif

using namespace ESockDebug;
using namespace Den;

TLookupTableEntry8 KUidLookupTable[] =
	{
		// MCPRs
		{_S8("NetworkMCPR"), 0x10274547},
		{_S8("IpProtoMCPR"), 0x10281DEE},
		//{_S8("AgentMCPR"), 0xFFFFFFFF}, // Not instantiated
		{_S8("PPPMCPR"), 0x10281DEC},
		{_S8("EthMCPR"), 0x10281DFC},
		{_S8("RawIpMCPR"), 0x10281e00},
		{_S8("TunnelMCPR"), 0x10281E02},
		{_S8("PdpMCPR"), 0x102822F0},
		{_S8("DummyMCPR"), 0x10281E04},

		// CPRs
		{_S8("AgentCPR"), 0x10281DE8},
		//{_S8("DummyCPR"), 0xFFFFFFFF}, // No ReceivedL()
		{_S8("IpProtoCPR"), 0x10281DD3},
		{_S8("IpCPR"), 0x102070EF},

		// SCPRs
		{_S8("DummySCPR"), 0x1028302E},
		{_S8("AgentSCPR"), 0x10281DEA},
		{_S8("PPPSCPR"), 0x102822FC},
		{_S8("IpProtoSCPR"), 0x10281DD1},
		{_S8("PdpSCPR"), 0x102822E0},
		{_S8("IpDefaultSCPR"), 0x10204308},
		{_S8("IpSCPR"), 0x10204308},

		// Non-provider based nodes
		{_S8("Socket"), KSocketNodeUid},
		{_S8("Conn"), KConnectionNodeUid},
		{_S8("SubConn"), KSubConnectionNodeUid},
		{_S8("HostRslv"), KHostResolverNodeUid},
	};

#define MESSAGE_ID_LOOKUP_TABLE_ENTRY(messageid, idvalue) {_S8(#messageid), (idvalue)}

TLookupTableEntry8 KMessageIdLookupTable[] =
	{
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFNull, 1),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStart, 2),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStop, 3),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStarted, 4),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStopped, 5),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFGoneDown, 6),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFJoinComplete, 7),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFLeaveComplete, 8),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFProvisionConfig, 9),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFBindToRequest, 10),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFBindTo, 11),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFBindToComplete, 12),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFReBindTo, 13),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFReConnect, 14),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFReBindComplete, 15),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCommsBinderRequest, 16),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCommsBinderResponse, 17),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFBearer, 18),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFNoBearer, 19),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFRejoin, 20),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFRejoinComplete, 21),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFRejoinDataClientRequest, 22),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCancel, 23),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFError, 24),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFDestroy, 25),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCreateDataClient, 26),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStart, 27),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStarted, 28),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStop, 29),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStopped, 30),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFDataClientJoinRequest, 31),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFDataClientJoined, 32),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFDataClientGoneDown, 33),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFDataClientRouted, 34),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFDataClientIdle, 35),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCtrlClientJoinRequest, 36),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCtrlClientJoined, 37),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFErrorRecoveryRequest, 38),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFErrorRecoveryResponse, 39),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFClientLeavingRequest, 40),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFClientLeaving, 41),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFApplyRequest, 42),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFApplyResponse, 43),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFStateChange, 44),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFServiceChangeNotification, 45),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFRegisterHookError, 46),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFAttachRequest, 47),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFFindOrCreateFactoryObject, 48),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFFactoryObjectCreated, 49),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCreateCSR, 50),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFCSRCreated, 51),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFSelect, 52),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFSelectComplete, 53),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFImplicitFlowRequest, 54),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFConnFlowRequest, 55),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFParamsRequest, 56),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFParamsResponse, 57),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFSubConnFlowRequest, 58),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFSubConnNotification, 59),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFFindOrCreateTM, 60),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFTierStatusQuery, 61),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFTierStatus, 62),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFTierStatusCancel, 63),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFTierNotificationRegistration, 64),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFTierNotification, 65),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFTierNotificationCancel, 66),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFAvailabilityNotificationRegistration, 67),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFAvailabilityNotification, 68),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFDataMonitoringNotification, 69),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFFlowRequest, 70),
	MESSAGE_ID_LOOKUP_TABLE_ENTRY(ECFSubSess, 71)
	};

TLookupTableEntry8 KTiedIdLookupTable[] =
	{
		{_S8("_dummyTier"), 0x12345678} // TODO_CDG what are these values
	};

TLookupTableEntry8 KAccessPointIdLookupTable[] =
	{
		{_S8("_dummyAccessPoint"), 0x12345678} // TODO_CDG what are these values
	};


#define ERROR_CODE_LOOKUP_TABLE_ENTRY(errorcode) {_S8(#errorcode), static_cast<TUint>(errorcode)}
TLookupTableEntry8 KErrorCodeLookupTable[] =
	{
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrNone),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrNotFound),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrGeneral),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrCancel),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrNoMemory),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrNotSupported),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrArgument),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrAbort),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrBadHandle),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrAlreadyExists),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrInUse),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrServerTerminated),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrServerBusy),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrNotReady),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrAccessDenied),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrCouldNotConnect),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrCouldNotDisconnect),
	ERROR_CODE_LOOKUP_TABLE_ENTRY(KErrDisconnected)
	};

TLookupTableEntry8 KActionTypeLookupTable[] =
	{
		{_S8("None"), TAction::ENoAction},
		{_S8("Leave"), TAction::ELeaveAction},
		{_S8("Panic"), TAction::EPanicAction}
	};

TLookupTableEntry8 KRealmIdLookupTable[] =
	{
		{_S8("Zero"), 0}
	};

#define LOOKUP_TABLE_LENGTH(tableinstance) (sizeof(tableinstance) / sizeof(TLookupTableEntry8))

void CExpressionXmlHandler::LookupValueByNameL(TLookupTableType aLookupType, const TDesC8& aName, TUint& aValue)
	{
		TLookupTableEntry8* table = NULL;
		TUint tableLength = 0;

		// Find the table first
		switch(aLookupType)
			{
			case EUidTable:
				table = KUidLookupTable;
				tableLength = LOOKUP_TABLE_LENGTH(KUidLookupTable);
				break;

			case EMessageIdTable:
				table = KMessageIdLookupTable;
				tableLength = LOOKUP_TABLE_LENGTH(KMessageIdLookupTable);
				break;

			case ETierIdTable:
				table = KTiedIdLookupTable;
				tableLength = LOOKUP_TABLE_LENGTH(KTiedIdLookupTable);
				break;

			case EAccessPointIdTable:
				table = KAccessPointIdLookupTable;
				tableLength = LOOKUP_TABLE_LENGTH(KAccessPointIdLookupTable);
				break;

			case EErrorCodeTable:
				table = KErrorCodeLookupTable;
				tableLength = LOOKUP_TABLE_LENGTH(KErrorCodeLookupTable);
				break;

			case EActionTypeTable:
				table = KActionTypeLookupTable;
				tableLength = LOOKUP_TABLE_LENGTH(KActionTypeLookupTable);
				break;

			case ERealmIdTable:
				table = KRealmIdLookupTable;
				tableLength = LOOKUP_TABLE_LENGTH(KRealmIdLookupTable);
				break;

			default:
                User::Panic(_L("Illegal name lookup access"), KErrArgument);
				break;
			}

		TLookupTableEntry8* tablePtr = table;
		for(TUint index = 0; index < tableLength; index++)
			{
			// Simply return the first match
			if(tablePtr[index].iIdentifier == aName)
				{
				aValue = tablePtr[index].iValue;
				return;
				}
			}

		// Couldn't find it
		User::Leave(KErrNotFound);
	}

void CExpressionXmlHandler::ParseIdentifierValueL(const TDesC8& aBuffer, TDes8& aIdentifier)
	{
	TLex8 lexer(aBuffer);

	// Skip whitespace to start with
	lexer.SkipSpace();

	// First char must be "_" or alpha
	if(!lexer.Peek().IsAlpha() && !(lexer.Peek() == TChar('_')))
		User::Leave(KErrNotFound);
	else
		lexer.Mark();

	// All and only identifier chararacters (alpha, digit and underscore)
	while(lexer.Peek().IsDigit() || lexer.Peek().IsAlpha() || (lexer.Peek() == TChar('_')))
		{
		lexer.Inc();
		}

	// Fetch the token (identifier) we marked out
	aIdentifier.Copy(lexer.MarkedToken());
	}

void CExpressionXmlHandler::FetchAttributeValueByNameL(
	const Xml::RAttributeArray& aAttributes,
	const TDesC8& aAttributeTag,
	TDes8& aAttributeValue)
	{
	// Step thru the array of attributes return the first found with name specified
	for(TUint index = 0; index < aAttributes.Count(); index++)
		{
		if(aAttributes[index].Attribute().LocalName().DesC() == aAttributeTag)
			{
			// Found our attribute so copy its value as expected
			aAttributeValue.Copy(aAttributes[index].Value().DesC());
			return;
			}
		}

	// Didn't find the attribute we expected to
	User::Leave(KErrNotFound);
	}

void CExpressionXmlHandler::FetchValueOrNamedValueL(TLookupTableType aType, const TDesC8& aBuffer, TUint& aValue)
	{
	_LIT8(KHexPrefix, "0x");

	// First see if this is a hex value
	TLex8 lexer(aBuffer);
	lexer.SkipSpaceAndMark();

	if(lexer.Remainder().Length() > 2)
		{
		lexer.Inc(2);
		if(lexer.MarkedToken() == KHexPrefix)
			{
			// If we had the hex prefix but no value then something wrong
			User::LeaveIfError(lexer.Val(aValue, EHex));

			// Otherwise we got our value so out of here
			return;
			}
		}

	// Maybe its a decimal value (and allow for it to be signed)
	lexer.UnGetToMark();
	TInt error = lexer.Val(reinterpret_cast<TInt&>(aValue));

	// No? Then perhaps its a named value
	if(error != KErrNone)
		{
		TXmlTempBuf8 identifier;
		TRAP(error, ParseIdentifierValueL(aBuffer, identifier));
		if(error == KErrNone)
			{
			// Lookup identifier
			TRAP(error, LookupValueByNameL(aType, identifier, aValue));
			if(error != KErrNone)
				{
				User::Leave(KErrNotFound);
				}
			}
		else
			{
			User::Leave(KErrNotFound);
			}
		}
	}

// CActionXmlHandler
//------------------
_LIT8(KActionElementName, "Action");
_LIT8(KActionTypeAttributeName, "Type");
_LIT8(KActionErrorCodeAttributeName, "ErrorCode");

void CActionXmlHandler::ParseActionTypeAttributeL(const Xml::RAttributeArray& aAttributes, TAction::TActionType& aActionType)
	{
	TXmlTempBuf8 tempBuffer;
	FetchAttributeValueByNameL(aAttributes, KActionTypeAttributeName, tempBuffer);

	// Parse out the attribute in to the action's type value
	TUint actionTypeValue;
	FetchValueOrNamedValueL(EActionTypeTable, tempBuffer, actionTypeValue);
	aActionType = static_cast<TAction::TActionType>(actionTypeValue);
	}

void CActionXmlHandler::ParseActionErrorCodeAttributeL(const Xml::RAttributeArray& aAttributes, TInt& aError)
	{
	TXmlTempBuf8 tempBuffer;
	FetchAttributeValueByNameL(aAttributes, KActionErrorCodeAttributeName, tempBuffer);

	// Parse out the attribute in to the action's error code
	TUint errorCodeValue;
	FetchValueOrNamedValueL(EErrorCodeTable, tempBuffer, errorCodeValue);
	aError = static_cast<TInt>(errorCodeValue);
	}


void CActionXmlHandler::OnStartElementL(
		const Xml::RTagInfo& aElementName,
		const Xml::RAttributeArray& aAttributes,
		TInt /*aErrorCode*/)
	{
	// Check we are on the correct element first of all
	if (aElementName.LocalName().DesC() == KActionElementName)
		{
		// Parse the "Type" attribute. Must have this attribute so let if leave here if not found
		TAction::TActionType actionType;
		ParseActionTypeAttributeL(aAttributes, actionType);

		// Parse the "ErrorCode" attribute. Must have this attribute so let if leave here if not found
		TInt errorCode;
		ParseActionErrorCodeAttributeL(aAttributes, errorCode);

		// Got the attributes we need so build the action
		iAction = TAction(actionType, errorCode);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}


// CEventExpressionXmlHandler
//---------------------------

void CEventExpressionXmlHandler::ParseNamedNumericAttributeL(
		const TDesC8& aAttribName,
		const Xml::RAttributeArray& aAttributes,
		TLookupTableType aTableId,
		TUint& aParsedValue)
	{
	TXmlTempBuf8 tempBuffer;
	FetchAttributeValueByNameL(aAttributes, aAttribName, tempBuffer);
	FetchValueOrNamedValueL(aTableId, tempBuffer, aParsedValue);
	}

void CEventExpressionXmlHandler::ParseMessageElementL(
		const Xml::RTagInfo& aElementName,
		const Xml::RAttributeArray& aAttributes,
		TMsgExpression& aMessage)
	{
	_LIT8(KMsgElementName, "Msg");
	_LIT8(KMsgIdAttributeName, "Id");
	_LIT8(KRealmAttributeName, "Realm");

	// Check we have the element name first
	if (aElementName.LocalName().DesC() == KMsgElementName)
		{
		// Now check we have the "Id" attribute which is mandatory
		TUint msgId;
		ParseNamedNumericAttributeL(KMsgIdAttributeName, aAttributes, EMessageIdTable, msgId);

		// And check for a realm which is optional (default to esock)
		TUint realm = 0x101F7482; // esocksvr uid
		TRAPD(error, ParseNamedNumericAttributeL(KRealmAttributeName, aAttributes, ERealmIdTable, realm));
		if(error != KErrNone && error != KErrNotFound)
			User::Leave(error);

		aMessage = TMsgExpression(msgId, realm);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

_LIT8(KNodeElementName, "Node");
_LIT8(KNodeTagToApplyAttributeName, "ApplyTag");
_LIT8(KProviderInfoAttributeName, "ProviderInfo");
_LIT8(KUidAttributeName, "Uid");
_LIT8(KNodeTagAttributeName, "Tag");

void CEventExpressionXmlHandler::ParseNodeExpressionElementL(
		const Xml::RTagInfo& aElementName,
		const Xml::RAttributeArray& aAttributes,
		TNodeExpression& aNodeExpression)
	{
	// Check we have the element itself first
	if (aElementName.LocalName().DesC() == KNodeElementName)
		{
		// First look to see if we have an "ApplyTag" attribute
		// its not mandatory so if not found then no bother
		TInt error;
		TNodeTag tagToApplyOnMatch(KNullDesC8);
		TRAP(error, ParseNodeTagToApplyAttributeL(aAttributes, tagToApplyOnMatch));
		if(error != KErrNone && error != KErrNotFound)
			User::Leave(error);

		TUid tierId;
		TUint accessPoint;
		TRAP(error, ParseProviderInfoAttributeL(aAttributes, tierId, accessPoint));
		if(error == KErrNone)
			{
			//TODO: TNodeExpression need to be based on a more dynamic mechanism
			aNodeExpression = TNodeExpression(tierId, /*accessPoint,*/ tagToApplyOnMatch);
			}
		else if(error == KErrNotFound)
			{
			TUid uid;
			TRAP(error, ParseUidAttributeL(aAttributes, uid));
			if(error == KErrNone)
				{
				aNodeExpression = TNodeExpression(uid, tagToApplyOnMatch);
				}
			else if (error == KErrNotFound)
				{
				TNodeTag nodeMatchTag;
				TRAP(error, ParseNodeTagAttributeL(aAttributes, nodeMatchTag));
				if(error == KErrNone)
					{
					aNodeExpression = TNodeExpression(nodeMatchTag);
					}
				else
					{
					User::Leave(error);
					}
				}
			}
		else
			{
			// Any other error then just get out of here
			User::Leave(error);
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CEventExpressionXmlHandler::OnStartElementL(
		const Xml::RTagInfo& aElementName,
		const Xml::RAttributeArray& aAttributes,
		TInt /*aErrorCode*/)
	{
	// Check which element we are supposed to be parsing
	switch(iParseState)
		{
		case EEventElement:
			iParseState = ESenderNodeElement;
			break;

		case ESenderNodeElement:
			ParseNodeExpressionElementL(aElementName, aAttributes, iSender);
			break;

		case EMsgElement:
			ParseMessageElementL(aElementName, aAttributes, iMessage);
			break;

		case EReceiverNodeElement:
			ParseNodeExpressionElementL(aElementName, aAttributes, iReceiver);
			break;

		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestXmlPrsn, 1));
			break;
		}
	}

void CEventExpressionXmlHandler::OnEndElementL(const Xml::RTagInfo& /*aElement*/, TInt /*aErrorCode*/)
	{
	// Check which element we are supposed to be parsing
	switch(iParseState)
		{
		case EEventElement:
			// This is the last element we were looking for so build our event expression
			iEventExpression = TEventExpression(iSender, iMessage, iReceiver);
			iParseState = EDone;
			break;

		case ESenderNodeElement:
			iParseState = EMsgElement;
			break;

		case EMsgElement:
			iParseState = EReceiverNodeElement;
			break;

		case EReceiverNodeElement:
			iParseState = EEventElement;
			break;

		case EDone:
			break;

		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestXmlPrsn, 2));
			break;
		}
	}

void CEventExpressionXmlHandler::ParseProviderInfoAttributeL(
		const Xml::RAttributeArray& aAttributes,
		TUid& aTierId,
		TUint& aAPId)
	{
	TXmlTempBuf8 tempBuffer;
	FetchAttributeValueByNameL(aAttributes, KProviderInfoAttributeName, tempBuffer);

	TPtrC8 tierIdentifier;
	TPtrC8 accessPointIdentifier;

	// Provider info is several fields
	// Split in to comma separated fields
	TLex8 lexer(tempBuffer);
	lexer.SkipSpace();
	lexer.Mark();
	while((lexer.Peek() != TChar(',')) && !lexer.Eos())
		{
		lexer.Inc();
		}

	// If indeed we have just finished the first chunk
	if(lexer.Peek() == TChar(','))
		{
		// Grab the first chunk and slurp the comma
		tierIdentifier.Set(lexer.MarkedToken());
		lexer.Inc();

		// Grab the rest
		if(lexer.Eos())
			{
			User::Leave(KErrArgument);
			}
		else
			{
			lexer.Mark();
			accessPointIdentifier.Set(lexer.Remainder());
			}
		}
	else
		{
		User::Leave(KErrArgument);
		}

	// Now parse out the values themselves
	TUint tierIdValue;
	FetchValueOrNamedValueL(ETierIdTable, tierIdentifier, tierIdValue);
	FetchValueOrNamedValueL(EAccessPointIdTable, accessPointIdentifier, aAPId);

	aTierId = TUid::Uid(tierIdValue);
	}

void CEventExpressionXmlHandler::ParseUidAttributeL(
		const Xml::RAttributeArray& aAttributes,
		TUid& aUid)
	{
	TXmlTempBuf8 tempBuffer;
	FetchAttributeValueByNameL(aAttributes, KUidAttributeName, tempBuffer);

	TUint fetchedValue;
	FetchValueOrNamedValueL(EUidTable, tempBuffer, fetchedValue);
	aUid = TUid::Uid(fetchedValue);
	}

void CEventExpressionXmlHandler::ParseNodeTagAttributeL(const Xml::RAttributeArray& aAttributes, TDes8& aTagToApply)
	{
	TXmlTempBuf8 tempBuffer;
	FetchAttributeValueByNameL(aAttributes, KNodeTagAttributeName, tempBuffer);
	aTagToApply.Copy(tempBuffer);
	}

void CEventExpressionXmlHandler::ParseNodeTagToApplyAttributeL(const Xml::RAttributeArray& aAttributes, TDes8& aTagToApply)
	{
	TXmlTempBuf8 tempBuffer;
	FetchAttributeValueByNameL(aAttributes, KNodeTagToApplyAttributeName, tempBuffer);
	aTagToApply.Copy(tempBuffer);
	}


