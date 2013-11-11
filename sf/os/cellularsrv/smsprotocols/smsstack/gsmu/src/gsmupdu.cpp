// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains implementations for the classes defined in gsmpdu.h
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "gsmupduTraces.h"
#endif

#include <etelmm.h>
#include "gsmuNmspaceMobMsg.h"
#include "gsmuetel.h"
#include <exterror.h>
#include <e32uid.h>

#include "gsmupdu.h"
#include "Gsmumain.h"
#include "Gsmumsg.h"

/**
 *  Restores a CSmsPDU from a stream where the object has bean previously persisted.
 *  
 *  The type is determined from the first byte in the stream.
 *  
 *  @param aStream Stream from which to restore this CSmsPDU
 *  @param aCharacterSetConverter Character converter utility, required for encoding
 *  and decoding this PDU
 *  @param aFs File system handle, required for encoding and decoding this PDU
 *  @return Newly constructed CSmsPDU-derived object restored from aStream
 *  @capability None
 */
EXPORT_C CSmsPDU* CSmsPDU::NewL(RReadStream& aStream,CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NEWL_1, "CSmsPDU::NewL()");

	TInt type=aStream.ReadUint8L();
	CSmsPDU* smspdu=NULL;
	switch (type)
		{
		case ESmsDeliver:
			{
			smspdu=new(ELeave) CSmsDeliver();
			break;
			}
		case ESmsSubmit:
			{
			smspdu=new(ELeave) CSmsSubmit();
			break;
			}
		case ESmsDeliverReport:
			{
			smspdu=new(ELeave) CSmsDeliverReport();
			break;
			}
		case ESmsSubmitReport:
			{
			smspdu=new(ELeave) CSmsSubmitReport();
			break;
			}
		case ESmsStatusReport:
			{
			smspdu=new(ELeave) CSmsStatusReport();
			break;
			}
		case ESmsCommand:
			{
			smspdu=new(ELeave) CSmsCommand();
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		};
	CleanupStack::PushL(smspdu);
	smspdu->ConstructL(aCharacterSetConverter,aFs);
	smspdu->InternalizeMessagePDUL(aStream);
	CleanupStack::Pop();
	return smspdu;
	} // CSmsPDU::NewL


/**
 *  Allocates and constructs a CSmsPDU from a TGsmSms.
 *  
 *  The type of SMS to construct is determined from the first octet in aGsmSms
 *  and whether the SMS is mobile terminated.
 *  
 *  @param aGsmSms Encoded (raw) GSM SMS PDU
 *  @param aCharacterSetConverter Character converter utility, required for encoding
 *  and decoding this PDU
 *  @param aFs File system handle, required for encoding and decoding this PDU
 *  @param aIsRPError True if the PDU is part of an RP Error. This is used only
 *  for SUBMIT REPORT (CSmsSubmitReport) and DELIVER REPORT (CSmsDeliverReport),
 *  as the format of these PDUs differs depending on whether the PDU is part of
 *  an RP Ack or RP Error.
 *  @param aIsMobileTerminated Used to determine (with the first octet in aGsmSms)
 *  the TSmsPDUType
 *  @return Newly constructed CSmsPDU-derived object restored from aGsmSms
 *  @capability None
 */
EXPORT_C CSmsPDU* CSmsPDU::NewL(const TGsmSms& aGsmSms,CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs, TBool aIsRPError,TBool aIsMobileTerminated)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NEWL1_1, "CSmsPDU::NewL(): aIsRPError=%d, aIsMobileTerminated=%d",aIsRPError, aIsMobileTerminated);

    const TUint8* ptr1=aGsmSms.Pdu().Ptr();

    TInt mti=*ptr1 & TSmsFirstOctet::ESmsMTIMask; // mask first two bits
	CSmsPDU* smspdu=NULL;
	switch (mti)
		{
		case TSmsFirstOctet::ESmsMTIDeliverOrDeliverReport:
			{
			if (aIsMobileTerminated)
				smspdu=new(ELeave) CSmsDeliver();
			else
				smspdu=new(ELeave) CSmsDeliverReport(aIsRPError);
			break;
			}
		case TSmsFirstOctet::ESmsMTISubmitOrSubmitReport:
			{
			if (aIsMobileTerminated)
				smspdu=new(ELeave) CSmsSubmitReport(aIsRPError);
			else
				smspdu=new(ELeave) CSmsSubmit();
			break;
			}
		case TSmsFirstOctet::ESmsMTIStatusReportOrCommand:
			{
			if (aIsMobileTerminated)
				smspdu=new(ELeave) CSmsStatusReport();
			else
				smspdu=new(ELeave) CSmsCommand();
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		};
	CleanupStack::PushL(smspdu);
	smspdu->ConstructL(aCharacterSetConverter,aFs);

	TGsmuLex8 lex(aGsmSms.Pdu());
	smspdu->DecodeL(lex);

	smspdu->SetParsedServiceCenterAddressL(aGsmSms.Sca());

	CleanupStack::Pop(smspdu);
	return smspdu;
	} // CSmsPDU::NewL


/**
 *  Allocates and constructs a CSmsPDU, with the type specified by a TSmsPDUType.
 *  
 *  @param aType The PDU type to construct
 *  @param aCharacterSetConverter Character converter utility, required for encoding
 *  and decoding this PDU
 *  @param aFs File system handle, required for encoding and decoding this PDU
 *  @param aIsRPError True if the PDU is part of an RP Error. This is used only
 *  for SUBMIT REPORT (CSmsSubmitReport) and DELIVER REPORT (CSmsDeliverReport),
 *  as the format of these PDUs differs depending on whether the PDU is part of
 *  an RP Ack or RP Error.
 *  @return Newly constructed CSmsPDU-derived object
 *  @capability None
 */
EXPORT_C CSmsPDU* CSmsPDU::NewL(TSmsPDUType aType,CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TBool aIsRPError)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NEWL2_1, "CSmsPDU::NewL(): aIsRPError=%d", aIsRPError);

	CSmsPDU* smspdu=NULL;
	switch (aType)
		{
		case ESmsDeliver:
			{
			smspdu=new(ELeave) CSmsDeliver();
			break;
			}
		case ESmsSubmit:
			{
			smspdu=new(ELeave) CSmsSubmit();
			break;
			}
		case ESmsDeliverReport:
			{
			smspdu=new(ELeave) CSmsDeliverReport(aIsRPError);
			break;
			}
		case ESmsSubmitReport:
			{
			smspdu=new(ELeave) CSmsSubmitReport(aIsRPError);
			break;
			}
		case ESmsStatusReport:
			{
			smspdu=new(ELeave) CSmsStatusReport();
			break;
			}
		case ESmsCommand:
			{
			smspdu=new(ELeave) CSmsCommand();
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		};
	CleanupStack::PushL(smspdu);
	smspdu->ConstructL(aCharacterSetConverter,aFs);
	CleanupStack::Pop();
	return smspdu;
	} // CSmsPDU::NewL


/**
 *  Allocates and constructs a CSmsPDU as a copy of another CSmsPDU.
 *  
 *  @return Newly constructed CSmsPDU-derived object
 *  @capability None
 */
EXPORT_C CSmsPDU* CSmsPDU::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_DUPLICATEL_1, "CSmsPDU::DuplicateL()");

	CSmsPDU*  smsPDU = NULL;

	switch (Type())
		{
		case ESmsDeliver:
			{
			smsPDU = ((CSmsDeliver*) this)->DuplicateL();
			}
			break;

		case ESmsDeliverReport:
			{
			smsPDU = ((CSmsDeliverReport*) this)->DuplicateL();
			}
			break;

		case ESmsSubmit:
			{
			smsPDU = ((CSmsSubmit*) this)->DuplicateL();
			}
			break;

		case ESmsSubmitReport:
			{
			smsPDU = ((CSmsSubmitReport*) this)->DuplicateL();
			}
			break;

		case ESmsStatusReport:
			{
			smsPDU = ((CSmsStatusReport*) this)->DuplicateL();
			}
			break;

		case ESmsCommand:
			{
			smsPDU = ((CSmsCommand*) this)->DuplicateL();
			}
			break;

		default:
			{
			User::Leave(KErrNotSupported);
			}
			break;
		};

	return smsPDU;
	} // CSmsPDU::DuplicateL


/**
 *  Externalises the object.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void CSmsPDU::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(iSmsPDUType);
	ExternalizeMessagePDUL(aStream);
	} // CSmsPDU::ExternalizeL


/**
 *  Encodes a TGsmSms for the given type of CSmsPDU.
 *  
 *  @param aGsmSms On return, encoded GSM SMS PDU
 *  @capability None
 */
EXPORT_C void CSmsPDU::EncodeMessagePDUL(TGsmSms& aGsmSms) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_ENCODEMESSAGEPDUL_1, "CSmsPDU::EncodeMessagePDUL()");

	NMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
	pdu.SetLength(NMobileSmsMessaging::KGsmTpduSize);
	TUint8 *ptr1=(TUint8*) pdu.Ptr();
	TUint8 *ptr2=ptr1;

	TGsmSmsTelNumber parsedAddress;
	iServiceCenterAddress->ParsedAddress(parsedAddress);
	aGsmSms.SetSca(parsedAddress);

	ptr2=EncodeL(ptr1);
	pdu.SetLength(ptr2-ptr1);
	aGsmSms.SetPdu(pdu);
	} // CSmsPDU::EncodeMessagePDUL

void CSmsPDU::EncodeMessagePDUL(TGsmSms& aGsmSms, const TEncodeParams* aEncodeParams) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDU_ENCODEMESSAGEPDUL1_1, "CSmsPDU::EncodeMessagePDUL()");

	NMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
	pdu.SetLength(NMobileSmsMessaging::KGsmTpduSize);
	TUint8 *ptr1=(TUint8*) pdu.Ptr();
	TUint8 *ptr2=ptr1;

	TGsmSmsTelNumber parsedAddress;
	iServiceCenterAddress->ParsedAddress(parsedAddress);
	aGsmSms.SetSca(parsedAddress);

	ptr2=EncodeL(ptr1, aEncodeParams);
	pdu.SetLength(ptr2-ptr1);
	aGsmSms.SetPdu(pdu);
	} // CSmsPDU::EncodeMessagePDUL	

/**
 *  Gets the service center address.
 *  
 *  It is assumed that the address is Unicode, and can contain spaces and alphabetic
 *  characters.
 *  
 *  @return Service center address
 *  @capability None
 */
EXPORT_C TPtrC CSmsPDU::ServiceCenterAddress() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SERVICECENTERADDRESS_1, "CSmsPDU::ServiceCenterAddress()");

	return iServiceCenterAddress->Address();
	} // CSmsPDU::ServiceCenterAddress


/**
 *  Sets the service center address.
 *  
 *  A prepended '+' is used to indicate an international number.
 *  
 *  @param aAddress Service center address
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetServiceCenterAddressL(const TDesC& aAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETSERVICECENTERADDRESSL_1, "CSmsPDU::SetServiceCenterAddressL()");

	iServiceCenterAddress->SetAddressL(aAddress);
	} // CSmsPDU::SetServiceCenterAddressL


/**
 *  Gets the service center address in a TGsmSmsTelNumber object.
 *  
 *  A prepended '+', spaces and aphabetic characters are removed.
 *  
 *  @param aParsedAddress Service center address
 *  @capability None
 */
EXPORT_C void CSmsPDU::ParsedServiceCenterAddress(TGsmSmsTelNumber& aParsedAddress) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_PARSEDSERVICECENTERADDRESS_1, "CSmsPDU::ParsedServiceCenterAddress()");

	iServiceCenterAddress->ParsedAddress(aParsedAddress);
	} // CSmsPDU::ParsedServiceCenterAddress


/**
 *  Sets the service center address through a TGsmSmsTelNumber object.
 *  
 *  These functions panic for a DELIVER REPORT (CSmsDeliverReport) or a SUBMIT REPORT (CSmsSubmitReport).
 *  
 *  @param aParsedAddress Service center address
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetParsedServiceCenterAddressL(const TGsmSmsTelNumber& aParsedAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETPARSEDSERVICECENTERADDRESSL_1, "CSmsPDU::SetParsedServiceCenterAddressL()");

	iServiceCenterAddress->SetParsedAddressL(aParsedAddress);
	} // CSmsPDU::SetParsedServiceCenterAddressL


/**
 *  Gets the "to from" address.
 *  
 *  This address is the original (sender's) address for a DELIVER (CSmsDeliver),
 *  the destination address for a SUBMIT (CSmsSubmit), the recipient (sender's)
 *  address for a STATUS REPORT (CSmsStatusReport) and the destination address
 *  for a COMMAND (CSmsCommand).
 *  
 *  The function panics for a DELIVER REPORT (CSmsDeliverReport) or a SUBMIT REPORT
 *  (CSmsSubmitReport).
 *  
 *  It is assumed that the address is Unicode, and can contain spaces and alphabetic
 *  characters.
 *  
 *  @return The destination or sender address
 *  @capability None
 */
EXPORT_C TPtrC CSmsPDU::ToFromAddress() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_TOFROMADDRESS_1, "CSmsPDU::SetParsedServiceCenterAddressL()");

	if (ToFromAddressPtr() == NULL)
		{
		return TPtrC(KNullDesC16);
		}

	return ToFromAddressPtr()->Address();
	} // CSmsPDU::ToFromAddress


/**
 *  Sets the "to from" address.
 *  
 *  The function panics for a DELIVER REPORT (CSmsDeliverReport) or a SUBMIT REPORT
 *  (CSmsSubmitReport).
 *  
 *  A prepended '+' is used to indicate an international number.
 *  
 *  @param aAddress The destination or sender address
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetToFromAddressL(const TDesC& aAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETTOFROMADDRESSL_1, "CSmsPDU::SetToFromAddressL()");

	__ASSERT_DEBUG(ToFromAddressPtr()!=NULL,Panic(KGsmuPanicToFromAddressNotPresent));
	CSmsAddress* tofromaddress=(CSmsAddress*) ToFromAddressPtr();
	tofromaddress->SetAddressL(aAddress);
	} // CSmsPDU::SetToFromAddressL


/**
 *  Gets the "to from" in a TGsmSmsTelNumber object.
 *  
 *  A prepended '+', spaces and aphabetic characters are removed.
 *  
 *  @param aParsedAddress The destination or sender address
 *  @capability None
 */
EXPORT_C void CSmsPDU::ParsedToFromAddress(TGsmSmsTelNumber& aParsedAddress) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_PARSEDTOFROMADDRESS_1, "CSmsPDU::ParsedToFromAddress()");

	__ASSERT_DEBUG(ToFromAddressPtr()!=NULL,Panic(KGsmuPanicToFromAddressNotPresent));
	ToFromAddressPtr()->ParsedAddress(aParsedAddress);
	} // CSmsPDU::ParsedToFromAddress


/**
 *  Sets the "to from" with a TGsmSmsTelNumber object.
 *  
 *  @param aParsedAddress The destination or sender address
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetParsedToFromAddressL(const TGsmSmsTelNumber& aParsedAddress)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETPARSEDTOFROMADDRESSL_1, "CSmsPDU::SetParsedToFromAddressL()");

	__ASSERT_DEBUG(ToFromAddressPtr()!=NULL,Panic(KGsmuPanicToFromAddressNotPresent));
	CSmsAddress* tofromaddress=(CSmsAddress*) ToFromAddressPtr();
	tofromaddress->SetParsedAddressL(aParsedAddress);
	} // CSmsPDU::SetParsedToFromAddressL


//
/**
 *  Updates the three pieces of concatenation data (reference number,
 *  pdu index and max pdu number) in this PDU.
 * 
 *  @param aRef       Reference number.
 *  @param aPduIndex  Current PDU number.
 *  @param aMaxPdu    Total PDU number.
 */
void CSmsPDU::UpdateConcatenationDataL(TInt aRef, TInt aPduIndex, TInt aMaxPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDU_UPDATECONCATENATIONDATAL_1, "CSmsPDU::UpdateConcatenationDataL()");

	SetConcatenatedMessageReference(aRef);
	SetConcatenatedMessagePDUIndex(aPduIndex);
	SetNumConcatenatedMessagePDUs(aMaxPdu);
	} // CSmsPDU::UpdateConcatenationDataL


/**
 *  Updates the email header data in this PDU.
 * 
 *  @param aEmailOverallHeaderLength  Length of the email header.
 */
