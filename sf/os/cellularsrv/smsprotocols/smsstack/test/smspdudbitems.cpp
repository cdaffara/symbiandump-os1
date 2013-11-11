// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "smspdudbitems.h"
#include "Gsmumsg.h"
#include "gsmubuf.h"
#include <testconfigfileparser.h>


EXPORT_C CSmsPduDbConcatSegment* CSmsPduDbConcatSegment::NewL(RFs& aFs, const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aType)
	{
	CSmsPduDbConcatSegment* self = new (ELeave) CSmsPduDbConcatSegment(aType);
	CleanupStack::PushL(self);
	self->DecodeL(aFs, aItem);
	CleanupStack::Pop(self);
	return self;
	}



EXPORT_C CSmsPduDbMessage* CSmsPduDbMessage::NewL(RFs& aFs, const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aType)
	{
	CSmsPduDbMessage* pdu = new (ELeave) CSmsPduDbMessage(aType);
	CleanupStack::PushL(pdu);

	pdu->DecodeL(aFs, aItem);

	CleanupStack::Pop(pdu);

	return pdu;
	}





EXPORT_C CSmsPduDbPdu* CSmsPduDbPdu::NewL(RFs& aFs, const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aType)
	{
	CSmsPduDbPdu* self = new (ELeave) CSmsPduDbPdu(aType);
	CleanupStack::PushL(self);
	self->DecodeL(aFs, aItem);
	CleanupStack::Pop(self);
	return self;
	}



EXPORT_C CSmsPduDbBase::~CSmsPduDbBase()
	{
	delete iSmsMessage;
	}

EXPORT_C CSmsPduDbConcat::~CSmsPduDbConcat()
	{
	iSegments.ResetAndDestroy();
	}

void CSmsPduDbPdu::DecodeL(RFs& aFs, const CTestConfigItem& aItem)
	{
	const TDesC8& val = aItem.Value();

	User::LeaveIfError(CTestConfig::GetElement(val, KSmsPduDbDelimiter, CSmsPduDbBase::ESmsPduDbId, iId));

	TPtrC8 pdu;
	User::LeaveIfError(CTestConfig::GetElement(val, KSmsPduDbDelimiter, EPdu, pdu));
	SetPduL(pdu);

	const TInt err = CTestConfig::GetElement(val, KSmsPduDbDelimiter, EPduError, iDecodeError);
	if (err != KErrNone)
		{
		iDecodeError = KErrNone;
		}

	(void) CTestConfig::GetElement(val, KSmsPduDbDelimiter, EPduDesc, iDescription); //ignore error

	TRAP(iSmsMessageError, ConstructSmsMessageL(aFs));
	}

void CSmsPduDbPdu::ConstructSmsMessageL(RFs& aFs)
	{
	delete iSmsMessage;
	iSmsMessage = NULL;
	TGsmSms sms;
	sms.SetPdu(iPdu);
	const TBool mt = IsMobileTerminatedL();
	iSmsMessage = CSmsMessage::NewL(aFs, sms, CSmsBuffer::NewL(), EFalse, mt);
	}

void CSmsPduDbConcatSegment::DecodeL(RFs& aFs, const CTestConfigItem& aItem)
	{
	const TDesC8& val = aItem.Value();

	User::LeaveIfError(CTestConfig::GetElement(val, KSmsPduDbDelimiter, CSmsPduDbBase::ESmsPduDbId, iId));
	User::LeaveIfError(CTestConfig::GetElement(val, KSmsPduDbDelimiter, EConcatSegment, iSegment));

	TPtrC8 pdu;
	User::LeaveIfError(CTestConfig::GetElement(val, KSmsPduDbDelimiter, EConcatPdu, pdu));
	SetPduL(pdu);

	const TInt err = CTestConfig::GetElement(val, KSmsPduDbDelimiter, EConcatError, iDecodeError);
	if (err != KErrNone)
		{
		iDecodeError = KErrNone;
		}

	(void) CTestConfig::GetElement(val, KSmsPduDbDelimiter, EConcatDesc, iDescription); //ignore error

	TRAP(iSmsMessageError, ConstructSmsMessageL(aFs));
	}

EXPORT_C void CSmsPduDbPdu::GetHexPdu(TDes8& aHexPdu) const
	{
	const TInt len = iPdu.Length();
	aHexPdu.Zero();

	for (TInt i=0; i < len; i++)
		{
		_LIT8(KHexFormat, "%02X");
		const TUint8 val = iPdu[i];
		aHexPdu.AppendFormat(KHexFormat, val);
		}
	}

