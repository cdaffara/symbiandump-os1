/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file XmlParsing.h
*/

#ifndef _XMLPARSING_H_
#define _XMLPARSING_H_

#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>

#include <elements/sd_mintercept.h>



static const TUint KXmlParsingBufferMaxLength = 128;
typedef TBuf8<KXmlParsingBufferMaxLength> TXmlTempBuf8;
_LIT8(KXmlMimeType, "text/xml");

class TLookupTableEntry8
	{
public:
	TPtrC8 iIdentifier;
	TUint iValue;
	};

class CExpressionXmlHandler : public Xml::MContentHandler
	{
	// MContentHandler
protected:
	void OnStartDocumentL(const Xml::RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/) {}
	void OnEndDocumentL(TInt /*aErrorCode*/) {}
	void OnStartElementL(const Xml::RTagInfo& /*aElement*/, const Xml::RAttributeArray& /*aAttributes*/, TInt /*aErrorCode*/) {}
	void OnEndElementL(const Xml::RTagInfo& /*aElement*/, TInt /*aErrorCode*/) {}
	void OnContentL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/){}
	void OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/){}
	void OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/){}
	void OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/){}
	void OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/) {}
	void OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/){}
	void OnExtensionL(const RString& /*aData*/, TInt /*aToken*/, TInt /*aErrorCode*/){}
	void OnError(TInt /*aErrorCode*/){}
	TAny* GetExtendedInterface(const TInt32 /*aUid*/){return NULL;}
	void FetchAttributeValueByNameL(
		const Xml::RAttributeArray& aAttributes,
		const TDesC8& aAttributeTag,
		TDes8& aAttributeValue);

	enum TLookupTableType {EUidTable, EMessageIdTable, ETierIdTable, EAccessPointIdTable, EErrorCodeTable, EActionTypeTable, ERealmIdTable};

	void ParseIdentifierValueL(const TDesC8& aBuffer, TDes8& aIdentifier);
	void ParseHexValueL(const TDesC8& aBuffer, TUint& aHexValue);
	void LookupValueByNameL(TLookupTableType aLookupType, const TDesC8& aName, TUint& aValue);
	void FetchValueOrNamedValueL(TLookupTableType aType, const TDesC8& aBuffer, TUint& aValue);
	};

class CActionXmlHandler : public CExpressionXmlHandler
	{
	public:
		CActionXmlHandler();
		CActionXmlHandler(Den::TAction& aParsedAction) : iAction(aParsedAction) {}
		virtual ~CActionXmlHandler() {}

	private:
		// From MContentHandler
		void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);

		void ParseActionTypeAttributeL(const Xml::RAttributeArray& aAttributes, Den::TAction::TActionType& aActionType);
		void ParseActionErrorCodeAttributeL(const Xml::RAttributeArray& aAttributes, TInt& aError);

		Den::TAction& iAction;
	};

class CEventExpressionXmlHandler : public CExpressionXmlHandler
	{

	public:
		CEventExpressionXmlHandler();
		CEventExpressionXmlHandler(Den::TEventExpression& aEventExpression) :
			iEventExpression(aEventExpression),
			iParseState(EEventElement) {}
		virtual ~CEventExpressionXmlHandler() {}

	private:
		// From MContentHandler
		void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
		void OnEndElementL(const Xml::RTagInfo& /*aElement*/, TInt /*aErrorCode*/);

		void ParseProviderInfoAttributeL(const Xml::RAttributeArray& aAttributes, TUid& aTierId, TUint& aAPId);
		void ParseUidAttributeL(const Xml::RAttributeArray& aAttributes, TUid& aUid);
		void ParseNodeTagAttributeL(const Xml::RAttributeArray& aAttributes, TDes8& aTagToApply);
		void ParseNodeTagToApplyAttributeL(const Xml::RAttributeArray& aAttributes, TDes8& aTagToApply);

		void ParseNodeExpressionElementL(
			const Xml::RTagInfo& aElementName,
			const Xml::RAttributeArray& aAttributes,
			Den::TNodeExpression& aNodeExpression);

		void ParseMessageElementL(
			const Xml::RTagInfo& aElementName,
			const Xml::RAttributeArray& aAttributes,
			Den::TMsgExpression& aMessage);

		void ParseNamedNumericAttributeL(
			const TDesC8& aAttribName,
			const Xml::RAttributeArray& aAttributes,
			TLookupTableType aTableId,
			TUint& aParsedValue);

		Den::TNodeExpression iSender;
		Den::TMsgExpression iMessage;
		Den::TNodeExpression iReceiver;
		Den::TEventExpression& iEventExpression;

		enum TParseState {EEventElement, ESenderNodeElement, EMsgElement, EReceiverNodeElement, EDone};
		TParseState iParseState;
	};

#endif // _XMLPARSING_H_