void CSmsPDU::UpdateEmailHeaderDataL(TInt& aEmailOverallHeaderLength)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDU_UPDATEEMAILHEADERDATAL_1, "CSmsPDU::UpdateEmailHeaderDataL()");

	TInt emailIndex(0);
	TInt udLength=0;
	TInt headerPortionLength=0;
	if(UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIRFC822EmailHeader,emailIndex))
		{
		if(aEmailOverallHeaderLength<=0)
			headerPortionLength=0;
		else
			{
			udLength=UserData().Body().Length();
			if(Alphabet() == TSmsDataCodingScheme::ESmsAlphabetUCS2)
				udLength/=2;
			if(aEmailOverallHeaderLength>=udLength)
				headerPortionLength=udLength;
			else
				headerPortionLength=aEmailOverallHeaderLength;
			aEmailOverallHeaderLength-=udLength;
			}
		UserData().InformationElement(emailIndex).Data()[0]=static_cast<TUint8>(headerPortionLength);
		}
	else
		User::Leave(KErrCorrupt);
	} // CSmsPDU::UpdateEmailHeaderDataL

	
/**
 *  Updates the SMSC control parameter in this PDU.
 * 
 *  @param aOctet  Value to store in the SMSC control parameter.
 */
TBool CSmsPDU::UpdateSMSCCtrlParameterL(const TUint8 aOctet)
	{
	TInt smscIndex(0);
	if(UserData().InformationElementIndex(CSmsInformationElement::ESmsIEISMSCControlParameters,smscIndex))
		{
		UserData().InformationElement(smscIndex).Data()[0] = aOctet;
		if (Type() == ESmsSubmit)
			{
			CSmsSubmit* smsSubmit = (CSmsSubmit*)this;
			smsSubmit->SetStatusReportRequest(ETrue);	
			}
		else if (Type() == ESmsDeliver)
			{
			CSmsDeliver* smsDeliver = (CSmsDeliver*)this;
			smsDeliver->SetStatusReportIndication(ETrue);
			}
		}
	else
		{
		User::Leave(KErrCorrupt);	
		} 

	return ETrue;
	} // CSmsPDU::UpdateSMSCCtrlParameterL

	
/**
 *  Updates the TPSRRL control parameter in this PDU.
 * 
 *  @param aSmsReportRequest  Status Report Request setting.
 * 
 *  @return  Always returns ETrue.
 */
TBool CSmsPDU::UpdateTPSRRL(TSmsFirstOctet aSmsReportRequest)
	{
	if (Type() == ESmsSubmit)
		{
		CSmsSubmit* smsSubmit = (CSmsSubmit*)this;
		if (aSmsReportRequest)
			{
			smsSubmit->SetStatusReportRequest(ETrue);
			}
		else
			{
			smsSubmit->SetStatusReportRequest(EFalse);	
			}
		}
	else if (Type() == ESmsDeliver)
			{
			CSmsDeliver* smsDeliver = (CSmsDeliver*)this;
			if (aSmsReportRequest)
				{
				smsDeliver->SetStatusReportIndication(ETrue);
				}
			else
				{
				smsDeliver->SetStatusReportIndication(EFalse);	
				}
			}
	
	return ETrue;
	} // CSmsPDU::UpdateTPSRRL


EXPORT_C TSmsEncoding CSmsPDU::NationalLanguageEncoding() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NATIONALLANGUAGEENCODING_1, "CSmsPDU::NationalLanguageEncoding()");
	
	TSmsEncoding  encodingUsed = ESmsEncodingNone;
	
	//
	// Only valid in 7bit...
	//
	if (Alphabet() != TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		return ESmsEncodingNone;
		}
	
	//
	// Get the locking shift setting...
	//
	TInt  lockingIndex = 0;
	TSmsNationalLanguageIdentifier  lockingShiftValue  = (TSmsNationalLanguageIdentifier) 0;
	
	if (UserData().InformationElementLastIndex(CSmsInformationElement::ESmsNationalLanguageLockingShift, lockingIndex))
		{
		lockingShiftValue = (TSmsNationalLanguageIdentifier) UserData().InformationElement(lockingIndex).Data()[0];
		
		//
		// Store the locking shift value. We will combine this with the single
		// shift value later.
		//
		switch (lockingShiftValue)
			{
			case ESmsNationalLanguageIdentifierTurkish:
				{
				encodingUsed = ESmsEncodingTurkishLockingShift;
				}
				break;

			case ESmsNationalLanguageIdentifierPortuguese:
				{
				encodingUsed = ESmsEncodingPortugueseLockingShift;
				}
				break;

			default:
				{
				//
				// Invalid or not understood, so ignore it!
				//
				}
			};
		}
	
	//
	// Get the single shift setting...
	//
	TInt  singleIndex = 0;
	TSmsNationalLanguageIdentifier  singleShiftValue  = (TSmsNationalLanguageIdentifier) 0;
	
	if (UserData().InformationElementLastIndex(CSmsInformationElement::ESmsNationalLanguageSingleShift, singleIndex))
		{
		singleShiftValue = (TSmsNationalLanguageIdentifier) UserData().InformationElement(singleIndex).Data()[0];
		
		//
		// Combine the single shift value into the encoding setting. If any
		// single shift does not match an accepted locking shift, then it is
		// ignored as the locking table has more value.
		//
		switch (singleShiftValue)
			{
			case ESmsNationalLanguageIdentifierTurkish:
				{
				if (encodingUsed == ESmsEncodingNone)
					{
					encodingUsed = ESmsEncodingTurkishSingleShift;
					}
				else if (encodingUsed == ESmsEncodingTurkishLockingShift)
					{
					encodingUsed = ESmsEncodingTurkishLockingAndSingleShift;
					}
				}
				break;

			case ESmsNationalLanguageIdentifierSpanish:
				{
				if (encodingUsed == ESmsEncodingNone)
					{
					encodingUsed = ESmsEncodingSpanishSingleShift;
					}
				}
				break;

			case ESmsNationalLanguageIdentifierPortuguese:
				{
				if (encodingUsed == ESmsEncodingNone)
					{
					encodingUsed = ESmsEncodingPortugueseSingleShift;
					}
				else if (encodingUsed == ESmsEncodingPortugueseLockingShift)
					{
					encodingUsed = ESmsEncodingPortugueseLockingAndSingleShift;
					}
				}
				break;

			default:
				{
				//
				// Invalid or not understood, so ignore it!
				//
				}
			};
		}
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NATIONALLANGUAGEENCODING_2, "CSmsPDU::NationalLanguageEncoding(): lockingShift=%d", lockingShiftValue);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NATIONALLANGUAGEENCODING_3, "CSmsPDU::NationalLanguageEncoding(): singleShift=%d", singleShiftValue);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NATIONALLANGUAGEENCODING_4, "CSmsPDU::NationalLanguageEncoding(): encodingUsed=%d", encodingUsed);
	
	return encodingUsed;
	} // CSmsPDU::NationalLanguageEncoding


EXPORT_C void CSmsPDU::SetNationalLanguageEncodingL(TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETNATIONALLANGUAGEENCODINGL_1, "CSmsPDU::SetNationalLanguageEncodingL(): aEncoding=%d", aEncoding);
	
	//
	// Convert the encoding enum into two parts: Single Shift and Locking Shift
	//
	TSmsNationalLanguageIdentifier  lockingShiftValue = (TSmsNationalLanguageIdentifier) 0;
	TSmsNationalLanguageIdentifier  singleShiftValue  = (TSmsNationalLanguageIdentifier) 0;
	TBool  lockingIERequired = EFalse;
	TBool  singleIERequired = EFalse;

	switch (aEncoding)
		{
		case ESmsEncodingNone:
			{
			// Nothing to set.
			}
			break;
		
		case ESmsEncodingTurkishSingleShift:
			{
			singleShiftValue = ESmsNationalLanguageIdentifierTurkish;
			singleIERequired = ETrue;
			}
			break;
		
		case ESmsEncodingTurkishLockingShift:
			{
			lockingShiftValue = ESmsNationalLanguageIdentifierTurkish;
			lockingIERequired = ETrue;
			}
			break;
		
		case ESmsEncodingTurkishLockingAndSingleShift:
			{
			lockingShiftValue = ESmsNationalLanguageIdentifierTurkish;
			singleShiftValue  = ESmsNationalLanguageIdentifierTurkish;
			lockingIERequired = ETrue;
			singleIERequired  = ETrue;
			}
			break;
		
		case ESmsEncodingSpanishSingleShift:
			{
			singleShiftValue = ESmsNationalLanguageIdentifierSpanish;
			singleIERequired = ETrue;
			}
			break;
		
		case ESmsEncodingPortugueseSingleShift:
			{
			singleShiftValue = ESmsNationalLanguageIdentifierPortuguese;
			singleIERequired = ETrue;
			}
			break;
		
		case ESmsEncodingPortugueseLockingShift:
			{
			lockingShiftValue = ESmsNationalLanguageIdentifierPortuguese;
			lockingIERequired = ETrue;
			}
			break;
		
		case ESmsEncodingPortugueseLockingAndSingleShift:
			{
			lockingShiftValue = ESmsNationalLanguageIdentifierPortuguese;
			singleShiftValue  = ESmsNationalLanguageIdentifierPortuguese;
			lockingIERequired = ETrue;
			singleIERequired  = ETrue;
			}
			break;

		default:
			{
			//
			// Invalid encoder method!
			//
			User::Leave(KErrArgument);
			}
		};
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETNATIONALLANGUAGEENCODINGL_2, "CSmsPDU::SetNationalLanguageEncodingL(): lockingShift=%d", lockingShiftValue);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETNATIONALLANGUAGEENCODINGL_3, "CSmsPDU::SetNationalLanguageEncodingL(): singleShift=%d", singleShiftValue);
	
	//
	// Update the locking shift setting...
	//
	TInt  lockingIndex = 0;
	TBool  lockingExists = UserData().InformationElementIndex(
							CSmsInformationElement::ESmsNationalLanguageLockingShift, lockingIndex);
	
	if (lockingExists)
		{
		if (lockingIERequired)
			{
			// Update the IE...
			UserData().InformationElement(lockingIndex).Data()[0] = (TUint8) lockingShiftValue;
			}
		else
			{
			// Remove the element as it is not needed...
			UserData().RemoveInformationElement(lockingIndex);
			}
		}
	else
		{
		if (lockingIERequired)
			{
			// Add the IE...
			TBuf8<1> data;
			data.SetLength(1);
			data[0] = lockingShiftValue;
			UserData().AddInformationElementL(CSmsInformationElement::ESmsNationalLanguageLockingShift, data);
			}
		else
			{
			// Nothing to do!
			}
		}

	//
	// Update the single shift setting...
	//
	TInt  singleIndex = 0;
	TBool  singleExists = UserData().InformationElementIndex(
							CSmsInformationElement::ESmsNationalLanguageSingleShift, singleIndex);
	
	if (singleExists)
		{
		if (singleIERequired)
			{
			// Update the IE...
			UserData().InformationElement(singleIndex).Data()[0] = (TUint8) singleShiftValue;
			}
		else
			{
			// Remove the element as it is not needed...
			UserData().RemoveInformationElement(singleIndex);
			}
		}
	else
		{
		if (singleIERequired)
			{
			// Add the IE...
			TBuf8<1> data;
			data.SetLength(1);
			data[0] = singleShiftValue;
			UserData().AddInformationElementL(CSmsInformationElement::ESmsNationalLanguageSingleShift, data);
			}
		else
			{
			// Nothing to do!
			}
		}
	} // CSmsPDU::SetNationalLanguageEncodingL


/**
 *  Gets bits 7 to 4 on the data coding scheme.
 *  
 *  The value of bits 7 to 4 effects the meaning of the lower order bits.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @return Bits 7 to 4
 *  @capability None
 */
EXPORT_C TSmsDataCodingScheme::TSmsDCSBits7To4 CSmsPDU::Bits7To4() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_BITS7TO4_1, "CSmsPDU::Bits7To4()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));

	return DataCodingScheme()->Bits7To4();
	} // TSmsDataCodingScheme::TSmsDCSBits7To4


/**
 *  Sets key bits 7 to 4 of the data coding scheme.
 *  
 *  This is designed to be used for message waiting indication, as it is not needed
 *  for the normal type of data coding scheme where the alphabet and class are
 *  defined in the lower order bits.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @param aBits7To4 Bits 7 to 4
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetBits7To4(TSmsDataCodingScheme::TSmsDCSBits7To4 aBits7To4)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETBITS7TO4_1, "CSmsPDU::SetBits7To4()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	TSmsDataCodingScheme* datacodingscheme=(TSmsDataCodingScheme*) DataCodingScheme();
	datacodingscheme->SetBits7To4(aBits7To4);
	} // CSmsPDU::SetBits7To4


/**
 *  Gets the alphabet encoded in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @return Alphabet
 *  @capability None
 */
EXPORT_C TSmsDataCodingScheme::TSmsAlphabet CSmsPDU::Alphabet() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_ALPHABET_1, "CSmsPDU::Alphabet()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return DataCodingScheme()->Alphabet();
	} // TSmsDataCodingScheme::TSmsAlphabet


/**
 *  Sets the alphabet encoded in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @param aAlphabet Alphabet
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetAlphabet(TSmsDataCodingScheme::TSmsAlphabet aAlphabet)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETALPHABET_1, "CSmsPDU::SetAlphabet()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	TSmsDataCodingScheme* datacodingscheme=(TSmsDataCodingScheme*) DataCodingScheme();
	datacodingscheme->SetAlphabet(aAlphabet);
	} // CSmsPDU::SetAlphabet


/**
 *  Gets the GSM SMS PDU class in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @param aClass Sms class 0 - 3 encoded in the data coding scheme
 *  @return True if SMS class is defined, else false
 *  @capability None
 */
EXPORT_C TBool CSmsPDU::Class(TSmsDataCodingScheme::TSmsClass& aClass) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_CLASS_1, "CSmsPDU::Class()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return DataCodingScheme()->Class(aClass);
	} // CSmsPDU::Class


/**
 *  Sets the GSM SMS PDU class in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @param aClassDefined True if SMS class is defined, else false
 *  @param aClass Sms class 0 - 3 encoded in the data coding scheme
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetClass(TBool aClassDefined,TSmsDataCodingScheme::TSmsClass aClass)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETCLASS_1, "CSmsPDU::SetClass()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	TSmsDataCodingScheme* datacodingscheme=(TSmsDataCodingScheme*) DataCodingScheme();
	datacodingscheme->SetClass(aClassDefined,aClass);
	} // CSmsPDU::SetClass


/**
 *  True if Text Compressed is encoded in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @return True if Text Compressed is encoded
 *  @capability None
 */
EXPORT_C TBool CSmsPDU::TextCompressed() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_TEXTCOMPRESSED_1, "CSmsPDU::TextCompressed()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return DataCodingScheme()->TextCompressed();
	} // CSmsPDU::TextCompressed


/**
 *  Set to encode Text Compressed in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @param aCompressed True to encode Text Compressed
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetTextCompressed(TBool aCompressed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETTEXTCOMPRESSED_1, "CSmsPDU::SetTextCompressed()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	TSmsDataCodingScheme* datacodingscheme=(TSmsDataCodingScheme*) DataCodingScheme();
	datacodingscheme->SetTextCompressed(aCompressed);
	} // CSmsPDU::SetTextCompressed


/**
 *  Gets the Indication State encoded in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @return Indication State
 *  @capability None
 */
EXPORT_C TSmsDataCodingScheme::TSmsIndicationState CSmsPDU::IndicationState() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_INDICATIONSTATE_1, "CSmsPDU::IndicationState()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return DataCodingScheme()->IndicationState();
	} // TSmsDataCodingScheme::TSmsIndicationState


/**
 *  Sets the Indication State encoded in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @param aState Indication State
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetIndicationState(TSmsDataCodingScheme::TSmsIndicationState aState)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETINDICATIONSTATE_1, "CSmsPDU::SetIndicationState()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	TSmsDataCodingScheme* datacodingscheme=(TSmsDataCodingScheme*) DataCodingScheme();
	datacodingscheme->SetIndicationState(aState);
	} // CSmsPDU::SetIndicationState


/**
 *  Gets the Indication Type encoded in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @return Indication Type
 *  @capability None
 */
EXPORT_C TSmsDataCodingScheme::TSmsIndicationType CSmsPDU::IndicationType() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_INDICATIONTYPE_1, "CSmsPDU::IndicationType()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return DataCodingScheme()->IndicationType();
	} // TSmsDataCodingScheme::TSmsIndicationType