void CSmsPduDbPdu::SetPduL(const TDesC8& aHexPdu)
	{
	const TInt len = aHexPdu.Length();

	if (len % 2 != 0)
		User::Leave(KErrArgument);

	for (TInt i = 0; i < len; i+=2)
		{
		const TPtrC8 ptr(aHexPdu.Mid(i, 2));
		TLex8 lex(ptr);
		TUint8 val;
		User::LeaveIfError(lex.Val(val, EHex));
		iPdu.Append(val);
		}
	}

TBool CSmsPduDbPdu::IsMobileTerminatedL() const
	{
	TBool ret(EFalse);

	switch(iType)
		{
		case CSmsPDU::ESmsSubmit:
		case CSmsPDU::ESmsCommand:

			ret = EFalse;
			break;

		case CSmsPDU::ESmsDeliver:
		case CSmsPDU::ESmsStatusReport:

			ret = ETrue;
			break;

		default:

			User::Leave(KErrNotSupported);
			break;
		}


	return ret;
	}

void CSmsPduDbMessage::DecodeL(RFs& aFs, const CTestConfigItem& aItem)
	{
	const TDesC8& val = aItem.Value();

	iPdus.Reset();
	delete iSmsMessage;
	iSmsMessage = NULL;

	CSmsBuffer* buffer = CSmsBuffer::NewL();
	iSmsMessage = CSmsMessage::NewL(aFs, iType, buffer); //takes ownership of buffer

	TPtrC8 body;
	TBool statusReport(EFalse);
	TInt dataCodingScheme(0);
	TInt protocolIdentifier(0);

	User::LeaveIfError(CTestConfig::GetElement(val, KSmsPduDbDelimiter, CSmsPduDbBase::ESmsPduDbId, iId));

	TInt err = CTestConfig::GetElement(val, KSmsPduDbDelimiter, EMsgBody, body);
	if (err == KErrNone)
		{
		SetBufferL(body, *buffer);
		}

	err = CTestConfig::GetElement(val, KSmsPduDbDelimiter, EMsgStatusReport, statusReport);
	if (err == KErrNone)
		{
		if (iType == CSmsPDU::ESmsSubmit)
			{
			CSmsSubmit& submit = static_cast<CSmsSubmit&>(iSmsMessage->SmsPDU());
			submit.SetStatusReportRequest(statusReport);
			}
		else if (iType == CSmsPDU::ESmsCommand)
			{
			CSmsCommand& command = static_cast<CSmsCommand&>(iSmsMessage->SmsPDU());
			command.SetStatusReportRequest(statusReport);
			}
		}

	err = CTestConfig::GetElement(val, KSmsPduDbDelimiter, EMsgDCS, dataCodingScheme);
	if (err == KErrNone)
		{
		SetDataCodingScheme(dataCodingScheme);
		}

	err = CTestConfig::GetElement(val, KSmsPduDbDelimiter, EMsgPID, protocolIdentifier);
	if (err == KErrNone)
		{
		SetProtocolIdentifier(protocolIdentifier);
		}

	(void) CTestConfig::GetElement(val, KSmsPduDbDelimiter, EMsgDesc, iDescription); //ignore error

	TRAP(iPdusError, iSmsMessage->EncodeMessagePDUsL(iPdus));
	}

void CSmsPduDbMessage::SetDataCodingScheme(TInt aDCS)
	{
	TSmsOctet octet(aDCS);
	TSmsDataCodingScheme& dcs = reinterpret_cast<TSmsDataCodingScheme&>(octet);

	CSmsPDU& pdu = iSmsMessage->SmsPDU();

	pdu.SetBits7To4(dcs.Bits7To4());

	TSmsDataCodingScheme::TSmsClass dcsClass;
	const TBool hasClass = dcs.Class(dcsClass);
	pdu.SetClass(hasClass, dcsClass);

	pdu.SetAlphabet(dcs.Alphabet());
	pdu.SetTextCompressed(dcs.TextCompressed());
	pdu.SetIndicationState(dcs.IndicationState());
	pdu.SetIndicationType(dcs.IndicationType());
	}

