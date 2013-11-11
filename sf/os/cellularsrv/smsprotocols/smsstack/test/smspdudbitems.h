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

/**
 @file
 @internalComponent 
*/

#ifndef __SMS_PDU_DB_ITEMS_H__
#define __SMS_PDU_DB_ITEMS_H__

#include <e32base.h>
#include <etelmm.h>
#include "gsmupdu.h"
#include "gsmuetel.h"

class CTestConfigItem;
class CSmsMessage;
class CSmsBufferBase;

_LIT8(KSmsPduDbSubmit,			"submit");
_LIT8(KSmsPduDbSubmitConcat,	"submitconcat");
_LIT8(KSmsPduDbDeliver,			"deliver");
_LIT8(KSmsPduDbDeliverConcat,	"deliverconcat");
_LIT8(KSmsPduDbStatusReport,	"statusreport");
_LIT8(KSmsPduDbCommand,			"command");
_LIT8(KSmsPduDbMessage,			"message");
_LIT8(KSmsPduDbSubmitReport,	"submitreport");
_LIT8(KSmsPduDbDeliverReport,	"deliverreport");

const TInt KSmsPduDbDelimiter = ',';
const TInt KSmsPduDbTagStart = '<';
const TInt KSmsPduDbTagEnd = '>';
const TInt KSmsPduDbTagDelimitier = ';';

class CSmsPduDbBase : public CBase
	{
	public:

		enum
			{
			ESmsPduDbId = 0
			};

	public:

		IMPORT_C ~CSmsPduDbBase();
		HBufC* DescriptionLC() const;

	protected:

		inline CSmsPduDbBase();
		virtual void DecodeL(RFs& aFs, const CTestConfigItem& aItem)=0; //< Constructs the CSmsMessage

	public:

		TInt iId;
		TPtrC8 iDescription;
		CSmsMessage* iSmsMessage;
		TInt iSmsMessageError; //< Error if construction of CSmsMessage failed
	};

class CSmsPduDbPdu : public CSmsPduDbBase
	{
	private:

		enum TSmsPduDbPduPosition
			{
			EPdu = ESmsPduDbId + 1,
			EPduError,
			EPduDesc
			};

	public:

		IMPORT_C static CSmsPduDbPdu* NewL(RFs& aFs, const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aType);
		IMPORT_C void GetHexPdu(TDes8& aHexPdu) const;

	protected:

		inline CSmsPduDbPdu(CSmsPDU::TSmsPDUType aType);
		void DecodeL(RFs& aFs, const CTestConfigItem& aItem);
		void ConstructSmsMessageL(RFs& aFs);
		void SetPduL(const TDesC8& aHexPdu);
		TBool IsMobileTerminatedL() const;

	public:

		CSmsPDU::TSmsPDUType iType;
		RMobileSmsMessaging::TMobileSmsGsmTpdu iPdu;
		TInt iDecodeError;
	};

class CSmsPduDbConcatSegment : public CSmsPduDbPdu
	{
	public:

		enum TSmsPduDbConcatPosition
			{
			EConcatSegment = ESmsPduDbId + 1,
			EConcatPdu,
			EConcatError,
			EConcatDesc
			};

	public:

		IMPORT_C static CSmsPduDbConcatSegment* NewL(RFs& aFs, const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aType);

	private:

		inline CSmsPduDbConcatSegment(CSmsPDU::TSmsPDUType aType);
		void DecodeL(RFs& aFs, const CTestConfigItem& aItem);

	public:

		TInt iSegment;
	};

class CSmsPduDbConcat : public CSmsPduDbBase
	{
	public:

		inline CSmsPduDbConcat();
		IMPORT_C ~CSmsPduDbConcat();
		IMPORT_C void DecodeL(RFs& aFs);

	private:

		void DecodeL(RFs&, const CTestConfigItem&) {};

	public:

		RPointerArray<CSmsPduDbConcatSegment> iSegments;
	};


class CSmsPduDbMessage : public CSmsPduDbPdu
	{
	private:

		enum TSmsPduDbMessagePosition
			{
			EMsgBody = ESmsPduDbId + 1,
			EMsgStatusReport,
			EMsgDCS,
			EMsgPID,
			EMsgDesc
			};

		enum TSmsPduDbUnicodeChar
			{
			EUnicodeChar = 0,
			EUnicodeCharRepeat = 1
			};

	public:

		IMPORT_C static CSmsPduDbMessage* NewL(RFs& aFs, const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aType);

	private:

		inline CSmsPduDbMessage(CSmsPDU::TSmsPDUType aType);
		void DecodeL(RFs& aFs, const CTestConfigItem& aItem);
		void SetDataCodingScheme(TInt aDCS);
		void SetProtocolIdentifier(TInt aPID);
		void SetBufferL(const TDesC8& aBody, CSmsBufferBase& aBuffer);
		void AppendToBufferL(const TDesC8& aItem, CSmsBufferBase& aBuffer);
		void ParseUnicodeL(const TDesC8& aUnicode, CSmsBufferBase& aBuffer);
		void ProcessUnicodeL(TLex8& aBody, CSmsBufferBase& aBuffer);

	public:

		CArrayFixFlat<TGsmSms> iPdus;	//< Pdus created by calling iSmsMessage->EncodeMessagePDUsL(). This is done in DecodeL()
		TInt iPdusError;				//< Error if iSmsMessage->EncodeMessagePDUsL() left
	};

#include "smspdudbitems.inl"

#endif