/**
 *  Sets the Indication Type encoded in the data coding scheme.
 *  
 *  The function panics if the data coding scheme is not present.
 *  
 *  @param aType Indication Type
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetIndicationType(TSmsDataCodingScheme::TSmsIndicationType aType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETINDICATIONTYPE_1, "CSmsPDU::SetIndicationType()");

	__ASSERT_DEBUG(DataCodingScheme()!=NULL,Panic(KGsmuPanicDataCodingSchemeNotPresent));
	TSmsDataCodingScheme* datacodingscheme=(TSmsDataCodingScheme*) DataCodingScheme();
	datacodingscheme->SetIndicationType(aType);
	} // CSmsPDU::SetIndicationType


/**
 *  Tests if the PDU is part of a concatenated message.
 *  
 *  @param aIs16Bit Content is set true if Concatenation Information Element is
 *  16-bit. Default is null.
 *  @return True if the PDU is part of a concatenated message.
 *  @capability None
 */
EXPORT_C TBool CSmsPDU::TextConcatenated(TBool* aIs16Bit) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_TEXTCONCATENATED_1, "CSmsPDU::TextConcatenated()");

	TInt index;
	return DoTextConcatenated(index,aIs16Bit);
	} // CSmsPDU::TextConcatenated


/**
 *  Sets whether the PDU is part of a concatenated message or not.
 *  
 *  It is a leaving function as it is inserting an information element into the
 *  user data.
 *  
 *  @param aConcatenated True if message is concatenated
 *  @param aIs16Bit True if type of Concatenation Information Element is 16 bit
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetTextConcatenatedL(TBool aConcatenated,TBool aIs16Bit)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETTEXTCONCATENATEDL_1, "CSmsPDU::SetTextConcatenatedL()");

	TInt index=0;
	TInt is16bit;
	TBool textconcatenated=DoTextConcatenated(index,&is16bit);
	if (aConcatenated)
		{
		if (!textconcatenated)
			{
			DoSetTextConcatenatedL(aIs16Bit);
			}
		else
			{
			if (((!aIs16Bit) && (is16bit)) || (aIs16Bit && (!is16bit)))
				{
				UserData().RemoveInformationElement(index);
				DoSetTextConcatenatedL(aIs16Bit);
				}
			}
		}
	else
		{
		if (textconcatenated)
			{
			UserData().RemoveInformationElement(index);
			}
		}
	} // CSmsPDU::SetTextConcatenatedL


/**
 *  Gets the reference contained in the Concatenation Information Element.
 *  
 *  @return Reference contained in the Concatenation Information Element
 *  @capability None
 */
EXPORT_C TInt CSmsPDU::ConcatenatedMessageReference() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_CONCATENATEDMESSAGEREFERENCE_1, "CSmsPDU::ConcatenatedMessageReference()");

	TInt index=0;
	TBool is16bit;
	TInt reference=0;
	TBool textconcatenated=DoTextConcatenated(index,&is16bit);
	__ASSERT_ALWAYS(textconcatenated,Panic(KGsmuPanicMessageNotConcatenated));
	if (is16bit)
		{
		reference=UserData().InformationElement(index).Data()[0]<<8;
		reference+=UserData().InformationElement(index).Data()[1];
		}
	else
		reference=UserData().InformationElement(index).Data()[0];
	return reference;
	} // CSmsPDU::ConcatenatedMessageReference


/**
 *  Sets the reference contained in the Concatenation Information Element.
 *  
 *  The function panics if aReference is out of range for the message type.
 *  
 *  @param aReference Value to set the Concatenated Message Reference.
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetConcatenatedMessageReference(TInt aReference)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETCONCATENATEDMESSAGEREFERENCE_1, "CSmsPDU::SetConcatenatedMessageReference()");

	TInt index=0;
	TBool is16bit;
	TBool textconcatenated=DoTextConcatenated(index,&is16bit);
	__ASSERT_ALWAYS(textconcatenated,Panic(KGsmuPanicMessageNotConcatenated));
	if (is16bit)
		{
		__ASSERT_DEBUG((aReference>=0x00) && (aReference<=0xFFFF),Panic(KGsmuPanicConcatenatedMessageReferenceOutOfRange));
		UserData().InformationElement(index).Data()[0]=(TUint8) (aReference >> 8);
		UserData().InformationElement(index).Data()[1]=(TUint8) aReference;
		}
	else
		{
		__ASSERT_DEBUG((aReference>=0x00) && (aReference<=0xFF),Panic(KGsmuPanicConcatenatedMessageReferenceOutOfRange));
		UserData().InformationElement(index).Data()[0]=(TUint8) aReference;
		}
	} // CSmsPDU::SetConcatenatedMessageReference


/**
 *  Gets the number of PDU's in a Concatenated Message.
 *  
 *  The function panics if the PDU is not concatenated.
 *  
 *  @return Number of PDU's in a Concatenated Message
 *  @capability None
 */
EXPORT_C TInt CSmsPDU::NumConcatenatedMessagePDUs() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_NUMCONCATENATEDMESSAGEPDUS_1, "CSmsPDU::NumConcatenatedMessagePDUs()");

	TInt index=0;
	TBool is16bit;
	TBool textconcatenated=DoTextConcatenated(index,&is16bit);
	__ASSERT_ALWAYS(textconcatenated,Panic(KGsmuPanicMessageNotConcatenated));
	TInt offset=is16bit? 2: 1;
	return UserData().InformationElement(index).Data()[offset];
	} // CSmsPDU::NumConcatenatedMessagePDUs


/**
 *  Sets the number of PDU's in a Concatenated Message.
 *  
 *  The function panics if the PDU is not concatenated or if aNum is out of range.
 *  
 *  @param aNum Number of PDU's in a Concatenated Message
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetNumConcatenatedMessagePDUs(TInt aNum)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETNUMCONCATENATEDMESSAGEPDUS_1, "CSmsPDU::SetNumConcatenatedMessagePDUs()");

	TInt index=0;
	TBool is16bit;
	TBool textconcatenated=DoTextConcatenated(index,&is16bit);
	__ASSERT_ALWAYS(textconcatenated,Panic(KGsmuPanicMessageNotConcatenated));
	__ASSERT_DEBUG(((aNum>=0x01) && (aNum<=0xFF)),Panic(KGsmuPanicNumConcatenatedMessagePDUsOutOfRange));
	TInt offset=is16bit? 2: 1;
	UserData().InformationElement(index).Data()[offset]=(TUint8) aNum;
	} // CSmsPDU::SetNumConcatenatedMessagePDUs


/**
 *  Gets the index of the PDU within the Concatenated Message.
 *  
 *  The function panics if the PDU is not concatenated.
 *  
 *  @return Index of the PDU within the Concatenated Message
 *  @capability None
 */
EXPORT_C TInt CSmsPDU::ConcatenatedMessagePDUIndex() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_CONCATENATEDMESSAGEPDUINDEX_1, "CSmsPDU::ConcatenatedMessagePDUIndex()");

	TInt index=0;
	TBool is16bit;
	TBool textconcatenated=DoTextConcatenated(index,&is16bit);
	__ASSERT_ALWAYS(textconcatenated,Panic(KGsmuPanicMessageNotConcatenated));
	TInt offset=is16bit? 3: 2;
	return UserData().InformationElement(index).Data()[offset];
	} // CSmsPDU::ConcatenatedMessagePDUIndex


/**
 *  Sets the index of the PDU within the Concatenated Message.
 *  
 *  The function panics if the PDU is not concatenated or aIndex is out of range.
 *  
 *  @param aIndex Index of the PDU within the Concatenated Message
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetConcatenatedMessagePDUIndex(TInt aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETCONCATENATEDMESSAGEPDUINDEX_1, "CSmsPDU::SetConcatenatedMessagePDUIndex()");

	TInt index=0;
	TBool is16bit;
	TBool textconcatenated=DoTextConcatenated(index,&is16bit);
	__ASSERT_ALWAYS(textconcatenated,Panic(KGsmuPanicMessageNotConcatenated));
	__ASSERT_DEBUG(((aIndex>=0x01) && (aIndex<=0xFF)),Panic(KGsmuPanicConcatenatedMessagePDUIndexOutOfRange));
	TInt offset=is16bit? 3: 2;
	UserData().InformationElement(index).Data()[offset]=(TUint8) aIndex;
	} // CSmsPDU::SetConcatenatedMessagePDUIndex


/**
 *  Gets application port addressing information in the user data.
 *  
 *  @param aDestination The destination port address
 *  @param aOriginator The originating port address
 *  @param aIs16Bit Set to true if the addressing is 16 bit. Default is null.
 *  @return True if there is an application port addressing information element
 *  in the user data
 *  @capability None
 */
EXPORT_C TBool CSmsPDU::ApplicationPortAddressing(TInt& aDestination,TInt& aOriginator,TBool* aIs16Bit) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_APPLICATIONPORTADDRESSING_1, "CSmsPDU::ApplicationPortAddressing()");

	TInt index;
	return DoApplicationPortAddressing(index,aDestination,aOriginator,aIs16Bit);
	} // CSmsPDU::ApplicationPortAddressing


/**
 *  Sets application port addressing information in the user data.
 *  
 *  @param aAddressing If true, set application port addressing in the PDU. If
 *  false, removes addressing if it's already set
 *  @param aDestination The destination port address to set in the PDU
 *  @param aOriginator The originating port address to set in the PDU
 *  @param aIs16Bit True if the addresses are 16 bit, false if 8 bit
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetApplicationPortAddressingL(TBool aAddressing,TInt aDestination,TInt aOriginator,TBool aIs16Bit)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETAPPLICATIONPORTADDRESSINGL_1, "CSmsPDU::SetApplicationPortAddressingL()");

	TInt index=0;
	TInt is16bit;
	TBool addressing=DoApplicationPortAddressing(index,aDestination,aOriginator,&is16bit);
	if (aAddressing)
		{
		if (!addressing)
			{
			DoSetApplicationPortAddressingL(aDestination,aOriginator,aIs16Bit);
			}
		else
			{
			if (((!aIs16Bit) && (is16bit)) || (aIs16Bit && (!is16bit)))
				{
				UserData().RemoveInformationElement(index);
				DoSetApplicationPortAddressingL(aDestination,aOriginator,aIs16Bit);
				}
			}
		}
	else
		{
		if (addressing)
			{
			UserData().RemoveInformationElement(index);
			}
		}
	} // CSmsPDU::SetApplicationPortAddressingL


/**
 *  Gets key bits 7 and 6 of the PID field.
 *  
 *  @return Bits 7 and 6 of the PID field
 *  @capability None
 */
EXPORT_C TSmsProtocolIdentifier::TSmsPIDType CSmsPDU::PIDType() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_PIDTYPE_1, "CSmsPDU::PIDType()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	return ProtocolIdentifier()->PIDType();
	} // TSmsProtocolIdentifier::TSmsPIDType


/**
 *  Sets key bits 7 and 6 of the PID field.
 *  
 *  @param aSmsPIDType Bits 7 and 6 of the PID field
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetPIDType(TSmsProtocolIdentifier::TSmsPIDType aSmsPIDType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETPIDTYPE_1, "CSmsPDU::SetPIDType()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	TSmsProtocolIdentifier* protocolidentifier=(TSmsProtocolIdentifier*) ProtocolIdentifier();
	protocolidentifier->SetPIDType(aSmsPIDType);
	} // CSmsPDU::SetPIDType


/**
 *  Gets the Telematic device indicator from the PID field.
 *  
 *  @return Telematic device indicator
 *  @capability None
 */
EXPORT_C TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator CSmsPDU::TelematicDeviceIndicator() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_TELEMATICDEVICEINDICATOR_1, "CSmsPDU::TelematicDeviceIndicator()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	return ProtocolIdentifier()->TelematicDeviceIndicator();
	} // TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator


/**
 *  Sets the Telematic device indicator from the PID field.
 *  
 *  @param aIndicator Telematic device indicator
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetTelematicDeviceIndicator(TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator aIndicator)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETTELEMATICDEVICEINDICATOR_1, "CSmsPDU::SetTelematicDeviceIndicator()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	TSmsProtocolIdentifier* protocolidentifier=(TSmsProtocolIdentifier*) ProtocolIdentifier();
	protocolidentifier->SetTelematicDeviceIndicator(aIndicator);
	} // CSmsPDU::SetTelematicDeviceIndicator


/**
 *  Gets the Short Message Type in the PID field.
 *  
 *  @return Short Message Type
 *  @capability None
 */
EXPORT_C TSmsProtocolIdentifier::TSmsShortMessageType CSmsPDU::ShortMessageType() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SHORTMESSAGETYPE_1, "CSmsPDU::ShortMessageType()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	return (TSmsProtocolIdentifier::TSmsShortMessageType) ProtocolIdentifier()->ShortMessageType();
	} // TSmsProtocolIdentifier::TSmsShortMessageType


/**
 *  Sets the Short Message Type in the PID field.
 *  
 *  @param aShortMessageType Short Message Type
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetShortMessageType(TSmsProtocolIdentifier::TSmsShortMessageType aShortMessageType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETSHORTMESSAGETYPE_1, "CSmsPDU::SetShortMessageType()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	TSmsProtocolIdentifier* protocolidentifier=(TSmsProtocolIdentifier*) ProtocolIdentifier();
	protocolidentifier->SetShortMessageType(aShortMessageType);
	} // CSmsPDU::SetShortMessageType


/**
 *  Gets the Telematic device type in the PID field.
 *  
 *  @return Telematic device type
 *  @capability None
 */
EXPORT_C TSmsProtocolIdentifier::TSmsTelematicDeviceType CSmsPDU::TelematicDeviceType() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_TELEMATICDEVICETYPE_1, "CSmsPDU::TelematicDeviceType()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	return ProtocolIdentifier()->TelematicDeviceType();
	} // TSmsProtocolIdentifier::TSmsTelematicDeviceType


/**
 *  Sets the Telematic device type in the PID field.
 *  
 *  @param aDeviceType Telematic device type
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetTelematicDeviceType(TSmsProtocolIdentifier::TSmsTelematicDeviceType aDeviceType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETTELEMATICDEVICETYPE_1, "CSmsPDU::SetTelematicDeviceType()");

	__ASSERT_DEBUG(ProtocolIdentifier()!=NULL,Panic(KGsmuPanicProtocolIdentifierNotPresent));
	TSmsProtocolIdentifier* protocolidentifier=(TSmsProtocolIdentifier*) ProtocolIdentifier();
	protocolidentifier->SetTelematicDeviceType(aDeviceType);
	} // CSmsPDU::SetTelematicDeviceType


/**
 *  Tests if the User Data Header Indicator is set.
 *  
 *  It panics if PDU type is unsupported.
 *  
 *  @return True if the User Data Header Indicator is set
 *  @capability None
 */
EXPORT_C TBool CSmsPDU::UserDataPresent() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_USERDATAPRESENT_1, "CSmsPDU::UserDataPresent()");

	TBool udPresent=ETrue;
	switch (iSmsPDUType)
		{
		case ESmsDeliver:
		case ESmsSubmit:
			break;
		case ESmsDeliverReport:
		case ESmsSubmitReport:
			udPresent=ParameterIndicator()->UserDataPresent();
			break;
		case ESmsStatusReport:
			udPresent=((CSmsStatusReport*)this)->ParameterIndicatorPresent()
				&& ParameterIndicator()->UserDataPresent();
			break;
		default:
			Panic(KGsmuPanicUnsupportedPduType);
		}
	return udPresent;
	} // CSmsPDU::UserDataPresent


/**
 *  Sets or unsets the User Data Header Indicator.
 *  
 *  It panics if PDU type is unsupported.
 *  
 *  @param aPresent True to set the User Data Header Indicator
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetUserDataPresent(TBool aPresent)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETUSERDATAPRESENT_1, "CSmsPDU::SetUserDataPresent(): aPresent=%d", aPresent);

	__ASSERT_DEBUG(ParameterIndicator()!=NULL,Panic(KGsmuPanicParameterIndicatorNotPresent));
	TSmsParameterIndicator* parameterindicator=(TSmsParameterIndicator*) ParameterIndicator();
	parameterindicator->SetUserDataPresent(aPresent);
	} // CSmsPDU::SetUserDataPresent


/**
 *  Tests if data coding scheme is present.
 *  
 *  Panics if PDU type is unsupported.
 *  
 *  @return True if data coding scheme is present
 *  @capability None
 */