void CSmsPduDbMessage::SetProtocolIdentifier(TInt aPID)
	{
	TSmsOctet octet(aPID);
	TSmsProtocolIdentifier& pid = reinterpret_cast<TSmsProtocolIdentifier&>(octet);
	CSmsPDU& pdu = iSmsMessage->SmsPDU();

	pdu.SetPIDType(pid.PIDType());
	pdu.SetTelematicDeviceIndicator(pid.TelematicDeviceIndicator());

	if (pid.PIDType()==TSmsProtocolIdentifier::ESmsPIDShortMessageType)
		{
		TSmsProtocolIdentifier::TSmsShortMessageType pidSMT = (TSmsProtocolIdentifier::TSmsShortMessageType) pid.ShortMessageType();
		pdu.SetShortMessageType(pidSMT);
		}

	if (pid.TelematicDeviceIndicator()==TSmsProtocolIdentifier::ESmsTelematicDevice)
		{
		pdu.SetTelematicDeviceType(pid.TelematicDeviceType());
		}
	}

void CSmsPduDbMessage::SetBufferL(const TDesC8& aBody, CSmsBufferBase& aBuffer)
	{
	TLex8 body(aBody);
	body.Mark();
	TPtrC8 marked;


	while (!body.Eos())
		{
		if (body.Peek() == KSmsPduDbTagStart)
			{
			marked.Set(body.MarkedToken());
			AppendToBufferL(marked, aBuffer);
			body.Mark();
			ProcessUnicodeL(body, aBuffer);
			}
		else
			{
			body.Inc();
			}
		}

	marked.Set(body.MarkedToken());
	if (marked.Length() != 0)
		{
		AppendToBufferL(marked, aBuffer);
		}
	}

void CSmsPduDbMessage::ProcessUnicodeL(TLex8& aBody, CSmsBufferBase& aBuffer)
	{
	const TPtrC8 remainder(aBody.Remainder());
	const TInt locate = remainder.Locate(KSmsPduDbTagEnd);

	if (locate != KErrNotFound)
		{
		aBody.Inc();
		if (!aBody.Eos() && aBody.Peek().IsDigit())
			{
			aBody.Mark();
			aBody.Inc(locate - 1);
			TPtrC8 marked(aBody.MarkedToken());
			ParseUnicodeL(marked, aBuffer);
			aBody.Inc();
			if (!aBody.Eos())
				aBody.Mark();
			}
		}
	else
		aBody.Inc();
	}

void CSmsPduDbMessage::AppendToBufferL(const TDesC8& aItem, CSmsBufferBase& aBuffer)
	{
	HBufC* body16 = HBufC::NewLC(aItem.Length());
	body16->Des().Copy(aItem);

	const TInt len = aBuffer.Length();
	aBuffer.InsertL(len, *body16);

	CleanupStack::PopAndDestroy(body16);
	}

void CSmsPduDbMessage::ParseUnicodeL(const TDesC8& aUnicode, CSmsBufferBase& aBuffer)
	{
	TInt unicodeChar(0);
	TInt unicodeRepeat(1);

	User::LeaveIfError(CTestConfig::GetElement(aUnicode, KSmsPduDbTagDelimitier, EUnicodeChar, unicodeChar));

	TInt err = CTestConfig::GetElement(aUnicode, KSmsPduDbTagDelimitier, EUnicodeCharRepeat, unicodeRepeat);

	if (err != KErrNone || unicodeRepeat < 1)
		{
		unicodeRepeat = 1;
		}

	TBuf8<1> buf;
	buf.Append(unicodeChar);

	while (unicodeRepeat--)
		{
		AppendToBufferL(buf, aBuffer);
		}
	}

EXPORT_C void CSmsPduDbConcat::DecodeL(RFs& aFs)
	{
	delete iSmsMessage;
	iSmsMessage = NULL;
	const CSmsPDU::TSmsPDUType type = iSegments[0]->iType;
	iSmsMessage = CSmsMessage::NewL(aFs, type, CSmsBuffer::NewL());

	CArrayFixFlat<TGsmSms>* pdus = new (ELeave) CArrayFixFlat<TGsmSms>(1);
	CleanupStack::PushL(pdus);

	const TInt count = iSegments.Count();

	for (TInt i=0; i<count; i++)
		{
		const CSmsPduDbConcatSegment& segment = *iSegments[i];
		TGsmSms sms;
		sms.SetPdu(segment.iPdu);
		pdus->AppendL(sms);
		}

	TRAP(iSmsMessageError, iSmsMessage->DecodeMessagePDUsL(*pdus));
	CleanupStack::PopAndDestroy(pdus);
	}