EXPORT_C TBool CSmsPDU::DataCodingSchemePresent() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_DATACODINGSCHEMEPRESENT_1, "CSmsPDU::DataCodingSchemePresent()");

	TBool dcsPresent=ETrue;
	switch (iSmsPDUType)
		{
		case ESmsDeliver:
		case ESmsSubmit:
			break;
		case ESmsDeliverReport:
		case ESmsSubmitReport:
			dcsPresent=ParameterIndicator()->DataCodingSchemePresent();
			break;
		case ESmsStatusReport:
			dcsPresent=((CSmsStatusReport*)this)->ParameterIndicatorPresent()
				&& ParameterIndicator()->DataCodingSchemePresent();
			break;
		case ESmsCommand:
			dcsPresent=EFalse;
			break;
		default:
			Panic(KGsmuPanicUnsupportedPduType);
		}
	return dcsPresent;
	} // CSmsPDU::DataCodingSchemePresent


/**
 *  Sets if data coding scheme is present.
 *  
 *  @param aPresent True if data coding scheme is present
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetDataCodingSchemePresent(TBool aPresent)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETDATACODINGSCHEMEPRESENT_1, "CSmsPDU::SetDataCodingSchemePresent(): aPresent=%d", aPresent);

	__ASSERT_DEBUG(ParameterIndicator()!=NULL,Panic(KGsmuPanicParameterIndicatorNotPresent));
	TSmsParameterIndicator* parameterindicator=(TSmsParameterIndicator*) ParameterIndicator();
	parameterindicator->SetDataCodingSchemePresent(aPresent);
	} // CSmsPDU::SetDataCodingSchemePresent


/**
 *  Tests if Protocol Identifier is present.
 *  
 *  It panics if the PDU type is unsupported.
 *  
 *  @return True if Protocol Identifier is present
 *  @capability None
 */
EXPORT_C TBool CSmsPDU::ProtocolIdentifierPresent() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_PROTOCOLIDENTIFIERPRESENT_1, "CSmsPDU::ProtocolIdentifierPresent()");

	TBool pidPresent=ETrue;
	switch (iSmsPDUType)
		{
		case ESmsDeliver:
		case ESmsSubmit:
		case ESmsCommand:
			break;
		case ESmsDeliverReport:
		case ESmsSubmitReport:
			pidPresent=ParameterIndicator()->ProtocolIdentifierPresent();
			break;
		case ESmsStatusReport:
			pidPresent=((CSmsStatusReport*)this)->ParameterIndicatorPresent()
				&& ParameterIndicator()->ProtocolIdentifierPresent();
			break;
		default:
			Panic(KGsmuPanicUnsupportedPduType);
		}
	return pidPresent;
	} // CSmsPDU::ProtocolIdentifierPresent


/**
 *  Sets if Protocol Identifier is present.
 *  
 *  It panics if the PDU type is unsupported.
 *  
 *  @param aPresent True if Protocol Identifier is present
 *  @capability None
 */
EXPORT_C void CSmsPDU::SetProtocolIdentifierPresent(TBool aPresent)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_SETPROTOCOLIDENTIFIERPRESENT_1, "CSmsPDU::SetProtocolIdentifierPresent(): aPresent=%d", aPresent);

	__ASSERT_DEBUG(ParameterIndicator()!=NULL,Panic(KGsmuPanicParameterIndicatorNotPresent));
	TSmsParameterIndicator* parameterindicator=(TSmsParameterIndicator*) ParameterIndicator();
	parameterindicator->SetProtocolIdentifierPresent(aPresent);
	} // CSmsPDU::SetProtocolIdentifierPresent


/**
 *  
 *  Gets User Data (non-const).
 *  
 *  @return User Data
 *  @capability None
 */
EXPORT_C CSmsUserData& CSmsPDU::UserData()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_USERDATA_1, "CSmsPDU::UserData()");

	__ASSERT_DEBUG(UserDataPtr()!=NULL,Panic(KGsmuPanicUserDataNotPresent));
	CSmsUserData* userdata=(CSmsUserData*) UserDataPtr();
	return *userdata;
	} // CSmsPDU::UserData


/**
 *  Gets User Data (const).
 *  
 *  @return User Data
 *  @capability None
 */
EXPORT_C const CSmsUserData& CSmsPDU::UserData() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSPDU_USERDATA1_1, "CSmsPDU::UserData()");

	__ASSERT_DEBUG(UserDataPtr()!=NULL,Panic(KGsmuPanicUserDataNotPresent));
	return *UserDataPtr();
	} // CSmsPDU::UserData


CSmsPDU::CSmsPDU(TSmsPDUType aSmsPDUType):
	iSmsPDUType(aSmsPDUType)
	{
	// NOP
	} // CSmsPDU::CSmsPDU


const TSmsDataCodingScheme* CSmsPDU::DataCodingScheme() const
    {
    // Ignore in code coverage - for PDUs that are meant to have a DCS
    // this method is overridden; the base class implementation is not
    // intended to be used.
    BULLSEYE_OFF
    return NULL;
    BULLSEYE_RESTORE
    }

const TSmsProtocolIdentifier* CSmsPDU::ProtocolIdentifier() const
    {
    // Ignore in code coverage - for PDUs that are meant to have a PID
    // this method is overridden; the base class implementation is not
    // intended to be used.
    BULLSEYE_OFF
    return NULL;
    BULLSEYE_RESTORE
    }

const TSmsParameterIndicator* CSmsPDU::ParameterIndicator() const
    {
    // Ignore in code coverage - for PDUs that are meant to have a PI
    // this method is overridden; the base class implementation is not
    // intended to be used.
    BULLSEYE_OFF
    return NULL;
    BULLSEYE_RESTORE
    }

const CSmsUserData* CSmsPDU::UserDataPtr() const
    {
    // Ignore in code coverage - for PDUs that are meant to have a UD
    // this method is overridden; the base class implementation is not
    // intended to be used.
    BULLSEYE_OFF
    return NULL;
    BULLSEYE_RESTORE
    }

const CSmsAddress* CSmsPDU::ToFromAddressPtr() const
    {
    return NULL;
    }

TBool CSmsPDU::DoTextConcatenated(TInt& aIndex,TBool* aIs16Bit) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDU_DOTEXTCONCATENATED_1, "CSmsPDU::DoTextConcatenated()");

	TBool is8bit=UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference,aIndex);
	TBool is16bit=EFalse;
	if (!is8bit)
		is16bit=UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIConcatenatedShortMessages16BitReference,aIndex);
	if (aIs16Bit!=NULL)
		*aIs16Bit=is16bit;
	return is8bit || is16bit;
	} // CSmsPDU::DoTextConcatenated


void CSmsPDU::DoSetTextConcatenatedL(TBool aIs16Bit)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDU_DOSETTEXTCONCATENATEDL_1, "CSmsPDU::DoSetTextConcatenatedL(): aIs16Bit=%d", aIs16Bit);

	if (!aIs16Bit)
		{
		TBuf8<3> data;
		data.SetLength(3);
		data[0]=1;
		data[1]=1;
		data[2]=1;
		UserData().AddInformationElementL(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference,data);
		}
	else
		{
		TBuf8<4> data;
		data.SetLength(4);
		data[0]=0;
		data[1]=1;
		data[2]=1;
		data[3]=1;
		UserData().AddInformationElementL(CSmsInformationElement::ESmsIEIConcatenatedShortMessages16BitReference,data);
		}
	} // CSmsPDU::DoSetTextConcatenatedL


TBool CSmsPDU::DoApplicationPortAddressing(TInt& aIndex,TInt& aDestination,TInt& aOriginator,TBool* aIs16Bit) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDU_DOAPPLICATIONPORTADDRESSING_1, "CSmsPDU::DoApplicationPortAddressing()");

	TBool is8bit=UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit,aIndex);
	TBool is16bit=EFalse;
	if (is8bit)
		{
		TPtr8 data=UserData().InformationElement(aIndex).Data();
		aDestination=data[0];
		aOriginator=data[1];
		}
	else
		{
		is16bit=UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIApplicationPortAddressing16Bit,aIndex);
		if (is16bit)
			{
			TPtr8 data=UserData().InformationElement(aIndex).Data();
			aDestination=data[0]<<8;
			aDestination+=data[1];
			aOriginator=data[2]<<8;
			aOriginator+=data[3];
			}
		}
	if (aIs16Bit!=NULL)
		*aIs16Bit=is16bit;
	return is8bit || is16bit;
	} // CSmsPDU::DoApplicationPortAddressing


void CSmsPDU::DoSetApplicationPortAddressingL(TInt aDestination,TInt aOriginator,TBool aIs16Bit)
	{
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDU_DOSETAPPLICATIONPORTADDRESSINGL_1, "CSmsPDU::DoSetApplicationPortAddressingL(): aDestination=%d, aOriginator=%d, aIs16Bit=%d",aDestination, aOriginator, aIs16Bit);

	if (!aIs16Bit)
		{
		__ASSERT_ALWAYS((aDestination>=0x00) && (aDestination<=0xFF) && (aOriginator>=0x00) && (aOriginator<=0xFF),Panic(KGsmuPanicPortOutOfRange));
		TBuf8<2> data;
		data.SetLength(2);
		data[0]=(TUint8) aDestination;
		data[1]=(TUint8) aOriginator;
		UserData().AddInformationElementL(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit,data);
		}
	else
		{
		__ASSERT_ALWAYS((aDestination>=0x00) && (aDestination<=0xFFFF) && (aOriginator>=0x00) && (aOriginator<=0xFFFF),Panic(KGsmuPanicPortOutOfRange));
		TBuf8<4> data;
		data.SetLength(4);
		data[0]=(TUint8) (aDestination>>8);
		data[1]=(TUint8) aDestination;
		data[2]=(TUint8) (aOriginator>>8);
		data[3]=(TUint8) aOriginator;
		UserData().AddInformationElementL(CSmsInformationElement::ESmsIEIApplicationPortAddressing16Bit,data);
		}
	} // CSmsPDU::DoSetApplicationPortAddressingL


CSmsDeliver::CSmsDeliver():
	CSmsPDU(ESmsDeliver),
	iFirstOctet(TSmsFirstOctet::ESmsMTIDeliverOrDeliverReport)
	{
	//NOP
	} // TSmsFirstOctet::ESmsMTIDeliverOrDeliverReport


/**
 *  Gets More Messages to Send flag.
 *  
 *  @return True if More Messages to Send flag set
 *  @capability None
 */
EXPORT_C TBool CSmsDeliver::MoreMessagesToSend() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_MOREMESSAGESTOSEND_1, "CSmsDeliver::MoreMessagesToSend");

	return (iFirstOctet&TSmsFirstOctet::ESmsMoreMessagesToSendMask)==TSmsFirstOctet::ESmsMoreMessagesToSend;
	} // CSmsDeliver::MoreMessagesToSend


/**
 *  Sets More Messages to Send flag.
 *  
 *  @param aMore True if More Messages to Send
 *  @capability None
 */
EXPORT_C void CSmsDeliver::SetMoreMessagesToSend(TBool aMore)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_SETMOREMESSAGESTOSEND_1, "CSmsDeliver::SetMoreMessagesToSend(): aMore=%d", aMore);

	iFirstOctet=aMore? (iFirstOctet&(~TSmsFirstOctet::ESmsMoreMessagesToSendMask)|TSmsFirstOctet::ESmsMoreMessagesToSend):
	                   (iFirstOctet&(~TSmsFirstOctet::ESmsMoreMessagesToSendMask)|TSmsFirstOctet::ESmsNoMoreMessagesToSend);
	} // CSmsDeliver::SetMoreMessagesToSend


/**
 *  Gets Reply Path flag.
 *  
 *  If a reply path exists, the service center encoded in the DELIVER can be used
 *  to construct a SUBMIT reply.
 *  
 *  @return True if Reply Path exists
 *  @capability None
 */
EXPORT_C TBool CSmsDeliver::ReplyPath() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_REPLYPATH_1, "CSmsDeliver::ReplyPath");

	return (iFirstOctet&TSmsFirstOctet::ESmsReplyPathMask)==TSmsFirstOctet::ESmsReplyPathExists;
	} // CSmsDeliver::ReplyPath


/**
 *  Sets Reply Path flag.
 *  
 *  @param aReplyPath True to set Reply Path
 *  @capability None
 */
EXPORT_C void CSmsDeliver::SetReplyPath(TBool aReplyPath)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_SETREPLYPATH_1, "CSmsDeliver::SetReplyPath(): aReplyPath=%d", aReplyPath);

	iFirstOctet=aReplyPath? (iFirstOctet&(~TSmsFirstOctet::ESmsReplyPathMask)|TSmsFirstOctet::ESmsReplyPathExists):
	                        (iFirstOctet&(~TSmsFirstOctet::ESmsReplyPathMask)|TSmsFirstOctet::ESmsReplyPathNone);
	} // CSmsDeliver::SetReplyPath


/**
 *  Gets Status Report flag.
 *  
 *  @return True if Status Report to be returned.
 *  @capability None
 */
EXPORT_C TBool CSmsDeliver::StatusReportIndication() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_STATUSREPORTINDICATION_1, "CSmsDeliver::StatusReportIndication");

	return (iFirstOctet&TSmsFirstOctet::ESmsStatusReportIndicatorMask)==TSmsFirstOctet::ESmsStatusReportReturned;
	} // CSmsDeliver::StatusReportIndication


/**
 *  Sets Status Report flag.
 *  
 *  @param aIndication Set True to request Status Report
 *  @capability None
 */
EXPORT_C void CSmsDeliver::SetStatusReportIndication(TBool aIndication)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_SETSTATUSREPORTINDICATION_1, "CSmsDeliver::SetStatusReportIndication(): aIndication=%d", aIndication);

	iFirstOctet=aIndication? (iFirstOctet&(~TSmsFirstOctet::ESmsStatusReportIndicatorMask)|TSmsFirstOctet::ESmsStatusReportReturned):
	                         (iFirstOctet&(~TSmsFirstOctet::ESmsStatusReportIndicatorMask)|TSmsFirstOctet::ESmsStatusReportNotReturned);
	} // CSmsDeliver::SetStatusReportIndication


/**
 *  Destructor.
 */
CSmsDeliver::~CSmsDeliver()
	{
	delete iServiceCenterAddress;
	delete iOriginalAddress;
	delete iUserData;
	} // CSmsDeliver::SetStatusReportIndication


/**
 *  Gets Service Center Time Stamp.
 *  
 *  @param aTime Service Center Time Stamp represented in Universal Time
 *  @param aNumQuarterHours +/- Time Zone difference to GMT in quarter hours
 *  @capability None
 */
EXPORT_C void CSmsDeliver::ServiceCenterTimeStamp(TTime& aTime,TInt& aNumQuarterHours)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_SERVICECENTERTIMESTAMP_1, "CSmsDeliver::ServiceCenterTimeStamp()");

	aTime=iServiceCenterTimeStamp.Time();
	aNumQuarterHours=iServiceCenterTimeStamp.TimeOffset();
	} // CSmsDeliver::ServiceCenterTimeStamp


/**
 *  Sets Service Center Time Stamp.
 *  
 *  @param aTime Service Center Time Stamp represented in Universal Time
 *  @param aNumQuarterHours +/- Time Zone difference to GMT in quarter hours
 *  @capability None
 */
EXPORT_C void CSmsDeliver::SetServiceCenterTimeStamp(const TTime& aTime,TInt aNumQuarterHours)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_SETSERVICECENTERTIMESTAMP_1, "CSmsDeliver::ServiceCenterTimeStamp(): aNumQuarterHours=%d", aNumQuarterHours);

	iServiceCenterTimeStamp.SetTime(aTime);
	iServiceCenterTimeStamp.SetTimeOffset(aNumQuarterHours);
	} // CSmsDeliver::SetServiceCenterTimeStamp


void CSmsDeliver::ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_CONSTRUCTL_1, "CSmsDeliver::ConstructL()");

	iServiceCenterAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iOriginalAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iUserData=CSmsUserData::NewL(aCharacterSetConverter,aFs,iFirstOctet,iDataCodingScheme);
	} // CSmsDeliver::ConstructL


/**
 *  Duplicates this CSmsDeliver object.
 * 
 *  @return  Pointer to the newly created CSmsDeliver object.
 */
EXPORT_C CSmsDeliver* CSmsDeliver::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_DUPLICATEL_1, "CSmsDeliver::DuplicateL()");

	CSmsDeliver*  smsDeliver = new (ELeave) CSmsDeliver();
	CleanupStack::PushL(smsDeliver);

	smsDeliver->iServiceCenterAddress   = iServiceCenterAddress->DuplicateL();
	smsDeliver->iFirstOctet             = iFirstOctet;
	smsDeliver->iOriginalAddress        = iOriginalAddress->DuplicateL();
	smsDeliver->iProtocolIdentifier     = iProtocolIdentifier;
	smsDeliver->iDataCodingScheme       = iDataCodingScheme;
	smsDeliver->iServiceCenterTimeStamp = iServiceCenterTimeStamp;
	smsDeliver->iUserData               = iUserData->DuplicateL(smsDeliver->iFirstOctet,
																smsDeliver->iDataCodingScheme);

	CleanupStack::Pop(smsDeliver);

	return smsDeliver;
	} // CSmsDeliver::DuplicateL


TUint8* CSmsDeliver::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_ENCODEL_1, "CSmsDeliver::EncodeL()");

	aPtr=iFirstOctet.EncodeL(aPtr);
	aPtr=iOriginalAddress->EncodeL(aPtr);
	aPtr=iProtocolIdentifier.EncodeL(aPtr);
	aPtr=iDataCodingScheme.EncodeL(aPtr);
	aPtr=iServiceCenterTimeStamp.EncodeL(aPtr);
	return iUserData->EncodeL(aPtr);
	} // CSmsDeliver::EncodeL

TUint8* CSmsDeliver::EncodeL(TUint8* aPtr, const TEncodeParams* ) const
	{
	return EncodeL(aPtr);		
	}	

void CSmsDeliver::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_DECODEL_1, "CSmsDeliver::DecodeL()");

	iFirstOctet.DecodeL(aPdu);
	iOriginalAddress->DecodeL(aPdu);
	iProtocolIdentifier.DecodeL(aPdu);
	iDataCodingScheme.DecodeL(aPdu);
	TInt bit7to4=iDataCodingScheme.Bits7To4();
	TInt lowerLimit = TSmsDataCodingScheme::ESmsDCSReserved5;
	TInt upperLimit = TSmsDataCodingScheme::ESmsDCSReserved8;
	if ((bit7to4>=lowerLimit) &&
	    (bit7to4<=upperLimit))
		User::Leave(KErrGsmSMSUnspecifiedDCSError);

	TInt timeError = KErrNone;
	iServiceCenterTimeStamp.DecodeL(aPdu, timeError);  //  Review, fix for bug
	if (timeError != KErrNone)
		{
		TTime time;
		time.UniversalTime();
		iServiceCenterTimeStamp.SetTime(time);
		iServiceCenterTimeStamp.SetTimeOffset((User::UTCOffset().Int()) / CSmsMessage::E15MinutesRepresentedInSeconds);
		}

	iUserData->DecodeL(aPdu);
	} // CSmsDeliver::DecodeL


void CSmsDeliver::InternalizeMessagePDUL(RReadStream& aStream)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_INTERNALIZEMESSAGEPDUL_1, "CSmsDeliver::InternalizeMessagePDUL()");

	iServiceCenterAddress->InternalizeL(aStream);

	aStream >> iFirstOctet;
	iOriginalAddress->InternalizeL(aStream);
/*	TUint8 tmp;
	aStream >> tmp;
	SetMessageConversion((TSmsProtocolIdentifier::TSmsPIDConversion)tmp);
*/	aStream >> iProtocolIdentifier;
	aStream >> iDataCodingScheme;
	aStream >> iServiceCenterTimeStamp;

	aStream >> *iUserData;
	} // CSmsDeliver::InternalizeMessagePDUL


void CSmsDeliver::ExternalizeMessagePDUL(RWriteStream& aStream) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_EXTERNALIZEMESSAGEPDUL_1, "CSmsDeliver::ExternalizeMessagePDUL()");

	iServiceCenterAddress->ExternalizeL(aStream);

	aStream << iFirstOctet;
	iOriginalAddress->ExternalizeL(aStream);
/*	TSmsProtocolIdentifier::TSmsPIDConversion tmp;
	tmp=MessageConversion();
	aStream << (TUint8)tmp;
*/	aStream << iProtocolIdentifier;
	aStream << iDataCodingScheme;
	aStream << iServiceCenterTimeStamp;

	aStream << *iUserData;
	} // CSmsDeliver::ExternalizeMessagePDUL


const TSmsDataCodingScheme* CSmsDeliver::DataCodingScheme() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_DATACODINGSCHEME_1, "CSmsDeliver::DataCodingScheme()");

	return &iDataCodingScheme;
	} // CSmsDeliver::DataCodingScheme


/**
 *  Gets the Deliver PID field.
 *  
 *  @return The Deliver PID field
 *  @capability None
 */
EXPORT_C const TSmsProtocolIdentifier* CSmsDeliver::ProtocolIdentifier() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVER_PROTOCOLIDENTIFIER_1, "CSmsDeliver::ProtocolIdentifier()");

	return &iProtocolIdentifier;
	} // CSmsDeliver::ProtocolIdentifier


const CSmsUserData* CSmsDeliver::UserDataPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_USERDATAPTR_1, "CSmsDeliver::UserDataPtr()");

	return iUserData;
	} // CSmsDeliver::UserDataPtr


const CSmsAddress* CSmsDeliver::ToFromAddressPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVER_TOFROMADDRESSPTR_1, "CSmsDeliver::ToFromAddressPtr()");

	return iOriginalAddress;
	} // CSmsDeliver::ToFromAddressPtr


CSmsSubmit::CSmsSubmit():
	CSmsPDU(ESmsSubmit),
	iFirstOctet(TSmsFirstOctet::ESmsMTISubmitOrSubmitReport|TSmsFirstOctet::ESmsNoMoreMessagesToSend|TSmsFirstOctet::ESmsVPFInteger),
	iValidityPeriod(iFirstOctet)
	{
	// NOP
	} // TSmsFirstOctet::ESmsMTISubmitOrSubmitReport


/**
 *  Destructor.
 */
CSmsSubmit::~CSmsSubmit()
	{
	delete iServiceCenterAddress;
	delete iDestinationAddress;
	delete iUserData;
	} // TSmsFirstOctet::ESmsMTISubmitOrSubmitReport


/**
 *  Gets Reject Duplicates flag.
 *  
 *  SUBMITs with duplicate message reference and destination address can be rejected.
 *  
 *  @return True if the SC is being instructed to reject duplicates
 *  @capability None
 */
EXPORT_C TBool CSmsSubmit::RejectDuplicates() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_REJECTDUPLICATES_1, "CSmsSubmit::RejectDuplicates()");

	return (iFirstOctet&TSmsFirstOctet::ESmsRejectDuplicatesMask)==TSmsFirstOctet::ESmsRejectDuplicates;
	} // CSmsSubmit::RejectDuplicates


/**
 *  Sets Reject Duplicates flag.
 *  
 *  @param aRejectDuplicates True to instruct the SC to reject duplicates
 *  @capability None
 */
EXPORT_C void CSmsSubmit::SetRejectDuplicates(TBool aRejectDuplicates)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_SETREJECTDUPLICATES_1, "CSmsSubmit::SetRejectDuplicates(): aRejectDuplicates=%d", aRejectDuplicates);

	iFirstOctet=aRejectDuplicates? (iFirstOctet&(~TSmsFirstOctet::ESmsRejectDuplicatesMask)|TSmsFirstOctet::ESmsRejectDuplicates):
	                        (iFirstOctet&(~TSmsFirstOctet::ESmsRejectDuplicatesMask)|TSmsFirstOctet::ESmsAcceptDuplicates);
	} // CSmsSubmit::SetRejectDuplicates


/**
 *  Gets the Validity Period Format.
 *  
 *  @return Validity Period Format
 *  @capability None
 */
EXPORT_C TSmsFirstOctet::TSmsValidityPeriodFormat CSmsSubmit::ValidityPeriodFormat() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_VALIDITYPERIODFORMAT_1, "CSmsSubmit::ValidityPeriodFormat()");

	return iValidityPeriod.ValidityPeriodFormat();
	} // TSmsFirstOctet::TSmsValidityPeriodFormat


/**
 *  Sets the Validity Period Format.
 *  
 *  @param aValidityPeriodFormat Validity Period Format
 *  @capability None
 */
EXPORT_C void CSmsSubmit::SetValidityPeriodFormat(TSmsFirstOctet::TSmsValidityPeriodFormat aValidityPeriodFormat)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_SETVALIDITYPERIODFORMAT_1, "CSmsSubmit::SetValidityPeriodFormat()");

	iValidityPeriod.SetValidityPeriodFormat(aValidityPeriodFormat);
	} // CSmsSubmit::SetValidityPeriodFormat


/**
 *  Gets Reply Path flag.
 *  
 *  If a Reply Path exists, the recipient of the SMS can reply using the same
 *  service center address.
 *  
 *  @return True if Reply Path exists
 *  @capability None
 */
EXPORT_C TBool CSmsSubmit::ReplyPath() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_REPLYPATH_1, "CSmsSubmit::ReplyPath()");

	return (iFirstOctet&TSmsFirstOctet::ESmsReplyPathMask)==TSmsFirstOctet::ESmsReplyPathExists;
	} // CSmsSubmit::ReplyPath


/**
 *  Sets Reply Path flag.
 *  
 *  @param aReplyPath Set to True to set Reply Path
 *  @capability None
 */
EXPORT_C void CSmsSubmit::SetReplyPath(TBool aReplyPath)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_SETREPLYPATH_1, "CSmsSubmit::SetReplyPath(): aReplyPath=%d", aReplyPath);

	iFirstOctet=aReplyPath? (iFirstOctet&(~TSmsFirstOctet::ESmsReplyPathMask)|TSmsFirstOctet::ESmsReplyPathExists):
	                        (iFirstOctet&(~TSmsFirstOctet::ESmsReplyPathMask)|TSmsFirstOctet::ESmsReplyPathNone);
	} // CSmsSubmit::SetReplyPath


/**
 *  Gets Status Report Request flag.
 *  
 *  A sender can request STATUS REPORTs for the SUBMIT being sent.
 *  
 *  @return True if the sender is requesting Status Reports
 *  @capability None
 */
EXPORT_C TBool CSmsSubmit::StatusReportRequest() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_STATUSREPORTREQUEST_1, "CSmsSubmit::StatusReportRequest()");

	return (iFirstOctet&TSmsFirstOctet::ESmsStatusReportRequestMask)==TSmsFirstOctet::ESmsStatusReportRequested;
	} // CSmsSubmit::StatusReportRequest


/**
 *  Sets Status Report Request flag.
 *  
 *  @param aRequest Status Report Request flag
 *  @capability None
 */
EXPORT_C void CSmsSubmit::SetStatusReportRequest(TBool aRequest)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_SETSTATUSREPORTREQUEST_1, "CSmsSubmit::SetStatusReportRequest(): aRequest=%d", aRequest);

	iFirstOctet=aRequest? (iFirstOctet&(~TSmsFirstOctet::ESmsStatusReportRequestMask)|TSmsFirstOctet::ESmsStatusReportRequested):
	                      (iFirstOctet&(~TSmsFirstOctet::ESmsStatusReportRequestMask)|TSmsFirstOctet::ESmsStatusReportNotRequested);
	} // CSmsSubmit::SetStatusReportRequest


/**
 *  Gets the Message Reference.
 *  
 *  @return Message Reference
 *  @capability None
 */
EXPORT_C TInt CSmsSubmit::MessageReference() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_MESSAGEREFERENCE_1, "CSmsSubmit::MessageReference()");

	return iMessageReference;
	} // CSmsSubmit::MessageReference


/**
 *  Sets the Message Reference.
 *  
 *  @param aMessageReference Message Reference
 *  @capability None
 */
EXPORT_C void CSmsSubmit::SetMessageReference(TInt aMessageReference)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_SETMESSAGEREFERENCE_1, "CSmsSubmit::SetMessageReference(): aMessageReference=%d",aMessageReference);
	iMessageReference=aMessageReference;
	} // CSmsSubmit::SetMessageReference


/**
 *  Gets the Validity Period for the SUBMIT.
 *  
 *  @return Validity Period
 *  @capability None
 */
EXPORT_C const TTimeIntervalMinutes&  CSmsSubmit::ValidityPeriod() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_VALIDITYPERIOD_1, "CSmsSubmit::ValidityPeriod()");

	return iValidityPeriod.TimeIntervalMinutes();
	} // CSmsSubmit::ValidityPeriod


/**
 *  Sets the Validity Period for the SUBMIT.
 *  
 *  @param aTimeIntervalMinutes Validity Period
 *  @capability None
 */
EXPORT_C void CSmsSubmit::SetValidityPeriod(const TTimeIntervalMinutes& aTimeIntervalMinutes)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_SETVALIDITYPERIOD_1, "CSmsSubmit::SetValidityPeriod(): %d",aTimeIntervalMinutes.Int());

	iValidityPeriod.SetTimeIntervalMinutes(aTimeIntervalMinutes);
	} // CSmsSubmit::SetValidityPeriod


const TSmsDataCodingScheme* CSmsSubmit::DataCodingScheme() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_DATACODINGSCHEME_1, "CSmsSubmit::DataCodingScheme()");

	return &iDataCodingScheme;
	} // CSmsSubmit::DataCodingScheme


const TSmsProtocolIdentifier* CSmsSubmit::ProtocolIdentifier() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_PROTOCOLIDENTIFIER_1, "CSmsSubmit::ProtocolIdentifier()");

	return &iProtocolIdentifier;
	} // CSmsSubmit::ProtocolIdentifier


const CSmsUserData* CSmsSubmit::UserDataPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_USERDATAPTR_1, "CSmsSubmit::UserDataPtr()");

	return iUserData;
	} // CSmsSubmit::UserDataPtr


const CSmsAddress* CSmsSubmit::ToFromAddressPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_TOFROMADDRESSPTR_1, "CSmsSubmit::ToFromAddressPtr()");

	return iDestinationAddress;
	} // CSmsSubmit::ToFromAddressPtr


void CSmsSubmit::ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_CONSTRUCTL_1, "CSmsSubmit::ConstructL()");

	iServiceCenterAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iDestinationAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iUserData=CSmsUserData::NewL(aCharacterSetConverter,aFs,iFirstOctet,iDataCodingScheme);
	} // CSmsSubmit::ConstructL


/**
 *  Duplicates this CSmsSubmit object.
 * 
 *  @return  Pointer to the newly created CSmsSubmit object.
 */
EXPORT_C CSmsSubmit* CSmsSubmit::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMIT_DUPLICATEL_1, "CSmsSubmit::DuplicateL()");

	CSmsSubmit*  smsSubmit = new (ELeave) CSmsSubmit();
	CleanupStack::PushL(smsSubmit);
	
	smsSubmit->iServiceCenterAddress = iServiceCenterAddress->DuplicateL();
	smsSubmit->iDestinationAddress   = iDestinationAddress->DuplicateL();
	smsSubmit->iUserData             = iUserData->DuplicateL(smsSubmit->iFirstOctet,
															 smsSubmit->iDataCodingScheme);
	smsSubmit->iFirstOctet           = iFirstOctet;
	smsSubmit->iMessageReference     = iMessageReference;
	smsSubmit->iProtocolIdentifier   = iProtocolIdentifier;
	smsSubmit->iDataCodingScheme     = iDataCodingScheme;
	smsSubmit->iValidityPeriod.SetValidityPeriodFormat(iValidityPeriod.ValidityPeriodFormat());
	smsSubmit->iValidityPeriod.SetTimeIntervalMinutes(iValidityPeriod.TimeIntervalMinutes());
	
	CleanupStack::Pop(smsSubmit);

	return smsSubmit;
	} // CSmsSubmit::DuplicateL


TUint8* CSmsSubmit::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_ENCODEL_1, "CSmsSubmit::EncodeL()");

	aPtr=iFirstOctet.EncodeL(aPtr);
	aPtr=iMessageReference.EncodeL(aPtr);
	aPtr=iDestinationAddress->EncodeL(aPtr);
	aPtr=iProtocolIdentifier.EncodeL(aPtr);
	aPtr=iDataCodingScheme.EncodeL(aPtr);
	aPtr=iValidityPeriod.EncodeL(aPtr);
	return iUserData->EncodeL(aPtr);
	} // CSmsSubmit::EncodeL

TUint8* CSmsSubmit::EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const		
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_ENCODEL1_1, "CSmsSubmit::EncodeL()");

	aPtr=iFirstOctet.EncodeL(aPtr);
	aPtr=iMessageReference.EncodeL(aPtr);
	aPtr=iDestinationAddress->EncodeL(aPtr);
	aPtr=iProtocolIdentifier.EncodeL(aPtr);
	aPtr=iDataCodingScheme.EncodeL(aPtr);
	aPtr=iValidityPeriod.EncodeL(aPtr, aEncodeParams);
	return iUserData->EncodeL(aPtr);
	} // CSmsSubmit::EncodeL

void CSmsSubmit::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_DECODEL_1, "CSmsSubmit::DecodeL()");

	iFirstOctet.DecodeL(aPdu);
	iMessageReference.DecodeL(aPdu);
	iDestinationAddress->DecodeL(aPdu);
	iProtocolIdentifier.DecodeL(aPdu);
	iDataCodingScheme.DecodeL(aPdu);
	TInt bit7to4=iDataCodingScheme.Bits7To4();
	TInt lowerLimit = TSmsDataCodingScheme::ESmsDCSReserved5;
	TInt upperLimit = TSmsDataCodingScheme::ESmsDCSReserved8;
	if ((bit7to4>=lowerLimit) &&
	    (bit7to4<=upperLimit))
		User::Leave(KErrGsmSMSUnspecifiedDCSError);
	iValidityPeriod.DecodeL(aPdu);
	iUserData->DecodeL(aPdu);
	} // CSmsSubmit::DecodeL


void CSmsSubmit::InternalizeMessagePDUL(RReadStream& aStream)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMIT_INTERNALIZEMESSAGEPDUL_1, "CSmsSubmit::InternalizeMessagePDUL()");

	iServiceCenterAddress->InternalizeL(aStream);

	aStream >> iFirstOctet;
	aStream >> iMessageReference;
	iDestinationAddress->InternalizeL(aStream);
	aStream >> iProtocolIdentifier;
	aStream >> iDataCodingScheme;
	aStream >> iValidityPeriod;

	aStream >> *iUserData;
	} // CSmsSubmit::InternalizeMessagePDUL


void CSmsSubmit::ExternalizeMessagePDUL(RWriteStream& aStream) const
	{
	iServiceCenterAddress->ExternalizeL(aStream);

	aStream << iFirstOctet;
	aStream << iMessageReference;
	iDestinationAddress->ExternalizeL(aStream);
	aStream << iProtocolIdentifier;
	aStream << iDataCodingScheme;
	aStream << iValidityPeriod;

	aStream << *iUserData;
	} // CSmsSubmit::ExternalizeMessagePDUL


CSmsDeliverReport::CSmsDeliverReport(TBool aIsRPError):
	CSmsPDU(ESmsDeliverReport),
	iIsRPError((TUint8) aIsRPError),
	iFirstOctet(TSmsFirstOctet::ESmsMTIDeliverOrDeliverReport)
	{
	} // TSmsFirstOctet::ESmsMTIDeliverOrDeliverReport


/**
 *  Destructor.
 */
CSmsDeliverReport::~CSmsDeliverReport()
	{
	delete iServiceCenterAddress;
	delete iUserData;
	} // TSmsFirstOctet::ESmsMTIDeliverOrDeliverReport


/**
 *  Gets the Failure Cause.
 *  
 *  @return The Failure Cause
 *  @capability None
 */
EXPORT_C TInt CSmsDeliverReport::FailureCause() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVERREPORT_FAILURECAUSE_1, "CSmsDeliverReport::FailureCause()");

	__ASSERT_DEBUG(iIsRPError,Panic(KGsmuPanicNotRPError));
	return iFailureCause.Error();
	} // CSmsDeliverReport::FailureCause


/**
 *  Sets the Failure Cause.
 *  
 *  @param aFailureCause The Failure Cause
 *  @capability None
 */
EXPORT_C void CSmsDeliverReport::SetFailureCause(TSmsFailureCause::TSmsFailureCauseError aFailureCause)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVERREPORT_SETFAILURECAUSE_1, "CSmsDeliverReport::SetFailureCause()");

	__ASSERT_DEBUG(iIsRPError,Panic(KGsmuPanicNotRPError));
	iFailureCause.SetError(aFailureCause);
	} // CSmsDeliverReport::SetFailureCause


const TSmsDataCodingScheme* CSmsDeliverReport::DataCodingScheme() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVERREPORT_DATACODINGSCHEME_1, "CSmsDeliverReport::DataCodingScheme()");

	__ASSERT_DEBUG(DataCodingSchemePresent(),Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return &iDataCodingScheme;
	} // CSmsDeliverReport::DataCodingScheme


const TSmsProtocolIdentifier* CSmsDeliverReport::ProtocolIdentifier() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVERREPORT_PROTOCOLIDENTIFIER_1, "CSmsDeliverReport::ProtocolIdentifier()");

	__ASSERT_DEBUG(ProtocolIdentifierPresent(),Panic(KGsmuPanicProtocolIdentifierNotPresent));
	return &iProtocolIdentifier;
	} // CSmsDeliverReport::ProtocolIdentifier


const TSmsParameterIndicator* CSmsDeliverReport::ParameterIndicator() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVERREPORT_PARAMETERINDICATOR_1, "CSmsDeliverReport::ParameterIndicator()");

	return &iParameterIndicator;
	} // CSmsDeliverReport::ParameterIndicator


const CSmsUserData* CSmsDeliverReport::UserDataPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVERREPORT_USERDATAPTR_1, "CSmsDeliverReport::UserDataPtr()");

	__ASSERT_DEBUG(UserDataPresent(),Panic(KGsmuPanicUserDataNotPresent));
	return iUserData;
	} // CSmsDeliverReport::UserDataPtr


void CSmsDeliverReport::ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVERREPORT_CONSTRUCTL_1, "CSmsDeliverReport::ConstructL()");

	iServiceCenterAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iUserData=CSmsUserData::NewL(aCharacterSetConverter,aFs,iFirstOctet,iDataCodingScheme);
	} // CSmsDeliverReport::ConstructL


/**
 *  Duplicates this CSmsDeliverReport object.
 * 
 *  @return  Pointer to the newly created CSmsDeliverReport object.
 */
EXPORT_C CSmsDeliverReport* CSmsDeliverReport::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSDELIVERREPORT_DUPLICATEL_1, "CSmsDeliverReport::DuplicateL()");

	CSmsDeliverReport*  smsDeliverReport = new (ELeave) CSmsDeliverReport(iIsRPError);
	CleanupStack::PushL(smsDeliverReport);

	smsDeliverReport->iServiceCenterAddress = iServiceCenterAddress->DuplicateL();
	smsDeliverReport->iFirstOctet           = iFirstOctet;
	smsDeliverReport->iFailureCause         = iFailureCause;
	smsDeliverReport->iParameterIndicator   = iParameterIndicator;
	smsDeliverReport->iProtocolIdentifier   = iProtocolIdentifier;
	smsDeliverReport->iDataCodingScheme     = iDataCodingScheme;
	smsDeliverReport->iUserData             = iUserData->DuplicateL(smsDeliverReport->iFirstOctet,
																	smsDeliverReport->iDataCodingScheme);

	CleanupStack::Pop(smsDeliverReport);

	return smsDeliverReport;
	} // CSmsDeliverReport::DuplicateL


TUint8* CSmsDeliverReport::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVERREPORT_ENCODEL_1, "CSmsDeliverReport::EncodeL()");

	aPtr=iFirstOctet.EncodeL(aPtr);
	if (iIsRPError)
		aPtr=iFailureCause.EncodeL(aPtr);
	aPtr=iParameterIndicator.EncodeL(aPtr);
	if (iParameterIndicator.ProtocolIdentifierPresent())
		aPtr=iProtocolIdentifier.EncodeL(aPtr);
	if (iParameterIndicator.DataCodingSchemePresent())
		aPtr=iDataCodingScheme.EncodeL(aPtr);
	if (iParameterIndicator.UserDataPresent())
		aPtr=iUserData->EncodeL(aPtr);
	return aPtr;
	} // CSmsDeliverReport::EncodeL

TUint8* CSmsDeliverReport::EncodeL(TUint8* aPtr, const TEncodeParams* ) const
	{
	return EncodeL(aPtr);		
	}	

void CSmsDeliverReport::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSDELIVERREPORT_DECODEL_1, "CSmsDeliverReport::DecodeL()");

	iFirstOctet.DecodeL(aPdu);
	if (iIsRPError)
		iFailureCause.DecodeL(aPdu);
	iParameterIndicator.DecodeL(aPdu);
	if (iParameterIndicator.Extension())
		{
		//
		//  Throw away rest of the pdu - some pdus received with this bit set don't appear to conform to 03.40 v7.4.0 spec.
		//  This appears safest option.
		//	TODO check new versions of 03.40 to make sre that it is still the right option
		//
		((TSmsOctet&)iParameterIndicator)=TSmsParameterIndicator::ESmsPIDExtension;
		return;
		}
	if (iParameterIndicator.ProtocolIdentifierPresent())
		iProtocolIdentifier.DecodeL(aPdu);
	if (iParameterIndicator.DataCodingSchemePresent())
		{
		iDataCodingScheme.DecodeL(aPdu);
		TInt bit7to4=iDataCodingScheme.Bits7To4();
		TInt lowerLimit = TSmsDataCodingScheme::ESmsDCSReserved5;
	    TInt upperLimit = TSmsDataCodingScheme::ESmsDCSReserved8;
       	if ((bit7to4>=lowerLimit) &&
	        (bit7to4<=upperLimit))
		    User::Leave(KErrGsmSMSUnspecifiedDCSError);
		}
	if (iParameterIndicator.UserDataPresent())
		{
		if (!iParameterIndicator.DataCodingSchemePresent())
		    {
		    iParameterIndicator.SetDataCodingSchemePresent(TBool (ETrue));
		    iDataCodingScheme = TSmsDataCodingScheme();
		    }

		iUserData->DecodeL(aPdu);
		}
	} // CSmsDeliverReport::DecodeL


void CSmsDeliverReport::InternalizeMessagePDUL(RReadStream& aStream)
	{
	iServiceCenterAddress->InternalizeL(aStream);

	aStream >> iIsRPError;

	aStream >> iFirstOctet;
	aStream >> iFailureCause;
	aStream >> iParameterIndicator;
	aStream >> iProtocolIdentifier;
	aStream >> iDataCodingScheme;
	aStream >> *iUserData;
	} // CSmsDeliverReport::InternalizeMessagePDUL


void CSmsDeliverReport::ExternalizeMessagePDUL(RWriteStream& aStream) const
	{
	iServiceCenterAddress->ExternalizeL(aStream);

	aStream << iIsRPError;

	aStream << iFirstOctet;
	aStream << iFailureCause;
	aStream << iParameterIndicator;
	aStream << iProtocolIdentifier;
	aStream << iDataCodingScheme;
	aStream << *iUserData;
	} // CSmsDeliverReport::ExternalizeMessagePDUL


CSmsSubmitReport::CSmsSubmitReport(TBool aIsRPError):
	CSmsPDU(ESmsSubmitReport),
	iIsRPError((TUint8) aIsRPError),
	iFirstOctet(TSmsFirstOctet::ESmsMTISubmitOrSubmitReport)
	{
	} // TSmsFirstOctet::ESmsMTISubmitOrSubmitReport


/**
 *  Destructor.
 */
CSmsSubmitReport::~CSmsSubmitReport()
	{
	delete iServiceCenterAddress;
	delete iUserData;
	} // TSmsFirstOctet::ESmsMTISubmitOrSubmitReport


/**
 *  Gets the Failure Cause.
 *  
 *  @return The Failure Cause
 *  @capability None
 */
EXPORT_C TInt CSmsSubmitReport::FailureCause() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMITREPORT_FAILURECAUSE_1, "CSmsSubmitReport::FailureCause()");

	__ASSERT_DEBUG(iIsRPError,Panic(KGsmuPanicNotRPError));
	return iFailureCause.Error();
	} // CSmsSubmitReport::FailureCause


/**
 *  Sets the Failure Cause.
 *  
 *  @param aFailureCause The Failure Cause
 *  @capability None
 */
EXPORT_C void CSmsSubmitReport::SetFailureCause(TSmsFailureCause::TSmsFailureCauseError aFailureCause)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMITREPORT_SETFAILURECAUSE_1, "CSmsSubmitReport::SetFailureCause()");

	__ASSERT_DEBUG(iIsRPError,Panic(KGsmuPanicNotRPError));
	iFailureCause.SetError(aFailureCause);
	} // CSmsSubmitReport::SetFailureCause


const TSmsDataCodingScheme* CSmsSubmitReport::DataCodingScheme() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMITREPORT_DATACODINGSCHEME_1, "CSmsSubmitReport::DataCodingScheme()");

	__ASSERT_DEBUG(DataCodingSchemePresent(),Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return &iDataCodingScheme;
	} // CSmsSubmitReport::DataCodingScheme


const TSmsProtocolIdentifier* CSmsSubmitReport::ProtocolIdentifier() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMITREPORT_PROTOCOLIDENTIFIER_1, "CSmsSubmitReport::ProtocolIdentifier()");

	__ASSERT_DEBUG(ProtocolIdentifierPresent(),Panic(KGsmuPanicProtocolIdentifierNotPresent));
	return &iProtocolIdentifier;
	} // CSmsSubmitReport::ProtocolIdentifier


const TSmsParameterIndicator* CSmsSubmitReport::ParameterIndicator() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMITREPORT_PARAMETERINDICATOR_1, "CSmsSubmitReport::ParameterIndicator()");

	return &iParameterIndicator;
	} // CSmsSubmitReport::ParameterIndicator


const CSmsUserData* CSmsSubmitReport::UserDataPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMITREPORT_USERDATAPTR_1, "CSmsSubmitReport::UserDataPtr()");

	__ASSERT_DEBUG(UserDataPresent(),Panic(KGsmuPanicUserDataNotPresent));
	return iUserData;
	} // CSmsSubmitReport::UserDataPtr


void CSmsSubmitReport::ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMITREPORT_CONSTRUCTL_1, "CSmsSubmitReport::ConstructL()");

	iServiceCenterAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iUserData=CSmsUserData::NewL(aCharacterSetConverter,aFs,iFirstOctet,iDataCodingScheme);
	} // CSmsSubmitReport::ConstructL


/**
 *  Duplicates this CSmsSubmitReport object.
 * 
 *  @return  Pointer to the newly created CSmsSubmitReport object.
 */
EXPORT_C CSmsSubmitReport* CSmsSubmitReport::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSUBMITREPORT_DUPLICATEL_1, "CSmsSubmitReport::DuplicateL()");

	CSmsSubmitReport*  smsSubmitReport = new (ELeave) CSmsSubmitReport(iIsRPError);
	CleanupStack::PushL(smsSubmitReport);

	smsSubmitReport->iServiceCenterAddress   = iServiceCenterAddress->DuplicateL();
	smsSubmitReport->iFirstOctet             = iFirstOctet;
	smsSubmitReport->iFailureCause           = iFailureCause;
	smsSubmitReport->iParameterIndicator     = iParameterIndicator;
	smsSubmitReport->iServiceCenterTimeStamp = iServiceCenterTimeStamp;
	smsSubmitReport->iProtocolIdentifier     = iProtocolIdentifier;
	smsSubmitReport->iDataCodingScheme       = iDataCodingScheme;
	smsSubmitReport->iUserData               = iUserData->DuplicateL(smsSubmitReport->iFirstOctet,
																	 smsSubmitReport->iDataCodingScheme);

	CleanupStack::Pop(smsSubmitReport);

	return smsSubmitReport;
	} // CSmsSubmitReport::DuplicateL


TUint8* CSmsSubmitReport::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMITREPORT_ENCODEL_1, "CSmsSubmitReport::EncodeL()");

	aPtr=iFirstOctet.EncodeL(aPtr);
	if (iIsRPError)
		aPtr=iFailureCause.EncodeL(aPtr);
	aPtr=iParameterIndicator.EncodeL(aPtr);
	aPtr=iServiceCenterTimeStamp.EncodeL(aPtr);
	if (iParameterIndicator.ProtocolIdentifierPresent())
		aPtr=iProtocolIdentifier.EncodeL(aPtr);
	if (iParameterIndicator.DataCodingSchemePresent())
		aPtr=iDataCodingScheme.EncodeL(aPtr);
	if (iParameterIndicator.UserDataPresent())
		aPtr=iUserData->EncodeL(aPtr);
	return aPtr;
	} // CSmsSubmitReport::EncodeL

TUint8* CSmsSubmitReport::EncodeL(TUint8* aPtr, const TEncodeParams* ) const
	{
	return EncodeL(aPtr);		
	}	

void CSmsSubmitReport::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSUBMITREPORT_DECODEL_1, "CSmsSubmitReport::DecodeL()");

	iFirstOctet.DecodeL(aPdu);
	if (iIsRPError)
		iFailureCause.DecodeL(aPdu);
	iParameterIndicator.DecodeL(aPdu);

	if (iParameterIndicator.Extension())
		{
		//
		//  Throw away rest of the pdu - some pdus received with this bit set don't appear to conform to 03.40 v7.4.0 spec.
		//  This appears safest option.
		//	TODO check new versions of 03.40 to make sre that it is still the right option
		//
		((TSmsOctet&)iParameterIndicator)=TSmsParameterIndicator::ESmsPIDExtension;
		return;
		}

	TInt timeError;
	iServiceCenterTimeStamp.DecodeL(aPdu, timeError);
	if (timeError != KErrNone)
		{
		TTime time;
		time.UniversalTime();
		iServiceCenterTimeStamp.SetTime(time);
		iServiceCenterTimeStamp.SetTimeOffset((User::UTCOffset().Int())/CSmsMessage::E15MinutesRepresentedInSeconds);
		}

	if (iParameterIndicator.ProtocolIdentifierPresent())
		iProtocolIdentifier.DecodeL(aPdu);

	if (iParameterIndicator.DataCodingSchemePresent())
		{
		iDataCodingScheme.DecodeL(aPdu);
    	TInt bit7to4=iDataCodingScheme.Bits7To4();
	    TInt lowerLimit = TSmsDataCodingScheme::ESmsDCSReserved5;
	    TInt upperLimit = TSmsDataCodingScheme::ESmsDCSReserved8;
	    if ((bit7to4>=lowerLimit) &&
	        (bit7to4<=upperLimit))

			User::Leave(KErrGsmSMSUnspecifiedDCSError);
		}
	if (iParameterIndicator.UserDataPresent())
		{
		if (!iParameterIndicator.DataCodingSchemePresent())
		    {
		    iParameterIndicator.SetDataCodingSchemePresent(TBool (ETrue));
		    iDataCodingScheme = TSmsDataCodingScheme();
		    }

		iUserData->DecodeL(aPdu);
		}
	} // CSmsSubmitReport::DecodeL


void CSmsSubmitReport::InternalizeMessagePDUL(RReadStream& aStream)
	{
	iServiceCenterAddress->InternalizeL(aStream);

	aStream >> iIsRPError;

	aStream >> iFirstOctet;
	aStream >> iFailureCause;
	aStream >> iParameterIndicator;
	aStream >> iServiceCenterTimeStamp;
	aStream >> iProtocolIdentifier;
	aStream >> iDataCodingScheme;
	aStream >> *iUserData;
	} // CSmsSubmitReport::InternalizeMessagePDUL


void CSmsSubmitReport::ExternalizeMessagePDUL(RWriteStream& aStream) const
	{
	iServiceCenterAddress->ExternalizeL(aStream);

	aStream << iIsRPError;

	aStream << iFirstOctet;
	aStream << iFailureCause;
	aStream << iParameterIndicator;
	aStream << iServiceCenterTimeStamp;
	aStream << iProtocolIdentifier;
	aStream << iDataCodingScheme;
	aStream << *iUserData;
	} // CSmsSubmitReport::ExternalizeMessagePDUL


/**
 *  Gets More Messages to Send flag.
 *  
 *  @return True if More Messages to Send flag set
 *  @capability None
 */
EXPORT_C TBool CSmsStatusReport::MoreMessagesToSend() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_MOREMESSAGESTOSEND_1, "CSmsStatusReport::MoreMessagesToSend()");

	return (iFirstOctet&TSmsFirstOctet::ESmsMoreMessagesToSendMask)==TSmsFirstOctet::ESmsMoreMessagesToSend;
	} // CSmsStatusReport::MoreMessagesToSend


/**
 *  Sets More Messages to Send flag.
 *  
 *  @param aMore True if More Messages to Send
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::SetMoreMessagesToSend(TBool aMore)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_SETMOREMESSAGESTOSEND_1, "CSmsStatusReport::SetMoreMessagesToSend()");

	iFirstOctet=aMore? (iFirstOctet&(~TSmsFirstOctet::ESmsMoreMessagesToSendMask)|TSmsFirstOctet::ESmsMoreMessagesToSend):
	                   (iFirstOctet&(~TSmsFirstOctet::ESmsMoreMessagesToSendMask)|TSmsFirstOctet::ESmsNoMoreMessagesToSend);
	} // CSmsStatusReport::SetMoreMessagesToSend


/**
 *  Gets Status Report Qualifier: the field in the Status Report which determines whether it's
 *  the result of a SUBMIT or COMMAND.
 *  
 *  @return Status Report Qualifier
 *  @capability None
 */
EXPORT_C TSmsFirstOctet::TSmsStatusReportQualifier CSmsStatusReport::StatusReportQualifier() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_STATUSREPORTQUALIFIER_1, "CSmsStatusReport::StatusReportQualifier()");

	return (TSmsFirstOctet::TSmsStatusReportQualifier) (iFirstOctet&TSmsFirstOctet::ESmsStatusReportQualifierMask);
	} // TSmsFirstOctet::TSmsStatusReportQualifier


/**
 *  Sets Status Report Qualifier to SUBMIT or COMMAND.
 *  
 *  @param aQualifier Status Report Qualifier
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::SetStatusReportQualifier(TSmsFirstOctet::TSmsStatusReportQualifier aQualifier)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_SETSTATUSREPORTQUALIFIER_1, "CSmsStatusReport::SetStatusReportQualifier()");

	iFirstOctet=iFirstOctet&(~TSmsFirstOctet::ESmsStatusReportQualifierMask)|aQualifier;
	} // CSmsStatusReport::SetStatusReportQualifier


CSmsStatusReport::CSmsStatusReport():
	CSmsPDU(ESmsStatusReport),
	iFirstOctet(TSmsFirstOctet::ESmsMTIStatusReportOrCommand|TSmsFirstOctet::ESmsNoMoreMessagesToSend|TSmsFirstOctet::ESmsStatusReportResultOfSubmit)
	{
	} // TSmsFirstOctet::ESmsMTIStatusReportOrCommand


/**
 *  Destructor.
 */
CSmsStatusReport::~CSmsStatusReport()
	{
	delete iServiceCenterAddress;
	delete iRecipientAddress;
	delete iUserData;
	} // TSmsFirstOctet::ESmsMTIStatusReportOrCommand


/**
 *  Gets the Message Reference.
 *  
 *  @return Message Reference
 *  @capability None
 */
EXPORT_C TInt CSmsStatusReport::MessageReference() const
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_MESSAGEREFERENCE_1, "CSmsStatusReport::MessageReference %d", (TInt)iMessageReference );
	return iMessageReference;
	} // CSmsStatusReport::MessageReference


/**
 *  Sets the Message Reference.
 *  
 *  @param aMessageReference Message Reference
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::SetMessageReference(TInt aMessageReference)
	{
	iMessageReference=aMessageReference;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_SETMESSAGEREFERENCE_1, "CSmsStatusReport::SetMessageReference %d", (TInt)iMessageReference );
	} // CSmsStatusReport::SetMessageReference


/**
 *  Gets Service Center Time Stamp.
 *  
 *  @param aTime Service Center Time Stamp represented in Universal Time.
 *  @param aNumQuarterHours +/- Time Zone difference to GMT in quarter hours.
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::ServiceCenterTimeStamp(TTime& aTime,TInt& aNumQuarterHours)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_SERVICECENTERTIMESTAMP_1, "CSmsStatusReport::ServiceCenterTimeStamp()");

	aTime=iServiceCenterTimeStamp.Time();
	aNumQuarterHours=iServiceCenterTimeStamp.TimeOffset();
	} // CSmsStatusReport::ServiceCenterTimeStamp


/**
 *  Sets the Service Center Time Stamp.
 *  
 *  @param aTime Service Center Time Stamp represented in Universal Time.
 *  @param aNumQuarterHours +/- Time Zone difference to GMT in quarter hours.
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::SetServiceCenterTimeStamp(const TTime& aTime,TInt& aNumQuarterHours)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_SETSERVICECENTERTIMESTAMP_1, "CSmsStatusReport::SetServiceCenterTimeStamp()");

	iServiceCenterTimeStamp.SetTime(aTime);
	iServiceCenterTimeStamp.SetTimeOffset(aNumQuarterHours);
	} // CSmsStatusReport::SetServiceCenterTimeStamp


/**
 *  Gets the Discharge Time.
 *  
 *  @param aTime The Discharge Time represented in Universal Time.
 *  @param aNumQuarterHours +/- Time Zone difference to GMT in quarter hours
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::DischargeTime(TTime& aTime,TInt& aNumQuarterHours)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_DISCHARGETIME_1, "CSmsStatusReport::DischargeTime()");

	aTime=iDischargeTime.Time();
	aNumQuarterHours=iDischargeTime.TimeOffset();
	} // CSmsStatusReport::DischargeTime


/**
 *  Sets the Discharge Time.
 *  
 *  @param aTime The Discharge Time represented in Universal Time.
 *  @param aNumQuarterHours +/- Time Zone difference to GMT in quarter hours
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::SetDischargeTime(const TTime& aTime,TInt& aNumQuarterHours)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_SETDISCHARGETIME_1, "CSmsStatusReport::SetDischargeTime()");

	iDischargeTime.SetTime(aTime);
	iDischargeTime.SetTimeOffset(aNumQuarterHours);
	} // CSmsStatusReport::SetDischargeTime


/**
 *  Gets the Status of the Message.
 *  
 *  @return Status
 *  @capability None
 */
EXPORT_C TSmsStatus::TSmsStatusValue CSmsStatusReport::Status() const
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_STATUS_1, "CSmsStatusReport::Status %d", iStatus.Status());
	return iStatus.Status();
	} // TSmsStatus::TSmsStatusValue


/**
 *  Sets the Status of the Message.
 *  
 *  @param aValue Status
 *  @capability None
 */
EXPORT_C void CSmsStatusReport::SetStatus(TSmsStatus::TSmsStatusValue aValue)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_SETSTATUS_1, "CSmsStatusReport::SetStatus()");

	iStatus.SetStatus(aValue);
	} // CSmsStatusReport::SetStatus


const TSmsDataCodingScheme* CSmsStatusReport::DataCodingScheme() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_DATACODINGSCHEME_1, "CSmsStatusReport::DataCodingScheme()");

	__ASSERT_DEBUG(DataCodingSchemePresent(),Panic(KGsmuPanicDataCodingSchemeNotPresent));
	return &iDataCodingScheme;
	} // CSmsStatusReport::DataCodingScheme


const TSmsProtocolIdentifier* CSmsStatusReport::ProtocolIdentifier() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_PROTOCOLIDENTIFIER_1, "CSmsStatusReport::ProtocolIdentifier()");

	__ASSERT_DEBUG(ProtocolIdentifierPresent(),Panic(KGsmuPanicProtocolIdentifierNotPresent));
	return &iProtocolIdentifier;
	} // CSmsStatusReport::ProtocolIdentifier


const TSmsParameterIndicator* CSmsStatusReport::ParameterIndicator() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_PARAMETERINDICATOR_1, "CSmsStatusReport::ParameterIndicator()");

	__ASSERT_DEBUG(iParameterIndicatorPresent,Panic(KGsmuPanicParameterIndicatorNotPresent));
	return &iParameterIndicator;
	} // CSmsStatusReport::ParameterIndicator


const CSmsUserData* CSmsStatusReport::UserDataPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_USERDATAPTR_1, "CSmsStatusReport::UserDataPtr()");

	__ASSERT_DEBUG(UserDataPresent(),Panic(KGsmuPanicUserDataNotPresent));
	return iUserData;
	} // CSmsStatusReport::UserDataPtr


const CSmsAddress* CSmsStatusReport::ToFromAddressPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_TOFROMADDRESSPTR_1, "CSmsStatusReport::ToFromAddressPtr()");

	return iRecipientAddress;
	} // CSmsStatusReport::ToFromAddressPtr


void CSmsStatusReport::ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_CONSTRUCTL_1, "CSmsStatusReport::ConstructL()");

	iServiceCenterAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iRecipientAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iUserData=CSmsUserData::NewL(aCharacterSetConverter,aFs,iFirstOctet,iDataCodingScheme);
	} // CSmsStatusReport::ConstructL


/**
 *  Duplicates this CSmsStatusReport object.
 * 
 *  @return  Pointer to the newly created CSmsStatusReport object.
 */
EXPORT_C CSmsStatusReport* CSmsStatusReport::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSSTATUSREPORT_DUPLICATEL_1, "CSmsStatusReport::DuplicateL()");

	CSmsStatusReport*  smsStatusReport = new (ELeave) CSmsStatusReport();
	CleanupStack::PushL(smsStatusReport);

	smsStatusReport->iServiceCenterAddress      = iServiceCenterAddress->DuplicateL();
	smsStatusReport->iParameterIndicatorPresent = iParameterIndicatorPresent;
	smsStatusReport->iFirstOctet                = iFirstOctet;
	smsStatusReport->iMessageReference          = iMessageReference;
	smsStatusReport->iRecipientAddress          = iRecipientAddress->DuplicateL();
	smsStatusReport->iServiceCenterTimeStamp    = iServiceCenterTimeStamp;
	smsStatusReport->iDischargeTime             = iDischargeTime;
	smsStatusReport->iStatus                    = iStatus;
	smsStatusReport->iParameterIndicator        = iParameterIndicator;
	smsStatusReport->iProtocolIdentifier        = iProtocolIdentifier;
	smsStatusReport->iDataCodingScheme          = iDataCodingScheme;
	smsStatusReport->iUserData                  = iUserData->DuplicateL(smsStatusReport->iFirstOctet,
																		smsStatusReport->iDataCodingScheme);

	CleanupStack::Pop(smsStatusReport);

	return smsStatusReport;
	} // CSmsStatusReport::DuplicateL


TUint8* CSmsStatusReport::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_ENCODEL_1, "CSmsStatusReport::EncodeL()");

	aPtr=iFirstOctet.EncodeL(aPtr);
	aPtr=iMessageReference.EncodeL(aPtr);
	aPtr=iRecipientAddress->EncodeL(aPtr);
	aPtr=iServiceCenterTimeStamp.EncodeL(aPtr);
	aPtr=iDischargeTime.EncodeL(aPtr);
	aPtr=iStatus.EncodeL(aPtr);
	if (iParameterIndicatorPresent)
		{
		aPtr=iParameterIndicator.EncodeL(aPtr);
		if (iParameterIndicator.ProtocolIdentifierPresent())
			aPtr=iProtocolIdentifier.EncodeL(aPtr);
		if (iParameterIndicator.DataCodingSchemePresent())
			aPtr=iDataCodingScheme.EncodeL(aPtr);
		if (iParameterIndicator.UserDataPresent())
			aPtr=iUserData->EncodeL(aPtr);
		}
	return aPtr;
	} // CSmsStatusReport::EncodeL

TUint8* CSmsStatusReport::EncodeL(TUint8* aPtr, const TEncodeParams* ) const
	{
	return EncodeL(aPtr);	
	}

void CSmsStatusReport::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTATUSREPORT_DECODEL_1, "CSmsStatusReport::DecodeL()");

	iFirstOctet.DecodeL(aPdu);
	iMessageReference.DecodeL(aPdu);
	iRecipientAddress->DecodeL(aPdu);

	TInt timeError;
	iServiceCenterTimeStamp.DecodeL(aPdu, timeError);
	if (timeError != KErrNone)
		{
		TTime time;
		time.UniversalTime();
		iServiceCenterTimeStamp.SetTime(time);
		iServiceCenterTimeStamp.SetTimeOffset((User::UTCOffset().Int()) / CSmsMessage::E15MinutesRepresentedInSeconds);
		}

	iDischargeTime.DecodeL(aPdu, timeError);
	if (timeError != KErrNone)
		{
		TTime time;
		time.UniversalTime();
		iDischargeTime.SetTime(time);
		iDischargeTime.SetTimeOffset((User::UTCOffset().Int()) / CSmsMessage::E15MinutesRepresentedInSeconds);
		}

	iStatus.DecodeL(aPdu);

	iParameterIndicatorPresent = (aPdu.Remainder().Length() > 0);

	if (iParameterIndicatorPresent)
		{
		iParameterIndicator.DecodeL(aPdu);
		if (!iParameterIndicator.Extension())
			{
			if (iParameterIndicator.ProtocolIdentifierPresent())
				iProtocolIdentifier.DecodeL(aPdu);
			if (iParameterIndicator.DataCodingSchemePresent())
				{
				iDataCodingScheme.DecodeL(aPdu);
	            TInt bit7to4=iDataCodingScheme.Bits7To4();
	            TInt lowerLimit = TSmsDataCodingScheme::ESmsDCSReserved5;
	            TInt upperLimit = TSmsDataCodingScheme::ESmsDCSReserved8;
	            if ((bit7to4>=lowerLimit) &&
	                (bit7to4<=upperLimit))
                    {
                    //
                    // defect fix for:  HOE-56GLND
                    // Enumerating of status reports with 6210 leaves with
                    // (-4688) KErrGsmSMSTpduNotSupported
                    // && problems with
                    // (-4671) KErrGsmSMSUnspecifiedDCSError
					//User::Leave(KErrGsmSMSUnspecifiedDCSError);
                    iParameterIndicator.SetExtension(EFalse);
                    iParameterIndicator.SetUserDataPresent(EFalse);
                    iParameterIndicator.SetDataCodingSchemePresent(EFalse);
                    iParameterIndicator.SetProtocolIdentifierPresent(EFalse);
					aPdu.UnGet();
                    }
				}
			if (iParameterIndicator.UserDataPresent())
				{
				if (!iParameterIndicator.DataCodingSchemePresent())
                    {
		            iParameterIndicator.SetDataCodingSchemePresent(TBool (ETrue));
		            iDataCodingScheme = TSmsDataCodingScheme();
                    }
				// PDEF137451: ETrue parameter indicates that a mismatch between length indicator and actual length is acceptable 
				// (Service Centre may occasionally truncate the user data of a status report PDU) 
				iUserData->DecodeL(aPdu,ETrue);
				}
			}
		else
			{
			// Throw away rest of the pdu - some pdus received with this bit set don't
			// appear to conform to 03.40 v7.4.0 spec.  Will need to review later, though
			// this appears safest option now.
			((TSmsOctet&)iParameterIndicator)=TSmsParameterIndicator::ESmsPIDExtension;
			return;
			}
		}
	} // CSmsStatusReport::DecodeL


void CSmsStatusReport::InternalizeMessagePDUL(RReadStream& aStream)
	{
	iServiceCenterAddress->InternalizeL(aStream);

	iParameterIndicatorPresent=aStream.ReadUint8L();

	aStream >> iFirstOctet;
	aStream >> iMessageReference;
	iRecipientAddress->InternalizeL(aStream);
	aStream >> iServiceCenterTimeStamp;
	aStream >> iDischargeTime;
	aStream >> iStatus;
	aStream >> iParameterIndicator;
	aStream >> iProtocolIdentifier;
	aStream >> iDataCodingScheme;
	aStream >> *iUserData;
	} // CSmsStatusReport::InternalizeMessagePDUL


void CSmsStatusReport::ExternalizeMessagePDUL(RWriteStream& aStream) const
	{
	iServiceCenterAddress->ExternalizeL(aStream);

	aStream.WriteUint8L(iParameterIndicatorPresent);

	aStream << iFirstOctet;
	aStream << iMessageReference;
	iRecipientAddress->ExternalizeL(aStream);
	aStream << iServiceCenterTimeStamp;
	aStream << iDischargeTime;
	aStream << iStatus;
	aStream << iParameterIndicator;
	aStream << iProtocolIdentifier;
	aStream << iDataCodingScheme;
	aStream << *iUserData;
	} // CSmsStatusReport::ExternalizeMessagePDUL


CSmsCommand::CSmsCommand():
	CSmsPDU(ESmsCommand),
	iFirstOctet(TSmsFirstOctet::ESmsMTIStatusReportOrCommand | TSmsFirstOctet::ESmsStatusReportNotRequested)
	{
	} // TSmsFirstOctet::ESmsMTIStatusReportOrCommand


/**
 *  Destructor.
 */
CSmsCommand::~CSmsCommand()
	{
	delete iServiceCenterAddress;
	delete iDestinationAddress;
	delete iCommandData;
	} // TSmsFirstOctet::ESmsMTIStatusReportOrCommand


/**
 *  Gets Status Report Request flag.
 *  
 *  @return True if the sender is requesting Status Reports
 *  @capability None
 */
EXPORT_C TBool CSmsCommand::StatusReportRequest() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_STATUSREPORTREQUEST_1, "CSmsCommand::StatusReportRequest()");

	return (iFirstOctet&TSmsFirstOctet::ESmsStatusReportRequestMask)==TSmsFirstOctet::ESmsStatusReportRequested;
	} // CSmsCommand::StatusReportRequest


/**
 *  Sets Status Report Request flag.
 *  
 *  @param aRequest  Set to True to Request Status Report
 *  @capability None
 */
EXPORT_C void CSmsCommand::SetStatusReportRequest(TBool aRequest)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_SETSTATUSREPORTREQUEST_1, "CSmsCommand::SetStatusReportRequest()");

	__ASSERT_DEBUG(CommandType()==TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest,Panic(KGsmuPanicSetStatusReportRequestNotSupportedForCommandType));
	DoSetStatusReportRequest(aRequest);
	} // CSmsCommand::SetStatusReportRequest


/**
 *  Gets the Message Reference.
 *  
 *  @return Message Reference
 *  @capability None
 */
EXPORT_C TInt CSmsCommand::MessageReference() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_MESSAGEREFERENCE_1, "CSmsCommand::MessageReference()");

	return iMessageReference;
	} // CSmsCommand::MessageReference


/**
 *  Sets the Message Reference.
 *  
 *  @param aMessageReference Message Reference
 *  @capability None
 */
EXPORT_C void CSmsCommand::SetMessageReference(TInt aMessageReference)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_SETMESSAGEREFERENCE_1, "CSmsCommand::SetMessageReference()");

	iMessageReference=aMessageReference;
	} // CSmsCommand::SetMessageReference


/**
 *  Gets the Command Type.
 *  
 *  @return Command Type
 *  @capability None
 */
EXPORT_C TInt CSmsCommand::CommandType() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_COMMANDTYPE_1, "CSmsCommand::CommandType()");

	return iCommandType.CommandType();
	} // CSmsCommand::CommandType


/**
 *  Sets the Command Type.
 *  
 *  @param aCommandType Command Type
 *  @capability None
 */
EXPORT_C void CSmsCommand::SetCommandType(TSmsCommandType::TSmsCommandTypeValue aCommandType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_SETCOMMANDTYPE_1, "CSmsCommand::SetCommandType()");

	//  Some command types have default status report request
	switch (aCommandType)
		{
		case TSmsCommandType::ESmsCommandTypeEnquiry:
			{
			DoSetStatusReportRequest(ETrue);
			break;
			}
		case TSmsCommandType::ESmsCommandTypeCancel:
		case TSmsCommandType::ESmsCommandTypeDelete:
		case TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest:
			{
			DoSetStatusReportRequest(EFalse);
			break;
			}
		default:
			{
			}
		}
	iCommandType.SetCommandType(aCommandType);
	} // CSmsCommand::SetCommandType


/**
 *  Gets the Message Number.
 *  
 *  @return Message Number
 *  @capability None
 */
EXPORT_C TInt CSmsCommand::MessageNumber() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_MESSAGENUMBER_1, "CSmsCommand::MessageNumber()");

	return iMessageNumber;
	} // CSmsCommand::MessageNumber


/**
 *  Sets the Message Number.
 *  
 *  @param aMessageNumber Message Number
 *  @capability None
 */
EXPORT_C void CSmsCommand::SetMessageNumber(TInt aMessageNumber)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_SETMESSAGENUMBER_1, "CSmsCommand::SetMessageNumber()");

	iMessageNumber=aMessageNumber;
	} // CSmsCommand::SetMessageNumber


/**
 *  Gets the number of Information Elements in the User Data.
 *  
 *  @return Number of Information Elements in the User Data
 *  @capability None
 */
EXPORT_C TInt CSmsCommand::NumInformationElements() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_NUMINFORMATIONELEMENTS_1, "CSmsCommand::NumInformationElements()");

	return iCommandData->NumInformationElements();
	} // CSmsCommand::NumInformationElements


/**
 *  Gets an Information Element.
 *  
 *  @param aIndex The Information Element Index within the PDU
 *  @return Information Element
 *  @capability None
 */
EXPORT_C CSmsInformationElement& CSmsCommand::InformationElement(TInt aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_INFORMATIONELEMENT_1, "CSmsCommand::InformationElement()");

	return iCommandData->InformationElement(aIndex);
	} // CSmsCommand::InformationElement


/**
 *  Gets a pointer to the Information Element located at aIndex.
 *  
 *  @param aIndex The Information Element Index within the PDU
 *  @return Pointer to Information Element
 *  @capability None
 */
CSmsInformationElement*& CSmsCommand::InformationElementPtr(TInt aIndex) const
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMAND_INFORMATIONELEMENTPTR_1, "CSmsCommand::InformationElementPtr()");
    return iCommandData->InformationElementPtr(aIndex);
    BULLSEYE_RESTORE
    }

/**
 *  Gets index of a specified Information Element.
 *  
 *  @param aIdentifier Information Element Identifier to match
 *  @param aIndex On return, index of the Information Element matching aIdentifier
 *  @return True if Information Element present matching aIdentifier
 *  @capability None
 */
EXPORT_C TBool CSmsCommand::InformationElementIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,
		TInt& aIndex) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_INFORMATIONELEMENTINDEX_1, "CSmsCommand::InformationElementIndex()");

	return iCommandData->InformationElementIndex(aIdentifier,aIndex);
	} // CSmsCommand::InformationElementIndex


/**
 *  Adds an Information Element.
 *  
 *  @param aIdentifier Information Element Identifier to add
 *  @param aData The Information Element data
 *  @capability None
 */
EXPORT_C void CSmsCommand::AddInformationElementL(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier, TDesC8& aData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_ADDINFORMATIONELEMENTL_1, "CSmsCommand::AddInformationElementL()");

	iCommandData->AddInformationElementL(aIdentifier,aData);
	} // CSmsCommand::AddInformationElementL


/**
 *  Removes an Information Element.
 *  
 *  @param aIndex Index of the Information Element to be removed
 *  @capability None
 */
EXPORT_C void CSmsCommand::RemoveInformationElement(TInt aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_REMOVEINFORMATIONELEMENT_1, "CSmsCommand::RemoveInformationElement()");

	iCommandData->RemoveInformationElement(aIndex);
	} // CSmsCommand::RemoveInformationElement


/**
 *  Gets the Maximum Data Length of the Command.
 *  
 *  @return Maximum Data Length of the Command
 *  @capability None
 */
EXPORT_C TInt CSmsCommand::MaxCommandDataLength() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_MAXCOMMANDDATALENGTH_1, "CSmsCommand::MaxCommandDataLength()");

	return iCommandData->MaxDataLength();
	} // CSmsCommand::MaxCommandDataLength


/**
 *  Gets the Command Data.
 *  
 *  @return The Command Data
 *  @capability None
 */
EXPORT_C TPtrC8 CSmsCommand::CommandData() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_COMMANDDATA_1, "CSmsCommand::CommandData()");

	return iCommandData->Data();
	} // CSmsCommand::CommandData


/**
 *  Sets the Command Data.
 *  
 *  @param aData The Command Data
 *  @capability None
 */
EXPORT_C void CSmsCommand::SetCommandDataL(const TDesC8& aData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_SETCOMMANDDATAL_1, "CSmsCommand::SetCommandDataL()");

	iCommandData->SetDataL(aData);
	} // CSmsCommand::SetCommandDataL


const TSmsProtocolIdentifier* CSmsCommand::ProtocolIdentifier() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMAND_PROTOCOLIDENTIFIER_1, "CSmsCommand::ProtocolIdentifier()");

	return &iProtocolIdentifier;
	} // CSmsCommand::ProtocolIdentifier


const CSmsAddress* CSmsCommand::ToFromAddressPtr() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMAND_TOFROMADDRESSPTR_1, "CSmsCommand::ToFromAddressPtr()");

	return iDestinationAddress;
	} // CSmsCommand::ToFromAddressPtr


void CSmsCommand::ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMAND_CONSTRUCTL_1, "CSmsCommand::ConstructL()");

	iServiceCenterAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iDestinationAddress=CSmsAddress::NewL(aCharacterSetConverter,aFs);
	iCommandData=CSmsCommandData::NewL(iFirstOctet);
	} // CSmsCommand::ConstructL


/**
 *  Duplicates this CSmsCommand object.
 * 
 *  @return  Pointer to the newly created CSmsCommand object.
 */
EXPORT_C CSmsCommand* CSmsCommand::DuplicateL() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSCOMMAND_DUPLICATEL_1, "CSmsCommand::DuplicateL()");

	CSmsCommand*  smsCommand = new (ELeave) CSmsCommand();
	CleanupStack::PushL(smsCommand);

	smsCommand->iServiceCenterAddress      = iServiceCenterAddress->DuplicateL();
	smsCommand->iFirstOctet                = iFirstOctet;
	smsCommand->iMessageReference          = iMessageReference;
	smsCommand->iProtocolIdentifier        = iProtocolIdentifier;
	smsCommand->iCommandType               = iCommandType;
	smsCommand->iMessageNumber             = iMessageNumber;
	smsCommand->iDestinationAddress        = iDestinationAddress->DuplicateL();
	smsCommand->iCommandData               = iCommandData->DuplicateL();

	CleanupStack::Pop(smsCommand);

	return smsCommand;
	} // CSmsCommand::DuplicateL


TUint8* CSmsCommand::EncodeL(TUint8* aPtr) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMAND_ENCODEL_1, "CSmsCommand::EncodeL()");

	aPtr=iFirstOctet.EncodeL(aPtr);
	aPtr=iMessageReference.EncodeL(aPtr);
	aPtr=iProtocolIdentifier.EncodeL(aPtr);
	aPtr=iCommandType.EncodeL(aPtr);
	aPtr=iMessageNumber.EncodeL(aPtr);
	aPtr=iDestinationAddress->EncodeL(aPtr);
	return iCommandData->EncodeL(aPtr);
	} // CSmsCommand::EncodeL

TUint8* CSmsCommand::EncodeL(TUint8* aPtr, const TEncodeParams* ) const
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF
    return EncodeL(aPtr);
    BULLSEYE_RESTORE
    }	

void CSmsCommand::DecodeL(TGsmuLex8& aPdu)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMAND_DECODEL_1, "CSmsCommand::DecodeL()");

	iFirstOctet.DecodeL(aPdu);
	iMessageReference.DecodeL(aPdu);
	iProtocolIdentifier.DecodeL(aPdu);
	iCommandType.DecodeL(aPdu);
	iMessageNumber.DecodeL(aPdu);
	iDestinationAddress->DecodeL(aPdu);
	iCommandData->DecodeL(aPdu);
	} // CSmsCommand::DecodeL


void CSmsCommand::InternalizeMessagePDUL(RReadStream& aStream)
	{
	iServiceCenterAddress->InternalizeL(aStream);

	aStream >> iFirstOctet;
	aStream >> iMessageReference;
	aStream >> iProtocolIdentifier;
	aStream >> iCommandType;
	aStream >> iMessageNumber;
	iDestinationAddress->InternalizeL(aStream);
	iCommandData->InternalizeL(aStream);
	} // CSmsCommand::InternalizeMessagePDUL


void CSmsCommand::ExternalizeMessagePDUL(RWriteStream& aStream) const
	{
	iServiceCenterAddress->ExternalizeL(aStream);

	aStream << iFirstOctet;
	aStream << iMessageReference;
	aStream << iProtocolIdentifier;
	aStream << iCommandType;
	aStream << iMessageNumber;
	iDestinationAddress->ExternalizeL(aStream);
	iCommandData->ExternalizeL(aStream);
	} // CSmsCommand::ExternalizeMessagePDUL


void CSmsCommand::DoSetStatusReportRequest(TBool aRequest)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSCOMMAND_DOSETSTATUSREPORTREQUEST_1, "CSmsCommand::DoSetStatusReportRequest()");

	iFirstOctet=aRequest? (iFirstOctet&(~TSmsFirstOctet::ESmsStatusReportRequestMask)|TSmsFirstOctet::ESmsStatusReportRequested):
	                      (iFirstOctet&(~TSmsFirstOctet::ESmsStatusReportRequestMask)|TSmsFirstOctet::ESmsAcceptDuplicates);
	} // CSmsCommand::DoSetStatusReportRequest
